/****************************************************************************
 * drivers/usbdev/rndis.c
 *
 *   Copyright (C) 2011-2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Authors: Sakari Kapanen <sakari.m.kapanen@gmail.com>,
 *            Petteri Aimonen <jpa@git.mail.kapsi.fi>
 *
 * References:
 *   [MS-RNDIS]:
 *     Remote Network Driver Interface Specification (RNDIS) Protocol
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
/****************************************************************************
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations,
 * which might include those applicable to Huawei LiteOS of U.S. and the country in
 * which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in
 * compliance with such applicable export control laws and regulations.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <securec.h>

#include <lwip/netifapi.h>
#include "rndis_std.h"
#include "gadget/rndis.h"
#include "gadget/f_ether.h"
#include "gadget/composite.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define FRNDIS_MAX_PACKET_SIZE   0x0400
#else
#define FRNDIS_MAX_PACKET_SIZE   0x0200
#endif

#define WORK_AVAILABLE(work) ((work)->work_status == 0)
typedef uint32_t irqstate_t;
#define UINFO PRINT_INFO

SPIN_LOCK_INIT(g_rndis_spinlock);
static inline irqstate_t enter_critical_section(void)
{
  irqstate_t flags;

  LOS_SpinLockSave(&g_rndis_spinlock, &flags);

  return flags;
}

static inline void leave_critical_section(irqstate_t flags)
{
  LOS_SpinUnlockRestore(&g_rndis_spinlock, flags);
}

static pthread_mutex_t rndis_mtx = PTHREAD_MUTEX_INITIALIZER;
#define NET_LOCK()   (void)pthread_mutex_lock(&rndis_mtx)
#define NET_UNLOCK() (void)pthread_mutex_unlock(&rndis_mtx)

#define LINK_STATUS_EVENT_MASK    (1)

#define CONFIG_NET_ETH_MTU      (2048)
#define CONFIG_RNDIS_BULKIN_REQLEN (CONFIG_NET_ETH_MTU + RNDIS_PACKET_HDR_SIZE)
#define CONFIG_RNDIS_BULKOUT_REQLEN CONFIG_RNDIS_BULKIN_REQLEN

#define RNDIS_CONFIGID          (1)
#define RNDIS_CONFIGIDNONE      (0)

#define RNDIS_BUFFER_SIZE       CONFIG_NET_ETH_MTU

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This structure describes the internal state of the driver */

struct rndis_dev_s
{
  struct los_eth_driver   netdev;        /* Network driver structure */
  FAR struct usbdev_s     *usbdev;       /* usbdev driver pointer */
  FAR struct usbdev_ep_s  *epintin;      /* Interrupt IN endpoint structure */
  FAR struct usbdev_ep_s  *epbulkin;     /* Bulk IN endpoint structure */
  FAR struct usbdev_ep_s  *epbulkout;    /* Bulk OUT endpoint structure */
  FAR struct usbdev_req_s *ctrlreq;      /* Pointer to preallocated control request */
  FAR struct usbdev_req_s *epintin_req;  /* Pointer to preallocated interrupt in endpoint request */
  FAR struct usbdev_req_s *rdreq;        /* Pointer to Preallocated control endpoint read request */
  struct list_head reqlist;              /* List of free write request containers */

  void *resp_buf;                        /* Used to store response messages */

  bool epintin_enable;                   /* Interrupt endpoint enable flag */
  bool epbulkin_enable;                  /* IN endpoint enable flag */
  bool epbulkout_enable;                 /* OUT endpoint enable flag */
  struct workqueue_struct *wq;           /* Workqueue for TX and RX */
  struct work_struct rxwork;             /* Worker for dispatching RX packets */
  struct work_struct pollwork;           /* TX poll worker */

  uint8_t config;                        /* USB Configuration number */
  FAR struct usbdev_req_s *net_req;      /* Pointer to request whose buffer is assigned to network */
  FAR struct usbdev_req_s *rx_req;       /* Pointer request container that holds RX buffer */
  size_t current_rx_received;            /* Number of bytes of current RX datagram received over USB */
  size_t current_rx_datagram_size;       /* Total number of bytes of the current RX datagram */
  size_t current_rx_datagram_offset;     /* Offset of current RX datagram */
  size_t current_rx_msglen;              /* Length of the entire message to be received */
  bool rdreq_submitted;                  /* Indicates if the read request is submitted */
  bool rx_blocked;                       /* Indicates if we can receive packets on bulk in endpoint */
  bool ctrlreq_has_encap_response;       /* Indicates if ctrlreq buffer holds a response */
  bool connected;                        /* Connection status indicator */
  uint32_t rndis_packet_filter;          /* RNDIS packet filter value */
  uint32_t rndis_host_tx_count;          /* TX packet counter */
  uint32_t rndis_host_rx_count;          /* RX packet counter */
  uint8_t host_mac_address[6];           /* Host side MAC address */
  EVENT_CB_S link_status_event;          /* Network connection event */
  pthread_t task_id;                     /* Network connection status processing task */
  atomic_t tx_transferring;              /* Tx transmission flag */
};

/* The internal version of the class driver */

struct rndis_driver_s
{
  struct usbdevclass_driver_s drvr;
  FAR struct rndis_dev_s      *dev;
};

/* This is what is allocated */

struct rndis_alloc_s
{
  struct rndis_dev_s    dev;
  struct rndis_driver_s drvr;
};

/* RNDIS object ID - value pair structure */

struct rndis_oid_value_s
{
  uint32_t objid;
  uint32_t length;
  uint32_t value;
  FAR const void *data; /* Data pointer overrides value if non-NULL. */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Netdev driver callbacks */

static int rndis_txavail(FAR struct rndis_dev_s *dev);
static int rndis_transmit(FAR struct rndis_dev_s *priv);
static int rndis_txpoll(FAR struct rndis_dev_s *priv);

/* usbclass callbacks */

static int  usbclass_setup(FAR struct usbdevclass_driver_s *driver,
                           FAR struct usbdev_s *dev,
                           FAR const struct usb_device_request *ctrl,
                           FAR uint8_t *dataout, size_t outlen);
static int  usbclass_bind(FAR struct usbdevclass_driver_s *driver,
                          FAR struct usbdev_s *dev);
static int usbclass_unbind(FAR struct usbdevclass_driver_s *driver,
                           FAR struct usbdev_s *dev);
static void usbclass_disconnect(FAR struct usbdevclass_driver_s *driver,
                                FAR struct usbdev_s *dev);
static int  usbclass_setconfig(FAR struct rndis_dev_s *priv, uint8_t config);
static void usbclass_resetconfig(FAR struct rndis_dev_s *priv);
static FAR struct usbdev_req_s *usbclass_allocreq(FAR struct usbdev_ep_s *ep,
                                                  uint16_t len);
static void usbclass_freereq(FAR struct usbdev_ep_s *ep,
                             FAR struct usbdev_req_s *req);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* USB driver operations */

const struct usbdevclass_driverops_s g_eth_driverops =
{
  usbclass_bind,
  usbclass_unbind,
  usbclass_setup,
  usbclass_disconnect,
  NULL,
  NULL
};

struct rndis_hs_function_descriptor g_rndis_hs_func_desc =
{
  {
    .bLength           = sizeof(struct usb_interface_assoc_descriptor),
    .bDescriptorType   = 0x0b,
    .bInterfaceCount   = 0x02,
    .bFunctionClass    = 0x02,
    .bFunctionSubClass = 0x06,
    .bFunctionProtocol = 0,
    .iFunction         = 6,
  },
  {
    .bLength            = sizeof(struct usb_interface_descriptor),
    .bDescriptorType    = UDESC_INTERFACE,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 1,    /* Endpoints Num */
    .bInterfaceClass    = UDCLASS_COMM,
    .bInterfaceSubClass = UICLASS_CDC,
    .bInterfaceProtocol = UIPROTO_DATA_VENDOR,
    .iInterface         = 0x04
  },
  {
    .bLength            = sizeof(struct usb_cdc_header_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubType = 0,
    HSETW(.bcdCDC, 0x0110),
  },
  {
    .bLength            = sizeof(struct usb_cdc_call_mgmt_descriptor),
    .bDescriptorType    = 0x24,
    .bDescriptorSubType = 0x01,
    .bmCapabilities     = 0,
    .bDataInterface     = 0x01, /* data-interfac */
  },
  {
    .bLength            = sizeof(struct usb_cdc_acm_descriptor),
    .bDescriptorType    = 0x24,
    .bDescriptorSubType = 0x02,
    .bmCapabilities     = 0,
  },
  {
    .bLength            = sizeof(struct usb_cdc_union_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubType = 0x06,
  },
  {
    .bLength            = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType    = UDESC_ENDPOINT,
    .bEndpointAddress   = UE_DIR_IN,
    .bmAttributes       = UE_INTERRUPT,
    HSETW(.wMaxPacketSize, 0x0008),
    .bInterval          = 9,
  },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  {
    .bLength         = 6,
    .bDescriptorType = 0x30,
    .bMaxBurst       = 0,
    .bmAttributes    = 0,
    HSETW(.wBytesPerInterval, 0x0008),
  },
#endif
  {
    .bLength             = sizeof(struct usb_interface_descriptor),
    .bDescriptorType     = UDESC_INTERFACE,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 2,   /* Endpoints Num */
    .bInterfaceClass     = UICLASS_CDC_DATA,
    .bInterfaceSubClass  = 0,   /* interface subclass */
    .bInterfaceProtocol  = 0,   /* interface protocol */
    .iInterface          = 5,
  },
  {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes     = UE_BULK,
    HSETW(.wMaxPacketSize, FRNDIS_MAX_PACKET_SIZE),
    .bInterval        = 0,
  },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  {
    .bLength            = 6,
    .bDescriptorType    = 0x30,
    .bMaxBurst          = 0,
    .bmAttributes       = 0,
    .wBytesPerInterval  = {0x00}
  },
#endif
  {
    .bLength             = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType     = UDESC_ENDPOINT,
    .bEndpointAddress    = UE_DIR_OUT,
    .bmAttributes        = UE_BULK,
    HSETW(.wMaxPacketSize, FRNDIS_MAX_PACKET_SIZE),
    .bInterval           = 0,
  },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  {
    .bLength            = 6,
    .bDescriptorType    = 0x30,
    .bMaxBurst          = 0,
    .bmAttributes       = 0,
    .wBytesPerInterval  = {0x00}
  },
#endif
};

struct usb_config_descriptor g_rndis_hs_config_desc =
{
  .bLength             = USB_CONFIG_DESC_SIZE,
  .bDescriptorType     = UDESC_CONFIG,
  HSETW(.wTotalLength, USB_CONFIG_DESC_SIZE + sizeof(g_rndis_hs_func_desc)),
  .bNumInterface       = 2,
  .bConfigurationValue = 1,
  .iConfiguration      = 0,
  .bmAttributes        = UC_SELF_POWERED | UC_BUS_POWERED,
  .bMaxPower           = 1, /* max power */
};

struct rndis_hs_descriptor g_rndis_hs_desc =
{
  .rndis_config = &g_rndis_hs_config_desc,
  .rndis_func   = &g_rndis_hs_func_desc
};

/* These lists give dummy responses to be returned to PC. The values are
 * chosen so that Windows is happy - other operating systems don't really care
 * much.
 */

static const uint32_t g_rndis_supported_oids[] =
{
  RNDIS_OID_GEN_SUPPORTED_LIST,
  RNDIS_OID_GEN_HARDWARE_STATUS,
  RNDIS_OID_GEN_MEDIA_SUPPORTED,
  RNDIS_OID_GEN_MEDIA_IN_USE,
  RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE,
  RNDIS_OID_GEN_LINK_SPEED,
  RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,
  RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE,
  RNDIS_OID_GEN_VENDOR_ID,
  RNDIS_OID_GEN_VENDOR_DESCRIPTION,
  RNDIS_OID_GEN_VENDOR_DRIVER_VERSION,
  RNDIS_OID_GEN_CURRENT_PACKET_FILTER,
  RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,
  RNDIS_OID_GEN_MEDIA_CONNECT_STATUS,
  RNDIS_OID_GEN_PHYSICAL_MEDIUM,
  RNDIS_OID_GEN_XMIT_OK,
  RNDIS_OID_GEN_RCV_OK,
  RNDIS_OID_GEN_XMIT_ERROR,
  RNDIS_OID_GEN_RCV_ERROR,
  RNDIS_OID_GEN_RCV_NO_BUFFER,
  RNDIS_OID_802_3_PERMANENT_ADDRESS,
  RNDIS_OID_802_3_CURRENT_ADDRESS,
  RNDIS_OID_802_3_MULTICAST_LIST,
  RNDIS_OID_802_3_MAC_OPTIONS,
  RNDIS_OID_802_3_MAXIMUM_LIST_SIZE,
  RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT,
  RNDIS_OID_802_3_XMIT_ONE_COLLISION,
  RNDIS_OID_802_3_XMIT_MORE_COLLISION,
};

static const struct rndis_oid_value_s g_rndis_oid_values[] =
{
  {RNDIS_OID_GEN_SUPPORTED_LIST, sizeof(g_rndis_supported_oids), 0, g_rndis_supported_oids},
  {RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE,    4, CONFIG_NET_ETH_MTU,  NULL},
#ifdef CONFIG_USBDEV_DUALSPEED
  {RNDIS_OID_GEN_LINK_SPEED,            4, 100000,              NULL},
#else
  {RNDIS_OID_GEN_LINK_SPEED,            4, 2000000,             NULL},
#endif
  {RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,   4, CONFIG_NET_ETH_MTU,  NULL},
  {RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE,    4, CONFIG_NET_ETH_MTU,  NULL},
  {RNDIS_OID_GEN_VENDOR_ID,             4, 0x00FFFFFF,          NULL},
  {RNDIS_OID_GEN_VENDOR_DESCRIPTION,    6, 0,                   "RNDIS"},
  {RNDIS_OID_GEN_CURRENT_PACKET_FILTER, 4, 0,                   NULL},
  {RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,    4, 2048,                NULL},
  {RNDIS_OID_GEN_XMIT_OK,               4, 0,                   NULL},
  {RNDIS_OID_GEN_RCV_OK,                4, 0,                   NULL},
  {RNDIS_OID_802_3_PERMANENT_ADDRESS,   6, 0,                   NULL},
  {RNDIS_OID_802_3_CURRENT_ADDRESS,     6, 0,                   NULL},
  {RNDIS_OID_802_3_MULTICAST_LIST,      4, 0xE0000000,          NULL},
  {RNDIS_OID_802_3_MAXIMUM_LIST_SIZE,   4, 1,                   NULL},
  {0x0,                                 4, 0,                   NULL}, /* Default fallback */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static void link_status_check_func(struct rndis_dev_s *dev)
{
  int link_status;
  struct netif *netif = &dev->netdev.ac_if;

  link_status = dev->connected;
  if (netif_is_up(netif) && !link_status)
    {
      (void)netifapi_netif_set_link_down(netif);
      (void)netifapi_netif_set_down(netif);
      dprintf("set usb rndis network link down!\n");
    }
  else if (!netif_is_up(netif) && link_status)
    {
      (void)netifapi_netif_set_link_up(netif);
      (void)netifapi_netif_set_up(netif);
      dprintf("set usb rndis network link up!\n");
    }
}

static void link_status_check_thread(UINTPTR para)
{
  struct rndis_dev_s *dev = (struct rndis_dev_s *)para;

  for (;;)
    {
      (void)LOS_EventRead(&dev->link_status_event, LINK_STATUS_EVENT_MASK, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

      link_status_check_func(dev);
    }
}

static void link_status_check_init(struct rndis_dev_s *dev)
{
#define LINK_STATUS_TASK_PRI  10
  uint32_t ret;

  (void)LOS_EventInit(&dev->link_status_event);
  ret = usb_os_task_creat(&dev->task_id, (TSK_ENTRY_FUNC)link_status_check_thread, LINK_STATUS_TASK_PRI, "USB_Status_Task", (UINTPTR)(dev));
  if (ret)
    {
      (void)LOS_EventDestroy(&dev->link_status_event);
      return;
    }
}

static void link_status_check_deinit(struct rndis_dev_s *dev)
{
  (void)usb_os_task_delete(dev->task_id);
  (void)LOS_EventDestroy(&dev->link_status_event);
}

static void link_status_change(struct rndis_dev_s *dev)
{
  (void)LOS_EventWrite(&dev->link_status_event, LINK_STATUS_EVENT_MASK);
}

/****************************************************************************
 * Buffering of data is implemented in the following manner:
 *
 * RNDIS driver holds a number of preallocated bulk IN endpoint write
 * requests along with buffers large enough to hold an Ethernet packet and
 * the corresponding RNDIS header.
 *
 * One of these is always reserved for packet reception - when data arrives
 * on the bulk OUT endpoint, it is copied to the reserved request buffer.
 * When the reception of an Ethernet packet is complete, a worker to process
 * the packet is scheduled and bulk OUT endpoint is set to NAK.
 *
 * The processing worker passes the buffer to the network. When the network is
 * done processing the packet, the buffer might contain data to be sent.
 * If so, the corresponding write request is queued on the bulk IN endpoint.
 * The NAK state on bulk OUT endpoint is cleared to allow new packets to
 * arrive. If there's no data to send, the request is returned to the list of
 * free requests.
 *
 * When a bulk IN write operation is complete, the request is added to the
 * list of free requests.
 *
 ****************************************************************************/

/****************************************************************************
 * Name: rndis_submit_rdreq
 *
 * Description:
 *   Submits the bulk OUT read request. Takes care not to submit the request
 *   when the RX packet buffer is already in use.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Returned Value:
 *   The return value of the EP_SUBMIT operation
 *
 ****************************************************************************/

static int rndis_submit_rdreq(FAR struct rndis_dev_s *priv)
{
  int ret = OK;

  if (!priv->rdreq_submitted && !priv->rx_blocked)
    {
      priv->rdreq->len            = CONFIG_RNDIS_BULKOUT_REQLEN;
      priv->epbulkout->handle_req = priv->rdreq;

      ret = EP_SUBMIT(priv->epbulkout, priv->rdreq);
      if (ret != OK)
        {
          usb_err("%s %d, len:%u\n", __FUNCTION__, __LINE__, priv->rdreq->len);
        }
      else
        {
          priv->rdreq_submitted = true;
        }
    }

  return ret;
}

/****************************************************************************
 * Name: rndis_cancel_rdreq
 *
 * Description:
 *   Cancels the bulk OUT endpoint read request.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 ****************************************************************************/

static void rndis_cancel_rdreq(FAR struct rndis_dev_s *priv)
{
  if (priv->rdreq_submitted)
    {
      EP_CANCEL(priv->epbulkout, priv->rdreq);
      priv->rdreq_submitted = false;
    }
}

/****************************************************************************
 * Name: rndis_block_rx
 *
 * Description:
 *   Blocks reception of further bulk OUT endpoint data.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 ****************************************************************************/

static void rndis_block_rx(FAR struct rndis_dev_s *priv)
{
  priv->rx_blocked = true;
  rndis_cancel_rdreq(priv);
}

/****************************************************************************
 * Name: rndis_unblock_rx
 *
 * Description:
 *   Unblocks reception of bulk OUT endpoint data.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static void rndis_unblock_rx(FAR struct rndis_dev_s *priv)
{
  priv->rx_blocked = false;
}

/****************************************************************************
 * Name: rndis_allocwrreq
 *
 * Description:
 *   Allocates a bulk IN endpoint request from the list of free request
 *   buffers.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Returned Value:
 *   NULL if allocation failed; pointer to allocated request if succeeded
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static FAR struct usbdev_req_s *rndis_allocwrreq(FAR struct rndis_dev_s *priv)
{
  struct usbdev_req_s *req;

  req = usbclass_allocreq(priv->epbulkin, CONFIG_RNDIS_BULKIN_REQLEN);
  if (req == NULL)
    {
      return NULL;
    }

  return req;
}

/****************************************************************************
 * Name: rndis_hasfreereqs
 *
 * Description:
 *   Checks if there are free requests usable for TX data.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Returned Value:
 *   true if requests available; false if no requests available
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static bool rndis_hasfreereqs(FAR struct rndis_dev_s *priv)
{
  return list_empty(&priv->reqlist) ? true : false;
}

/****************************************************************************
 * Name: rndis_freewrreq
 *
 * Description:
 *   Returns a bulk IN endpoint write requests to the list of free requests.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *   req: pointer to the request
 *
 * Assumptions:
 *   Called with interrupts disabled.
 *
 ****************************************************************************/

static void rndis_freewrreq(FAR struct rndis_dev_s *priv,
                            FAR struct usbdev_req_s *req)
{
  DEBUGASSERT(req != NULL);

  usbclass_freereq(priv->epbulkin, req);
  rndis_submit_rdreq(priv);
}

/****************************************************************************
 * Name: rndis_allocnetreq
 *
 * Description:
 *   Allocates a request buffer to be used on the network.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Returned Value:
 *   true if succeeded; false if failed
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static bool rndis_allocnetreq(FAR struct rndis_dev_s *priv)
{
  irqstate_t flags = enter_critical_section();

  if (rndis_hasfreereqs(priv))
    {
      leave_critical_section(flags);
      return false;
    }

  if (atomic_read(&priv->tx_transferring))
    {
      leave_critical_section(flags);
      return false;
    }

  priv->net_req = list_first_entry(&priv->reqlist, struct usbdev_req_s, list);
  list_del_init(&priv->net_req->list);

  atomic_set(&priv->tx_transferring, 1);
  leave_critical_section(flags);
  return priv->net_req != NULL;
}

/****************************************************************************
 * Name: rndis_sendnetreq
 *
 * Description:
 *   Submits the request buffer held by the network.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static void rndis_sendnetreq(FAR struct rndis_dev_s *priv)
{
  irqstate_t flags = enter_critical_section();

  DEBUGASSERT(priv->net_req != NULL);

  priv->net_req->priv = priv->net_req;
  priv->epbulkin->handle_req = priv->net_req;
  EP_SUBMIT(priv->epbulkin, priv->net_req);

  priv->net_req = NULL;

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: rndis_freenetreq
 *
 * Description:
 *   Frees the request buffer held by the network.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static void rndis_freenetreq(FAR struct rndis_dev_s *priv)
{
  irqstate_t flags = enter_critical_section();

  rndis_freewrreq(priv, priv->net_req);
  priv->net_req = NULL;

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: rndis_allocrxreq
 *
 * Description:
 *   Allocates a buffer for packet reception if there already isn't one.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Returned Value:
 *   true if succeeded; false if failed
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static bool rndis_allocrxreq(FAR struct rndis_dev_s *priv)
{
  if (priv->rx_req != NULL)
    {
      return true;
    }

  priv->rx_req = rndis_allocwrreq(priv);
  return priv->rx_req != NULL;
}

/****************************************************************************
 * Name: rndis_giverxreq
 *
 * Description:
 *   Passes the RX packet buffer to the network
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static void rndis_giverxreq(FAR struct rndis_dev_s *priv)
{
  DEBUGASSERT(priv->rx_req != NULL);
  DEBUGASSERT(priv->net_req == NULL);

  priv->net_req = priv->rx_req;
  priv->rx_req  = NULL;
}

/****************************************************************************
 * Name: rndis_fillrequest
 *
 * Description:
 *   Fills the RNDIS header to the request buffer
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *   req: the request whose buffer we should fill
 *
 * Returned Value:
 *   The total length of the request data
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static uint16_t rndis_fillrequest(FAR struct rndis_dev_s *priv,
                                  FAR struct usbdev_req_s *req)
{
  size_t datalen = req->len - RNDIS_PACKET_HDR_SIZE;

  if (datalen > 0)
    {
      /* Send the required headers */

      FAR struct rndis_packet_msg *msg = (FAR struct rndis_packet_msg *)req->buf;
      (void)memset_s(msg, RNDIS_PACKET_HDR_SIZE, 0, RNDIS_PACKET_HDR_SIZE);

      msg->msgtype    = RNDIS_PACKET_MSG;
      msg->msglen     = RNDIS_PACKET_HDR_SIZE + datalen;
      msg->dataoffset = RNDIS_PACKET_HDR_SIZE - 8;
      msg->datalen    = datalen;

      req->flags      = USBDEV_REQFLAGS_NULLPKT;
    }

  return req->len;
}

/****************************************************************************
 * Name: rndis_rxdispatch
 *
 * Description:
 *   Processes the received Ethernet packet. Called from work queue.
 *
 * Input Parameters:
 *   arg: pointer to RNDIS device driver structure
 *
 ****************************************************************************/

static void rndis_rxdispatch(FAR struct work_struct *arg)
{
  FAR struct rndis_dev_s *priv = (FAR struct rndis_dev_s *)arg->data;
  irqstate_t flags;

  NET_LOCK();
  flags = enter_critical_section();
  rndis_giverxreq(priv);
  leave_critical_section(flags);

  if (priv->net_req == NULL)
    {
      return;
    }

  eth_rx(&priv->netdev, priv->net_req);

  priv->current_rx_datagram_size = 0;
  rndis_unblock_rx(priv);

  rndis_freenetreq(priv);
  NET_UNLOCK();
}

/****************************************************************************
 * Name: rndis_txpoll
 *
 * Description:
 *   Sends the packet that is stored in the network packet buffer. Called
 *   from work queue by e.g. txavail and txpoll callbacks.
 *
 * Input Parameters:
 *   dev: pointer to network driver structure
 *
 * Assumptions:
 *   Caller holds the network lock
 *
 ****************************************************************************/

static int rndis_txpoll(FAR struct rndis_dev_s *priv)
{
  int ret = OK;

  if (!priv->connected)
    {
      return -EBUSY;
    }

  if (priv->net_req->len > 0)
    {
      ret = rndis_transmit(priv);
    }

  /* If zero is returned, the polling will continue until all connections have
   * been examined.
   */

  return ret;
}

/****************************************************************************
 * Name: rndis_transmit
 *
 * Description:
 *   Start hardware transmission.
 *
 ****************************************************************************/

static int rndis_transmit(FAR struct rndis_dev_s *priv)
{
  /* Queue the packet */

  rndis_fillrequest(priv, priv->net_req);
  rndis_sendnetreq(priv);

  return OK;
}

/****************************************************************************
 * Name: rndis_txavail_work
 *
 * Description:
 *   txavail worker function
 *
 ****************************************************************************/

static void rndis_txavail_work(FAR struct work_struct *arg)
{
  FAR struct rndis_dev_s *priv = (FAR struct rndis_dev_s *)arg->data;

  NET_LOCK();

  if (rndis_allocnetreq(priv))
    {
      rndis_txpoll(priv);
    }

  NET_UNLOCK();
}

/****************************************************************************
 * Name: rndis_txavail
 *
 * Description:
 *   Network txavail callback that's called when there are buffers available
 *   for sending data. May be called from an interrupt, so we must queue a
 *   worker to do the actual processing.
 *
 ****************************************************************************/

static int rndis_txavail(FAR struct rndis_dev_s *priv)
{
  bool ret;

  if (WORK_AVAILABLE(&priv->pollwork))
    {
      ret = queue_work(priv->wq, &priv->pollwork);
      if (ret == false) {
          usb_err("queue work failed!\n");
      }
    }

  return OK;
}

/************************************************************************************
 * Name: rndis_recvpacket
 *
 * Description:
 *   Handles a USB packet arriving on the data bulk out endpoint.
 *
 * Assumptions:
 *   Called from the USB interrupt handler with interrupts disabled.
 *
 ************************************************************************************/

static inline int rndis_recvpacket(FAR struct rndis_dev_s *priv,
                                   FAR uint8_t *reqbuf, uint16_t reqlen)
{
  int ret;

  if (!rndis_allocrxreq(priv))
    {
      return -ENOMEM;
    }

  if (!priv->connected)
    {
      return -EBUSY;
    }

  if (!priv->current_rx_datagram_size)
    {
      if (reqlen < 16)
        {
          /* Packet too small to contain a message header */
        }
      else
        {
          /* The packet contains a RNDIS packet message header */

          FAR struct rndis_packet_msg *msg = (FAR struct rndis_packet_msg *)reqbuf;
          if (msg->msgtype == RNDIS_PACKET_MSG)
            {
              priv->current_rx_received      = reqlen;
              priv->current_rx_datagram_size = msg->datalen;
              priv->current_rx_msglen        = msg->msglen;

              /* According to RNDIS-over-USB send, if the message length is a
               * multiple of endpoint max packet size, the host must send an
               * additional single-byte zero packet. Take that in account here.
               */

              if ((priv->current_rx_msglen % priv->epbulkout->maxpacket) == 0)
                {
                  priv->current_rx_msglen += 1;
                }

              /* Data offset is defined as an offset from the beginning of the
               * offset field itself
               */

              priv->current_rx_datagram_offset = msg->dataoffset + 8;
              if (priv->current_rx_datagram_offset < reqlen)
                {
                  ret = memcpy_s(&priv->rx_req->buf[RNDIS_PACKET_HDR_SIZE],
                                 CONFIG_RNDIS_BULKIN_REQLEN - RNDIS_PACKET_HDR_SIZE,
                                 &reqbuf[priv->current_rx_datagram_offset],
                                 reqlen - priv->current_rx_datagram_offset);
                  if (ret != EOK)
                    {
                      usb_err("memcpy failed!\n");
                      return -ENOMEM;
                    }
                }
            }
          else
            {
              usb_err("Unknown RNDIS message type %u\n", msg->msgtype);
            }
        }
    }
  else
    {
      if (priv->current_rx_received >= priv->current_rx_datagram_offset &&
          priv->current_rx_received <= priv->current_rx_datagram_size +
          priv->current_rx_datagram_offset)
        {
          size_t index = priv->current_rx_received - priv->current_rx_datagram_offset;
          size_t copysize = min(reqlen, priv->current_rx_datagram_size - index);

          /* Check if the received packet exceeds request buffer */

          if ((RNDIS_PACKET_HDR_SIZE + index + copysize) <= CONFIG_NET_ETH_MTU)
            {
              ret = memcpy_s(&priv->rx_req->buf[RNDIS_PACKET_HDR_SIZE + index],
                             CONFIG_RNDIS_BULKIN_REQLEN - RNDIS_PACKET_HDR_SIZE - index,
                             reqbuf, copysize);
              if (ret != EOK)
                {
                  usb_err("memcpy failed!\n");
                  return -ENOMEM;
                }
            }
          else
            {
              usb_err("The packet exceeds request buffer (reqlen=%u) \n", reqlen);
            }
        }
      priv->current_rx_received += reqlen;
    }

  if (priv->current_rx_received >= priv->current_rx_msglen)
    {
      /* Check for a usable packet length (4 added for the CRC) */

      if (priv->current_rx_datagram_size > (CONFIG_NET_ETH_MTU + 4) ||
          priv->current_rx_datagram_size <= (sizeof(struct eth_hdr) + 4))
        {
          priv->current_rx_datagram_size = 0;
        }
      else
        {
          priv->rx_req->xfrd = priv->current_rx_received;

          ret = queue_work(priv->wq, &priv->rxwork);
          DEBUGASSERT(ret == 0);
          UNUSED(ret);

          rndis_block_rx(priv);
          priv->rndis_host_tx_count++;
          return -EBUSY;
        }
    }

  return OK;
}

/****************************************************************************
 * Name: rndis_prepare_response
 *
 * Description:
 *   Passes the RX packet buffer to the network
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static bool rndis_prepare_response(FAR struct rndis_dev_s *priv, size_t size,
                                   FAR struct rndis_command_header *request_hdr)
{
  FAR struct rndis_response_header *hdr;
  priv->resp_buf = malloc(size);
  if (!priv->resp_buf)
    {
      usb_err("malloc fail!\n");
      return false;
    }

  hdr = (FAR struct rndis_response_header *)priv->resp_buf;

  hdr->msgtype = request_hdr->msgtype | RNDIS_MSG_COMPLETE;
  hdr->msglen  = size;
  hdr->reqid   = request_hdr->reqid;
  hdr->status  = RNDIS_STATUS_SUCCESS;

  priv->ctrlreq_has_encap_response = true;

  return true;
}

/****************************************************************************
 * Name: rndis_send_encapsulated_response
 *
 * Description:
 *   Give a notification to the host that there is an encapsulated response
 *   available.
 *
 * Input Parameters:
 *   priv: pointer to RNDIS device driver structure
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static int rndis_send_encapsulated_response(FAR struct rndis_dev_s *priv)
{
  FAR struct rndis_notification *notif =
    (FAR struct rndis_notification *)priv->epintin_req->buf;

  notif->notification       = RNDIS_NOTIFICATION_RESPONSE_AVAILABLE;
  notif->reserved           = 0;
  priv->epintin_req->len    = sizeof(struct rndis_notification);
  priv->epintin->handle_req = priv->epintin_req;

  EP_SUBMIT(priv->epintin, priv->epintin_req);

  return OK;
}

/****************************************************************************
 * Name: rndis_handle_control_message
 *
 * Description:
 *   Handle a RNDIS control message.
 *
 * Input Parameters:
 *   ctrlreq: pointer to RNDIS control request
 *
 * Assumptions:
 *   Called from critical section
 *
 ****************************************************************************/

static void rndis_handle_control_message(struct usbdev_ep_s *ep, struct usbdev_req_s *ctrlreq)
{
  FAR struct rndis_dev_s *priv = (struct rndis_dev_s *)ctrlreq->priv;
  FAR struct rndis_command_header *cmd_hdr =
    (FAR struct rndis_command_header *)ctrlreq->buf;
  int ret;
  bool resp_flag;

  (void)ep;

  switch (cmd_hdr->msgtype)
    {
      case RNDIS_INITIALIZE_MSG:
        {
          UINFO("rndis initialize msg!\n");
          FAR struct rndis_initialize_cmplt *resp;

          resp_flag = rndis_prepare_response(priv, sizeof(struct rndis_initialize_cmplt), cmd_hdr);
          if (!resp_flag)
            {
              return;
            }

          resp = (FAR struct rndis_initialize_cmplt *)priv->resp_buf;

          resp->major      = RNDIS_MAJOR_VERSION;
          resp->minor      = RNDIS_MINOR_VERSION;
          resp->devflags   = RNDIS_DEVICEFLAGS;
          resp->medium     = RNDIS_MEDIUM_802_3;
          resp->pktperxfer = 1;
          resp->xfrsize    = (4 + 44 + 22) + RNDIS_BUFFER_SIZE;
          resp->pktalign   = 2;

          rndis_send_encapsulated_response(priv);
        }
        break;

      case RNDIS_HALT_MSG:
        {
          UINFO("rndis halt msg disconnect!\n");
          priv->connected = false;
        }
        break;

      case RNDIS_QUERY_MSG:
        {
          int i;
          size_t max_reply_size = sizeof(struct rndis_query_cmplt) +
                                  sizeof(g_rndis_supported_oids);
          FAR struct rndis_query_msg req;
          ret = memcpy_s(&req, sizeof(struct rndis_query_msg), ctrlreq->buf, sizeof(struct rndis_query_msg));
          if (ret != EOK)
            {
              usb_err("memcpy failed!\n");
              return;
            }

          resp_flag = rndis_prepare_response(priv, max_reply_size, cmd_hdr);
          if (!resp_flag)
            {
              return;
            }

          FAR struct rndis_query_cmplt *resp =
            (FAR struct rndis_query_cmplt *)priv->resp_buf;

          resp->hdr.msglen = sizeof(struct rndis_query_cmplt);
          resp->bufoffset  = 0;
          resp->buflen     = 0;
          resp->hdr.status = RNDIS_STATUS_NOT_SUPPORTED;

          for (i = 0;
               i < sizeof(g_rndis_oid_values)/sizeof(g_rndis_oid_values[0]);
               i++)
            {
              bool match = (g_rndis_oid_values[i].objid == req.objid);

              if (!match && g_rndis_oid_values[i].objid == 0)
                {
                  int j;

                  /* Check whether to apply the fallback entry */

                  for (j = 0; j < sizeof(g_rndis_supported_oids)/sizeof(uint32_t); j++)
                    {
                      if (g_rndis_supported_oids[j] == req.objid)
                        {
                          match = true;
                          break;
                        }
                    }
                }

              if (match)
                {
                  resp->hdr.status = RNDIS_STATUS_SUCCESS;
                  resp->bufoffset  = 16;
                  resp->buflen     = g_rndis_oid_values[i].length;

                  if (req.objid == RNDIS_OID_GEN_CURRENT_PACKET_FILTER)
                    {
                      resp->buffer[0] = priv->rndis_packet_filter;
                    }
                  else if (req.objid == RNDIS_OID_GEN_XMIT_OK)
                    {
                      resp->buffer[0] = priv->rndis_host_tx_count;
                    }
                  else if (req.objid == RNDIS_OID_GEN_RCV_OK)
                    {
                      resp->buffer[0] = priv->rndis_host_rx_count;
                    }
                  else if (req.objid == RNDIS_OID_802_3_CURRENT_ADDRESS ||
                           req.objid == RNDIS_OID_802_3_PERMANENT_ADDRESS)
                    {
                      ret = memcpy_s(resp->buffer, USB_COMP_EP0_BUFSIZ - sizeof(struct rndis_query_cmplt),
                                     priv->host_mac_address, 6);
                      if (ret != EOK)
                        {
                          usb_err("memcpy failed!\n");
                          return;
                        }
                    }
                  else if (g_rndis_oid_values[i].data)
                    {
                      ret = memcpy_s(resp->buffer, USB_COMP_EP0_BUFSIZ - sizeof(struct rndis_query_cmplt),
                                     g_rndis_oid_values[i].data, resp->buflen);
                      if (ret != EOK)
                        {
                          usb_err("memcpy failed!\n");
                          return;
                        }
                    }
                  else
                    {
                      ret = memcpy_s(resp->buffer, USB_COMP_EP0_BUFSIZ - sizeof(struct rndis_query_cmplt),
                                     &g_rndis_oid_values[i].value, resp->buflen);
                      if (ret != EOK)
                        {
                          usb_err("memcpy failed!\n");
                          return;
                        }
                    }
                  break;
                }
            }

          resp->hdr.msglen += resp->buflen;

          rndis_send_encapsulated_response(priv);
        }
        break;

      case RNDIS_SET_MSG:
        {
          FAR struct rndis_set_msg *req;
          FAR struct rndis_response_header *resp;

          req = malloc(USB_DWC_MAX_PACKET_SIZE);
          if (!req)
            {
              usb_err("malloc fail!\n");
              return;
            }
          (void)memcpy_s(req, USB_DWC_MAX_PACKET_SIZE, ctrlreq->buf, USB_DWC_MAX_PACKET_SIZE);

          resp_flag = rndis_prepare_response(priv, sizeof(struct rndis_response_header),
                                             cmd_hdr);
          if (!resp_flag)
            {
              free(req);
              return;
            }

          resp = (FAR struct rndis_response_header *)priv->resp_buf;

          if (req->objid == RNDIS_OID_GEN_CURRENT_PACKET_FILTER)
            {
              priv->rndis_packet_filter = req->buffer[0];

              if (req->buffer[0] == 0)
                {
                  UINFO("rndis set msg disconnect!\n");
                  priv->connected = false;
                }
              else
                {
                  UINFO("RNDIS is now connected\n");
                  priv->connected = true;
                  link_status_change(priv);
                }
            }
          else if (req->objid == RNDIS_OID_802_3_MULTICAST_LIST)
            {
              UINFO("RNDIS multicast list ignored\n");
            }
          else
            {
              resp->status = RNDIS_STATUS_NOT_SUPPORTED;
            }

          rndis_send_encapsulated_response(priv);
          free(req);
        }
        break;

      case RNDIS_RESET_MSG:
        {
          FAR struct rndis_reset_cmplt *resp;

          resp_flag = rndis_prepare_response(priv, sizeof(struct rndis_reset_cmplt),
                                             cmd_hdr);
          if (!resp_flag)
            {
              return;
            }

          resp = (FAR struct rndis_reset_cmplt *)priv->resp_buf;
          resp->addreset  = 0;
          priv->connected = false;
          UINFO("rndis reset msg disconnect!\n");
          rndis_send_encapsulated_response(priv);
        }
        break;

      case RNDIS_KEEPALIVE_MSG:
        {
          UINFO("rndis keepalive msg!\n");
          resp_flag = rndis_prepare_response(priv, sizeof(struct rndis_response_header),
                                             cmd_hdr);
          if (!resp_flag)
            {
              return;
            }

          rndis_send_encapsulated_response(priv);
        }
        break;

      default:
        usb_err("Unsupported RNDIS control message: %u\n", cmd_hdr->msgtype);
    }
}

/****************************************************************************
 * Name: rndis_rdcomplete
 *
 * Description:
 *   Handle completion of read request on the bulk OUT endpoint.
 *
 ****************************************************************************/

static void rndis_rdcomplete(FAR struct usbdev_ep_s *ep,
                             FAR struct usbdev_req_s *req)
{
  FAR struct rndis_dev_s *priv;
  irqstate_t flags;
  int ret;

  /* Sanity check */

#ifdef CONFIG_DEBUG_FEATURES
  if (!ep || !ep->priv || !req)
    {
      return;
     }
#endif

  /* Extract references to private data */

  priv = (FAR struct rndis_dev_s *)ep->priv;

  /* Process the received data unless this is some unusual condition */

  ret = OK;

  flags = enter_critical_section();
  priv->rdreq_submitted = false;

  switch (req->result)
    {
    case 0: /* Normal completion */
      ret = rndis_recvpacket(priv, req->buf, (uint16_t)req->xfrd);
      DEBUGASSERT(ret != -ENOMEM);
      break;

    case -ESHUTDOWN: /* Disconnection */
      leave_critical_section(flags);
      return;

    default: /* Some other error occurred */
      break;
    };

  if (ret == OK)
    {
      rndis_submit_rdreq(priv);
    }

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: rndis_wrcomplete
 *
 * Description:
 *   Handle completion of write request.  This function probably executes
 *   in the context of an interrupt handler.
 *
 ****************************************************************************/

static void rndis_wrcomplete(FAR struct usbdev_ep_s *ep,
                             FAR struct usbdev_req_s *req)
{
  FAR struct rndis_dev_s *priv;
  FAR struct usbdev_req_s *req_wr;
  irqstate_t flags;

  /* Sanity check */

#ifdef CONFIG_DEBUG_FEATURES
  if (!ep || !ep->priv || !req || !req->priv)
    {
      return;
    }
#endif

  /* Extract references to our private data */

  priv   = (FAR struct rndis_dev_s *)ep->priv;
  req_wr = (FAR struct usbdev_req_s *)req->priv;

  /* Return the write request to the free list */

  flags = enter_critical_section();

  switch (req_wr->result)
    {
    case OK: /* Normal completion */
      atomic_set(&priv->tx_transferring, 0);
      priv->rndis_host_rx_count++;
      break;

    case -ESHUTDOWN: /* Disconnection */
      break;

    default: /* Some other error occurred */
      break;
    }

  rndis_freewrreq(priv, req_wr);
  priv->epbulkin->handle_req = NULL;
  if (!rndis_hasfreereqs(priv))
    {
      (void)rndis_txavail(priv);
    }

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: usbclass_ep0incomplete
 *
 * Description:
 *   Handle completion of EP0 control operations
 *
 ****************************************************************************/

static void usbclass_ep0incomplete(FAR struct usbdev_ep_s *ep,
                                   FAR struct usbdev_req_s *req)
{
  (void)ep;

  if (req->result)
    {
      usb_err("result:%d, xfrd:%u, len:%u\n", req->result, req->xfrd, req->len);
    }
  else if (req->len > 0)
    {
      struct rndis_dev_s *priv = (FAR struct rndis_dev_s *)req->priv;
      priv->ctrlreq_has_encap_response = false;
    }
}

/****************************************************************************
 * Name: usbclass_ep0incomplete
 *
 * Description:
 *   Handle completion of interrupt IN endpoint operations
 *
 ****************************************************************************/

static void usbclass_epintin_complete(FAR struct usbdev_ep_s *ep,
                                      FAR struct usbdev_req_s *req)
{
  if (req->result || req->xfrd != req->len)
    {
      usb_err("result:%d, xfrd:%u, len:%u\n", req->result, req->xfrd, req->len);
    }
}

/****************************************************************************
 * Name: usbclass_freereq
 *
 * Description:
 *   Free a request instance along with its buffer
 *
 ****************************************************************************/

static void usbclass_freereq(FAR struct usbdev_ep_s *ep,
                             FAR struct usbdev_req_s *req)
{
  if (ep != NULL && req != NULL)
    {
      if (req->buf != NULL)
        {
          EP_FREEBUFFER(ep, req->buf);
        }

      EP_FREEREQ(ep, req);
    }
}

/****************************************************************************
 * Name: usbclass_allocreq
 *
 * Description:
 *   Allocate a request instance along with its buffer
 *
 ****************************************************************************/

static FAR struct usbdev_req_s *usbclass_allocreq(FAR struct usbdev_ep_s *ep,
                                                  uint16_t len)
{
  FAR struct usbdev_req_s *req;

  req = EP_ALLOCREQ(ep);
  if (req != NULL)
    {
      req->len = len;
      req->buf = EP_ALLOCBUFFER(ep, len);

      if (req->buf == NULL)
        {
          EP_FREEREQ(ep, req);
          req = NULL;
        }
    }

  return req;
}

/****************************************************************************
 * Name: usbclass_bind
 *
 * Description:
 *   Invoked when the driver is bound to a USB device driver
 *
 ****************************************************************************/

static int usbclass_bind(FAR struct usbdevclass_driver_s *driver,
                         FAR struct usbdev_s *dev)
{
  FAR struct rndis_dev_s *priv = ((FAR struct rndis_driver_s *)driver)->dev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct composite_dev_s *cdev;
  uint16_t reqlen;
  int ret;

  /* Bind the structures */

  priv->usbdev = dev;

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      usb_err("rndis eth bind failed!\n");
      return -1;
    }

  devobj = usbclass_devobj_get(cdev, DEV_ETHERNET);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;

  /* Preallocate control request */

  priv->ctrlreq = cdev->ctrlreq;
  if (priv->ctrlreq == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }

  /* Pre-allocate all endpoints... the endpoints will not be functional
   * until the SET CONFIGURATION request is processed in usbclass_setconfig.
   * This is done here because there may be calls to kmm_malloc and the SET
   * CONFIGURATION processing probably occurrs within interrupt handling
   * logic where kmm_malloc calls will fail.
   */

  priv->epintin = DEV_ALLOCEP(dev, g_rndis_hs_func_desc.nepd.bEndpointAddress,
                              (struct usb_endpoint_descriptor *)&g_rndis_hs_func_desc.nepd);
  if (!priv->epintin)
    {
      ret = -ENODEV;
      goto errout;
    }
  RNDIS_MKEPINTIN(devinfo) = priv->epintin->eplog;
  priv->epintin->priv      = priv;
  priv->epintin_enable     = false;

  priv->epintin_req = usbclass_allocreq(priv->epintin, sizeof(struct rndis_notification));
  if (priv->epintin_req == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }

  priv->epintin_req->callback = usbclass_epintin_complete;

  priv->epbulkin = DEV_ALLOCEP(dev, g_rndis_hs_func_desc.iepd.bEndpointAddress,
                               (struct usb_endpoint_descriptor *)&g_rndis_hs_func_desc.iepd);
  if (!priv->epbulkin)
    {
      ret = -ENODEV;
      goto errout;
    }
  RNDIS_MKEPBULKIN(devinfo)  = priv->epbulkin->eplog;
  priv->epbulkin->handle_req = NULL;
  priv->epbulkin->priv       = priv;
  priv->epbulkin_enable      = false;

  priv->epbulkout = DEV_ALLOCEP(dev, g_rndis_hs_func_desc.oepd.bEndpointAddress,
                                (struct usb_endpoint_descriptor *)&g_rndis_hs_func_desc.oepd);
  if (!priv->epbulkout)
    {
      ret = -ENODEV;
      goto errout;
    }
  RNDIS_MKEPBULKOUT(devinfo) = priv->epbulkout->eplog;
  priv->epbulkout->priv      = priv;
  priv->epbulkout_enable     = false;

  /* Pre-allocate read requests.  The buffer size is one full packet. */

  reqlen = 64;
  if (CONFIG_RNDIS_BULKOUT_REQLEN > reqlen)
    {
      reqlen = CONFIG_RNDIS_BULKOUT_REQLEN;
    }

  priv->rdreq = usbclass_allocreq(priv->epbulkout, reqlen);
  if (priv->rdreq == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }

  priv->rdreq->callback = rndis_rdcomplete;

  g_rndis_hs_func_desc.ifcad.bFirstInterface            = (uint8_t)devinfo->ifnobase;
  g_rndis_hs_func_desc.ifcd.bInterfaceNumber            = (uint8_t)devinfo->ifnobase;
  g_rndis_hs_func_desc.cdc_union_desc.bMasterInterface0 = (uint8_t)devinfo->ifnobase;

  g_rndis_hs_func_desc.ifdd.bInterfaceNumber            = (uint8_t)(devinfo->ifnobase + 1);
  g_rndis_hs_func_desc.cdc_union_desc.bSlaveInterface0  = (uint8_t)(devinfo->ifnobase + 1);

  /* Report if we are selfpowered */

#ifdef CONFIG_USBDEV_SELFPOWERED
  DEV_SETSELFPOWERED(dev);
#endif

  INIT_WORK(&priv->rxwork, rndis_rxdispatch);
  priv->rxwork.data = (atomic_long_t)priv;
  INIT_WORK(&priv->pollwork, rndis_txavail_work);
  priv->pollwork.data = (atomic_long_t)priv;
  priv->wq = create_workqueue("rndis workqueue");
  if (!priv->wq)
    {
      ret = -EFAULT;
      goto errout;
    }

  priv->connected = false;
  netdev_register(&priv->netdev);

  link_status_check_init(priv);

  /* And pull-up the data line for the soft connect function */

  DEV_CONNECT(dev);
  return OK;

errout:
  (void)usbclass_unbind(driver, dev);
  return ret;
}

/****************************************************************************
 * Name: usbclass_unbind
 *
 * Description:
 *    Invoked when the driver is unbound from a USB device driver
 *
 ****************************************************************************/

static int usbclass_unbind(FAR struct usbdevclass_driver_s *driver,
                           FAR struct usbdev_s *dev)
{
  FAR struct rndis_dev_s *priv;

#ifdef CONFIG_DEBUG_FEATURES
  if (!driver || !dev || !dev->ep0)
    {
      return -1;
    }
#endif

  /* Extract reference to private data */

  priv = ((FAR struct rndis_driver_s *)driver)->dev;

#ifdef CONFIG_DEBUG_FEATURES
  if (!priv)
    {
      return -1;
    }
#endif

  /* Make sure that we are not already unbound */

  if (priv != NULL)
    {
      destroy_workqueue(priv->wq);
      priv->wq = NULL;

      /* Make sure that the endpoints have been unconfigured.  If
       * we were terminated gracefully, then the configuration should
       * already have been reset.  If not, then calling usbclass_resetconfig
       * should cause the endpoints to immediately terminate all
       * transfers and return the requests to us (with result == -ESHUTDOWN)
       */

      usbclass_resetconfig(priv);
      mdelay(50);

      netdev_unregister(&priv->netdev);

      link_status_check_deinit(priv);

      /* Free the bulk IN endpoint */

      if (priv->epbulkin)
        {
          DEV_FREEEP(dev, priv->epbulkin);
          priv->epbulkin = NULL;
        }

      /* Free the pre-allocated control request */

      if (priv->ctrlreq != NULL)
        {
          priv->ctrlreq = NULL;
        }

      if (priv->epintin_req != NULL)
        {
          usbclass_freereq(priv->epintin, priv->epintin_req);
          priv->epintin_req = NULL;
        }

      /* Free the interrupt IN endpoint */

      if (priv->epintin)
        {
          DEV_FREEEP(dev, priv->epintin);
          priv->epintin = NULL;
        }

      /* Free pre-allocated read requests (which should all have
       * been returned to the free list at this time -- we don't check)
       */

      if (priv->rdreq)
        {
          usbclass_freereq(priv->epbulkout, priv->rdreq);
          priv->rdreq = NULL;
        }

      /* Free the bulk OUT endpoint */

      if (priv->epbulkout)
        {
          DEV_FREEEP(dev, priv->epbulkout);
          priv->epbulkout = NULL;
        }
    }

  return 0;
}

/****************************************************************************
 * Name: usbclass_setup
 *
 * Description:
 *   Invoked for ep0 control requests.  This function probably executes
 *   in the context of an interrupt handler.
 *
 ****************************************************************************/

static int usbclass_setup(FAR struct usbdevclass_driver_s *driver,
                          FAR struct usbdev_s *dev,
                          FAR const struct usb_device_request *ctrl,
                          FAR uint8_t *dataout, size_t outlen)
{
  FAR struct rndis_dev_s *priv;
  FAR struct usbdev_req_s *ctrlreq;
  uint16_t value;
  uint16_t len;
  int ret = -EOPNOTSUPP;

  (void)dataout;
  (void)outlen;

#ifdef CONFIG_DEBUG_FEATURES
  if (!driver || !dev || !dev->ep0 || !ctrl)
    {
      return -EIO;
     }
#endif

  /* Extract reference to private data */

  priv = ((FAR struct rndis_driver_s *)driver)->dev;

#ifdef CONFIG_DEBUG_FEATURES
  if (!priv || !priv->ctrlreq || !priv->resp_buf)
    {
      return -ENODEV;
    }
#endif
  ctrlreq = priv->ctrlreq;

  /* Extract the little-endian 16-bit values to host order */

  value = UGETW(ctrl->wValue);
  len   = UGETW(ctrl->wLength);

  switch (ctrl->bmRequestType & USB_TYPE_MASK)
    {
     /***********************************************************************
      * Standard Requests
      ***********************************************************************/

    case USB_TYPE_STANDARD:
      {
        switch (ctrl->bRequest)
          {
          case USB_REQ_SET_CONFIGURATION:
          case USB_REQ_SET_INTERFACE:
            {
              if (ctrl->bmRequestType == 0)
                {
                  return usbclass_setconfig(priv, value);
                }
            }
            break;

          default:
            usb_err("Request is not supported!\n");
            break;
          }
      }
      break;

    /* Class requests */

    case USB_TYPE_CLASS:
      {
        if ((ctrl->bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
          {
            if (ctrl->bRequest == RNDIS_SEND_ENCAPSULATED_COMMAND)
              {
                ctrlreq->callback = rndis_handle_control_message;
                ctrlreq->priv     = priv;
                ret               = len;
              }
            else if (ctrl->bRequest == RNDIS_GET_ENCAPSULATED_RESPONSE)
              {
                if (!priv->ctrlreq_has_encap_response)
                  {
                    ret = 1;
                    ctrlreq->buf[0] = 0;
                  }
                else
                  {
                    FAR struct rndis_response_header *hdr =
                      (FAR struct rndis_response_header *)priv->resp_buf;

                    /* Msglen represents the length of the message
                     * to be sent to the USB host.
                     */

                    ret = memcpy_s(ctrlreq->buf, hdr->msglen, priv->resp_buf, hdr->msglen);
                    if (ret != EOK)
                      {
                        usb_err("memcpy failed!\n");
                        free(priv->resp_buf);
                        priv->resp_buf = NULL;
                        return ret;
                      }
                    ret = hdr->msglen;
                    ctrlreq->callback = usbclass_ep0incomplete;
                    ctrlreq->priv     = priv;
                    free(priv->resp_buf);
                    priv->resp_buf = NULL;
                  }
              }
          }
      }
      break;

    default:
      break;
    }

  /* Respond to the setup command if data was returned.  On an error return
   * value (ret < 0), the USB driver will stall.
   */

  if (ret >= 0)
    {
      ctrlreq->len   = min(len, (uint32_t)ret);
      ctrlreq->flags = USBDEV_REQFLAGS_NULLPKT;
      ret            = EP_SUBMIT(dev->ep0, ctrlreq);
      if (ret < 0)
        {
          ctrlreq->result = OK;
          usbclass_ep0incomplete(dev->ep0, ctrlreq);
        }
    }

  return ret;
}

/****************************************************************************
 * Name: usbclass_disconnect
 *
 * Description:
 *   Invoked after all transfers have been stopped, when the host is
 *   disconnected.  This function is probably called from the context of an
 *   interrupt handler.
 *
 ****************************************************************************/

static void usbclass_disconnect(FAR struct usbdevclass_driver_s *driver,
                                FAR struct usbdev_s *dev)
{
  FAR struct rndis_dev_s *priv;

#ifdef CONFIG_DEBUG_FEATURES
  if (!driver || !dev || !dev->ep0)
    {
      return;
     }
#endif

  /* Extract reference to private data */

  priv = ((FAR struct rndis_driver_s *)driver)->dev;

#ifdef CONFIG_DEBUG_FEATURES
  if (!priv)
    {
      return;
    }
#endif

  /* Reset the configuration */

  usbclass_resetconfig(priv);

  /* Perform the soft connect function so that we will we can be
   * re-enumerated.
   */

  DEV_CONNECT(dev);
}

/****************************************************************************
 * Name: usbclass_resetconfig
 *
 * Description:
 *   Mark the device as not configured and disable all endpoints.
 *
 ****************************************************************************/

static void usbclass_resetconfig(FAR struct rndis_dev_s *priv)
{
  struct usbdev_req_s *req;
  irqstate_t flags;

  link_status_change(priv);

  /* Disable endpoints.  This should force completion of all pending
   * transfers.
   */

  if (priv->epintin_enable == true)
    {
      EP_DISABLE(priv->epintin);
      priv->epintin_enable = false;
    }
  if (priv->epbulkin_enable == true)
    {
      EP_DISABLE(priv->epbulkin);
      priv->epbulkin_enable = false;
    }
  if (priv->epbulkout_enable == true)
    {
      EP_DISABLE(priv->epbulkout);
      priv->epbulkout_enable = false;
    }

  flags = enter_critical_section();

  /* Are we configured? */

  if (priv->config != RNDIS_CONFIGIDNONE)
    {
      /* Yes.. but not anymore */

      priv->config = RNDIS_CONFIGIDNONE;

      priv->connected = false;

      if (priv->resp_buf != NULL)
        {
          free(priv->resp_buf);
          priv->resp_buf = NULL;
        }

      /* Free write requests that are not in use (which should be all
       * of them
       */

      req = priv->epbulkin->handle_req;
      if (req != NULL)
        {
          usbclass_freereq(priv->epbulkin, req);
          priv->epbulkin->handle_req = NULL;
        }

      if (priv->net_req)
        {
          usbclass_freereq(priv->epbulkout, priv->net_req);
          priv->net_req = NULL;
        }

      if (priv->rx_req)
        {
          usbclass_freereq(priv->epbulkout, priv->rx_req);
          priv->rx_req = NULL;
        }

      while (!list_empty(&priv->reqlist))
        {
          req = list_first_entry(&priv->reqlist, struct usbdev_req_s, list);
          list_del_init(&req->list);

          if (req->priv != NULL)
            {
              req->priv = NULL;
            }
          usbclass_freereq(priv->epbulkin, req);
        }
      atomic_set(&priv->tx_transferring, 0);
    }
  leave_critical_section(flags);
}

/****************************************************************************
 * Name: usbclass_setconfig
 *
 * Description:
 *   Set the device configuration by allocating and configuring endpoints and
 *   by allocating and queue read and write requests.
 *
 ****************************************************************************/

static int usbclass_setconfig(FAR struct rndis_dev_s *priv, uint8_t config)
{
  struct usbdev_s *dev;
  int ret;

#ifdef CONFIG_DEBUG_FEATURES
  if (priv == NULL)
    {
      return -EIO;
    }
#endif

  if (config == priv->config)
    {
      /* Already configured -- Do nothing */

      return 0;
    }

  /* Discard the previous configuration data */

  usbclass_resetconfig(priv);

  /* Was this a request to simply discard the current configuration? */

  if (config == RNDIS_CONFIGIDNONE)
    {
      return 0;
    }

  /* We only accept one configuration */

  if (config != RNDIS_CONFIGID)
    {
      return -EINVAL;
    }
  dev = priv->usbdev;

  /* Configure the IN interrupt endpoint */

  ret = EP_CONFIGURE(priv->epintin, &g_rndis_hs_func_desc.nepd, false);
  if (ret < 0)
    {
      goto errout;
    }

  priv->epintin->priv  = priv;
  priv->epintin_enable = true;

  /* Configure the IN bulk endpoint */

  usbd_configep_byspeed(dev, &g_rndis_hs_func_desc.iepd);
  ret = EP_CONFIGURE(priv->epbulkin, &g_rndis_hs_func_desc.iepd, false);
  if (ret < 0)
    {
      goto errout;
    }

  priv->epbulkin->priv  = priv;
  priv->epbulkin_enable = true;

  /* Configure the OUT bulk endpoint */

  usbd_configep_byspeed(dev, &g_rndis_hs_func_desc.oepd);
  ret = EP_CONFIGURE(priv->epbulkout, &g_rndis_hs_func_desc.oepd, true);
  if (ret < 0)
    {
      goto errout;
    }

  priv->epbulkout->priv  = priv;
  priv->epbulkout_enable = true;

  /* Queue read requests in the bulk OUT endpoint */

  priv->rdreq->callback = rndis_rdcomplete;

  ret = rndis_submit_rdreq(priv);
  if (ret != OK)
    {
      goto errout;
    }

  /* We are successfully configured */

  priv->config = config;

  link_status_change(priv);
  return OK;

errout:
  usbclass_resetconfig(priv);
  return ret;
}

void rndis_tx(struct los_eth_driver *sc, const struct eth_drv_sg *sg_list,
              int sg_len, uint32_t total_len, UINTPTR key)
{
  struct rndis_dev_s *dev = (struct rndis_dev_s *)sc->driver_context;
  struct usbdev_req_s *req;
  irqstate_t flags;
  uint8_t *d_buf;
  uint32_t len = 0;
  int ret;
  int i;

  (void)key;

  req = usbclass_allocreq(dev->epbulkin, (uint16_t)(total_len + RNDIS_PACKET_HDR_SIZE));
  if (req == NULL)
    {
      usb_err("alloc req failed!\n");
      return;
    }
  req->priv     = req;
  req->callback = rndis_wrcomplete;
  req->len      = total_len + RNDIS_PACKET_HDR_SIZE;

  d_buf = &req->buf[RNDIS_PACKET_HDR_SIZE];
  for (i = 0; i < sg_len; i++)
    {
      ret = memcpy_s((void *)(d_buf + len), total_len - len, (const void *)(sg_list[i].buf), sg_list[i].len);
      if (ret != EOK)
        {
          usb_err("memcpy fail, ret = %d\n", ret);
          return;
        }
      len += sg_list[i].len;
    }

  flags = enter_critical_section();

  if (dev->connected == false)
    {
      leave_critical_section(flags);
      usbclass_freereq(dev->epbulkin, req);
      return;
    }

  list_add_tail(&req->list, &dev->reqlist);

  (void)rndis_txavail(dev);
  leave_critical_section(flags);
}

int rndis_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                      struct usbdevclass_driver_s **devclass_drvr)
{
  FAR struct rndis_alloc_s *alloc;
  FAR struct rndis_dev_s *priv;
  FAR struct rndis_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  alloc = (FAR struct rndis_alloc_s *)malloc(sizeof(struct rndis_alloc_s));
  if (!alloc)
    {
      return -ENOMEM;
    }

  /* Convenience pointers into the allocated blob */

  priv = &alloc->dev;
  drvr = &alloc->drvr;

  /* Initialize the USB ethernet driver structure */

  (void)memset_s(priv, sizeof(struct rndis_dev_s), 0, sizeof(struct rndis_dev_s));
  INIT_LIST_HEAD(&priv->reqlist);
  eth_random_addr(priv->host_mac_address);

  (void)memset_s(&priv->netdev, sizeof(struct los_eth_driver), 0, sizeof(struct los_eth_driver));
  priv->netdev.driver_context = priv;

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_eth_driverops;
  drvr->dev        = priv;
  *devclass_drvr   = &drvr->drvr;

  return 0;
}

void rndis_uninitialize(struct usbdevclass_driver_s *devclass_drvr)
{
  struct rndis_driver_s *drvr = (struct rndis_driver_s *)devclass_drvr;
  struct rndis_dev_s *priv;
  struct rndis_alloc_s *alloc;

  if (drvr == NULL)
    {
      return;
    }

  priv = drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  alloc = container_of(drvr, struct rndis_alloc_s, drvr);

  free(alloc);
}

void usbdev_rndis_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the ETH driver to fill in the constants we didn't
   * know here.
   */

  rndis_get_composite_devdesc(dev);

  /* Overwrite and correct some values... */
  /* The callback functions for the ETH class */

  dev->classobject  = rndis_classobject;
  dev->uninitialize = rndis_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase;    /* Offset to Interface-IDs */
  dev->minor            = minor;       /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;            /* Offset to String Numbers */
}

int usbdev_rndis_initialize(struct module *mod, int n, void *arg)
{
  struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev;
  int ret;

  (void)mod;
  (void)n;

  if (com_s == NULL)
    {
      PRINTK("  ** Rndis device initialized failed! **\n");
      return -1;
    }
  usbdev_rndis_initialize_sub(&dev, 0, DEV_ETHERNET);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      PRINTK("  ** Rndis device initialized failed! **\n");
      return -1;
    }
  PRINTK("  ** Rndis device initialized successfully! **\n");
  return 0;
}
