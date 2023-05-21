/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: LiteOS USB Driver RNDIS Protocol HeadFile
 * Author: huangjieliang
 * Create: 2016-07-13
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

#ifndef F_ETHER_H
#define F_ETHER_H

#include <los_mac.h>
#include "gadget/usbdev.h"
#include "implementation/freebsd_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct fether_rndis_config_desc
{
  struct usb_config_descriptor confd;
  struct usb_interface_assoc_descriptor ifcad;
  struct usb_interface_descriptor ifcd;
  struct usb_cdc_header_desc cdc_desc;
  struct usb_cdc_call_mgmt_descriptor cdc_call_desc;
  struct usb_cdc_acm_descriptor cdc_acm_desc;
  struct usb_cdc_union_desc cdc_union_desc;
  struct usb_endpoint_descriptor nepd;
  struct usb_interface_descriptor ifdd;
  struct usb_endpoint_descriptor iepd;
  struct usb_endpoint_descriptor oepd;
} __packed;

/* Device descriptor */

struct rndis_hs_function_descriptor
{
  struct usb_interface_assoc_descriptor ifcad;
  struct usb_interface_descriptor ifcd;
  struct usb_cdc_header_desc cdc_desc;
  struct usb_cdc_call_mgmt_descriptor cdc_call_desc;
  struct usb_cdc_acm_descriptor cdc_acm_desc;
  struct usb_cdc_union_desc cdc_union_desc;
  struct usb_endpoint_descriptor nepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_endpoint_ss_comp_descriptor ncompd;
#endif
  struct usb_interface_descriptor ifdd;
  struct usb_endpoint_descriptor iepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_endpoint_ss_comp_descriptor icompd;
#endif
  struct usb_endpoint_descriptor oepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_endpoint_ss_comp_descriptor ocompd;
#endif
} __packed;

struct rndis_hs_descriptor
{
  struct usb_config_descriptor        *rndis_config;
  struct rndis_hs_function_descriptor *rndis_func;
};

extern void rndis_get_composite_devdesc(struct composite_devdesc_s *dev);
extern void eth_rx(struct los_eth_driver *sc, const struct usbdev_req_s *req);
extern void netdev_register(struct los_eth_driver *sc);
extern void netdev_unregister(struct los_eth_driver *sc);
extern void eth_random_addr(uint8_t *addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
