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
#include "obj_ref.h"
#include "usb_obj.h"
#include "f_common.h"
#include <linux/spinlock.h>

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int usbobj_init(struct usb_obj *obj, const char *name,
                void (*release)(struct usb_obj *obj))
{
  if (!obj)
  {
    return -1;
  }

  obj->name = name;
  obj->parent = NULL;
  objref_init(&obj->ref, 1);
  INIT_LIST_HEAD(&obj->entry);
  INIT_LIST_HEAD(&obj->children);
  INIT_LIST_HEAD(&obj->objres_head);
  spin_lock_init(&obj->objres_lock);
  spin_lock_init(&obj->lock);
  obj->release = release;

  return 0;
}

int usbobj_add(struct usb_obj *obj, struct usb_obj *parent)
{
  uint32_t flags;

  if (!obj || !parent)
    {
      return -1;
    }

  usbobj_get(parent);
  spin_lock_irqsave(&parent->lock, flags);
  list_add_tail(&obj->entry, &parent->children);
  obj->parent = parent;
  spin_unlock_irqrestore(&parent->lock, flags);

  return 0;
}

int usbobj_remove(struct usb_obj *obj)
{
  struct usb_obj *parent;
  uint32_t flags;

  if (!obj || !obj->parent)
    {
      return -1;
    }

  parent = obj->parent;
  spin_lock_irqsave(&parent->lock, flags);
  list_del_init(&obj->entry);
  spin_unlock_irqrestore(&parent->lock, flags);
  obj->parent = NULL;

  return 0;
}

struct usb_obj *usbobj_find(struct usb_obj *obj, usbobj_match_t match, void *match_data)
{
  struct usb_obj *child;
  bool found = false;
  uint32_t flags;

  if (!obj)
    {
      return NULL;
    }

  spin_lock_irqsave(&obj->lock, flags);
  list_for_each_entry(child, &obj->children, entry)
    {
      if (match(child, match_data))
        {
          found = true;
          break;
        }
    }
  spin_unlock_irqrestore(&obj->lock, flags);

  return found ? child : NULL;
}

void usbobj_get(struct usb_obj *obj)
{
  if (obj)
    {
      objref_get(&obj->ref);
    }
}

static void usbobj_release_child(struct usb_obj *obj, void *data)
{
  (void)data;
  usbobj_put(obj);
}

static void usbobj_release(struct obj_ref *ref)
{
  struct usb_obj *obj = container_of(ref, struct usb_obj, ref);
  struct usb_obj *parent = obj->parent;
  uint32_t flags;

  if (parent)
    {
      spin_lock_irqsave(&parent->lock, flags);
      list_del_init(&obj->entry);
      spin_unlock_irqrestore(&parent->lock, flags);
    }

  usbobj_for_each_child(obj, NULL, NULL, usbobj_release_child, NULL);

  objres_release_all(obj);
  if (obj->release)
    {
      obj->release(obj);
    }

  if (parent)
    {
      usbobj_put(parent);
    }
}

void usbobj_put(struct usb_obj *obj)
{
  if (obj)
    {
      (void)objref_put(&obj->ref, usbobj_release);
    }
}

void usbobj_for_each_child(struct usb_obj *obj,
                           usbobj_match_t match, void *match_data,
                           void (*fn)(struct usb_obj *, void *),
                           void *data)
{
  struct usb_obj *child, *tmp;
  uint32_t flags;

  if (!obj || !fn)
    {
      return;
    }

  spin_lock_irqsave(&obj->lock, flags);
  list_for_each_entry_safe(child, tmp, &obj->children, entry)
    {
      if (match && !match(child, match_data))
        {
          continue;
        }
      fn(child, data);
    }
  spin_unlock_irqrestore(&obj->lock, flags);
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
