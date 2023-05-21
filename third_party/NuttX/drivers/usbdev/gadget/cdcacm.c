/****************************************************************************
 * drivers/usbdev/cdcacm.c
 *
 *   Copyright (C) 2011-2013, 2016-2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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

#include <los_event.h>
#include <fs/fs.h>
#include <los_magickey.h>
#include <console.h>
#include <los_hwi.h>
#include <linux/spinlock.h>
#include "cdcacm.h"
#include "gadget/usbdev.h"

#ifdef CDCACM_DEBUG
#define FALOG(x...)   dprintf(x)
#else
#define FALOG(x...)   do{}while(0)
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

volatile unsigned int g_usbSerialMask;

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Container to support a list of requests */

struct cdcacm_wrreq_s
{
  struct usbdev_req_s *req;               /* The contained request */
  char *data_buffer;                      /* Ringbuffer starting address ,this is data buffer yet */
  char *receive_buffer;                   /* Record the offset of the received data */
  char *send_buffer;                      /* Record the offset of the sent data */
};

struct cdcacm_rdreq_s
{
  struct usbdev_req_s *req;               /* The contained request */
  void *buf;                              /* The read event buffer */
  size_t buflen;                          /* Offset to valid data in the RX request */
};

/* This structure describes the internal state of the driver */

struct cdcacm_dev_s
{
  struct usbdev_s *usbdev;                /* Usb device pointer */
  uint8_t config;                         /* Configuration number */
  uint16_t serialstate;                   /* State of the DSR/DCD */
  bool bdisconnect;                       /* Flags of device action */
  bool wating;                            /* if notify request is NULL, try submit(notify, req) again */
  bool received_char;                     /* Flags of write event */
  atomic_t shutdown;                      /* Device shutdown when transfer data */
  atomic_t send_char;                     /* Flag of read event */

  struct usb_cdc_line_state  linecoding;  /* Buffered line status */
  struct usbdev_ep_s *epintin;            /* Interrupt IN endpoint structure */
  struct usbdev_ep_s *epbulkin;           /* Bulk IN endpoint structure */
  struct usbdev_ep_s *epbulkout;          /* Bulk OUT endpoint structure */
  bool epintin_enabled;                   /* Interrupt IN endpoint enable flag */
  bool epbulkin_enabled;                  /* Bulk IN endpoint enable flag */
  bool epbulkout_enabled;                 /* Bulk OUT endpoint enable flag */
  struct usbdev_req_s *ctrlreq;           /* Allocated control request */
  struct usbdev_devinfo_s devinfo;

  /* Pre-allocated write request containers.  The write requests will
   * be save here, and used to send requests to EPBULKIN ; Read requests
   * will be save in the EBULKOUT.
   */

  struct cdcacm_wrreq_s wrreqs;           /* Serial write buffer */
  struct cdcacm_rdreq_s rdreqs;           /* Serial read buffer */
  EVENT_CB_S read_event;                  /* Read event */
  uint8_t ctrl_id;                        /* Control id of Interface-IDs */
  uint8_t data_id;                        /* Data id of Interface-IDs */
  spinlock_t acm_lock;                    /* Spinlock */
  uint32_t read_offset;                   /* Quick command offset */
};

/* The internal version of the class driver */

struct cdcacm_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct cdcacm_dev_s     *dev;
};

/* This is what is allocated */

struct cdcacm_alloc_s
{
  struct cdcacm_dev_s    dev;
  struct cdcacm_driver_s drvr;
};

int usbdev_cdcacm_initialize (FAR struct module *mod, int n, FAR void *arg);

/* Device driver structure definition */

static driver_t g_fcdcacm_driver_t =
{
  .name     = "fcdcacm",
  .methods  = NULL,
  .size     = sizeof(struct cdcacm_alloc_s)
};

/* Private device class information */

static devclass_t g_fcdcacm_devclass;
DRIVER_MODULE(fcdcacm, simple, g_fcdcacm_driver_t, g_fcdcacm_devclass, usbdev_cdcacm_initialize, 0);

atomic_t g_magickey = 0;
bool g_readpend = false;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Transfer helpers *********************************************************/

static int      cdcacm_sndpacket(FAR struct cdcacm_dev_s *priv,
                  FAR struct cdcacm_wrreq_s *wrcontainer);
static int      cdcacm_recvpacket(FAR struct cdcacm_dev_s *priv,
                  FAR struct cdcacm_rdreq_s *rdcontainer);
static int      cdcacm_requeue_rdrequest(FAR struct cdcacm_dev_s *priv,
                  FAR struct cdcacm_rdreq_s *rdcontainer);
static ssize_t  cdcacm_release_rxpending(FAR struct file *filep,
                  FAR char *buffer, size_t buflen);

/* Request helpers *********************************************************/

static struct usbdev_req_s *cdcacm_allocreq(FAR struct usbdev_ep_s *ep,
                              uint16_t len);
static void                 cdcacm_freereq(FAR struct usbdev_ep_s *ep,
                              FAR struct usbdev_req_s *req);

/* Configuration ***********************************************************/

static void    cdcacm_resetconfig(FAR struct cdcacm_dev_s *priv);

static int     cdcacm_epconfigure(FAR struct usbdev_ep_s *ep,
                 enum cdcacm_epdesc_e epid,
                 FAR const usb_endpoint_descriptor_t *ep_desc,
                 FAR struct usbdev_s *dev,
                 bool last);

static int     cdcacm_setconfig(FAR struct cdcacm_dev_s *priv,
                 FAR struct usbdev_s *dev);

/* Completion event handlers ***********************************************/

static void    cdcacm_rdcomplete(FAR struct usbdev_ep_s *ep,
                 FAR struct usbdev_req_s *req);
static void    cdcacm_wrcomplete(FAR struct usbdev_ep_s *ep,
                 FAR struct usbdev_req_s *req);

/* USB class device ********************************************************/

static int     cdcacm_bind(FAR struct usbdevclass_driver_s *driver,
                 FAR struct usbdev_s *dev);
static int     cdcacm_unbind(FAR struct usbdevclass_driver_s *driver,
                 FAR struct usbdev_s *dev);
static int     cdcacm_setup(FAR struct usbdevclass_driver_s *driver,
                 FAR struct usbdev_s *dev,
                 FAR const struct usb_device_request *ctrl, uint8_t *dataout,
                 size_t outlen);
static int     cdcacm_serial_connect(FAR struct cdcacm_dev_s *priv);
static int     cdcacm_serial_disconnect(FAR struct cdcacm_dev_s *priv);
static void    cdcacm_disconnect(FAR struct usbdevclass_driver_s *driver,
                 FAR struct usbdev_s *dev);
static ssize_t cdcuart_write(FAR struct file *filep,
                 FAR const char *buffer, size_t buflen);
static int     cdcuart_open(FAR struct file *filep);
static int     cdcuart_ioctl(FAR struct file *filep,
                 int cmd, unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct file_operations_vfs g_vfs_ops =
{
  .open   = cdcuart_open,
  .close  = NULL,
  .read   = cdcacm_release_rxpending,
  .write  = cdcuart_write,
  .seek   = NULL,
  .ioctl  = cdcuart_ioctl,
  .mmap   = NULL,
#ifndef CONFIG_DISABLE_POLL
  .poll   = NULL,
#endif
  .unlink = NULL,
};

/* USB class device *********************************************************/

static struct usbdevclass_driverops_s g_driver_ops =
{
  cdcacm_bind,           /* bind */
  cdcacm_unbind,         /* unbind */
  cdcacm_setup,          /* setup */
  cdcacm_disconnect,     /* disconnect */
  NULL,                  /* suspend */
  NULL,                  /* resume */
};

static void cdcacm_receive_buf(struct cdcacm_wrreq_s *wrcontainer,
                               FAR const char *buffer,
                               size_t buflen)
{
  int ret;

  /* Determine if the receive_buffer pointer is offset at the end of the ringbuffer */

  const size_t length = RING_BUFFER_SIZE - (wrcontainer->receive_buffer - wrcontainer->data_buffer);
  if (length > buflen)
    {
      ret = usbd_copy_from_user(wrcontainer->receive_buffer, length,
                                buffer, buflen);
      if (ret != EOK)
        {
          usb_err("Copy from user failed!\r\n");
          return;
        }
      wrcontainer->receive_buffer += buflen;
    }
  else
    {
      /* The length of console data is bigger than receive_buffer length.
       * The second parameter "length" is the currently available remaining length of the ringbuffer.
       */

      ret = usbd_copy_from_user(wrcontainer->receive_buffer, length,
                                buffer, length);
      if (ret)
        {
          usb_err("Copy from user failed!\r\n");
          return;
        }
      buffer += length;
      ret = usbd_copy_from_user(wrcontainer->data_buffer, RING_BUFFER_SIZE,
                                buffer, buflen - length);
      if (ret != EOK)
        {
          usb_err("Copy from user failed!\r\n");
          return;
        }
      wrcontainer->receive_buffer = wrcontainer->data_buffer + buflen - length;
    }
}

static ssize_t cdcuart_write(FAR struct file *filep,
                             FAR const char *buffer,
                             size_t buflen)
{
  struct cdcacm_dev_s *priv;
  struct cdcacm_wrreq_s *wrcontainer;
  int ret;
  uint32_t flags;

  struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
  priv = (struct cdcacm_dev_s *)drv->priv;
  if (priv == NULL)
    {
      usb_err("file write failed!\r\n");
      return -ENODEV;
    }

  wrcontainer = &priv->wrreqs;
  if (buflen)
    {
      cdcacm_receive_buf(wrcontainer, buffer, buflen);
    }
  spin_lock_irqsave(&priv->acm_lock, flags);
  ret = cdcacm_sndpacket(priv, wrcontainer);
  spin_unlock_irqrestore(&priv->acm_lock, flags);
  if (ret < 0)
    {
      usb_err("file write failed!\r\n");
    }

  return ret;
}

/****************************************************************************
 * Name: cdcacm_sndpacket
 *
 * Description:
 *   This function obtains write requests, transfers the TX data into the
 *   request, and submits the requests to the USB controller.  This
 *   continues until either (1) there are no further packets available, or
 *   (2) there is no further data to send.
 *
 ****************************************************************************/

static int cdcacm_sndpacket(FAR struct cdcacm_dev_s *priv,
                            FAR struct cdcacm_wrreq_s *wrcontainer)
{
  FAR struct usbdev_ep_s *ep;
  FAR struct usbdev_req_s *req;
  size_t data_size;
  size_t reqlen;
  int ret = 0;
  uint32_t length;

  if (priv == NULL)
    {
      return -EINVAL;
    }

  ep  = priv->epbulkin;
  req = wrcontainer->req;

#ifdef DWC3_USB_SERIAL
  reqlen = CONFIG_CDCACM_EPBULKIN_HSSIZE;
#else
  reqlen = CONFIG_CDCACM_EPBULKIN_FSSIZE;
#endif

  while (priv->received_char == false)
    {
      /* The data need to submit to host */

      data_size = (RING_BUFFER_SIZE + (wrcontainer->receive_buffer - wrcontainer->send_buffer)) % RING_BUFFER_SIZE;

      /* Stitching the beginning and end of the ringbuffer */

      length = RING_BUFFER_SIZE - (wrcontainer->send_buffer - wrcontainer->data_buffer);
      if (data_size == 0)
        {
          break;
        }
      if (data_size > MAX_PACKET_SIZE)
        {
          data_size = MAX_PACKET_SIZE;
        }
      if (length < data_size)
        {
          ret = memcpy_s(req->buf, reqlen, wrcontainer->send_buffer, length);
          if (ret != EOK)
            {
              usb_err("Memcpy failed!\r\n");
              return ret;
            }
          ret = memcpy_s(req->buf + length, reqlen, wrcontainer->data_buffer, data_size - length);
          if (ret != EOK)
            {
              usb_err("Memcpy failed!\r\n");
              return ret;
            }
          wrcontainer->send_buffer = wrcontainer->data_buffer + data_size - length;
        }
      else
        {
          ret = memcpy_s(req->buf, data_size, wrcontainer->send_buffer, data_size);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed!\r\n");
              return ret;
            }
          wrcontainer->send_buffer += data_size;
        }

      priv->received_char = true;

      /* Fill the request with serial TX data.
       * Then submit the request to the endpoint.
       */

      req->len  = data_size;
      req->priv = wrcontainer;
      ret       = EP_SUBMIT(ep, req);
      if (ret != OK)
        {
          usb_err("submit filed!\r\n");
          priv->received_char = false;
        }
    }
  return ret;
}

static int cdcacm_fill_rdrequest(struct cdcacm_dev_s *priv,
                                 struct cdcacm_rdreq_s *rdcontainer,
                                 size_t buflen)
{
  struct usbdev_req_s *req;
  char *req_buf;
  char *read_buf;
  int nbytes;
  int ret;

  req = rdcontainer->req;
  read_buf = rdcontainer->buf;
  req_buf  = (char *)req->buf;

  /* Copy character into read buffer and retrun the nbytes */

  ret = usbd_copy_to_user(read_buf, req->len, (req_buf + priv->read_offset), buflen);
  if (ret != EOK)
    {
      usb_err("memcpy_s failed, %d\n", ret);
      return -1;
    }

  nbytes = (int)buflen;
  priv->read_offset += buflen;

  if (priv->read_offset < req->xfrd)
    {
      ret = (int)LOS_EventWrite(&priv->read_event, USB_SERIAL_READ_EVENT);
      if (ret != OK)
        {
          usb_err("write event failed!\n");
          return -1;
        }
      return nbytes;
    }
  atomic_set(&priv->send_char, 0);
  priv->read_offset = 0;
  return nbytes;
}

/****************************************************************************
 * Name: cdcacm_recvpacket
 *
 * Description:
 *   A normal completion event was received by the read completion handler
 *   at the interrupt level (with interrupts disabled).  This function handles
 *   the USB packet and provides the received data to the uart RX buffer.
 *
 * Assumptions:
 *   Called from the USB interrupt handler with interrupts disabled.
 *
 ****************************************************************************/

static int cdcacm_recvpacket(FAR struct cdcacm_dev_s *priv,
                             FAR struct cdcacm_rdreq_s *rdcontainer)
{
  FAR struct usbdev_req_s *req;
  size_t buflen;
  int nbytes = 0;

  if (priv == NULL || rdcontainer == NULL)
    {
      usb_err("device unavailable!\r\n");
      return -ENODEV;
    }

  if (atomic_read(&g_magickey) == 1)
    {
      buflen = 1;
    }
  else
    {
      buflen = rdcontainer->buflen;
    }

  if (g_readpend == true && atomic_read(&g_magickey) == 0)
    {
      (void)LOS_EventRead(&priv->read_event, USB_SERIAL_READ_EVENT,
                          LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

  req = rdcontainer->req;
  if (req == NULL)
    {
      usb_err("illagel request!\r\n");
      return -ENODEV;
    }

  /* Process the received data unless this is some unusual condition */

  switch (req->result)
    {
    default: /* Some other error occurred */
      {
        FALOG("unexpected RX status\r\n");
      }

    case 0: /* Normal completion */
      {
        nbytes = cdcacm_fill_rdrequest(priv, rdcontainer, buflen);

        /* Tell host device is ready for next read event */

        (void)cdcacm_requeue_rdrequest(priv, rdcontainer);
      }
      break;

    case -ESHUTDOWN: /* Disconnection */
      {
        atomic_set(&priv->shutdown, 1);
        atomic_set(&priv->send_char, 0);
        FALOG("serial shutdown!\r\n");
      }
      break;
    }
  return nbytes;
}

/****************************************************************************
 * Name: cdcacm_requeue_rdrequest
 *
 * Description:
 *   Add any pending RX packets to the upper half serial drivers RX buffer.
 *
 ****************************************************************************/

static int cdcacm_requeue_rdrequest(FAR struct cdcacm_dev_s *priv,
                                    FAR struct cdcacm_rdreq_s *rdcontainer)
{
  FAR struct usbdev_req_s *req;
  FAR struct usbdev_ep_s *ep;
  uint32_t flags;
  int ret = 0;

  if (priv == NULL || rdcontainer == NULL)
    {
      return -EINVAL;
    }

  /* Shield shutdown interrupt signal */

  spin_lock_irqsave(&priv->acm_lock, flags);
  if (atomic_read(&priv->send_char) == 0)
    {

      atomic_set(&priv->send_char, 1);

      /* Requeue the read request */

      req      = rdcontainer->req;
      ep       = priv->epbulkout;
      req->len = ep->maxpacket;
      ret      = EP_SUBMIT(ep, req);
      if (ret != OK)
        {
          usb_err("epbulkout submit failed!\r\n");
          atomic_set(&priv->send_char, 0);
        }
    }
  spin_unlock_irqrestore(&priv->acm_lock, flags);
  return ret;
}

/****************************************************************************
 * Name: cdcacm_release_rxpending
 *
 * Description:
 *   Add any pending RX packets to the upper half serial drivers RX buffer.
 *
 ****************************************************************************/

static ssize_t cdcacm_release_rxpending(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
  FAR struct cdcacm_dev_s *priv;
  FAR struct cdcacm_rdreq_s *rdcontainer;
  ssize_t nbytes = 0;
  int ret = -1;

  struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
  priv = (struct cdcacm_dev_s *)drv->priv;
  if (priv == NULL)
    {
      return -EINVAL;
    }

  rdcontainer         = &priv->rdreqs;
  rdcontainer->buf    = buffer;
  rdcontainer->buflen = buflen;

  if (g_readpend == true)
    {
      /* cdcacm_recvpacket() will return OK if the entire packet was
       * successful buffered. In the case of RX buffer overrun,
       * cdcacm_recvpacket() will return a failure (-ENOSPC) and will
       * set the req->offset field
       */

      nbytes = (ssize_t)cdcacm_recvpacket(priv, rdcontainer);

      /* The entire packet was processed and may be removed from the
       * pending RX list and returned to the DCD.
       */

      /* If we are still connected, refill the USB RX buffer. */

      if (atomic_read(&priv->shutdown) == 0)
        {
          ret = cdcacm_requeue_rdrequest(priv, rdcontainer);
          if (ret < 0)
            {
              usb_err("device is not ready for receive next data \r\n");
              return ret;
            }
        }
    }

  return nbytes;
}

/****************************************************************************
 * Name: cdcacm_allocreq
 *
 * Description:
 *   Allocate a request instance along with its buffer
 *
 ****************************************************************************/

static struct usbdev_req_s *cdcacm_allocreq(FAR struct usbdev_ep_s *ep, uint16_t len)
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
          return NULL;
        }
      #ifdef CONFIG_USBDEV_DMA
      req->dma = (DMA_ADDR_T)VMM_TO_UNCACHED_ADDR((unsigned long)req->buf);
      #else
      req->dma = NULL;
      #endif
    }
  return req;
}

/****************************************************************************
 * Name: cdcacm_freereq
 *
 * Description:
 *   Free a request instance along with its buffer
 *
 ****************************************************************************/

static void cdcacm_freereq(FAR struct usbdev_ep_s *ep,
                           FAR struct usbdev_req_s *req)
{
  if (ep != NULL && req != NULL)
    {
      if (req->buf != NULL)
        {
          EP_FREEBUFFER(ep, req->buf);
		  req->buf = NULL;
        }

      EP_FREEREQ(ep, req);
    }
}

/****************************************************************************
 * Name: cdcacm_resetconfig
 *
 * Description:
 *   Mark the device as not configured and disable all endpoints.
 *
 ****************************************************************************/

static void cdcacm_resetconfig(FAR struct cdcacm_dev_s *priv)
{
  if (priv == NULL)
    {
      usb_err("Disable ep failed!\r\n");
      return;
    }

  /* Disable endpoints. This should force completion of all pending
   * transfers.
   */

  if (priv->epintin_enabled == true)
    {
      EP_DISABLE(priv->epintin);
      priv->epintin_enabled = false;
    }

  if (priv->epbulkin_enabled == true)
    {
      EP_DISABLE(priv->epbulkin);
      priv->epbulkin_enabled = false;
    }

  if (priv->epbulkout_enabled == true)
    {
      EP_DISABLE(priv->epbulkout);
      priv->epbulkout_enabled = false;
    }
}

/****************************************************************************
 * Name: cdcacm_epconfigure
 *
 * Description:
 *   Configure one endpoint.
 *
 ****************************************************************************/

static int cdcacm_epconfigure(FAR struct usbdev_ep_s *ep,
                              enum cdcacm_epdesc_e epid,
                              FAR const usb_endpoint_descriptor_t *ep_desc,
                              FAR struct usbdev_s *dev,
                              bool last)
{
  if (epid != CDCACM_EPINTIN)
    {
      usbd_configep_byspeed(dev, (struct usb_endpoint_descriptor *)ep_desc);
    }
  return EP_CONFIGURE(ep, ep_desc, last);
}

/****************************************************************************
 * Name: cdcacm_setconfig
 *
 * Description:
 *   Set the device configuration by allocating and configuring endpoints and
 *   by allocating and queue read and write requests.
 *
 ****************************************************************************/

static int cdcacm_setconfig(FAR struct cdcacm_dev_s *priv,
                            struct usbdev_s *dev)
{
  FAR struct cdcacm_rdreq_s *rdcontainer;
  int ret;
  uint32_t flags;

  if (priv == NULL)
    {
      return -EINVAL;
    }

  rdcontainer = &priv->rdreqs;

  if (priv->epintin_enabled == true)
    {
      EP_DISABLE(priv->epintin);
      priv->epintin_enabled = false;
    }

  /* Configure the IN interrupt endpoint */

  ret = cdcacm_epconfigure(priv->epintin, CDCACM_EPINTIN,
                           (const usb_endpoint_descriptor_t *)&g_cdcacm_hs_func_desc.nepd, dev, false);
  if (ret < 0)
    {
      usb_err("config interrupt ep failed!\r\n");
      goto errout;
    }
  priv->epintin_enabled = true;
  priv->epintin->priv   = priv;

  if (priv->epbulkin_enabled == true)
    {
      EP_DISABLE(priv->epbulkin);
      priv->epbulkin_enabled = false;
    }

  /* Configure the IN bulk endpoint */

  ret = cdcacm_epconfigure(priv->epbulkin, CDCACM_EPBULKIN,
                           (const usb_endpoint_descriptor_t *)&g_cdcacm_hs_func_desc.iepd, dev, false);
  if (ret < 0)
    {
      usb_err("config bulk in ep failed!\r\n");
      goto errout;
    }
  priv->epbulkin_enabled = true;
  priv->epbulkin->priv   = priv;

  if (priv->epbulkout_enabled == true)
    {
      EP_DISABLE(priv->epbulkout);
      priv->epbulkout_enabled = false;
    }

  /* Configure the OUT bulk endpoint */

  ret = cdcacm_epconfigure(priv->epbulkout, CDCACM_EPBULKOUT,
                           (const usb_endpoint_descriptor_t *)&g_cdcacm_hs_func_desc.oepd, dev, false);
  if (ret < 0)
    {
      usb_err("config bulk out ep failed!\r\n");
      goto errout;
    }
  priv->epbulkout_enabled = true;
  priv->epbulkout->priv   = priv;

  /* Queue read requests in the bulk OUT endpoint */

  ret = cdcacm_requeue_rdrequest(priv, rdcontainer);
  if (ret != OK)
    {
      goto errout;
    }

  /* We are successfully configured.
   * Tell host that device is available
   * and ready for data transfer.
   */

  spin_lock_irqsave(&priv->acm_lock, flags);
  ret = cdcacm_serial_connect(priv);
  if (ret < 0)
    {
      usb_err("cdcacm connect failed!\r\n");
      spin_unlock_irqrestore(&priv->acm_lock, flags);
      goto errout;
    }
  atomic_set(&priv->shutdown, 0);
  priv->bdisconnect = false;

  /* tell console dprintf to virtrue serial */

  g_usbSerialMask = 1;
  spin_unlock_irqrestore(&priv->acm_lock, flags);

  return OK;

errout:
  cdcacm_resetconfig(priv);
  return ret;
}

/****************************************************************************
 * Name: cdcacm_rdcomplete
 *
 * Description:
 *   Handle completion of read request on the bulk OUT endpoint.  This
 *   is handled like the receipt of serial data on the "UART"
 *
 ****************************************************************************/

static void cdcacm_rdcomplete(FAR struct usbdev_ep_s *ep,
                              FAR struct usbdev_req_s *req)
{
  FAR struct cdcacm_rdreq_s *rdcontainer;
  FAR struct cdcacm_dev_s *priv;
  uint32_t ret;
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, req->xfrd);
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, req->result);

  if (ep == NULL || ep->priv == NULL || req == NULL)
    {
      usb_err("illegal request or ep\r\n");
      return;
    }

  /* Extract references to private data */

  priv = (struct cdcacm_dev_s *)ep->priv;

  rdcontainer = &priv->rdreqs;

  /* Save private data of reqd request */

  rdcontainer->req      = req;
  rdcontainer->req->len = req->len;

  /* Magic key */

  if (CheckMagicKey(*(char *)req->buf, CONSOLE_SERIAL))
    {
      atomic_set(&g_magickey, 1);
      (void)cdcacm_recvpacket(priv, rdcontainer);
      (void)cdcacm_requeue_rdrequest(priv, rdcontainer);
      atomic_set(&g_magickey, 0);
      return;
    }

  ret = LOS_EventWrite(&priv->read_event, USB_SERIAL_READ_EVENT);
  if (ret != OK)
    {
      usb_err("write event failed!\r\n");
    }
}

/****************************************************************************
 * Name: cdcacm_wrcomplete
 *
 * Description:
 *   Handle completion of write request.  This function probably executes
 *   in the context of an interrupt handler.
 *
 ****************************************************************************/

static void cdcacm_wrcomplete(FAR struct usbdev_ep_s *ep,
                              FAR struct usbdev_req_s *req)
{
  FAR struct cdcacm_dev_s *priv;
  FAR struct cdcacm_wrreq_s *wrcontainer;

  /* Extract references to our private data */
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, req->xfrd);
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, req->result);

  priv = (struct cdcacm_dev_s *)ep->priv;
  if (priv == NULL)
    {
      usb_err("device is unavailable!\r\n");
      return;
    }
  spin_lock(&priv->acm_lock);

  /* protect for received_char */

  wrcontainer = (struct cdcacm_wrreq_s *)req->priv;

  priv->received_char = false;

  /* Send the next packet unless this was some unusual termination
   * condition
   */

  switch (req->result)
    {
    default: /* Some other error occurred */
      {
        FALOG("unexpected RX status\r\n");
      }

    case 0: /* Normal completion */
      {
        (void)cdcacm_sndpacket(priv, wrcontainer);
      }
      break;

    case -ESHUTDOWN: /* Disconnection */
      {
        FALOG("usb device disconnect!");
      }
      break;
    }
  spin_unlock(&priv->acm_lock);
}

static void cdcacm_notify_complete(FAR struct usbdev_ep_s *ep,
                                   FAR struct usbdev_req_s *usbd_req)
{
  FAR struct cdcacm_dev_s *priv;
  bool flag = false;
  int ret;
  uint32_t flags;
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, usbd_req->xfrd);
  PRINT_ERR("%s req->xfrd:%x\r\n", __FUNCTION__, usbd_req->result);

  (void)ep;
  priv = usbd_req->priv;
  if (priv == NULL)
    {
      usb_err("No device!\r\n");
      return;
    }

  spin_lock_irqsave(&priv->acm_lock, flags);
  if (usbd_req->result != -ESHUTDOWN)
    {
      flag = priv->wating;
    }

  if (flag == true)
    {
      ret = cdcacm_serial_connect(priv);
      if (ret < 0)
        {
          usb_err("cdcacm connect failed!\r\n");
        }
    }
  spin_unlock_irqrestore(&priv->acm_lock, flags);
}

/****************************************************************************
 * USB Class Driver Methods
 ****************************************************************************/

/****************************************************************************
 * Name: cdcacm_bind
 *
 * Description:
 *   Invoked when the driver is bound to a USB device driver
 *
 ****************************************************************************/

static int cdcacm_bind(FAR struct usbdevclass_driver_s *driver,
                       FAR struct usbdev_s *dev)
{
  FAR struct cdcacm_dev_s *priv;
  FAR struct composite_devobj_s *devobj;
  FAR struct usbdev_devinfo_s *devinfo;
  FAR struct cdcacm_wrreq_s *wrcontainer;
  FAR struct cdcacm_rdreq_s *rdcontainer;
  FAR struct composite_dev_s *cdev;
  uint16_t reqlen;
  int ret;

  if (driver == NULL || dev == NULL)
    {
      usb_err("Bind failed!\r\n");
      return -ENODEV;
    }

  priv = ((struct cdcacm_driver_s *)driver)->dev;

  /* Bind the structures */

  priv->usbdev      = dev;
  priv->bdisconnect = true;
  atomic_set(&priv->shutdown, 0);

  /* Save the reference to our private data structure in EP0 so that it
   * can be recovered in ep0 completion events (Unless we are part of
   * a composite device and, in that case, the composite device owns
   * EP0).
   */

  cdev = dev->ep0->priv;
  (VOID)LOS_EventInit(&priv->read_event);
  spin_lock_init(&priv->acm_lock);

  /* Register a character driver inode the pseudo file system */

  ret = register_driver(CDCACM_DEVNAME_FORMAT, &g_vfs_ops, 0666, priv);
  if (ret != 0)
    {
      usb_err("chardev register failed!");
      return ret;
    }

  /* Preallocate control request */

  priv->ctrlreq = cdev->ctrlreq;
  if (priv->ctrlreq == NULL)
    {
      ret = -EINVAL;
      goto errout;
    }

  devobj = usbclass_devobj_get(cdev, DEV_SERIAL);
  if (devobj == NULL)
    {
      ret = -EINVAL;
      goto errout;
    }
  devinfo = &devobj->compdesc.devinfo;

  priv->ctrl_id = (uint8_t)devinfo->ifnobase;
  priv->data_id = (uint8_t)devinfo->ifnobase + 1;

  /* Pre-allocate all endpoints... the endpoints will not be functional
   * until the SET CONFIGURATION request is processed in cdcacm_setconfig.
   * This is done here because there may be calls to kmm_malloc and the SET
   * CONFIGURATION processing probably occurrs within interrupt handling
   * logic where kmm_malloc calls will fail.
   */

  /* Pre-allocate the IN interrupt endpoint */

  priv->epintin = DEV_ALLOCEP(dev, g_cdcacm_hs_func_desc.nepd.bEndpointAddress, &g_cdcacm_hs_func_desc.nepd);
  if (!priv->epintin)
    {
      usb_err("alloc epintin fail!");
      ret = -ENODEV;
      goto errout;
    }
  CDCACM_MKEPINTIN(devinfo) = priv->epintin->eplog;
  priv->epintin->priv = priv;
  priv->epintin->handle_req = cdcacm_allocreq(priv->epintin,
                                              sizeof(struct usb_cdc_notification) + sizeof(priv->serialstate));
  if (priv->epintin->handle_req == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }
  priv->epintin->handle_req->callback = cdcacm_notify_complete;

  /* Pre-allocate the IN bulk endpoint */

  priv->epbulkin = DEV_ALLOCEP(dev, g_cdcacm_hs_func_desc.iepd.bEndpointAddress, &g_cdcacm_hs_func_desc.iepd);
  if (!priv->epbulkin)
    {
      usb_err("alloc epbulkin fail!");
      ret = -ENODEV;
      goto errout;
    }
  CDCACM_MKEPBULKIN(devinfo) = priv->epbulkin->eplog;
  priv->epbulkin->priv = priv;

  /* Pre-allocate the OUT bulk endpoint */

  priv->epbulkout = DEV_ALLOCEP(dev, g_cdcacm_hs_func_desc.oepd.bEndpointAddress, &g_cdcacm_hs_func_desc.oepd);
  if (!priv->epbulkout)
    {
      usb_err("alloc epbulkout fail!");
      ret = -ENODEV;
      goto errout;
    }
  CDCACM_MKEPBULKOUT(devinfo) = priv->epbulkout->eplog;
  priv->epbulkout->priv = priv;

  /* Pre-allocate read requests.  The buffer size is one full packet. */

#ifdef DWC3_USB_SERIAL
  reqlen = CONFIG_CDCACM_EPBULKOUT_HSSIZE;
#else
  reqlen = CONFIG_CDCACM_EPBULKOUT_FSSIZE;
#endif

  priv->epbulkout->maxpacket = reqlen;

  /* initialize read req buffer */

  rdcontainer = &priv->rdreqs;

  /* alloc req and req->buf */

  rdcontainer->req = cdcacm_allocreq(priv->epbulkout, reqlen);
  if (rdcontainer->req == NULL)
    {
      usb_err("allocreq failed!\r\n");
      ret = -ENOMEM;
      goto errout;
    }

  rdcontainer->buflen         = reqlen;
  rdcontainer->req->priv      = rdcontainer;
  rdcontainer->req->callback  = cdcacm_rdcomplete;
  priv->epbulkout->handle_req = rdcontainer->req;

  /* Pre-allocate write request containers and put in a free list.  The
   * buffer size should be larger than a full build IN packet.  Otherwise,
   * we will send a bogus null packet at the end of each packet.
   *
   * Pick the larger of the max packet size and the configured request size.
   *
   * NOTE: These write requests are sized for the bulk IN endpoint but are
   * shared with interrupt IN endpoint which does not need a large buffer.
   */

#ifdef DWC3_USB_SERIAL
  reqlen = CONFIG_CDCACM_EPBULKIN_HSSIZE;
#else
  reqlen = CONFIG_CDCACM_EPBULKIN_FSSIZE;
#endif

  wrcontainer      = &priv->wrreqs;
  wrcontainer->req = cdcacm_allocreq(priv->epbulkin, reqlen);
  if (wrcontainer->req == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }
  wrcontainer->req->priv     = wrcontainer;
  wrcontainer->req->callback = cdcacm_wrcomplete;
  priv->epbulkin->handle_req = wrcontainer->req;

  /* Alloc mem for ringbuffer */

  wrcontainer->data_buffer = malloc(RING_BUFFER_SIZE);
  if (wrcontainer->data_buffer == NULL)
    {
      ret = -ENOMEM;
      goto errout;
    }
  (void)memset_s(wrcontainer->data_buffer, RING_BUFFER_SIZE, 0, RING_BUFFER_SIZE);
  wrcontainer->receive_buffer = wrcontainer->data_buffer;
  wrcontainer->send_buffer    = wrcontainer->data_buffer;

  return OK;

errout:
  (void)cdcacm_unbind(driver, dev);
  return ret;
}

