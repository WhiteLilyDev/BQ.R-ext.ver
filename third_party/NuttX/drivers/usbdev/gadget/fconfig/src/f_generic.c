/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: LiteOS USB Driver Generic Function
 * Author: Yannik Li
 * Create: 2021-02-04
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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <los_event.h>
#include <fs/fs.h>
#include <los_magickey.h>
#include <los_hwi.h>
#include <linux/spinlock.h>
#include "osal_atomic.h"
#include "gadget/usbdev.h"
#include "f_common.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef sizeof_field
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))
#endif

static int usbclass_generic_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_generic_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_generic_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                                  const struct usb_device_request *ctrl, uint8_t *dataout,
                                  size_t outlen);
static void usbclass_generic_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

struct usbdevclass_driverops_s g_generic_driverops =
{
  usbclass_generic_bind,
  usbclass_generic_unbind,
  usbclass_generic_setup,
  usbclass_generic_disconnect,
  NULL,
  NULL
};

struct usb_config_descriptor g_generic_config_desc =
{
  .bLength             = USB_CONFIG_DESC_SIZE,
  .bDescriptorType     = UDESC_CONFIG,
  .wTotalLength        = {0, 0}, /* dynamic */
  .bNumInterface       = 1,
  .bConfigurationValue = 1,
  .iConfiguration      = 0,
  .bmAttributes        = UC_SELF_POWERED | UC_BUS_POWERED,
  .bMaxPower           = 1, /* max power */
};

static uint64_t g_generic_minor = 0;
static struct usb_obj g_generic_obj;
EVENT_CB_S eps_event_all;
uint32_t eps_event_mask = 0;

LINUX_LIST_HEAD(g_generic_devices);
DEFINE_SPINLOCK(g_generic_spinlock);
static inline void generic_dev_lock(void)
{
  spin_lock(&g_generic_spinlock);
}

static inline void generic_dev_unlock(void)
{
  spin_unlock(&g_generic_spinlock);
}

static inline uint32_t generic_ffz64(uint64_t x)
{
  uint32_t num = 0;

  if ((x & 0xffffffff) == 0xffffffff)
    {
      num += 32;
      x >>= 32;
    }
  if ((x & 0xffff) == 0xffff)
    {
      num += 16;
      x >>= 16;
    }
  if ((x & 0xff) == 0xff)
    {
      num += 8;
      x >>= 8;
    }
  if ((x & 0xf) == 0xf)
    {
      num += 4;
      x >>= 4;
    }
  if ((x & 0x3) == 0x3)
    {
      num += 2;
      x >>= 2;
    }
  if ((x & 0x1) == 0x1)
    {
      num += 1;
    }
  return num;
}

static void generic_discard_events(struct generic_dev_s *dev,
                                   enum usb_generic_event_type type1,
                                   enum usb_generic_event_type type2,
                                   int keep)
{
  uint32_t ev = dev->efifo.out, out = ev;
  uint32_t mask = USB_GENERIC_EVENTS_NUM - 1;

  for (; ev < dev->efifo.in; ++ev)
    {
      if ((dev->efifo.data[ev & mask] == type1 ||
         dev->efifo.data[ev & mask] == type2) == keep)
        {
          dev->efifo.data[out++ & mask] = dev->efifo.data[ev & mask];
        }
      else
        {
          usb_debug("remove event %d\n", dev->efifo.data[ev & mask]);
        }
    }
  dev->efifo.in = out;
}

static void __generic_event_add(struct generic_dev_s *dev,
                                enum usb_generic_event_type type)
{
  enum usb_generic_event_type ev_type1, ev_type2 = type;
  int keep = 0;

  if (dev->setup_state == GENERIC_SETUP_PENDING)
    {
      dev->setup_state = GENERIC_SETUP_CANCELLED;
    }

  switch (type)
    {
    case GENERIC_RESUME:
      /* remove previous GENERIC_SUSPEND */
      ev_type2 = GENERIC_SUSPEND;
    case GENERIC_SUSPEND:
    case GENERIC_SETUP:
      /* remove repeated event */
      ev_type1 = type;
      break;
    case GENERIC_BIND:
    case GENERIC_UNBIND:
    case GENERIC_DISABLE:
    case GENERIC_ENABLE:
      /* just keep GENERIC_SUSPEND and GENERIC_RESUME */
      ev_type1 = GENERIC_SUSPEND;
      ev_type2 = GENERIC_RESUME;
      keep = 1;
      break;

    default:
      usb_err("%d: unknown event\n", type);
      return;
    }

  generic_discard_events(dev, ev_type1, ev_type2, keep);
  usb_debug("adding event %d\n", type);
  event_fifo_put(&dev->efifo, type);
  int bits = dev->minor_offset * EVENT_FOR_FUNCTION;
  (void)LOS_EventWrite(&eps_event_all, (1 << bits));
}

static void generic_event_add(struct generic_dev_s *dev,
                              enum usb_generic_event_type type)
{
  unsigned long flags;
  spin_lock_irqsave(&dev->event_lock, flags);
  __generic_event_add(dev, type);
  spin_unlock_irqrestore(&dev->event_lock, flags);
}

static inline void generic_dev_get(struct generic_dev_s *dev)
{
  usbobj_get(&dev->obj);
}

static inline void generic_dev_put(struct generic_dev_s *dev)
{
  usbobj_put(&dev->obj);
}

static struct generic_dev_s *generic_dev_find_by_name(const char *name)
{
  struct usb_obj *obj = NULL;
  if (name == NULL)
    {
      usb_err("%s: name is NULL\n", __FUNCTION__);
      return NULL;
    }

  obj = usbobj_find(&g_generic_obj, usbobj_default_match, (void *)name);
  if (obj == NULL)
    {
      usb_err("%s is not exists!\n", name);
      return NULL;
    }

  return container_of(obj, struct generic_dev_s, obj);
}

static int usbobj_minor_match(struct usb_obj *obj, void *match_data)
{
  struct generic_dev_s *dev = container_of(obj, struct generic_dev_s, obj);

  return (dev->minor == *(int *)match_data);
}

static struct generic_dev_s *__generic_dev_find_by_minor(int minor)
{
  struct usb_obj *obj = NULL;

  obj = usbobj_find(&g_generic_obj, usbobj_minor_match, &minor);
  if (obj == NULL)
    {
      usb_err("%d not found!\n", minor);
      return NULL;
    }

  return container_of(obj, struct generic_dev_s, obj);
}

static struct generic_dev_s *generic_dev_find_by_minor(int minor)
{
  struct generic_dev_s *dev;

  generic_dev_lock();
  dev = __generic_dev_find_by_minor(minor);
  generic_dev_unlock();
  return dev;
}

static void *generic_acquire_dev(int minor)
{
  struct generic_dev_s *dev = NULL;

  generic_dev_lock();
  dev = __generic_dev_find_by_minor(minor);
  if (!dev || dev->inuse)
    {
      generic_dev_unlock();
      return NULL;
    }
  else
    {
      dev->inuse = false;
    }
  generic_dev_unlock();

  return dev;
}

static struct generic_dev_s *generic_dev_new(const char *name)
{
  struct generic_dev_s *dev = NULL;
  static bool is_eevnt_initialized = false;
  int ret;

  if (g_generic_minor == (uint64_t)-1)
    {
      usb_err("%s: generic device is more than 64\n", __FUNCTION__);
      return NULL;
    }

  dev = (struct generic_dev_s *)usbm_malloc(&g_generic_obj, sizeof(*dev));
  if (!dev)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return NULL;
    }
  (void)memset_s(dev, sizeof(*dev), 0, sizeof(*dev));
  (void)usbobj_init(&dev->obj, name, usbobj_default_release);
  (void)usbobj_init(&dev->memory_obj, name, NULL);
  (void)usbobj_init(&dev->epfiles_obj, name, NULL);
  spin_lock_init(&dev->event_lock);
  spin_lock_init(&dev->eps_lock);
  spin_lock_init(&dev->ep0_lock);

  /* initialize the generic device */
  ret = generic_ffz64(g_generic_minor);
  g_generic_minor |= (1 << ret);
  dev->minor = DEV_GENERIC + ret;
  dev->minor_offset = ret;
  dev->name  = usbm_strdup(&dev->obj, name);
  dev->event_mask = 0x0;
  (VOID)LOS_EventInit(&dev->ep_event_all);
  if (!is_eevnt_initialized)
    {
      (VOID)LOS_EventInit(&eps_event_all);
      is_eevnt_initialized = true;
    }
  eps_event_mask |= (1 << (dev->minor_offset * EVENT_FOR_FUNCTION));
  atomic_set(&dev->opened, 0);
  dev->eps_enbale = false;
  dev->state = GENERIC_READ_DESCRIPTORS;
  usbobj_add(&dev->obj, &g_generic_obj);
  generic_dev_opened(dev);
  return dev;
}

static void generic_dev_clear(struct generic_dev_s *dev)
{
  usbobj_put(&dev->epfiles_obj);
  objres_release_all(&dev->obj);
}

static void generic_dev_reset(struct generic_dev_s *dev)
{

  generic_dev_clear(dev);
  /* initialize descriptors related variables */
  dev->raw_descs_data   = NULL;
  dev->raw_descs        = NULL;
  dev->raw_strings      = NULL;
  dev->raw_descs_length = 0;
  dev->fs_descs_count   = 0;
  dev->hs_descs_count   = 0;
  dev->ss_descs_count   = 0;
  dev->strings_count    = 0;
  dev->interfaces_count = 0;
  dev->eps_count        = 0;
  dev->alt_setting      = 0;
  /* initialize the state of device */
  dev->state = GENERIC_READ_DESCRIPTORS;
  dev->setup_state = GENERIC_NO_SETUP;
  dev->flags = 0;
}

void generic_dev_opened(struct generic_dev_s *dev)
{
  generic_dev_get(dev);
  if ((atomic_add_return(1, &dev->opened) == 1) &&
      (dev->state == GENERIC_DEACTIVATED))
    {
      dev->state = GENERIC_CLOSING;
      generic_dev_reset(dev);
    }
}

void generic_dev_closed(struct generic_dev_s *dev)
{
  /* set state to closing when opened flag is 0 */
  if (atomic_dec_and_test(&dev->opened))
    {
      dev->state = GENERIC_CLOSING;
      generic_dev_reset(dev);
    }

  /* set state to closing when opened flag is negative */
  if (atomic_read(&dev->opened) < 0)
    {
      dev->state = GENERIC_CLOSING;
      generic_dev_reset(dev);
    }

  /* decress the reference value and release the device
   * if there is no reference.
   */
  generic_dev_put(dev);
}

static void generic_eps_free(struct generic_dev_s *priv)
{
  struct generic_ep *ep = priv->eps;
  uint16_t count = priv->eps_count;
  unsigned long flags;

  spin_lock_irqsave(&priv->eps_lock, flags);
  while (count--)
    {
      /* free usb request of the endpoint */
      if (ep->ep && ep->req)
        {
          EP_FREEREQ(ep->ep, ep->req);
        }
      ep->req = NULL;
      /* free endpoint */
      if (ep->ep && priv->usbdev)
        {
          DEV_FREEEP(priv->usbdev, ep->ep);
          ep->ep = NULL;
        }
      ++ep;
    }
  spin_unlock_irqrestore(&priv->eps_lock, flags);
  usbm_free(&priv->obj, priv->eps);
  priv->eps = NULL;
}

static int generic_eps_alloc(struct generic_dev_s *priv)
{
  struct usbdev_req_s *req = NULL;
  uint16_t eps_count = priv->eps_count;
  size_t size = sizeof(struct generic_ep) * eps_count;
  uint16_t i, j;

  priv->eps = (struct generic_ep *)usbm_zalloc(&priv->obj, size);
  if (priv->eps == NULL)
    {
      usb_err("%s: allocate eps failed", __FUNCTION__);
      return -1;
    }

  for (i = 0; i < eps_count; i++)
    {
      usb_endpoint_descriptor_t *desc;
      struct usbdev_ep_s *ep;
      for (j = 0; j < 3; j++)
        {
          priv->eps[i].descs[j] = priv->eps_descs[i][j];
        }
      switch (priv->speed)
        {
        case USB_SPEED_SUPER:
          desc = priv->eps[i].descs[2];
          break;
        case USB_SPEED_HIGH:
          desc = priv->eps[i].descs[1];
          break;
        default:
          desc = priv->eps[i].descs[1];
          break;
        }

      ep = DEV_ALLOCEP(priv->usbdev, desc->bEndpointAddress, desc);
      if (ep == NULL)
        {
          usb_err("%s: alloc ep %u failed", __FUNCTION__, desc->bEndpointAddress);
          goto err;
        }
      priv->eps[i].ep  = ep;
      priv->eps[i].num = ep->eplog;
      priv->devinfo->epno[i] = ep->eplog;

      req = EP_ALLOCREQ(ep);
      if (req == NULL)
        {
          usb_err("%s: alloc request for ep %u failed",
                  __FUNCTION__, desc->bEndpointAddress);
          goto err;
        }

      priv->eps[i].req = req;
      (VOID)LOS_EventInit(&priv->eps[i].event);
      priv->eps_revmap[ep->eplog & USB_ENDPOINT_NUMBER_MASK] = i + 1;
    }
  return 0;

err:
  generic_eps_free(priv);
  return -1;
}

static int usbclass_generic_bind(struct usbdevclass_driver_s *driver,
                                 struct usbdev_s *dev)
{
  struct generic_driver_s *drvr = (struct generic_driver_s *)driver;
  struct composite_dev_s *cdev = NULL;
  struct generic_dev_s *priv = NULL;
  struct composite_devobj_s *devobj = NULL;
  int ret;

  if (drvr == NULL || dev == NULL)
    {
      usb_err("%s: invalid paramter\n", __FUNCTION__);
      return -1;
    }

  cdev = dev->ep0->priv;
  priv = drvr->dev;
  if (priv == NULL || priv->state != GENERIC_ACTIVE)
    {
      usb_err("%s: priv is NULL or state is error\n", __FUNCTION__);
      return -1;
    }
  if (OsalTestSetBit(GENERIC_FL_BOUND, &priv->flags))
    {
      usb_err("%s: the generic device is bound\n", __FUNCTION__);
      return -1;
    }
  priv->usbdev = dev;
  devobj = usbclass_devobj_get(cdev, priv->minor);
  if (devobj == NULL)
    {
      usb_err("%s: devobj is NULL\n", __FUNCTION__);
      return -1;
    }
  priv->devinfo = &devobj->compdesc.devinfo;

  /* initialize control request */
  priv->ctrlreq = cdev->ctrlreq;
  if (priv->ctrlreq == NULL)
    {
      usb_err("Ctrlreq is NULL!\r\n");
      return -ENOMEM;
    }
  (VOID)LOS_EventInit(&priv->ctrlreq_event);
  (VOID)LOS_EventInit(&priv->ep0_event);
  /* allocate endpoints and conresponding reqeusts */
  priv->speed = USB_SPEED_HIGH;
  ret = generic_eps_alloc(priv);
  if (ret != 0)
    {
      usb_err("%s: eps allocated error\n", __FUNCTION__);
      return -1;
    }
  /* send bind event to user */
  generic_event_add(priv, GENERIC_BIND);

  return 0;
}

static int usbclass_generic_unbind(struct usbdevclass_driver_s *driver,
                                   struct usbdev_s *dev)
{
  struct generic_driver_s *drvr = NULL;
  struct generic_dev_s *priv = NULL;

  if (driver == NULL || dev == NULL)
    {
      usb_err("%s: invalid parameter\n", __FUNCTION__);
      return -1;
    }

  drvr = (struct generic_driver_s *)driver;
  priv = drvr->dev;
  if (priv == NULL)
    {
      usb_err("%s: priv is NULL\n", __FUNCTION__);
      return -1;
    }
  if (!OsalTestClearBit(GENERIC_FL_BOUND, &priv->flags))
    {
      usb_err("%s: the generic device is unbound\n", __FUNCTION__);
      return -1;
    }

  generic_eps_disable(priv);
  generic_eps_free(priv);
  generic_event_add(priv, GENERIC_UNBIND);
  return 0;
}

static int generic_revmap_ep(struct generic_dev_s *dev, uint8_t num)
{
  num = dev->eps_revmap[num & USB_ENDPOINT_NUMBER_MASK];
  return num ? num : -1;
}

static int generic_revmap_intf(struct generic_dev_s *dev, uint8_t intf)
{
  return intf - dev->devinfo->ifnobase;
}

static int generic_set_alt(struct generic_dev_s *dev,
                           uint32_t interface,  uint32_t alt)
{
  int ret = 0, intf;

  if (alt != (uint32_t)-1)
    {
      intf = generic_revmap_intf(dev, interface);
      if (intf < 0)
        {
          return intf;
        }
    }

  if (dev->alt_setting != alt)
    {
      generic_eps_disable(dev);
    }

  if (alt == (uint32_t)-1)
    {
      generic_event_add(dev, GENERIC_DISABLE);
      return 0;
    }
  if (dev->eps_enbale)
    {
      generic_eps_disable(dev);
    }
  ret = generic_eps_enable(dev);
  if (ret >= 0)
    {
      generic_event_add(dev, GENERIC_ENABLE);
      dev->alt_setting = alt;
    }
  return ret;
}

static int generic_set_config(struct generic_dev_s *dev)
{
  int i, ret;

  for (i = 0; i < 1; i++)
    {
      ret = generic_set_alt(dev, dev->devinfo->ifnobase + i, 0);
      if (ret < 0)
        {
          usb_err("%s: interface %d set alt failed\n", __FUNCTION__, i);
          return -1;
        }
    }
  return 0;
}

static void use_tw(uWord w, int v)
{
  w[0] = (uint8_t)(v);
  w[1] = (uint8_t)((v) >> 8);
}

static int usbclass_generic_setup(struct usbdevclass_driver_s *driver,
                                  struct usbdev_s *dev,
                                  const struct usb_device_request *ctrl,
                                  uint8_t *dataout, size_t outlen)
{
  uint16_t w_value;
  uint16_t w_index;
  struct generic_dev_s *priv = NULL;
  struct generic_driver_s *drvr = NULL;
  unsigned long flags;
  int ret = -1;

  if (dev == NULL || driver == NULL || ctrl == NULL)
    {
      usb_err("%s: invalid parameter\n", __FUNCTION__);
      return -1;
    }

  drvr = (struct generic_driver_s *)driver;
  priv = drvr->dev;
  if (priv == NULL || priv->state != GENERIC_ACTIVE)
    {
      usb_err("%s: priv is NULL or state is incorrect\n", __FUNCTION__);
      return -1;
    }

  w_index = UGETW(ctrl->wIndex);
  w_value = UGETW(ctrl->wValue);

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
              ret = generic_set_config(priv);
            }
          return ret;

        case USB_REQ_SET_INTERFACE:
          if (ctrl->bmRequestType == 0)
            {
              ret = generic_set_alt(priv, w_index, w_value);
            }
          return ret;

        default:
          break;
        }
    }

  /* process class or vendor specific requests */
  switch (UT_GET_RECIPIENT(ctrl->bmRequestType))
    {
    case USB_RECIP_INTERFACE:
      ret = generic_revmap_intf(priv, w_index);
      if (ret < 0)
        {
          return ret;
        }
      break;

    case USB_RECIP_ENDPOINT:
      ret = generic_revmap_ep(priv, w_index);
      if (ret < 0)
        {
          return ret;
        }
      break;

    default:
      if (priv->flags & GENERIC_ALL_CTRL_RECIP)
        {
          ret = w_index;
        }
      else
        {
          return -1;
        }
    }

  spin_lock_irqsave(&priv->event_lock, flags);
  priv->setup = *ctrl;
  use_tw(priv->setup.wIndex, ret);
  __generic_event_add(priv, GENERIC_SETUP);
  spin_unlock_irqrestore(&priv->event_lock, flags);

  return 0;
}

static void usbclass_generic_disconnect(struct usbdevclass_driver_s *driver,
                                        struct usbdev_s *dev)
{
  struct generic_dev_s *priv = NULL;

  (void)dev;
  if (driver == NULL)
    {
      usb_err("driver pointer is NULL!\n");
      return;
    }

  priv = ((struct generic_driver_s *)driver)->dev;
  if (priv == NULL)
    {
      usb_err("No device!\n");
      return;
    }

  (void)generic_set_alt(priv, 0, (uint32_t)-1);
}

static void generic_mkdevdesc(uint8_t *buf)
{
  (void)buf;

  /* f_generic does not provide device descriptor */
  return;
}

static int16_t generic_mkcfgdesc(uint8_t *buf,
                                 struct usbdev_devinfo_s *devinfo)
{
  struct composite_devobj_s *device = container_of(devinfo,
                                      struct composite_devobj_s,
                                      compdesc.devinfo);
  struct generic_driver_s *drvr = (struct generic_driver_s *)device->dev;
  struct generic_dev_s *dev = drvr->dev;
  int16_t total_len = 0;
  int16_t len = USB_CONFIG_DESC_SIZE;
  errno_t ret;

  g_generic_config_desc.bNumInterface = devinfo->ninterfaces;

  /* Copy device configure descriptor. */

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ,
                 (const void *)&g_generic_config_desc,
                 (uint32_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  /* Copy device function descriptor. */

  len = dev->raw_descs_length;
  ret = memcpy_s(buf, (USB_COMP_EP0_BUFSIZ - total_len),
                 (const void *)dev->raw_descs, (uint32_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  return (int16_t)total_len;
}

static int generic_mkstrdesc(uint8_t id, uint8_t *buf)
{
  struct generic_dev_s *dev = NULL;
  int minor = (int)*buf;
  struct usb_string_descriptor *s = ( struct usb_string_descriptor *)buf;
  errno_t ret;
  int i;

  dev = generic_dev_find_by_minor(minor);
  if (!dev)
    {
      return -1;
    }

  for (i = 0; dev->dev_strings->strings[i].s != NULL; i++)
    {
      const char *str = dev->dev_strings->strings[i].s;
      uint32_t len = strlen(str);
      if (dev->dev_strings->strings[i].id == id)
        {
          ret = utf8_to_utf16le(str, (uint8_t *)(buf + 2), len);
          if (ret <= 0)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          s->bLength = 2 + (ret * 2);
          s->bDescriptorType = UDESC_STRING;
          return s->bLength;
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

#define GENERIC_NCONFIGS    1
#define GENERIC_CONFIGID    0
static void generic_get_composite_devdesc(struct composite_devdesc_s *dev,
                                          int minor)
{
  struct generic_dev_s *priv = NULL;

  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0,
                 sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = generic_mkdevdesc;
  dev->mkconfdesc = generic_mkcfgdesc;
  dev->mkstrdesc  = generic_mkstrdesc;

  dev->nconfigs = GENERIC_NCONFIGS; /* Number of configurations supported */
  dev->configid = GENERIC_CONFIGID; /* The only supported configuration ID */

  /* get generic deivce by minor */
  priv = generic_dev_find_by_minor(minor);
  if (priv == NULL)
    {
      usb_err("%s: can't find generic dev %d", __FUNCTION__, minor);
      return;
    }

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */
  dev->devinfo.ninterfaces = priv->interfaces_count;

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = priv->strings_count;

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = priv->eps_count;
}

static int generic_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                               struct usbdevclass_driver_s **classdev)
{
  struct generic_dev_s *dev = NULL;
  struct generic_driver_s *drvr = NULL;

  (void)devinfo;

  /* Acquire the USB generic driver object */

  dev = generic_acquire_dev(minor);
  if (dev == NULL)
    {
      usb_err("%s: acquire the generic driver failed\n", __FUNCTION__);
      return -1;
    }

  /* Allocate the structures needed */

  drvr = (struct generic_driver_s *)usbm_malloc(&dev->obj, sizeof(*drvr));
  if (drvr == NULL)
    {
      dev->inuse = false;
      return -1;
    }
  memset_s(drvr, sizeof(*drvr), 0, sizeof(*drvr));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_generic_driverops;
  drvr->dev        = dev;

  *classdev = &drvr->drvr;
  return 0;
}

static void generic_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct generic_driver_s *generic_drvr = (struct generic_driver_s *)classdev;
  struct generic_dev_s *priv = NULL;

  if (generic_drvr == NULL || generic_drvr->dev == NULL)
    {
      usb_err("%s: invalid parameter\n", __FUNCTION__);
      return;
    }

  priv = generic_drvr->dev;
  generic_dev_put(priv);
}

static void generic_obj_release_dev(struct generic_dev_s *dev)
{
  if (dev)
    {
      usbobj_remove(&dev->obj);
      g_generic_minor &= ~(1 << dev->minor_offset);
    }
}

int usbdev_generic_alloc_instance(const char *name)
{
  static bool is_initialized = false;
  struct generic_dev_s *dev = NULL;
  struct usb_obj *obj = NULL;
  int ret;

  if (!name || (strlen(name) == 0) ||
      (strlen(name) > GENERIC_NAME_LEN))
    {
      return -1;
    }

  if (!is_initialized)
    {
      (void)usbobj_init(&g_generic_obj, "generic", NULL);
      is_initialized = true;
    }

  /* check the device is exist or not */
  obj = usbobj_find(&g_generic_obj, usbobj_default_match, (void *)name);
  if (obj != NULL)
    {
      usb_err("%s already exists!\n", name);
      return -1;
    }

  dev = generic_dev_new(name);
  if (dev == NULL)
    {
      return -1;
    }

  ret = generic_create_ep0(dev);
  if (ret)
    {
      generic_dev_put(dev);
      return -1;
    }

  return dev->minor;
}

int usbdev_generic_free_instance(const char *name)
{
  struct generic_dev_s *dev = NULL;
  int ret;

  if (!name || (strlen(name) == 0))
    {
      usb_err("%s: invalid parameter\n", __FUNCTION__);
      return -1;
    }

  dev = generic_dev_find_by_name(name);
  if (!dev)
    {
      usb_err("%s does not exist!\n", name);
      return -1;
    }

  /* remove ep0 device node */
  ret = generic_remove_ep0(dev);
  if (ret)
    {
      return -1;
    }
  ret = generic_remove_epfiles(dev);
  if (ret)
    {
      return -1;
    }

  generic_obj_release_dev(dev);
  generic_dev_closed(dev);

  return 0;
}

void usbdev_generic_initialize_sub(struct composite_devdesc_s *dev,
                                   int ifnobase, int minor)
{
  //int ret;
  generic_get_composite_devdesc(dev, minor);

  /* Overwrite and correct some values... */

  dev->classobject  = generic_classobject;
  dev->uninitialize = generic_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase;  /* Offset to Interface-IDs */
  dev->minor            = minor;     /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;          /* Offset to String Numbers */
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
