/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Hicamera Control HeadFile
 * Author: huangjieliang
 * Create: 2017-11-25
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

#ifndef HICAMERA_CONTROL_H
#define HICAMERA_CONTROL_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CMD_MAX_NUM       32

#define HICAMERA_GET_VERSION_CONTROL    0x01
#define HICAMERA_START_UPDATE_CONTROL   0x02
#define HICAMERA_TRUN_ON_CONTROL        0x03
#define HICAMERA_TRUN_OFF_CONTROL       0x04
#define HICAMERA_RESET_CONTROL          0x05
#define HICAMERA_VIBRATOR_UP_CONTROL    0x06
#define HICAMERA_VIBRATOR_DOWN_CONTROL  0x07
#define HICAMERA_VIBRATOR_LEFT_CONTROL  0x08
#define HICAMERA_VIBRATOR_RIGHT_CONTROL 0x09

enum camera_cmd
{
  CMD_CAMERA_UNIT_CONTROL   = 1,
  CMD_PROCESS_UNIT_CONTROL  = 2,
  CMD_HICAMERA_UNIT_CONTROL = 0x11,  /* Extern unit */
  CMD_H264_UNIT_CONTROL     = 0xa,   /* Extern unit */
};

typedef uint32_t (*camera_control_func)(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype);

struct uvc_camera_cmd
{
  uint32_t id;
  uint8_t name[32];
  camera_control_func uvc_control_func;
};

extern void hi_camera_cmd_init(void);
extern int hi_camera_register_cmd(struct uvc_camera_cmd *cmd);
extern int run_cmd_func(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype, uint32_t cmd_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
