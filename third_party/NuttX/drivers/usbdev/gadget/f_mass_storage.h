/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2019. All rights reserved.
 * Description: LiteOS USB Driver Mass Storage Protocol HeadFile
 * Author: huangjieliang
 * Create: 2015-07-30
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

#ifndef _F_MASS_STORAGE_H
#define _F_MASS_STORAGE_H

#include <fs/fs.h>
#include <disk.h>
#include "gadget/composite.h"
#include "implementation/freebsd_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Command Block Wrapper */

typedef struct bulk_cbw
{
  uint32_t   dCBWSignature;          /* Command Block Wrapper Signature */
  uint32_t   dCBWTag;                /* Command Block Tag */
  uint32_t   dCBWDataTransferLength; /* Data Transfer Length */
  uint8_t    bmCBWFlags;
  uint8_t    bCBWLUN;                /* Logical Unit Number */
  uint8_t    bCBWLEN;                /* Command Block Length */
  uint8_t    CDB[16];
} bulk_cbw_t;

/* Command Status Wrapper */

typedef struct bulk_csw
{
  uint32_t   dCSWSignature;     /* Command Status Wrapper Signature */
  uint32_t   dCSWTag;           /* Command Status Tag */
  uint32_t   dCSWResidue;
  uint8_t    bCSWStatus;
} bulk_csw_t;

#define BULK_CBW_WRAP_LEN   31
#define BULK_CBW_SIGN       0x43425355
#define BULK_CBW_FLAG_IN    (1 << 7)
#define BULK_CBW_FLAG_OUT   0

#define BULK_CSW_WRAP_LEN    13
#define BULK_CSW_SIGN        0x53425355
#define BULK_CSW_STAT_OK     0
#define BULK_CSW_STAT_FAIL   1
#define BULK_CSW_STAT_PHASE  2

/* Bulk-only class specific requests */

#define USB_BULK_RESET_REQUEST   0xff
#define USB_BULK_GET_MAX_LUN     0xfe

#define SCSI_TEST_UNIT_READY        0x00
#define SCSI_REQUEST_SENSE          0x03
#define SCSI_FORMAT_UNIT            0x04
#define SCSI_INQUIRY                0x12
#define SCSI_MODE_SELECT_6          0x15
#define SCSI_MODE_SELECT_10         0x55
#define SCSI_MODE_SENSE_6           0x1a
#define SCSI_MODE_SENSE_10          0x5a
#define SCSI_ALLOW_MEDIUM_REMOVAL   0x1e
#define SCSI_READ_FORMAT_CAPACITIES 0x23
#define SCSI_READ_CAPACITY          0x25
#define SCSI_READ_6                 0x08
#define SCSI_READ_10                0x28
#define SCSI_READ_12                0xa8
#define SCSI_WRITE_6                0x0a
#define SCSI_WRITE_10               0x2a
#define SCSI_WRITE_12               0xaa
#define START_STOP                  0x1b

/* SCSI Sense Key/Additional Sense Code/ASC Qualifier values */

#define SCSI_NO_SENSE                           0
#define SCSI_COMMUNICATION_FAILURE              0x040800
#define SCSI_INVALID_COMMAND                    0x052000
#define SCSI_INVALID_FIELD_IN_CDB               0x052400
#define SCSI_INVALID_COM                        0x052600
#define SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x052100
#define SCSI_LOGICAL_UNIT_NOT_SUPPORTED         0x052500
#define SCSI_MEDIUM_NOT_PRESENT                 0x023a00
#define SCSI_MEDIUM_REMOVAL_PREVENTED           0x055302
#define SCSI_UNIT_ATTENTION                     0x060000
#define SCSI_NOT_READY_TO_READY_TRANSITION      0x062800
#define SCSI_RESET_OCCURRED                     0x062900
#define SCSI_SAVING_PARAMETERS_NOT_SUPPORTED    0x053900
#define SCSI_UNRECOVERED_READ_ERROR             0x031100
#define SCSI_WRITE_ERROR                        0x030c02
#define SCSI_WRITE_PROTECTED                    0x072700

#define SK(x)         ((uint8_t) ((x) >> 16))    /* Sense Key byte, etc. */
#define ASC(x)        ((uint8_t) ((x) >> 8))
#define ASCQ(x)       ((uint8_t) (x))

#define FMASS_MAX_LUNS    8

