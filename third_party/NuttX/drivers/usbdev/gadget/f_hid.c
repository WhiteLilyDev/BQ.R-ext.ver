/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS USB Driver HID Protocol
 * Author: wanghongxu
 * Create: 2019-10-24
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

#include "gadget/f_hid.h"
#include "gadget/usbd_hid.h"
#include "core/usbhid.h"
#include "implementation/global_implementation.h"
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otg_pcd.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int usbdev_hid_initialize(struct module *mod, int n, void *arg);

/* device driver structure definition */

static const driver_t g_fhid_driver =
{
  .name     = "fhid",
  .methods  = NULL,
  .size     = sizeof(struct hid_softc)
};

/* private device class information */

static devclass_t g_fhid_devclass;
DRIVER_MODULE(fhid, simple, g_fhid_driver, g_fhid_devclass, usbdev_hid_initialize, 0);

static int usbclass_hid_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_hid_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_hid_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_hid_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_hid_driverops =
{
  usbclass_hid_bind,
  usbclass_hid_unbind,
  usbclass_hid_setup,
  usbclass_hid_disconnect,
  NULL,
  NULL
};

static struct dev_report_desc g_fhid_report_desc;

#define HID_STRING_HEAD_LEN   2
#define HID_STRING_DESC_NUM   3
#define HID_STRING_LEN_MAX    0xFF

#define HID_STR_LANG 4
static const char g_fhid_str_lang[HID_STR_LANG] =
{
  HID_STR_LANG,
  UDESC_STRING,
  0x09, 0x04
};

#define HID_STR_IDX_INTERFACE 28
static const char g_fhid_str_interface[HID_STR_IDX_INTERFACE] =
{
  HID_STR_IDX_INTERFACE,
  UDESC_STRING,
  'H', 0, 'I', 0, 'D', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0,
  'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

struct usbd_string g_fhid_device_strings[6] =
{
  { 0, g_fhid_str_lang },
  { 1, NULL },
  { 2, NULL },
  { 3, NULL },
  { 4, g_fhid_str_interface },
  USBD_DEVICE_STRINGS_END
};

static struct usb_device_descriptor g_fhid_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE, /* Constant for device descriptor */
  HSETW(.bcdUSB, UD_BCD_USB),         /* USB version required: 2.0 */
  .bDeviceClass       = 0x00,         /* Miscellaneous Device Class */
  .bDeviceSubClass    = 0x00,         /* Common Class */
  .bDeviceProtocol    = 0x00,         /* Interface Association Descriptor */
  .bMaxPacketSize     = UD_USB_MPS,   /* Control Endpoint packet size */
  HSETW(.bcdDevice,   0x0100),        /* Device release code */
  .iManufacturer      = 1,            /* Manufacturer name, string index */
  .iProduct           = 2,            /* Product name, string index */
  .iSerialNumber      = 3,            /* Used */
  .bNumConfigurations = 1             /* One Configuration */
};

static struct usb_config_descriptor g_fhid_config_desc =
{
  .bLength              = sizeof(struct usb_config_descriptor),
  .bDescriptorType      = UDESC_CONFIG,
  HSETW(.wTotalLength, 0x0029), /* Size of all descriptors, set later */
  .bNumInterface        = 0x1,  /* Number of Interfaces */
  .bConfigurationValue  = 0x1,  /* ID of this configuration */
  .iConfiguration       = 0x0,  /* Index of string descriptor */
  .bmAttributes         = 0xc0, /* Self-powered */
  .bMaxPower            = 0x32  /* Maximum power consumption from the bus */
};

static const struct usb_interface_descriptor g_fhid_intf_desc =
{
  .bLength              = sizeof(struct usb_interface_descriptor),
  .bDescriptorType      = UDESC_INTERFACE,
  .bInterfaceNumber     = 0,    /* index number of this interface */
  .bAlternateSetting    = 0,    /* index of this settings */
  .bNumEndpoints        = 2,    /* Number of endpoint */
  .bInterfaceClass      = 0x03, /* bInterfaceClass: HID */
  .bInterfaceSubClass   = 0,    /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  .bInterfaceProtocol   = 0,    /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  .iInterface           = 4     /* index of string descriptor */
};

static struct usb_hid_descriptor g_fhid_desc =
{
  .bLength          = sizeof(struct usb_hid_descriptor),
  .bDescriptorType  = UDESC_HID, /* HID type is 0x21 */
  HSETW(.bcdHID, 0x0110),   /* bcdHID: HID Class Spec release number HID 1.1 */
  .bCountryCode     = 0x00, /* bCountryCode: Hardware target country */
  .bNumDescriptors  = 0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
  {
    {
      .bDescriptorType = 0x22, /* bDescriptorType */
    }
  }
};

static const struct hid_endpoint_descriptor g_fhid_in_ep_desc =
{
  .bLength          = sizeof(struct hid_endpoint_descriptor),
  .bDescriptorType  = UDESC_ENDPOINT,
  .bEndpointAddress = UE_DIR_IN | 0x01,
  .bmAttributes     = 0x03,                  /* bmAttributes = 00000011b */
  HSETW(.wMaxPacketSize,  HID_IN_DATA_SIZE), /* wMaxPacketSize = 64 */
  .bInterval        = 0x04                   /* bInterval = 4ms */
};

static const struct hid_endpoint_descriptor g_fhid_out_ep_desc =
{
  .bLength          = sizeof(struct hid_endpoint_descriptor),
  .bDescriptorType  = UDESC_ENDPOINT,
  .bEndpointAddress = UE_DIR_OUT | 0x01,
  .bmAttributes     = 0x03,
  HSETW(.wMaxPacketSize,  HID_OUT_DATA_SIZE),
  .bInterval        = 0x04
};

const uint8_t *g_fhid_desc_array[] =
{
  (const uint8_t *)&g_fhid_config_desc,
  (const uint8_t *)&g_fhid_intf_desc,
  (const uint8_t *)&g_fhid_desc,
  (const uint8_t *)&g_fhid_in_ep_desc,
  (const uint8_t *)&g_fhid_out_ep_desc,
  NULL,
};

void hid_deviceid_info(uint16_t vendorid, uint16_t productid)
{
  USETW(g_fhid_device_desc.idVendor, vendorid);

  USETW(g_fhid_device_desc.idProduct, productid);
}

int hid_device_string_info(const struct dev_string_desc *str_manufacturer,
                           const struct dev_string_desc *str_product,
                           const struct dev_string_desc *str_serial_number)
{
  char *buf[HID_STRING_DESC_NUM] = {NULL};
  const struct dev_string_desc *str_desc[HID_STRING_DESC_NUM];
  int i;
  uint32_t len;

  if (str_manufacturer == NULL || str_product == NULL || str_serial_number == NULL)
    {
      usb_err("%s failed, invalid param!\n", __FUNCTION__);
      return -1;
    }

  if (str_manufacturer->str == NULL || str_manufacturer->len == 0 ||
      str_serial_number->str == NULL || str_serial_number->len == 0 ||
      str_product->str == NULL || str_product->len == 0)
    {
      usb_err("%s failed, str is NULL or len is 0\n", __FUNCTION__);
      return -1;
    }

  if (str_manufacturer->len > (HID_STRING_LEN_MAX - HID_STRING_HEAD_LEN) ||
      str_serial_number->len > (HID_STRING_LEN_MAX - HID_STRING_HEAD_LEN) ||
      str_product->len > (HID_STRING_LEN_MAX - HID_STRING_HEAD_LEN))
    {
      usb_err("%s failed, len exceeds maximum limit! str_manufacturer->len = %u"
              "str_serial_number->len = %u str_product->len = %u\n", __FUNCTION__,
              str_manufacturer->len, str_serial_number->len, str_product->len);
      return -1;
    }

  str_desc[0] = str_manufacturer;
  str_desc[1] = str_product;
  str_desc[2] = str_serial_number;

  for (i = 0; i < HID_STRING_DESC_NUM; i++)
    {
      len = str_desc[i]->len + HID_STRING_HEAD_LEN;
      buf[i] = (char *)malloc(len);
      if (buf[i] == NULL)
        {
          usb_err("%s malloc failed\n", __FUNCTION__);
          goto errout;
        }
      g_fhid_device_strings[i + 1].s = buf[i];

      *buf[i] = (char)len;
      *(buf[i] + 1) = UDESC_STRING;

      /* len represents the size of the string */

      (void)memcpy_s(buf[i] + HID_STRING_HEAD_LEN, (size_t)str_desc[i]->len,
                     str_desc[i]->str, (size_t)str_desc[i]->len);
    }

  return 0;

errout:
  for (i = 0; i < HID_STRING_DESC_NUM; i++)
    {
      if (buf[i] != NULL)
        {
          free(buf[i]);
        }
    }
  return -1;
}