/****************************************************************************
 * Name: cdcacm_unbind
 *
 * Description:
 *    Invoked when the driver is unbound from a USB device driver
 *
 ****************************************************************************/

static int cdcacm_unbind(FAR struct usbdevclass_driver_s *driver,
                         FAR struct usbdev_s *dev)
{
  FAR struct cdcacm_dev_s *priv;
  FAR struct cdcacm_wrreq_s *wrcontainer;
  FAR struct cdcacm_rdreq_s *rdcontainer;
  int ret = -EINVAL;

  if (driver == NULL || dev == NULL)
    {
      usb_err("no device!\r\n");
      return -EINVAL;
    }

  /* Extract reference to private data */

  priv = ((FAR struct cdcacm_driver_s *)driver)->dev;

  /* Make sure that we are not already unbound */

  if (priv != NULL)
    {
      /* Close device */

      cdcacm_disconnect(driver, dev);

      if (priv->epintin->handle_req != NULL)
        {
          cdcacm_freereq(priv->epintin, priv->epintin->handle_req);
          priv->epintin->handle_req = NULL;
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

      rdcontainer = &priv->rdreqs;
      if (rdcontainer->req != NULL)
        {
          cdcacm_freereq(priv->epbulkout, rdcontainer->req);
          rdcontainer->req = NULL;
        }

      /* Free the bulk OUT endpoint */

      if (priv->epbulkout)
        {
          DEV_FREEEP(dev, priv->epbulkout);
          priv->epbulkout = NULL;
        }

      /* Free write requests that are not in use (which should be all
       * of them)
       */

      wrcontainer = &priv->wrreqs;
      if (wrcontainer->req != NULL)
        {
          cdcacm_freereq(priv->epbulkin, wrcontainer->req);
          wrcontainer->req = NULL;
        }

      /* Free the bulk IN endpoint */

      if (priv->epbulkin)
        {
          DEV_FREEEP(dev, priv->epbulkin);
          priv->epbulkin = NULL;
        }

      /* Unregister driver */

      ret = unregister_driver(CDCACM_DEVNAME_FORMAT);
      if (ret != OK)
        {
          PRINT_ERR("%s chardev unregister failed!\n", __FUNCTION__);
          return ret;
        }

        /* Destroy read event */

      (void)LOS_EventDestroy(&priv->read_event);

      /* Free ringbuffer */

      if (wrcontainer->data_buffer != NULL)
        {
          free(wrcontainer->data_buffer);
          wrcontainer->data_buffer = NULL;
        }
    }

  return ret;
}

static void cdcacm_set_line_complete(FAR struct usbdev_ep_s *ep,
                                     FAR struct usbdev_req_s *req)
{
  FAR struct cdcacm_dev_s *priv;
  int ret;

  (void)ep;
  priv = req->priv;
  if (req->result != 0)
    {
      return;
    }

  g_usbSerialMask = 1;
  ret = memcpy_s(&priv->linecoding, sizeof(struct usb_cdc_line_state), req->buf, SIZEOF_CDC_LINECODING);
  if (ret != EOK)
    {
      usb_err("Memcpy failed!\r\n");
      return;
    }
}

/****************************************************************************
 * Name: cdcacm_setup
 *
 * Description:
 *   Invoked for ep0 control requests.  This function probably executes
 *   in the context of an interrupt handler.
 *
 ****************************************************************************/

static int cdcacm_setup(FAR struct usbdevclass_driver_s *driver,
                        FAR struct usbdev_s *dev,
                        FAR const struct usb_device_request *ctrl,
                        uint8_t *dataout, size_t outlen)
{
  FAR struct cdcacm_dev_s *priv;
  FAR struct usbdev_req_s *req;
  uint16_t value;
  uint16_t index;
  uint16_t len;
  int val = -EOPNOTSUPP;
  int ret;

  (void)dataout;
  (void)outlen;
  if (driver == NULL || dev == NULL || ctrl == NULL)
    {
      return -EINVAL;
    }

  /* Extract reference to private data */

  priv = ((struct cdcacm_driver_s *)driver)->dev;
  if (priv == NULL)
    {
      return -ENODEV;
    }

  req = priv->ctrlreq;

  /* Extract the little-endian 16-bit values to host order */

  value = UGETW(ctrl->wValue);
  index = UGETW(ctrl->wIndex);
  len   = UGETW(ctrl->wLength);

  FALOG("type=%02x req=%02x value=%04x index=%04x len=%04x\n",
        ctrl->bmRequestType, ctrl->bRequest, value, index, len);
  if (UT_GET_TYPE(ctrl->bmRequestType) == UT_STANDARD)
    {
      /**********************************************************************
       * Standard Requests
      **********************************************************************/

      switch (ctrl->bRequest)
        {
        case USB_REQ_SET_CONFIGURATION:
          if (ctrl->bmRequestType == 0)
            {
              val = cdcacm_setconfig(priv, dev);
              return val;
            }
          break;

        case USB_REQ_SET_INTERFACE:
          break;

        /* If the serial device is used in as part of a composite device,
         * then the overall composite class configuration is managed by logic
         * in the composite device implementation.
         */

        default:
          break;
        }
    }
  else if (UT_GET_TYPE(ctrl->bmRequestType) == UT_CLASS)
    {
      switch (ctrl->bRequest)
        {
        /* ACM_GET_LINE_CODING requests current DTE rate, stop-bits, parity,
         * and number-of-character bits. (Optional)
         */

        case ACM_GET_LINE_CODING:
          if (ctrl->bmRequestType == (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) &&
              index == priv->devinfo.ifnobase) /* bmRequestType = 0xa1, bRequest = 0x21 */
            {
              if(index != priv->ctrl_id)
                {
                  goto failed_out;
                }

              /* Return the current line status from the private data structure. */

              val = (int)min_t(unsigned, len, sizeof(struct usb_cdc_line_state));
              ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, &priv->linecoding, (uint32_t)val);
              if (ret != EOK)
                {
                  usb_err("memcpy_s fail, ret:%d\n", ret);
                  return ret;
                }
            }
          else
            {
              goto failed_out;
            }
          break;

        /* ACM_SET_LINE_CODING configures DTE rate, stop-bits, parity, and
         * number-of-character bits. (Optional)
         */

        case ACM_SET_LINE_CODING:
          if (ctrl->bmRequestType == (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) &&
              len == SIZEOF_CDC_LINECODING && /* dataout && len == outlen && */
              index == priv->devinfo.ifnobase) /* bmRequestType = 0x21, bRequest = 0x20 */
            {
              if (len != sizeof(struct usb_cdc_line_state) ||
                  index != priv->ctrl_id)
                {
                  goto failed_out;
                }

              /* Save the new line coding in the private data structure.
               * NOTE: that this is conditional now because not all device
               * controller drivers supported provision of EP0 OUT data
               * with the setup command.
               */

              req->priv = priv;
              req->callback = cdcacm_set_line_complete;

              /* Respond with a zero length packet */

              val = len;
            }
          else
            {
              goto failed_out;
            }
          break;

        /* ACM_SET_CTRL_LINE_STATE: RS-232 signal used to tell the DCE
         * device the DTE device is now present. (Optional)
         */

        case ACM_SET_CTRL_LINE_STATE:
          if (ctrl->bmRequestType == (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) &&
              index == priv->devinfo.ifnobase) /* bmRequestType = 0x21, bRequest = 0x22 */
            {
              if(index != priv->ctrl_id)
                {
                  goto failed_out;
                }

              /* Save the control line state in the private data
               * structure. Only bits 0 and 1 have meaning.  Respond with a zero length packet.
               */

              val = 0;
            }
          else
            {
              goto failed_out;
            }
          break;
        default:
          break;
failed_out:
          usb_err("type=%02x req=%02x value=%04x index=%04x len=%04x\n",
                  ctrl->bmRequestType, ctrl->bRequest, value, index, len);
        }
    }

  if (val >= 0)
    {
      /* Configure the response */

      req->len = (uint32_t)val;
      val      = EP_SUBMIT(dev->ep0, req);
      if (val != OK)
        {
          usb_err("acm response on err %d\n", val);
          req->result = OK;
        }
    }
  return val;
}

static uint32_t cdcacm_serial_connect_state(const struct cdcacm_dev_s *priv,
                                            struct usbdev_req_s *req,
                                            const void *data,
                                            uint16_t value,
                                            uint32_t len)
{
  struct usb_cdc_notification *state_buf;
  void *buf;
  uint32_t length;
  uint32_t ret;

  state_buf = (struct usb_cdc_notification *)req->buf;

  /* bmRequestType = 0xa1 , bNotification = 0x20 */

  state_buf->bmRequestType = USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE;
  state_buf->bNotification = ACM_NOTIFY_SERIAL_STATE;
  USETW(state_buf->wValue, value);
  USETW(state_buf->wIndex, priv->ctrl_id);
  USETW(state_buf->wLength, len);
  length = sizeof(struct usb_cdc_notification) + len;

  buf = state_buf + STAE_BUFFER_OFFSET;
  ret = (uint32_t)memcpy_s(buf, sizeof(struct usb_cdc_notification), data, len);
  if (ret != EOK)
    {
      usb_err("memcpy failed!\r\n");
      return ret;
    }

  return length;
}

static int cdcacm_serial_connect(FAR struct cdcacm_dev_s *priv)
{
  FAR struct usbdev_ep_s *ep;
  FAR struct usbdev_req_s *req;
  uint32_t length;
  int ret;

  if (priv == NULL)
    {
      usb_err("no device can be found!\r\n");
      return -ENODEV;
    }

  /* DCD:online; DSR:ready for data transhfer */

  priv->serialstate |= CDCACM_UART_DSR | CDCACM_UART_DCD;
  ep = priv->epintin;
  if (ep == NULL)
    {
      return -EINVAL;
    }
  req = ep->handle_req;
  if (req == NULL)
    {
      priv->wating = true;
      return 0;
    }

  /* Status of serial for connect and disconnect */

  length = cdcacm_serial_connect_state(priv, req, &priv->serialstate, 0, sizeof(priv->serialstate));

  req->priv = priv;
  req->len  = length;

  /* submit serial notify state */

  ret = EP_SUBMIT(ep, req);
  if (ret != OK)
    {
      usb_err("can not send notify data to host\r\n");
    }
  priv->wating = false;
  return ret;
}

static int cdcacm_serial_disconnect(FAR struct cdcacm_dev_s *priv)
{
  FAR struct usbdev_req_s *req;
  FAR struct usbdev_ep_s *ep;
  uint32_t length;
  int ret;

  g_usbSerialMask = 0;
  priv->received_char = false;

  /* Offline, close data transfer */

  priv->serialstate &= ~CDC_UART_CONSISTENT;
  ep  = priv->epintin;
  req = ep->handle_req;

  /* Status of serial for connect and disconnect */

  length = cdcacm_serial_connect_state(priv, req, &priv->serialstate, 0, sizeof(priv->serialstate));

  req->len = length;

  /* Submit serial notify state */

  ret = EP_SUBMIT(ep, req);
  if (ret != OK)
    {
      usb_err("Can not send notify data to host!\r\n");
    }

  return ret;
}

static int cdcuart_open(FAR struct file *filep)
{
  /* do nothing */

  (void)filep;
  return 0;
}

/****************************************************************************
 * Name: cdcuart_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int cdcuart_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
  //FAR struct inode *inode;
  FAR struct cdcacm_dev_s *priv;

  struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
  priv = (struct cdcacm_dev_s *)drv->priv;
  if (priv == NULL)
    {
      usb_err("No Device!\r\n");
      return -ENODEV;
    }
  switch (cmd)
    {
    /* CAICO_REGISTERCB
     *   Register a callback for serial event notification. Argument:
     *   cdcacm_callback_t.  See cdcacm_callback_t type definition below.
     *   NOTE:  The callback will most likely invoked at the interrupt level.
     *   The called back function should, therefore, limit its operations to
     *   invoking some kind of IPC to handle the serial event in some normal
     *   task environment.
     */

    case CONSOLE_CMD_RD_BLOCK_SERIAL:
      {
        if (arg != 0)
          {
            g_readpend = true;
          }
        else
          {
            g_readpend = false;
            (void)LOS_EventWrite(&priv->read_event, USB_SERIAL_READ_EVENT);
          }
      }
      break;

    default:
      usb_err("Unknown CMD!\r\n");
      break;
    }

  return OK;
}

/****************************************************************************
 * Name: cdcacm_disconnect
 *
 * Description:
 *   Invoked after all transfers have been stopped, when the host is
 *   disconnected.  This function is probably called from the context of an
 *   interrupt handler.
 *
 ****************************************************************************/

static void cdcacm_disconnect(FAR struct usbdevclass_driver_s *driver,
                              FAR struct usbdev_s *dev)
{
  FAR struct cdcacm_dev_s *priv;

  (void)dev;
  if (driver == NULL)
    {
      usb_err("driver pointer is NULL!\r\n");
      return;
    }

  priv = ((FAR struct cdcacm_driver_s *)driver)->dev;
  if (priv == NULL)
    {
      usb_err("No device!\r\n");
      return;
    }

  if (priv->bdisconnect == false)
    {
      (void)cdcacm_serial_disconnect(priv);
      cdcacm_resetconfig(priv);
      priv->bdisconnect = true;
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

volatile unsigned int userial_mask_get(void)
{
  return g_usbSerialMask;
}

void userial_mask_set(unsigned int val)
{
  g_usbSerialMask = val;
}

/****************************************************************************
 * Name: cdcacm_classobject
 *
 * Description:
 *   Register USB serial port (and USB serial console if so configured) and
 *   return the class object.
 *
 * Input Parameters:
 *   devinfo - Describes the low level bindings of an usb device.
 *   classdev - The location to return the CDC serial class' device
 *     instance.
 *
 * Returned Value:
 *   A pointer to the allocated class object (NULL on failure).
 *
 ****************************************************************************/

int cdcacm_classobject(int minor,
                       FAR struct usbdev_devinfo_s *devinfo,
                       FAR struct usbdevclass_driver_s **classdev)
{
  FAR struct cdcacm_alloc_s *alloc;
  FAR struct cdcacm_dev_s *priv;
  FAR struct cdcacm_driver_s *drvr;

  (void)minor;

  /* Allocate the structures needed */

  alloc = (FAR struct cdcacm_alloc_s *)malloc(sizeof(struct cdcacm_alloc_s));
  if (alloc == NULL)
    {
      usb_err("%s: malloc fialed!\r\n", __FUNCTION__);
      return -ENOMEM;
    }

  /* Convenience pointers into the allocated blob */

  priv = &alloc->dev;
  drvr = &alloc->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct cdcacm_dev_s), 0, sizeof(struct cdcacm_dev_s));
  (void)memcpy_s(&priv->devinfo, sizeof(struct usbdev_devinfo_s), devinfo, sizeof(struct usbdev_devinfo_s));

  /* Fake line status */

  USETDW(priv->linecoding.dwDTERate, 9600);        /* Baud = 9600 */
  priv->linecoding.bCharFormat = UCDC_STOP_BIT_1;  /* Stop bits */
  priv->linecoding.bParityType = UCDC_PARITY_NONE; /* No parity */
  priv->linecoding.bDataBits   = 8;                /* 8 data bits */

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_driver_ops;
  drvr->dev        = priv;
  *classdev        = &drvr->drvr;

  return 0;
}

/****************************************************************************
 * Name: cdcacm_uninitialize
 *
 * Description:
 *   Un-initialize the USB storage class driver.  This function is used
 *   internally by the USB composite driver to uninitialize the CDC/ACM
 *   driver.  This same interface is available (with an untyped input
 *   parameter) when the CDC/ACM driver is used standalone.
 *
 * Input Parameters:
 *   There is one parameter, it differs in typing depending upon whether the
 *   CDC/ACM driver is an internal part of a composite device, or a standalone
 *   USB driver:
 *
 *     classdev - The class object returned by cdcacm_classobject()
 *     handle - The opaque handle representing the class object returned by
 *       a previous call to cdcacm_initialize().
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void cdcacm_uninitialize(FAR struct usbdevclass_driver_s *devclass_drvr)
{
  FAR struct cdcacm_driver_s *drvr = (struct cdcacm_driver_s *)devclass_drvr;
  FAR struct cdcacm_alloc_s *alloc;

  if (drvr == NULL)
    {
      usb_err("Invalid pointer!\r\n");
      return;
    }

  alloc = container_of(drvr, struct cdcacm_alloc_s, drvr);
  free(alloc);
}

/****************************************************************************
 * Name: cdcacm_get_composite_devdesc
 *
 * Description:
 *   Helper function to fill in some constants into the composite
 *   configuration struct.
 *
 * Input Parameters:
 *     dev - Pointer to the configuration struct we should fill
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void cdcacm_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = cdcacm_mkdevdesc;
  dev->mkconfdesc = cdcacm_mkcfgdesc;
  dev->mkstrdesc  = cdcacm_mkstrdesc;

  dev->nconfigs = CDCACM_NCONFIGS;  /* Number of configurations supported */
  dev->configid = CDCACM_CONFIGID;  /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = CDCACM_NINTERFACES;  /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = CDCACM_NSTRIDS;  /* Number of Strings */

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = CDCACM_NUM_EPS; /* Number of Endpoint */
}

/* Init composite public action */

void usbdev_cdcacm_initialize_sub(FAR struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the serial driver to fill in the constants we didn't
   * know here.
   */

  cdcacm_get_composite_devdesc(dev);

  /* Overwrite and correct some values... */

  dev->classobject  = cdcacm_classobject;
  dev->uninitialize = cdcacm_uninitialize;

  /* init ifnobase of usbdev_devinfo_s for judegment wIndex */

  dev->devinfo.ifnobase = ifnobase;  /* Offset to Interface-IDs */
  dev->minor            = minor;     /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;          /* Offset to String Numbers */
}

int usbdev_cdcacm_initialize (FAR struct module *mod, int n, FAR void *arg)
{
  FAR struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev;
  int ret;

  (void)mod;
  (void)n;
  if (com_s == NULL)
    {
      return -1;
    }

  usbdev_cdcacm_initialize_sub(&dev, 0, DEV_SERIAL);
  ret = composite_initialize(com_s, 1, &dev);
  if (ret)
    {
      return -1;
    }
  PRINTK(" ** Serial device initialized successfully! **\n ");

  return 0;
}
