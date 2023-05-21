/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS USB Driver HID Protocol HeadFile
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

#ifndef _F_GENERIC_GADGET_H
#define _F_GENERIC_GADGET_H

#include "usb_obj.h"
#include "gadget/usbdev.h"
#include "gadget/composite.h"


#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Define usb generic function gadget path. */

#define USB_GENERIC_DEV                 "/dev/generic"

#define GENERIC_IOC_MAGIC               'g'
#define GENERIC_CMD_FREE_MEM            _IO(GENERIC_IOC_MAGIC, 1)
#define GENERIC_CMD_CANCEL_REQUEST      _IO(GENERIC_IOC_MAGIC, 2)
#define GENERIC_CMD_GET_PIPE_INFO       _IO(GENERIC_IOC_MAGIC, 3)
#define GENERIC_CMD_GET_EP0_EVENT       _IO(GENERIC_IOC_MAGIC, 4)
#define GENERIC_CMD_ENDPOINT_IO         _IO(GENERIC_IOC_MAGIC, 5)
#define GENERIC_CMD_GET_REQ_STATUS      _IO(GENERIC_IOC_MAGIC, 6)

#define USB_GENERIC_EVENTS_NUM          4
#define USB_GENERIC_COMPLETE_EVENT      (1 << 0)
#define USB_GENERIC_EP0_EVENT           (1 << 0)

struct event_fifo
{
  uint32_t      in;
  uint32_t      out;
  uint8_t       data[USB_GENERIC_EVENTS_NUM];
};

enum generic_state
{
  /*
   * Waiting for descriptors and strings.
   */
  GENERIC_READ_DESCRIPTORS,
  GENERIC_READ_STRINGS,

  /*
   * We've got descriptors and strings.
   */
  GENERIC_ACTIVE,

  /*
   * Function is deactived, all epfiles, except ep0, are deleted
   * so there is no way to perform any operations on them.
   */
  GENERIC_DEACTIVATED,

  /*
   * All endpoints have been closed.
   */
  GENERIC_CLOSING
};

enum generic_setup_state
{
  /* There is no setup request pending. */
  GENERIC_NO_SETUP,
  /*
   * There was a setup request event there, the request
   * will be handled on the next read/write on ep0.
   */
  GENERIC_SETUP_PENDING,
  /*
   * The existing event was canceled.
   */
  GENERIC_SETUP_CANCELLED
};

struct generic_memory
{
  size_t size;
  size_t buf;
  char storage[];
};

struct IoData {
  uint32_t aio;       /* 0 for sync ,1 for async */
  uint32_t read;      /* 0 for write ,1 for read */
  uint32_t len;       /* the len of this io request */
  uint32_t timeout;   /* sync timeout */
  uint32_t buf;       /* the address of map buf */
};

struct generic_ep
{
  struct usbdev_ep_s            *ep;
  struct usbdev_req_s           *req;
  EVENT_CB_S                    event;

  /* [0]: full speed, [1]: high speed, [2]: super speed */
  usb_endpoint_descriptor_t     *descs[3];
  uint8_t                       num;
  int                           status;
  void                          *priv;
};

struct usb_memory
{
  struct usb_obj obj;
  unsigned long mem;
  unsigned long vm_start;
  uint32_t size;
  atomic_t usage;
};

struct generic_strings {
  uint16_t                      language;
  struct usbd_string            *strings;
};

struct generic_dev_s
{
  struct usb_obj                obj;
  struct usbdev_s               *usbdev;
  struct generic_driver_s       *drvr;
  const char                    *name;
  int                           minor;
  int                           minor_offset;
  pthread_mutex_t               mutex;
  struct usbdev_devinfo_s       *devinfo;
  struct list_head              ep0_comp_list;
  struct list_head              ep0_req_list;
  bool                          inuse;
  bool                          desc_ready;
  bool                          eps_enbale;

  /* the number of files are opened (EP0 and others) */
  atomic_t                      opened;

  /* EP0 state */
  enum generic_state            state;
  enum generic_setup_state      setup_state;
  struct usb_device_request     setup;
  uint16_t                      ep0_can_stall;
  struct event_fifo             efifo;
  EVENT_CB_S                    ep0_event;
  spinlock_t                    event_lock;
  struct usb_obj                memory_obj;
  struct usbdev_req_s           *ctrlreq;
  EVENT_CB_S                    ctrlreq_event;
  uint32_t                      event_mask;
  EVENT_CB_S                    ep_event_all;
  uint32_t                      cur_buff;

  /* Flags */
  unsigned long                 flags;
  const void                    *raw_descs_data;
  const void                    *raw_descs;
  uint32_t                      raw_descs_length;
  uint32_t                      fs_descs_count;
  uint32_t                      hs_descs_count;
  uint32_t                      ss_descs_count;
  uint32_t                      fs_descs_len;
  uint32_t                      hs_descs_len;
  uint32_t                      ss_descs_len;
  uint32_t                      descs_flags;
  uint32_t                      strings_count;
  uint32_t                      lang_count;
  const void                    *raw_strings;
  struct generic_strings        *dev_strings;

#define GENERIC_MAX_EPS_COUNT 31
  uint8_t                       eps_addrmap[GENERIC_MAX_EPS_COUNT];
  usb_endpoint_descriptor_t     *eps_descs[GENERIC_MAX_EPS_COUNT][3];
  uint8_t                       speed;
  spinlock_t                    eps_lock;
  spinlock_t                    ep0_lock;
  struct generic_ep             *eps;
  uint8_t                       eps_revmap[16];
  uint32_t                      alt_setting;
  uint16_t                      interfaces_count;
  uint16_t                      eps_count;
  struct usb_obj                epfiles_obj;
};

struct generic_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct generic_dev_s *dev;
};

struct generic_softc
{
  struct generic_dev_s dev;
  struct generic_driver_s drvr;
};

static inline void event_fifo_reset(struct event_fifo *efifo)
{
  efifo->in = efifo->out = 0;
}

static inline bool event_fifo_len(struct event_fifo *efifo)
{
  return efifo->in - efifo->out;
}

static inline bool event_fifo_is_empty(struct event_fifo *efifo)
{
  return efifo->in == efifo->out;
}

static inline bool event_fifo_is_full(struct event_fifo *efifo)
{
  uint32_t mask = USB_GENERIC_EVENTS_NUM - 1;
  return event_fifo_len(efifo) > (mask);
}

static inline int event_fifo_put(struct event_fifo *efifo, uint8_t event)
{
  int ret = !event_fifo_is_full(efifo);
  if (ret)
    {
      uint32_t mask = USB_GENERIC_EVENTS_NUM - 1;
      efifo->data[efifo->in & mask] = event;
      efifo->in++;
    }

  return ret;
}

static inline int event_fifo_get(struct event_fifo *efifo, uint8_t *event)
{
  int ret = !event_fifo_is_empty(efifo);
  if (ret)
    {
      uint32_t mask = USB_GENERIC_EVENTS_NUM - 1;
      *event = efifo->data[efifo->out & mask];
      efifo->out++;
    }

  return ret;
}

extern int usbdev_generic_alloc_instance(const char *name);
extern int usbdev_generic_free_instance(const char *name);
extern void usbdev_generic_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void generic_dev_closed(struct generic_dev_s *dev);
extern void generic_dev_opened(struct generic_dev_s *dev);

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _F_GENERIC_GADGET_H */
