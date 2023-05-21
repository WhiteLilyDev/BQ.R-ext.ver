/****************************************************************************
 * drivers/usbdev/composite.c
 *
 *   Copyright (C) 2012, 2016-2017 Gregory Nutt. All rights reserved.
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
#include "los_vm_iomap.h"
#include "gadget/composite.h"
#define FCOFNIG_FUN

#undef USB_DEBUG_VAR
#define USB_DEBUG_VAR g_composite_debug
#ifdef LOSCFG_USB_DEBUG
static int g_composite_debug = 0;
void usb_composite_debug_func(int level)
{
  g_composite_debug = level;
  PRINTK("The level of usb composite debug is %d\n", level);
}
DEBUG_MODULE(composite, usb_composite_debug_func);
#endif

int composite_device_init(struct composite_softc *cdev);

static const uint8_t g_bos_desc_buf[22] =
{
  5, 0xf, 0x16, 0, 2, 7, 0x10, 2, 6, 0, 0, 0,
  0xa, 0x10, 3, 0, 0xf, 0, 1, 1, 0xf4, 1
};

static device_probe_t  composite_probe;
static device_attach_t composite_attach;
static device_detach_t composite_detach;
static devclass_t      composite_devclass;

static device_method_t g_composite_methods[] =
{
  /* Device interface */

  DEVMETHOD(device_probe,  composite_probe),
  DEVMETHOD(device_attach, composite_attach),
  DEVMETHOD(device_detach, composite_detach),
  DEVMETHOD_END
};

static driver_t g_composite_driver =
{
  .name    = "composite",
  .methods = g_composite_methods,
  .size    = sizeof(struct composite_softc)
};

DRIVER_MODULE(composite, hiudc3, g_composite_driver, composite_devclass, NULL, NULL);

void usbd_configep_byspeed(struct usbdev_s *dev, struct usb_endpoint_descriptor *ep_desc)
{
  if (ep_desc->bmAttributes == UE_BULK)
    {
      switch (dev->speed)
        {
        case USB_SPEED_SUPER:
          USETW(ep_desc->wMaxPacketSize, USB_DWC_MAX_PACKET_SIZE);
          break;

        case USB_SPEED_HIGH:
          USETW(ep_desc->wMaxPacketSize, USB_DWC_U2_MAX_PACKET_SIZE);
          break;

        default:
          PRINT_ERR("%s invalid speed:%u\r\n", __FUNCTION__, dev->speed);
          break;
        }
    }

  return;
}

void composite_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
}

static struct usbdev_req_s *composite_allocreq(uint16_t len)
{
  DMA_ADDR_T paddr;
  struct usbdev_req_s *req = zalloc(sizeof(struct usbdev_req_s));
  if (req != NULL)
    {
      req->len = len;
      req->buf = (uint8_t *)LOS_DmaMemAlloc(&paddr, len, USB_CACHE_ALIGN_SIZE, DMA_NOCACHE);
      if (req->buf == NULL)
        {
          free(req);
          return NULL;
        }
      req->dma = (DMA_ADDR_T)VMM_TO_UNCACHED_ADDR((unsigned long)req->buf);
    }

  return req;
}

static void composite_freereq(struct usbdev_req_s *req)
{
  if (req != NULL)
    {
      if (req->buf != NULL)
        {
          (VOID)LOS_DmaMemFree(req->buf);
          req->buf = NULL;
        }
      free(req);
    }
}

static int usb_composite_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  int ret;

  if (driver == NULL || dev == NULL)
    {
      PRINT_ERR("unbind fail, invalid gadget\n");
      return -1;
    }

  cdev = ((struct composite_driver_s *)driver)->dev;

  /* Make sure that we are not already unbound */

  if (cdev != NULL)
    {
      int i;

      /* Unbind the constituent class drivers */

      for (i = 0; i < cdev->ndevices; i++)
        {
          ret = CLASS_UNBIND(cdev->device[i].dev, dev);
          if (ret < 0)
            {
              PRINT_ERR("unbind gadget fail, busy!\n");
              return -1;
            }
        }

      /* Free the pre-allocated control request */

      cdev->config = COMPOSITE_CONFIGIDNONE;
      composite_freereq(cdev->ctrlreq);
      cdev->ctrlreq = NULL;
      cdev->usbdev  = NULL;
    }
  dev->ep0->priv = NULL;

  return 0;
}

static int usb_composite_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  int ret;
  int i;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }
  cdev = ((struct composite_driver_s *)driver)->dev;
  if (cdev == NULL)
    {
      PRINT_ERR("cdev == NULL\n");
      return -1;
    }

  /* Bind the structures */

  cdev->usbdev = dev;

  /* initialize ctrlreq and allocate memory for it's buf */

  cdev->ctrlreq = composite_allocreq(USB_COMP_EP0_BUFSIZ);
  if (cdev->ctrlreq == NULL)
    {
      PRINT_ERR("%s malloc control request buf failed!\n", __FUNCTION__);
      goto failed;
    }
  dev->ep0->handle_req    = cdev->ctrlreq;
  cdev->ctrlreq->callback = composite_request_complete;

  /* Save the reference to our private data structure in EP0 so that it
   * can be recovered in ep0 completion events.
   */
  dev->ep0->priv = cdev;
  for (i = 0; i < cdev->ndevices; i++)
    {
      ret = CLASS_BIND(cdev->device[i].dev, dev);
      if (ret < 0)
        {
          PRINT_ERR(" composite device bind gadget failed!\n");
          goto failed;
        }
    }

  DPRINTF("composite bind device success!\n");
  return 0;

failed:
  (void)usb_composite_unbind(driver, dev);
  return -1;
}

static int modify_config_descriptor_byspeed(struct usbdev_s *dev, struct usbdev_req_s *req)
{
  struct usb_descriptor *descriptors;
  uint32_t config_len = req->len;
  uint32_t new_config_len;
  uint8_t *config_tmp_buf;
  struct usb_config_descriptor *config_des;
  uint32_t i;
  int ret;

  if (dev->speed != USB_SPEED_SUPER && dev->speed != USB_SPEED_HIGH)
    {
      usb_err("Device controller speed can not support!\n");
      return -EINVAL;
    }

  if (dev->speed == USB_SPEED_SUPER)
    {
      return 0; /* The configuration descriptor defaults to the superspeed form. */
    }

  config_tmp_buf = malloc(config_len);
  if (config_tmp_buf == NULL)
    {
      usb_err("malloc failed\n");
      return -ENOMEM;
    }

  ret = memset_s(config_tmp_buf, config_len, 0, config_len);
  if (ret != EOK)
    {
      usb_err("memset_s failed, %d\n", ret);
      goto err;
    }

  descriptors = (struct usb_descriptor *)req->buf;
  i = 0;
  new_config_len = 0;
  while (i < config_len)
    {
      i += descriptors->bLength;
      if (descriptors->bDescriptorType != UDESC_ENDPOINT_SS_COMP)
        {
          ret = memcpy_s(config_tmp_buf + new_config_len, config_len - new_config_len,
                         descriptors, descriptors->bLength);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, %d\n", ret);
              goto err;
            }
          new_config_len += descriptors->bLength;
        }
      descriptors = (struct usb_descriptor *)((char *)descriptors + descriptors->bLength);
    }
  ret = memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, config_len);
  if (ret != EOK)
    {
      usb_err("memset_s failed, %d\n", ret);
      goto err;
    }

  ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, config_tmp_buf, new_config_len);
  if (ret != EOK)
    {
      usb_err("memcpy_s failed, %d\n", ret);
      goto err;
    }

  config_des = (struct usb_config_descriptor *)req->buf;
  USETW(config_des->wTotalLength, new_config_len);
  req->len = new_config_len;
  free(config_tmp_buf);

  return 0;

err:
  free(config_tmp_buf);
  return -EINVAL;
}

int composite_get_config_descriptor(struct usbdev_s *dev,
                                           struct usbdev_req_s *req, unsigned len)
{
  int retval;
  struct usb_descriptor *descriptors;
  struct usb_endpoint_descriptor *ep_des;
  int ret;
  int i = 0;

  ret = modify_config_descriptor_byspeed(dev, req);
  if (ret < 0)
    {
      return -1;
    }

  retval = min(len, req->len);
  descriptors = (struct usb_descriptor *)req->buf;

  while (i < retval)
    {
      i += descriptors->bLength;
      if (descriptors->bDescriptorType != UDESC_ENDPOINT)
        {
          descriptors = (struct usb_descriptor *)((char *)descriptors + descriptors->bLength);
          continue;
        }

      ep_des = (struct usb_endpoint_descriptor *)descriptors;
      usbd_configep_byspeed(dev, ep_des);
      descriptors = (struct usb_descriptor *)((char *)descriptors + descriptors->bLength);
    }

  req->len = retval;
  return retval;
}

void modify_device_descriptor_byspeed(struct usbdev_s *dev, uint8_t *buf)
{
  struct usb_device_descriptor *desc = (struct usb_device_descriptor *)buf;
  switch (dev->speed)
    {
    case USB_SPEED_SUPER:
      USETW(desc->bcdUSB, 0x0300);
      desc->bMaxPacketSize = 0x09;
      break;

    case USB_SPEED_HIGH:
      USETW(desc->bcdUSB, 0x0200);
      desc->bMaxPacketSize = 0x40;
      break;

    default:
      usb_err("invalid speed:%u\n", dev->speed);
      break;
    }
}

static int composite_classsetup(struct composite_dev_s *priv,
                                struct usbdev_s *dev,
                                const struct usb_device_request *ctrl,
                                uint8_t *dataout, size_t outlen)
{
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  uint16_t index;
  uint8_t  interface;
  int ret = -EOPNOTSUPP;
  int i;
  int j;

  (void)dataout;
  (void)outlen;

  index     = UGETW(ctrl->wIndex);
  interface = (uint8_t)(index & 0xff);

  /* Usb standard protocol logic processing, such as set interface. */

  for (i = 0; i < priv->ndevices; i++)
    {
      if (interface >= priv->device[i].compdesc.devinfo.ifnobase &&
          interface < (priv->device[i].compdesc.devinfo.ifnobase +
          priv->device[i].compdesc.devinfo.ninterfaces))
        {
          ret = CLASS_SETUP(priv->device[i].dev, dev, ctrl, dataout, outlen);
          return ret;
        }
    }

  /* Specific protocol logic processing, such as UAC class setup. */

  for (i = 0; i < priv->ndevices; i++)
    {
      devobj  = &priv->device[i];
      devinfo = &devobj->compdesc.devinfo;
      for (j = 0; j < USBDEV_MAX_EPNUM; j++)
        {
          if (devinfo->epno[j] == interface)
            {
              ret = CLASS_SETUP(devobj->dev, dev, ctrl, dataout, outlen);
              break;
            }
        }
    }

  return ret;
}

static void composite_setup_complete(struct usbdev_ep_s *ep,
                                     struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
}

int usb_composite_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  int ret     = -1;
  int value   = 0;
  int new_req = 0;
  uint16_t w_value, w_length, w_Index;
  struct composite_driver_s *driv;
  struct composite_dev_s *usb_cdev;
  struct usbdev_req_s *usb_req;
  int i;

  if (driver == NULL || ctrl == NULL || dev == NULL)
    {
      return -1;
    }

  driv     = (struct composite_driver_s *)driver;
  usb_cdev = driv->dev;
  if (usb_cdev == NULL)
    {
      return -1;
    }

  w_value  = UGETW(ctrl->wValue);
  w_length = UGETW(ctrl->wLength);
  w_Index  = UGETW(ctrl->wIndex);

  usb_req = usb_cdev->ctrlreq;

  /* Update in the 'setup' function of a specific protocol, or use the default */
  usb_req->callback = composite_setup_complete;
  usb_req->len      = 0;

  switch (ctrl->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR:
      {
        if (ctrl->bmRequestType != USB_DIR_IN)
          {
            goto unknowned;
          }

        switch (w_value >> 8)
          {
          case UDESC_DEVICE:
            value = MIN(w_length, (UINT16)sizeof(struct usb_device_descriptor));
            composite_mkdevdesc(usb_cdev, usb_req->buf);
            modify_device_descriptor_byspeed(dev, usb_req->buf);
            usb_req->len = value;
            new_req      = 1;
            break;

          case UDESC_CONFIG:
            value = composite_mkcfgdesc(usb_cdev, usb_req->buf);
            if (value < 0)
              {
                break;
              }
            usb_req->len = value;
            (void)composite_get_config_descriptor(dev, usb_req, w_length);
            new_req = 1;
            break;

          case UDESC_STRING:
              value = composite_mkstrdesc(usb_cdev, (w_value & 0xff), w_Index, usb_req->buf);
              if (value < 0)
                {
                  break;
                }
              usb_req->len = MIN(w_length, value);
              new_req      = 1;
              break;

          case UDESC_BOS:
              usb_req->len = w_length;
              ret = memcpy_s(usb_req->buf, USB_COMP_EP0_BUFSIZ, (void *)g_bos_desc_buf, SKB_DATA_ALIGN(usb_req->len));
              if (ret < 0)
                {
                  return -1;
                }
              new_req = 1;
              break;

          default:
              break;
          }
      }
      break;

    case USB_REQ_SET_CONFIGURATION:
      {
        if (ctrl->bmRequestType != USB_DIR_OUT)
          {
            goto unknowned;
          }
        dprintf("SET_CONFIGURATION\n");
        /* Save the configuration and inform the constituent classes */
        for (i = 0; i < usb_cdev->ndevices; i++)
          {
            ret = CLASS_SETUP(usb_cdev->device[i].dev, dev, ctrl, dataout, outlen);
            if (ret < 0)
              {
                PRINT_ERR("set config fail!\n");
                return -1;
              }
          }

        /* the ret of usbclass_mass_set_alt is '1', the invoking of usbd_endpoint_request is fmass's protocal */
        if (ret == 0)
          {
            new_req = 1;
          }
        usb_req->len = 0;
        usb_cdev->config = w_value;
      }
      break;

    case USB_REQ_GET_CONFIGURATION:
      {
        dprintf("%s, USB_REQ_GET_CONFIGURATION\n", __func__);
        usb_req->len = 1;
        usb_req->buf[0] = 1;
        new_req = 1;
        break;
      }

    case USB_REQ_SET_INTERFACE:
      {
        if (ctrl->bmRequestType == USB_RECIP_INTERFACE &&
            usb_cdev->config == COMPOSITE_CONFIGID)
          {
            value = composite_classsetup(usb_cdev, dev, ctrl, dataout, outlen);
            new_req = 1;
          }
        break;
      }

    case USB_REQ_GET_STATUS:
      {
        if (ctrl->bmRequestType != (USB_DIR_IN | USB_RECIP_INTERFACE))
          {
            goto unknowned;
          }

        break;
      }

    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
      {
        if (ctrl->bmRequestType != (USB_DIR_OUT | USB_RECIP_INTERFACE))
          {
            goto unknowned;
          }

        break;
      }

    default:
unknowned:
      switch (ctrl->bmRequestType & USB_RECIP_MASK)
        {
        case USB_RECIP_INTERFACE:
        case USB_RECIP_ENDPOINT:
          (void)composite_classsetup(usb_cdev, dev, ctrl, dataout, outlen);
          break;

        default:
          break;
        }
      goto success;
    }

  if (new_req)
    {
      value = EP_SUBMIT(dev->ep0, usb_req);
      if (value < 0)
        {
          PRINT_ERR("endpoint send fail!\n");
          usb_req->result = 0;
          return -1;
        }
    }

success:
  return value;
}

static void usb_composite_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *priv;
  int i;

  /* Extract reference to private data */

  priv = ((struct composite_driver_s *)driver)->dev;
  if (priv == NULL)
    {
      return;
    }

  for (i = 0; i < priv->ndevices; i++)
    {
      CLASS_DISCONNECT(priv->device[i].dev, dev);
    }
}

static void usb_composite_suspend(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  (void)driver;
  (void)dev;
  PRINT_WARN("%s is not support now!\n", __FUNCTION__);
}

static void usb_composite_resume(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  (void)driver;
  (void)dev;
  PRINT_WARN("%s is not support now!\n", __FUNCTION__);
}

static struct usbdevclass_driverops_s g_composite_driverops =
{
  .bind       = usb_composite_bind,       /* bind */
  .unbind     = usb_composite_unbind,     /* unbind */
  .setup      = usb_composite_setup,      /* setup */
  .disconnect = usb_composite_disconnect, /* disconnect */
  .suspend    = usb_composite_suspend,    /* suspend */
  .resume     = usb_composite_resume,     /* resume */
};

/* probe composite device driver */
int usbd_composite_probe(struct composite_softc *cdev)
{
  int ret;

  if (cdev == NULL)
    {
      return -INVAL_ARGU;
    }

  /* probe and initiate device driver */
  ret = composite_device_init(cdev);
  if (ret < 0)
    {
      PRINT_ERR("composite device init failed!, ret:%d\n", ret);
      return -1;
    }

  return 0;
}

static int composite_probe(device_t dev)
{
  return 0;
}
extern struct driver_module_data fcdcacm_simple_driver_mod;
extern struct driver_module_data fconfig_simple_driver_mod;
int composite_device_init(struct composite_softc *cdev)
{
  int ret;
#ifndef FCOFNIG_FUN
  struct driver_module_data *mod = &fcdcacm_simple_driver_mod;
#else
  struct driver_module_data *mod = &fconfig_simple_driver_mod;
#endif
  if (mod->dmd_chainevh)
  	ret = mod->dmd_chainevh(NULL, MOD_LOAD, cdev);
  return 0;
}

static int composite_attach(device_t dev)
{
  struct composite_softc *usb_cdev = device_get_softc(dev);
  void *parnet_conext = (void *)usb_cdev;

  int ret;

  if (usb_cdev == NULL || parnet_conext == NULL)
    {
      PRINT_ERR("%s usb_cdev or parnet_conext is null!\n", __FUNCTION__);
      return -1;
    }

  /* probe gadget drivers */
  ret = usbd_composite_probe(usb_cdev);
  if (ret < 0)
    {
      return -1;
    }

  usb_cdev->drvr.dev = &usb_cdev->dev;
  usb_cdev->parnet_conext = parnet_conext;
#ifndef FCOFNIG_FUN
  usbd_start_udc();
  ret = usbd_gadget_attach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret < 0)
    {
      goto detach;
    }
#endif
  DPRINTF("composite attach success!\n");
  return 0; /* Attach success */
#ifndef FCOFNIG_FUN
detach:
  (void)composite_detach(dev);
  PRINT_ERR("%s failed, err=%d!\n", __FUNCTION__, ret);
  return -1; /* No such device or address */
#endif
}

void composite_uninitialize(void *handle);
static int composite_detach(device_t dev)
{
  struct composite_softc *usb_cdev = device_get_softc(dev);
  void *parnet_conext = device_get_softc(device_get_parent(dev));
  int ret;

  if (usb_cdev == NULL || parnet_conext == NULL)
    {
      return -1;
    }
  PRINT_ERR("composite_detach!!!!!!!!!!!!!\n");
  sleep(1);
  ret = usbd_gadget_detach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret != 0)
    {
      PRINT_ERR("%s failed, err=%d!\n", __FUNCTION__, ret);
      return ret;
    }
  composite_uninitialize(usb_cdev);

  DPRINTF("%s\n", __FUNCTION__);
  return (0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
/****************************************************************************
 * Name: composite_initialize
 *
 * Description:
 *   Register USB composite device as configured.  This function will call
 *   board-specific implementations in order to obtain the class objects for
 *   each of the members of the composite.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   A non-NULL "handle" is returned on success.  This handle may be used
 *   later with composite_uninitialize() in order to removed the composite
 *   device.  This handle is the (untyped) internal representation of the
 *   the class driver instance.
 *
 *   NULL is returned on any failure.
 *
 ****************************************************************************/

int composite_initialize(struct composite_softc *softc, uint8_t ndevices,
                         struct composite_devdesc_s *pdevices)
{
  struct composite_dev_s *priv;
  struct composite_driver_s *drvr;
  int ret;
  int i;

  /* Convenience pointers into the allocated blob */

  priv = &softc->dev;
  drvr = &softc->drvr;

  /* Initialize the USB composite driver structure */

  (void)memset_s(priv, sizeof(struct composite_dev_s), 0, sizeof(struct composite_dev_s));
  priv->cfgdescsize = 0;
  priv->ninterfaces = 0;

  /* Get the constituent class driver objects */

  for (i = 0; i < ndevices; i++)
    {
      (void)memcpy_s(&priv->device[i].compdesc, sizeof(struct composite_devdesc_s),
                     &pdevices[i], sizeof(struct composite_devdesc_s));
      ret = priv->device[i].compdesc.classobject(priv->device[i].compdesc.minor,
                                                 &priv->device[i].compdesc.devinfo,
                                                 &priv->device[i].dev);
      if (ret < 0)
        {
          return -1;
        }
      priv->cfgdescsize += priv->device[i].compdesc.cfgdescsize;
      priv->ninterfaces += priv->device[i].compdesc.devinfo.ninterfaces;
    }
  priv->ndevices = ndevices;

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_composite_driverops;
  drvr->dev        = priv;

  return 0;
}

void composite_uninitialize(void *handle)
{
  struct composite_softc *usb_cdev = (struct composite_softc *)handle;
  struct composite_dev_s *cdev = &usb_cdev->dev;
  int i;

  /* First phase uninitialization each of the member classes */

  for (i = 0; i < cdev->ndevices; i++)
    {
      cdev->device[i].compdesc.uninitialize(cdev->device[i].dev);
    }
}

struct composite_devobj_s *usbclass_devobj_get(struct composite_dev_s *cdev, device_type type)
{
  int i;
  struct composite_devdesc_s *compdesc;

  for (i = 0; i < NUM_DEVICES_TO_HANDLE; i++)
    {
      compdesc = &cdev->device[i].compdesc;
      if (compdesc->minor == type)
        {
          return &cdev->device[i];
        }
    }

  return NULL;
}

#undef USB_DEBUG_VAR