typedef enum data_direct_t
{
  DATA_DIRECT_UNKNOWN = 0,
  DATA_DIRECT_FROM_HOST,
  DATA_DIRECT_TO_HOST,
  DATA_DIRECT_NONE
} data_direct;

typedef enum fmass_task_state_t
{
  FMASS_TASK_IDLE               = 0x0,
  FMASS_TASK_DISCONNECT         = 0x01,
  FMASS_TASK_CONFIG_CHANGE      = 0x02,
  FMASS_TASK_REQ_COMMAND_PHASE  = 0x04,
  FMASS_TASK_DONE_COMMAND_PHASE = 0x08,
  FMASS_TASK_REQ_DATA_PHASE     = 0x10,
  FMASS_TASK_DONE_DATA_PHASE    = 0x20,
  FMASS_TASK_REQ_STATUS_PHASE   = 0x40,
  FMASS_TASK_DONE_STATUS_PHASE  = 0x80,
  FMASS_TASK_REPORT_USB_STATUS  = 0x100
} fmass_task_state;

/* Length of a SCSI Command Data Block */

#define MAX_COMMAND_SIZE       16

#define MAX_BLOCK_RW_SECTORS   256
#define MAX_DATA_BUFFER_NUM    2
#define MAX_FILE_STORAGE_LUNS  32
#define MAX_NOFIFY_NUM         10
typedef void (*fmass_notify_cb)(void *context, int status);

struct fmass_notify
{
  fmass_notify_cb notifycb;
  void *notifydata;
  int is_used;
};

enum fmass_data_buf_state
{
  DBUF_STATE_EMPTY = 0,
  DBUF_STATE_FULL,
  DBUF_STATE_BUSY
};

struct fmass_data_buf_t
{
  void *buf;
  uint32_t filledbit;
  struct fmass_data_buf_t *next;
  enum fmass_data_buf_state state;
};

struct fmass_capacity
{
  size_t nsectors;     /* Number of sectors on the device */
  size_t sectorsize;   /* Size of one sector */
  int read_only;
};

struct mass_dev_s
{
  data_direct data_dir;
  uint32_t    data_size;
  uint32_t    data_size_from_cmd;
  uint32_t    tag;
  uint32_t    residue;
  uint32_t    usb_amount_left;


  int     cmd_size;
  uint8_t cmd[MAX_COMMAND_SIZE];

  uint32_t    sense_data;
  uint32_t    sense_data_info;
  uint32_t    unit_attention_data;
  uint32_t    info_valid;

  uint8_t bulk_in_enabled;
  uint8_t bulk_out_enabled;

  struct bulk_csw csw;

  /*
   * Vendor (8 chars), product (16 chars), release (4
   * hexadecimal digits) and NULL byte
   */

  char inquiry_str[8 + 16 + 4 + 1];

  uint32_t    nluns;
  uint32_t    lun;

#define GET_SECTOR_COUNT  1 /* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE   2 /* Get sector size (needed at _MAX_SS != _MIN_SS) */

  los_part *parts[MAX_FILE_STORAGE_LUNS];
  FAR struct inode *fileNode[MAX_FILE_STORAGE_LUNS];
  struct fmass_capacity caps[MAX_FILE_STORAGE_LUNS];
  struct fmass_data_buf_t *databuf_fill;
  struct fmass_data_buf_t databuf[MAX_DATA_BUFFER_NUM];
  uint32_t fmass_db_bitmap;

#define FMASS_DATA_PROC     0x01
#define FMASS_NEED_EXIT     0x02
#define FMASS_THREAD_EXITED 0x04
  EVENT_CB_S task_event;

  struct mtx task_mtx;
  wait_queue_head_t xfer_wait;
  uint32_t task_state;
  spinlock_t lock;

  struct fmass_notify *notify;
  int dev_status;    /* 0: usb device disconnect , 1 : usb device connectted */
#define DEV_ST_DISCONNECT 0
#define DEV_ST_CONNECTTED 1

  struct usbdev_req_s ctrlreq;
  struct usbdev_req_s bulkreq;
  struct usbdev_ep_s *bulkout;
  struct usbdev_ep_s *bulkin;
};

struct mass_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct mass_dev_s *dev;
};

struct mass_softc
{
  struct mass_dev_s    dev;
  struct mass_driver_s drvr;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif