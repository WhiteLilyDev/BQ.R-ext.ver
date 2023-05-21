/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver DFU Protocol HeadFile
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

#ifndef _USB_F_DFU_H_
#define _USB_F_DFU_H_

#include "gadget/dfu.h"
#include "gadget/usbdev.h"
#include "implementation/global_implementation.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DFU_NAME_MAX_LEN  32

struct usb_dfu_dev
{
  struct dfu_getstatus_response_s dfu_status;
  int dfu_state;
  uint32_t seq_num;
};

struct usb_dfu_entity
{
  char dfu_name[DFU_NAME_MAX_LEN];
  void *ram_addr;
  uint32_t ram_size;
  int (*dfu_read)(uint8_t *buf, uint32_t len);
  int (*dfu_write)(const uint8_t *buf, uint32_t len);
  uint64_t trans_size;
  uint32_t trans_complete;
  uint32_t seq_num;
  uint32_t offset;
};

struct string_des
{
  char *str;
  int len;
};

/* The status of Flash upgrade state */

#define DFU_FLASH_STATE_UPGRADING   1
#define DFU_FLASH_STATE_UPGRADED    0
#define DFU_FLASH_STATE_ERROR       2

/* Minimum time, in milliseconds, that the host should wait before
 * sending a subsequent DFU_GETSTATUS request.
 */

#define USB_DFU_POLLTIMEOUT 100

/* Define the return value for request */

#define USB_DFU_RET_ERR        -1
#define USB_DFU_RET_OK          0
#define USB_DFU_RET_STATUS_LEN  6
#define USB_DFU_RET_STATE_LEN   1
#define USB_DFU_STATUS_OK       0

/* Define the type for request */

#define USB_DFU_DETACH      0
#define USB_DFU_DNLOAD      1
#define USB_DFU_UPLOAD      2
#define USB_DFU_GET_STATUS  3
#define USB_DFU_CLR_STATUS  4
#define USB_DFU_GET_STATE   5
#define USB_DFU_ABORT       6
#define USB_DFU_FLASH_STATE 7

/* Define the state for request */

#define USB_DFU_APP_IDLE            0
#define USB_DFU_APP_DETACH          1
#define USB_DFU_IDLE                2
#define USB_DFU_DNLOAD_SYNC         3
#define USB_DFU_DNBUSY              4
#define USB_DFU_DNLOAD_IDLE         5
#define USB_DFU_MANIFEST_SYNC       6
#define USB_DFU_MANIFEST            7
#define USB_DFU_MANIFEST_WAIT_RESET 8
#define USB_DFU_UPLOAD_IDLE         9
#define USB_DFU_ERROR              10

/* Define the status for request */

#define USB_DFU_OK                0x00
#define USB_DFU_ERR_TARGET        0x01
#define USB_DFU_ERR_FILE          0x02
#define USB_DFU_ERR_WRITE         0x03
#define USB_DFU_ERR_ERASE         0x04
#define USB_DFU_ERR_CHECK_ERASED  0x05
#define USB_DFU_ERR_PROG          0x06
#define USB_DFU_ERR_VERIFY        0x07
#define USB_DFU_ERR_ADDRESS       0x08
#define USB_DFU_ERR_NOTDONE       0x09
#define USB_DFU_ERR_FIRMWARE      0x0A
#define USB_DFU_ERR_VENDOR        0x0B
#define USB_DFU_ERR_USBR          0x0C
#define USB_DFU_ERR_POR           0x0D
#define USB_DFU_ERR_UNKNOWN       0x0E
#define USB_DFU_ERR_STALLEDPKT    0x0F

extern struct usb_config_descriptor g_dfu_config_desc;

bool usb_dfu_running(void);
void usbdev_dfu_mkdevdesc(uint8_t *buf);
int usbdev_dfu_mkstrdesc(uint8_t id, uint8_t *buf);
void usbdev_dfu_set_config(void);
void usbdev_dfu_init(void);
void usbdev_dfu_dev_init(void);
void usbdev_dfu_dev_deinit(void);
void usbdev_dfu_transaction_cleanup(void);
int usbdev_dfu_class_requests(const struct usbdev_s *dev,
                              const struct usb_device_request *ctrl,
                              struct usbdev_req_s *ctrlreq);

void usb_dfu_free_entities(void);
int usb_dfu_init_env_entities(char *type, char *envstr, char *devstr);
struct usb_dfu_entity *usb_dfu_get_entity(int alter);
void set_flash_state(volatile uint8_t flash_state);
void get_flash_state(struct usbdev_req_s *req);
uint32_t usb_dfu_update_status(void);
uint64_t *usb_dfu_update_size_get(void);
int usb_dfu_read(struct usb_dfu_entity *dfu, void *buf, int size, uint32_t blk_seq_num);
int usb_dfu_write(struct usb_dfu_entity *dfu, void *buf, int size, uint32_t blk_seq_num);
extern char uart_putc(char);

void set_manufacturer_string(struct string_des *des);
void set_serial_number_string(struct string_des *des);
void set_product_string(struct string_des *des);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _USB_F_DFU_H_ */
