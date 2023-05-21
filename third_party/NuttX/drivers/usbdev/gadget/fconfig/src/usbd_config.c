/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS USB Driver Config Data Stream
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

#include <poll.h>
#include "implementation/global_implementation.h"
#include "gadget/composite.h"
#include "f_common.h"
#include "devsvc_manager_clnt.h"
#include "hdf_device_desc.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct func_info {
  uint8_t ndevices;
  int ifnobase;
  int strbase;
  struct composite_devdesc_s *devs;
};

extern void usbdev_generic_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void usbdev_cdcacm_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void usbdev_rndis_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void usbdev_mass_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void usbdev_uac_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern void usbdev_uvc_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern int gadget_strings_match(struct usb_obj *obj, void *match_data);

struct usbdev_info g_generic_usbdev_info =
{
  "generic", DEV_GENERIC, 0, usbdev_generic_initialize_sub
};

struct usbdev_info *g_fconfig_usbdev_info[] =
{
  &g_generic_usbdev_info,
  NULL,
};

static struct usbdev_info *fconfig_find_usbdev_info(char *name)
{
  struct usbdev_info **dev_info = g_fconfig_usbdev_info;
  int i;

  for (i = 0; dev_info[i] != NULL; i++)
    {
      if (strcmp(dev_info[i]->name, name) == 0)
        {
          return dev_info[i];
        }
    }

  return NULL;
}

static char *fconfig_get_string(struct usb_obj *obj, struct fconfig_string *str)
{
  size_t size = str->len;
  char *buf = NULL;
  int ret;
  buf = (char *)usbm_malloc(obj, (size + 1));
  if (buf == NULL)
    {
      usb_err("%s: malloc name buffer failed\n", __FUNCTION__);
      return NULL;
    }
  memset_s(buf, size, 0, size);

  ret = memcpy_s((void *)buf, (size + 1), (void *)str->s, size);
  if (ret != EOK)
    {
      usb_err("%s: copy string from user failed\n", __FUNCTION__);
      usbm_free(obj, buf);
      return NULL;
    }
  *(buf + str->len) = '\0';
  return buf;
}

static int gadget_match(struct usb_obj *obj, void *match_data)
{
  struct gadget_info *gi = container_of(obj, struct gadget_info, obj);
  return !strcmp(gi->name, (char *)match_data);
}

static struct gadget_info *gadget_find_by_name(struct fconfig_softc *cdev,
                                               struct fconfig_string *name)
{
  struct gadget_info *gi = NULL;
  struct usb_obj *obj = NULL;
  char *gadget_name = NULL;

  gadget_name = fconfig_get_string(&cdev->obj, name);
  if (gadget_name == NULL)
    {
      usb_err("%s: get gadget name failed\n", __FUNCTION__);
      return NULL;
    }

  obj = usbobj_find(&cdev->obj, gadget_match, (void *)gadget_name);
  if (obj)
    {
      gi = container_of(obj, struct gadget_info, obj);
    }

  usbm_free(&cdev->obj, gadget_name);
  return gi;
}

static int gadget_make(struct fconfig_softc *cdev, char *name)
{
  struct gadget_info *gi = NULL;
  unsigned long flags;

  spin_lock_irqsave(&cdev->obj.lock, flags);
  if (!list_empty(&cdev->obj.children))
    {
      usb_err("%s, %d: gadget had make\n", __FUNCTION__, __LINE__);
      spin_unlock_irqrestore(&cdev->obj.lock, flags);
      return -1;
  }
  spin_unlock_irqrestore(&cdev->obj.lock, flags);

  gi = usbm_malloc(&cdev->obj, sizeof(*gi));
  if (gi == NULL)
    {
      usb_err("%s: malloc gadget failed\n", __FUNCTION__);
      return -1;
    }

  memset_s(gi, sizeof(*gi), 0, sizeof(*gi));
  usbobj_init(&gi->obj, name, usbobj_default_release);
  usbobj_init(&gi->strings, "strings", usbobj_default_release);
  gi->name = name;
  gi->cdev = cdev;
  gi->com_s = cdev->com_s;
  usbobj_add(&gi->obj, &cdev->obj);
  return 0;
}

static int gadget_drop(struct fconfig_softc *cdev, struct fconfig_string *name)
{
  struct gadget_info *gi = NULL;

  gi = gadget_find_by_name(cdev, name);
  if (gi == NULL)
    {
      usb_err("%s: can not find gadget\n", __FUNCTION__);
      return -1;
    }

  usbm_free(&cdev->obj, (void *)gi->name);
  usbobj_put(&gi->strings);
  usbobj_put(&gi->obj);
  usbm_free(&cdev->obj, gi);
  return 0;
}

static int fconfig_make_gadget(struct fconfig_softc *cdev, void *user_data)
{
  struct fconfig_string str;
  char *name = NULL;
  int ret;

  ret = memcpy_s(&str, sizeof(str), user_data, sizeof(str));
  if (ret != EOK)
    {
      return -1;
    }

  name = fconfig_get_string(&cdev->obj, &str);
  if (name == NULL)
    {
      usb_err("%s: get gadget name failed\n", __FUNCTION__);
      return -1;
    }

  return gadget_make(cdev, name);
}

static int fconfig_drop_gadget(struct fconfig_softc *cdev, void *user_data)
{
  struct fconfig_string str;
  int ret;

  ret = memcpy_s(&str, sizeof(str), user_data, sizeof(str));
  if (ret != EOK)
    {
      return -1;
    }

  return gadget_drop(cdev, &str);
}

static int fconfig_write_dev_desc(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_info *gi = NULL;
  struct fconfig_dev_desc desc;
  struct usb_device_descriptor *dev_desc = NULL;
  int ret;

  ret = memcpy_s(&desc, sizeof(desc), user_data, sizeof(desc));
  if (ret != EOK)
    {
      return -1;
    }

  gi = gadget_find_by_name(cdev, &desc.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can not find the gadget\n", __FUNCTION__);
       return -1;
    }

  dev_desc = (struct usb_device_descriptor *)usbm_malloc(&gi->obj, sizeof(*dev_desc));
  if (dev_desc == NULL)
    {
      usb_err("%s: malloc device descriptor failed\n", __FUNCTION__);
      goto err;
    }

  ret = memcpy_s(dev_desc, sizeof(*dev_desc), &desc.dev_desc, sizeof(*dev_desc));
  if (ret != EOK)
    {
      goto err;
    }

  gi->dev_desc = dev_desc;
  return 0;

err:
  usbm_free(&gi->obj, dev_desc);
  return -1;
}

static struct gadget_config *fconfig_alloc_gadget_cfg(struct gadget_info *gi, char *cfg_name,
                                                      struct usb_config_descriptor *cfg_desc)
{
  struct gadget_config *cfg = NULL;

  cfg = (struct gadget_config *)usbm_malloc(&gi->obj, sizeof(*cfg));
  if (cfg == NULL)
    {
      usb_err("%s: malloc usb cfg failed\n", __FUNCTION__);
      return NULL;
    }
  memset_s(cfg, sizeof(*cfg), 0, sizeof(*cfg));
  usbobj_init(&cfg->obj, cfg_name, usbobj_default_release);

  cfg->gi       = gi;
  cfg->name     = cfg_name;
  cfg->cfg_desc = cfg_desc;
  cfg->cfg_num  = cfg_desc->bConfigurationValue;

  return cfg;
}

static int fconfig_add_gadget_cfg(struct fconfig_softc *cdev, void *user_data)
{
  struct usb_config_descriptor *cfg_desc = NULL;
  struct fconfig_cfg_desc desc;
  struct gadget_config *cfg = NULL;
  struct gadget_info *gi = NULL;
  struct usb_obj *obj = NULL;
  char *cfg_name = NULL;
  int ret;

  ret = memcpy_s(&desc, sizeof(desc), user_data, sizeof(desc));
  if (ret != EOK)
    {
      return -1;
    }

  gi = gadget_find_by_name(cdev, &desc.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      goto err;
    }

  cfg_name = fconfig_get_string(&gi->obj, &desc.config_name);
  if (cfg_name == NULL)
    {
      usb_err("%s: get config name failed\n", __FUNCTION__);
      goto err;
    }

  obj = usbobj_find(&gi->obj, usbobj_default_match, cfg_name);
   if (obj)
    {
      usb_err("%s: the config %s has been existed\n",
              __FUNCTION__, cfg_name);
      goto err;
    }

  cfg_desc = (struct usb_config_descriptor *)usbm_malloc(&gi->obj, sizeof(*cfg_desc));
  if (cfg_desc == NULL)
    {
      usb_err("%s: malloc config descriptor failed\n", __FUNCTION__);
      goto err;
    }

  ret = memcpy_s(cfg_desc, sizeof(*cfg_desc), &desc.cfg_desc, sizeof(*cfg_desc));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      goto err;
    }
  cfg_desc->bLength = USB_CONFIG_DESC_SIZE;
  cfg_desc->bDescriptorType = UDESC_CONFIG;
  cfg = fconfig_alloc_gadget_cfg(gi, cfg_name, cfg_desc);
  if (cfg == NULL)
    {
      usb_err("%s: alloc usb cfg failed\n", __FUNCTION__);
      goto err;
    }
  usbobj_add(&cfg->obj, &gi->obj);
  return 0;

