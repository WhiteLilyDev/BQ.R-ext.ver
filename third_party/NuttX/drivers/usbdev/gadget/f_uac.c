/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver UAC Protocol
 * Author: huangjieliang
 * Create: 2017-12-12
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

#include "gadget/f_uac.h"
#include "gadget/usbd_audio.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#include "implementation/global_implementation.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int usbdev_uac_initialize(struct module *mod, int n, void *arg);

/* device driver structure definition */

static const driver_t g_fuac_driver =
{
  .name    = "fuac",
  .methods = NULL,
  .size    = sizeof(struct uac_softc)
};

/* private device class information */

static devclass_t g_fuac_devclass;
DRIVER_MODULE(fuac, simple, g_fuac_driver, g_fuac_devclass, usbdev_uac_initialize, 0);

static int usbclass_uac_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uac_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uac_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_uac_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_uac_driverops =
{
  usbclass_uac_bind,
  usbclass_uac_unbind,
  usbclass_uac_setup,
  usbclass_uac_disconnect,
  NULL,
  NULL
};

static const char g_fuac_str_lang[] =
{
  4, UDESC_STRING,
  0x09, 0x04
};

#define STR_AC_IF 4
static const char g_fuac_str_ac_if[] =
{
  38, UDESC_STRING,
  'H', 0, 'i', 0, 's', 0, 'i', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0,
  ' ', 0, 'H', 0, 'D', 0, ' ', 0, 'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0
};

static const char g_fuac_str_manufacturer[] =
{
  14, UDESC_STRING,
  'H', 0, 'U', 0, 'A', 0, 'W', 0, 'E', 0, 'I', 0
};

#define UAC_STR_IDX_PRODUCT  2
static const char g_fuac_str_product[] =
{
  48, UDESC_STRING,
  'L', 0, 'i', 0, 't', 0, 'e', 0, 'O', 0, 'S', 0, ' ', 0, 'U', 0, 'S', 0,
  'B', 0, ' ', 0, 'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0, ' ', 0, 'G', 0,
  'a', 0, 'd', 0, 'g', 0, 'e', 0, 't', 0
};

#define UAC_IN_TERMINAL  8
static const char g_fuac_input_terminal[] =
{
  46, UDESC_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'I', 0,
  'n', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0, 'm', 0,
  'i', 0, 'n', 0, 'a', 0, 'l', 0
};

static const char g_fuac_output_terminal[] =
{
  48, UDESC_STRING,
  'C', 0, 'a', 0, 'p', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'O', 0,
  'u', 0, 't', 0, 'p', 0, 'u', 0, 't', 0, ' ', 0, 't', 0, 'e', 0, 'r', 0,
  'm', 0, 'i', 0, 'n', 0, 'a', 0, 'l', 0
};

#define ISO_DATA_SIZE   200
size_t g_uac_iso_data_size = 16;

#define UAC_DEF_CCHMASK 0x3
#define UAC_DEF_CSRATE  48000
#define UAC_DEF_CSSIZE  2
#define UAC_DEF_PCHMASK 0x3 /* Playback support dual channel. */
#define UAC_DEF_PSRATE  8000
#define UAC_DEF_PSSIZE  2

struct uac_opts g_uac_opts =
{
  UAC_DEF_CCHMASK, UAC_DEF_CSRATE, UAC_DEF_CSSIZE,
  UAC_DEF_PCHMASK, UAC_DEF_PSRATE, UAC_DEF_PSSIZE
};

static uint32_t fuac_channel_num_get(uint32_t channel_mask)
{
  uint32_t num = 0;

  while (channel_mask)
    {
      if (channel_mask & 1)
        {
          num++;
        }
      channel_mask >>= 1;
    }

  return num;
}

int fuac_opts_set(struct uac_opts *opts)
{
  if (opts == NULL)
    {
      usb_err("opts is NULL\n");
      return -1;
    }

  (void)memcpy_s(&g_uac_opts, sizeof(g_uac_opts), opts, sizeof(struct uac_opts));

  return 0;
}

static size_t g_uac_rate = 0;
uint32_t fuac_rate_get(void)
{
  if (g_uac_rate != 0)
    {
      return g_uac_rate;
    }

  return 0;
}

static const struct usb_device_descriptor g_fuac_device_desc __attribute__((aligned(4))) =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE, /* Constant for device descriptor */
  HSETW(.bcdUSB, UD_BCD_USB),         /* USB version required: 2.0 */
  .bDeviceClass       = UICLASS_IAD,  /* Miscellaneous Device Class */
  .bDeviceSubClass    = 0x2,          /* Common Class */
  .bDeviceProtocol    = 0x1,          /* Interface Association Descriptor */
  .bMaxPacketSize     = 64,           /* Control Endpoint packet size */
  HSETW(.idVendor,    0x1d6b),        /* Vendor ID of Huawei Technologies */
  HSETW(.idProduct,   0x0101),        /* Product ID, webcamera ? */
  HSETW(.bcdDevice,   0x318),         /* Device release code */
  .iManufacturer      = 1,            /* Manufacturer name, string index */
  .iProduct           = 2,            /* Product name, string index */
  .iSerialNumber      = 0,            /* Not Used */
  .bNumConfigurations = 1             /* One Configuration */
};

static struct usb_config_descriptor g_fuac_config_desc =
{
  .bLength              = sizeof(struct usb_config_descriptor),
  .bDescriptorType      = UDESC_CONFIG,
  HSETW(.wTotalLength, 0x006c),   /* Size of all descriptors, set later */
  .bNumInterface        = 0x2,    /* Two Interfaces */
  .bConfigurationValue  = 0x1,    /* ID of this configuration */
  .iConfiguration       = 0x0,    /* Index of string descriptor */
  .bmAttributes         = 0xc0,   /* Self-powered */
  .bMaxPower            = 0x1     /* Maximum power consumption from the bus */
};

static struct usb_interface_assoc_descriptor g_fuac_iad =
{
  .bLength              = sizeof(struct usb_interface_assoc_descriptor),
  .bDescriptorType      = UDESC_IFACE_ASSOC,
  .bFirstInterface      = 0,   /* Interface number of VideoControl interface */
  .bInterfaceCount      = 2,   /* Number of contiguous Video interfaces */
  .bFunctionClass       = UICLASS_AUDIO,
  .bFunctionSubClass    = 0,
  .bFunctionProtocol    = 0,
  .iFunction            = STR_AC_IF  /* index of string descriptor */
};

static struct usb_interface_descriptor g_fuac_ac_intf_desc =
{
  .bLength              = sizeof(struct usb_interface_descriptor),
  .bDescriptorType      = UDESC_INTERFACE,
  .bInterfaceNumber     = 0,   /* index number of this interface */
  .bAlternateSetting    = 0,   /* index of this settings */
  .bNumEndpoints        = 0,   /* one endpoint */
  .bInterfaceClass      = UICLASS_AUDIO,
  .bInterfaceSubClass   = 1,
  .bInterfaceProtocol   = 0,
  .iInterface           = STR_AC_IF   /* index of string descriptor */
};

static struct uac_ac_header_descriptor g_fuac_ac_head_desc =
{
  .bLength              = sizeof(struct uac_ac_header_descriptor),
  .bDescriptorType      = UAC_CS_INTERFACE,
  .bDescriptorSubtype   = UAC_HEADER,
  .bcdADC               = 0x0100,  /* UAC specification version, 1.0 */
  .wTotalLength         = 0x1e,    /* total length, currently not set */
  .bInCollection        = 1,       /* Number of streaming interfaces */
  .baInterfaceNr        = {0x01}   /* Associated Audio Streaming Interface */
};

static const struct uac1_input_terminal_descriptor g_fuac_input_terminal_desc =
{
  .bLength              = sizeof(struct uac1_input_terminal_descriptor),
  .bDescriptorType      = UAC_CS_INTERFACE,
  .bDescriptorSubtype   = UAC_INPUT_TERMINAL,
  .bTerminalID          = 0x03,
  .wTerminalType        = UAC_INPUT_TERMINAL_MICROPHONE,
  .bAssocTerminal       = 0,
  .bNrChannels          = 0x1,
  .wChannelConfig       = 0x0001,
  .iChannelNames        = 0x10,
  .iTerminal            = 0x8,
};

