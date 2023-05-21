/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Composite Devices
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
#include "usb_handle.h"
#include "f_common.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static LINUX_LIST_HEAD(g_hanle_list);
static DEFINE_SPINLOCK(g_hanle_lock);
static unsigned int devmap = 0;

static int alloc_fd_for_handle(void)
{
  int bitsize = 0x8 * sizeof (devmap);
  bool found = false;
  int i;

  for (i = 1; i < bitsize; i++)
    {
      if ((devmap & (1 << i)) == 0)
        {
          devmap |= (1 << i);
          found = true;
          break;
        }
    }
  return found ? i : -1;
}

static int free_fd_for_handle(int fd)
{
  if (fd <= 0 || fd > 31)
    {
      return -1;
    }
  devmap &= (~(1 << fd));

  return 0;
}

static struct usb_handle *handle_find_by_name(const char *devName)
{
  struct usb_handle *handle = NULL;
  bool found = false;
  uint32_t flags;

  if (!devName)
    {
      return NULL;
    }

  spin_lock_irqsave(&g_hanle_lock, flags);
  list_for_each_entry(handle, &g_hanle_list, entry)
    {
      if (strcmp(handle->name, devName) == 0)
        {
          found = true;
          break;
        }
    }
  spin_unlock_irqrestore(&g_hanle_lock, flags);

  return found ? handle : NULL;
}

static struct usb_handle *handle_find_by_fd(int fd)
{
  struct usb_handle *handle = NULL;
  bool found = false;
  uint32_t flags;

  if (fd <= 0)
    {
      return NULL;
    }

  spin_lock_irqsave(&g_hanle_lock, flags);
  list_for_each_entry(handle, &g_hanle_list, entry)
    {
      if (handle->fd == fd)
        {
          found = true;
          break;
        }
    }
  spin_unlock_irqrestore(&g_hanle_lock, flags);

  return found ? handle : NULL;
}

int register_handle(char *devName, struct UsbHandleOps *ops, void *priv, struct usb_obj *obj)
{
  struct usb_handle *handle = NULL;
  uint32_t flags;

  if (devName == NULL || ops == NULL) {
    return -1;
  }
  handle = handle_find_by_name(devName);
  if (handle)
    {
      return -1;
    }
  handle = (struct usb_handle *)usbm_malloc(obj, sizeof(*handle));
  if (!handle)
    {
      return -1;
    }
  handle->name = devName;
  handle->fd = -1;
  handle->ops = ops;
  handle->obj = obj;
  handle->priv = priv;
  spin_lock_irqsave(&g_hanle_lock, flags);
  list_add_tail(&handle->entry, &g_hanle_list);
  spin_unlock_irqrestore(&g_hanle_lock, flags);

  return 0;
}

int unregister_handle(const char *devName)
{
  uint32_t flags;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_name(devName);
  if (handle == NULL)
    {
      return -1;
    }
  spin_lock_irqsave(&g_hanle_lock, flags);
  list_del_init(&handle->entry);
  spin_unlock_irqrestore(&g_hanle_lock, flags);
  usbm_free(handle->obj, handle->name);
  usbm_free(handle->obj, handle);

  return 0;
}

int handle_open(const char *name)
{
  int fd;
  struct usb_handle *handle = NULL;
  if (name == NULL)
    {
      return -1;
    }
  handle = handle_find_by_name(name);
  if (handle == NULL)
    {
      return -1;
    }
  if (handle->fd > 0)
    {
      return -1;
    }
  fd = alloc_fd_for_handle();
  if (fd <= 0)
    {
      return -1;
    }
  handle->fd = fd;
  if (handle->ops && handle->ops->open)
    {
      handle->ops->open(handle);
    }
  return fd;
}

int handle_close(int fd)
{
  int ret = -1;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return -1;
    }
  if (handle->ops && handle->ops->close)
    {
      ret = handle->ops->close(handle);
      if (ret)
        {
          return ret;
        }
    }
  handle->fd = -1;
  free_fd_for_handle(fd);

  return ret;
}

int handle_read(int fd, void *buffer, size_t len)
{
  int ret = -1;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return -1;
    }
  if (handle->ops && handle->ops->read)
    {
      ret = handle->ops->read(handle, buffer, len);
    }

  return ret;
}

int handle_write(int fd, void *buffer, size_t len)
{
  int ret = -1;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return -1;
    }
  if (handle->ops && handle->ops->write)
    {
      ret = handle->ops->write(handle, buffer, len);
    }

  return ret;
}

int handle_ioctl(int fd, int cmd, void *arg)
{
  int ret = -1;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return -1;
    }
  if (handle->ops && handle->ops->ioctl)
    {
      ret = handle->ops->ioctl(handle, cmd, arg);
    }

  return ret;
}

void *handle_mmap(int fd, uint32_t size)
{
  void *mem = NULL;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return NULL;
    }
  if (handle->ops && handle->ops->mmap)
    {
      mem = handle->ops->mmap(handle, size);
    }

  return mem;
}

int handle_poll(int fd, int timeout)
{
  int ret = -1;
  struct usb_handle *handle = NULL;

  handle = handle_find_by_fd(fd);
  if (handle == NULL)
    {
      dprintf("%s, can not found hand, fd = %d\n", __func__, fd);
      return -1;
    }
  if (handle->ops && handle->ops->poll)
    {
      ret = handle->ops->poll(handle, timeout);
    }

  return ret;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
