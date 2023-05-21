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

extern EVENT_CB_S eps_event_all;
extern uint32_t eps_event_mask;

static void generic_epfile_submit_request(struct generic_epfile *epfile);
extern struct usb_memory *generic_find_memory_area(struct usb_obj *obj,
                                                   const void *buf, size_t len);

static void generic_epfile_req_sync_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct generic_epfile *epfile = (struct generic_epfile *)req->priv;
  int ret;

  if (ep == NULL || epfile == NULL || req == NULL)
    {
      usb_err("Illegal request or ep!\n");
      return;
    }

  ret = LOS_EventWrite(&epfile->sync_event, USB_GENERIC_COMPLETE_EVENT);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
}

static int generic_epfile_submit_sync(struct generic_epfile *epfile, struct IoData *ioData)
{
  struct usbdev_req_s *req = epfile->ep->req;
  uint32_t timeout = ioData->timeout > 0 ? ioData->timeout : LOS_WAIT_FOREVER;
  int ret;

  if (atomic_read(&epfile->busy_flag) == 1) {
    usb_err("epfile is busy\n");
    return -EFAULT;
  }
  atomic_set(&epfile->busy_flag, 1);
  req->priv    = epfile;
  req->callback = generic_epfile_req_sync_complete;
  req->len    = ioData->len;
  req->buf    = (uint8_t *)ioData->buf;
  req->dma = (DMA_ADDR_T)VMM_TO_UNCACHED_ADDR((unsigned long)req->buf);
  ret = EP_SUBMIT(epfile->ep->ep, req);
  if (ret != EOK) {
    usb_err("EP_SUBMIT failed, ret = %d\n", ret);
    atomic_set(&epfile->busy_flag, 0);
    return -EFAULT;
  }
  ret = LOS_EventRead(&epfile->sync_event, USB_GENERIC_COMPLETE_EVENT,
    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, timeout);
  if (ret == USB_GENERIC_COMPLETE_EVENT) {
    if (req->result == 0) {
      ret = req->xfrd;
    } else {
      usb_err("ep failed, ret = %d\n", req->result);
      atomic_set(&epfile->busy_flag, 0);
      return -EFAULT;
    }
  } else {
    usb_err("read timeout, ret = %d\n", ret);
    atomic_set(&epfile->busy_flag, 0);
    return -EFAULT;
  }
  atomic_set(&epfile->busy_flag, 0);
  return ret;
}

static void generic_epfile_do_submit(struct generic_epfile *epfile)
{
  struct generic_request *req;

  if (!list_empty(&epfile->req_list))
    {
      req = list_first_entry(&epfile->req_list, struct generic_request, entry);
      list_del_init(&req->entry);
      epfile->req_count--;
      epfile->cur_req = req;
      generic_epfile_submit_request(epfile);
    }
}

static void generic_epfile_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct generic_epfile *epfile = (struct generic_epfile *)req->priv;
  int ret;
  uint32_t list_flags;
  if (ep == NULL || epfile == NULL || req == NULL)
    {
      usb_err("Illegal request or ep!\n");
      return;
    }
  spin_lock_irqsave(&epfile->list_lock, list_flags);
  atomic_set(&epfile->busy_flag, 0);

  if (epfile->cur_req != NULL)
    {
      struct generic_request *cur_req = epfile->cur_req;
      epfile->cur_req = NULL;

      cur_req->io_data.result = req->result;
      cur_req->io_data.xfrd   = req->xfrd;
      cur_req->io_data.buf    = (uint32_t)cur_req->user_buf;
      list_add_tail(&cur_req->entry, &epfile->comp_list);

      ret = LOS_EventWrite(&eps_event_all, epfile->event_mask);
      if (ret != OK)
        {
          usb_err("%s: write event failed!\r\n", __FUNCTION__);
        }
    }
  if(req->result == 0)
    {
      generic_epfile_do_submit(epfile);
    }
  spin_unlock_irqrestore(&epfile->list_lock, list_flags);
}

static void generic_epfile_submit_request(struct generic_epfile *epfile)
{
  struct usbdev_req_s *req;
  int ret;

  if (epfile->ep == NULL) {
    goto fail;
  }
  req = epfile->ep->req;
  atomic_set(&epfile->busy_flag, 1);

  req->priv     = epfile;
  req->callback = generic_epfile_req_complete;
  req->len      = epfile->cur_req->io_data.len;
  req->buf      = (void *)epfile->cur_req->io_data.buf;
  req->dma = (DMA_ADDR_T)VMM_TO_UNCACHED_ADDR((unsigned long)req->buf);

  ret = EP_SUBMIT(epfile->ep->ep, req);
  if (ret != OK)
    {
      usb_err("can not send data to host\r\n");
    }
  return;
fail:
  struct generic_request *cur_req = epfile->cur_req;
  epfile->cur_req = NULL;
  cur_req->io_data.result = -USB_ERR_NO_PIPE;
  cur_req->io_data.xfrd = 0;
  cur_req->io_data.buf = (uint32_t)cur_req->user_buf;
  list_add_tail(&cur_req->entry, &epfile->comp_list);
  ret = LOS_EventWrite(&eps_event_all, epfile->event_mask);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
  generic_epfile_do_submit(epfile);
}

static int generic_epfile_cancel_cur_request(struct generic_epfile *epfile)
{
  if (atomic_read(&epfile->busy_flag) == 1)
    {
      return EP_CANCEL(epfile->ep->ep, epfile->ep->req);
    }

  return -1;
}

static int generic_epfile_cancel_pending_request(struct generic_epfile *epfile,
                                                 void *buffer)
{
  struct generic_request *req_iter, *tmp;
  bool found = false;
  unsigned long flags;
  int ret;

  spin_lock_irqsave(&epfile->dev->eps_lock, flags);
  list_for_each_entry_safe(req_iter, tmp, &epfile->req_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          list_del_init(&req_iter->entry);
          found = true;
          break;
        }
    }
  spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);

  if (!found)
    {
      usb_err("%s: can not find the buffer\n", __FUNCTION__);
      return -1;
    }

  req_iter->io_data.result = -USB_ERR_CANCELLED;
  req_iter->io_data.xfrd            = 0;

  spin_lock_irqsave(&epfile->dev->eps_lock, flags);
  list_add_tail(&req_iter->entry, &epfile->comp_list);
  ret = LOS_EventWrite(&eps_event_all, epfile->event_mask);
  if (ret != OK)
    {
      usb_err("%s: write event failed!\r\n", __FUNCTION__);
    }
  generic_epfile_do_submit(epfile);
  spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);

  return 0;
}

static int generic_epfile_request_status(struct generic_epfile *epfile,
                                                 void *buffer)
{
  struct generic_request *req_iter, *tmp;
  unsigned long flags;

  spin_lock_irqsave(&epfile->dev->eps_lock, flags);
  list_for_each_entry_safe(req_iter, tmp, &epfile->comp_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);
          return req_iter->io_data.result;
        }
    }
  list_for_each_entry_safe(req_iter, tmp, &epfile->req_list, entry)
    {
      if (req_iter->user_buf == buffer)
        {
          spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);
          return USB_ERR_NOT_STARTED;
        }
    }
  spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);

  return -1;
}

static void generic_epfile_free_memory(struct generic_epfile *epfile)
{
  /* free all requests */
  usbobj_put(&epfile->obj);
  /* free all memory */
  usbobj_put(&epfile->memory_obj);
}

static int generic_epfile_handle_open(struct usb_handle *handle)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;

  if (epfile->dev->state != GENERIC_ACTIVE)
    {
      usb_err("%s: state is not active\n", __FUNCTION__);
      return -1;
    }
  generic_dev_opened(epfile->dev);

  return 0;
}

static int generic_epfile_handle_close(struct usb_handle *handle)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;

  if (epfile == NULL)
    {
      usb_err("%s: epfile is null\n", __FUNCTION__);
      return -1;
    }
  generic_dev_closed(epfile->dev);

  return 0;
}

static ssize_t generic_epfile_handle_read(struct usb_handle *handle, void *buffer, size_t len)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;
  struct generic_request *req = NULL;
  uint32_t flags;
  uint32_t list_flags;
  int ret;
  uint32_t copied = 0;
  struct generic_io_data_ret io_ret;

  if (epfile == NULL)
    {
      usb_err("Invalid parameter!\n");
      return -1;
    }

  if (epfile->dev->state != GENERIC_ACTIVE)
    {
      usb_err("%s: state is not active\n", __FUNCTION__);
      return -1;
    }
  spin_lock_irqsave(&epfile->list_lock, list_flags);
  spin_lock_irqsave(&epfile->dev->eps_lock, flags);
  (VOID)LOS_EventClear(&eps_event_all, ~epfile->event_mask);
  while (!list_empty(&epfile->comp_list))
    {
      req = list_first_entry(&epfile->comp_list, struct generic_request, entry);
      io_ret.buf = req->io_data.buf;
      io_ret.actual = req->io_data.xfrd;
      io_ret.status = req->io_data.result;

      ret = memcpy_s((uint8_t *)buffer + copied, sizeof(struct generic_io_data_ret), &io_ret, sizeof(struct generic_io_data_ret));
      if (ret != EOK) {
        usb_err("copy failed\n");
        spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);
        spin_unlock_irqrestore(&epfile->list_lock, list_flags);
        return -EFAULT;
      }
      list_del_init(&req->entry);
      copied += sizeof(struct generic_io_data_ret);
      usbm_free(&epfile->obj, req);
    }
  spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);
  spin_unlock_irqrestore(&epfile->list_lock, list_flags);
  return copied;
}

static ssize_t generic_epfile_handle_write(struct usb_handle *handle, void *buffer, size_t len)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;
  struct usb_memory *usbm;
  struct generic_request *req;
  uint32_t flags;
  uint32_t list_flags;
  if (epfile == NULL)
    {
      usb_err("%s: Invalide parameter!\n", __FUNCTION__);
      return -1;
    }

  if (epfile->dev->state != GENERIC_ACTIVE)
    {
      usb_err("%s: state is not active\n", __FUNCTION__);
      return -1;
    }

  usbm = generic_find_memory_area(&epfile->memory_obj, buffer, len);
  if (usbm == NULL)
    {
      return -1;
    }

  req = usbm_malloc(&epfile->obj, sizeof(*req));
  if (req == NULL)
    {
      usb_err("%s malloc failed!\n", __FUNCTION__);
      return -1;
    }
  spin_lock_irqsave(&epfile->list_lock, list_flags);
  INIT_LIST_HEAD(&req->entry);
  req->io_data.buf = (uint32_t)(buffer);
  req->io_data.len = len;
  req->user_buf    = buffer;

  spin_lock_irqsave(&epfile->dev->eps_lock, flags);
  list_add_tail(&req->entry, &epfile->req_list);
  epfile->req_count++;
  if (!atomic_read(&epfile->busy_flag))
    {
      generic_epfile_do_submit(epfile);
    }

  spin_unlock_irqrestore(&epfile->dev->eps_lock, flags);
  spin_unlock_irqrestore(&epfile->list_lock, list_flags);
  return 0;
}

static int generic_epfile_handle_ioctl(struct usb_handle *handle, int cmd, void *arg)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;
  void *argp = (void __user *)(uintptr_t)arg;
  struct usb_memory *usbm;
  struct generic_memory mem;
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
      usbm = generic_find_memory_area(&epfile->memory_obj, (void *)mem.buf, mem.size);
      if (usbm == NULL)
        {
          break;
        }
      usbobj_put(&usbm->obj);
      LOS_DmaMemFree((void *)usbm->mem);
      usbm_free(&epfile->memory_obj, usbm);
      ret = 0;
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
        ret = generic_epfile_submit_sync(epfile, &ioData);
      }
      break;
    case GENERIC_CMD_CANCEL_REQUEST:
      ret = memcpy_s(&mem, sizeof(mem), argp, sizeof(mem));
      if (ret != EOK)
        {
          usb_err("copy from user failed\n");
          return -EFAULT;
        }
      if ((epfile->cur_req != NULL) && \
        (epfile->cur_req->user_buf == (void *)mem.buf))
        {
          ret = generic_epfile_cancel_cur_request(epfile);
        }
      else
        {
          ret = generic_epfile_cancel_pending_request(epfile, (void *)mem.buf);
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
        ret = generic_epfile_request_status(epfile, (void *)ioData.buf);
      }
      break;
    case GENERIC_CMD_GET_PIPE_INFO:
      {
        int desc_idx;
        struct generic_dev_s *dev = epfile->dev;
        int ep_index = 0;
        usb_endpoint_descriptor_t *desc;

        switch (epfile->dev->speed)
          {
          case USB_SPEED_SUPER:
            desc_idx = 2;
            break;
          case USB_SPEED_HIGH:
            desc_idx = 1;
            break;
          default:
            desc_idx = 1;
          }
        ep_index = atoi(epfile->name + 2);
        if (ep_index < 1 || ep_index > dev->eps_count)
          {
            usb_err("invail ep_index = %d\n", ep_index);
            return -EFAULT;
          }
        desc = dev->eps_descs[ep_index - 1][desc_idx];
        ret = memcpy_s(arg, sizeof(*desc), desc, sizeof(*desc));
        if (ret != EOK)
          {
            usb_err("copy from user failed\n");
            return -EFAULT;
          }
        return ret;
      }
      break;
    default:
      usb_err("%s: the cmd %d is not supporten", __FUNCTION__);
      break;
    }

  return ret;
}

static void *generic_epfile_handle_mmap(struct usb_handle *handle, uint32_t size)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;
  struct usb_memory *usbm;
  void *mem;

  if (epfile == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return NULL;
    }

  usbm = usbm_malloc(&epfile->memory_obj, sizeof(*usbm) + size);
  if (usbm == NULL)
    {
      usb_err("%s alloc memory failed!\n", __FUNCTION__);
      return NULL;
    }
  usbobj_init(&usbm->obj, epfile->name, usbobj_default_release);
  DMA_ADDR_T paddr;
  mem = LOS_DmaMemAlloc(&paddr, size, USB_CACHE_ALIGN_SIZE, DMA_NOCACHE);
  usbm->mem      = (unsigned long)mem;
  usbm->size     = size;
  usbm->vm_start = (unsigned long)mem;

  usbobj_add(&usbm->obj, &epfile->memory_obj);
  return mem;
}

static int generic_epfile_handle_poll(struct usb_handle *handle, int timeout)
{
  struct generic_epfile *epfile = (struct generic_epfile *)handle->priv;
  uint32_t revents = POLLRDNORM;
  uint32_t ret;
  uint32_t value = POLLIN;

  if (epfile == NULL)
    {
      usb_err("Invalid parameter!\n");
      return POLLERR;
    }

  if (value)
    {
      ret = LOS_EventRead(&eps_event_all, eps_event_mask,
                          LOS_WAITMODE_OR, timeout > 0 ? timeout:LOS_WAIT_FOREVER);
      if (ret & epfile->event_mask)
        {
          revents |= value;
        }
    }

  return (int)revents;
}

static struct UsbHandleOps generic_epfile_handle_ops = {
  .open   = generic_epfile_handle_open,
  .close  = generic_epfile_handle_close,
  .read   = generic_epfile_handle_read,
  .write  = generic_epfile_handle_write,
  .ioctl  = generic_epfile_handle_ioctl,
  .mmap   = generic_epfile_handle_mmap,
  .poll   = generic_epfile_handle_poll,
};

void generic_epfile_release(struct usb_obj *obj)
{
  struct generic_epfile *epfile;
  char dev_name[GENERIC_PATH_LEN];

  epfile = container_of(obj, struct generic_epfile, obj);
  if (epfile->is_created)
    {
      (void)snprintf_s(dev_name, sizeof(dev_name), sizeof(dev_name) - 1,
                       "/dev/%s/%s", epfile->dev->name, epfile->name);
      (void)unregister_handle(dev_name);
    }
  generic_epfile_free_memory(epfile);
  usbm_free(&epfile->dev->obj, epfile);
}

int  generic_remove_epfiles(struct generic_dev_s *dev)
{
  char epfile_name[5] = {0};
  unsigned i, count;
  struct usb_obj *obj;

  count = dev->eps_count;
  for (i = 1; i <= count; ++i)
    {
      sprintf(epfile_name, "ep%u", i);
      obj = usbobj_find(&dev->epfiles_obj, usbobj_default_match, epfile_name);
      if (!obj)
        {
          dprintf("%s, usbobj_find %s obj failed\n", __func__, epfile_name);
          return -1;
        }
      usbobj_put(obj);
    }
  return 0;
}

int generic_epfiles_create(struct generic_dev_s *dev)
{
  struct generic_epfile *epfile;
  char *dev_name = NULL;
  unsigned i, count, ret;

  count = dev->eps_count;
  for (i = 1; i <= count; ++i)
    {
      epfile = usbm_zalloc(&dev->obj, sizeof(*epfile));
      if (!epfile)
        {
          return -ENOMEM;
        }
      dev_name = usbm_zalloc(&dev->obj, GENERIC_PATH_LEN);
      if (!dev_name)
        {
          usbm_free(&dev->obj, epfile);
          return -ENOMEM;
        }
      epfile->dev = dev;
      sprintf(epfile->name, "ep%u", i);
      mtx_init(&epfile->mutex, epfile->name, NULL, 0);
      INIT_LIST_HEAD(&epfile->req_list);
      INIT_LIST_HEAD(&epfile->comp_list);
      spin_lock_init(&epfile->list_lock);
      usbobj_init(&epfile->obj, epfile->name, generic_epfile_release);
      usbobj_init(&epfile->memory_obj, epfile->name, NULL);

      ret = snprintf_s(dev_name, GENERIC_PATH_LEN, GENERIC_PATH_LEN - 1,
                       "/dev/%s/%s", dev->name, epfile->name);
      if (ret < 0)
        {
          usb_err("make directory path error! ret = %d, errono = %d\n",
                  ret, get_errno());
          usbobj_put(&dev->epfiles_obj);
          return ret;
        }
      ret = register_handle(dev_name, &generic_epfile_handle_ops, epfile, &dev->obj);
      if (ret != 0)
        {
          usb_err("chardev register failed!");
          usbobj_put(&dev->epfiles_obj);
          return ret;
        }
      epfile->is_created = true;
      uint32_t mask = 1 << (dev->minor_offset * EVENT_FOR_FUNCTION + 1 + i);
      eps_event_mask |= mask;
      epfile->event_mask = mask;
      (VOID)LOS_EventInit(&epfile->sync_event);
      usbobj_add(&epfile->obj, &dev->epfiles_obj);
    }

  return 0;
}

void generic_eps_disable(struct generic_dev_s *dev)
{
  struct generic_ep *ep = dev->eps;
  uint32_t count = dev->eps_count;
  struct generic_epfile *epfile;
  unsigned long flags;

  spin_lock_irqsave(&dev->eps_lock, flags);
  while (count--)
    {
      if (ep->ep)
        {
          EP_DISABLE(ep->ep);
        }

      if (ep->priv)
        {
          epfile = ep->priv;
          epfile->ep = NULL;
          ep->priv   = NULL;
        }
      ++ep;
    }
  dev->eps_enbale = false;
  spin_unlock_irqrestore(&dev->eps_lock, flags);
}

static int epfile_match_unused(struct usb_obj *obj, void *match_data)
{
  struct generic_epfile *epfile = container_of(obj, struct generic_epfile, obj);

  return epfile->ep == NULL;
}

int generic_eps_enable(struct generic_dev_s *dev)
{
  struct generic_ep *ep         = dev->eps;
  uint32_t count                = dev->eps_count;
  struct generic_epfile *epfile;
  struct usb_obj *obj;
  unsigned long flags;
  int ret = 0;

  dprintf("%s, dev->eps_count = %d \n",__func__, dev->eps_count);

  spin_lock_irqsave(&dev->eps_lock, flags);
  while (count--)
    {
      usb_endpoint_descriptor_t *desc;
      /* find a unused epfile and bind to a endpoint */
      obj = usbobj_find(&dev->epfiles_obj, epfile_match_unused, NULL);
      if (obj == NULL)
        {
          usb_err("can't find a unused epfile\n");
          break;
        }
      ep->ep->priv = ep;
      EP_DISABLE(ep->ep);
      switch (dev->speed)
        {
        case USB_SPEED_SUPER:
          desc = ep->descs[2];
          break;
        case USB_SPEED_HIGH:
          desc = ep->descs[1];
          break;
        default:
          desc = ep->descs[1];
          break;
        }
      ret = EP_CONFIGURE(ep->ep, desc, 0);
      if (ret < 0)
        {
          usb_err("Config ep%d failed!\n", ep->num);
          break;
        }

      epfile = container_of(obj, struct generic_epfile, obj);
      ep->priv     = epfile;
      epfile->ep   = ep;
      epfile->in   = USB_ISEPIN(desc->bEndpointAddress);
      epfile->isoc = ((desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
                     USB_ENDPOINT_XFER_ISOC);
      ++ep;
    }
  dev->eps_enbale = true;
  spin_unlock_irqrestore(&dev->eps_lock, flags);

  return ret;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
