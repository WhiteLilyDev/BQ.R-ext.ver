/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: LiteOS USB Driver RNDIS Protocol
 * Author: huangjieliang
 * Create: 2016-07-13
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */

#include <lwip/netifapi.h>
#include "gadget/f_ether.h"
#include "gadget/rndis.h"
#include "gadget/composite.h"
#include "implementation/global_implementation.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#include "controller/usb_device/dwc_otg_pcd_intr.h"
#include "net/usb_eth_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

const struct netif *g_ether_usb0;

/* device driver structure definition */

static const driver_t g_fethernet_driver_t =
{
  .name     = "fethernet",
  .methods  = NULL,
  .size     = 0
};

/* private device class information */

static devclass_t g_fethernet_devclass;
DRIVER_MODULE(fethernet, simple, g_fethernet_driver_t, g_fethernet_devclass, usbdev_rndis_initialize, 0);

#define DEIVICE_VENDOR_ID    0x0525
#define DEIVICE_PRODUCT_ID   0xa4a2

#define ETHER_NCONFIGS       1
#define ETHER_CONFIGID       0
#define ETHER_NINTERFACES    2
#define ETHER_NSTRIDS        5
#define ETHER_NUM_EPS        3

#define RNDIS_STRING_ID_LEN      4
#define RNDIS_STRING_PID_LEN     44
#define RNDIS_VERSION_STRING_LEN 62
#define RNDIS_STRING_LEN         58
#define RNDIS_TYPE_LEN           40

static const char g_rndis_string_id[RNDIS_STRING_ID_LEN] =
{
  RNDIS_STRING_ID_LEN, UDESC_STRING, 0x09, 0x04
};

static const char g_rndis_string_pid[RNDIS_STRING_PID_LEN] =
{
  RNDIS_STRING_PID_LEN, UDESC_STRING, 'R', 0, 'N', 0, 'D', 0, 'I',
  0, 'S', 0, '/', 0, 'E', 0, 't', 0, 'h', 0, 'e', 0, 'r', 0, 'n',
  0, 'e', 0, 't', 0, ' ', 0, 'G', 0, 'a', 0, 'd', 0, 'g', 0, 'e',
  0, 't', 0
};

static const char g_version_string[RNDIS_VERSION_STRING_LEN] =
{
  RNDIS_VERSION_STRING_LEN, UDESC_STRING, 0x4c, 0x00, 0x69, 0x00,
  0x6e, 0x00, 0x75, 0x00, 0x78, 0x00, 0x20, 0x00, 0x33, 0x00, 0x2e,
  0x00, 0x31, 0x00, 0x38, 0x00, 0x2e, 0x00, 0x32, 0x00, 0x30, 0x00,
  0x20, 0x00, 0x77, 0x00, 0x69, 0x00, 0x74, 0x00, 0x68, 0x00, 0x20,
  0x00, 0x64, 0x00, 0x77, 0x00, 0x63, 0x00, 0x5f, 0x00, 0x6f, 0x00,
  0x74, 0x00, 0x67, 0x00, 0x5f, 0x00, 0x70, 0x00, 0x63, 0x00, 0x64,
  0x00
};

static const char g_rndis_string[RNDIS_STRING_LEN] =
{
  RNDIS_STRING_LEN, UDESC_STRING, 0x52, 0x00, 0x4e, 0x00, 0x44, 0x00,
  0x49, 0x00, 0x53, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6f, 0x00, 0x6d,
  0x00, 0x6d, 0x00, 0x75, 0x00, 0x6e, 0x00, 0x69, 0x00, 0x63, 0x00,
  0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x73,
  0x00, 0x20, 0x00, 0x43, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x74, 0x00,
  0x72, 0x00, 0x6f, 0x00, 0x6c, 0x00
};