int hid_report_descriptor_info(const void *buf, size_t len)
{
  uint8_t *report_desc;

  if (buf == NULL || len == 0)
    {
      usb_err("%s failed, buf is NULL or len is 0\n", __FUNCTION__);
      return -1;
    }

  report_desc = (uint8_t *)malloc(len);
  if (report_desc == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return -1;
    }

  g_fhid_report_desc.report_size = len;
  g_fhid_report_desc.report_desc = report_desc;

  /* len represents the size of the report */

  (void)memcpy_s(report_desc, len, buf, len);

  return 0;
}

static void usbdev_hid_free(void)
{
  int i;

  if (g_fhid_report_desc.report_desc != NULL)
    {
      free(g_fhid_report_desc.report_desc);
      g_fhid_report_desc.report_desc = NULL;
    }

  for (i = 1; i <= HID_STRING_DESC_NUM; i++)
    {
      if (g_fhid_device_strings[i].s != NULL)
        {
          free((void *)g_fhid_device_strings[i].s);
          g_fhid_device_strings[i].s = NULL;
        }
    }
}

static void fhid_output_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct hid_dev_s *hid;
  errno_t ret;
  uint32_t ret_event;

  if (ep == NULL || ep->priv == NULL || req == NULL)
    {
      usb_err("Illegal request or ep!\n");
      return;
    }

  if (req->result != 0)
    {
      return;
    }

  hid = (struct hid_dev_s *)ep->priv;
  spin_lock(&hid->hid_lock);

  /* Save private data of read request */

  hid->read_len = req->xfrd;
  ret = memcpy_s(hid->read_buf, HID_OUT_DATA_SIZE, req->buf, req->xfrd);
  if (ret != EOK)
    {
      spin_unlock(&hid->hid_lock);
      usb_err("memcpy fail!\n");
      return;
    }
  spin_unlock(&hid->hid_lock);

  ret_event = LOS_EventWrite(&hid->read_event, USB_HID_READ_EVENT);
  if (ret_event != LOS_OK)
    {
      usb_err("write event failed!\r\n");
    }

  (void)EP_SUBMIT(hid->out_ep, req);
}

static void fhid_input_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct hid_dev_s *hid = (struct hid_dev_s *)ep->priv;
  struct hid_queue_node *node_temp;

  spin_lock(&hid->hid_lock);
  atomic_set(&hid->busy_flag, 0);

  if (req->result != 0)
    {
      hid_queue_free(hid);
      spin_unlock(&hid->hid_lock);
      return;
    }

  if (hid->cur_node != NULL)
    {
      node_temp = hid->cur_node;
      if (node_temp->buf_len == 0)
        {
          hid_queue_node_free(node_temp);
          hid->cur_node = NULL;
        }
      else
        {
          hid_send_data_sub(hid);
          spin_unlock(&hid->hid_lock);
          return;
        }
    }

  if (!list_empty(&hid->hid_queue))
    {
      node_temp = list_first_entry(&hid->hid_queue, struct hid_queue_node, irqqueue);
      hid->cur_node = node_temp;
      list_del_init(&node_temp->irqqueue);
      hid->hid_queue_len--;
      hid_send_data_sub(hid);
    }
  spin_unlock(&hid->hid_lock);
}

static void fhid_source_free(struct hid_dev_s *hid, struct usbdev_s *dev)
{
  if (hid != NULL)
    {
      DEV_FREEEP(dev, hid->out_ep);
      DEV_FREEEP(dev, hid->in_ep);
    }
}

static int usbclass_hid_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep;
  struct hid_driver_s *drvr;
  struct composite_dev_s *cdev;
  struct hid_dev_s *hid;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  int ret;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  cdev = dev->ep0->priv;
  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;
  if (hid == NULL)
    {
      return -1;
    }

  INIT_LIST_HEAD(&hid->hid_queue);
  hid->busy_flag     = 0;
  hid->hid_queue_len = 0;
  hid->cur_node      = NULL;

  devobj = usbclass_devobj_get(cdev, DEV_HID);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;

  /* Initialize the interrupt output endpoint */

  ep = DEV_ALLOCEP(dev, g_fhid_out_ep_desc.bEndpointAddress, (struct usb_endpoint_descriptor *)&g_fhid_out_ep_desc);
  if (ep == NULL)
    {
      return -1;
    }

  (void)memset_s(&(hid->outputreq), sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  hid->outputreq.callback = fhid_output_request_complete;
  hid->outputreq.priv     = (void *)hid;
  hid->outputreq.buf      = NULL;
  ep->priv                = (void *)hid;
  ep->handle_req          = &hid->outputreq;
  hid->out_ep             = ep;
  devinfo->epno[0]        = ep->eplog;

  (VOID)LOS_EventInit(&hid->read_event);
  spin_lock_init(&hid->hid_lock);
  hid->read_len = 0;
  hid->read_buf = memalign(USB_CACHE_ALIGN_SIZE, HID_OUT_DATA_SIZE);
  if (hid->read_buf == NULL)
    {
      usb_err("Malloc failed!\n");
      goto errout;
    }

  /* Initialize the interrupt input endpoint */

  ep = DEV_ALLOCEP(dev, g_fhid_in_ep_desc.bEndpointAddress, (struct usb_endpoint_descriptor *)&g_fhid_in_ep_desc);
  if (ep == NULL)
    {
      goto errout;
    }

  (void)memset_s(&hid->inputreq, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  hid->inputreq.callback = fhid_input_req_complete;
  hid->inputreq.priv     = (void *)hid;
  hid->inputreq.buf      = NULL;
  ep->priv               = (void *)hid;
  ep->handle_req         = &hid->inputreq;
  hid->in_ep             = ep;
  devinfo->epno[1]       = ep->eplog;

  /* Registered character device */

  ret = hid_fops_init(hid);
  if (ret != LOS_OK)
    {
      goto errout;
    }

  return 0;
errout:
  (void)usbclass_hid_unbind(driver, dev);
  return -1;
}

static int usbclass_hid_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct hid_driver_s *drvr;
  struct hid_dev_s *hid;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;

  if (atomic_read(&hid->open_flag))
    {
      usb_err("HID device is busy!\n");
      return -1;
    }

  (void)hid_fops_deinit(hid);

  usbclass_hid_disconnect(driver, dev);

  /* Destroy read event */

  (VOID)LOS_EventDestroy(&hid->read_event);
  if (hid->read_buf != NULL)
    {
      free(hid->read_buf);
      hid->read_buf = NULL;
    }

  fhid_source_free(hid, dev);

  cdev = dev->ep0->priv;
  devobj = usbclass_devobj_get(cdev, DEV_HID);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));

  return 0;
}

static void usbclass_hid_set_endpoint(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_req_s *req;
  struct hid_driver_s *drvr;
  struct hid_dev_s *hid;
  int ret;

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;

  hid->busy_flag = 0;
  if (hid->in_ep_enabled == true)
    {
      (void)EP_DISABLE(hid->in_ep);
      hid->in_ep_enabled = false;
    }

  ret = EP_CONFIGURE(hid->in_ep, (const usb_endpoint_descriptor_t *)&g_fhid_in_ep_desc, 0);
  if (ret < 0)
    {
      usb_err("Config interrupt in_ep failed!\n");
      goto errout;
    }
  hid->in_ep_enabled = true;

  if (hid->out_ep_enabled == true)
    {
      (void)EP_DISABLE(hid->out_ep);
      hid->out_ep_enabled = false;
    }

  ret = EP_CONFIGURE(hid->out_ep, (const usb_endpoint_descriptor_t *)&g_fhid_out_ep_desc, 0);
  if (ret < 0)
    {
      usb_err("Config interrupt out_ep failed!\n");
      goto errout;
    }
  hid->out_ep_enabled = true;

  req = hid->out_ep->handle_req;
  req->buf = (uint8_t *)hid->read_buf;
  req->len = HID_OUT_DATA_SIZE;
  ret      = EP_SUBMIT(hid->out_ep, req);
  if (ret != OK)
    {
      usb_err("out_ep submit failed!\n");
      goto errout;
    }

  return;

errout:
  usbclass_hid_disconnect(driver, dev);
}

static void usbclass_hid_get_report(struct usbdev_s *dev, uint32_t len)
{
  struct usbdev_req_s *req = dev->ep0->handle_req;
  errno_t ret;

  ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                 g_fhid_report_desc.report_desc, g_fhid_report_desc.report_size);
  if (ret != EOK)
    {
      usb_err("memcpy fail\n");
      return;
    }

  req->len = MIN(len, g_fhid_report_desc.report_size);
}

static int usbclass_hid_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  uint16_t w_value;
  struct hid_dev_s *hid;
  struct hid_driver_s *drvr;
  struct usbdev_req_s *req;
  int ret;
  int new_req = 0;

  (void)dataout; (void)outlen;

  if (dev == NULL || driver == NULL || ctrl == NULL)
    {
      return -1;
    }

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;
  if (hid == NULL)
    {
      return -1;
    }

  w_value   = UGETW(ctrl->wValue);
  req       = dev->ep0->handle_req;
  req_type  = ctrl->bmRequestType;
  req->priv = hid;

  if (UT_GET_TYPE(req_type) == UT_STANDARD)
    {
      switch (ctrl->bRequest)
        {
        case USB_REQ_SET_CONFIGURATION:
        case USB_REQ_SET_INTERFACE:
          usbclass_hid_set_endpoint(driver, dev);
          break;

        case UR_GET_HID_DESCRIPTOR:
          {
            if ((w_value >> 8) == UDESC_REPORT)
              {
                usbclass_hid_get_report(dev, UGETW(ctrl->wLength));
                new_req = 1;
              }
          }
          break;

        default:
          break;
        }
    }
  else
    {
      switch (ctrl->bRequest)
        {
        case UR_SET_IDLE:
          {
            req->len = 0;
            new_req  = 1;
          }
          break;

        case UR_SET_REPORT:
          {
            req->len = MIN(UGETW(ctrl->wLength), HID_IN_DATA_SIZE);
            ret = memset_s(req->buf, req->len, 0, req->len);
            if (ret != EOK)
              {
                usb_err("memset fail!\n");
                return -1;
              }
            new_req = 1;
          }
          break;

        case UR_GET_IDLE:
          {
            req->len = 0;
            new_req  = 1;
          }
          break;

        case UR_GET_REPORT:
          {
            req->len = MIN(UGETW(ctrl->wLength), HID_IN_DATA_SIZE);
            ret = memset_s(req->buf, req->len, 0, req->len);
            if (ret != EOK)
              {
                usb_err("memset fail!\n");
                return -1;
              }
            new_req = 1;
          }
          break;

        default:
          break;
        }
  }

  if (new_req)
    {
      ret = EP_SUBMIT(dev->ep0, req);
      if (ret != OK)
        {
          usb_err("Endpoint send fail!\n");
          req->result = 0;
          return -1;
        }
    }
  return 0;
}

static void usbclass_hid_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct hid_driver_s *hid_drvr;
  struct hid_dev_s *hid_dev;

  (void)dev;

  hid_drvr = (struct hid_driver_s *)driver;
  hid_dev  = hid_drvr->dev;
  if (hid_dev == NULL)
    {
      return;
    }

  if (hid_dev->in_ep_enabled == true)
    {
      (void)EP_DISABLE(hid_dev->in_ep);
      hid_dev->in_ep_enabled = false;
    }

  if (hid_dev->out_ep_enabled == true)
    {
      (void)EP_DISABLE(hid_dev->out_ep);
      hid_dev->out_ep_enabled = false;
    }
}

static uint8_t *linkg_fhid_descriptors(const uint8_t *prefer, uint16_t ps, uint16_t *total_size)
{
  int i;
  uint8_t *des;
  uint8_t *pdes;
  uint16_t ds = 0;
  errno_t ret;

  (void)prefer;
  (void)ps;

  /* Add the length of descriptors one by one */

  for (i = 0; g_fhid_desc_array[i] != NULL; ++i)
    {
      ds += (uint16_t)(*g_fhid_desc_array[i]);
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
  (void)memset_s((void *)des, SKB_DATA_ALIGN(ds), 0, SKB_DATA_ALIGN(ds));

  pdes = des;

  /* configuration descriptor needs to have the full length of rest of descriptors */

  USETW(g_fhid_config_desc.wTotalLength, ds);

  for (i = 0; g_fhid_desc_array[i] != NULL; ++i)
    {
      const uint8_t *des_src = g_fhid_desc_array[i];
      uint8_t des_len        = *des_src;
      ret = memcpy_s(pdes, SKB_DATA_ALIGN(ds), des_src, des_len);
      if (ret != EOK)
        {
          usb_err("memcpy fail!\n");
          free(des);
          return NULL;
        }
      pdes += des_len;
    }

  return des;
}

static void hid_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fhid_device_desc, sizeof(g_fhid_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

static int16_t hid_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  uint16_t total_len;
  uint8_t *des;
  errno_t ret;

  (void)devinfo;

  USETW(g_fhid_desc.descrs[0].wDescriptorLength, g_fhid_report_desc.report_size);

  des = linkg_fhid_descriptors(NULL, 0, &total_len);
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

static int hid_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fhid_device_strings[i].s != NULL; i++)
    {
      str = g_fhid_device_strings[i].s;
      if (g_fhid_device_strings[i].id == id)
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

#define HID_NCONFIGS    1
#define HID_CONFIGID    0
#define HID_NINTERFACES 1
#define HID_NSTRIDS     5
#define HID_NUM_EPS     2
static void hid_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = hid_mkdevdesc;
  dev->mkconfdesc = hid_mkcfgdesc;
  dev->mkstrdesc  = hid_mkstrdesc;

  dev->nconfigs = HID_NCONFIGS; /* Number of configurations supported */
  dev->configid = HID_CONFIGID; /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = HID_NINTERFACES; /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = HID_NSTRIDS; /* Number of Strings */

  /* Endpoints.
    *
    * Endpoint numbers must be provided by board-specific logic.
    */

  dev->devinfo.nendpoints = HID_NUM_EPS;
}

static int hid_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                           struct usbdevclass_driver_s **classdev)
{
  struct hid_softc *hid_s;
  struct hid_dev_s *priv;
  struct hid_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  hid_s = (struct hid_softc *)malloc(sizeof(struct hid_softc));
  if (hid_s == NULL)
    {
      return -1;
    }

  /* Convenience pointers into the allocated blob */

  priv = &hid_s->dev;
  drvr = &hid_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct hid_dev_s), 0, sizeof(struct hid_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_hid_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

static void hid_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct hid_driver_s *hid_drvr = (struct hid_driver_s *)classdev;
  struct hid_dev_s *priv;
  struct hid_softc *hid_s;

  usbdev_hid_free();

  if (hid_drvr == NULL)
    {
      return;
    }

  priv = hid_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  hid_s = container_of(hid_drvr, struct hid_softc, drvr);
  if (hid_s != NULL)
    {
      free(hid_s);
    }
}

static void usbdev_hid_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the HID driver to fill in the constants we didn't
   * know here.
   */

  hid_get_composite_devdesc(dev);

  /* Overwrite and correct some values. */

  /* The callback functions for the HID class */

  dev->classobject = hid_classobject;
  dev->uninitialize = hid_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor = minor;               /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_hid_initialize(struct module *mod, int n, void *arg)
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

  usbdev_hid_initialize_sub(&dev, 0, DEV_HID);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** hid device initialized successfully! **\n");
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */