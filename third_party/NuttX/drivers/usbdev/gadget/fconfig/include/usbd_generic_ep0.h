/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS USB Driver Generic EP0 Data Stream HeadFile
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

#ifndef LITEOS_USB_DEVICE_GENERIC_EP0_H
#define LITEOS_USB_DEVICE_GENERIC_EP0_H

#include "f_generic.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

enum {
  GENERIC_DESCRIPTORS_MAGIC = 1,
  GENERIC_STRINGS_MAGIC = 2,
};

enum generic_flags {
  GENERIC_HAS_FS_DESC = 1,
  GENERIC_HAS_HS_DESC = 2,
  GENERIC_HAS_SS_DESC = 4,
  GENERIC_ALL_CTRL_RECIP = 8,
};

struct generic_descs_head {
  uint32_t magic;
  uint32_t length;
  uint32_t flags;
} __attribute__((packed));

struct generic_strings_head {
  uint32_t magic;
  uint32_t length;
  uint32_t str_count;
  uint32_t lang_count;
} __attribute__((packed));

struct ep0_io_data
{
  uint32_t      buf;
  int32_t       result;
  uint32_t      xfrd;
  uint32_t      len;
};

struct ep0_io_data_ret
{
    uint32_t buf;
    uint32_t actual;
    int      status;
};

struct ep0_request
{
  struct list_head          entry;
  struct ep0_io_data        io_data;
  void                      *priv;
  uint32_t                  user_buf;
};

extern int generic_create_ep0(struct generic_dev_s *dev);
extern int generic_remove_ep0(struct generic_dev_s *dev);

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* LITEOS_USB_DEVICE_GENERIC_EP0_H */
