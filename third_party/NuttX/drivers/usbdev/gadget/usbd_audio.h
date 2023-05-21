/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Audio Stream HeadFile
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

#ifndef LITEOS_USB_DEVICE_AUDIO_H
#define LITEOS_USB_DEVICE_AUDIO_H

#include "gadget/f_uac.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* UAC function error codes */

#define UAC_OK                   0x00     /* No error, function has successfully completed */
#define UAC_ERROR_NOMATCH        0x01     /* The operation can not be complete, due to mismatch of UAC state */
#define UAC_ERROR_PTR            0x02     /* Invalid pointer */
#define UAC_ERROR_MEMORY         0x03     /* Memory error */
#define UAC_ERROR_VALUE          0x04     /* Incorrect value */
#define UAC_ERROR_FATAL          0x05     /* Fatal internal error, please contact the one who wrote the code */

#define UAC_WAIT_HOST_NOP      0x0
#define UAC_WAIT_HOST_FOREVER  0x1

extern int uac_wait_host(int wait_option);
extern int fuac_send_message(const void *buf,int len);

struct uac_queue_node
{
  uint8_t *buf;
  uint32_t buf_len;
  uint32_t buf_used;
  struct list_head irqqueue;
};

struct uac_queue_node *uac_queue_node_alloc(unsigned len);
void uac_queue_node_free(struct uac_queue_node *node);
void uac_send_data_sub(struct uac_dev_s *sc);
void uac_queue_free(struct uac_dev_s *sc);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif