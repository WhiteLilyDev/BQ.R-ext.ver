/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: LiteOS USB Driver Virtual Serial Port Descriptor
 * Author: huangjieliang
 * Create: 2016-07-06
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

#include "gadget/cdcacm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static const struct usb_device_descriptor g_cdcacm_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,
  HSETW(.bcdUSB,      UD_BCD_USB),    /* usb version */
  .bDeviceClass       = UDCLASS_COMM, /* 0x02 */
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize     = UD_USB_MPS,
  HSETW(.idVendor, DEVICE_VENDOR_ID),   /* vendor 0x0525 */
  HSETW(.idProduct, DEVICE_PRODUCT_ID), /* product 0xa4a7 */
  HSETW(.bcdDevice, DEVICE_VERSION),    /* device version */
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,
};

struct cdcacm_hs_function_descriptor g_cdcacm_hs_func_desc =
{
  {
    .bLength            = sizeof(struct usb_interface_assoc_descriptor),
    .bDescriptorType    = UDESC_IFACE_ASSOC,
    .bInterfaceCount    = 2,
    .bFunctionClass     = 2, /* UFCLASS_CDC */
    .bFunctionSubClass  = 2,
    .bFunctionProtocol  = 1,

    /* Dynamic configuration when composite driver. 0x0c */

    .iFunction = 7,
  },
  {
    .bLength            = sizeof(struct usb_interface_descriptor),
    .bDescriptorType    = UDESC_INTERFACE,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 0x1,
    .bInterfaceClass    = UICLASS_CDC,
    .bInterfaceSubClass = UISUBCLASS_ABSTRACT_CONTROL_MODEL,
    .bInterfaceProtocol = UIPROTO_CDC_AT,

    /* Dynamic configuration when composite driver. 0x0a */

    .iInterface             = 5
  },
  {
    .bLength            = sizeof(struct usb_cdc_header_descriptor),
    .bDescriptorType    = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_HEADER,
    HSETW(.bcdCDC, 0x0110)
  },
  {
    .bLength            = sizeof(struct usb_cdc_cm_descriptor),
    .bDescriptorType    = UDESC_CS_INTERFACE,
    .bDescriptorSubtype = UDESCSUB_CDC_CM, /* call Management */
    .bmCapabilities     = 0x0,
  },
  {
    .bLength            = sizeof(struct usb_cdc_acm_descriptor),
    .bDescriptorType    = UDESC_CS_INTERFACE,
    .bDescriptorSubType = UDESCSUB_CDC_ACM, /* Abstract Control Model */
    .bmCapabilities     = USB_CDC_ACM_HAS_LINE
  },
  {
    .bLength            = sizeof(struct usb_cdc_union_desc),
    .bDescriptorType    = UDESC_CS_INTERFACE,
    .bDescriptorSubType = UDESCSUB_CDC_UNION,
  },
  {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes     = UE_INTERRUPT,
    HSETW(.wMaxPacketSize, 0x000a),
    .bInterval        = 9
  },
#ifdef DWC3_USB_SERIAL
  {
    .bLength           = 6,
    .bDescriptorType   = 0x30,
    .bMaxBurst         = 0,
    .bmAttributes      = 0,
    .wBytesPerInterval = {0}
  },
#endif
  {
    .bLength            = sizeof(struct usb_interface_descriptor),
    .bDescriptorType    = UDESC_INTERFACE,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 0x2,
    .bInterfaceClass    = UICLASS_CDC_DATA,
    .bInterfaceSubClass = UISUBCLASS_DATA,
    .bInterfaceProtocol = 0x0,

    /* Dynamic configuration when composite driver. 0x0b */

    .iInterface = 6
  },
  {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_IN,
    .bmAttributes     = UE_BULK,
    HSETW(.wMaxPacketSize, MAX_PACKET_SIZE),
    .bInterval        = 0
  },
#ifdef DWC3_USB_SERIAL
  {
    .bLength           = 6,
    .bDescriptorType   = 0x30,
    .bMaxBurst         = 0,
    .bmAttributes      = 0,
    .wBytesPerInterval = {0}
  },
#endif
  {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = UDESC_ENDPOINT,
    .bEndpointAddress = UE_DIR_OUT,
    .bmAttributes     = UE_BULK,
    HSETW(.wMaxPacketSize, MAX_PACKET_SIZE),
    .bInterval        = 0
  },
#ifdef DWC3_USB_SERIAL
  {
    .bLength           = 6,
    .bDescriptorType   = 0x30,
    .bMaxBurst         = 0,
    .bmAttributes      = 0,
    .wBytesPerInterval = {0}
  },
#endif
};

/* config contain func */

static const struct usb_config_descriptor g_cdcacm_hs_config_desc =
{
  .bLength             = USB_CONFIG_DESC_SIZE,
  .bDescriptorType     = UDESC_CONFIG,
  HSETW(.wTotalLength, USB_CONFIG_DESC_SIZE + sizeof(g_cdcacm_hs_func_desc)),
  .bNumInterface       = 2,
  .bConfigurationValue = 2,
  .iConfiguration      = 4,
  .bmAttributes        = UC_SELF_POWERED | UC_BUS_POWERED, /* 0xc0 */
  .bMaxPower           = 1                                 /* max power */
};

#define DT_STRING_ID_LEN 4
static const char g_dt_string_id[DT_STRING_ID_LEN] =
{
  DT_STRING_ID_LEN,
  UDESC_STRING,
  0x09, 0x04
};

#ifdef DWC3_USB_SERIAL
#define DT_STRING_VID_LEN 94
#else
#define DT_STRING_VID_LEN 16
#endif
static const char g_dt_string_vid[DT_STRING_VID_LEN] =
{
  DT_STRING_VID_LEN,
  UDESC_STRING,
#ifndef DWC3_USB_SERIAL
  'D',0,'W',0,'C',0,'-',0,'O',0,'T',0,'G',0
#else
  0x4c, 0, 0x69, 0, 0x6e, 0, 0x75, 0, 0x78, 0, 0x20, 0, 0x33, 0, 0x2e, 0, 0x31, 0, 0x38, 0,
  0x2e, 0, 0x31, 0, 0x33, 0, 0x2d, 0, 0x67, 0, 0x64, 0, 0x37, 0, 0x32, 0, 0x32, 0, 0x36, 0,
  0x65, 0, 0x37, 0, 0x2d, 0, 0x64, 0, 0x69, 0, 0x72, 0, 0x74, 0, 0x79, 0, 0x20, 0, 0x77, 0,
  0x69, 0, 0x74, 0, 0x68, 0, 0x20, 0, 0x64, 0, 0x77, 0, 0x63, 0, 0x5f, 0, 0x75, 0, 0x73, 0,
  0x62, 0, 0x33, 0, 0x5f, 0, 0x70, 0, 0x63, 0, 0x64, 0
#endif
};

#define DT_STRING_PID_LEN 38
static const char g_dt_string_pid[DT_STRING_PID_LEN] =
{
  DT_STRING_PID_LEN,
  UDESC_STRING,
  'G',0,'a',0,'d',0,'g',0,'e',0,'t',0,' ',0,
  'S',0,'e',0,'r',0,'i',0,'a',0,'l',0,' ',0,
  'v',0,'2',0,'.',0,'4',0
};

#define DT_STRING_SERIAL_LEN    18
static const char g_dt_string_serial[DT_STRING_SERIAL_LEN] =
{
  DT_STRING_SERIAL_LEN,
  UDESC_STRING,
  '2',0,'0',0,'1',0,'5',0,'0',0,'7',0,'3',0,'0',0
};

#define DT_STRING_4_LEN 30
static const char g_dt_string_4[DT_STRING_4_LEN] =
{
  DT_STRING_4_LEN,
  UDESC_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x41, 0, 0x43, 0, 0x4d, 0,
  0x20, 0, 0x63, 0, 0x6f, 0, 0x6e, 0, 0x66, 0, 0x69, 0, 0x67, 0
};

#define DT_STRING_5_LEN 66
static const char g_dt_string_5[DT_STRING_5_LEN] =
{
  DT_STRING_5_LEN,
  UDESC_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x41, 0, 0x62, 0, 0x73, 0, 0x74, 0, 0x72, 0, 0x61, 0,
  0x63, 0, 0x74, 0, 0x20, 0, 0x43, 0, 0x6f, 0, 0x6e, 0, 0x74, 0, 0x72, 0, 0x6f, 0, 0x6c, 0,
  0x20, 0, 0x4d, 0, 0x6f, 0, 0x64, 0, 0x65, 0, 0x6c, 0, 0x20, 0, 0x28, 0, 0x41, 0, 0x43, 0,
  0x4d, 0, 0x29, 0
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

#define FSERIAL_DSTR_IDX 9
static struct usbd_string g_cdcacm_device_strings[FSERIAL_DSTR_IDX] =
{
  { 0, g_dt_string_id },
  { 1, g_dt_string_vid },
  { 2, g_dt_string_pid },
  { 3, g_dt_string_serial },
  { 4, g_dt_string_4 },
  { 5, g_dt_string_5 },
  { 6, g_dt_string_6 },
  { 7, g_dt_string_7 },
  USBD_DEVICE_STRINGS_END
};

/* ***************************************************************************
 * Name: cdcacm_mkdevdesc
 *
 * Description:
 *   Construct the device descriptor
 *
 * ***************************************************************************
 */

void cdcacm_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_cdcacm_device_desc, sizeof(g_cdcacm_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

/* ***************************************************************************
 * Name: cdcacm_mkcfgdesc
 *
 * Description:
 *   Construct the configuration descriptor
 *
 * ***************************************************************************
 */

int16_t cdcacm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  int16_t total_len = 0;
  int16_t len = USB_CONFIG_DESC_SIZE;
  errno_t ret;

  g_cdcacm_hs_func_desc.ifcad.bFirstInterface = (uint8_t)devinfo->ifnobase;
  g_cdcacm_hs_func_desc.ifcd.bInterfaceNumber = (uint8_t)devinfo->ifnobase;
  g_cdcacm_hs_func_desc.cdc_union_desc.bMasterInterface0 = (uint8_t)devinfo->ifnobase;
  g_cdcacm_hs_func_desc.ifdd.bInterfaceNumber = (uint8_t)devinfo->ifnobase + 1;
  g_cdcacm_hs_func_desc.cdc_union_desc.bSlaveInterface0 = (uint8_t)devinfo->ifnobase + 1;
  g_cdcacm_hs_func_desc.cdc_call_desc.bDataInterface = (uint8_t)devinfo->ifnobase + 1;

  /* Copy Serial device configure descriptor. */

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (const void *)&g_cdcacm_hs_config_desc, (uint32_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  /* Copy Serial device function descriptor. */

  buf += USB_CONFIG_DESC_SIZE;
  len = sizeof(g_cdcacm_hs_func_desc);
  ret = memcpy_s(buf, (USB_COMP_EP0_BUFSIZ - total_len), (const void *)&g_cdcacm_hs_func_desc, (uint32_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  return total_len;
}

/****************************************************************************
 * Name: cdcacm_mkstrdesc
 *
 * Description:
 *   Construct a string descriptor
 *
 ****************************************************************************/

int cdcacm_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_cdcacm_device_strings[i].s != NULL; i++)
    {
      str = g_cdcacm_device_strings[i].s;
      if (g_cdcacm_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (const void *)str, (uint32_t)str[0]);
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

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */