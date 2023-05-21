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

#ifndef LITEOS_USB_OBJ_H
#define LITEOS_USB_OBJ_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include "obj_ref.h"
#include <linux/spinlock.h>
#include <linux/list.h>

/************************************************************************************
 * Public Functions
 ************************************************************************************/

struct usb_obj {
  const char *name;
  struct list_head entry;
  struct usb_obj *parent;
  struct list_head children;
  spinlock_t lock;
  struct obj_ref ref;
  struct list_head objres_head;
  spinlock_t objres_lock;
  void (*release)(struct usb_obj *obj);
};

typedef int (*usbobj_match_t)(struct usb_obj *obj, void *match_data);

extern int usbobj_add(struct usb_obj *obj, struct usb_obj *parent);
extern int usbobj_remove(struct usb_obj *obj);
extern struct usb_obj *usbobj_find(struct usb_obj *obj, usbobj_match_t match, void *match_data);
extern int usbobj_init(struct usb_obj *obj, const char *name, void (*release)(struct usb_obj *obj));
extern void usbobj_get(struct usb_obj *obj);
extern void usbobj_put(struct usb_obj *obj);
extern void usbobj_for_each_child(struct usb_obj *obj,
                                  usbobj_match_t match, void *match_data,
                                  void (*fn)(struct usb_obj *, void *),
                                  void *data);
extern void objres_release_all(struct usb_obj *obj);

extern void *usbm_malloc(struct usb_obj *obj, size_t size);
extern void *usbm_zalloc(struct usb_obj *obj, size_t size);
extern void usbm_free(struct usb_obj *obj, void *p);
extern char *usbm_strdup(struct usb_obj *obj, const char *s);

static inline int usbobj_default_match(struct usb_obj *obj, void *match_data)
{
  return !strcmp(obj->name, (char *)match_data);
}

/* the obj has to be the first memter */
static inline void usbobj_default_release(struct usb_obj *obj)
{
  usbm_free(obj->parent, obj);
}

#endif /* LITEOS_USB_OBJ_H */
