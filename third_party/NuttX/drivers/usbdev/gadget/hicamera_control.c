/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Hicamera Control
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

#include "implementation/global_implementation.h"
#include "hicamera_control.h"
#include "gadget/f_uvc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UVC_UNIT_MAP_SIZE 4
static struct uvc_camera_cmd g_cmd_map[CMD_MAX_NUM] = {0};
static bool g_uvc_unit_is_register = false;
static struct uvc_camera_cmd g_uvc_unit_mappings[UVC_UNIT_MAP_SIZE];
DEFINE_SPINLOCK(g_cmd_spinlock);

static int cmd_check(struct uvc_camera_cmd *cmd)
{
  if (cmd->id >= CMD_MAX_NUM)
    {
      usb_err("the cmd id is too big, make sure it less than %d\n", CMD_MAX_NUM);
      return -1;
    }

  return 0;
}

void uvc_unit_control_register(void)
{
  int ret;
  uint32_t i;

  if (g_uvc_unit_is_register == false)
    {
      hi_camera_cmd_init();

      for (i = 0; i < sizeof(g_uvc_unit_mappings) / sizeof(g_uvc_unit_mappings[0]); i++)
        {
          ret = hi_camera_register_cmd(&g_uvc_unit_mappings[i]);
          if (ret < 0)
            {
              usb_err("register cmd failed\n");
            }
        }
    }
}

int run_cmd_func(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype, uint32_t cmd_id)
{
  struct uvc_camera_cmd *cmd_cb;
  uint32_t flags;
  int ret;

  if (cmd_id >= CMD_MAX_NUM)
    {
      return -1;
    }

  spin_lock_irqsave(&g_cmd_spinlock, flags);
  cmd_cb = &g_cmd_map[cmd_id];

  ret = cmd_check(cmd_cb);
  if (ret < 0)
    {
      spin_unlock_irqrestore(&g_cmd_spinlock, flags);
      usb_err("cmd check is invalid\n");
      return -1;
    }

  if (cmd_cb->uvc_control_func != NULL)
    {
      ret = (int)cmd_cb->uvc_control_func(buf, len, event_id, cmdtype);
    }
  spin_unlock_irqrestore(&g_cmd_spinlock, flags);

  return ret;
}

void hi_camera_cmd_init(void)
{
  uint32_t flags;

  spin_lock_irqsave(&g_cmd_spinlock, flags);
  (void)memset_s(g_cmd_map, (CMD_MAX_NUM * sizeof(struct uvc_camera_cmd)),
                 0, (CMD_MAX_NUM * sizeof(struct uvc_camera_cmd)));
  g_uvc_unit_is_register = false;
  spin_unlock_irqrestore(&g_cmd_spinlock, flags);
}

static void new_cmd_add(struct uvc_camera_cmd *cmd)
{
  uint32_t cmd_id = cmd->id;
  struct uvc_camera_cmd *cmd_cb = &g_cmd_map[cmd_id];

  cmd_cb->id = cmd_id;
  (void)memcpy_s(cmd_cb->name, sizeof(cmd_cb->name), cmd->name, sizeof(cmd->name));
  cmd_cb->uvc_control_func = cmd->uvc_control_func;
}

int hi_camera_register_cmd(struct uvc_camera_cmd *cmd)
{
  uint32_t flags;
  int ret;

  if (cmd == NULL)
    {
      usb_err("the arg is NULL\n");
      return -1;
    }

  ret = cmd_check(cmd);
  if (ret == -1)
    {
      usb_err("cmd check is invalid\n");
      return -1;
    }

  spin_lock_irqsave(&g_cmd_spinlock, flags);
  new_cmd_add(cmd);
  g_uvc_unit_is_register = true;
  spin_unlock_irqrestore(&g_cmd_spinlock, flags);

  return 0;
}

void hi_camera_cmd_info_print(void)
{
  int i;

  for (i = 0; i < CMD_MAX_NUM; i++)
    {
      dprintf("cmd_id=%u, name:%s, func:%x\n", g_cmd_map[i].id, g_cmd_map[i].name, g_cmd_map[i].uvc_control_func);
    }
}

static uint32_t camera_unit_control_sample(void *buf, uint32_t len, uint32_t cmdtype)
{
  uint8_t *data = (uint8_t *)buf;
  uint32_t ret  = 0;

  (void)len;

  switch (cmdtype)
    {
    case UVC_RC_SETCUR:
    case UVC_RC_GETCUR:
    case UVC_RC_GETMIN:
      ret = 4;
      break;

    case UVC_RC_GETLEN:
      data[0] = 0x40;
      data[1] = 0x00;
      ret     = 2;
      break;

    case UVC_RC_GETINFO:
      data[0] = 0x03;
      ret     = 1;
      break;

    default:
      break;
    }

  return ret;
}

static uint32_t hicamera_unit_control(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype)
{
  uint32_t ret;

  PRINT_INFO("%s %d, event_id=%u, len=%u, cmdtype=%02x\n", __FUNCTION__, __LINE__, event_id, len, cmdtype);

  switch(event_id)
    {
    case HICAMERA_GET_VERSION_CONTROL:
    case HICAMERA_START_UPDATE_CONTROL:
    case HICAMERA_TRUN_ON_CONTROL:
    case HICAMERA_RESET_CONTROL:
    case HICAMERA_TRUN_OFF_CONTROL:
    case HICAMERA_VIBRATOR_UP_CONTROL:
    case HICAMERA_VIBRATOR_DOWN_CONTROL:
    case HICAMERA_VIBRATOR_LEFT_CONTROL:
    case HICAMERA_VIBRATOR_RIGHT_CONTROL:
    default:
      ret = camera_unit_control_sample(buf, len, cmdtype);
      break;
    }

  return ret;
}

static uint32_t h264_unit_control(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype)
{
  uint32_t ret  = 0;
  uint8_t *data = (uint8_t *)buf;

  PRINT_INFO("%s %d, event_id=%u, len=%u, cmdtype=%02x\n", __FUNCTION__, __LINE__, event_id, len, cmdtype);

  switch (cmdtype)
    {
    case UVC_RC_SETCUR:
      ret = 4;
      break;

    case UVC_RC_GETLEN:
      data[0] = 0x40;
      data[1] = 0x00;
      ret     = 0x2;
      break;

    case UVC_RC_GETINFO:
      data[0] = 0x3;
      ret     = 0x1;
      break;

    case UVC_RC_GETMIN:
    case UVC_RC_GETMAX:
    case UVC_RC_GETRES:
    case UVC_RC_GETDEF:
      ret = 4;
      break;

    default:
      usb_err("Yet to be supported cmdtype: %#x\n", cmdtype);
      break;
    }

  return ret;
}

static void histream_pu_set_brightness(uint16_t val)
{
  PRINT_INFO("set brightness val = %u\n", val);
}

static uint16_t histream_pu_get_brightness(void)
{
  const uint16_t cur_val = 30;

  PRINT_INFO("get brightness val = %u\n", cur_val);

  return cur_val;
}

static uint32_t sample_uvc_pu_brightness_ctrl(void *buf, uint32_t len, uint32_t cmdtype)
{
  uint32_t ret  = 0;
  uint8_t *data = (uint8_t *)buf;
  uint16_t val;

  (void)len;

  switch (cmdtype)
    {
    case UVC_RC_SETCUR:
      val = data[0] + (data[1] << 8);
      histream_pu_set_brightness(val);
      ret = 2;
      break;

    case UVC_RC_GETCUR:
      val = histream_pu_get_brightness();
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETRES:
      data[0] = 0x1;
      ret     = 2;
      break;

    case UVC_RC_GETINFO:
      data[0] = 0x3;
      ret     = 1;
      break;

    case UVC_RC_GETMIN:
      val = 0;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETMAX:
      val = 0x64;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETDEF:
      val = 0x32;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    default:
      break;
    }

  return ret;
}

static void histream_pu_set_contrast(uint16_t val)
{
  PRINT_INFO("set contrast val = %u\n", val);
}

static uint16_t histream_pu_get_contrast(void)
{
  const uint16_t cur_cal = 30;

  PRINT_INFO("get contrast val = %u\n", __FUNCTION__, __LINE__);

  return cur_cal;
}

static uint32_t sample_uvc_pu_contrast_ctrl(void *buf, uint32_t len, uint32_t cmdtype)
{
  uint32_t ret  = 0;
  uint8_t *data = (uint8_t *)buf;
  uint16_t val;

  (void)len;

  switch (cmdtype)
    {
    case UVC_RC_SETCUR:
      val = data[0] + (data[1] << 8);
      histream_pu_set_contrast(val);
      ret = 2;
      break;

    case UVC_RC_GETCUR:
      val = histream_pu_get_contrast();
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETRES:
      data[0] = 0x1;
      ret     = 2;
      break;

    case UVC_RC_GETINFO:
      data[0] = 0x3;
      ret     = 1;
      break;

    case UVC_RC_GETMIN:
      val = 0;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETMAX:
      val = 0x64;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    case UVC_RC_GETDEF:
      val = 0x32;
      data[0] = val & 0xff;
      data[1] = (val >> 8) & 0xff;
      ret     = 2;
      break;

    default:
      break;
    }

  return ret;
}

static uint32_t uvc_process_unit_contorl(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype)
{
  uint32_t ret = 0;

  PRINT_INFO("%s %d, event_id=%u, len=%u, cmdtype=%02x\n", __FUNCTION__, __LINE__, event_id, len, cmdtype);

  switch (event_id)
    {
    case USBD_UVC_PU_CONTROL_UNDEFINED:
      break;

    case USBD_UVC_PU_BACKLIGHT_COMPENSATION_CONTROL:
      break;

    case USBD_UVC_PU_BRIGHTNESS_CONTROL:
      ret = sample_uvc_pu_brightness_ctrl(buf, len, cmdtype);
      break;

    case USBD_UVC_PU_CONTRAST_CONTROL:
      ret = sample_uvc_pu_contrast_ctrl(buf, len, cmdtype);
      break;

    case USBD_UVC_PU_GAIN_CONTROL:
      break;

    case USBD_UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
      break;

    case USBD_UVC_PU_HUE_CONTROL:
      break;

    case USBD_UVC_PU_SATURATION_CONTROL:
      break;

    case USBD_UVC_PU_SHARPNESS_CONTROL:
      break;

    case USBD_UVC_PU_GAMMA_CONTROL:
      break;

    case USBD_UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
      break;

    case USBD_UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTORL:
      break;

    case USBD_UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL:
      break;

    case USBD_UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
      break;

    case USBD_UVC_PU_DIGITAL_MULTIPLIER_CONTROL:
      break;

    case USBD_UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
      break;

    case USBD_UVC_PU_HUE_AUTO_CONTROL:
      break;

    case USBD_UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL:
      break;

    case USBD_UVC_PU_ANALOG_LOCK_STATUS_CONTROL:
      break;

    case USBD_UVC_PU_CONTRAST_AUTO_CONTROL:
      break;

    default:
      break;
    }

  return ret;
}

static uint32_t camera_terminal_unit_contorl(void *buf, uint32_t len, uint32_t event_id, uint32_t cmdtype)
{
  (void)buf;

  PRINT_INFO("%s %d, event_id=%u, len=%u, cmdtype=%02x\n", __FUNCTION__, __LINE__, event_id, len, cmdtype);

  switch (event_id)
    {
    case USBD_UVC_CT_CONTROL_UNDEFINED:
      break;

    case USBD_UVC_CT_SCANNING_MODE_CONTROL:
      break;

    case USBD_UVC_CT_AE_MODE_CONTROL:
      break;

    case USBD_UVC_CT_AE_PRIORITY_CONTROL:
      break;

    case USBD_UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_FOCUS_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_FOCUS_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_FOCUS_AUTO_CONTROL:
      break;

    case USBD_UVC_CT_IRIS_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_IRIS_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_ZOOM_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_ZOOM_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_PANTILT_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_PANTILT_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_ROLL_ABSOLUTE_CONTROL:
      break;

    case USBD_UVC_CT_ROLL_RELATIVE_CONTROL:
      break;

    case USBD_UVC_CT_PRIVACY_CONTROL:
      break;

    case USBD_UVC_CT_FOCUS_SIMPLE_CONTROL:
      break;

    case USBD_UVC_CT_WINDOW_CONTROL:
      break;

    case USBD_UVC_CT_REGION_OF_INTEREST_CONTROL:
      break;

    default:
      break;
    }

  return 0;
}

static struct uvc_camera_cmd g_uvc_unit_mappings[UVC_UNIT_MAP_SIZE] =
{
  {
    .id = CMD_HICAMERA_UNIT_CONTROL,
    .name = "hicamera_unit_control",
    .uvc_control_func = hicamera_unit_control,
  },
  {
    .id = CMD_H264_UNIT_CONTROL,
    .name = "h264_unit_control",
    .uvc_control_func = h264_unit_control,
  },
  {
    .id = CMD_CAMERA_UNIT_CONTROL,
    .name = "camera_terminal_unit_contorl",
    .uvc_control_func = camera_terminal_unit_contorl,
  },
  {
    .id = CMD_PROCESS_UNIT_CONTROL,
    .name = "uvc_process_unit_contorl",
    .uvc_control_func = uvc_process_unit_contorl,
  },
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
