/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS USB Driver Config Data Stream HeadFile
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

#ifndef LITEOS_USB_DEVICE_CONFIG_H
#define LITEOS_USB_DEVICE_CONFIG_H

#include "usb_obj.h"
#include "gadget/composite.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define FCONFIG_IOC_MAGIC                'c'
#define FCONFIG_CMD_MAKE_GADGET          _IO(FCONFIG_IOC_MAGIC, 1)
#define FCONFIG_CMD_DROP_GADGET          _IO(FCONFIG_IOC_MAGIC, 2)
#define FCONFIG_CMD_WRITE_DEV_DESC       _IO(FCONFIG_IOC_MAGIC, 3)
#define FCONFIG_CMD_ADD_CONFIG           _IO(FCONFIG_IOC_MAGIC, 4)
#define FCONFIG_CMD_REMOVE_CONFIG        _IO(FCONFIG_IOC_MAGIC, 5)
#define FCONFIG_CMD_WRITE_STRINGS        _IO(FCONFIG_IOC_MAGIC, 6)
#define FCONFIG_CMD_MAKE_FUNCTION        _IO(FCONFIG_IOC_MAGIC, 7)
#define FCONFIG_CMD_DROP_FUNCTION        _IO(FCONFIG_IOC_MAGIC, 8)
#define FCONFIG_CMD_ENABLE_UDC           _IO(FCONFIG_IOC_MAGIC, 9)
#define FCONFIG_CMD_DISABLE_UDC          _IO(FCONFIG_IOC_MAGIC, 10)
#define FCONFIG_CMD_CHAGE_DEVINFO        _IO(FCONFIG_IOC_MAGIC, 11)
#define FCONFIG_CMD_CHAGE_DEVSTRING      _IO(FCONFIG_IOC_MAGIC, 12)

/* USB_DT_DEVICE: Device descriptor */
#define USBDEV_CLASS "bDeviceClass"
#define USBDEV_SUBCLASS "bDeviceSubClass"
#define USBDEV_PROTOCOL "bDeviceProtocol"
#define USBDEV_MAXSIZE "bMaxPacketSize0"
#define USBDEV_MANUFACTURER "manufacturer"
#define USBDEV_PRODUCT "product"
#define USBDEV_SERIALNUM "serialnumber"
#define USBDEV_NUMCFG "numConfigurations"
#define USBDEV_BCD "bcdUSB"
#define USBDEV_VENDOR "idVendor"
#define USBDEV_IDPRODUCT "idProduct"
#define USBDEV_BCDDEVICE "bcdDevice"

struct fconfig_string {
  uint32_t                      len;
  char                          *s;
};

struct fconfig_usb_string {
  uint8_t                       id;
  struct fconfig_string         str;
};

struct fconfig_dev_strings {
  struct fconfig_string         gadget_name;
  uint16_t                      language;
  uint32_t                      str_count;
  struct fconfig_usb_string     *strings;
} __attribute__((packed));

struct fconfig_dev_desc {
  struct fconfig_string         gadget_name;
  struct usb_device_descriptor  dev_desc;
} __attribute__((packed));

struct fconfig_cfg_desc {
  struct fconfig_string         gadget_name;
  struct fconfig_string         config_name;
  struct usb_config_descriptor  cfg_desc;
} __attribute__((packed));

struct fconfig_func_info {
  struct fconfig_string         gadget_name;
  struct fconfig_string         config_name;
  struct fconfig_string         func_name;
};

struct fconfig_udc_info {
  struct fconfig_string         gadget_name;
  struct fconfig_string         udc_name;
};

struct usbdev_info {
  const char                    *name;
  device_type                   type;
  int                           ifnum;
  void (*initialize_sub)(struct composite_devdesc_s *dev, int ifnobase, int minor);
};

struct function_instance {
  struct usb_obj                obj;
  struct function_info          *func_info;
  struct usbdev_info            *dev_info;
  struct gadget_config          *cfg;
  char                          *name;
  int                           minor;
  int                           ifnobase;
};

struct gadget_config {
  struct usb_obj                obj;
  struct gadget_info            *gi;
  struct usb_config_descriptor  *cfg_desc;
  uint8_t                       cfg_num;
  char                          *name;
};

struct gadget_strings {
  struct usb_obj                obj;
  uint16_t                      language;
  struct usbd_string            *strings;
};

struct gadget_info {
  struct usb_obj                obj;
  const char                    *name;
  struct fconfig_softc          *cdev;
  char                          *udc_name;
  struct composite_softc        *com_s;
  device_t                      dev;
  struct usb_device_descriptor  *dev_desc;
  struct usb_obj                strings;
  uint32_t                      str_count;
  uint32_t                      lang_count;
};

struct fconfig_prop {
  const char  *prop_name;
  uint16_t    prop_value;
};

struct fconfig_propSting {
  uint16_t    lang;
  const char  *prop_name;
  const char  *prop_value;
};

struct fconfig_dev_desc_info {
  struct fconfig_string      gadget_name;
  struct fconfig_prop        prop;
};

struct fconfig_dev_desc_string {
  struct fconfig_string      gadget_name;
  struct fconfig_propSting   prop;
};

extern int fconfig_fops_init(struct fconfig_softc *cdev);
extern int fconfig_fops_deinit(const struct fconfig_softc *cdev);

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* LITEOS_USB_DEVICE_CONFIG_H */
