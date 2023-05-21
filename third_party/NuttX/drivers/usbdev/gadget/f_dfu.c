/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver DFU Protocol
 * Author: huangjieliang
 * Create: 2017-12-25
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

#include <hisoc/uart.h>
#include "gadget/f_dfu.h"
#include "gadget/composite.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int usbdev_dfu_initialize(struct module *mod, int n, void *arg);

/* device driver structure definition */

static const driver_t g_dfu_driver =
{
  .name     = "fdfu",
  .methods  = NULL,
  .size     = 0
};

/* private device class information */

static devclass_t g_dfu_devclass;
DRIVER_MODULE(fdfu, simple, g_dfu_driver, g_dfu_devclass, usbdev_dfu_initialize, 0);

static struct usb_dfu_dev *g_dfu_dev       = NULL;
static struct usb_dfu_entity *g_dfu_entity = NULL;
static struct pthread_mutex g_dfu_mtx      = PTHREAD_MUTEX_INITIALIZER;

static struct string_des g_manufacturer_str;
static struct string_des g_serial_number_str;
static struct string_des g_product_str;

volatile uint8_t g_flash_upgrade_state = DFU_FLASH_STATE_UPGRADING;

/* define descriptor */

#define DEVICE_VENDOR_ID        0x0955
#define DEVICE_PRODUCT_ID       0x701A
#define DEVICE_VERSION          0x0110

#define DFU_STRING_LEN_BYTE     0
#define DFU_STRING_DATA_BYTE    2
#define DFU_STRING_HEAD_LEN     2
#define DFU_MANU_STRING_LEN     28
#define DFU_PRODUCT_STRING_LEN  48
#define DFU_SERIAL_STRING_LEN   22
#define DFU_STRING_ID_LEN       4

static const struct usb_device_descriptor g_dfu_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,
  HSETW(.bcdUSB, 0x0200),
  .bDeviceClass       = 0x02,
  .bDeviceSubClass    = 0x02, /* CDC-modem */
  .bDeviceProtocol    = 0x01, /* Interface Association Descriptor */
  .bMaxPacketSize     = 64,   /* Control Endpoint packet size */
  HSETW(.idVendor, DEVICE_VENDOR_ID),
  HSETW(.idProduct, DEVICE_PRODUCT_ID),
  HSETW(.bcdDevice, DEVICE_VERSION),
  .iManufacturer      = 1,    /* Manufacturer name, string index */
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1,
};

struct usb_config_descriptor g_dfu_config_desc =
{
  .bLength             = USB_CONFIG_DESC_SIZE,
  .bDescriptorType     = UDESC_CONFIG,

  /* HSETW(.wTotalLength, USB_CONFIG_DESC_SIZE + sizeof(usb_dfu_func) + sizeof(usb_dfu_intf_desc)), */

  HSETW(.wTotalLength, 0),  /* Modify according to the actual length */
  .bNumInterface       = 1,
  .bConfigurationValue = 1,
  .iConfiguration      = 0,
  .bmAttributes        = UC_SELF_POWERED | UC_BUS_POWERED,   /* 0xc0 */
  .bMaxPower           = 1                                   /* max power */
};

/* HUAWEI LiteOS */

static char g_dfu_manufacturer[DFU_MANU_STRING_LEN] =
{
  DFU_MANU_STRING_LEN,
  UDESC_STRING,
  'H', 0, 'U', 0, 'A', 0, 'W', 0, 'E', 0, 'I', 0, ' ', 0,
  'L', 0, 'i', 0, 't', 0, 'e', 0, 'O', 0, 'S', 0
};

/* USB download gadget */

static char g_dfu_product[DFU_PRODUCT_STRING_LEN] =
{
  DFU_PRODUCT_STRING_LEN,
  UDESC_STRING,
  'U', 0, 'S', 0, 'B', 0, ' ', 0, 'd', 0, 'o', 0, 'w', 0, 'n', 0, 'l', 0, 'o', 0,
  'w', 0, 'n', 0, 'l', 0, 'o', 0, 'a', 0, 'd', 0, ' ', 0, 'g', 0, 'a', 0, 'd', 0,
  'g', 0, 'e', 0, 't', 0
};

static char g_dfu_serial[DFU_SERIAL_STRING_LEN] =
{
  DFU_SERIAL_STRING_LEN,
  UDESC_STRING,
  '1', 0, '2', 0, '3', 0, '4', 0, '5', 0,
  '6', 0, '7', 0, '8', 0, '9', 0, '0', 0
};

static const char g_dfu_string_id[DFU_STRING_ID_LEN] =
{
  DFU_STRING_ID_LEN,
  UDESC_STRING,
  0x09, 0x04
};

static struct usbd_string g_dfu_string_defs[] =
{
  { 0, g_dfu_string_id },
  { 1, g_dfu_manufacturer },
  { 2, g_dfu_product },
  { 3, g_dfu_serial },
  { 4, g_dfu_product },
  USBD_DEVICE_STRINGS_END /* end of list */
};

void set_manufacturer_string(struct string_des *des)
{
  char *buf;
  struct string_des *str_des;
  int len;

  if (des == NULL)
    {
      usb_err("%s failed\n", __FUNCTION__);
      return;
    }

  if (des->str == NULL || des->len <= 0)
    {
      usb_err("%s failed, des->str is NULL or des->len is 0\n", __FUNCTION__);
      return;
    }

  str_des = des;
  len     = str_des->len + DFU_STRING_HEAD_LEN;

  buf = (char *)malloc(len);
  if (buf == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return;
    }

  g_manufacturer_str.str = buf;
  g_manufacturer_str.len = len;

  *buf = len;
  *(buf + 1) = UDESC_STRING;
  (void)memcpy_s(buf + DFU_STRING_HEAD_LEN, (size_t)str_des->len, str_des->str, (size_t)str_des->len);
}

void set_serial_number_string(struct string_des *des)
{
  char *buf;
  struct string_des *str_des;
  int len;

  if (des == NULL)
    {
      usb_err("%s failed\n", __FUNCTION__);
      return;
    }

  if (des->str == NULL || des->len <= 0)
    {
      usb_err("%s failed, des->str is NULL or des->len is 0\n", __FUNCTION__);
      return;
    }

  str_des = des;
  len     = str_des->len + DFU_STRING_HEAD_LEN;

  buf = (char *)malloc(len);
  if (buf == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return;
    }

  g_serial_number_str.str = buf;
  g_serial_number_str.len = len;

  *buf = len;
  *(buf + 1) = UDESC_STRING;
  (void)memcpy_s(buf + DFU_STRING_HEAD_LEN, (size_t)str_des->len, str_des->str, (size_t)str_des->len);
}

void set_product_string(struct string_des *des)
{
  char *buf;
  struct string_des *str_des;
  int len;

  if (des == NULL)
    {
      usb_err("%s failed\n", __FUNCTION__);
      return;
    }

  if (des->str == NULL || des->len <= 0)
    {
      usb_err("%s failed, des->str is NULL or des->len is 0\n", __FUNCTION__);
      return;
    }

  str_des = des;
  len     = str_des->len + DFU_STRING_HEAD_LEN;

  buf = (char *)malloc(len);
  if (buf == NULL)
    {
      usb_err("%s malloc failed\n", __FUNCTION__);
      return;
    }

  g_product_str.str = buf;
  g_product_str.len = len;

  *buf = len;
  *(buf + 1) = UDESC_STRING;
  (void)memcpy_s(buf + DFU_STRING_HEAD_LEN, (size_t)str_des->len, str_des->str, (size_t)str_des->len);
}

void set_flash_state(volatile uint8_t flash_state)
{
  if (flash_state != DFU_FLASH_STATE_UPGRADING &&
      flash_state != DFU_FLASH_STATE_UPGRADED &&
      flash_state != DFU_FLASH_STATE_ERROR)
    {
      PRINT_ERR("The state of flash upgrade is set failed!\n");
      return;
    }

  g_flash_upgrade_state = flash_state;
}

void get_flash_state(struct usbdev_req_s *req)
{
  uint8_t *buf = req->buf;

  buf[0] = g_flash_upgrade_state; /* The 0 bit is valid */
}

bool usb_dfu_running(void)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);

  if (entity == NULL)
    {
      return false;
    }

  return (entity->trans_complete || entity->trans_size) ? true: false;
}

uint32_t usb_dfu_update_status(void)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);

  if (entity == NULL)
    {
      return 0;
    }

  return entity->trans_complete;
}

uint64_t *usb_dfu_update_size_get(void)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);

  if (entity == NULL)
    {
      return NULL;
    }

  return &entity->trans_size;
}

static int dfu_ram_read(uint8_t *buf, uint32_t len)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);
  uint32_t offset;
  errno_t ret;

  if (buf == NULL || entity == NULL)
    {
      return -1;
    }

  offset = entity->offset;
  if (offset > entity->ram_size)
    {
      return -1;
    }

  len = min(len, (entity->ram_size - offset));

  ret = memcpy_s(buf, len, (const void *)((uint8_t *)entity->ram_addr + offset), len);
  if (ret != EOK)
    {
      usb_err("memcpy failed!\n");
      return -1;
    }
  entity->offset += len;

  (void)uart_putc('#');

  return (int)len;
}

static int dfu_ram_write(const uint8_t *buf, uint32_t len)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);
  uint32_t offset;
  errno_t ret;

  if (buf == NULL || entity == NULL)
    {
      return -1;
    }

  offset = entity->offset;
  if (offset > entity->ram_size)
    {
      return -1;
    }

  len = min(len, (entity->ram_size - offset));

  ret = memcpy_s((void *)((uint8_t *)entity->ram_addr + offset), entity->ram_size - offset, buf, len);
  if (ret != EOK)
    {
      usb_err("memcpy failed!\n");
      return -1;
    }
  entity->offset     += len;
  entity->trans_size += len;

  (void)uart_putc('#');

  return 0;
}

void usbdev_dfu_dev_init(void)
{
  struct usb_dfu_dev *dev;
  struct string_des str;

  dev = malloc(sizeof(struct usb_dfu_dev));
  if (dev == NULL)
    {
      return;
    }
  (void)memset_s(dev, sizeof(struct usb_dfu_dev), 0, sizeof(struct usb_dfu_dev));

  g_dfu_dev = dev;

  str.str = &g_dfu_manufacturer[DFU_STRING_DATA_BYTE];
  str.len = g_dfu_manufacturer[DFU_STRING_LEN_BYTE] - DFU_STRING_HEAD_LEN;
  set_manufacturer_string(&str);
  str.str = &g_dfu_serial[DFU_STRING_DATA_BYTE];
  str.len = g_dfu_serial[DFU_STRING_LEN_BYTE] - DFU_STRING_HEAD_LEN;
  set_serial_number_string(&str);
  str.str = &g_dfu_product[DFU_STRING_DATA_BYTE];
  str.len = g_dfu_product[DFU_STRING_LEN_BYTE] - DFU_STRING_HEAD_LEN;
  set_product_string(&str);
}

void usbdev_dfu_dev_deinit(void)
{
  if (g_dfu_dev != NULL)
    {
      free(g_dfu_dev);
      g_dfu_dev = NULL;
    }

  if (g_manufacturer_str.str != NULL)
    {
      free((void *)g_manufacturer_str.str);
      g_manufacturer_str.str = NULL;
    }

  if (g_product_str.str != NULL)
    {
      free((void *)g_product_str.str);
      g_product_str.str = NULL;
    }

  if (g_serial_number_str.str != NULL)
    {
      free((void *)g_serial_number_str.str);
      g_serial_number_str.str = NULL;
    }
}

static struct usb_dfu_dev *usb_dfu_get_dev(void)
{
  return g_dfu_dev;
}

static inline void dfu_set_state(int new_state)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  dev->dfu_state = new_state;
}

static inline int dfu_get_state(void)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  return dev->dfu_state;
}

static void dfu_dnload_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  int ret;

  (void)ep;

  ret = usb_dfu_write(usb_dfu_get_entity(0), req->buf, (int)req->len, dev->seq_num);
  if (ret < 0)
    {
      dev->dfu_status.status = USB_DFU_ERR_UNKNOWN;
      dev->dfu_state = USB_DFU_ERROR;
    }
}

void usbdev_dfu_set_config(void)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();

  if (dev->dfu_state == USB_DFU_APP_DETACH)
    {
      dev->dfu_state = USB_DFU_IDLE;
    }

  dev->dfu_status.status = USB_DFU_STATUS_OK;
}

void usbdev_dfu_transaction_cleanup(void)
{
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);

  if (entity == NULL)
    {
      return;
    }

  entity->trans_size     = 0;
  entity->trans_complete = 0;
  entity->seq_num        = 0;
  entity->offset         = 0;
}

void usbdev_dfu_init(void)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();

  if (dev == NULL)
    {
      return;
    }

  /* When the host confirm that the dfu "bInterfaceProtocol" is the "Runtime mode"
   * it will send a "DFU_DETACH" request to the device. When unload device, the device
   * mode should change from "DFU mode" to "Runtime mode", so that next upgrade action
   * can be performed smoothly.
   *
   * bInterfaceProtocol value: 0x1 --- Runtime protocol.
   * bInterfaceProtocol value: 0x2 --- DFU mode protocol.
   *
   * More information, please refer to DFU_1.1.pdf
   */

  to_runtime_mode();
  dev->dfu_state = USB_DFU_APP_IDLE;
  (void)memset_s(&dev->dfu_status, sizeof(struct dfu_getstatus_response_s),
                 0, sizeof(struct dfu_getstatus_response_s));
  dev->dfu_status.poll_timeout[0] = USB_DFU_POLLTIMEOUT;

  usbdev_dfu_transaction_cleanup();
}

static int dfu_get_status_for_req(struct usbdev_req_s *ctrlreq)
{
  struct dfu_getstatus_response_s *status = (struct dfu_getstatus_response_s *)ctrlreq->buf;
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  int dfu_state = dev->dfu_state;

  if (dfu_state == USB_DFU_DNLOAD_SYNC)
    {
      dev->dfu_state = USB_DFU_DNLOAD_IDLE;
    }
  else if (dfu_state == USB_DFU_DNBUSY)
    {
      dev->dfu_state = USB_DFU_DNLOAD_SYNC;
    }
  else if (dfu_state == USB_DFU_MANIFEST_SYNC)
    {
      dev->dfu_state = USB_DFU_MANIFEST;
    }
  else if (dfu_state == USB_DFU_MANIFEST)
    {
      dev->dfu_state = USB_DFU_IDLE;
    }

  *status = dev->dfu_status;
  status->state = (uint8_t)dev->dfu_state;

  return USB_DFU_RET_STATUS_LEN;
}

static int dfu_get_state_for_req(struct usbdev_req_s *ctrlreq)
{
  uint8_t *state = ctrlreq->buf;

  *state = (uint8_t)dfu_get_state();

  return USB_DFU_RET_STATE_LEN;
}

static int dfu_dnload(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  int len                 = UGETW(ctrl->wLength);
  uint16_t value          = UGETW(ctrl->wValue);

  dev->dfu_state = USB_DFU_DNLOAD_SYNC;
  dev->seq_num   = value;

  if (len == 0)
    {
      if (dev->dfu_state == USB_DFU_IDLE)
        {
          dev->dfu_state = USB_DFU_ERROR;
          return USB_DFU_RET_ERR;
        }
      else
        {
          dev->dfu_state = USB_DFU_MANIFEST_SYNC;
        }
    }

  ctrlreq->callback = dfu_dnload_complete;

  return len;
}

static int dfu_upload(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  uint16_t value          = UGETW(ctrl->wValue);

  if (dev->dfu_state == USB_DFU_IDLE)
    {
      dev->dfu_state = USB_DFU_UPLOAD_IDLE;
      dev->seq_num   = 0;
      usbdev_dfu_transaction_cleanup();
    }
  else if (dev->dfu_state == USB_DFU_UPLOAD_IDLE)
    {
      dev->seq_num = value;
    }

  return usb_dfu_read(usb_dfu_get_entity(0), ctrlreq->buf, (int)ctrlreq->len, dev->seq_num);
}

static int dfu_app_idle_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret = USB_DFU_RET_OK;

  switch (ctrl->bRequest)
    {
    case USB_DFU_DETACH:
      {
        to_dfu_mode();
        dfu_set_state(USB_DFU_APP_DETACH);
      }
      break;

    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_APP_IDLE);
      }
      break;
    }

  return ret;
}

static int dfu_app_detach_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_APP_IDLE);
      }
      break;
    }

  return ret;
}

static int dfu_idle_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret = USB_DFU_RET_OK;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    case USB_DFU_DNLOAD:
      ret = dfu_dnload(ctrl, ctrlreq);
      break;

    case USB_DFU_UPLOAD:
      ret = dfu_upload(ctrl, ctrlreq);
      break;

    case USB_DFU_ABORT:
      dfu_set_state(USB_DFU_IDLE);
      break;

    case USB_DFU_FLASH_STATE:
      {
        get_flash_state(ctrlreq);
        ret = 1;
      }
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_dnload_sync_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_dnbusy_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_dnload_idle_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret = USB_DFU_RET_OK;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    case USB_DFU_DNLOAD:
      ret = dfu_dnload(ctrl, ctrlreq);
      break;

    case USB_DFU_ABORT:
      dfu_set_state(USB_DFU_IDLE);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_manifest_sync_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);
  int ret;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      {
        dev->seq_num = 0;
        ret          = dfu_get_status_for_req(ctrlreq);
        if (entity != NULL)
          {
            entity->seq_num = 0;
            entity->offset  = 0;
          }
      }
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_manifest_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  struct usb_dfu_entity *entity = usb_dfu_get_entity(0);
  int ret;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      {
        ret = dfu_get_status_for_req(ctrlreq);
        dev->seq_num = 0;
        if (entity != NULL)
          {
            entity->trans_complete = 1;
          }
        dprintf("\nDOWNLOAD ... OK\n");
      }
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_manifest_wait_reset(const struct usb_device_request *ctrl, const struct usbdev_req_s *ctrlreq)
{
  (void)ctrl;
  (void)ctrlreq;
  dprintf("Do nothing!\n");
  return 0;
}

static int dfu_upload_idle_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  int ret = USB_DFU_RET_OK;
  int len = UGETW(ctrl->wLength);

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    case USB_DFU_UPLOAD:
      {
        ret = dfu_upload(ctrl, ctrlreq);
        if (ret >= 0 && ret < len)
          {
            dfu_set_state(USB_DFU_IDLE);
          }
      }
      break;

    case USB_DFU_ABORT:
      dfu_set_state(USB_DFU_IDLE);
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

static int dfu_error_request(const struct usb_device_request *ctrl, struct usbdev_req_s *ctrlreq)
{
  struct usb_dfu_dev *dev = usb_dfu_get_dev();
  int ret                 = USB_DFU_RET_OK;

  switch (ctrl->bRequest)
    {
    case USB_DFU_GET_STATUS:
      ret = dfu_get_status_for_req(ctrlreq);
      break;

    case USB_DFU_GET_STATE:
      ret = dfu_get_state_for_req(ctrlreq);
      break;

    case USB_DFU_CLR_STATUS:
      {
        dev->dfu_status.status = USB_DFU_STATUS_OK;
        dfu_set_state(USB_DFU_IDLE);
      }
      break;

    default:
      {
        ret = USB_DFU_RET_ERR;
        dfu_set_state(USB_DFU_ERROR);
      }
      break;
    }

  return ret;
}

int usbdev_dfu_class_requests(const struct usbdev_s *dev,
                              const struct usb_device_request *ctrl,
                              struct usbdev_req_s *ctrlreq)
{
  int ret = -EOPNOTSUPP;

  (void)dev;

  switch (dfu_get_state())
    {
    case USB_DFU_APP_IDLE:
      ret = dfu_app_idle_request(ctrl, ctrlreq);
      break;

    case USB_DFU_APP_DETACH:
      ret = dfu_app_detach_request(ctrl, ctrlreq);
      break;

    case USB_DFU_IDLE:
      ret = dfu_idle_request(ctrl, ctrlreq);
      break;

    case USB_DFU_DNLOAD_SYNC:
      ret = dfu_dnload_sync_request(ctrl, ctrlreq);
      break;

    case USB_DFU_DNBUSY:
      ret = dfu_dnbusy_request(ctrl, ctrlreq);
      break;

    case USB_DFU_DNLOAD_IDLE:
      ret = dfu_dnload_idle_request(ctrl, ctrlreq);
      break;

    case USB_DFU_MANIFEST_SYNC:
      ret = dfu_manifest_sync_request(ctrl, ctrlreq);
      break;

    case USB_DFU_MANIFEST:
      ret = dfu_manifest_request(ctrl, ctrlreq);
      break;

    case USB_DFU_MANIFEST_WAIT_RESET:
      ret = dfu_manifest_wait_reset(ctrl, ctrlreq);
      break;

    case USB_DFU_UPLOAD_IDLE:
      ret = dfu_upload_idle_request(ctrl, ctrlreq);
      break;

    case USB_DFU_ERROR:
      ret = dfu_error_request(ctrl, ctrlreq);
      break;

    default:
      usb_err("The state = %d of dfu is not support!\n", dfu_get_state());
      break;
    }

  return ret;
}

static int dfu_ram_init_env(struct usb_dfu_entity *entity, char *envstr)
{
  char *st;
  int ret;

  st = strsep(&envstr, " ");
  if (st == NULL)
    {
      return -1;
    }
  ret = strncpy_s(entity->dfu_name, DFU_NAME_MAX_LEN, st, strlen(st) + 1);
  if (ret != EOK)
    {
      return -1;
    }

  st = strsep(&envstr, " ");
  if (strcmp(st, "ram"))
    {
      return -1;
    }

  st = strsep(&envstr, " ");
  if (st == NULL)
    {
      return -1;
    }
  entity->ram_addr = (void *)strtoul(st, &st, 16);

  st = strsep(&envstr, " ");
  if (st == NULL)
    {
      return -1;
    }
  entity->ram_size = strtoul(st, &st, 16);

  entity->dfu_write = dfu_ram_write;
  entity->dfu_read  = dfu_ram_read;

  return 0;
}

int usb_dfu_init_env_entities(char *type, char *envstr, char *devstr)
{
  struct usb_dfu_entity *entity;
  int ret;

  (void)devstr;

  if (type == NULL || envstr == NULL)
    {
      return -1;
    }

  mtx_lock(&g_dfu_mtx);
  if (g_dfu_entity != NULL)
    {
      goto err;
    }

  entity = malloc(sizeof(struct usb_dfu_entity));
  if (entity == NULL)
    {
      goto err;
    }
  (void)memset_s(entity, sizeof(struct usb_dfu_entity), 0, sizeof(struct usb_dfu_entity));

  if (!strcmp(type, "ram"))
    {
      ret = dfu_ram_init_env(entity, envstr);
      if (ret < 0)
        {
          free(entity);
          goto err;
        }
    }
  else
    {
      free(entity);
      usb_err("Device %s not (yet) supported!\n", type);
      goto err;
    }

  g_dfu_entity = entity;
  usbdev_dfu_transaction_cleanup();
  mtx_unlock(&g_dfu_mtx);

  return 0;
err:
  mtx_unlock(&g_dfu_mtx);
  return -1;
}

void usb_dfu_free_entities(void)
{
  mtx_lock(&g_dfu_mtx);
  if (g_dfu_entity == NULL)
    {
      mtx_unlock(&g_dfu_mtx);
      return;
    }
  free(g_dfu_entity);
  g_dfu_entity = NULL;
  mtx_unlock(&g_dfu_mtx);
}

struct usb_dfu_entity *usb_dfu_get_entity(int alter)
{
  (void)alter;
  return g_dfu_entity;
}

int usb_dfu_read(struct usb_dfu_entity *dfu, void *buf, int size, uint32_t blk_seq_num)
{
  int ret;

  if (dfu == NULL || buf == NULL)
    {
      return -1;
    }

  if (dfu->seq_num != blk_seq_num)
    {
      return -1;
    }
  dfu->seq_num++;

  ret = dfu->dfu_read(buf, size);
  if (ret < 0)
    {
      return -1;
    }

  if (dfu->offset >= dfu->trans_size)
    {
      dprintf("\nUPLOAD ... done\n");
      usbdev_dfu_transaction_cleanup();
    }

  return ret;
}

int usb_dfu_write(struct usb_dfu_entity *dfu, void *buf, int size, uint32_t blk_seq_num)
{
  int ret;

  if (dfu == NULL || buf == NULL)
    {
      return -1;
    }

  if (dfu->seq_num != blk_seq_num)
    {
      usbdev_dfu_transaction_cleanup();
      return -1;
    }
  dfu->seq_num++;

  ret = dfu->dfu_write(buf, size);
  if (ret < 0)
    {
      usbdev_dfu_transaction_cleanup();
      return -1;
    }

  return 0;
}

void usbdev_dfu_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_dfu_device_desc, sizeof(g_dfu_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int usbdev_dfu_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_dfu_string_defs[i].s != NULL; i++)
    {
      str = g_dfu_string_defs[i].s;
      if (g_dfu_string_defs[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }
  usb_err("Can not find the id = %u of string\n", id);

  return -1;
}

static void usbdev_dfu_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  usbdev_dfu_get_composite_devdesc(dev);

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor = minor;               /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_dfu_initialize(struct module *mod, int n, void *arg)
{
  struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev;
  int ret;

  /* ignore compilation warnings */

  (void)mod;
  (void)n;

  if (com_s == NULL)
    {
      return -1;
    }

  usbdev_dfu_initialize_sub(&dev, 0, DEV_DFU);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("** DFU device initialized successfully! **\n");
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */