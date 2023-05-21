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
#include "usbd_config.h"
#include "implementation/global_implementation.h"
#include "usb_string.h"

#ifdef __cplusplus
#if __cplusplus
//extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int utf8_to_utf16le(const char *src, uint8_t *target, uint32_t len)
{
  int count = 0;
  uint8_t c;
  uint16_t wc;

  while (len != 0 && (c = (uint8_t)*src++) != 0)
    {
      if ((c & 0x80) == 0)
        { /* 0xxxxxxx => 000000000xxxxxxx */
          wc = c;
        }
      else if ((c & 0xe0) == 0xc0)
        { /* 110yyyyy 10xxxxxx => 00000yyyyyxxxxxx */
          wc = (c & 0x1f) << 6;

          c = (uint8_t)*src++;
          if ((c & 0xc0) != 0x80)
            goto err;
          c &= 0x3f;
          wc |= c;
        }
      else if ((c & 0xf0) == 0xe0)
        { /* 1110zzzz 10yyyyyy 10xxxxxx => zzzzyyyyyyxxxxxx */
          wc = (c & 0x0f) << 12;

          c = (uint8_t)*src++;
          if ((c & 0xc0) != 0x80)
            goto err;
          c &= 0x3f;
          wc |= c << 6;

          c = (uint8_t)*src++;
          if ((c & 0xc0) != 0x80)
            goto err;
          c &= 0x3f;
          wc |= c;

          /* don't support surrogates */
          if (0xd800 <= wc && wc <= 0xdfff)
            {
              goto err;
            }
        }
      else
        { /* 4 bytes sequence is not supported */
          goto err;
        }

      *target++ = (uint8_t)wc;
      *target++ = (uint8_t)(wc >> 8);
      count++;
      len--;
    }
  return count;

err:
  return -1;
}

#ifdef __cplusplus
#if __cplusplus
//}
#endif /* __cplusplus */
#endif /* __cplusplus */