static const struct uac_output_terminal_descriptor g_fuac_output_terminal_desc =
{
  .bLength                = sizeof(struct uac_output_terminal_descriptor),
  .bDescriptorType        = UAC_CS_INTERFACE,
  .bDescriptorSubtype     = UAC_OUTPUT_TERMINAL,
  .bTerminalID            = 4,
  .wTerminalType          = 0x101,
  .bAssocTerminal         = 0,
  .bSourceID              = 3,
  .iTerminal              = 0xb,
};

static struct usb_interface_descriptor g_fuac_as_interface_alt_0_desc =
{
  .bLength               = sizeof(struct usb_interface_descriptor),
  .bDescriptorType       = UDESC_INTERFACE,
  .bInterfaceNumber      = 1,
  .bAlternateSetting     = 0,
  .bNumEndpoints         = 0,
  .bInterfaceClass       = UICLASS_AUDIO,
  .bInterfaceSubClass    = 2,
  .bInterfaceProtocol    = 0,
  .iInterface            = 0,
};

static struct usb_interface_descriptor g_fuac_as_interface_alt_1_desc =
{
  .bLength               = sizeof(struct usb_interface_descriptor),
  .bDescriptorType       = UDESC_INTERFACE,
  .bInterfaceNumber      = 1,
  .bAlternateSetting     = 1,
  .bNumEndpoints         = 1,
  .bInterfaceClass       = UICLASS_AUDIO,
  .bInterfaceSubClass    = 2,
  .bInterfaceProtocol    = 0,
  .iInterface            = 0,
};

static const struct uac_as_header_descriptor g_fuac_as_head_desc =
{
  .bLength              = sizeof(struct uac_as_header_descriptor),
  .bDescriptorType      = UAC_CS_INTERFACE,
  .bDescriptorSubtype   = UAC_HEADER,
  .bTerminalLink        = 4,
  .bDelay               = 0,
  .wFormatTag           = 1,
};

static const struct uac_format_type_i_discrete_descriptor g_fuac_as_type_i_desc =
{
  .bLength              = sizeof(struct uac_format_type_i_discrete_descriptor),
  .bDescriptorType      = UAC_CS_INTERFACE,
  .bDescriptorSubtype   = UAC_FORMAT_TYPE,
  .bFormatType          = 1,
  .bNrChannels          = 1,
  .bSubframeSize        = 2,
  .bBitResolution       = 16,
  .bSamFreqType         = 4,
  HSETM(.tSamFreq[0], 0x001f40),
  HSETM(.tSamFreq[1], 0x003e80),
  HSETM(.tSamFreq[2], 0x007d00),
  HSETM(.tSamFreq[3], 0x00bb80)
};

/* Standard ISO OUT Endpoint Descriptor */

static struct uac_endpoint_descriptor g_fuac_as_out_ep_desc =
{
  .bLength            = sizeof(struct uac_endpoint_descriptor),
  .bDescriptorType    = UDESC_ENDPOINT,
  .bEndpointAddress   = 0,
  .bmAttributes       = 0x0d,
  HSETW(.wMaxPacketSize, ISO_DATA_SIZE),
  .bInterval          = 4,
  .bRefresh           = 0,
  .bSynchAddress      = 0,
};

/* Standard ISO IN Endpoint Descriptor */

struct uac_endpoint_descriptor g_fuac_as_in_ep_desc =
{
  .bLength          = sizeof(struct uac_endpoint_descriptor),
  .bDescriptorType  = UDESC_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .bEndpointAddress = UE_DIR_IN | 0x1,
  .bmAttributes     = USB_ENDPOINT_XFER_ISOC | USB_ENDPOINT_XFER_SYNC,
  HSETW(.wMaxPacketSize, ISO_DATA_SIZE),
  .bInterval        = 4,
  .bRefresh         = 0,
  .bSynchAddress    = 0,
};

static const struct uac2_iso_endpoint_descriptor g_fuac_as_iso_in_desc =
{
  .bLength            = sizeof(struct uac2_iso_endpoint_descriptor),
  .bDescriptorType    = UDESC_CS_ENDPOINT,

  .bDescriptorSubtype = 1,
  .bmAttributes       = 1,
  .bmControls         = 0,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
};

const uint8_t *g_fuac_descriptors_array[] =
{
  (const uint8_t *)&g_fuac_config_desc,
  (const uint8_t *)&g_fuac_iad,
  (const uint8_t *)&g_fuac_ac_intf_desc,
  (const uint8_t *)&g_fuac_ac_head_desc,

  (const uint8_t *)&g_fuac_input_terminal_desc,
  (const uint8_t *)&g_fuac_output_terminal_desc,
  (const uint8_t *)&g_fuac_as_interface_alt_0_desc,
  (const uint8_t *)&g_fuac_as_interface_alt_1_desc,
  (const uint8_t *)&g_fuac_as_head_desc,
  (const uint8_t *)&g_fuac_as_type_i_desc,
  (const uint8_t *)&g_fuac_as_in_ep_desc,
  (const uint8_t *)&g_fuac_as_iso_in_desc,
  NULL,
};

uint8_t *link_fuac_descriptors(uint8_t *prefer, uint16_t ps, uint16_t *total_size)
{
  int i;
  uint8_t *des;
  uint8_t *pdes;
  uint16_t cs;
  errno_t err;
  uint16_t ds = 0;

  (void)prefer;
  (void)ps;

  /* Add the length of descriptors one by one */

  for (i = 0; g_fuac_descriptors_array[i]; ++i)
    {
      ds += (uint16_t)(*g_fuac_descriptors_array[i]);
    }

  if (total_size != NULL)
    {
      *total_size = ds;
    }

  des = memalign(64, SKB_DATA_ALIGN(ds));
  if (des == NULL)
    {
      usb_err("System out of memory! Descriptors length: %u\n", ds);
      return NULL;
    }
  (void)memset_s(des, SKB_DATA_ALIGN(ds), 0, SKB_DATA_ALIGN(ds));

  pdes = des;

  /* configuration descriptor needs to have the full length of rest of descriptors */

  g_fuac_config_desc.wTotalLength[0] = (uint8_t)ds;
  g_fuac_config_desc.wTotalLength[1] = (uint8_t)(ds >> 8);

  /* Add the length of audio control descriptors */

  cs  = (uint32_t)g_fuac_ac_head_desc.bLength;
  cs += (uint32_t)g_fuac_input_terminal_desc.bLength;
  cs += (uint32_t)g_fuac_output_terminal_desc.bLength;

  g_fuac_ac_head_desc.wTotalLength = (uint16_t)cs;

  for (i = 0; g_fuac_descriptors_array[i]; ++i)
    {
      const u8 *des_src = g_fuac_descriptors_array[i];
      u8 des_len = *des_src;
      err = memcpy_s(pdes, des_len, des_src, des_len);
      if (err != EOK)
        {
          usb_err("memcpy fail!\n");
          free(des);
          return NULL;
        }
      pdes += des_len;
    }

  return des;
}

static void fuac_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = (struct uac_dev_s *)req->priv;
  volatile uint32_t rate;

  (void) ep; /* ignored */
  switch (uac->control)
    {
    case ~0u:
      break;

    case UAC_VS_COMMIT_CONTROL:
      spin_lock(&uac->lock);
      uac->connected = 0x1;
      spin_unlock(&uac->lock);
      uac->control = ~0;
      break;

    case UAC_SETCUR_COMPLETE:
      rate = (((uint8_t *)req->buf)[0] | (((uint8_t *)req->buf)[1] << 8) |
              (((uint8_t *)req->buf)[2] << 16));

      if (uac->count == 0 || rate != g_uac_rate)
        {
          uac->count = 1;
          spin_lock(&uac->lock);
          uac_queue_free(uac);
          uac->busy_flag = 0;
          uac->connected = 1;
          spin_unlock(&uac->lock);
          g_uac_iso_data_size = (rate / 1000) * g_uac_opts.p_ssize * fuac_channel_num_get(g_uac_opts.p_chmask);
          g_uac_rate = rate;
          PRINTK("<<<rate:%u, size:%u>>>\n", rate, g_uac_iso_data_size);
          (void)EP_DISABLE(uac->in_ep);
          (void)EP_FLUSH(uac->in_ep);
          (void)EP_CONFIGURE(uac->in_ep, (const usb_endpoint_descriptor_t *)&g_fuac_as_in_ep_desc, 0);
        }
      uac->control = ~0;
      break;

    default:
      uac->control = ~0;
      break;
    }
}

static void fuac_output_request_complete(struct usbdev_ep_s *ep,
                                         struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
}

void fuac_input_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uac_dev_s *uac = &((struct uac_softc *)(ep->priv))->dev;
  struct uac_queue_node *node_tx;
  struct usbdev_req_s *req_temp = &uac->inputreq;

  spin_lock(&uac->lock);
  if (req->result != 0)
    {
      uac_queue_free(uac);
      spin_unlock(&uac->lock);
      return;
    }

  if (uac->cur_node != NULL)
    {
      node_tx = uac->cur_node;
      if (node_tx->buf_len == 0)
        {
          uac_queue_node_free(node_tx);
          uac->cur_node = NULL;
        }
      else
        {
          uac_send_data_sub(uac);
          spin_unlock(&uac->lock);
          return;
        }
    }

  if (!list_empty(&uac->uac_queue))
    {
      node_tx = list_first_entry(&uac->uac_queue, struct uac_queue_node, irqqueue);
      uac->cur_node = node_tx;
      list_del_init(&node_tx->irqqueue);
      uac->uac_queue_len--;
      uac_send_data_sub(uac);
    }
  else
    {
      req_temp->len = 0;
      (void)EP_SUBMIT(uac->in_ep, req_temp);
    }
  spin_unlock(&uac->lock);
}

static int audio_set_endpoint_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                  const struct usb_device_request *ctrl)
{
  uint8_t request = ctrl->bRequest;
  uint8_t new_req = 0;
  switch (request)
    {
    case SET_CUR_UAC:
    case SET_MIN_UAC:
    case SET_MAX_UAC:
    case SET_RES_UAC:
      req->len = UGETW(ctrl->wLength);
      new_req++;
      break;

    default:
      PRINT_ERR("Unknown index in [%s]: %#x\n", __FUNCTION__, (uint8_t)request);
      break;
    }

  if (new_req)
    {
      (void)EP_SUBMIT(dev->ep0, req);
    }
  return 0;
}

static int audio_get_endpoint_req(struct usbdev_s *dev, struct usbdev_req_s *req,
                                  const struct usb_device_request *ctrl)
{
  uint8_t request = ctrl->bRequest;
  uint8_t new_req = 0;
  uint8_t *buf    = (uint8_t *)req->buf;

  switch (request)
    {
    case GET_CUR_UAC:
    case GET_MIN_UAC:
    case GET_MAX_UAC:
    case GET_RES_UAC:
      req->len = UGETW(ctrl->wLength);
      buf[0]   = (uint8_t)g_uac_rate;
      buf[1]   = (uint8_t)(g_uac_rate >> 8);
      buf[2]   = (uint8_t)(g_uac_rate >> 16);
      new_req++;
      break;

    case GET_MEM_UAC:
      break;

    default:
      PRINT_ERR("Unknown index in [%s]: %#x\n", __FUNCTION__, request);
      break;
    }
  if (new_req)
    {
      (void)EP_SUBMIT(dev->ep0, req);
    }
  return 0;
}

static void fuac_source_free(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct uac_driver_s *drvr;
  struct uac_dev_s *uac;

  drvr = (struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return;
    }

  DEV_FREEEP(dev, uac->out_ep);
  DEV_FREEEP(dev, uac->in_ep);
}

static int usbclass_uac_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep;
  struct uac_driver_s *drvr;
  struct composite_dev_s *cdev;
  struct uac_dev_s *uac;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  cdev = dev->ep0->priv;
  drvr = (struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return -1;
    }

  INIT_LIST_HEAD(&uac->uac_queue);
  uac->busy_flag     = 0;
  uac->uac_queue_len = 0;
  uac->cur_node      = NULL;
  uac->count         = 0;
  spin_lock_init(&uac->lock);

  devobj = usbclass_devobj_get(cdev, DEV_UAC);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;

  /* initialize control endpoint */

  ep = DEV_ALLOCEP(dev, g_fuac_as_out_ep_desc.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fuac_as_out_ep_desc);
  if (ep == NULL)
    {
      return -1;
    }
  ep->priv = (void *)uac;
  (void)memset_s(&(uac->outputreq), sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uac->outputreq.callback =  fuac_output_request_complete;
  uac->outputreq.priv     = (void *)uac;
  ep->handle_req          = &uac->outputreq;
  uac->out_ep             = ep;
  devinfo->epno[0]        = ep->eplog;

  /* initialize AudioStreaming endpoint */

  ep = DEV_ALLOCEP(dev, g_fuac_as_in_ep_desc.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fuac_as_in_ep_desc);
  if (ep == NULL)
    {
      goto fail;
    }
  (void)memset_s(&uac->inputreq, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uac->inputreq.callback = fuac_input_req_complete;
  uac->inputreq.priv     = (void *)uac;
  uac->inputreq.buf      = NULL;
  ep->priv               = (void *)uac;
  ep->handle_req         = &uac->inputreq;
  uac->in_ep             = ep;
  devinfo->epno[1]       = ep->eplog;
  uac->control           = ~0;

  return 0;
fail:
  (void)usbclass_uac_unbind(driver, dev);
  return -1;
}

static int usbclass_uac_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  usbclass_uac_disconnect(driver, dev);

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }

  devobj = usbclass_devobj_get(cdev, DEV_UAC);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));

  fuac_source_free(driver, dev);
  return 0;
}

static int usbclass_uac_set_alt(struct uac_dev_s *dev, unsigned intf, unsigned alt)
{
  (void)intf;
  (void)alt;

  dev->busy_flag = 0;
  if (dev->in_ep_enabled == true)
    {
      (void)EP_DISABLE(dev->in_ep);
      dev->in_ep_enabled = false;
    }

  (void)EP_CONFIGURE(dev->in_ep, (const usb_endpoint_descriptor_t *)&g_fuac_as_in_ep_desc, 0);
  dev->in_ep_enabled = true;

  return 0;
}

static int usbclass_uac_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  struct uac_dev_s *uac;
  struct uac_driver_s *drvr;
  struct usbdev_req_s *req;

  (void)dataout;
  (void)outlen;

  if (dev == NULL || driver == NULL || ctrl == NULL)
    {
      return -1;
    }

  drvr = (struct uac_driver_s *)driver;
  uac  = drvr->dev;
  if (uac == NULL)
    {
      return -1;
    }

  req           = dev->ep0->handle_req;
  req_type      = ctrl->bmRequestType;
  req->callback = fuac_request_complete;
  req->priv     = uac;

  if (UT_GET_TYPE(req_type) == UT_STANDARD)
    {
      switch (ctrl->bRequest)
        {
        case USB_REQ_SET_CONFIGURATION:
        case USB_REQ_SET_INTERFACE:
          (void)usbclass_uac_set_alt(uac, UGETW(ctrl->wIndex), UGETW(ctrl->wValue));
          break;

        default:
          break;
        }
    }
  else
    {
      switch (req_type)
        {
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
          (void)audio_set_endpoint_req(dev, req, ctrl);
          uac->control = UAC_SETCUR_COMPLETE;
          break;

        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER:
        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_OTHER:
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
          break;

        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
          (void)audio_get_endpoint_req(dev, req, ctrl);
          break;

        default:
          break;
        }
    }
  return 0;
}

static void usbclass_uac_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct uac_driver_s *uac_drvr;
  struct uac_dev_s *uac_dev;

  uac_drvr = (struct uac_driver_s *)driver;
  uac_dev  = uac_drvr->dev;
  if (uac_dev == NULL)
    {
      return;
    }

  if (uac_dev->in_ep_enabled == true)
    {
      (void)EP_DISABLE(uac_dev->in_ep);
      uac_dev->in_ep_enabled = false;
    }
}

struct usbd_string g_fuac_device_strings[7] =
{
  { 0,  g_fuac_str_lang },
  { 1,  g_fuac_str_manufacturer },
  { 2,  g_fuac_str_product },
  { 4,  g_fuac_str_ac_if },
  { 8,  g_fuac_input_terminal },
  { 11, g_fuac_output_terminal },
  USBD_DEVICE_STRINGS_END
};

void uac_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fuac_device_desc, sizeof(g_fuac_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int16_t uac_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  uint16_t total_len;
  uint8_t *des;
  errno_t ret;

  g_fuac_iad.bFirstInterface                      = devinfo->ifnobase;
  g_fuac_ac_intf_desc.bInterfaceNumber            = devinfo->ifnobase;
  g_fuac_as_interface_alt_0_desc.bInterfaceNumber = devinfo->ifnobase + 1;
  g_fuac_as_interface_alt_1_desc.bInterfaceNumber = devinfo->ifnobase + 1;
  g_fuac_ac_head_desc.baInterfaceNr[0]            = devinfo->ifnobase + 1;

  des = link_fuac_descriptors(NULL, 0, &total_len);
  if (des != NULL)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, des, total_len);
      if (ret != EOK)
        {
          usb_err("memcpy_s fail!, ret:%d\n", ret);
          free(des);
          return 0;
        }
      free(des);
    }

  return (int16_t)total_len;
}

int uac_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fuac_device_strings[i].s != NULL; i++)
    {
      str = g_fuac_device_strings[i].s;
      if (g_fuac_device_strings[i].id == id)
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

#define UAC_NCONFIGS    1
#define UAC_CONFIGID    0
#define UAC_NINTERFACES 2
#define UAC_NSTRIDS     6
#define UAC_NUM_EPS     2
void uac_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = uac_mkdevdesc;
  dev->mkconfdesc = uac_mkcfgdesc;
  dev->mkstrdesc  = uac_mkstrdesc;

  dev->nconfigs = UAC_NCONFIGS;           /* Number of configurations supported */
  dev->configid = UAC_CONFIGID;           /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = UAC_NINTERFACES; /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = UAC_NSTRIDS; /* Number of Strings */

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = UAC_NUM_EPS;
}

int uac_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                    struct usbdevclass_driver_s **classdev)
{
  struct uac_softc *uac_s;
  struct uac_dev_s *priv;
  struct uac_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  uac_s = (struct uac_softc *)malloc(sizeof(struct uac_softc));
  if (uac_s == NULL)
    {
      return -1;
    }

  /* Convenience pointers into the allocated blob */

  priv = &uac_s->dev;
  drvr = &uac_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct uac_dev_s), 0, sizeof(struct uac_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_uac_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void uac_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct uac_driver_s *uac_drvr = (struct uac_driver_s *)classdev;
  struct uac_dev_s *priv;
  struct uac_softc *uac_s;

  if (uac_drvr == NULL)
    {
      return;
    }

  priv = uac_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  uac_s = container_of(uac_drvr, struct uac_softc, drvr);
  free(uac_s);
}

void usbdev_uac_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the UAC driver to fill in the constants we didn't
   * know here.
   */

  uac_get_composite_devdesc(dev);

  /* Overwrite and correct some values... */
  /* The callback functions for the UAC class */

  dev->classobject  = uac_classobject;
  dev->uninitialize = uac_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_uac_initialize(struct module *mod, int n, void *arg)
{
  struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev;
  int ret;

  (void)mod;
  (void)n;
  if (com_s == NULL)
    {
      return -1;
    }

  usbdev_uac_initialize_sub(&dev, 0, DEV_UAC);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** uac device initialized successfully! **\n");
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */