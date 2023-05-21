/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Composite Devices
 * Author: huangjieliang
 * Create: 2017-04-21
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

#include "gadget/composite.h"
#include "gadget/cdcacm.h"
#include "gadget/rndis.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define FMULTI_DEBUG
#ifdef FMULTI_DEBUG
#define FMSLOG(x...) dprintf(x)
#else
#define FMSLOG(x...) do {} while(0)
#endif

#define USB_ETHER_FIRST_INTERFACE_NUM 0
#define USB_SERIAL_FIRST_INTERFACE_NUM 2

int usbdev_multi_initialize(struct module *mod, int n, void *arg);

/* device driver structure definition */

static driver_t g_fmulti_driver_t =
{
  .name    = "fmulti",
  .methods = NULL,
  .size    = 0
};

/* private device class information */

static devclass_t g_fmulti_devclass;
DRIVER_MODULE(fmulti, simple, g_fmulti_driver_t, g_fmulti_devclass, usbdev_multi_initialize, 0);

#define MULTI_VENDOR_ID     0x1d6b
#define MULTI_PRODUCT_ID    0x0104

/* This struct is aimed at enumerating index for composite device strings. */

enum
{
  /* Rndis Device */

  MULTI_RNDIS_CTRL_IDX = 6,
  MULTI_RNDIS_DATA_IDX,
  MULTI_RNDIS_IAD_IDX,

  /* ACM Device */

  MULTI_ACM_CTRL_IDX = 0xa,
  MULTI_ACM_DATA_IDX,
  MULTI_ACM_IAD_IDX
};

/* Define and initialize multi device descriptor. */

static const struct usb_device_descriptor g_fmulti_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,
  HSETW(.bcdUSB, UD_BCD_USB), /* USB version */
  .bDeviceClass       = UDCLASS_MISC,
  .bDeviceSubClass    = 2,
  .bDeviceProtocol    = 1,
  .bMaxPacketSize     = UD_USB_MPS,
  HSETW(.idVendor,    MULTI_VENDOR_ID),  /* vendor */
  HSETW(.idProduct,   MULTI_PRODUCT_ID), /* product */
  HSETW(.bcdDevice, 0x0318),             /* device version */
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,
};

#define MULTI_STRING_ID_LEN  4
static const char g_multi_string_id[MULTI_STRING_ID_LEN] =
{
  MULTI_STRING_ID_LEN,
  UDESC_STRING,
  0x09,
  0x04,
};

#define MULTI_VERSION_STRING_LEN 62
static const char g_multi_version_string[MULTI_VERSION_STRING_LEN] =
{
  MULTI_VERSION_STRING_LEN, UDESC_STRING,
  'M', 0, 'u', 0, 'l', 0, 't', 0, 'i', 0, 'f', 0, 'u', 0, 'n', 0, 'c', 0, 't', 0,
  'i', 0, 'o', 0, 'n', 0, ' ', 0, 'C', 0, 'o', 0, 'm', 0, 'p', 0, 'o', 0, 's', 0,
  'i', 0, 't', 0, 'e', 0, ' ', 0, 'G', 0, 'a', 0, 'd', 0, 'g', 0, 'e', 0, 't', 0
};

#define MULTI_STRING_4_LEN 38
static const char g_dt_string_4[MULTI_STRING_4_LEN] =
{
  MULTI_STRING_4_LEN,
  UDESC_STRING,
  0x47, 0, 0x61, 0, 0x64, 0, 0x67, 0, 0x65, 0, 0x74, 0, 0x20, 0,
  0x53, 0, 0x65, 0, 0x72, 0, 0x69, 0, 0x61, 0, 0x6c, 0, 0x20, 0,
  0x76, 0, 0x32, 0, 0x2e, 0, 0x34, 0
};

#define DT_STRING_6_LEN 26
static const char g_dt_string_6[DT_STRING_6_LEN] =
{
  DT_STRING_6_LEN,
  UDESC_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x41, 0,
  0x43, 0, 0x4d, 0, 0x20, 0, 0x44, 0, 0x61, 0, 0x74, 0, 0x61, 0
};

#define DT_STRING_7_LEN 22
static const char g_dt_string_7[DT_STRING_7_LEN] =
{
  DT_STRING_7_LEN,
  UDESC_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x53, 0, 0x65, 0,
  0x72, 0, 0x69, 0, 0x61, 0, 0x6c, 0
};

#define MULTI_RNDIS_STRING_LEN 12
static const char g_multi_rndis_string[MULTI_RNDIS_STRING_LEN] =
{
  MULTI_RNDIS_STRING_LEN,
  UDESC_STRING,
  'R',0,'N',0,'D',0,'I',0,'S',0
};

#define MULTI_SERIAL_STRING_LEN 22
static const char g_multi_serial_string[MULTI_SERIAL_STRING_LEN] =
{
  MULTI_SERIAL_STRING_LEN,
  UDESC_STRING,
  'C',0,'D',0,'C',0,' ',0,'S',0,'e',0,'i',0,'a',0,'l',0
};

#define FMULTI_DSTR_IDX 8
struct usbd_string g_fmulti_device_strings[FMULTI_DSTR_IDX] =
{
  { 0,                   g_multi_string_id },
  { 2,                   g_multi_version_string },
  { 4,                   g_dt_string_4 },
  { 6,                   g_dt_string_6 },
  { 7,                   g_dt_string_7 },
  { MULTI_RNDIS_IAD_IDX, g_multi_rndis_string },
  { MULTI_ACM_IAD_IDX,   g_multi_serial_string },
  USBD_DEVICE_STRINGS_END
};

void multi_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmulti_device_desc, sizeof(g_fmulti_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int multi_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fmulti_device_strings[i].s != NULL; i++)
    {
      str = g_fmulti_device_strings[i].s;
      if (g_fmulti_device_strings[i].id == id)
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

void multi_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  dev->mkdevdesc = multi_mkdevdesc;
  dev->mkstrdesc = multi_mkstrdesc;
}

int usbdev_multi_initialize (struct module *mod, int n, void *arg)
{
  struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev[USB_COMPOSITE_DEV_NUM];
  int ret;

  (void)mod;
  (void)n;
  if (com_s == NULL)
    {
      PRINTK("com_s is NULL!\n");
      return -1;
    }

  usbdev_rndis_initialize_sub(&dev[0], USB_ETHER_FIRST_INTERFACE_NUM, DEV_ETHERNET);
  multi_get_composite_devdesc(&dev[0]);
  usbdev_cdcacm_initialize_sub(&dev[1], USB_SERIAL_FIRST_INTERFACE_NUM, DEV_SERIAL);
  multi_get_composite_devdesc(&dev[1]);

  ret = composite_initialize(com_s, USB_COMPOSITE_DEV_NUM, dev);
  if (ret < 0)
    {
      PRINTK("  ** multi device initialized failed! **\n");
      return -1;
    }

  PRINTK("  ** multi device initialized successfully! **\n");
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */