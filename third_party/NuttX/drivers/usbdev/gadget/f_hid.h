/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS USB Driver HID Protocol HeadFile
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

#ifndef _F_HID_GADGET_H
#define _F_HID_GADGET_H

#include "gadget/usbdev.h"
#include "gadget/composite.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Define usb hid gadget path. */

#define USB_HID_DEV "/dev/hid"

#define HID_IN_DATA_SIZE       0x40U
#define HID_OUT_DATA_SIZE      0x40U
#define USB_HID_READ_EVENT     0x11

struct hid_endpoint_descriptor
{
  uByte bLength;
  uByte bDescriptorType;
  uByte bEndpointAddress;
  uByte bmAttributes;
  uWord wMaxPacketSize;
  uByte bInterval;
} __attribute__((packed));

struct hid_dev_s
{
  struct usbdev_ep_s *out_ep; /* interrupt output endpoint */
  struct usbdev_ep_s *in_ep;  /* interrupt input endpoint */
  bool out_ep_enabled;
  bool in_ep_enabled;
  struct usbdev_req_s outputreq; /* interrupt output report */
  struct usbdev_req_s inputreq;  /* HID report request */

  void *read_buf;
  size_t read_len;
  EVENT_CB_S read_event;
  spinlock_t hid_lock;
  atomic_t open_flag; /* Hid device status. 1: open, 0: close */

  struct list_head hid_queue;
  int hid_queue_len;
  volatile atomic_t busy_flag;
  struct hid_queue_node *cur_node;
};

struct hid_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct hid_dev_s *dev;
};

struct hid_softc
{
  struct hid_dev_s dev;
  struct hid_driver_s drvr;
};

struct dev_report_desc
{
  size_t  report_size;
  uint8_t *report_desc;
};

struct dev_string_desc
{
  char *str;
  uint32_t len;
};

extern void hid_deviceid_info(uint16_t vendorid, uint16_t productid);
extern int hid_report_descriptor_info(const void *buf, size_t len);
extern int hid_device_string_info(const struct dev_string_desc *str_manufacturer,
                                  const struct dev_string_desc *str_product,
                                  const struct dev_string_desc *str_serial_number);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _F_HID_GADGET_H */