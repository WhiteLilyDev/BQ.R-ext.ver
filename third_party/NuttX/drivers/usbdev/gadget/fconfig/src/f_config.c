/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Composite Devices
 * Author: Yannik Li
 * Create: 2021-02-21
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
#include "f_common.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define FCONFIG_NSTRIDS                 5

#define USB_ETHER_FIRST_INTERFACE_NUM   0
#define USB_SERIAL_FIRST_INTERFACE_NUM  2

extern int composite_get_config_descriptor(struct usbdev_s *dev,
                                           struct usbdev_req_s *req, unsigned len);
extern void modify_device_descriptor_byspeed(struct usbdev_s *dev, uint8_t *buf);
extern int usb_composite_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);

int usbdev_fconfig_initialize(struct module *mod, int n, void *arg);

/* device driver structure definition */

static driver_t g_fconfig_driver_t =
{
  .name    = "fconfig",
  .methods = NULL,
  .size    = 0
};

/* private device class information */

static devclass_t g_fconfig_devclass;
DRIVER_MODULE(fconfig, simple, g_fconfig_driver_t, g_fconfig_devclass, usbdev_fconfig_initialize, 0);

static struct fconfig_softc g_fconfig_softc;

static int gadget_match(struct usb_obj *obj, void *match_data)
{
  struct composite_softc *com_s = (struct composite_softc *)match_data;
  struct gadget_info *gi = container_of(obj, struct gadget_info, obj);

  return gi->com_s == com_s;
}

static struct gadget_info *fconfig_find_gadget(struct composite_dev_s *com_dev)
{
  struct composite_softc *com_s = container_of(com_dev, struct composite_softc, dev);
  struct fconfig_softc *cdev = &g_fconfig_softc;
  struct usb_obj *obj;

  obj = usbobj_find(&cdev->obj, gadget_match, com_s);
  if (obj)
    {
      return container_of(obj, struct gadget_info, obj);
    }

  return NULL;
}

static void fconfig_setup_complete(struct usbdev_ep_s *ep,
                                   struct usbdev_req_s *req)
{
  (void)ep;
  if (req->result != 0)
    {
      dprintf("%s, result = %d\n", __func__, req->result);
    }
}

static int fconfig_submit_ctrlreq(struct usbdev_s *dev, struct usbdev_req_s *usb_req)
{
  int ret;

  usb_req->callback = fconfig_setup_complete;
  ret = EP_SUBMIT(dev->ep0, usb_req);
  if (ret < 0)
    {
      usb_err("%s: endpoint send fail!\n", __FUNCTION__);
      usb_req->result = 0;
    }
  return ret;
}

int gadget_strings_match(struct usb_obj *obj, void *match_data)
{
  struct gadget_strings *strings = container_of(obj, struct gadget_strings, obj);
  uint16_t language = *(uint16_t *)match_data;

  return strings->language == language;
}

#define STRING_ID_LEN 4
static int fconfig_mkstrdesc(struct composite_dev_s *com_dev, uint16_t language, uint8_t id, uint8_t *buf)
{
  struct gadget_info *gi;
  struct usb_string_descriptor *str_desc = (struct usb_string_descriptor *)buf;
  struct gadget_strings *dev_strings;
  struct usb_obj *obj;
  char lang_string[STRING_ID_LEN] = {STRING_ID_LEN, UDESC_STRING, 0x9, 0x4 };
  int i;
  int ret = -1;

  if (id == 0 && language == 0)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (void *)lang_string, lang_string[0]);
      if (ret != EOK)
        {
          return -1;
        }
      return lang_string[0];
    }

  gi = fconfig_find_gadget(com_dev);
  if (gi == NULL)
    {
      usb_err("%s: can not find gi\n", __FUNCTION__);
      return -1;
    }
  if (id < gi->str_count)
    {
      obj = usbobj_find(&gi->strings, gadget_strings_match, &language);
      if (!obj)
        {
          usb_err("%s: can not find the string with id = %u\n", __FUNCTION__, id);
          return -1;
        }
      dev_strings = container_of(obj, struct gadget_strings, obj);

      for (i = 0; dev_strings->strings[i].s != NULL; i++)
        {
          const char *str = dev_strings->strings[i].s;
          if (dev_strings->strings[i].id == id)
            {
              errno_t ret = utf8_to_utf16le(str, buf + 2, strlen(str));
              if (ret <= 0)
                {
                  usb_err("%s: memcpy_s failed, ret = %d\n", __FUNCTION__, ret);
                  return -1;
                }
              str_desc->bLength = 2 + (ret * 2);
              str_desc->bDescriptorType = UDESC_STRING;
              return str_desc->bLength;
            }
        }
    }
  else
    {
      for (i = 0; i < com_dev->ndevices; i++)
        {
          struct usbdev_devinfo_s *devinfo = &com_dev->device[i].compdesc.devinfo;
          if (id >= (devinfo->strbase + gi->str_count) &&
              id < (devinfo->strbase + devinfo->nstrings + gi->str_count))
            {
              *buf = (uint8_t)com_dev->device[i].compdesc.minor; /* pass minor number to driver */
              ret = com_dev->device[i].compdesc.mkstrdesc(id - devinfo->strbase - gi->str_count, buf);
              break;
            }
        }
    }
  return ret;
}

static void fconfig_mkdevdesc(struct composite_dev_s *priv, uint8_t *buf)
{
  struct gadget_info *gi;
  struct usb_device_descriptor  *dev_desc;
  errno_t ret;

  gi = fconfig_find_gadget(priv);
  if (gi == NULL)
    {
      usb_err("%s: gadget info is NULL\n", __FUNCTION__);
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, gi->dev_desc, sizeof(*gi->dev_desc));
  if (ret != EOK)
    {
      usb_err("%s: memcpy_s fail!, ret:%d\n", __FUNCTION__, ret);
      return;
    }
  dev_desc = ( struct usb_device_descriptor  *)buf;
  dev_desc->iManufacturer += 1;
  dev_desc->iProduct += 1;
  dev_desc->iSerialNumber += 1;
}

static int usb_config_match(struct usb_obj *obj, void *match_data)
{
  struct gadget_config *cfg = container_of(obj, struct gadget_config, obj);
  uint8_t config_num = *(uint8_t *)match_data;

  return (cfg->cfg_num == config_num);
}

static struct gadget_config *fconfig_find_cfg(struct composite_dev_s *com_dev, uint32_t *string_num)
{
  struct composite_softc *com_s = container_of(com_dev, struct composite_softc, dev);
  struct fconfig_softc *cdev = &g_fconfig_softc;
  struct usb_obj *obj;
  struct gadget_info *gi;
  uint8_t config_num = 0;
  struct gadget_config *cfg;

  obj = usbobj_find(&cdev->obj, gadget_match, com_s);
  if (!obj)
    {
      usb_err("%s: can't find gadget info\n", __FUNCTION__);
      return NULL;
    }
  gi = container_of(obj, struct gadget_info, obj);
  *string_num = gi->str_count;
  config_num = 1;
  obj = usbobj_find(&gi->obj, usb_config_match, &config_num);
  if (!obj)
    {
      usb_err("%s: can't find the configNumber %d\n", __FUNCTION__, config_num);
      return NULL;
    }
  cfg = container_of(obj, struct gadget_config, obj);

  return cfg;
}

static int modify_string_index_bystrbase(uint8_t *buf, int16_t len,
                                         uint32_t dev_str_num, int func_strbase)
{
  struct usb_descriptor *descriptors;
  struct usb_interface_assoc_descriptor *iad;
  struct usb_interface_descriptor *intf;
  uint32_t i;

  descriptors = (struct usb_descriptor *)buf;
  i = 0;
  while (i < len)
    {
      i += descriptors->bLength;
      if (descriptors->bDescriptorType == UDESC_IFACE_ASSOC)
        {
          iad = (struct usb_interface_assoc_descriptor *)descriptors;
          iad->iFunction += (uint8_t)(dev_str_num + func_strbase - 1);
        }
      else if (descriptors->bDescriptorType == UDESC_INTERFACE)
        {
          intf = (struct usb_interface_descriptor *)descriptors;
          if (intf->bNumEndpoints > 0)
            {
              intf->iInterface += (uint8_t)(dev_str_num + func_strbase - 1);
            }
        }
      descriptors = (struct usb_descriptor *)((char *)descriptors + descriptors->bLength);
    }
  return 0;
}

static int fconfig_mkcfgdesc(struct composite_dev_s *priv, uint8_t *buf)
{
  struct gadget_config *cfg;
  struct usb_config_descriptor *config_desc;
  int16_t total_len = 0;
  int16_t len = USB_CONFIG_DESC_SIZE;
  uint8_t *buf_tmp = buf;
  uint32_t dev_string_num;
  int ret, i;

  cfg = fconfig_find_cfg(priv, &dev_string_num);
  if (!cfg)
    {
      usb_err("%s: can't find gadget config\n", __FUNCTION__);
      return -1;
    }
  ret = memcpy_s(buf_tmp, USB_COMP_EP0_BUFSIZ,
                 (const void *)cfg->cfg_desc, (uint32_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;
  buf_tmp += len;
  for (i = 0; i < priv->ndevices; i++)
    {
      struct generic_driver_s *drvr = (struct generic_driver_s *)priv->device[i].dev;
      struct generic_dev_s *dev = drvr->dev;
      int func_strbase = priv->device[i].compdesc.devinfo.strbase;
      uint16_t off_len = 0;

      dprintf("%s,%d, raw_descs_length = %d\n", __func__, __LINE__, dev->raw_descs_length);
      dprintf("%s,%d, fs_len = %d\n", __func__, __LINE__, dev->fs_descs_len);
      dprintf("%s,%d, hs_len = %d\n", __func__, __LINE__, dev->hs_descs_len);
      dprintf("%s,%d, ss_len = %d\n", __func__, __LINE__, dev->ss_descs_len);
      dprintf("%s,%d, speed = %d\n", __func__, __LINE__, drvr->drvr.speed);
      len = dev->fs_descs_len;
      if (drvr->drvr.speed == USB_SPEED_HIGH)
        {
          off_len = dev->fs_descs_len;
          len = dev->fs_descs_len;
        }
      else if (drvr->drvr.speed == USB_SPEED_SUPER)
        {
          off_len = dev->fs_descs_len + dev->hs_descs_len;
          len = dev->ss_descs_len;
        }

      ret = memcpy_s(buf_tmp, (USB_COMP_EP0_BUFSIZ - total_len),
                     (const void *)((uint8_t *)dev->raw_descs + off_len), (uint32_t)len);
      if (ret != EOK)
        {
          usb_err("memcpy_s fail, ret:%d\n", ret);
          return -1;
        }
      modify_string_index_bystrbase(buf_tmp, len, dev_string_num, func_strbase);
      total_len   += len;
      buf_tmp += len;
    }
  config_desc = (struct usb_config_descriptor *)buf;
  USETW(config_desc->wTotalLength, total_len);
  config_desc->bNumInterface = priv->ninterfaces;
  config_desc->iConfiguration += 1;

  return total_len;
}

static int fconfig_composite_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                                   const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  struct composite_driver_s *driv = (struct composite_driver_s *)driver;
  int value   = 0;

  if (driv == NULL || driv->dev == NULL || ctrl == NULL || dev == NULL)
    {
      usb_err("%s: invalid parameter\n", __FUNCTION__);
      return -1;
    }

  if (ctrl->bRequest == USB_REQ_GET_DESCRIPTOR)
    {
      uint16_t w_value  = UGETW(ctrl->wValue);
      uint16_t w_length = UGETW(ctrl->wLength);
      uint16_t w_Index  = UGETW(ctrl->wIndex);
      struct composite_dev_s *usb_cdev = driv->dev;
      struct usbdev_req_s *usb_req = usb_cdev->ctrlreq;

      usb_req->len = 0;
      switch (w_value >> 8)
        {
        case UDESC_DEVICE:
          usb_debug("UDESC_DEVICE, usb_req:%x, buf:%x\n", usb_req, usb_req->buf);
          value = MIN(w_length, (UINT16)sizeof(struct usb_device_descriptor));
          fconfig_mkdevdesc(usb_cdev, usb_req->buf);
          modify_device_descriptor_byspeed(dev, usb_req->buf);
          usb_req->len = value;
          return fconfig_submit_ctrlreq(dev, usb_req);
        case UDESC_CONFIG:
          usb_debug("%s, UDESC_CONFIG, usb_req:%x, buf:%x\n", __func__, usb_req, usb_req->buf);
          value = fconfig_mkcfgdesc(usb_cdev, usb_req->buf);
          if (value < 0)
            {
              break;
            }
          usb_req->len = value;
          (void)composite_get_config_descriptor(dev, usb_req, w_length);
          dprintf("%s, UDESC_CONFIG, usb_req->len: %x\n", __func__, usb_req->len);
          return fconfig_submit_ctrlreq(dev, usb_req);

        case UDESC_STRING:
          {
            uint8_t strid = w_value & 0xff;
            usb_debug("UDESC_STRING:%d\n", strid);
            value = fconfig_mkstrdesc(usb_cdev, w_Index, strid, usb_req->buf);
            if (value < 0)
              {
                usb_err("%s: make string desc error\n", __FUNCTION__);
                return value;
              }
            usb_req->len = MIN(w_length, value);
            return fconfig_submit_ctrlreq(dev, usb_req);
          }
        default:
          /* will be processed by usb_composite_setup() */
          break;
        }
    }

  return usb_composite_setup(driver, dev, ctrl, dataout, outlen);
}

int fconfig_do_composite_initialize(struct composite_softc *softc, uint8_t ndevices,
                                    struct composite_devdesc_s *pdevices)
{
  struct composite_driver_s *drvr = &softc->drvr;
  int ret;

  ret = composite_initialize(softc, ndevices, pdevices);
  if (ret != 0)
    {
      return -1;
    }

  drvr->drvr.ops->setup = fconfig_composite_setup;
  return 0;
}

int usbdev_fconfig_initialize(struct module *mod, int n, void *arg)
{
  struct fconfig_softc *cdev = &g_fconfig_softc;
  struct composite_softc *com_s  = (struct composite_softc *)arg;
  int ret;

  (void)mod;
  (void)memset_s(cdev, sizeof(*cdev), 0, sizeof(*cdev));
  usbobj_init(&cdev->obj, "fconfig", NULL);

  if (com_s == NULL)
    {
      usb_err("%s: com_s is null\n", __FUNCTION__);
      return -1;
    }
  cdev->com_s = com_s;
  com_s->dev.config = 1;
  /* register fconfig device */
  ret = fconfig_fops_init(cdev);
  if (ret != LOS_OK)
    {
      usb_err("%s: create fconfig device node failed\n", __FUNCTION__);
      return -1;
    }
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
