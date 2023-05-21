/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS USB Driver HID Data Stream
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

#include <poll.h>
#include "gadget/usbd_hid.h"
#include "implementation/global_implementation.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static int hid_open(FAR struct file *filep);
static int hid_close(FAR struct file *filep);
static ssize_t hid_read(FAR struct file *filep, FAR char *buffer, size_t buflen);
static ssize_t hid_write(FAR struct file *filep, FAR const char *buffer, size_t buflen);
static ssize_t hid_poll(FAR struct file *filep, poll_table *fds);

static const struct file_operations_vfs g_hid_fops =
{
  .open   = hid_open,
  .close  = hid_close,
  .read   = hid_read,
  .write  = hid_write,
  .seek   = NULL,
  .ioctl  = NULL,
  .mmap   = NULL,
#ifndef CONFIG_DISABLE_POLL
  .poll   = hid_poll,
#endif
  .unlink = NULL,
};

static int hid_open(FAR struct file *filep)
{
  struct hid_dev_s *hid = (struct hid_dev_s *)filep->f_inode->i_private;

  if (hid == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  atomic_set(&hid->open_flag, 1);

  return 0;
}

static int hid_close(FAR struct file *filep)
{
  struct hid_dev_s *hid = (struct hid_dev_s *)filep->f_inode->i_private;

  if (hid == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  atomic_set(&hid->open_flag, 0);

  return 0;
}

int hid_fops_init(struct hid_dev_s *hid)
{
  int ret;

  hid->open_flag = 0;
  ret = register_driver(USB_HID_DEV, &g_hid_fops, O_RDWR, hid);
  if (ret != OK)
    {
      usb_err("hid_dev register failed!\n");
    }

  return ret;
}

int hid_fops_deinit(const struct hid_dev_s *hid)
{
  int ret;

  if (hid == NULL)
    {
      return -1;
    }

  ret = unregister_driver(USB_HID_DEV);
  if (ret != OK)
    {
      usb_err("hid_dev unregister failed!\n");
    }

  return ret;
}

struct hid_queue_node *hid_queue_node_alloc(size_t len)
{
  struct hid_queue_node *queue_node;

  queue_node = zalloc(sizeof(struct hid_queue_node));
  if (queue_node == NULL)
    {
      usb_err("Malloc hid queue node failed\n");
      return queue_node;
    }

  queue_node->buf_len  = len;
  queue_node->buf_used = 0;
  queue_node->buf      = malloc(len);
  if (queue_node->buf == NULL)
    {
      free(queue_node);
      usb_err("Malloc hid queue node buf failed\n");
      return NULL;
    }

  return queue_node;
}

void hid_queue_node_free(struct hid_queue_node *node)
{
  free(node->buf);
  node->buf = NULL;
  free(node);
}

void hid_queue_free(struct hid_dev_s *sc)
{
  struct hid_queue_node *node;
  struct hid_dev_s *fhid_sc = sc;

  if (fhid_sc->cur_node != NULL)
    {
      hid_queue_node_free(fhid_sc->cur_node);
      fhid_sc->cur_node = NULL;
    }

  while (!list_empty(&fhid_sc->hid_queue))
    {
      node = list_first_entry(&fhid_sc->hid_queue, struct hid_queue_node, irqqueue);
      list_del_init(&node->irqqueue);
      hid_queue_node_free(node);
    }
  INIT_LIST_HEAD(&fhid_sc->hid_queue);

  fhid_sc->hid_queue_len = 0;
}

void hid_send_data_sub(struct hid_dev_s *sc)
{
  struct hid_dev_s *fhid = sc;
  struct hid_queue_node *node;
  struct usbdev_req_s *req;

  node = fhid->cur_node;
  req  = &fhid->inputreq;

  atomic_set(&fhid->busy_flag, 1);

  if (node->buf_len > HID_IN_DATA_SIZE)
    {
      req->len = HID_IN_DATA_SIZE;
    }
  else
    {
      req->len = node->buf_len;
    }
  req->buf = node->buf + node->buf_used;

  node->buf_len -= req->len;
  node->buf_used += req->len;
  (void)EP_SUBMIT(fhid->in_ep, req);
}

static ssize_t hid_poll(FAR struct file *filep, poll_table *fds)
{
  struct hid_dev_s *hid = (struct hid_dev_s *)filep->f_inode->i_private;
  uint32_t revents = 0;
  uint32_t ret;
  uint32_t value = fds->key & (POLLIN | POLLRDNORM);

  if (hid == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return (POLLERR);
    }

  if (value)
    {
      ret = LOS_EventPoll(&hid->read_event.uwEventID, USB_HID_READ_EVENT, LOS_WAITMODE_OR);
      if (ret == USB_HID_READ_EVENT)
        {
          revents |= value;
        }
    }

  return (ssize_t)revents;
}

static ssize_t hid_read(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
  struct hid_dev_s *hid = (struct hid_dev_s *)filep->f_inode->i_private;
  uint32_t flags;
  errno_t ret;

  if (buflen == 0)
    {
      usb_err("buflen is 0!\n");
      return -1;
    }

  if (buflen > HID_OUT_DATA_SIZE)
    {
      usb_err("buflen exceeds buffer size: %u\n", HID_OUT_DATA_SIZE);
      return -1;
    }

  if (hid == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  (VOID)LOS_EventRead(&hid->read_event, USB_HID_READ_EVENT, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

  spin_lock_irqsave(&hid->hid_lock, flags);
  ret = usbd_copy_to_user(buffer, buflen, (const void *)hid->read_buf, hid->read_len);
  spin_unlock_irqrestore(&hid->hid_lock, flags);
  if (ret != EOK)
    {
      usb_err("memcpy failed!\n");
      return -1;
    }

  return 0;
}

static ssize_t hid_write(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
  struct hid_queue_node *node;
  struct hid_queue_node *node_temp;
  struct hid_dev_s *hid = (struct hid_dev_s *)filep->f_inode->i_private;
  uint32_t flags;
  int ret;

  if (buflen == 0)
    {
      usb_err("buflen is 0!\n");
      return -1;
    }

  if (hid == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  node = hid_queue_node_alloc(buflen);
  if (node == NULL)
    {
      return -1;
    }

  /* buflen represents the size of the write data */

  ret = usbd_copy_from_user(node->buf, buflen, buffer, buflen);
  if (ret != 0)
    {
      hid_queue_node_free(node);
      return -1;
    }
  spin_lock_irqsave(&hid->hid_lock, flags);
  if (hid->hid_queue_len > 8)
    {
      hid_queue_free(hid);
    }

  list_add_tail(&node->irqqueue, &hid->hid_queue);
  hid->hid_queue_len++;

  if (atomic_read(&hid->busy_flag))
    {
      spin_unlock_irqrestore(&hid->hid_lock, flags);
      return 0;
    }

  if (!list_empty(&hid->hid_queue))
    {
      node_temp = list_first_entry(&hid->hid_queue, struct hid_queue_node, irqqueue);
      list_del_init(&node_temp->irqqueue);
      hid->hid_queue_len--;
      hid->cur_node = node_temp;
      hid_send_data_sub(hid);
    }
  spin_unlock_irqrestore(&hid->hid_lock, flags);

  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