err:
  usbm_free(&gi->obj, cfg_desc);
  usbm_free(&gi->obj, cfg_name);
  return -1;
}

static int fconfig_remove_gadget_cfg(struct fconfig_softc *cdev, void *user_data)
{
  struct fconfig_cfg_desc desc;
  struct gadget_info *gi = NULL;
  struct gadget_config *cfg = NULL;
  struct usb_obj *obj = NULL;
  char *config_name = NULL;
  int ret;

  ret = memcpy_s(&desc, sizeof(desc), (void *)user_data, sizeof(desc));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &desc.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      goto err;
    }

  config_name = fconfig_get_string(&gi->obj, &desc.config_name);
  if (config_name == NULL)
    {
      usb_err("%s: get config name failed\n", __FUNCTION__);
      goto err;
    }

  obj = usbobj_find(&gi->obj, usbobj_default_match, config_name);
  if (!obj)
    {
      usb_err("%s: can't find the config %s\n", __FUNCTION__, config_name);
      goto err;
    }
  cfg = container_of(obj, struct gadget_config, obj);
  usbobj_put(&cfg->obj);

  usbm_free(&gi->obj, cfg->name);
  usbm_free(&gi->obj, cfg->cfg_desc);
  return 0;

err:
  usbm_free(&gi->obj, config_name);
  return -1;
}

static int fconfig_fill_strings(struct gadget_info *gi,
                                struct fconfig_dev_strings *str_data)
{
  struct fconfig_usb_string *strings = NULL;
  struct gadget_strings *dev_strings = NULL;
  uint32_t size;
  int i;
  int ret;

  size = sizeof(*dev_strings);
  dev_strings = (struct gadget_strings *)usbm_malloc(&gi->obj, size);
  if (dev_strings == NULL)
    {
      usb_err("%s: malloc strings failed\n", __FUNCTION__);
      return -1;
    }
  memset_s(dev_strings, size, 0, size);
  size = sizeof(struct usbd_string) * (str_data->str_count + 1);
  dev_strings->strings = usbm_malloc(&gi->obj, size);
  if (dev_strings == NULL)
    {
      usb_err("%s: malloc strings failed\n", __FUNCTION__);
      return -1;
    }

  usbobj_init(&dev_strings->obj, "dev_strings", usbobj_default_release);
  size = sizeof(*strings) * str_data->str_count;
  strings = (struct fconfig_usb_string *)usbm_malloc(&dev_strings->obj, size);
  if (strings == NULL)
    {
      usb_err("%s: malloc config usb strings failed\n", __FUNCTION__);
      goto err;
    }
  ret = memcpy_s(strings, size, (void *)str_data->strings, size);
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      goto err;
    }
  for (i = 0; i < str_data->str_count; i++)
    {
      char *str = fconfig_get_string(&dev_strings->obj, &strings[i].str);
      if (str == NULL)
        {
          usb_err("%s: get string failed\n", __FUNCTION__);
          goto err;
        }
      dev_strings->strings[i].id = strings[i].id + 1;
      dev_strings->strings[i].s  = str;
    }
  dev_strings->strings[i].id = 0;
  dev_strings->strings[i].s  = NULL;
  dev_strings->language = str_data->language;

  usbm_free(&dev_strings->obj, strings);
  usbobj_add(&dev_strings->obj, &gi->strings);
  return 0;

err:
  usbobj_put(&dev_strings->obj);
  return -1;
}

static int fconfig_write_strings(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_info *gi = NULL;
  struct fconfig_dev_strings str_data;
  int ret;

  ret = memcpy_s(&str_data, sizeof(str_data), (void *)user_data, sizeof(str_data));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &str_data.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      return -1;
    }

  /* set the string count for first time */
  if (!gi->str_count)
    {
      gi->str_count = str_data.str_count + 1;
    }
  else if (gi->str_count != (str_data.str_count + 1))
    {
      usb_err("%s: str_count %u is not equeal to %u\n",
              __FUNCTION__, str_data.str_count, gi->str_count);
      return -1;
    }
  return fconfig_fill_strings(gi, &str_data);
}

void function_instance_release(struct usb_obj *obj)
{
  int ret;
  struct function_instance *func_inst = container_of(obj, struct function_instance, obj);

  if (func_inst->minor >= DEV_GENERIC)
    {
      ret = usbdev_generic_free_instance(func_inst->name);
      if (ret != 0)
        {
          usb_err("%s: free generic_func_inst %s failed\n", __func__, func_inst->name);
        }
    }
  usbm_free(obj->parent, func_inst);
}

static int fconfig_do_make_function(struct gadget_config *cfg, char *func_name)
{
  struct function_instance *func_inst = NULL;
  struct usbdev_info *dev_info = NULL;
  char *instance_name = NULL;
  char *obj_name = NULL;

  /* split the whole name into function name and instance name */
  instance_name = strchr(func_name, '.');
  if (instance_name == NULL)
    {
      usb_err("%s: unable to locate . in %s\n", __FUNCTION__, func_name);
      return -1;
    }
  *instance_name = '\0';
  instance_name++;

  func_inst = usbm_malloc(&cfg->obj, sizeof(*func_inst));
  if (func_inst == NULL)
    {
      usb_err("%s: malloc function instance failed\n", __FUNCTION__);
      return -1;
    }
  memset_s(func_inst, sizeof(*func_inst), 0, sizeof(*func_inst));
  usbobj_init(&func_inst->obj, "", function_instance_release);

  dev_info = fconfig_find_usbdev_info(func_name);
  if (dev_info == NULL)
    {
      usb_err("%s: can not find dev_info\n", __FUNCTION__);
      goto err;
    }
  obj_name = usbm_strdup(&func_inst->obj, instance_name);
  func_inst->dev_info = dev_info;
  func_inst->name     = obj_name;
  func_inst->obj.name     = obj_name;
  usbobj_add(&func_inst->obj, &cfg->obj);
  dprintf("%s, func_inst->obj->name = %s\n", __func__, func_inst->obj.name);
  if (dev_info->type != DEV_GENERIC)
    {
      /*for normal kernel driver */
      func_inst->minor = dev_info->type;
    }
  else
    {
      /* for generic device */
      int ret = usbdev_generic_alloc_instance(func_inst->name);
      if (ret < DEV_GENERIC)
        {
          goto err;
        }
      func_inst->minor = ret;
    }

  return 0;

err:
  usbobj_put(&func_inst->obj);
  return -1;
}

static int fconfig_do_drop_function(struct gadget_config *cfg, char *func_name)
{
  struct usb_obj *obj = NULL;
  char *instance_name = NULL;

  /* split the whole name into function name and instance name */
  instance_name = strchr(func_name, '.');
  if (instance_name == NULL)
    {
      usb_err("unable to locate . in %s\n", func_name);
      return -1;
    }
  *instance_name = '\0';
  instance_name++;

  obj = usbobj_find(&cfg->obj, usbobj_default_match, instance_name);
  if (!obj)
    {
      dprintf("%s, usbobj_find %s obj failed\n", __func__, instance_name);
      return -1;
    }

  usbobj_put(obj);
  return 0;
}

static int fconfig_get_function_info(struct fconfig_softc *cdev, void *user_data,
                                     char **name, struct gadget_config **usb_cfg)
{
  struct fconfig_func_info func_info;
  struct gadget_config *cfg = NULL;
  struct gadget_info *gi = NULL;
  struct usb_obj *obj = NULL;
  char *config_name = NULL;
  char *func_name   = NULL;
  int ret;

  ret = memcpy_s(&func_info, sizeof(func_info), (void *)user_data, sizeof(func_info));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &func_info.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      goto err;
    }

  config_name = fconfig_get_string(&gi->obj, &func_info.config_name);
  if (config_name == NULL)
    {
      usb_err("%s: get config name failed\n", __FUNCTION__);
      goto err;
    }

  /* find config from gadget object by the name */
  obj = usbobj_find(&gi->obj, usbobj_default_match, config_name);
  if (!obj)
    {
      usb_err("%s: can't find the config %s\n", __FUNCTION__, config_name);
      goto err;
    }
  cfg = container_of(obj, struct gadget_config, obj);

  func_name = fconfig_get_string(obj, &func_info.func_name);
  if (func_name == NULL)
    {
      usb_err("%s: get func name failed\n", __FUNCTION__);
      goto err2;
    }

  *name    = func_name;
  *usb_cfg = cfg;
  usbm_free(&gi->obj, config_name);
  return 0;

err2:
  usbm_free(&cfg->obj, func_name);
err:
  usbm_free(&gi->obj, config_name);
  return -1;
}

static int fconfig_make_function(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_config *cfg = NULL;
  char *func_name = NULL;
  int ret;

  ret = fconfig_get_function_info(cdev, user_data, &func_name, &cfg);
  if (ret != 0)
    {
      usb_err("%s: get function info failed\n", __FUNCTION__);
      return -1;
    }

  ret = fconfig_do_make_function(cfg, func_name);
  if (ret != 0)
    {
      usb_err("%s: make function failed\n", __FUNCTION__);
    }
  usbm_free(&cfg->obj, func_name);

  return ret;
}

static int fconfig_drop_function(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_config *cfg = NULL;
  char *func_name = NULL;
  int ret;

  ret = fconfig_get_function_info(cdev, user_data, &func_name, &cfg);
  if (ret != 0)
    {
      usb_err("%s: get function info failed\n", __FUNCTION__);
      return -1;
    }

  ret = fconfig_do_drop_function(cfg, func_name);
  if (ret != 0)
    {
      usb_err("%s: make function failed\n", __FUNCTION__);
    }

  usbm_free(&cfg->obj, func_name);
  return ret;
}

static int usb_config_match(struct usb_obj *obj, void *match_data)
{
  struct gadget_config *cfg = container_of(obj, struct gadget_config, obj);
  uint8_t config_num = *(uint8_t *)match_data;

  return (cfg->cfg_num == config_num);
}

static void func_inst_init(struct usb_obj *obj, void *data)
{
  struct function_instance *func = container_of(obj, struct function_instance, obj);
  struct func_info * info = (struct func_info *)data;
  struct usbdev_info *dev_info = NULL;

  if (func == NULL || func->dev_info == NULL)
    {
      usb_err("%s: get function_instance failed\n", __FUNCTION__);
      return;
    }
  dev_info = func->dev_info;

  if (dev_info->type == DEV_GENERIC)
    {
      dev_info->initialize_sub(&info->devs[info->ndevices], info->ifnobase, func->minor);
      info->devs[info->ndevices].devinfo.strbase = info->strbase;
      info->ifnobase += info->devs[info->ndevices].devinfo.ninterfaces;
      info->strbase += info->devs[info->ndevices].devinfo.nstrings;
    }
  else
    {
      dev_info->initialize_sub(&info->devs[info->ndevices], info->ifnobase, dev_info->type);
      info->ifnobase += dev_info->ifnum;
    }
  info->ndevices++;
}

static int fconfig_composite_initialize(struct gadget_info *gi,
                                        struct composite_softc *com_s)
{
  struct composite_devdesc_s devs[NUM_DEVICES_TO_HANDLE];
  struct func_info info;
  uint8_t config_num = 1;
  struct usb_obj *obj = NULL;
  struct gadget_config *cfg = NULL;

  if (com_s)
    {
      config_num = com_s->dev.config;
    }
  info.devs = devs;
  info.ndevices = 0;
  info.ifnobase = 0;
  info.strbase = 0;
  config_num = 1;
  obj = usbobj_find(&gi->obj, usb_config_match, &config_num);
  if (!obj)
    {
      usb_err("%s: can't find the configNumber %d\n", __FUNCTION__, config_num);
      return -1;
    }
  cfg = container_of(obj, struct gadget_config, obj);
  usbobj_for_each_child(&cfg->obj, NULL, NULL, func_inst_init, &info);

  return fconfig_do_composite_initialize(com_s, info.ndevices, devs);
}

static int fconfig_enable_udc(struct fconfig_softc *cdev, void *user_data)
{
  struct composite_softc *com_s = cdev->com_s;
  struct fconfig_udc_info udc_info;
  struct gadget_info *gi = NULL;
  char *udc_name = NULL;
  int ret;

  ret = memcpy_s(&udc_info, sizeof(udc_info), (void *)user_data, sizeof(udc_info));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &udc_info.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      goto err;
    }

  udc_name = fconfig_get_string(&gi->obj, &udc_info.udc_name);
  if (udc_name == NULL)
    {
      usb_err("%s: get udc name failed\n", __FUNCTION__);
      goto err;
    }

  gi->udc_name = udc_name;
  /* TODO: Enable UDC */
  ret = fconfig_composite_initialize(gi, com_s);
  if (ret)
    {
      usb_err("%s: fcofnig composite init failed\n", __FUNCTION__);
      goto err;
    }
  usbd_start_udc();
  ret = usbd_gadget_attach_driver(com_s->parnet_conext, &com_s->drvr.drvr);
  if (ret < 0)
    {
      usb_err("%s, usbd_gadget_attach_driver fail\n", __func__);
      goto err;
    }

  return 0;

err:
  usbm_free(&gi->obj, udc_name);
  return -1;
}

static int fconfig_disable_udc(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_info *gi = NULL;
  struct fconfig_udc_info udc_info;
  int ret;

  ret = memcpy_s(&udc_info, sizeof(udc_info), (void *)user_data, sizeof(udc_info));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &udc_info.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      goto err;
    }

  /* disable UDC */
  usbd_stop_udc();
  usbm_free(&gi->obj, gi->udc_name);
  gi->udc_name = NULL;
  return 0;

err:
  return -1;
}

static int change_dev_descritor_info(struct gadget_info *gi,
                                     struct fconfig_dev_desc_info *desc_info)
{
  const char *name = NULL;

  if (desc_info == NULL || gi == NULL)
    {
      usb_err("%s: param invail\n", __FUNCTION__);
      return -1;
    }
  name = desc_info->prop.prop_name;
  if (strcmp(USBDEV_VENDOR, name) == 0)
    {
      gi->dev_desc->idVendor[0] = (uint8_t)desc_info->prop.prop_value;
      gi->dev_desc->idVendor[1] = (uint8_t)(desc_info->prop.prop_value >> 8);
    }
  else if (strcmp(USBDEV_IDPRODUCT, name) == 0)
    {
      gi->dev_desc->idProduct[0] = (uint8_t)desc_info->prop.prop_value;
      gi->dev_desc->idProduct[1] = (uint8_t)(desc_info->prop.prop_value >> 8);
    }
  else if (strcmp(USBDEV_CLASS, name) == 0)
    {
      gi->dev_desc->bDeviceClass = (uint8_t)desc_info->prop.prop_value;
    }
  else if (strcmp(USBDEV_SUBCLASS, name) == 0)
    {
      gi->dev_desc->bDeviceSubClass = (uint8_t)desc_info->prop.prop_value;
    }
  else if (strcmp(USBDEV_PROTOCOL, name) == 0)
    {
      gi->dev_desc->bDeviceProtocol = (uint8_t)desc_info->prop.prop_value;
    }
  else if (strcmp(USBDEV_MAXSIZE, name) == 0)
    {
      gi->dev_desc->bMaxPacketSize = (uint8_t)desc_info->prop.prop_value;
    }
  else if (strcmp(USBDEV_NUMCFG, name) == 0)
    {
      gi->dev_desc->bNumConfigurations = (uint8_t)desc_info->prop.prop_value;
    }
  else if (strcmp(USBDEV_BCD, name) == 0)
    {
      gi->dev_desc->bcdUSB[0] = (uint8_t)desc_info->prop.prop_value;
      gi->dev_desc->bcdUSB[1] = (uint8_t)(desc_info->prop.prop_value >> 8);
    }
  else if (strcmp(USBDEV_BCDDEVICE, name) == 0)
    {
      gi->dev_desc->bcdDevice[0] = (uint8_t)desc_info->prop.prop_value;
      gi->dev_desc->bcdDevice[1] = (uint8_t)(desc_info->prop.prop_value >> 8);
    }
  else
    {
      usb_err("%{public}s: other prop", __func__);
      return -1;
    }
  return 0;
}

static int fconfig_change_dev_desc_info(struct fconfig_softc *cdev, void *user_data)
{
  struct gadget_info *gi = NULL;
  struct fconfig_dev_desc_info desc_info;
  int ret;

  ret = memcpy_s(&desc_info, sizeof(desc_info), (void *)user_data, sizeof(desc_info));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &desc_info.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      return -1;
    }
  if (gi->dev_desc == NULL)
    {
      usb_err("%s: dev_desc is null\n", __FUNCTION__);
      return -1;
    }
  ret = change_dev_descritor_info(gi, &desc_info);
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }
  return 0;
}

static int change_dev_descritor_string(struct gadget_info *gi,
                                       struct fconfig_dev_desc_string *desc_string,
                                       struct gadget_strings *dev_strings)
{
  uint8_t id = 0xff;
  int i;
  const char *name = NULL;

  if (desc_string == NULL || gi == NULL || dev_strings == NULL)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }
  for (i = 0; dev_strings->strings[i].s != NULL; i++)
    {
      const char *str = dev_strings->strings[i].s;
      if (strcmp(str, desc_string->prop.prop_value) == 0)
        {
          id = dev_strings->strings[i].id;
          break;
        }
    }
  if (id == 0xff)
    {
      usb_err("%s: not found ringht string ID\n", __FUNCTION__);
      return -1;
    }
  name = desc_string->prop.prop_name;
  if (strcmp(USBDEV_MANUFACTURER, name) == 0)
    {
      gi->dev_desc->iManufacturer = id;
    }
  else if (strcmp(USBDEV_PRODUCT, name) == 0)
    {
      gi->dev_desc->iProduct = id;
    }
  else if (strcmp(USBDEV_SERIALNUM, name) == 0)
    {
      gi->dev_desc->iSerialNumber = id;
    }
  else
    {
      usb_err("%{public}s: other prop", __func__);
      return -1;
    }
  return 0;
}

static int fconfig_change_dev_desc_string(struct fconfig_softc *cdev, void *user_data)
{
  struct usb_obj *obj = NULL;
  struct gadget_info *gi = NULL;
  struct fconfig_dev_desc_string desc_string;
  struct gadget_strings *dev_strings = NULL;
  int ret;

  ret = memcpy_s(&desc_string, sizeof(desc_string), (void *)user_data, sizeof(desc_string));
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }

  gi = gadget_find_by_name(cdev, &desc_string.gadget_name);
  if (gi == NULL)
    {
      usb_err("%s: can't find the gadget\n", __FUNCTION__);
      return -1;
    }
  if (gi->dev_desc == NULL)
    {
      usb_err("%s: dev_desc is null\n", __FUNCTION__);
      return -1;
    }

  obj = usbobj_find(&gi->strings, gadget_strings_match, &desc_string.prop.lang);
  if (!obj)
    {
      usb_err("%s: can not find language = 0x%x strings\n", __func__, desc_string.prop.lang);
      return -1;
    }
  dev_strings = container_of(obj, struct gadget_strings, obj);

  ret = change_dev_descritor_string(gi, &desc_string, dev_strings);
  if (ret != EOK)
    {
      usb_err("%s: copy config descs from user failed\n", __FUNCTION__);
      return -1;
    }
  return 0;
}

int fconfig_handle_open(struct usb_handle *handle)
{
  struct fconfig_softc *cdev = (struct fconfig_softc *)(handle->priv);


  if (cdev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  usbobj_get(&cdev->obj);
  return 0;
}

int fconfig_handle_close(struct usb_handle *handle)
{
  struct fconfig_softc *cdev = (struct fconfig_softc *)(handle->priv);

  if (cdev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  usbobj_put(&cdev->obj);
  return 0;
}

int fconfig_handle_read(struct usb_handle *handle, void *buffer, size_t len)
{
  struct fconfig_softc *cdev = (struct fconfig_softc *)(handle->priv);

  if (len == 0)
    {
      usb_err("buflen is 0!\n");
      return -1;
    }

  if (cdev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  return 0;
}

int fconfig_handle_write(struct usb_handle *handle, void *buffer, size_t len)
{
  struct fconfig_softc *cdev = (struct fconfig_softc *)(handle->priv);

  if (len == 0)
    {
      usb_err("buflen is 0!\n");
      return -1;
    }

  if (cdev == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -1;
    }

  return 0;
}

int fconfig_handle_ioctl(struct usb_handle *handle, int cmd, void *arg)
{
  struct fconfig_softc *cdev = (struct fconfig_softc *)(handle->priv);
  void *argp = (void *)arg;
  int ret = -1;

  if (cdev == NULL || argp == NULL)
    {
      usb_err("Invalid private parameter!\n");
      return -ENODEV;
    }

  switch (cmd)
    {
    case FCONFIG_CMD_MAKE_GADGET:
	  ret = fconfig_make_gadget(cdev, argp);
	  break;

    case FCONFIG_CMD_DROP_GADGET:
      ret = fconfig_drop_gadget(cdev, argp);
      break;

    case FCONFIG_CMD_WRITE_DEV_DESC:
      ret = fconfig_write_dev_desc(cdev, argp);
      break;

    case FCONFIG_CMD_ADD_CONFIG:
      ret = fconfig_add_gadget_cfg(cdev, argp);
      break;

    case FCONFIG_CMD_REMOVE_CONFIG:
      ret = fconfig_remove_gadget_cfg(cdev, argp);
      break;

    case FCONFIG_CMD_WRITE_STRINGS:
      ret = fconfig_write_strings(cdev, argp);
      break;

    case FCONFIG_CMD_MAKE_FUNCTION:
      ret = fconfig_make_function(cdev, argp);
      break;

    case FCONFIG_CMD_DROP_FUNCTION:
      ret = fconfig_drop_function(cdev, argp);
      break;

    case FCONFIG_CMD_ENABLE_UDC:
      ret = fconfig_enable_udc(cdev, argp);
      break;

    case FCONFIG_CMD_DISABLE_UDC:
      ret = fconfig_disable_udc(cdev, argp);
      break;
    case FCONFIG_CMD_CHAGE_DEVINFO:
      ret = fconfig_change_dev_desc_info(cdev, argp);
      break;
    case FCONFIG_CMD_CHAGE_DEVSTRING:
      ret = fconfig_change_dev_desc_string(cdev, argp);
      break;
    default:
      usb_err("the ioctl %d is not supported\n", cmd);
      break;
    }

  return ret;
}

static struct UsbHandleOps g_fconfig_handle_fops =
{
  .open   = fconfig_handle_open,
  .close  = fconfig_handle_close,
  .read   = fconfig_handle_read,
  .write  = fconfig_handle_write,
  .ioctl  = fconfig_handle_ioctl,
};

int fconfig_fops_init(struct fconfig_softc *cdev)
{
  int ret;
  char *devName = NULL;

  if (cdev == NULL)
    {
      usb_err("%s: cdev is NULL\n", __FUNCTION__);
      return -1;
    }

  devName = usbm_malloc(&cdev->obj, strlen(USB_FCONFIG_DEV) + 1);
  if (!devName)
    {
      return -1;
    }
  ret = snprintf_s(devName, strlen(USB_FCONFIG_DEV) + 1, strlen(USB_FCONFIG_DEV),
                   "%s", USB_FCONFIG_DEV);
    if (ret < 0)
      {
        goto free_devname;
      }
  ret = register_handle(devName, &g_fconfig_handle_fops, cdev, &cdev->obj);
  if (ret != OK)
    {
      usb_err("%s: fconfig device register failed!\n", __FUNCTION__);
      goto free_devname;
    }

  return 0;

free_devname:
  usbm_free(&cdev->obj, devName);
  return ret;
}

int fconfig_fops_deinit(const struct fconfig_softc *cdev)
{
  int ret;

  if (cdev == NULL)
    {
      usb_err("%s: cdev is NULL\n", __FUNCTION__);
      return -1;
    }
  ret = unregister_handle(USB_FCONFIG_DEV);
  if (ret != OK)
    {
      usb_err("%s: fconfig device unregister failed!\n", __FUNCTION__);
    }

  return ret;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
