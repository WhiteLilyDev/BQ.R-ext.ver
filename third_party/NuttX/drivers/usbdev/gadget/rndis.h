/************************************************************************************
 * include/nuttx/usb/rndis.h
 *
 *   Copyright (C) 2011-2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Authors: Sakari Kapanen <sakari.m.kapanen@gmail.com>,
              Petteri Aimonen <jpa@git.mail.kapsi.fi>
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
 ************************************************************************************/
/****************************************************************************
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations,
 * which might include those applicable to Huawei LiteOS of U.S. and the country in
 * which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in
 * compliance with such applicable export control laws and regulations.
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_USB_RNDIS_H
#define __INCLUDE_NUTTX_USB_RNDIS_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <linux/module.h>
#include <los_mac.h>
#include "gadget/usbdev.h"
#include "gadget/rndis_std.h"
#include "net/usb_eth_drv.h"

/************************************************************************************
 * Public Data
 ************************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#  define EXTERN extern "C"
extern "C"
{
#else
#  define EXTERN extern
#endif

#define RNDIS_PACKET_HDR_SIZE   (sizeof(struct rndis_packet_msg))

#define RNDIS_EP_INTIN_IDX        (0)
#define RNDIS_EP_BULKIN_IDX       (1)
#define RNDIS_EP_BULKOUT_IDX      (2)

/* Endpoint configuration ****************************************************/

#define RNDIS_MKEPINTIN(desc)     ((desc)->epno[RNDIS_EP_INTIN_IDX])

#define RNDIS_MKEPBULKIN(desc)    ((desc)->epno[RNDIS_EP_BULKIN_IDX])

#define RNDIS_MKEPBULKOUT(desc)   ((desc)->epno[RNDIS_EP_BULKOUT_IDX])

extern struct rndis_hs_descriptor g_rndis_hs_desc;
extern struct rndis_hs_function_descriptor g_rndis_hs_func_desc;

/************************************************************************************
 * Public Functions
 ************************************************************************************/

extern int usbdev_rndis_initialize (struct module *mod, int n, void *arg);

extern void rndis_tx(struct los_eth_driver *sc, const struct eth_drv_sg *sg_list,
                     int sg_len, uint32_t total_len, UINTPTR key);
extern void usbdev_rndis_initialize_sub(struct composite_devdesc_s *dev,
                                        int ifnobase, int minor);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __INCLUDE_NUTTX_USB_RNDIS_H */
