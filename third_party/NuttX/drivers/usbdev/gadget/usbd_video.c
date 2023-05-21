/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver Video Stream
 * Author: huangjieliang
 * Create: 2017-04-17
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

#include <los_mux.h>
#include "gadget/f_uvc.h"
#include "implementation/global_implementation.h"
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otgreg.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#endif
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#include "controller/usb_device/pcd.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*
 * module local variables
 */

uint32_t g_uvc_mutex = UVC_INVALID_HANDLE;

/* private pointer to the UVC handle structure */

static struct uvc_handle *volatile g_uvc = (struct uvc_handle *)UVC_INVALID_HANDLE;

/* private pointer to the F_UVC structure */

struct uvc_dev_s *volatile g_f_uvc = (struct uvc_dev_s *)UVC_INVALID_HANDLE;

static int uvc_copy_continue(struct uvc_handle *uvc, struct uvc_isoc_transfer *tran);
static int uvc_nocp_continue(struct uvc_handle *uvc, struct uvc_isoc_transfer *tran);

static void uvc_release_mutex(void)
{
  uint32_t ret;
  ret = LOS_MuxPost(g_uvc_mutex);
  if (ret != LOS_OK)
    {
      usb_err("Failed to unlock mutex: %#x\n", ret);
    }
}

void uvc_delete_mutex(void)
{
  (void)LOS_MuxDelete(g_uvc_mutex);
  g_uvc_mutex = UVC_INVALID_HANDLE;
}

bool uvc_is_running(void)
{
  return (g_uvc == (struct uvc_handle *)UVC_INVALID_HANDLE) ? false : true;
}

/*
 * Check the UVC handle provided by external source
 */

static int uvc_check_handle(uvc_t hdl)
{
  uint32_t ret;
  struct uvc_handle *uvc;

  /* catch NULL pointer */

  if (hdl == NULL)
    {
      return UVC_ERROR_PTR;
    }

  ret = LOS_MuxPend(g_uvc_mutex, LOS_WAIT_FOREVER);
  if (ret != LOS_OK)
    {
      usb_err("Failed to acquire UVC mutex: %#x, %p\n", ret, __builtin_return_address(0));
      return UVC_ERROR_VALUE;
    }

  /* check whether the provided handle matches the one privated stored */

  uvc = g_uvc;
  if (uvc == (struct uvc_handle *)UVC_INVALID_HANDLE ||
      hdl != (uvc_t)uvc)
    {
      uvc_release_mutex();
      return UVC_ERROR_HANDLE;
    }

  return UVC_OK;
}

enum format_switch_status uvc_format_status_check(void)
{
  struct uvc_dev_s *fuvc = g_f_uvc;

  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      usb_err("uvc is not initialization\n");
      return FORMAT_SWITCH_FINISH;
    }

  return (enum format_switch_status)fuvc->format_info.status;
}

int uvc_format_info_get(struct uvc_format_info *info)
{
  struct uvc_format_info *format_info;
  struct uvc_dev_s *fuvc = g_f_uvc;

  if (info == NULL)
    {
      usb_err("the info is NULL\n");
      return UVC_ERROR_PTR;
    }

  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      usb_err("uvc is not initialization\n");
      return UVC_ERROR_PTR;
    }

  format_info = &fuvc->format_info;
  if (format_info->status == FORMAT_SWITCH_PENDING)
    {
      if (format_info->format != V4L2_PIX_FMT_YUYV &&
          format_info->format != V4L2_PIX_FMT_H264 &&
          format_info->format != V4L2_PIX_FMT_MJPEG)
        {
          return UVC_ERROR_NOMATCH;
        }

      info->height = format_info->height;
      info->width  = format_info->width;
      info->format = format_info->format;

      fuvc->format_info.status = FORMAT_SWITCH_FINISH;

      return UVC_OK;
    }
  else
    {
      usb_err("the uvc format status is not update\n");
      return UVC_ERROR_NOMATCH;
    }
}

static int uvc_wait_host_sub(uvc_t hdl, int *connected)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }
  uvc = (struct uvc_handle *)hdl;

  fuvc = uvc->fuvc;
  if (fuvc == NULL || fuvc != g_f_uvc)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  if ((*connected = fuvc_host_connected(fuvc)) != 0)
    {
      if (uvc->state == UVC_STATE_IDLE)
        {
          uvc->state = UVC_STATE_CONN;
        }
    }

  uvc_release_mutex();
  return ret;
}

int uvc_wait_host(uvc_t hdl, int wait_option, int *connected)
{
  int ret;

  if (connected == NULL)
    {
      return UVC_ERROR_PTR;
    }

  switch (wait_option)
    {
    case UVC_WAIT_HOST_NOP:
      ret = uvc_wait_host_sub(hdl, connected);
      break;

    case UVC_WAIT_HOST_FOREVER:
      while (1)
        {
          ret = uvc_wait_host_sub(hdl, connected);
          if (ret != UVC_OK || *connected)
            {
              break;
            }
          (void) LOS_TaskDelay(10);
        }
      break;

    default:
      ret = UVC_ERROR_VALUE;
    }

  return ret;
}

int uvc_open_device(uvc_t *hdl, struct uvc_open_param *param)
{
  uint32_t rval;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;

  /* NULL pointers are not allowed */

  if (hdl == NULL || param == NULL)
    {
      return UVC_ERROR_PTR;
    }

  /* check the function input video parameter */

  if (param->vid_width <= 0x1 || param->vid_width > UVC_VIDEO_WIDTH_MAX)
    {
      return UVC_ERROR_VALUE;
    }
  if (param->vid_height <= 0x1 || param->vid_height > UVC_VIDEO_HEIGHT_MAX)
    {
      return UVC_ERROR_VALUE;
    }
  if (param->vid_format != UVC_VFF_YUY2 &&
      param->vid_format != UVC_VFF_H264 &&
      param->vid_format != UVC_VFF_MJPG)
    {
      return UVC_ERROR_VALUE;
    }

  rval = LOS_MuxPend(g_uvc_mutex, LOS_WAIT_FOREVER);
  if (rval != LOS_OK)
    {
      usb_err("Failed to lock UVC mutex: %#x\n", rval);
      return UVC_ERROR_VALUE;
    }

  uvc = g_uvc;
  if (uvc != (struct uvc_handle *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      return UVC_ERROR_HANDLE; /* already opened */
    }

  /* check whether F_UVC module has been initialized */

  fuvc = g_f_uvc;
  if (fuvc == (struct uvc_dev_s *)UVC_INVALID_HANDLE)
    {
      uvc_release_mutex();
      return UVC_ERROR_VALUE;
    }

  /* allocate memory for uvc_handle structure */

  uvc = calloc(0x1, sizeof(struct uvc_handle));
  if (uvc == NULL)
    {
      uvc_release_mutex();
      return UVC_ERROR_MEMORY;
    }

  /* initialize the UVC handle */

  uvc->vid_w       = param->vid_width;
  uvc->vid_h       = param->vid_height;
  uvc->vid_f       = param->vid_format;
  uvc->state       = UVC_STATE_IDLE;
  uvc->fuvc        = fuvc;
  fuvc->uvc_handle = (void *)uvc;

  fuvc->bulk_size = param->bulk_size;
  fuvc->imgsize   = param->imgsize;

  /* store a private copy of UVC handle */

  g_uvc = uvc;

  /* return the UVC handle */

  *hdl = (uvc_t)uvc;
  uvc_release_mutex();
  return UVC_OK;
}

/*
 * local function to stop video stream transmission process
 * return error only when a transmission cannot be stopped
 */

static int uvc_video_tran_stop(struct uvc_handle *uvc)
{
  switch (uvc->state)
    {
    case UVC_STATE_TRAN_COPY:
      uvc->copy_func = NULL;
      break;

    case UVC_STATE_TRAN_NOCP:
      uvc->next_func = NULL;
      break;

    default:
      return UVC_OK;
    }

  uvc->_priv = NULL;

  /* update UVC handle state */

  if (uvc->fuvc != NULL)
    {
      uvc->state = fuvc_host_connected(uvc->fuvc) ? UVC_STATE_CONN : UVC_STATE_IDLE;
    }
  else
    {
      uvc->state = UVC_STATE_IDLE;
    }
  return UVC_OK;
}

int uvc_video_stop(uvc_t hdl)
{
  int ret;

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }

  ret = uvc_video_tran_stop((struct uvc_handle *)hdl);
  uvc_release_mutex();
  return ret;
}

int uvc_close_device(uvc_t hdl)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;
  uint32_t flags;

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }
  uvc = (struct uvc_handle *)hdl;

  /* stop Video transmission first */

  if (uvc->state == UVC_STATE_TRAN_COPY ||
      uvc->state == UVC_STATE_TRAN_NOCP)
    {
      (void)uvc_video_tran_stop(uvc);
    }

  fuvc = uvc->fuvc;
  if (fuvc == NULL)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }
  spin_lock_irqsave(&fuvc->lock, flags);
  fuvc->uvc_handle = (void *)UVC_INVALID_HANDLE;

  /* set the global handle to NULL, and free the handle structure */

  g_uvc = (struct uvc_handle *)UVC_INVALID_HANDLE;
  uvc->fuvc = NULL;
  spin_unlock_irqrestore(&fuvc->lock, flags);
  free(uvc);
  uvc_release_mutex();
  return UVC_OK;
}

int uvc_get_state(uvc_t hdl, uint32_t *state)
{
  int ret;
  struct uvc_handle *uvc;

  /* validate function parameters */

  if (state == NULL)
    {
      return UVC_ERROR_PTR;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }

  uvc = (struct uvc_handle *)hdl;
  if (uvc->state == UVC_STATE_IDLE && uvc->fuvc &&
      fuvc_host_connected(uvc->fuvc))
    {
      uvc->state = UVC_STATE_CONN;
    }

  *state = uvc->state;
  uvc_release_mutex();
  return UVC_OK;
}

int uvc_video_tran_nocp(uvc_t hdl, uvc_continue_func next_func, void *priv)
{
  int ret;
  struct uvc_handle *uvc;
  struct nocp_reserve *res;

  /* function reference & private pointer should not be NULL */

  if (next_func == NULL || priv == NULL)
    {
      return UVC_ERROR_PTR;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }
  uvc = (struct uvc_handle *)hdl;

  if (uvc->fuvc == NULL)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  if (uvc->state != UVC_STATE_CONN)
    {
      uvc_release_mutex();
      return UVC_ERROR_NOMATCH;
    }

  /* initialize related UVC handle field members */

  uvc->frame_off    = 0;
  uvc->tdata.data   = NULL;
  uvc->tdata.length = 0;
  uvc->tdata.last   = 0;

  /* store callback function and parameter */

  uvc->copy_func = NULL;
  uvc->next_func = next_func;
  uvc->_priv     = priv;

  /* initialize reserve structure */

  res       = &(uvc->reserve);
  res->res  = NULL;
  res->resl = 0;

  /* get the first frame of video data */

  ret = next_func(hdl, &(uvc->tdata), priv);
  if (ret != UVC_OK)
    {
      uvc_release_mutex();
      return ret;
    }

  uvc->state = UVC_STATE_TRAN_NOCP;
  ret        = fuvc_transfer_initiate(uvc->fuvc);
  if (ret != UVC_OK)
    {
      uvc->state = UVC_STATE_ERR;
      uvc_release_mutex();
      return ret;
    }
  uvc_release_mutex();
  return UVC_OK;
}

int uvc_video_tran_copy(uvc_t hdl, uvc_continue_func copy_func, void *priv)
{
  int ret;
  struct uvc_handle *uvc;
  struct uvc_dev_s *fuvc;

  if (copy_func == NULL || priv == NULL)
    {
      return UVC_ERROR_PTR;
    }

  ret = uvc_check_handle(hdl);
  if (ret != UVC_OK)
    {
      return ret;
    }

  uvc = (struct uvc_handle *)hdl;
  if (uvc->fuvc == NULL)
    {
      uvc_release_mutex();
      return UVC_ERROR_PTR;
    }

  fuvc = uvc->fuvc;

  if (g_start_transfer == 0)
   {
      fuvc->connected = 0;
      uvc_release_mutex();
      usb_err("uvc can not start transfer!\n");
      return UVC_ERROR_PTR;
    }

  /* check whether UVC state matches */

  if (uvc->state != UVC_STATE_CONN)
    {
      uvc_release_mutex();
      return UVC_ERROR_NOMATCH;
    }

  /* initialize related UVC handle field members */

  uvc->frame_off    = 0;
  uvc->tdata.data   = NULL;
  uvc->tdata.length = 0;
  uvc->tdata.last   = 0;

  /* set the callback function and parameter */

  uvc->next_func = NULL;
  uvc->copy_func = copy_func;

  if (fuvc->transfer_status == STREAM_OFF)
    {
      uvc_release_mutex();
      usb_err("uvc transfer status is stream off!\n");
      return UVC_ERROR_PTR;
    }

  uvc->_priv = priv;

  /* update UVC handle state */

  uvc->state = UVC_STATE_TRAN_COPY;
  ret = fuvc_transfer_initiate(uvc->fuvc);
  if (ret != UVC_OK)
    {
      uvc->state = UVC_STATE_ERR;
      uvc_release_mutex();
      return ret;
    }
  uvc_release_mutex();

  ret = LOS_EventRead(&g_uvc_event, 0x01, LOS_WAITMODE_OR |
                      LOS_WAITMODE_CLR, 100);
  if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT)
    {
      usb_err("Transfer timeout!\n");
      ret = uvc_check_handle(hdl);
      if (ret)
        {
          return ret;
        }
      usbclass_uvc_stream_ep_reset(fuvc);
      uvc_release_mutex();
      return UVC_ERROR_VALUE;
    }

  return UVC_OK;
}

