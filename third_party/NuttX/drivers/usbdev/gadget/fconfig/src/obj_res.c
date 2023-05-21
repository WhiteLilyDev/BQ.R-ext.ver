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

#include "f_common.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct obj_res {
  struct list_head entry;
  uint8_t data[0];
};

void *objres_alloc(size_t size)
{
  size_t total_size;
  struct obj_res *res;

  total_size = sizeof(struct obj_res) + size;
  res = malloc(total_size);
  if (res == NULL)
    {
      return NULL;
    }

  INIT_LIST_HEAD(&res->entry);
  return res->data;
}

void objres_free(void *res)
{
  if (res)
    {
      struct obj_res *objRes = container_of(res, struct obj_res, data);
      free(objRes);
    }
}

void objres_add(struct usb_obj *obj, void *res)
{
  struct obj_res *objRes = container_of(res, struct obj_res, data);
  uint32_t flags;

  spin_lock_irqsave(&obj->objres_lock, flags);
  list_add_tail(&objRes->entry, &obj->objres_head);
  spin_unlock_irqrestore(&obj->objres_lock, flags);
}

static struct obj_res *objres_find(struct usb_obj *obj, void *res)
{
  struct obj_res *rc;

  list_for_each_entry_reverse(rc, &obj->objres_head, entry)
    {
      if (rc->data != res)
        {
          continue;
        }
      return rc;
    }

  return NULL;
}

int objres_remove(struct usb_obj *obj, void *res)
{
  struct obj_res *rc;
  uint32_t flags;

  spin_lock_irqsave(&obj->objres_lock, flags);
  rc = objres_find(obj, res);
  if (rc)
    {
      list_del_init(&rc->entry);
    }
  spin_unlock_irqrestore(&obj->objres_lock, flags);

  return rc ? 0 : -1;
}

void objres_release_all(struct usb_obj *obj)
{
  struct obj_res *res, *tmp;
  uint32_t flags;

  spin_lock_irqsave(&obj->objres_lock, flags);
  list_for_each_entry_safe_reverse(res, tmp, &obj->objres_head, entry)
    {
      list_del(&res->entry);
      free(res);
    }
  spin_unlock_irqrestore(&obj->objres_lock, flags);
}

void *usbm_malloc(struct usb_obj *obj, size_t size)
{
  void *res;

  if (!size)
    {
      return NULL;
    }

  res = objres_alloc(size);
  if (res == NULL)
    {
      return NULL;
    }

  objres_add(obj, res);
  return res;
}

void *usbm_zalloc(struct usb_obj *obj, size_t size)
{
  void *data;

  data = usbm_malloc(obj, size);
  if (data)
    {
      memset_s(data, size, 0, size);
    }
  return data;
}

void usbm_free(struct usb_obj *obj, void *p)
{
  int ret;

  if (p == NULL)
    {
      return;
    }

  ret = objres_remove(obj, p);
  if (!ret)
    {
      objres_free(p);
    }
}

char *usbm_strdup(struct usb_obj *obj, const char *s)
{
  size_t size;
  char *buf;

  if (s == NULL)
    {
      return NULL;
    }

  size = strlen(s) + 1;
  buf = usbm_malloc(obj, size);
  if (buf)
    {
      memcpy_s(buf, size, s, size);
    }
  return buf;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
