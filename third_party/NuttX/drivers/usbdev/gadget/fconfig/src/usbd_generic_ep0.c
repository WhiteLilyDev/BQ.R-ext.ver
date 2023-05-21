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

#include <poll.h>
#include "implementation/global_implementation.h"
#include "f_common.h"
#include "los_vm_iomap.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define GENERIC_SPEED_NUM        3

struct usb_generic_event {
  union {
    struct usb_device_request   setup;
  } u;

  /* enum usb_generic_event_type */
  uint8_t                       type;
};

struct mem_info {
  const void *buf;
  size_t len;
};

struct usb_descriptor_header {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
} __attribute__ ((packed));

struct usb_desc_helper {
  unsigned interfaces_count;
  unsigned eps_count;
};

extern EVENT_CB_S eps_event_all;
extern uint32_t eps_event_mask;

static uint16_t get_unaligned_le16(uint16_t *val)
{
  struct {
    uint16_t val_;
  } __attribute__((packed))*ptr = (void *)(val);
  return le16toh(ptr->val_);
}

static uint32_t get_unaligned_le32(uint32_t *val)
{
  struct {
    uint32_t val_;
  } __attribute__((packed))*ptr = (void *)(val);
  return le32toh(ptr->val_);
}

static inline int CmpXchg(unsigned int *lock, int old, int new)
{
  return LOS_AtomicCmpXchg32bits((Atomic *)lock, new, old);
}

static inline enum generic_setup_state clear_cancelled(enum generic_setup_state *state)
{
  return (enum generic_setup_state)
         CmpXchg(state, GENERIC_SETUP_CANCELLED, GENERIC_NO_SETUP);
}

static int generic_parse_single_desc(struct generic_dev_s *dev,  uint32_t desc_idx,
                                     char *data, uint32_t len, void *priv)
{
  struct usb_desc_helper *helper = priv;
  struct usb_descriptor_header *desc = (void *)data;
  uint8_t length = desc->bLength;

  if ((len < sizeof(*desc)) || (len < length))
    {
      usb_err("%s: the length of descriptor is error\n", __FUNCTION__);
      return -1;
    }

  /* Parse descriptor depending on type. */
  switch (desc->bDescriptorType)
    {
    case UDESC_DEVICE:
    case UDESC_CONFIG:
    case UDESC_STRING:
    case UDESC_DEVICE_QUALIFIER:
      usb_err("%s: descriptor %d is not supported\n", __FUNCTION__,
              desc->bDescriptorType);
      return -1;

    case UDESC_INTERFACE:
      {
        struct usb_interface_descriptor *ds = (void *)desc;
        /* check the length of the descriptor */
        if (length != sizeof(struct usb_interface_descriptor))
          {
            usb_err("%s: length is error, len=%d descriptor=%d\n",
                    __FUNCTION__, ds->bLength, ds->bDescriptorType);
            return -1;
          }
        if (ds->bNumEndpoints > 0)
          helper->interfaces_count++;
      }
      break;

    case UDESC_ENDPOINT:
      {
        struct usb_endpoint_descriptor *ds = (void *)desc;
        /* check the length of the descriptor */
        if (length != sizeof(*ds))
          {
            usb_err("%s: length is error, len=%d descriptor=%d\n",
                    __FUNCTION__, ds->bLength, ds->bDescriptorType);
            return -1;
          }

        /* assign endpoint address */
        dev->eps_addrmap[helper->eps_count] = ds->bEndpointAddress;
        dev->eps_descs[helper->eps_count][desc_idx] = ds;

        helper->eps_count++;
        if (dev->eps_count >= GENERIC_MAX_EPS_COUNT)
          {
            usb_err("%s: eps_count is too large\n", __FUNCTION__);
            return -1;
          }

      }
      break;

    case UDESC_OTG:
	  struct usb_otg_descriptor *ds = (struct usb_otg_descriptor *)desc;
      /* check the length of the descriptor */
      if (length != sizeof(struct usb_otg_descriptor))
        {
          usb_err("%s: length is error, len=%d descriptor=%d\n",
                  __FUNCTION__, ds->bLength, ds->bDescriptorType);
          return -1;
        }
      break;

    case UDESC_IFACE_ASSOC:
      {
        struct usb_interface_assoc_descriptor *ds = (void *)desc;

        if (length != sizeof(*ds))
          {
            usb_err("%s: length is error, len=%d descriptor=%d\n",
                    __FUNCTION__, ds->bLength, ds->bDescriptorType);
            return -1;
          }
      }
      break;

    case UDESC_ENDPOINT_SS_COMP:
      {
        struct usb_endpoint_ss_comp_descriptor *ds = (struct usb_endpoint_ss_comp_descriptor *)desc;
        /* check the length of the descriptor */
        if (length != sizeof(struct usb_endpoint_ss_comp_descriptor))
          {
            usb_err("%s: length is error, len=%d descriptor=%d\n",
                    __FUNCTION__, ds->bLength, ds->bDescriptorType);
            return -1;
          }
      }
      break;

    default:
      break;
    }

  return length;
}

static int generic_parse_descs(struct generic_dev_s *dev, uint32_t desc_idx,
                               uint32_t count, char *data, uint32_t len, void *priv)
{
  uint32_t _len = len;
  uint32_t num = 0;

  for (;;)
    {
      int ret;

      if (num == count)
        {
          data = NULL;
        }

      if (data == NULL)
        {
          return _len - len;
        }

      ret = generic_parse_single_desc(dev, desc_idx, data, len, priv);
      if (ret < 0)
        {
          usb_debug("%s: parse single desc error %d\n", __FUNCTION__, ret);
          return ret;
        }
      len  -= ret;
      data += ret;
      ++num;
    }
}

static int generic_got_descs(struct generic_dev_s *dev,
                             char *const buf, size_t len)
{
  char *data = buf, *raw_descs;
  struct usb_desc_helper helper;
  uint32_t counts[GENERIC_SPEED_NUM], flags;
  uint32_t descLen[GENERIC_SPEED_NUM] = {0, 0, 0};
  int ret, i;

  if (get_unaligned_le32((uint32_t *)(data + 4)) != len)
    {
      usb_err("%s: len is error\n", __FUNCTION__);
      return -1;
    }

  flags = get_unaligned_le32((uint32_t *)(data + 8));
  if (flags & ~(GENERIC_HAS_FS_DESC |
                GENERIC_HAS_HS_DESC |
                GENERIC_HAS_SS_DESC |
                GENERIC_ALL_CTRL_RECIP))
    {
      usb_err("%s: the flags %u is error\n", __FUNCTION__, flags);
      return -1;
    }
  dev->descs_flags = flags;
  data += 12;
  len  -= 12;

  /* Read fs_count, hs_count and ss_count */
  for (i = 0; i < GENERIC_SPEED_NUM; ++i)
    {
      if (!(flags & (1 << i)))
        {
          counts[i] = 0;
        }
      else if (len < 4)
        {
          return -1;
        }
      else
        {
          counts[i] = get_unaligned_le32((uint32_t *)data);
          data += 4;
          len  -= 4;
        }
    }

  /* Read descriptors */
  raw_descs = data;
  for (i = 0; i < GENERIC_SPEED_NUM; ++i)
    {
      if (!counts[i])
        {
          continue;
        }
      helper.interfaces_count = 0;
      helper.eps_count = 0;
      ret = generic_parse_descs(dev, i, counts[i], data, len, &helper);
      if (ret < 0)
        {
          usb_err("%s: parse desc %d failed\n", __FUNCTION__, i);
          return ret;
        }
      if (dev->eps_count == 0 && dev->interfaces_count == 0)
        {
          dev->eps_count = helper.eps_count;
          dev->interfaces_count = helper.interfaces_count;
        }
      else
        {
          if (dev->eps_count != helper.eps_count)
            {
              return -1;
            }
          if (dev->interfaces_count != helper.interfaces_count)
            {
              return -1;
            }
        }
      data += ret;
      len  -= ret;
      descLen[i] = ret;
    }

  if ((raw_descs == data) || len)
    {
      usb_err("%s: no descs data\n", __FUNCTION__);
      return -1;
    }

  dev->raw_descs_data   = buf;
  dev->raw_descs        = raw_descs;
  dev->raw_descs_length = data - raw_descs;
  dev->fs_descs_count   = counts[0];
  dev->hs_descs_count   = counts[1];
  dev->ss_descs_count   = counts[2];
  dev->fs_descs_len     = descLen[0];
  dev->hs_descs_len     = descLen[1];
  dev->ss_descs_len     = descLen[2];

  return 0;
}

static int generic_got_strings(struct generic_dev_s *dev,
                               char *const buf, size_t len)
{
  int ret;
  char *data = buf;
  size_t bytes = offsetof(struct generic_strings_head, length);
  uint32_t str_count, str_count_all, lang_count;
  struct generic_strings *dev_strings, *trav_strings;

  if ((len < sizeof(struct generic_strings_head) ||
    get_unaligned_le32((uint32_t *)data) != GENERIC_STRINGS_MAGIC ||
    get_unaligned_le32((uint32_t *)(data + bytes)) != len))
    {
      return -1;
    }

  bytes = offsetof(struct generic_strings_head, str_count);
  str_count = get_unaligned_le32((uint32_t *)(data + bytes));
  bytes = offsetof(struct generic_strings_head, lang_count);
  lang_count  = get_unaligned_le32((uint32_t *)(data + bytes));
  if ((!str_count != !lang_count) || (str_count == 0))
    {
      usb_err("%s: strings_count %u or lang_count %u is error\n",
              __FUNCTION__, str_count, lang_count);
      return -1;
    }

  dev_strings = usbm_zalloc(&dev->obj, sizeof(struct generic_strings));
  if (dev_strings == NULL)
    {
      usb_err("%s: alloc dev_strings error\n", __FUNCTION__);
      return -1;
    }
  bytes = sizeof(struct usbd_string) * (str_count + 1);
  dev_strings->strings = usbm_zalloc(&dev->obj, bytes);
  if (dev_strings == NULL)
    {
      usb_err("%s: alloc dev_strings error\n", __FUNCTION__);
      return -1;
    }

  trav_strings = dev_strings;

  data += sizeof(struct generic_strings_head);
  len  -= sizeof(struct generic_strings_head);

  while (lang_count--)
    {
      struct usbd_string *strings = (struct usbd_string *)trav_strings->strings;
      uint32_t id = 0;

      if (len < 3)
        {
          goto error;
        }

      trav_strings->language = get_unaligned_le16((uint16_t *)data);

      data += sizeof(uint16_t);
      len  -= sizeof(uint16_t);
      str_count_all = str_count;
      while (str_count--)
        {
          char *buff;
          size_t length = strnlen(data, len);
          if (length == len)
            {
              goto error;
            }
          buff = usbm_zalloc(&dev->obj, length + 1);
          if (buff == NULL)
            {
              usb_err("%s: alloc dev_strings error\n", __FUNCTION__);
              goto error;
            }
          ret = memcpy_s(buff, (length + 1), data, length);
          if (ret != EOK)
            {
              usb_err("%s: copy string from user failed\n", __FUNCTION__);
              goto error;
            }
          *(buff + length) = '\0';
          strings->id = id++;
          strings->s  = buff;
          ++strings;
          data += (length + 1);
          len  -= (length + 1);
        }
      strings->id = 0;
      strings->s  = NULL;
      ++trav_strings;
    }

  dev->raw_strings = data;
  dev->dev_strings = dev_strings;
  dev->strings_count = str_count_all;
  return 0;

error:
  usbm_free(&dev->obj, dev_strings);
  return -1;
}

static int memory_area_match(struct usb_obj *obj, void *match_data)
{
  struct usb_memory *usbm = container_of(obj, struct usb_memory, obj);
  struct mem_info *mem = (struct mem_info *)match_data;
  unsigned long buff_start = (unsigned long)mem->buf;
  if (buff_start < usbm->vm_start &&
    buff_start >= usbm->vm_start + usbm->size)
    {
      return 0;
    }

  return (buff_start + mem->len) <= (usbm->vm_start + usbm->size);
}

struct usb_memory *generic_find_memory_area(struct usb_obj *obj,
                                            const void *buf, size_t len)
{
  struct usb_obj *mem_obj;
  struct mem_info mem = { buf, len };

  mem_obj = usbobj_find(obj, memory_area_match, &mem);
  if (mem_obj != NULL)
    {
      return container_of(mem_obj, struct usb_memory, obj);
    }
  return NULL;
}

static void generic_ep0_req_sync_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)req->priv;
  int ret;

  if (ep == NULL || req == NULL)
    {
      usb_err("Illegal request or ep!\n");
      return;
    }

  ret = LOS_EventWrite(&dev->ep0_event, USB_GENERIC_COMPLETE_EVENT);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
  dev->setup_state = GENERIC_NO_SETUP;
}

static int generic_ep0_submit_sync(struct generic_dev_s *dev, struct IoData *ioData)
{
  struct usbdev_req_s *req = dev->ctrlreq;
  uint32_t timeout = ioData->timeout > 0 ? ioData->timeout : LOS_WAIT_FOREVER;
  int ret = -1;

  req->priv = dev;
  req->callback = generic_ep0_req_sync_complete;
  req->len = ioData->len;
  dev->cur_buff = ioData->buf;
  (void)memcpy_s((void *)req->buf, USB_COMP_EP0_BUFSIZ, (void *)ioData->buf,
                 ioData->len);
  ret = EP_SUBMIT(dev->usbdev->ep0, req);
  if (ret != EOK)
    {
      usb_err("EP_SUBMIT failed, ret = %d\n", ret);
      return -EFAULT;
    }
  ret = LOS_EventRead(&dev->ep0_event, USB_GENERIC_COMPLETE_EVENT,
                      LOS_WAITMODE_OR | LOS_WAITMODE_CLR, timeout);
  if (ret == USB_GENERIC_COMPLETE_EVENT)
    {
      if (req->result == 0)
        {
          (void)memcpy_s((void *)ioData->buf, req->xfrd, (void *)req->buf, req->xfrd);
          ret = (int)req->xfrd;
        }
      else
        {
          usb_err("ep failed, ret = %d\n", req->result);
          return -EFAULT;
        }
    }
  else
    {
      usb_err("read timeout, ret = %d\n", ret);
      return -EFAULT;
    }

  return ret;
}

static void generic_ep0_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  int ret;
  struct ep0_request *ep0req = NULL;
  struct generic_dev_s *dev = NULL;
  unsigned long flags;

  if (ep == NULL || req == NULL)
  {
    usb_err("Illegal request or ep!\n");
    return;
  }
  ep0req = (struct ep0_request *)req->priv;
  if (ep0req == NULL)
  {
    usb_err("Illegal ep0req!\n");
    return;
  }
  dev = (struct generic_dev_s *)ep0req->priv;
  if (dev == NULL)
    {
      usb_err("Illegal dev!\n");
      return;
    }
  ep0req->io_data.result = req->result;
  ep0req->io_data.xfrd = req->xfrd;
  ep0req->io_data.buf = ep0req->user_buf;
  (void)memcpy_s((void *)ep0req->io_data.buf, req->xfrd, req->buf, req->xfrd);
  spin_lock_irqsave(&dev->ep0_lock, flags);
  list_add_tail(&ep0req->entry, &dev->ep0_comp_list);
  ret = LOS_EventWrite(&eps_event_all, dev->event_mask);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
  spin_unlock_irqrestore(&dev->ep0_lock, flags);
  ret = LOS_EventWrite(&dev->ctrlreq_event, USB_GENERIC_COMPLETE_EVENT);
  if (ret != OK)
    {
      usb_err("%s write event failed!\n", __FUNCTION__);
    }
}

static int generic_ep0_submit_wait(struct ep0_request *ep0req)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)ep0req->priv;
  struct usbdev_req_s *req = dev->ctrlreq;
  int ret;

  req->priv = ep0req;
  req->callback = generic_ep0_complete;
  (void)memcpy_s((void *)req->buf, USB_COMP_EP0_BUFSIZ, (void *)ep0req->io_data.buf,
                 ep0req->io_data.len);
  req->len = ep0req->io_data.len;
  dev->cur_buff = ep0req->io_data.buf;
  ret = EP_SUBMIT(dev->usbdev->ep0, req);
  if (ret != OK)
    {
      usb_err("generic_ep0_submit_wait response on err %d\n", ret);
      req->result = OK;
      return ret;
    }
  (void)LOS_EventRead(&dev->ctrlreq_event, USB_GENERIC_COMPLETE_EVENT,
                      LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
  return req->result;
}

static ssize_t generic_ep0_read_events(struct generic_dev_s *dev,
                                       char *buffer, size_t buflen, int max_n)
{
  struct usb_generic_event events[USB_GENERIC_EVENTS_NUM];
  size_t size;
  unsigned int event_count = dev->efifo.in - dev->efifo.out;
  unsigned int mask = USB_GENERIC_EVENTS_NUM - 1;
  unsigned int i;
  unsigned long flag;
  int n;
  int ret;

  n = min(max_n, event_count);
  size = n * sizeof(events[0]);
  (void)memset_s(events, size, 0, size);
  spin_lock_irqsave(&dev->event_lock, flag);
  for (i = 0; i < n; i++)
    {
      events[i].type = dev->efifo.data[dev->efifo.out++ & mask];
      if (events[i].type == GENERIC_SETUP)
        {
          events[i].u.setup = dev->setup;
          dev->setup_state = GENERIC_SETUP_PENDING;
        }
    }

  spin_unlock_irqrestore(&dev->event_lock, flag);
  ret = memcpy_s(buffer, buflen, (const void *)events, size);
  if (ret != EOK)
    {
      usb_err("copy failed\n");
      return -EFAULT;
    }

  return 0;
}

static ssize_t generic_ep0_write_descs(struct generic_dev_s *dev, const char *buffer, size_t buflen)
{
  char *data;
  ssize_t ret;

  data = usbm_malloc(&dev->obj, buflen + 1);
  if (data == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return -1;
    }

  ret = memcpy_s(data, buflen + 1, buffer, buflen);
  if (ret != EOK)
    {
      usb_err("copy failed\n");
      goto err;
  }
  ret = generic_got_descs(dev, data, buflen);
  if (ret)
    {
      goto err;
    }
  dev->state = GENERIC_READ_STRINGS;
  return buflen;

err:
  usbm_free(&dev->obj, data);
  return -1;
}

static ssize_t generic_ep0_write_strings(struct generic_dev_s *dev, const char *buffer, size_t buflen)
{
  char *data;
  ssize_t ret;

  data = usbm_malloc(&dev->obj, buflen + 1);
  if (data == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return -1;
    }
  ret = memcpy_s(data, buflen + 1, buffer, buflen);
  if (ret != EOK)
    {
      usb_err("copy failed\n");
      goto err;
  }
  ret = generic_got_strings(dev, data, buflen);
  if (ret)
    {
      goto err;
    }

  ret = generic_epfiles_create(dev);
  if (ret)
    {
      dev->state = GENERIC_CLOSING;
      goto err;
    }
  dev->state = GENERIC_ACTIVE;

  // generic ready
  if (dev->desc_ready)
    {
      dev->state = GENERIC_CLOSING;
      goto err;
    }
  dev->desc_ready = true;
  return buflen;

err:
  usbm_free(&dev->obj, data);
  return -1;
}

static ssize_t generic_ep0_write_request(struct generic_dev_s *dev, const char *buffer, size_t buflen)
{
  struct usb_memory *usbm;
  struct ep0_request *req = NULL;

  usbm = generic_find_memory_area(&dev->memory_obj, buffer, buflen);
  if (usbm == NULL)
    {
      usb_err("%s generic_find_memory_area\n",__FUNCTION__);
      return -1;
    }
  if (!list_empty(&dev->ep0_req_list))
    {
      req = list_first_entry(&dev->ep0_req_list, struct ep0_request, entry);
      if (req == NULL)
        {
          usb_err("%s get req failed\n",__FUNCTION__);
          return -1;
        }
      list_del_init(&req->entry);
    }
  else
    {
      usb_err("%s get req failed\n",__FUNCTION__);
      return -1;
    }
  req->io_data.buf = (uint32_t)buffer;
  req->io_data.len = buflen;
  req->user_buf    = (uint32_t)buffer;
  req->priv = dev;

  return generic_ep0_submit_wait(req);
}

static int generic_ep0_cancel_pending_request(struct generic_dev_s *dev,
                                              uint32_t buffer)
{
  struct ep0_request *req_iter, *tmp;
  bool found = false;
  unsigned long flags;
  int ret;

  spin_lock_irqsave(&dev->ep0_lock, flags);
  list_for_each_entry_safe(req_iter, tmp, &dev->ep0_req_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          list_del_init(&req_iter->entry);
          found = true;
          break;
        }
    }
  spin_unlock_irqrestore(&dev->ep0_lock, flags);

  if (!found)
    {
      usb_err("%s: can not find the buffer\n", __FUNCTION__);
      return -1;
    }

  req_iter->io_data.result = -USB_ERR_CANCELLED;
  req_iter->io_data.xfrd            = 0;

  spin_lock_irqsave(&dev->ep0_lock, flags);
  list_add_tail(&req_iter->entry, &dev->ep0_comp_list);
  ret = LOS_EventWrite(&eps_event_all, dev->event_mask);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
  spin_unlock_irqrestore(&dev->ep0_lock, flags);

  return 0;
}

static int generic_ep0_request_status(struct generic_dev_s *dev, void *buffer)
{
  struct generic_request *req_iter, *tmp;
  unsigned long flags;

  spin_lock_irqsave(&dev->ep0_lock, flags);
  list_for_each_entry_safe(req_iter, tmp, &dev->ep0_comp_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          spin_unlock_irqrestore(&dev->ep0_lock, flags);
          return req_iter->io_data.result;
        }
    }
  list_for_each_entry_safe(req_iter, tmp, &dev->ep0_req_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          spin_unlock_irqrestore(&dev->ep0_lock, flags);
          return USB_ERR_NOT_STARTED;
        }
    }
  spin_unlock_irqrestore(&dev->ep0_lock, flags);

  return -1;
}

static int generic_ep0_handle_open(struct usb_handle *handle)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;

  if (dev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  if (dev->state == GENERIC_CLOSING)
    {
      usb_err("Device is closing!\n");
      return -1;
    }

  generic_dev_opened(dev);

  return 0;
}

static int generic_ep0_handle_close(struct usb_handle *handle)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;

  if (dev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }
  generic_dev_closed(dev);

  return 0;
}

static ssize_t generic_ep0_handle_read(struct usb_handle *handle, void *buffer, size_t len)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;
  int n;
  int ret;

  if (clear_cancelled(&dev->setup_state) == GENERIC_SETUP_CANCELLED)
    {
      usb_err("the setup is cancelled\n");
      return -1;
    }

  /* Acquire mutex */
  mtx_lock(&dev->mutex);

  /* Check state */
  if (dev->state != GENERIC_ACTIVE)
    {
      mtx_unlock(&dev->mutex);
      return -1;
    }

  switch (dev->setup_state)
    {
    case GENERIC_SETUP_CANCELLED:
      ret = -1;
      break;

    case GENERIC_NO_SETUP:
      n = len / sizeof(struct usb_generic_event);
      if (!n)
        {
          ret = -EINVAL;
          break;
        }
      uint32_t mask = 1 << (dev->minor_offset * EVENT_FOR_FUNCTION);
      (VOID)LOS_EventRead(&eps_event_all, mask,
                          LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      ret = generic_ep0_read_events(dev, buffer, len, n);
      break;

    default:
      ret = -1;
      break;
    }

  mtx_unlock(&dev->mutex);
  return ret;
}

static ssize_t generic_ep0_handle_write(struct usb_handle *handle, void *buffer, size_t len)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;
  ssize_t ret;

  if (clear_cancelled(&dev->setup_state) == GENERIC_SETUP_CANCELLED)
    {
      dprintf("[%s][%d] kotei clear_cancelled\n", __func__, __LINE__);
      return -1;
    }
  if (dev == NULL)
    {
      dprintf("[%s][%d] dev is null\n", __func__, __LINE__);
      return -1;
    }
  /* Acquire mutex */
  mtx_lock(&dev->mutex);

  switch (dev->state)
    {
    case GENERIC_READ_DESCRIPTORS:
      ret = generic_ep0_write_descs(dev, buffer, len);
      break;

    case GENERIC_READ_STRINGS:
      ret = generic_ep0_write_strings(dev, buffer, len);
      break;

    case GENERIC_ACTIVE:
      ret = generic_ep0_write_request(dev, buffer, len);
      break;

    default:
      ret = -1;
      break;
    }

  mtx_unlock(&dev->mutex);
  return ret;
}

static int generic_ep0_handle_ioctl(struct usb_handle *handle, int cmd, void *arg)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;
  void *argp = (void *)arg;
  struct usb_memory *usbm;
  struct generic_memory mem;
  unsigned long flags;
  struct ep0_request *req;
  struct ep0_io_data_ret io_ret;
  int memcpy_len = 0;
  int ret = -1;

  switch (cmd)
    {
    case GENERIC_CMD_FREE_MEM:
      ret = memcpy_s(&mem, sizeof(mem), argp, sizeof(mem));
      if (ret != EOK)
        {
          usb_err("copy from user failed\n");
         return -EFAULT;
       }
      usbm = generic_find_memory_area(&dev->memory_obj, (void *)mem.buf, mem.size);
      if (usbm == NULL)
        {
          break;
        }
      usbobj_put(&usbm->obj);
      usbm_free(&dev->obj, (void *)usbm->mem);
      usbm_free(&dev->obj, usbm);
      break;
    case GENERIC_CMD_ENDPOINT_IO:
      {
        struct IoData ioData;
        ret = memcpy_s(&ioData, sizeof(ioData), argp, sizeof(ioData));
        if (ret != EOK)
          {
            usb_err("copy from user failed\n");
            return -EFAULT;
          }
        ret = generic_ep0_submit_sync(dev, &ioData);
      }
      break;
    case GENERIC_CMD_CANCEL_REQUEST:
      ret = memcpy_s(&mem, sizeof(mem), argp, sizeof(mem));
      if (ret != EOK)
        {
          usb_err("copy from user failed\n");
          return -EFAULT;
        }
      if (dev->cur_buff == mem.buf)
        {
          ret = EP_CANCEL(dev->usbdev->ep0, dev->ctrlreq);
        }
      else
        {
          ret = generic_ep0_cancel_pending_request(dev, mem.buf);
        }
      break;
    case GENERIC_CMD_GET_REQ_STATUS:
      {
        struct IoData ioData;
        int ret;

        ret = memcpy_s(&ioData, sizeof(ioData), argp, sizeof(ioData));
        if (ret != EOK)
          {
            usb_err("copy from user failed\n");
            return -EFAULT;
          }
        ret = generic_ep0_request_status(dev, (void *)ioData.buf);
      }
      break;
    case GENERIC_CMD_GET_EP0_EVENT:
      (VOID)LOS_EventRead(&eps_event_all, dev->event_mask,
                          LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      spin_lock_irqsave(&dev->ep0_lock, flags);
      if (!list_empty(&dev->ep0_comp_list))
        {
          req = list_first_entry(&dev->ep0_comp_list, struct ep0_request, entry);
          list_del_init(&req->entry);
          io_ret.buf = req->io_data.buf;
          io_ret.actual = req->io_data.xfrd;
          io_ret.status = req->io_data.result;
          memcpy_len = sizeof(struct generic_io_data_ret);
          ret = memcpy_s(arg, memcpy_len, &io_ret, memcpy_len);
          if (ret != EOK)
            {
              usb_err("copy failed\n");
              spin_unlock_irqrestore(&dev->ep0_lock, flags);
              return -EFAULT;
            }
          dev->setup_state = GENERIC_NO_SETUP;
          list_add_tail(&req->entry, &dev->ep0_req_list);
        }
      spin_unlock_irqrestore(&dev->ep0_lock, flags);
      break;
    default:
      if (dev && dev->usbdev->ops->ioctl)
        {
          ret = dev->usbdev->ops->ioctl(dev->usbdev, cmd, (unsigned long)arg);
        }
      break;
    }

  return ret;
}

static void *generic_ep0_handle_mmap(struct usb_handle *handle, uint32_t size)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;
  struct usb_memory *usbm;
  void *mem;

  if (dev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return NULL;
    }

  usbm = usbm_malloc(&dev->obj, sizeof(*usbm));
  if (usbm == NULL)
  {
    usb_err("%s alloc memory failed!\n", __FUNCTION__);
    return NULL;
  }
  usbobj_init(&usbm->obj, "usbm", usbobj_default_release);
  mem = usbm_malloc(&dev->obj, size);
  if (mem == NULL)
  {
    usb_err("%s alloc memory failed!\n", __FUNCTION__);
    return NULL;
  }
  usbm->mem      = (unsigned long)mem;
  usbm->size     = size;
  usbm->vm_start = (unsigned long)mem;

  usbobj_add(&usbm->obj, &dev->memory_obj);
  return mem;
}

static int generic_ep0_handle_poll(struct usb_handle *handle, int timeout)
{
  struct generic_dev_s *dev = (struct generic_dev_s *)handle->priv;
  uint32_t revents = POLLWRNORM;
  uint32_t ret;
  uint32_t ep0_event = 0;

  if (dev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  mtx_lock(&dev->mutex);
  ret = LOS_EventRead(&eps_event_all, eps_event_mask,
                      LOS_WAITMODE_OR, timeout > 0 ? timeout:LOS_WAIT_FOREVER);
  switch (dev->state)
    {
    case GENERIC_READ_DESCRIPTORS:
    case GENERIC_READ_STRINGS:
      revents |= POLLOUT;
      break;

    case GENERIC_ACTIVE:
      if (dev->setup_state == GENERIC_NO_SETUP)
        {
          ep0_event = (1 << (dev->minor_offset * EVENT_FOR_FUNCTION));
          if ((ret & ep0_event) == 0)
            {
              mtx_unlock(&dev->mutex);
              return revents;
            }
          if (dev->efifo.in - dev->efifo.out > 0)
            {
              dprintf("[%s] %s can POLLIN\n", __func__, handle->name);
              revents |= POLLIN;
            }
        }
      else
        {
          if ((ret & dev->event_mask) == 0)
            {
              mtx_unlock(&dev->mutex);
              return revents;
            }
          if (!list_empty(&dev->ep0_comp_list))
            {
              revents |= POLLOUT;
              dprintf("[%s] %s can POLLOUT\n", __func__, handle->name);
           }
      }
      break;

    case GENERIC_CLOSING:
    case GENERIC_DEACTIVATED:
      break;
    }

  mtx_unlock(&dev->mutex);

  return (ssize_t)revents;
}


static struct UsbHandleOps generic_ep0_handle_ops = {
  .open   = generic_ep0_handle_open,
  .close  = generic_ep0_handle_close,
  .read   = generic_ep0_handle_read,
  .write  = generic_ep0_handle_write,
  .ioctl  = generic_ep0_handle_ioctl,
  .mmap   = generic_ep0_handle_mmap,
  .poll   = generic_ep0_handle_poll,
};

int generic_create_ep0(struct generic_dev_s *dev)
{
  struct ep0_request *req = NULL;
  char *dev_name = NULL;
  unsigned long flags;
  int ret;
  int i;

  dev_name = (char *)usbm_malloc(&dev->obj, GENERIC_PATH_LEN);
  if (dev_name == NULL)
    {
      usb_err("%s: malloc dev_name failed\n", __FUNCTION__);
      return -1;
    }
  (void)memset_s(dev_name, GENERIC_PATH_LEN, 0, GENERIC_PATH_LEN);
  ret = snprintf_s(dev_name, GENERIC_PATH_LEN, GENERIC_PATH_LEN - 1,
                   "/dev/%s", dev->name);
  if (ret < 0)
    {
      usb_err("make directory path error! ret = %d, errono = %d\n",
              ret, get_errno());
      goto err;
    }
  ret = strncat_s(dev_name, GENERIC_PATH_LEN, "/ep0", strlen("/ep0"));
  if (ret < 0)
    {
      usb_err("usb make ep0 file path error! ret = %d, errono = %d\n",
              ret, get_errno());
      goto err;
    }
  ret = register_handle(dev_name, &generic_ep0_handle_ops, (void *)dev, &dev->obj);
  if (ret != 0)
    {
      usb_err("chardev register failed!\n");
      goto err;
    }
  uint32_t mask = 1 << (dev->minor_offset * EVENT_FOR_FUNCTION + 1);
  eps_event_mask |= mask;
  dev->event_mask = mask;
  INIT_LIST_HEAD(&dev->ep0_comp_list);
  INIT_LIST_HEAD(&dev->ep0_req_list);
  for (i = 0; i < 0x2; i++)
    {
      req = usbm_malloc(&dev->obj, sizeof(*req));
      if (req == NULL)
        {
          usb_err("%s usbm_malloc\n",__FUNCTION__);
          return -1;
        }
      INIT_LIST_HEAD(&req->entry);
      spin_lock_irqsave(&dev->ep0_lock, flags);
      list_add_tail(&req->entry, &dev->ep0_req_list);
      spin_unlock_irqrestore(&dev->ep0_lock, flags);
    }
  return 0;
err:
  usbm_free(&dev->obj, dev_name);
  return ret;
}

int generic_remove_ep0(struct generic_dev_s *dev)
{
  struct ep0_request *req = NULL;
  struct ep0_request *tmp = NULL;
  char dev_name[GENERIC_PATH_LEN];
  int ret;
  unsigned long flags;

  spin_lock_irqsave(&dev->ep0_lock, flags);
  list_for_each_entry_safe(req, tmp, &dev->ep0_req_list, entry)
    {
      list_del_init(&req->entry);
      usbm_free(&dev->obj, req);
    }
  list_for_each_entry_safe(req, tmp, &dev->ep0_comp_list, entry)
    {
      list_del_init(&req->entry);
      usbm_free(&dev->obj, req);
    }
  spin_unlock_irqrestore(&dev->ep0_lock, flags);
  ret = snprintf_s(dev_name, sizeof(dev_name), sizeof(dev_name) - 1,
                   "/dev/%s/ep0", dev->name);
  if (ret < 0)
    {
      usb_err("usb make ep0 file path error! ret = %d, errono = %d\n",
              ret, get_errno());
      return ret;
    }
  ret = unregister_handle(dev_name);
  if (ret != OK)
    {
      usb_err("generic_dev unregister failed!\n");
      return ret;
    }

  return ret;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
