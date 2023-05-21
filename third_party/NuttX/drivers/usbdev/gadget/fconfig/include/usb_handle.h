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

#ifndef LITEOS_USB_HANDLE_H
#define LITEOS_USB_HANDLE_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include "gadget/usbdev.h"
#include "usb_obj.h"
/************************************************************************************
 * Public Functions
 ************************************************************************************/

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct usb_handle {
  char *name;
  int fd;
  struct list_head entry;
  struct UsbHandleOps *ops;
  struct usb_obj *obj;
  void *priv;
};

struct UsbHandleOps {
  int (*open)(struct usb_handle *handle);
  int (*close)(struct usb_handle *handle);
  int (*read)(struct usb_handle *handle, void *buffer, size_t len);
  int (*write)(struct usb_handle *handle, void *buffer, size_t len);
  int (*ioctl)(struct usb_handle *handle, int cmd, void *arg);
  void *(*mmap)(struct usb_handle *handle, uint32_t size);
  int (*poll)(struct usb_handle *handle, int timeout);
};

extern int register_handle(char *devName, struct UsbHandleOps *ops, void *priv, struct usb_obj *obj);
extern int unregister_handle(const char *devName);
extern int alloc_fd_for_handle(void);
extern int handle_open(const char *name);
extern int handle_close(int fd);
extern int handle_read(int fd, void *buffer, size_t len);
extern int handle_write(int fd, void *buffer, size_t len);
extern int handle_ioctl(int fd, int cmd, void *arg);
extern void *handle_mmap(int fd, uint32_t size);
extern int handle_poll(int fd, int timeout);

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* LITEOS_USB_OBJ_H */
