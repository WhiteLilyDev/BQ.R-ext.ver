/****************************************************************************
 * drivers/usbdev/composite_desc.c
 *
 *   Copyright (C) 2011-2012, 2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
/****************************************************************************
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations,
 * which might include those applicable to Huawei LiteOS of U.S. and the country in
 * which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in
 * compliance with such applicable export control laws and regulations.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "gadget/composite.h"

#ifdef CONFIG_USBDEV_COMPOSITE

#define OS_STRING_SIGN_LEN 18
#define OS_STRING_ID       0xEE

static const char g_os_string[OS_STRING_SIGN_LEN] =
{
  OS_STRING_SIGN_LEN,
  UDESC_STRING,
  'L', 0, 'i', 0, 't', 0, 'e', 0,
  'O', 0, 'S', 0, ' ', 0, 0, 0
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: composite_mkstrdesc
 *
 * Description:
 *   Construct a string descriptor
 *
 ****************************************************************************/

int composite_mkstrdesc(struct composite_dev_s *priv, uint8_t id, uint16_t index, uint8_t *buf)
{
  int len = -1;
  int i;
  int ret;

  if (index == 0 && id == OS_STRING_ID)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (void *)g_os_string, g_os_string[0]);
      if (ret != EOK)
        {
          return -1;
        }
      return g_os_string[0];
    }

  for (i = 0; i < priv->ndevices; i++)
    {
      len = priv->device[i].compdesc.mkstrdesc(id, buf);
    }

  return len;
}

void composite_mkdevdesc(struct composite_dev_s *priv, uint8_t *buf)
{
  int i;

  for (i = 0; i < priv->ndevices; i++)
    {
      priv->device[i].compdesc.mkdevdesc(buf);
    }
}

int16_t composite_mkcfgdesc_sub(uint8_t *buf, int16_t len)
{
  uint8_t *buf_tmp;
  struct usb_descriptor *desc;
  int16_t new_config_len = 0;
  int ret;
  int i;

  buf_tmp = (uint8_t *)malloc(len);
  if (buf_tmp == NULL)
    {
      return -1;
    }

  ret = memcpy_s(buf_tmp, len, buf, USB_CONFIG_DESC_SIZE);
  if (ret != EOK)
    {
      goto err;
    }
  len -= USB_CONFIG_DESC_SIZE;
  new_config_len += USB_CONFIG_DESC_SIZE;

  desc = (struct usb_descriptor *)(buf + USB_CONFIG_DESC_SIZE);
  i = 0;
  while (i < len)
    {
      i += desc->bLength;
      if (desc->bDescriptorType == UDESC_CONFIG)
        {
          desc = (struct usb_descriptor *)((char *)desc + desc->bLength);
          continue;
        }
      ret = memcpy_s(buf_tmp + new_config_len, (len + USB_CONFIG_DESC_SIZE - new_config_len),
                     desc, desc->bLength);
      if (ret != EOK)
        {
          goto err;
        }
      new_config_len += desc->bLength;
      desc = (struct usb_descriptor *)((char *)desc + desc->bLength);
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, buf_tmp, new_config_len);
  if (ret != EOK)
    {
      goto err;
    }
  free(buf_tmp);

  return new_config_len;

err:
  free(buf_tmp);
  return -1;
}

/****************************************************************************
 * Name: composite_mkcfgdesc
 *
 * Description:
 *   Construct the configuration descriptor
 *
 ****************************************************************************/

int16_t composite_mkcfgdesc(struct composite_dev_s *priv, uint8_t *buf)
{
  struct usb_config_descriptor *config_desc;
  uint8_t *buf_tmp = buf;
  int16_t len;
  int16_t total = 0;
  int16_t config_len;
  int i;

  /* Copy all contained interface descriptors into the buffer too */

  for (i = 0; i < priv->ndevices; i++)
    {
      len = priv->device[i].compdesc.mkconfdesc(buf_tmp, &priv->device[i].compdesc.devinfo);
      if (len <= 0)
        {
          return -1;
        }
      total   += len;
      buf_tmp += len;
    }

  if (priv->ndevices > 1)
    {
      config_len = total;
      total      = composite_mkcfgdesc_sub(buf, config_len);
      if (total < 0)
        {
          return -1;
        }

      config_len  = total;
      config_desc = (struct usb_config_descriptor *)buf;
      USETW(config_desc->wTotalLength, config_len);
      config_desc->bNumInterface = priv->ninterfaces;
    }

  return total;
}

#endif /* CONFIG_USBDEV_COMPOSITE */
