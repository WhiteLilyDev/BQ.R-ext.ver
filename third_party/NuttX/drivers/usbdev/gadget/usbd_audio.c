/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Audio Stream
 * Author: huangjieliang
 * Create: 2017-12-12
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

#include "gadget/usbd_audio.h"
#include "gadget/f_uac.h"
#include "implementation/global_implementation.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static struct uac_softc *get_uac(void)
{
  devclass_t dc;
  device_t dev;
  struct composite_softc *cdev;
  struct composite_devobj_s *devobj;
  struct usbdevclass_driver_s *drvr;
  struct uac_driver_s *driver;

  dc = devclass_find("composite");
  if (dc == NULL)
    {
      PRINT_ERR("%s is failed\n", __FUNCTION__);
      return NULL;
    }
  dev = devclass_get_device(dc, 0);
  if (dev == NULL)
    {
      PRINT_ERR("%s, get dev failed\n", __FUNCTION__);
      return NULL;
    }
  cdev = (struct composite_softc *)device_get_softc(dev);
  if (cdev == NULL)
    {
      PRINT_ERR("%s, get softc failed\n", __FUNCTION__);
      return NULL;
    }

  devobj = usbclass_devobj_get(&cdev->dev, DEV_UAC);
  if (devobj == NULL)
    {
      return NULL;
    }

  drvr = devobj->dev;
  if (drvr == NULL)
    {
      PRINT_ERR("%s, get driver failed\n", __FUNCTION__);
      return NULL;
    }
  driver = (struct uac_driver_s *)drvr;

  return container_of(driver, struct uac_softc, drvr);
}

static int fuac_host_connected(struct uac_dev_s *fuac)
{
  if (fuac == NULL)
    {
      return 0;
    }

  return fuac->connected;
}

static int uac_wait_hot_sub(void)
{
  int ret;
  struct uac_softc *fuac;
  struct uac_dev_s *dev;

  mtx_lock(&usb_mtx);
  fuac = get_uac();
  if (fuac == NULL)
    {
      mtx_unlock(&usb_mtx);
      return UAC_ERROR_PTR;
    }
  dev = &fuac->dev;
  ret = fuac_host_connected(dev) ;
  mtx_unlock(&usb_mtx);

  return ret;
}

int uac_wait_host(int wait_option)
{
  int ret;

  switch (wait_option)
    {
    case UAC_WAIT_HOST_NOP:
      ret = uac_wait_hot_sub();
      if (ret == 0) /* Device is not connected. */
        {
          ret = UAC_ERROR_NOMATCH;
        }
      else if (ret == 1) /* Device is connected. */
        {
          ret = UAC_OK;
        }
      break;

    case UAC_WAIT_HOST_FOREVER:
      while (!(ret = uac_wait_hot_sub()))
        {
          (void)LOS_TaskDelay(10);
        }

      if (ret != UAC_ERROR_PTR)
        {
          ret = UAC_OK;
        }
      break;

    default:
      return UAC_ERROR_VALUE;
    }

  return ret;
}

struct uac_queue_node *uac_queue_node_alloc(unsigned len)
{
  struct uac_queue_node *queue_node;

  queue_node = zalloc(sizeof(struct uac_queue_node));
  if (queue_node == NULL)
    {
      PRINT_ERR("%s, malloc uac queue node failed\n", __FUNCTION__);
      return NULL;
    }

  queue_node->buf_len  = len;
  queue_node->buf_used = 0;
  queue_node->buf      = malloc(len);
  if (queue_node->buf == NULL)
    {
      free(queue_node);
      PRINT_ERR("%s, malloc uac queue node buf failed\n", __FUNCTION__);
      return NULL;
    }

  return queue_node;
}

void uac_queue_node_free(struct uac_queue_node *node)
{
  free(node->buf);
  node->buf = NULL;
  free(node);
}

void uac_send_data_sub(struct uac_dev_s *sc)
{
  struct uac_dev_s *fuac      = sc;
  struct uac_queue_node *node = fuac->cur_node;
  struct usbdev_req_s *req    = &fuac->inputreq;

  atomic_set(&fuac->busy_flag, 1);

  if (node->buf_len > g_uac_iso_data_size)
    {
      req->len = g_uac_iso_data_size;
    }
  else
    {
      req->len = node->buf_len;
    }
  req->buf = node->buf + node->buf_used;

  node->buf_len  -= req->len;
  node->buf_used += req->len;
  (void)EP_SUBMIT(fuac->in_ep, req);
}

void uac_queue_free(struct uac_dev_s *sc)
{
  struct uac_queue_node *node;
  struct uac_dev_s *fuac_sc = sc;

  if (fuac_sc->cur_node != NULL)
    {
      uac_queue_node_free(fuac_sc->cur_node);
      fuac_sc->cur_node = NULL;
    }

  while (!list_empty(&fuac_sc->uac_queue))
    {
      node = list_first_entry(&fuac_sc->uac_queue, struct uac_queue_node, irqqueue);
      list_del_init(&node->irqqueue);
      uac_queue_node_free(node);
    }
  INIT_LIST_HEAD(&fuac_sc->uac_queue);

  fuac_sc->uac_queue_len = 0;
}

int fuac_send_message(const void *buf, int len)
{
  struct uac_softc *fuac;
  struct uac_dev_s *dev;
  struct uac_queue_node *node;
  struct uac_queue_node *node_tx;
  uint32_t flags;
  int ret = 0;

  if (buf == NULL || len < 0)
    {
      return UAC_ERROR_PTR;
    }

  node = uac_queue_node_alloc(len);
  if (node == NULL)
    {
      return UAC_ERROR_MEMORY;
    }
  (void)memcpy_s(node->buf, len, buf, len);

  mtx_lock(&usb_mtx);
  fuac = get_uac();
  if (fuac == NULL)
    {
      uac_queue_node_free(node);
      PRINT_ERR("%s, get uac failed\n", __FUNCTION__);
      ret = UAC_ERROR_PTR;
      goto error;
    }
  dev = &fuac->dev;
  spin_lock_irqsave(&dev->lock, flags);
  if (!dev->connected)
    {
      uac_queue_node_free(node);
      goto done;
    }

  if (dev->uac_queue_len > 8)
    {
      uac_queue_free(dev);
    }

  list_add_tail(&node->irqqueue, &dev->uac_queue);
  dev->uac_queue_len++;

  if (atomic_read(&dev->busy_flag))
    {
      goto done;
    }

  (void)EP_DISABLE(dev->in_ep);
  (void)EP_CONFIGURE(dev->in_ep, (const usb_endpoint_descriptor_t *)&g_fuac_as_in_ep_desc, 0);

  if (!list_empty(&dev->uac_queue))
    {
      node_tx = list_first_entry(&dev->uac_queue, struct uac_queue_node, irqqueue);
      list_del_init(&node_tx->irqqueue);
      dev->uac_queue_len--;
      dev->cur_node = node_tx;
      uac_send_data_sub(dev);
    }

done:
  spin_unlock_irqrestore(&dev->lock, flags);

error:
  mtx_unlock(&usb_mtx);
  return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */