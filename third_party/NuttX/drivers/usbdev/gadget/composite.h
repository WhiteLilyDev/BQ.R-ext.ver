/****************************************************************************
 * drivers/usbdev/composite.h
 *
 *   Copyright (C) 2011-2012, 2017 Gregory Nutt. All rights reserved.
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

#ifndef __COMPOSITE_H__
#define __COMPOSITE_H__

#include "gadget/usbdev.h"
#include "implementation/usb_init.h"
#include "osal_atomic.h"
#include <linux/kernel.h>

#define USB_COMP_EP0_BUFSIZ        4096U
#define USB_DWC_U2_MAX_PACKET_SIZE 512
#define USB_DWC_MAX_PACKET_SIZE    1024

#define USB_COMPOSITE_DEV_NUM      2

#define HSETW(ptr, val)  ptr = { (uint8_t)(val), (uint8_t)((val) >> 8) }
#define HSETDW(ptr, val) ptr = { (uint8_t)(val), (uint8_t)((val) >> 8), \
                                 (uint8_t)((val) >> 16), (uint8_t)((val) >> 24) }

/* This is a flag that device strings struct is ended. */

#define USBD_DEVICE_STRINGS_END { 0, NULL }

#define NO_DEV                    ENODEV
#define INVAL_ARGU                EINVAL
#define MAX_CONFIG_INTERFACES_NUM 16 /* arbitrary; max 255 */

/* predefined index for usb_composite_driver */

enum
{
  USBD_GADGET_MANUFACTURER_IDX = 0,
  USBD_GADGET_PRODUCT_IDX,
  USBD_GADGET_SERIAL_IDX,
  USBD_GADGET_FIRST_AVAIL_IDX,
};

struct usbd_string
{
  u8          id;
  const char  *s;
};

#define NUM_DEVICES_TO_HANDLE         (4)

/* Descriptors **************************************************************/
/* These settings are not modifiable via the NuttX configuration */

#define COMPOSITE_CONFIGIDNONE        (0)  /* Config ID = 0 means to return to address mode */
#define COMPOSITE_CONFIGID            (1)  /* The only supported configuration ID */

/* String language */

#define COMPOSITE_STR_LANGUAGE        (0x0409) /* en-us */

/* Descriptor strings */

#define COMPOSITE_MANUFACTURERSTRID   (1)
#define COMPOSITE_PRODUCTSTRID        (2)
#define COMPOSITE_SERIALSTRID         (3)
#define COMPOSITE_CONFIGSTRID         (4)

#ifndef MIN
#  define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

struct composite_devobj_s
{
  /* Device description given by the user code in the dynamic
   * configuration.
   */

  struct composite_devdesc_s compdesc;

  /* Pointer to device class */

  struct usbdevclass_driver_s *dev;
};

/* This structure describes the internal state of the driver */

struct composite_dev_s
{
  struct usbdev_s      *usbdev;      /* usbdev driver pointer */
  uint8_t               config;      /* Configuration number */
  struct usbdev_req_s  *ctrlreq;     /* Allocated control request */
  uint8_t               ndevices;    /* Num devices in this composite device */
  int                   cfgdescsize; /* Total size of the configuration descriptor: */
  int                   ninterfaces; /* The total number of interfaces in this composite device */

  struct composite_devobj_s device[NUM_DEVICES_TO_HANDLE]; /* Device class object */
};

/* The internal version of the class driver */

struct composite_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct composite_dev_s     *dev;
};

/* This structure describes the internal state of the driver */

struct composite_softc
{
  struct composite_dev_s    dev;
  struct composite_driver_s drvr;

#define TASK_STATE_EXIT    0
#define TASK_STATE_RUNNING 1
  unsigned int task_state;

#define TASK_EVENT_DATA   0x01
#define TASK_EVENT_EXIT   0x10
  EVENT_CB_S task_event;
  struct mtx task_mtx;
  void *parnet_conext;
};

extern void composite_mkdevdesc(struct composite_dev_s *priv, uint8_t *buf);
extern int composite_mkstrdesc(struct composite_dev_s *priv, uint8_t id, uint16_t index, uint8_t *buf);
extern int16_t composite_mkcfgdesc(struct composite_dev_s *priv, uint8_t *buf);
extern int composite_initialize(struct composite_softc *softc, uint8_t ndevices,
                                struct composite_devdesc_s *pdevices);
extern void usbd_configep_byspeed(struct usbdev_s *dev, struct usb_endpoint_descriptor *ep_desc);
extern struct composite_devobj_s *usbclass_devobj_get(struct composite_dev_s *cdev, device_type type);
extern int usbd_gadget_attach_driver(void *context,
                                     struct usbdevclass_driver_s *driver);
extern int usbd_gadget_detach_driver(void *context,
                                     struct usbdevclass_driver_s *driver);
extern int usbd_start_udc(void);
extern int usbd_stop_udc(void);
extern char *dev_name_get(void);
extern bool device_is_uvc(void);

#endif