static const char g_rndis_type_string[RNDIS_TYPE_LEN] =
{
  RNDIS_TYPE_LEN, UDESC_STRING, 0x52, 0x00, 0x4e, 0x00, 0x44, 0x00,
  0x49, 0x00, 0x53, 0x00, 0x20, 0x00, 0x45, 0x00, 0x74, 0x00, 0x68,
  0x00, 0x65, 0x00, 0x72, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x74, 0x00,
  0x20, 0x00, 0x44, 0x00, 0x61, 0x00, 0x74, 0x00, 0x61, 0x00
};

static const struct usb_device_descriptor g_fether_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,
  HSETW(.bcdUSB,      UD_BCD_USB),         /* USB version */
  .bDeviceClass       = UDCLASS_COMM,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize     = UD_USB_MPS,
  HSETW(.idVendor,    DEIVICE_VENDOR_ID),  /* vendor */
  HSETW(.idProduct,   DEIVICE_PRODUCT_ID), /* product */
  HSETW(.bcdDevice,   0x0318),             /* device version */
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,
};

static struct usbd_string g_fether_device_strings[] =
{
  {0, g_rndis_string_id},
  {1, g_version_string},
  {2, g_rndis_string_pid},
  {4, g_rndis_string},
  {5, g_rndis_type_string},
  USBD_DEVICE_STRINGS_END
};


static inline int is_multicast_eth_addr(const uint8_t *addr)
{
  return (0x01 & addr[0]);
}

uint8_t eth_set_hwaddr(struct netif *usb_netif, uint8_t *addr, uint8_t len)
{
  errno_t err;

  if (is_multicast_eth_addr(addr))
    {
      usb_err("config a muticast mac address, please check!\n");
      return 1;
    }

  if (len != NETIF_MAX_HWADDR_LEN)
    {
      usb_err("config wrong mac address len=%u\n", len);
      return 1;
    }

  err = memcpy_s(usb_netif->hwaddr, NETIF_MAX_HWADDR_LEN, addr, len);
  if (err != EOK)
    {
      usb_err("memcpy_s fail, err:%d\n", err);
      return 1;
    }

  return 0;
}

void eth_random_addr(uint8_t *addr)
{
  uint32_t rand_val;
  uint32_t nowclocks;

  msleep(200);
  nowclocks = hi_sched_clock() & 0xffffffff;  /* Get low 32-bit value */
  srand(nowclocks);
  rand_val = (uint32_t)rand();
  addr[0]  = rand_val & 0xff;
  addr[1]  = (rand_val >> 8) & 0xff;
  addr[2]  = (rand_val >> 16) & 0xff;
  addr[3]  = (rand_val >> 24) & 0xff;

  msleep(200);
  nowclocks = hi_sched_clock() & 0xffffffff;  /* Get low 32-bit value */
  srand(nowclocks);
  rand_val = (uint32_t)rand();
  addr[4]  = rand_val & 0xff;
  addr[5]  = (rand_val >> 8) & 0xff;
  addr[0] &= 0xfe;  /* clear multicast bit */
  addr[0] |= 0x02;  /* set local assignment bit (IEEE802) */
}

static void eth_tx(struct netif *usb_netif, struct pbuf *p)
{
  struct los_eth_driver *sc = (struct los_eth_driver *)usb_netif->state;
  struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
  struct pbuf *q;
  int sg_len = 0;

  for (q = p; q != NULL; q = q->next)
    {
      sg_list[sg_len].buf = (UINTPTR)q->payload;
      sg_list[sg_len++].len = q->len;
    }

  rndis_tx(sc, sg_list, sg_len, p->tot_len, (UINTPTR)p);
}

void eth_rx(struct los_eth_driver *sc, const struct usbdev_req_s *req)
{
  struct netif *usb_netif = &sc->ac_if;
  uint32_t total_len      = req->xfrd - RNDIS_PACKET_HDR_SIZE;
  struct pbuf *p;

  if (total_len > MAX_ETH_MSG)
    {
      total_len = MAX_ETH_MSG;
    }

  p = pbuf_alloc(PBUF_RAW, (uint16_t)(total_len + ETH_PAD_SIZE), PBUF_RAM);
  if (p == NULL)
    {
      usb_err("eth_drv_recv : pbuf_alloc failed\n");
      return;
    }

#if ETH_PAD_SIZE
  (void)pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

  (void)memcpy_s(p->payload, total_len, &req->buf[RNDIS_PACKET_HDR_SIZE], total_len);

#if ETH_PAD_SIZE
  (void)pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

  driverif_input(usb_netif, p);
}

#if PF_PKT_SUPPORT
static void eth_drv_config(struct netif *usb_netif, uint32_t cflags, uint8_t set_bit)
{
  LWIP_UNUSED_ARG(usb_netif);
  LWIP_UNUSED_ARG(cflags);
  LWIP_UNUSED_ARG(set_bit);

  /* Nothing to be done for now */

}
#endif

void netdev_register(struct los_eth_driver *sc)
{
  struct netif *usb_netif = &sc->ac_if;
  ip4_addr_t ipaddr, netmask, gw;
  uint8_t dev_addr[NETIF_MAX_HWADDR_LEN];
  err_t ret;

  IP4_ADDR(&gw, 10, 67, 233, 1);
  IP4_ADDR(&ipaddr, 10, 67, 233, 3);
  IP4_ADDR(&netmask, 255, 255, 255, 0);

  usb_netif->state           = sc;
  usb_netif->drv_send        = eth_tx;
  usb_netif->drv_set_hwaddr  = eth_set_hwaddr;
  usb_netif->link_layer_type = ETHERNET_DRIVER_IF;
  usb_netif->hwaddr_len      = NETIF_MAX_HWADDR_LEN;

#if PF_PKT_SUPPORT
  usb_netif->drv_config = eth_drv_config;
#endif

#if LWIP_NETIF_ETHTOOL
  usb_netif->ethtool_ops = NULL;
#endif

  eth_random_addr(dev_addr);
  (void)memcpy_s(usb_netif->hwaddr, NETIF_MAX_HWADDR_LEN, dev_addr, NETIF_MAX_HWADDR_LEN);

  ret = netifapi_netif_add(usb_netif, &ipaddr, &netmask, &gw);
  if (ret)
    {
      usb_err("%s %d, add netif failed, ret:%d\n", __FUNCTION__, __LINE__, ret);
      return;
    }

  (void)netifapi_netif_set_link_down(usb_netif);

  g_ether_usb0 = usb_netif;
}

void netdev_unregister(struct los_eth_driver *sc)
{
  struct netif *usb_netif = &sc->ac_if;

  if (netifapi_netif_remove(usb_netif) != ERR_OK)
    {
      usb_err("%s %d, remove netif failed!\n", __FUNCTION__, __LINE__);
    }
}

void rndis_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fether_device_desc, sizeof(g_fether_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int16_t rndis_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  int16_t total_len = 0;
  int16_t len       = USB_CONFIG_DESC_SIZE;
  errno_t ret;

  (void)devinfo;

  /* Copy RNDIS device configure descriptor. */

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, g_rndis_hs_desc.rndis_config, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  /* Copy RNDIS device configure descriptor. */

  buf += USB_CONFIG_DESC_SIZE;
  len  = sizeof(g_rndis_hs_func_desc);
  ret  = memcpy_s(buf, USB_COMP_EP0_BUFSIZ - (uint16_t)total_len, g_rndis_hs_desc.rndis_func, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  return total_len;
}

int rndis_mkstrdesc(uint8_t id, uint8_t *buf)
{
  const char *str;
  errno_t ret;
  int i;

  for (i = 0; g_fether_device_strings[i].s != NULL; i++)
    {
      str = g_fether_device_strings[i].s;
      if (g_fether_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

void rndis_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = rndis_mkdevdesc;
  dev->mkconfdesc = rndis_mkcfgdesc;
  dev->mkstrdesc  = rndis_mkstrdesc;
  dev->nconfigs   = ETHER_NCONFIGS; /* Number of configurations supported */
  dev->configid   = ETHER_CONFIGID; /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = ETHER_NINTERFACES; /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = ETHER_NSTRIDS;  /* Number of Strings */

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = ETHER_NUM_EPS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */