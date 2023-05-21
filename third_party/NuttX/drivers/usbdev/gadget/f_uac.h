/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver UAC Protocol HeadFile
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

#ifndef _F_UAC_GADGET_H
#define _F_UAC_GADGET_H

#include "gadget/usbdev.h"
#include "gadget/composite.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct uac_dev_s
{
  uint8_t            *descs;   /* pointer to the concatenated descriptors */
  uint32_t            desl;    /* length of all descriptors */
  uint32_t            control; /* Class-specific control request value */
  struct usbdev_ep_s *out_ep;  /* control transfer endpoint */
  struct usbdev_ep_s *in_ep;   /* Audio Streaming endpoint */
  bool out_ep_enabled;
  bool in_ep_enabled;

  struct usbdev_req_s ctrlreq;   /* control request */
  struct usbdev_req_s outputreq; /* Audio Control request */
  struct usbdev_req_s inputreq;  /* Audio Streaming request */
  uint32_t            dyn_fc;    /* dynamic format change, non-zero indicates that format change occurred */

  volatile int connected;        /* Connected to USB host ? */
  int          count;
  uint32_t     fid;              /* Frame Indentifier */

  struct list_head       uac_queue;
  int                    uac_queue_len;
  volatile atomic_t      busy_flag;
  struct uac_queue_node *cur_node;
  spinlock_t             lock;
};

struct uac_driver_s
{
  struct usbdevclass_driver_s  drvr;
  struct uac_dev_s            *dev;
};

/* private UAC device driver instance structure */

struct uac_softc
{
  struct uac_dev_s    dev;
  struct uac_driver_s drvr;
};

struct uac_ac_header_descriptor
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;

  uint16_t bcdADC;
  uint16_t wTotalLength;

  uint8_t  bInCollection;
  uint8_t  baInterfaceNr[2];
} __attribute__((packed));

struct uac1_input_terminal_descriptor
{
  uint8_t  bLength;            /* in bytes: 12 */
  uint8_t  bDescriptorType;    /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype; /* INPUT_TERMINAL descriptor subtype */
  uint8_t  bTerminalID;        /* Constant uniquely terminal ID */
  uint16_t wTerminalType;      /* USB Audio Terminal Types */
  uint8_t  bAssocTerminal;     /* ID of the Output Terminal associated */
  uint8_t  bNrChannels;        /* Number of logical output channels */
  uint16_t wChannelConfig;
  uint8_t  iChannelNames;
  uint8_t  iTerminal;
} __attribute__ ((packed));

#define UAC_DT_INPUT_TERMINAL_SIZE     12
#define UAC_DT_OUTPUT_TERMINAL_SIZE    9
#define UAC_INPUT_TERMINAL_MICROPHONE  0x201

struct uac_output_terminal_descriptor
{
  uint8_t  bLength;            /* in bytes: 9 */
  uint8_t  bDescriptorType;    /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype; /* OUTPUT_TERMINAL descriptor subtype */
  uint8_t  bTerminalID;        /* Constant uniquely terminal ID */
  uint16_t wTerminalType;      /* USB Audio Terminal Types */
  uint8_t  bAssocTerminal;     /* ID of the Input Terminal associated */
  uint8_t  bSourceID;          /* ID of the connected Unit or Terminal*/
  uint8_t  iTerminal;
} __attribute__ ((packed));

/* 4.5.2 Class-Specific AS Interface Descriptor */

struct uac_as_header_descriptor
{
  uint8_t  bLength;                   /* in bytes: 7 */
  uint8_t  bDescriptorType;           /* USB_DT_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;        /* AS_GENERAL */
  uint8_t  bTerminalLink;             /* Terminal ID of connected Terminal */
  uint8_t  bDelay;                    /* Delay introduced by the data path */
  uint16_t wFormatTag;                /* The Audio Data Format */
} __attribute__((packed));

/* Audio Class-Specific AS Interface Descriptor Subtypes */

#define UAC_AS_GENERAL                     0x01
#define UAC_FORMAT_TYPE                    0x02
#define UAC_FORMAT_SPECIFIC                0x03

struct uac_format_type_i_discrete_descriptor
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bFormatType;
  uint8_t  bNrChannels;
  uint8_t  bSubframeSize;
  uint8_t  bBitResolution;
  uint8_t  bSamFreqType;
  uint8_t  tSamFreq[4][3];
} __attribute__ ((packed));

struct uac_endpoint_descriptor
{
  uByte    bLength;
  uByte    bDescriptorType;
  uByte    bEndpointAddress;
  uByte    bmAttributes;
  uWord    wMaxPacketSize;
  uByte    bInterval;
  uByte    bRefresh;
  uByte    bSynchAddress;
} __attribute__((packed));

struct uac2_iso_endpoint_descriptor
{
  uint8_t  bLength;            /* in bytes: 8 */
  uint8_t  bDescriptorType;    /* USB_DT_CS_ENDPOINT */
  uint8_t  bDescriptorSubtype; /* EP_GENERAL */
  uint8_t  bmAttributes;
  uint8_t  bmControls;
  uint8_t  bLockDelayUnits;
  uint16_t wLockDelay;
} __attribute__((packed));

/* 4.3.2.5 Feature Unit Descriptor */

struct uac_feature_unit_descriptor
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bUnitID;
  uint8_t  bSourceID;
  uint8_t  bControlSize;
  uint16_t bmaControls[1]; /* variable length */
  uint8_t  iFeature;
} __attribute__((packed));

/*
 * UAC Audio Class-specific Descriptor Types, UAC spec. 1.0
 */

#define UAC_CS_UNDEFINED            0x20
#define UAC_CS_DEVICE               0x21
#define UAC_CS_CONFIGURATION        0x22
#define UAC_CS_STRING               0x23
#define UAC_CS_INTERFACE            0x24
#define UAC_CS_ENDPOINT             0x25

/*
 * VideoStreaming Interface Control Selectors, UAC spec. 1.0
 */

#define UAC_VS_CONTROL_UNDEFINED                   0x00
#define UAC_VS_PROBE_CONTROL                       0x01
#define UAC_VS_COMMIT_CONTROL                      0x02
#define UAC_VS_STILL_PROBE_CONTROL                 0x03
#define UAC_VS_STILL_COMMIT_CONTROL                0x04
#define UAC_VS_STILL_IMAGE_TRIGGER_CONTROL         0x05
#define UAC_VS_STREAM_ERROR_CODE_CONTROL           0x06
#define UAC_VS_GENERATE_KEY_FRAME_CONTROL          0x07
#define UAC_VS_UPDATE_FRAME_SEGMENT_CONTROL        0x08
#define UAC_VS_SYNCH_DELAY_CONTROL                 0x09

#define UAC_SETCUR_COMPLETE 0xfe

#define USB_ENDPOINT_XFER_SYNC        4

/* Audio Class-Specific AC Interface Descriptor Subtypes */

#define UAC_HEADER                      0x01
#define UAC_INPUT_TERMINAL              0x02
#define UAC_OUTPUT_TERMINAL             0x03
#define UAC_MIXER_UNIT                  0x04
#define UAC_SELECTOR_UNIT               0x05
#define UAC_FEATURE_UNIT                0x06
#define UAC1_PROCESSING_UNIT            0x07
#define UAC1_EXTENSION_UNIT             0x08

/* Audio Class-Specific Request Codes */

#define SET_CUR_UAC                     0x1
#define GET_CUR_UAC                     0x81
#define SET_MIN_UAC                     0x2
#define GET_MIN_UAC                     0x82
#define SET_MAX_UAC                     0x3
#define GET_MAX_UAC                     0x83
#define SET_RES_UAC                     0x4
#define GET_RES_UAC                     0x84
#define SET_MEM_UAC                     0x5
#define GET_MEM_UAC                     0x85

struct uac_opts
{
  int c_chmask; /* Capture channel mask. */
  int c_srate;  /* Capture default sample rate. */
  int c_ssize;  /* Capture default sample size. */
  int p_chmask; /* Playback channel mask. */
  int p_srate;  /* Playback default sample rate. */
  int p_ssize;  /* Playback default sample size. */
};

#define UAC_CS_INTERFACE                0x24

#define HSETM(_var, _val) _var = { (uint8_t)(_val), (uint8_t)(_val >> 0x8), (uint8_t)(_val >> 16) }

extern void usbdev_uac_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);
extern struct uac_endpoint_descriptor g_fuac_as_in_ep_desc;
extern size_t g_uac_iso_data_size;

extern int fuac_opts_set(struct uac_opts *opts);
extern uint32_t fuac_rate_get(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif