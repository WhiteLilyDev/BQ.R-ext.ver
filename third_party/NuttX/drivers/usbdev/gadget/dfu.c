/****************************************************************************
 * drivers/usbdev/dfu.c
 *
 *   Copyright (C) 2011-2018 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *
 *   Authors: Petteri Aimonen <jpa@git.mail.kapsi.fi>
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

/* This is a driver for the USB Device Firmware Upgrade protocol v1.1.
 * Currently it supports the app-side ("Run-Time") part of the protocol:
 * a sequence of DFU_DETACH and USB reset commands, which will reboot into
 * a separate USB DFU bootloader.
 *
 * The bootloader is provided by board-specific logic, or STM32's
 * built-in ROM bootloader can be used.
 *
 * https://www.usb.org/sites/default/files/DFU_1.1.pdf
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gadget/composite.h"
#include "gadget/f_dfu.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define DFU_MAX_TIMEOUT     255
#define DFU_MAX_TRANSFER    4096
#define DFU_VERSION         0x0110

/* All 16-bit values must be little-endian */

#define MSBYTE(u16)         ((u16) >> 8)     /* Get MS byte from uint16_t */
#define LSBYTE(u16)         ((u16) & 0xff)   /* Get LS byte from uint16_t */

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* DFU functional descriptor */

struct dfu_funcdesc_s
{
  uint8_t   len;                /* Descriptor length */
  uint8_t   type;               /* 0x21 = DFU FUNCTIONAL */
  uint8_t   attributes;         /* Bit mask of supported features */
  uint8_t   detach_timeout[2];  /* Maximum time in milliseconds between DFU_DETACH and USB reset */
  uint8_t   transfer_size[2];   /* Maximum number of bytes in control writes */
  uint8_t   dfu_version[2];     /* Version of DFU specification supported */
};

/* USB configuration descriptor */

struct dfu_cfgdesc_s
{
  struct usb_interface_descriptor ifdesc; /* DFU interface descriptor */
  struct dfu_funcdesc_s funcdesc;         /* DFU functional descriptor */
};

struct dfu_driver_s
{
  struct usbdevclass_driver_s drvr;
  FAR struct usbdev_req_s *ctrlreq;  /* Pointer to preallocated control request */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

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

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* USB driver operations */

const static struct usbdevclass_driverops_s g_dfu_driverops =
{
  &usbclass_bind,
  &usbclass_unbind,
  &usbclass_setup,
  &usbclass_disconnect,
  NULL,
  NULL
};

static struct dfu_cfgdesc_s g_dfu_cfgdesc =
{
  {
    .bLength            = sizeof(struct usb_interface_descriptor),
    .bDescriptorType    = UDESC_INTERFACE,
    .bInterfaceNumber   = 0,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 0,
    .bInterfaceClass    = 0xFE,
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = 0x01, /* DFU runtime protocol */
    .iInterface         = 0x01
  },
  {
    .len            = sizeof(struct dfu_funcdesc_s),
    .type           = 0x21,
    .attributes     = 0x0F,
    .detach_timeout = { LSBYTE(DFU_MAX_TIMEOUT), MSBYTE(DFU_MAX_TIMEOUT) },
    .transfer_size  = { LSBYTE(DFU_MAX_TRANSFER), MSBYTE(DFU_MAX_TRANSFER) },
    .dfu_version    = { LSBYTE(DFU_VERSION), MSBYTE(DFU_VERSION) }
  }
};

void to_runtime_mode(void)
{
  g_dfu_cfgdesc.ifdesc.bInterfaceProtocol = 0x01; /* The runtime mode */
}

void to_dfu_mode(void)
{
  g_dfu_cfgdesc.ifdesc.bInterfaceProtocol = 0x02; /* The DFU mode */
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void usbclass_ep0incomplete(FAR struct usbdev_ep_s *ep,
                                   FAR struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
}

static void usbclass_mkdevdesc(uint8_t *buf)
{
  usbdev_dfu_mkdevdesc(buf);
}

static int16_t usbclass_mkcfgdesc(FAR uint8_t *buf,
                                  FAR struct usbdev_devinfo_s *devinfo)
{
  int16_t total_len = 0;
  int16_t len       = USB_CONFIG_DESC_SIZE;
  int ret;

  /* Modify according to the actual length. */

  USETW(g_dfu_config_desc.wTotalLength, (USB_CONFIG_DESC_SIZE + sizeof(g_dfu_cfgdesc)));

  /* Copy DFU device configure descriptor. */

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_dfu_config_desc, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  /* Copy DFU device function descriptor. */

  buf += USB_CONFIG_DESC_SIZE;
  len  = sizeof(g_dfu_cfgdesc);
  ret  = memcpy_s(buf, (size_t)(USB_COMP_EP0_BUFSIZ - total_len), &g_dfu_cfgdesc, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  return total_len;
}

static int usbclass_mkstrdesc(uint8_t id, uint8_t *buf)
{
  return usbdev_dfu_mkstrdesc(id, buf);
}

static int usbclass_setup(FAR struct usbdevclass_driver_s *driver,
                          FAR struct usbdev_s *dev,
                          FAR const struct usb_device_request *ctrl,
                          FAR uint8_t *dataout, size_t outlen)
{
  FAR struct dfu_driver_s *priv = (FAR struct dfu_driver_s *)driver;
  FAR struct usbdev_req_s *ctrlreq = priv->ctrlreq;
  uint16_t value;
  uint16_t len;
  int ret = -EOPNOTSUPP;

  (void)dataout;
  (void)outlen;

  value = UGETW(ctrl->wValue);
  len   = UGETW(ctrl->wLength);

  if ((ctrl->bmRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD)
    {
      if (ctrl->bRequest == USB_REQ_SET_CONFIGURATION)
        {
          usbdev_dfu_set_config();
          return 0; /* Composite driver will send the reply */
        }
      else if (ctrl->bRequest == USB_REQ_SET_INTERFACE)
        {
          /* Only one alternate setting (0) is supported */

          if (value == 0)
            {
              ret = 0;
            }
        }
      else if (ctrl->bRequest == USB_REQ_GET_INTERFACE)
        {
          *(FAR uint8_t *)ctrlreq->buf = 0;
          ret = 1;
        }
    }
  else if ((ctrl->bmRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS)
    {
      ret = usbdev_dfu_class_requests(dev, ctrl, ctrlreq);
    }

  /* Respond to the setup command if data was returned.  On an error return
   * value (ret < 0), the USB driver will stall.
   */

  if (ret >= 0)
    {
      ctrlreq->len   = (len < ret) ? len : (uint32_t)ret;
      ctrlreq->flags = USBDEV_REQFLAGS_NULLPKT;
      ret            = EP_SUBMIT(dev->ep0, ctrlreq);
      if (ret < 0)
        {
          usb_err("req submit failed!\r\n");
          ctrlreq->result = OK;
        }
    }

  return ret;
}

static int usbclass_bind(FAR struct usbdevclass_driver_s *driver,
                         FAR struct usbdev_s *dev)
{
  FAR struct dfu_driver_s *priv = (FAR struct dfu_driver_s *)driver;
  struct composite_dev_s *cdev  = dev->ep0->priv;

  priv->ctrlreq = cdev->ctrlreq;
  if (priv->ctrlreq == NULL)
    {
      usb_err("Ctrlreq is NULL!\r\n");
      return -ENOMEM;
    }

  priv->ctrlreq->callback = usbclass_ep0incomplete;

  return OK;
}

static int usbclass_unbind(FAR struct usbdevclass_driver_s *driver,
                           FAR struct usbdev_s *dev)
{
  FAR struct dfu_driver_s *priv = (FAR struct dfu_driver_s *)driver;

  (void)dev;

  if (usb_dfu_running())
    {
      PRINT_ERR("%s fail, dfu busy\n", __FUNCTION__);
      return -1;
    }

  if (priv->ctrlreq != NULL)
    {
      priv->ctrlreq = NULL;
    }
  usbdev_dfu_init();

  return 0;
}

static void usbclass_disconnect(FAR struct usbdevclass_driver_s *driver,
                                FAR struct usbdev_s *dev)
{
  (void)driver;
  (void)dev;

  usbdev_dfu_transaction_cleanup();
  dprintf("reset config\n");
}

/****************************************************************************
 * Name: usbclass_classobject
 *
 * Description:
 *   Allocate memory for the RNDIS driver class object
 *
 * Returned Value:
 *   0 on success, negative error code on failure.
 *
 ****************************************************************************/

static int usbclass_classobject(int minor,
                                FAR struct usbdev_devinfo_s *devinfo,
                                FAR struct usbdevclass_driver_s **classdev)
{
  FAR struct dfu_driver_s *alloc;

  (void)minor;
  (void)devinfo;

  alloc = malloc(sizeof(struct dfu_driver_s));
  if (alloc == NULL)
    {
      return -ENOMEM;
    }

  *classdev = &alloc->drvr;

  alloc->drvr.speed = USB_SPEED_FULL;
  alloc->drvr.ops   = &g_dfu_driverops;

  usbdev_dfu_dev_init();

  return OK;
}

/****************************************************************************
 * Name: usbclass_uninitialize
 *
 * Description:
 *   Free allocated memory
 *
 * Returned Value:
 *   0 on success, negative error code on failure.
 *
 ****************************************************************************/

static void usbclass_uninitialize(FAR struct usbdevclass_driver_s *classdev)
{
  usbdev_dfu_dev_deinit();
  free(classdev);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usbdev_dfu_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc           = usbclass_mkdevdesc;
  dev->mkconfdesc          = usbclass_mkcfgdesc;
  dev->mkstrdesc           = usbclass_mkstrdesc;
  dev->classobject         = usbclass_classobject;
  dev->uninitialize        = usbclass_uninitialize;
  dev->nconfigs            = 1;
  dev->configid            = 0;
  dev->cfgdescsize         = (int)sizeof(g_dfu_cfgdesc);
  dev->devinfo.ninterfaces = 1;
  dev->devinfo.nstrings    = 1;
  dev->devinfo.nendpoints  = 0;
}