int uvc_continue_transfer(struct uvc_dev_s *fuvc, struct uvc_isoc_transfer *tran)
{
  struct uvc_handle *uvc;
  uint32_t flags;
  int ret = UVC_ERROR_PTR;

  spin_lock_irqsave(&fuvc->lock, flags);
  uvc = (struct uvc_handle *)fuvc->uvc_handle;
  if (uvc == (struct uvc_handle *)UVC_INVALID_HANDLE)
    {
      goto done;
    }

  if (uvc->copy_func)
    {
      ret = uvc_copy_continue(uvc, tran);
      goto done;
    }
  if (uvc->next_func)
    {
      ret = uvc_nocp_continue(uvc, tran);
    }

done:
  spin_unlock_irqrestore(&fuvc->lock, flags);
  return ret;
}

static int uvc_nocp_continue(struct uvc_handle *uvc, struct uvc_isoc_transfer *tran)
{
  uvc_continue_func next_func;
  struct nocp_reserve *reserve;
  struct uvc_transfer_data *transfer;
  uint32_t length, offset, res;

  /* initialize local variables */

  length   = tran->length;
  offset   = uvc->frame_off;
  transfer = &(uvc->tdata);
  reserve  = &(uvc->reserve);

  /* restore the reserved data */

  if (reserve->res != NULL)
    {
      uint32_t i, j;
      j = (reserve->resl > NOCP_RESERVE_MAX) ? NOCP_RESERVE_MAX : reserve->resl;
      for (i = 0; i < j; ++i)
        {
          reserve->res[i] = reserve->res_buf[i];
        }
    }

  /* determine how where the transfer should start */

  if (transfer->data == NULL)
    {
      return UVC_ERROR_PTR;
    }

  tran->data = &(transfer->data[offset]);
  res        = tran->reserved;
  if (offset < res)
    {
      res = 0;
      tran->reserved = 0;

      /* do not reserve */

      reserve->res  = NULL;
      reserve->resl = res;
    }
  else
    {
      uint32_t i, j;
      uint8_t *p;

      j = (res > NOCP_RESERVE_MAX) ? NOCP_RESERVE_MAX : res;
      p = tran->data - j;

      for (i = 0; i < j; ++i)
        {
          reserve->res_buf[i] = p[i];
        }
      reserve->resl = j;
      reserve->res  = p;
    }

  /* determine whether reached the frame end */

  if ((offset + length) >= transfer->length)
    {
      tran->length   = transfer->length - offset;
      transfer->data = NULL;

      /* get the next frame of video data */

      if ((next_func = uvc->next_func) != NULL)
        {
          int ret;
          ret = next_func(uvc, transfer, uvc->_priv);
          if (ret != UVC_OK)
            {
              dprintf("next_func %p returned: %d\n", next_func, ret);
            }
        }
      offset     = 0;
      tran->last = 1;
    }
  else
    {
      uintptr_t ptr;

      ptr  = (uintptr_t)tran->data;
      ptr -= tran->res_next;

      /* make sure that next transimission buffer aligns at 4-byte boundary */

      if ((ptr + length) & 0x3)
        {
          while ((ptr + length) & 0x3)
            {
              if (length == 0)
                {
                  break;
                }
              length--;
            }
          tran->length = length;
        }

      /* increment frame offset */

      offset += length;
      tran->last = 0;
    }

  /* write back computed values */

  uvc->frame_off = offset;
  return UVC_OK;
}

static int uvc_copy_continue(struct uvc_handle *uvc, struct uvc_isoc_transfer *tran)
{
  int ret;
  uint32_t cplen;
  uvc_continue_func copy;
  struct uvc_transfer_data *frame;

  copy = uvc->copy_func;
  if (copy == NULL)
    {
      return UVC_ERROR_PTR;
    }

  frame         = &(uvc->tdata);
  frame->data   = tran->data;
  frame->length = tran->length;
  frame->last   = 0;
  cplen         = tran->length;

  ret = copy(uvc, frame, uvc->_priv);
  if (ret != UVC_OK)
    {
      usb_err("copy(...) has failed: %d\n", ret);
      return ret;
    }

  if (cplen != frame->length)
    {
      tran->length = frame->length;
    }
  tran->last = frame->last;

  return UVC_OK;
}

int uvc_stream_init(struct uvc_dev_s *fuvc)
{
  uint32_t ret;

  g_f_uvc = fuvc;

  g_uvc_mutex = 0;
  ret = LOS_MuxCreate(&g_uvc_mutex);
  if (ret != LOS_OK)
    {
      usb_err("Failed to create Mutex for UVC: %#x\n", ret);
      return -1;
    }

  fuvc_fill_streaming_control(fuvc, &fuvc->probe, 0, 0);
  fuvc_fill_streaming_control(fuvc, &fuvc->commit, 0, 0);

  return 0;
}

#define GET_BYTE(data, bits)     (((data) >> (bits)) & 0xFF)
void usb_format_yuv_semiplanar_420(char *buf_virt_y, char *buf_virt_c, uint8_t *frame_mem,
                                   uint32_t frame_len, uint32_t frame_height, uint32_t frame_width)
{
  uint32_t height = frame_height;
  uint32_t width  = frame_width;
  char *temp_buf_virt_y = buf_virt_y;
  char *temp_buf_virt_c = buf_virt_c;
  char *py, *end;
  uint32_t *pyy, *pc;
  uint32_t *p, *pp;
  register uint32_t r1, r2, rr1, rr2;
  register uint32_t c[2];
  uint32_t i;
  uint32_t reg;

  reg   = GET_UINT32(0x12020ee0);
  reg >>= 24;
  if (reg == 0x4 || reg == 0 || reg == 0x10)
    {
      for(i = 0; i < height / 2; i++)
        {
          py  = temp_buf_virt_y + (i * 2) * width;
          pyy = (uint32_t *)(py + width);
          end = (char *)pyy;
          pc  = (uint32_t *)(temp_buf_virt_c + i * width);
          p   = (uint32_t *)(frame_mem + frame_len + (i * 4) * width);
          pp  = (uint32_t *)(frame_mem + frame_len + (i * 4 + 2) * width);
          for(; py < end; py += 8)
            {
              c[0] = *(pc++);
              c[1] = *(pc++);

              r1 = *((uint32_t *)py);
              r2 = *((uint32_t *)py + 1);
              *(p++) = ((GET_BYTE(r1, 0)) | (GET_BYTE(c[0], 8) << 8) |
                        (GET_BYTE(r1, 8) << 16) | (GET_BYTE(c[0], 0) << 24));
              *(p++) = ((GET_BYTE(r1, 16)) | (GET_BYTE(c[0], 24) << 8) |
                        (GET_BYTE(r1, 24) << 16) | (GET_BYTE(c[0], 16) << 24));
              *(p++) = ((GET_BYTE(r2, 0)) | (GET_BYTE(c[1], 8) << 8) |
                        (GET_BYTE(r2, 8) << 16) | (GET_BYTE(c[1], 0) << 24));
              *(p++) = ((GET_BYTE(r2, 16)) | (GET_BYTE(c[1], 24) << 8) |
                        (GET_BYTE(r2, 24) << 16) | (GET_BYTE(c[1], 16) << 24));

              rr1 = *(pyy++);
              rr2 = *(pyy++);
              *(pp++) = ((GET_BYTE(rr1, 0)) | (GET_BYTE(c[0], 8) << 8) |
                         (GET_BYTE(rr1, 8) << 16) | (GET_BYTE(c[0], 0) << 24));
              *(pp++) = ((GET_BYTE(rr1, 16)) | (GET_BYTE(c[0], 24) << 8) |
                         (GET_BYTE(rr1, 24) << 16) | (GET_BYTE(c[0], 16) << 24));
              *(pp++) = ((GET_BYTE(rr2, 0)) | (GET_BYTE(c[1], 8) << 8) |
                         (GET_BYTE(rr2, 8) << 16) | (GET_BYTE(c[1], 0) << 24));
              *(pp++) = ((GET_BYTE(rr2, 16)) | (GET_BYTE(c[1], 24) << 8) |
                         (GET_BYTE(rr2, 24) << 16) | (GET_BYTE(c[1], 16) << 24));
            }
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */