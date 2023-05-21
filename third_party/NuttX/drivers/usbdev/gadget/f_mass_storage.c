/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2019. All rights reserved.
 * Description: LiteOS USB Driver Mass Storage Protocol
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

#include <linux/wait.h>
#include "gadget/f_mass_storage.h"
#include "implementation/global_implementation.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#undef USB_DEBUG_VAR
#define USB_DEBUG_VAR   g_fmass_debug
#ifdef LOSCFG_USB_DEBUG
static int g_fmass_debug = 0;

void fmass_debug_func(int level)
{
  g_fmass_debug = level;
  PRINTK("The level of usb fmass debug is %d\n", level);
}
DEBUG_MODULE(fmass, fmass_debug_func);
#endif

extern void __init_waitqueue_head(wait_queue_head_t *wait);
extern void __wake_up_interruptible(wait_queue_head_t *wait);

int usbdev_mass_initialize(struct module *mod, int n, void *arg);

static devclass_t g_fmass_devclass;
static struct mass_softc *g_fmass = NULL;

static const driver_t g_fmass_driver =
{
  .name    = "fmass",
  .methods = NULL,
  .size    = sizeof(struct mass_softc),
};

DRIVER_MODULE(fmass, simple, g_fmass_driver, g_fmass_devclass, usbdev_mass_initialize, 0);

static int usbclass_mass_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_mass_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_mass_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_mass_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_mass_driverops =
{
  usbclass_mass_bind,
  usbclass_mass_unbind,
  usbclass_mass_setup,
  usbclass_mass_disconnect,
  NULL,
  NULL
};

static int fmass_thread_init(struct mass_dev_s *fmass, struct usbdev_s *dev);
static void fmass_main_thread_signal(struct mass_dev_s *fmass, fmass_task_state state);
static void fmass_task_change_state(struct mass_dev_s *fmass, fmass_task_state new_state);

static int fmass_dev_open(struct mass_dev_s *fmass);
static int fmass_dev_close(struct mass_dev_s *fmass);
static void fmass_dev_capacity(struct mass_dev_s *fmass);

static size_t fmass_dev_read(struct mass_dev_s *fmass,
                             uint8_t *buffer,
                             size_t stsector,
                             uint32_t nsectors);

static size_t fmass_dev_write(struct mass_dev_s *fmass,
                              uint8_t *buffer,
                              size_t stsector,
                              uint32_t nsectors);

static void fmass_report_usb_status(struct mass_dev_s *fmass);
static void fmass_notify_report(struct mass_dev_s *fmass, int status);

#define fmass_wait_intr(fm, cond, tm)    \
  wait_event_interruptible_timeout((((struct mass_dev_s *)fm)->xfer_wait), cond, tm)

#define fmass_wakeup(fm)    \
  wake_up_interruptible(&(((struct mass_dev_s *)fm)->xfer_wait))

static const char *g_mass_device_type[] =
{
  "/dev/mmcblk0p",
  "/dev/mmcblk1p",
  "/dev/uramp",
};

struct fmass_notify_cb
{
  struct fmass_notify notify[MAX_NOFIFY_NUM];
  struct mtx notify_mtx;
  bool init;
} g_notify = { .notify_mtx = PTHREAD_MUTEX_INITIALIZER, .init = false };

static inline void fmass_notify_init(void)
{
  mtx_lock(&g_notify.notify_mtx);
  if (g_notify.init == false)
    {
      g_notify.init = true;
      (void)memset_s(g_notify.notify, sizeof(struct fmass_notify) * MAX_NOFIFY_NUM,
                     0, sizeof(struct fmass_notify) * MAX_NOFIFY_NUM);
    }
  mtx_unlock(&g_notify.notify_mtx);
}

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define FMASS_MAX_PACKET_SIZE   0x0400
#else
#define FMASS_MAX_PACKET_SIZE   0x0200
#endif

#define DEIVICE_VENDOR_ID     0x0525
#define DEIVICE_PRODUCT_ID    0xa4a5
#define DEIVICE_VERSION       0x0100

static const struct usb_device_descriptor g_fmass_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,
  HSETW(.bcdUSB, UD_BCD_USB), /* USB version 0x0200 */
  .bDeviceClass       = UDCLASS_IN_INTERFACE,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize     = UD_USB_MPS,
  HSETW(.idVendor, DEIVICE_VENDOR_ID),   /* vendor */
  HSETW(.idProduct, DEIVICE_PRODUCT_ID), /* product */
  HSETW(.bcdDevice, 0x0318),             /* device version */
  .iManufacturer      = 0,
  .iProduct           = 0,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,
};

struct fmass_config_desc
{
  struct usb_config_descriptor confd;
  struct usb_interface_descriptor ifcd;
  struct usb_endpoint_descriptor iepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_endpoint_ss_comp_descriptor icompd;
#endif
  struct usb_endpoint_descriptor oepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_endpoint_ss_comp_descriptor ocompd;
#endif
} __packed;

static struct fmass_config_desc g_fmass_confd =
{
  .confd =
    {
      .bLength             = sizeof(struct usb_config_descriptor),
      .bDescriptorType     = UDESC_CONFIG,
      HSETW(.wTotalLength, sizeof(g_fmass_confd)),
      .bNumInterface       = 1,
      .bConfigurationValue = 1,
      .iConfiguration      = 0,
      .bmAttributes        = UC_SELF_POWERED | UC_BUS_POWERED,
      .bMaxPower           = 1 /* max power */
    },
  .ifcd =
    {
      .bLength             = sizeof(struct usb_interface_descriptor),
      .bDescriptorType     = UDESC_INTERFACE,
      .bInterfaceNumber    = 0,
      .bAlternateSetting   = 0,
      .bNumEndpoints       = 2,
      .bInterfaceClass     = UICLASS_MASS,
      .bInterfaceSubClass  = UISUBCLASS_SCSI,
      .bInterfaceProtocol  = UIPROTO_MASS_BULK,
      .iInterface          = 1
    },
  .iepd =
    {
      .bLength             = sizeof(struct usb_endpoint_descriptor),
      .bDescriptorType     = UDESC_ENDPOINT,
      .bEndpointAddress    = UE_DIR_IN,
      .bmAttributes        = UE_BULK,
      HSETW(.wMaxPacketSize, FMASS_MAX_PACKET_SIZE),
      .bInterval           = 0
    },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  .icompd =
    {
      .bLength             = 6,
      .bDescriptorType     = 0x30,
      .bMaxBurst           = 0xf,
      .bmAttributes        = 0,
      .wBytesPerInterval   = {0}
    },
#endif
  .oepd =
    {
      .bLength             = sizeof(struct usb_endpoint_descriptor),
      .bDescriptorType     = UDESC_ENDPOINT,
      .bEndpointAddress    = UE_DIR_OUT,
      .bmAttributes        = UE_BULK,
      HSETW(.wMaxPacketSize, FMASS_MAX_PACKET_SIZE),
      .bInterval           = 0
    },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  .ocompd =
    {
      .bLength             = 6,
      .bDescriptorType     = 0x30,
      .bMaxBurst           = 0xf,
      .bmAttributes        = 0,
      .wBytesPerInterval   = {0}
    },
#endif
};

#define DT_STRING_ID_LEN   4
static const char g_dt_string_id[DT_STRING_ID_LEN] =
{
  DT_STRING_ID_LEN,
  UDESC_STRING,
  0x09, 0x04,
};

#define DT_STRING_VID_LEN   16
static const char g_dt_string_vid[DT_STRING_VID_LEN] =
{
  DT_STRING_VID_LEN,
  UDESC_STRING,
  'D', 0, 'W', 0, 'C', 0, '-', 0, 'O', 0, 'T', 0, 'G', 0
};

#define DT_STRING_PID_LEN   16
static const char g_dt_string_pid[DT_STRING_PID_LEN] =
{
  DT_STRING_PID_LEN,
  UDESC_STRING,
  'M', 0, 8, 0, '-', 0, 'C', 0, 'H', 0, 'I', 0, 'P', 0

};

#define DT_STRING_SERIAL_LEN   18
static const char g_dt_string_serial[DT_STRING_SERIAL_LEN] =
{
  DT_STRING_SERIAL_LEN,
  UDESC_STRING,
  '2', 0, '0', 0, '1', 0, '5', 0, '0', 0, '7', 0, '3', 0, '0', 0
};

static char g_dt_string_buf[40] =
{
  0x28, 3, 0x4d, 0, 0x61, 0, 0x73, 0, 0x73, 0,
  0x20, 0, 0x53, 0, 0x74, 0, 0x6f, 0, 0x72, 0,
  0x61, 0, 0x67, 0, 0x65, 0, 0x20, 0, 0x47, 0,
  0x61, 0, 0x64, 0, 0x67, 0, 0x65, 0, 0x74, 0
};

inline static bool taskstate_match(struct mass_dev_s *fmass, fmass_task_state tarstate)
{
  return fmass->task_state & tarstate;
}

inline static bool taskstate_unmatch(struct mass_dev_s *fmass, fmass_task_state tarstate)
{
  return (!(fmass->task_state & tarstate));
}

static int fmass_device_status(struct mass_dev_s *fmass)
{
  return fmass->dev_status;
}

static void fmass_handle_bulkin_callback(struct usbdev_ep_s *ep,
                                         struct usbdev_req_s *req)
{
  struct mass_dev_s *fmass = (struct mass_dev_s *)(ep->priv);
  if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_DATA_PHASE);
    }
  else if (taskstate_match(fmass, FMASS_TASK_REQ_STATUS_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_STATUS_PHASE);
    }
  fmass_wakeup(fmass);
}

static void fmass_handle_bulkout_callback(struct usbdev_ep_s *ep,
                                          struct usbdev_req_s *req)
{
  struct mass_dev_s *fmass = (struct mass_dev_s *)(ep->priv);
  if (taskstate_match(fmass, FMASS_TASK_REQ_COMMAND_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_COMMAND_PHASE);
    }
  else if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_DATA_PHASE);
    }
  fmass_wakeup(fmass);
}

static int fmass_bulkin_request(struct mass_dev_s *fmass,
                                struct usbdev_req_s *req)
{
  struct usbdev_ep_s *ep = fmass->bulkin;
  req->callback = fmass_handle_bulkin_callback;
  return EP_SUBMIT(ep, req);
}

static int fmass_bulkout_request(struct mass_dev_s *fmass,
                                 struct usbdev_req_s *req)
{
  struct usbdev_ep_s *ep = fmass->bulkout;
  req->callback = fmass_handle_bulkout_callback;
  return EP_SUBMIT(ep, req);
}

static void report_scsi_command_error(struct mass_dev_s *fmass, uint32_t sense_data)
{
  fmass->sense_data      = sense_data;
  fmass->sense_data_info = 0;
  fmass->info_valid      = 1;
  fmass->csw.bCSWStatus  = BULK_CSW_STAT_FAIL;
}

int done_scsi_test_unit_ready(struct mass_dev_s *fmass)
{
  if (fmass_device_status(fmass))
    {
      if (fmass->nluns == 0)
        {
          (void)fmass_dev_open(fmass);
          fmass_dev_capacity(fmass);
          fmass->sense_data = SCSI_NOT_READY_TO_READY_TRANSITION;
          fmass->csw.bCSWStatus = BULK_CSW_STAT_FAIL;
          DPRINTFN(1, "this unint is not ready\n");
        }
    }
  else
    {
      fmass->sense_data = SCSI_MEDIUM_NOT_PRESENT;
      fmass->csw.bCSWStatus = BULK_CSW_STAT_FAIL;
    }
  return 0;
}

int done_scsi_request_sense(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  uint32_t sd, sdinfo;
  uint32_t valid;
  int ret;

  ret = memset_s(buf, (MAX_BLOCK_RW_SECTORS << 9), 0, 18);
  if (ret != EOK)
    {
      return 0;
    }

  if (!fmass->nluns && fmass_device_status(fmass)) /* Unsupported LUNs are okay */
    {
      sd     = SCSI_LOGICAL_UNIT_NOT_SUPPORTED;
      sdinfo = 0;
      valid  = 0;
    }
  else
    {
      sd     = fmass->sense_data;
      sdinfo = fmass->sense_data_info;
      valid  = fmass->info_valid << 7;
      fmass->sense_data      = SCSI_NO_SENSE;
      fmass->sense_data_info = 0;
      fmass->info_valid      = 0;
    }

  buf[0] = valid | 0x70; /* Valid, current error */
  buf[2] = SK(sd);
  put_unaligned_be32((&buf[3]), sdinfo); /* Sense information */
  buf[7]  = 10;  /* Additional sense length: 18 - 8 */
  buf[12] = ASC(sd);
  buf[13] = ASCQ(sd);
  return 18;
}

int done_scsi_mode_select(struct mass_dev_s *fmass)
{
  (void)fmass;
  return 0;
}

int done_scsi_mode_sense(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  int len;

  buf[0] = 0x03;  /* mode data length */
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[3] = 0x00;
  len    = 4;
  fmass->residue = len;
  return len;
}

int done_scsi_read_format_capacities(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;

  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 8; /* Only the Current/Maximum Capacity Descriptor */
  buf   += 4;

  put_unaligned_be32((&buf[0]), fmass->caps[fmass->lun].nsectors);
  put_unaligned_be32((&buf[4]), fmass->caps[fmass->lun].sectorsize);    /* Block length */
  buf[4] = 0x02; /* Current capacity */

  return 12;
}

int done_scsi_read_capacity(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  uint32_t sectors;
  uint32_t blksizes;

  if (!fmass->nluns)
    {
      (void)fmass_dev_open(fmass);
      fmass_dev_capacity(fmass);
      fmass->sense_data = SCSI_NOT_READY_TO_READY_TRANSITION;
      fmass->csw.bCSWStatus = BULK_CSW_STAT_FAIL;
      DPRINTFN(0, "fmass->fileNode is not exist\n");
      return 0;
    }

  sectors  = fmass->caps[fmass->lun].nsectors - 1;
  blksizes = fmass->caps[fmass->lun].sectorsize;

  put_unaligned_be32(buf, sectors);
  put_unaligned_be32((buf + 4), blksizes);

  return 8;
}

int done_scsi_read(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  uint32_t lba;
  uint32_t amount_left;
  uint32_t amount;
  uint32_t nreadsector;
  uintptr_t num_sectors = fmass->caps[fmass->lun].nsectors;
  struct fmass_data_buf_t *db;
  uint32_t doread ;

  if (fmass->cmd[0] == SCSI_READ_6)
    {
      lba = ((((uint32_t)fmass->cmd[1]) << 16) |
             get_unaligned_be16(&fmass->cmd[2]));
    }
  else
    {
      lba = get_unaligned_be32(&fmass->cmd[2]);

      /*
       * We allow DPO (Disable Page Out = don't save data in the
       * cache) and FUA (Force Unit Access = write directly to the
       * medium).  We don't implement DPO; we implement FUA by
       * performing synchronous output.
       */

      if ((fmass->cmd[1] & ~0x18) != 0)
        {
          report_scsi_command_error(fmass, SCSI_INVALID_FIELD_IN_CDB);
          return -EIO;
        }
    }

  if (lba >= num_sectors)
    {
      report_scsi_command_error(fmass, SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
      return -1;
    }

  if (unlikely(fmass->data_size_from_cmd == 0))
    {
      report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
      return -EIO;
    }

  nreadsector = fmass->data_size_from_cmd;

  amount_left = nreadsector;

  fmass->bulkreq.is_complete = 1;

  db = fmass->databuf_fill;

  for (; ;)
    {
      uint32_t data_size;

      amount = min(MAX_BLOCK_RW_SECTORS, amount_left);
      db = fmass->databuf_fill;
      if (!fmass_wait_intr(fmass, ((fmass->bulkreq.is_complete != 0) ||
          taskstate_match(fmass, FMASS_TASK_DISCONNECT)), (2 * HZ)))
        {
          fmass->bulkreq.is_complete = 1;
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          return -EIO;
        }

      if (taskstate_match(fmass, FMASS_TASK_DISCONNECT))
        {
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          return -EIO;
        }

      buf = (uint8_t *)db->buf;
      doread = fmass_dev_read(fmass, buf, lba, amount);
      data_size = doread << 9;
      lba += doread;
      amount_left -= doread;
      fmass->residue -= data_size;    /* doread * 512 */
      fmass->bulkreq.len = data_size;
      fmass->bulkreq.buf = buf;
      DPRINTFN(1, "read: db= 0x%x buf= 0x%x lba= %x doread= %u  nread= %u  amount_left = %u   residue= %u\n",
               db, buf, lba - doread , doread, amount, amount_left, fmass->residue);

      if (doread < amount)
        {
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          break;
        }

      if (amount_left == 0)
        {
          break;
        }

      (void)fmass_bulkin_request(fmass, &fmass->bulkreq);
      fmass->databuf_fill = db->next;
    }
  fmass->bulkreq.is_complete = 1;
  return -EIO;
}

int done_scsi_write(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  uint32_t lba;
  uint32_t idx;
  uint32_t amount, amount_left;
  uint32_t nwritesector;
  uintptr_t num_sectors = fmass->caps[fmass->lun].nsectors;
  uint32_t total_do_write;
  uint32_t do_write = 0;

  if (fmass->caps[fmass->lun].read_only)
    {
      report_scsi_command_error(fmass,SCSI_WRITE_PROTECTED);
      return -EINVAL;
    }

  if (fmass->cmd[0] == SCSI_WRITE_6)
    {
      lba = ((((uint32_t)fmass->cmd[1]) << 16) |
             get_unaligned_be16(&fmass->cmd[2]));
    }
  else
    {
      lba = get_unaligned_be32(&fmass->cmd[2]);

      /*
       * We allow DPO (Disable Page Out = don't save data in the
       * cache) and FUA (Force Unit Access = write directly to the
       * medium).  We don't implement DPO; we implement FUA by
       * performing synchronous output.
       */

      if ((fmass->cmd[1] & ~0x18) != 0)
        {
          report_scsi_command_error(fmass, SCSI_INVALID_FIELD_IN_CDB);
          return -EIO;
        }
      if (fmass->cmd[1] & 0x08)
        {
          /* FUA */
          /* XXX set SYNC flag here */

        }
    }

  if (lba >= num_sectors)
    {
      report_scsi_command_error(fmass, SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
      return -1;
    }

  if (unlikely(fmass->data_size_from_cmd == 0))
    {
      return -EIO; /* No default reply */
    }

  fmass_task_change_state(fmass, FMASS_TASK_REQ_DATA_PHASE);
  DPRINTFN(1, "------ Direct Data Phase ------\n");
  fmass->bulkreq.len = fmass->data_size;
  (void)fmass_bulkout_request(fmass, &fmass->bulkreq);

  if (!fmass_wait_intr(fmass, (taskstate_match(fmass, FMASS_TASK_DONE_DATA_PHASE) ||
      taskstate_match(fmass, FMASS_TASK_DISCONNECT)), (5*HZ)))
    {
      DPRINTFN(0, "error wait intr\n");
      return -EIO;
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DONE_DATA_PHASE))
    {
      DPRINTFN(0, "invalid task state : %u, unfinish transfer Data packet\n", fmass->task_state);
      return -EIO;
    }

  nwritesector = fmass->bulkreq.xfrd >> 9;  /* bytes -> sectors */
  nwritesector = min(nwritesector, fmass->data_size_from_cmd);

  amount = nwritesector / MAX_BLOCK_RW_SECTORS;
  amount_left = nwritesector % MAX_BLOCK_RW_SECTORS;

  total_do_write = 0;
  for (idx = 0; idx < amount ; idx++)
    {
      do_write = fmass_dev_write(fmass, buf, lba, MAX_BLOCK_RW_SECTORS);

      lba += MAX_BLOCK_RW_SECTORS;
      buf += (MAX_BLOCK_RW_SECTORS << 9);
      fmass->residue -= do_write << 9; /* doread * 512 */
      total_do_write += do_write;
    }

  if (amount == 0 || do_write > 0)
    {
      do_write = fmass_dev_write(fmass, buf, lba, amount_left);

      if (do_write > 0)
        {
          fmass->residue -= do_write << 9; /* doread * 512 */
          total_do_write += do_write;
        }
    }

  if (total_do_write < nwritesector)
    {
      fmass->sense_data = SCSI_UNRECOVERED_READ_ERROR;
      fmass->csw.bCSWStatus = BULK_CSW_STAT_FAIL;
    }

  fmass->data_size = 0;
  return -EIO;
}

int done_scsi_inquiry(struct mass_dev_s *fmass)
{
  errno_t ret;
  uint8_t *buf = (uint8_t *)fmass->bulkreq.buf;
  buf[0] = 0;        /* Peripheral Qualifier && Device Type */
  buf[1] = 0x80;     /* Removable Medium */
  buf[2] = 0;        /* ANSI SCSI level 2 */
  buf[3] = 1;        /* SCSI-2 INQUIRY data format */
  buf[4] = 31;       /* Additional length */
  buf[5] = 0;        /* SCCS SCC Support: 0 */
  buf[6] = 0;
  buf[7] = 0;

  ret = memcpy_s(buf + 8, ((MAX_BLOCK_RW_SECTORS << 9) - 8), fmass->inquiry_str, sizeof(fmass->inquiry_str));
  if (ret != EOK)
    {
      return -EIO;
    }

  fmass->bulkreq.len = 36;
  fmass->residue     = 0;
  return -EIO;
}

int done_scsi_allow_medium_removal(struct mass_dev_s *fmass)
{
  fmass->sense_data = SCSI_INVALID_COM;
  fmass->csw.bCSWStatus = BULK_CSW_STAT_FAIL;
  return 0;
}

static int fmass_check_receive_cbw(struct mass_dev_s *fmass)
{
  struct bulk_cbw *cbw = (struct bulk_cbw *)fmass->bulkreq.buf;
  errno_t ret;

  DPRINTFN(1, "\t -----Received CBW packet -----dCBWTag: 0x%x  bCBWLUN:%u\n", cbw->dCBWTag, cbw->bCBWLUN);

  /* Check the CBW is validid */

  if (fmass->bulkreq.xfrd != BULK_CBW_WRAP_LEN ||
      cbw->dCBWSignature != BULK_CBW_SIGN)
    {
      DPRINTFN(0, "invalid CBW: len %u sig 0x%x ,cvt_sig 0x%x  lct_sig 0x%x\n",
               fmass->bulkreq.xfrd, cbw->dCBWSignature, cbw->dCBWSignature, BULK_CBW_SIGN);
      return -1;
    }

  /* Check the CBW is meaningful */

  if (cbw->bCBWLUN >= FMASS_MAX_LUNS || cbw->bmCBWFlags & ~BULK_CBW_FLAG_IN ||
      cbw->bCBWLEN == 0 || cbw->bCBWLEN > MAX_COMMAND_SIZE)
    {
      DPRINTFN(0, "non-meaningful CBW: lun = %u, flags = 0x%x, cmdlen = %u\n",
               cbw->bCBWLUN, cbw->bmCBWFlags, cbw->bCBWLEN);
      return -1;
    }

  /* Save the command for later */

  fmass->cmd_size = cbw->bCBWLEN;
  ret = memcpy_s(fmass->cmd, MAX_COMMAND_SIZE, cbw->CDB, fmass->cmd_size);
  if (ret != EOK)
    {
      DPRINTFN(0, "memcpy_s failed, %d\n", ret);
      return -1;
    }
  if (cbw->bmCBWFlags & BULK_CBW_FLAG_IN)
    {
      fmass->data_dir = DATA_DIRECT_TO_HOST;
    }
  else
    {
      fmass->data_dir = DATA_DIRECT_FROM_HOST;
    }
  fmass->data_size = cbw->dCBWDataTransferLength;
  if (fmass->data_size == 0)
    {
      fmass->data_dir = DATA_DIRECT_NONE;
    }

  fmass->tag = cbw->dCBWTag;

  if (fmass->data_size < fmass->data_size_from_cmd)
    {
      fmass->data_size_from_cmd = fmass->data_size;
    }
  fmass->residue = fmass->data_size;

  return 0;
}

static void fmass_do_scsi_command(struct mass_dev_s *fmass)
{
  int i;
  int reply = -1;
  uint32_t reply_temp;

  uint8_t cmd = fmass->cmd[0];

  switch (cmd)
    {
    case SCSI_TEST_UNIT_READY:
      fmass->data_size_from_cmd = 0;
      reply = done_scsi_test_unit_ready(fmass);
      break;

    case SCSI_REQUEST_SENSE:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_request_sense(fmass);
      break;

    case SCSI_INQUIRY:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_inquiry(fmass);
      break;

    case SCSI_MODE_SELECT_6:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_mode_select(fmass);
      break;

    case SCSI_MODE_SELECT_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_mode_select(fmass);
      break;

    case SCSI_MODE_SENSE_6:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_mode_sense(fmass);
      break;

    case SCSI_MODE_SENSE_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_mode_sense(fmass);
      break;

    case SCSI_READ_FORMAT_CAPACITIES:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_read_format_capacities(fmass);
      break;

    case SCSI_READ_CAPACITY:
      fmass->data_size_from_cmd = 8;
      if (fmass_device_status(fmass))
        {
          reply = done_scsi_read_capacity(fmass);
        }
      else
        {
          reply = 0;
        }
      break;

    case SCSI_READ_6:
      i = fmass->cmd[4];
      fmass->data_size_from_cmd = (i == 0) ? 256 : i;
      reply = done_scsi_read(fmass);
      break;

    case SCSI_READ_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_read(fmass);
      break;

    case SCSI_READ_12:
      fmass->data_size_from_cmd = get_unaligned_be32(&fmass->cmd[6]);
      reply = done_scsi_read(fmass);
      break;

    case SCSI_WRITE_6:
      i = fmass->cmd[4];
      fmass->data_size_from_cmd = (i == 0) ? 256 : i;
      reply = done_scsi_write(fmass);
      break;

    case SCSI_WRITE_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_write(fmass);
      break;

    case SCSI_WRITE_12:
      fmass->data_size_from_cmd = get_unaligned_be32(&fmass->cmd[6]);
      reply = done_scsi_write(fmass);
      break;

    case SCSI_ALLOW_MEDIUM_REMOVAL:
      fmass->data_size_from_cmd = 0;
      reply = done_scsi_allow_medium_removal(fmass);
      break;

    case START_STOP:
      fmass->data_size_from_cmd = 0;
      reply = 0;
      fmass->dev_status = DEV_ST_DISCONNECT;
      (void)fmass_dev_close(fmass);
      break;

    default:
      DPRINTFN(0, "********[invalid SCSI command! %x]***********\n", cmd);
      fmass->data_size_from_cmd = 0;
      report_scsi_command_error(fmass, SCSI_INVALID_COMMAND);
      break;
    }

  if (reply >= 0 && fmass->data_dir == DATA_DIRECT_TO_HOST)
    {
      fmass->bulkreq.len = reply;
      reply_temp = min((uint32_t)reply, fmass->data_size_from_cmd);
      fmass->residue -= reply_temp;
    }
 }

void fmass_handle_scsi_data(struct mass_dev_s *fmass)
{
  int is_data_phase = 0;

  DPRINTFN(1, "\t-----REQ DATA PHASE-----\n");
  switch (fmass->data_dir)
    {
    case DATA_DIRECT_NONE:
      break;    /* Nothing to send */

    case DATA_DIRECT_UNKNOWN:
      break;

    /* All but the last buffer of data must have already been sent */

    case DATA_DIRECT_TO_HOST:
      if (fmass->data_size == 0)
        {
          /* do nothing */

        }
      else if (fmass->residue == 0)
        {
          (void)fmass_bulkin_request(fmass, &fmass->bulkreq);
        }
      else
        {
          (void)fmass_bulkin_request(fmass, &fmass->bulkreq);
          if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_DATA_PHASE), (2*HZ)))
            {
              DPRINTFN(0, "--- [Data Phase] timeout! state: %u  residue : %u ---\n",
                       fmass->task_state, fmass->residue);
              if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
                {
                  fmass_task_change_state(fmass, FMASS_TASK_IDLE);
                }
              return ;
            }

          (void)EP_RESUME(fmass->bulkin);
          fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
          return ;
        }
      is_data_phase = 1;
      break;

    case DATA_DIRECT_FROM_HOST:
      break;
    }

  if (is_data_phase)
    {
      /* wait for the data tranfer done */

      if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_DATA_PHASE), (5*HZ)))
        {
          DPRINTFN(0, "--- [Data Phase] timeout! state: %u ---\n", fmass->task_state);
          if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
            {
              fmass_task_change_state(fmass, FMASS_TASK_IDLE);
            }
          return ;
        }
      if (taskstate_unmatch(fmass, FMASS_TASK_DONE_DATA_PHASE))
        {
          DPRINTFN(0, "--- [Data Phase] unmatch! unexpect state: %u ---\n", fmass->task_state);
          return ;
        }
      fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
    }
  else
    {
      fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
    }
}

void fmass_set_scsi_status(struct mass_dev_s *fmass)
{
  struct bulk_csw *csw = (struct bulk_csw *)(fmass->bulkreq.buf);
  uint8_t status       = fmass->csw.bCSWStatus;

  DPRINTFN(1, "\t -----Response CSW packet -------dCBWTag:0x%x   residue: %u   status: %u\n",
           fmass->tag, fmass->residue, status);

  /* Store and send the Bulk-only CSW */

  csw->dCSWSignature = BULK_CSW_SIGN;
  csw->dCSWTag       = fmass->tag;
  csw->dCSWResidue   = fmass->residue;
  csw->bCSWStatus    = status;

  /* submit csw packet to controller */

  fmass->bulkreq.len = BULK_CSW_WRAP_LEN;
  (void)fmass_bulkin_request(fmass, &fmass->bulkreq);

  /* wait csw packet transfer finish */

  if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_STATUS_PHASE), (5 * HZ)))
    {
      DPRINTFN(0, "--- [CSW Phase] timeout! state: %u ---\n", fmass->task_state);
      if (taskstate_match(fmass, FMASS_TASK_REQ_STATUS_PHASE))
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
        }
      return ;
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DONE_STATUS_PHASE))
    {
      DPRINTFN(0, "--- [CSW Phase] unmatch! unexpect state: %u ---\n", fmass->task_state);
      return ;
    }

  DPRINTFN(1, "\t -----CSW Transfer Finish -----\n");
  fmass_task_change_state(fmass, FMASS_TASK_REQ_COMMAND_PHASE);
  fmass->csw.bCSWStatus = BULK_CSW_STAT_OK;
}

static void fmass_parts_registered(struct mass_dev_s *fmass, const char *dev_path)
{
#define NODE_NAME_LEN   0x10
  char node_name[NODE_NAME_LEN];
  struct inode *inode;
  los_part *part;
  int i;

  for (i = 0; i < MAX_FILE_STORAGE_LUNS; i++)
    {
      inode = NULL;
      (void)snprintf_s(node_name, sizeof(node_name), sizeof(node_name) - 1, "%s%0d", dev_path, i);
      (void)open_blockdriver(node_name, O_RDWR, &inode);
      if (inode == NULL)
        {
          return;
        }
      part = los_part_find(inode);
      if (part == NULL)
        {
          (void)close_blockdriver(inode);
          continue;
        }

      if (fmass->nluns == MAX_FILE_STORAGE_LUNS)
        {
          break;
        }

      fmass->fileNode[fmass->nluns] = inode;
      fmass->parts[fmass->nluns]    = part;
      fmass->nluns++;
    }

  return;
}

static int fmass_dev_open(struct mass_dev_s *fmass)
{
  uint32_t i;

  for (i = 0; i < sizeof(g_mass_device_type) / sizeof(g_mass_device_type[0]); i++)
    {
      fmass_parts_registered(fmass, g_mass_device_type[i]);
    }

  if (fmass->nluns == 0)
    {
      usb_err("device inode is unavailable!\n");
      return -1;
    }

  for (i = 0 ; i < MAX_DATA_BUFFER_NUM; i++)
    {
      struct fmass_data_buf_t *db = &fmass->databuf[i];

      if (db->buf != NULL)
        {
          continue;
        }

      db->buf = memalign(64, SKB_DATA_ALIGN(MAX_BLOCK_RW_SECTORS << 9));
      if (db->buf == NULL)
        {
            return -1;
        }
      db->state = DBUF_STATE_EMPTY;
      db->next  = db + 1;
    }
  fmass->databuf[i - 1].next = &fmass->databuf[0];

  fmass->dev_status = DEV_ST_CONNECTTED;
  fmass_notify_report(fmass, fmass->dev_status);

  return 0;
}

static int fmass_dev_close(struct mass_dev_s *fmass)
{
  uint32_t i;
  FAR struct inode *inode;

  if (fmass->nluns == 0)
    {
      return -1;
    }

  for (i = 0 ; i < fmass->nluns ; i++)
    {
      inode = fmass->fileNode[i];
      (void)close_blockdriver(inode);
      fmass->fileNode[i] = NULL;
      fmass->parts[i]    = NULL;
    }

  for (i = 0 ; i < MAX_DATA_BUFFER_NUM; i++)
    {
      free(fmass->databuf[i].buf);
      fmass->databuf[i].buf = NULL;
    }
  fmass->nluns = 0;
  return 0;
}

static void fmass_dev_capacity(struct mass_dev_s *fmass)
{
  uint32_t i;
  los_part *part;
  struct fmass_capacity *cap;

  for (i = 0; i < fmass->nluns; i++)
    {
      part = fmass->parts[i];
      cap  = &fmass->caps[i];
      cap->sectorsize = 0;
      cap->nsectors   = 0;

      (void)los_part_ioctl(part->part_id, GET_SECTOR_COUNT, &cap->nsectors);
      cap->nsectors += part->sector_start;
      (void)los_part_ioctl(part->part_id, GET_SECTOR_SIZE, &cap->sectorsize);
      PRINTK("*** %d  %d Bytes/Sector, Total %d Sectors ***\n", i, (int)cap->sectorsize, (int)cap->nsectors);
    }
}

static size_t fmass_dev_read(struct mass_dev_s *fmass,
                             uint8_t *buffer,
                             size_t stsector,
                             uint32_t nsectors)
{
  size_t rsectors = 0;
  los_part *part  = fmass->parts[fmass->lun];

  if (part != NULL)
    {
      int ret = los_part_read(part->part_id, buffer, stsector, nsectors);
      if (ret == 0)
        {
          rsectors = nsectors;
        }
    }
  return rsectors;
}

static size_t fmass_dev_write(struct mass_dev_s *fmass,
                              uint8_t *buffer,
                              size_t stsector,
                              uint32_t nsectors)
{
  size_t wsectors = 0;
  los_part *part  = fmass->parts[fmass->lun];

  if (part != NULL)
    {
      int ret = los_part_write(part->part_id, buffer, stsector, nsectors);
      if (ret == 0)
        {
          wsectors = nsectors;
        }
    }
  return wsectors;
}

void fmass_set_config(struct mass_dev_s *fmass, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep0;
  struct usbdev_req_s *req;

  if (dev->ep0 == NULL || dev->ep0->handle_req == NULL)
    {
      usb_err("set config fail!\n");
      return;
    }
  ep0 = dev->ep0;
  req = ep0->handle_req;

  if (fmass->nluns == 0)
    {
      if (fmass_dev_open(fmass) == 0)
        {
          fmass_dev_capacity(fmass);
        }
      else
        {
          usb_err("can not found inode!\n");
          return;
        }
    }

  if (fmass->bulk_in_enabled)
    {
      fmass->bulk_in_enabled = 0;
      (void)EP_DISABLE(fmass->bulkin);
    }

  if (fmass->bulk_out_enabled)
    {
      fmass->bulk_out_enabled = 0;
      (void)EP_DISABLE(fmass->bulkout);
    }

  usbd_configep_byspeed(dev, &g_fmass_confd.iepd);
  (void)EP_CONFIGURE(fmass->bulkin, (const usb_endpoint_descriptor_t *)&g_fmass_confd.iepd, 0);
  fmass->bulk_in_enabled = 1;

  usbd_configep_byspeed(dev, &g_fmass_confd.oepd);
  (void)EP_CONFIGURE(fmass->bulkout, (const usb_endpoint_descriptor_t *)&g_fmass_confd.oepd, 0);
  fmass->bulk_out_enabled = 1;

  fmass->databuf_fill        = &fmass->databuf[0];
  fmass->bulkreq.buf         = (uint8_t *)fmass->databuf[0].buf;
  fmass->bulkreq.is_complete = 1;

  req->len = 0;
  (void)EP_SUBMIT(ep0, req);

  fmass_task_change_state(fmass, FMASS_TASK_REQ_COMMAND_PHASE);
}

void fmass_set_next_command(struct mass_dev_s *fmass)
{
  fmass_task_state next_state;

  DPRINTFN(1, "\n\t -----REQUEST CBW packet -------\n");

  fmass->bulkreq.len = BULK_CBW_WRAP_LEN;

  /* start a request to receive a CBW packet */

  (void)fmass_bulkout_request(fmass, &fmass->bulkreq);

  /* wait for the CBW packet */

  if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_COMMAND_PHASE), osWaitForever))
    {
      DPRINTFN(0, "--- [CBW Phase] error! state: %u ---\n", fmass->task_state);
      if (taskstate_match(fmass, FMASS_TASK_REQ_COMMAND_PHASE))
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
        }
      return;
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DONE_COMMAND_PHASE))
    {
      DPRINTFN(0, "--- [CBW Phase] unmatch! unexpect state: %u ---\n", fmass->task_state);
      return ;
    }

  /* check and receive CBW packet */

  if (fmass_check_receive_cbw(fmass) < 0)
    {
      return ;
    }

  /* handle CBW packet */

  fmass_do_scsi_command(fmass);
  if (taskstate_unmatch(fmass, FMASS_TASK_DISCONNECT))
    {
      if (fmass->data_size)
        {
          next_state = FMASS_TASK_REQ_DATA_PHASE;
        }
      else
        {
          next_state = FMASS_TASK_REQ_STATUS_PHASE;
        }
      fmass_task_change_state(fmass, next_state);
    }
}

static void *fmass_main_thread(UINTPTR para, UINTPTR para1)
{
  struct mass_dev_s *fmass = (struct mass_dev_s *)para;
  struct usbdev_s *dev     = (struct usbdev_s *)para1;
  uint32_t oldstate        = FMASS_TASK_IDLE;
  uint32_t curstate        = fmass->task_state;
  PEVENT_CB_S fmass_event  = &fmass->task_event;
  uint32_t ret;

  while (1)
    {
      if (oldstate == curstate)
        {
          DPRINTFN(1, "\n -- process thread wait --(curstate:%u)\n", fmass->task_state);
          ret = LOS_EventRead(fmass_event, (FMASS_DATA_PROC | FMASS_NEED_EXIT),
                              LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
          if (ret & FMASS_NEED_EXIT)
            {
              break;
            }
          curstate = fmass->task_state;
        }
      oldstate = curstate;

      if (taskstate_match(fmass, FMASS_TASK_DISCONNECT))
        {
          fmass_report_usb_status(fmass);
          fmass->task_state &= ~FMASS_TASK_DISCONNECT;
          ret = LOS_EventPoll(&(fmass_event->uwEventID), FMASS_NEED_EXIT,
                              LOS_WAITMODE_OR | LOS_WAITMODE_CLR);
          if (ret == FMASS_NEED_EXIT)
            {
              break;
            }
        }
      else
        {
          switch (curstate)
            {
            case FMASS_TASK_CONFIG_CHANGE:
              fmass_set_config(fmass, dev);
              break;

            case FMASS_TASK_REQ_COMMAND_PHASE:
              fmass_set_next_command(fmass);
              break;

            case FMASS_TASK_REQ_DATA_PHASE:
              fmass_handle_scsi_data(fmass);
              break;

            case FMASS_TASK_REQ_STATUS_PHASE:
              fmass_set_scsi_status(fmass);
              break;

            default:
              break;
            }
        }

      curstate = fmass->task_state;
    }

  (void)LOS_EventWrite(&(fmass->task_event), FMASS_THREAD_EXITED);

  return NULL;
}

static void fmass_task_change_state(struct mass_dev_s *fmass, fmass_task_state new_state)
{
  uint32_t flags;
  uint32_t old_state = fmass->task_state;

  spin_lock_irqsave(&fmass->lock, flags);

  if (taskstate_unmatch(fmass, FMASS_TASK_DISCONNECT))
    {
      fmass->task_state &= ~old_state;
    }
  fmass->task_state |= new_state;

  spin_unlock_irqrestore(&fmass->lock, flags);
}

static void fmass_main_thread_signal(struct mass_dev_s *fmass, fmass_task_state state)
{
  DPRINTFN(1, " - process thread signal -(curstate:%u   newstate: %d)\n", fmass->task_state, state);
  fmass_task_change_state(fmass, state);
  (void)LOS_EventWrite(&(fmass->task_event), FMASS_DATA_PROC);
}

static uint32_t usb_task_creat(uint32_t *taskid, TSK_ENTRY_FUNC func, uint16_t prio,
                               const char *nm, UINTPTR para, UINTPTR para1)
{
  uint32_t ret;
  TSK_INIT_PARAM_S attr;

  (void)memset_s(&attr, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));

  attr.pfnTaskEntry = func;
  attr.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
  attr.auwArgs[0]   = (UINTPTR)para;
  attr.auwArgs[1]   = (UINTPTR)para1;
  attr.usTaskPrio   = prio;
  attr.pcName       = (char *)nm;
  attr.uwResved     = LOS_TASK_STATUS_DETACHED;

  ret = LOS_TaskCreate(taskid, &attr);
  if (ret != LOS_OK)
    {
      usb_err("create %s task error!\n", nm);
    }
  return ret;
}

static int fmass_thread_init(struct mass_dev_s *fmass, struct usbdev_s *dev)
{
  uint32_t td = 0;
  uint32_t ret;

  (void)LOS_EventInit(&fmass->task_event);
  mtx_init(&fmass->task_mtx, "tmtx", NULL, 0);
  init_waitqueue_head(&fmass->xfer_wait);

  fmass_task_change_state(fmass, FMASS_TASK_IDLE);

  ret = usb_task_creat(&td, (TSK_ENTRY_FUNC)fmass_main_thread, 10, "USB_FMASS_Task", (UINTPTR)fmass, (UINTPTR)dev);
  return (ret == 0) ? (0) : (-1);
}

static int fmass_thread_delete(struct mass_dev_s *fmass)
{
  uint32_t ret;

  (void)LOS_EventWrite(&(fmass->task_event), FMASS_NEED_EXIT);
  ret = LOS_EventRead(&fmass->task_event, FMASS_THREAD_EXITED, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, (10 * HZ));
  if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT)
    {
      usb_err("fmass, wait thread exit timeout\n");
      return -1;
    }

  (void)LOS_EventDestroy(&fmass->task_event);
  mtx_destroy(&fmass->task_mtx);

  return 0;
}

static void fmass_notify_report(struct mass_dev_s *fmass, int status)
{
  int i ;
  struct fmass_notify *notify;

  DPRINTFN(1, "\n< fmass storage %s >\n", (status == DEV_ST_CONNECTTED) ? "connected" : "disconnected");
  for (i = 0; i < MAX_NOFIFY_NUM ; i++)
    {
      notify = &fmass->notify[i];
      if (notify->is_used)
        {
          if (notify->notifycb != NULL)
            {
              notify->notifycb(notify->notifydata, status);
            }
        }
    }
}

static void fmass_report_usb_status(struct mass_dev_s *fmass)
{
  if (fmass->dev_status == DEV_ST_DISCONNECT)
    {
      if (fmass->nluns == 0) /* device is not ready, skip disconnect status */
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
          return;
        }
      (void)fmass_dev_close(fmass);
    }

  fmass_notify_report(fmass, fmass->dev_status);
  fmass_task_change_state(fmass, FMASS_TASK_IDLE);
}

/* status 0: disconnect  1: connect */

int fmass_register_notify(void(*notify)(void *context, int status), void *context)
{
  int i;
  struct fmass_notify *f_notify = g_notify.notify;
  struct mtx *notify_mtx        = &g_notify.notify_mtx;

  fmass_notify_init();

  mtx_lock(notify_mtx);
  for (i = 0; i < MAX_NOFIFY_NUM ; i++)
    {
      if (f_notify[i].is_used == 0)
        {
          f_notify[i].is_used    = 1;
          f_notify[i].notifycb   = notify;
          f_notify[i].notifydata = context;
          break;
        }
    }
  mtx_unlock(notify_mtx);
  return (i < MAX_NOFIFY_NUM) ? (i) : (-1);
}

int fmass_unregister_notify(int handle)
{
  struct fmass_notify *f_notify = g_notify.notify;
  struct mtx *notify_mtx        = &g_notify.notify_mtx;

  if (handle < 0 || handle >= MAX_NOFIFY_NUM)
    {
      return -1;
    }

  mtx_lock(notify_mtx);
  if (f_notify[handle].is_used == 0)
    {
      mtx_unlock(notify_mtx);
      return -1;
    }

  f_notify[handle].is_used    = 0;
  f_notify[handle].notifycb   = NULL;
  f_notify[handle].notifydata = NULL;
  mtx_unlock(notify_mtx);

  return 0;
}

int fmass_partition_startup(const char *path)
{
  struct mass_softc *mass = g_fmass;
  struct mass_dev_s *fmass;
  FAR struct inode *inode = NULL;
  uint32_t i;
  int ret;

  if (mass == NULL)
    {
      return -1;
    }
  fmass = &mass->dev;

  ret = open_blockdriver(path, O_RDWR, &inode);
  if (inode == NULL || ret != ENOERR)
    {
      DPRINTFN(0, "open blockdriver %s fail  ret = %d\n", path, ret);
      return -1;
    }

  for (i = 0; i < fmass->nluns; i++)
    {
      if (inode == fmass->fileNode[i])
        {
          fmass->lun = i;
          break;
        }
    }

  if (i == fmass->nluns)
    {
      usb_err("The device path is invalid\n");
      return -1;
    }

  return 0;
}

void *fmass_bind(void)
{
  return (void *)g_fmass;
}

static void fmass_source_free(void)
{
  g_fmass = NULL;
}

static int usbclass_mass_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep;
  struct mass_driver_s *mass_drvr;
  struct mass_dev_s *mass_dev;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  mass_drvr = (struct mass_driver_s *)driver;
  mass_dev  = mass_drvr->dev;
  if (mass_dev == NULL)
    {
      return -1;
    }

  (void)snprintf_s(mass_dev->inquiry_str, sizeof(mass_dev->inquiry_str), sizeof(mass_dev->inquiry_str) - 1,
                   "%-8s%-16s%-4s","Mass","Storage Device","1.00");
  mass_dev->notify = g_notify.notify;
  mass_dev->lun    = 0;
  mass_dev->nluns  = 0;

  ep = DEV_ALLOCEP(dev, g_fmass_confd.iepd.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fmass_confd.iepd);
  if (ep == NULL)
    {
      PRINT_ERR("%s,%d\n", __FUNCTION__, __LINE__);
      return -1;
    }
  ep->priv = (void *)mass_dev;
  ep->handle_req = &mass_dev->bulkreq;
  mass_dev->bulkin = ep;
  DPRINTFN(1, "bulkin:%#x, %02x\n", ep, ep->eplog);

  ep = DEV_ALLOCEP(dev, g_fmass_confd.oepd.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fmass_confd.oepd);
  if (ep == NULL)
    {
      PRINT_ERR("%s,%d\n", __FUNCTION__, __LINE__);
      return -1;
    }
  ep->priv = (void *)mass_dev;
  ep->handle_req = &mass_dev->bulkreq;
  mass_dev->bulkout = ep;
  DPRINTFN(1, "bulkout:%#x, %02x\n", ep, ep->eplog);

  if (fmass_thread_init(mass_dev, dev) < 0)
    {
      PRINT_ERR("fmass_thread_init failed\n");
      goto fail;
    }

  return 0;
fail:
  usb_err("composite_fmass_bind failed\n");
  (void)usbclass_mass_unbind(driver, dev);
  return -1;
}

static int usbclass_mass_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct mass_driver_s *mass_drvr;
  struct mass_dev_s *mass_dev;
  int ret;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  mass_drvr = (struct mass_driver_s *)driver;
  mass_dev  = mass_drvr->dev;
  if (mass_dev == NULL)
    {
      return -1;
    }

  if (mass_dev->dev_status == DEV_ST_CONNECTTED)
    {
      ret = fmass_dev_close(mass_dev);
      if (ret < 0)
        {
          PRINT_ERR("%s fmass dev close fail\n", __FUNCTION__);
          return -1;
        }
    }

  if (driver->ops != NULL && driver->ops->disconnect != NULL)
    {
      driver->ops->disconnect(driver, dev);
    }

  DEV_FREEEP(dev, mass_dev->bulkin);
  DEV_FREEEP(dev, mass_dev->bulkout);

  ret = fmass_thread_delete(mass_dev);
  if (ret < 0)
    {
      return -1;
    }
  fmass_source_free();

  return 0;
}

static int usbclass_mass_set_alt(struct mass_dev_s *mass, unsigned intf, unsigned alt)
{
  (void)intf;
  (void)alt;

  fmass_main_thread_signal(mass, FMASS_TASK_CONFIG_CHANGE);
  fmass_wakeup(mass);

  /* the '1' indecates that the invoking of 'usbd_endpoint_request' is in fmass's protocal */

  return 1;
}

static int usbclass_mass_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  struct mass_dev_s *mass;
  struct mass_driver_s *drvr;
  uint16_t w_index;
  uint16_t w_value;
  uint16_t w_length;
  struct usbdev_req_s *req;
  errno_t ret;
  uint32_t nlun_info;

  (void)dataout;
  (void)outlen;

  if (driver == NULL || ctrl == NULL || dev == NULL)
    {
      return -1;
    }

  drvr = (struct mass_driver_s *)driver;
  mass = drvr->dev;
  if (mass == NULL)
    {
      return -1;
    }
  w_index  = UGETW(ctrl->wIndex);
  w_value  = UGETW(ctrl->wValue);
  w_length = UGETW(ctrl->wLength);

  req = dev->ep0->handle_req;

  switch (ctrl->bRequest)
    {
    case USB_REQ_SET_CONFIGURATION:
    case USB_REQ_SET_INTERFACE:
      {
        return usbclass_mass_set_alt(mass, w_index, w_value);
      }
    case USB_BULK_GET_MAX_LUN:
      {
        if (ctrl->bmRequestType != (USB_DIR_IN |
            USB_TYPE_CLASS | USB_RECIP_INTERFACE))
          {
            break;
          }

        if (w_index != 0 || w_value != 0 || w_length != 1)
          {
            break;
          }

        nlun_info = mass->nluns ? (mass->nluns - 1) : (0);
        ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, &nlun_info, sizeof(nlun_info));
        if (ret != EOK)
          {
            usb_err("memcpy_s fail, %d \n", ret);
            return -1;
          }
        req->len = 1;
        (void)EP_SUBMIT(dev->ep0, req);
        PRINTK("****** USB BULK MAX LUN %u ******\n", mass->nluns);
      }
      break;

    default:
      break;
    }
  return 0;
}

static void usbclass_mass_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct mass_dev_s *mass;
  struct mass_driver_s *drvr;
  uint32_t flags;

  if (driver == NULL || dev == NULL)
    {
      return;
    }

  drvr = (struct mass_driver_s *)driver;
  mass = drvr->dev;
  if (mass == NULL)
    {
      return;
    }

  spin_lock_irqsave(&mass->lock, flags);
  if (taskstate_match(mass, FMASS_TASK_CONFIG_CHANGE))
    {
      spin_unlock_irqrestore(&mass->lock, flags);
      DPRINTFN(0, "Setting config, no need proc disconnect\n");
      return;
    }

  mass->dev_status = DEV_ST_DISCONNECT;
  spin_unlock_irqrestore(&mass->lock, flags);

  fmass_task_change_state(mass, FMASS_TASK_DISCONNECT);
  fmass_wakeup(mass);

  if (mass->bulk_in_enabled)
    {
      mass->bulk_in_enabled = 0;
      (void)EP_DISABLE(mass->bulkin);
    }

  if (mass->bulk_out_enabled)
    {
      mass->bulk_out_enabled = 0;
      (void)EP_DISABLE(mass->bulkout);
    }
}

struct usbd_string g_fmass_device_strings[6] =
{
  { 0, g_dt_string_id },
  { 1, g_dt_string_vid },
  { 2, g_dt_string_pid },
  { 3, g_dt_string_serial },
  { 4, g_dt_string_buf },
  USBD_DEVICE_STRINGS_END
};

void mass_mkdevdesc(uint8_t *buf)
{
  errno_t ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmass_device_desc, sizeof(g_fmass_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

int16_t mass_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  uint16_t total = UGETW(g_fmass_confd.confd.wTotalLength);
  errno_t ret;

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmass_confd, total);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return -1;
    }

  return (int16_t)total;
}

int mass_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fmass_device_strings[i].s != NULL; i++)
    {
      str = g_fmass_device_strings[i].s;
      if (g_fmass_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

#define MASS_NCONFIGS    1
#define MASS_CONFIGID    0
#define MASS_NINTERFACES 1
#define MASS_NSTRIDS     5
#define MASS_NUM_EPS     2
void mass_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = mass_mkdevdesc;
  dev->mkconfdesc = mass_mkcfgdesc;
  dev->mkstrdesc  = mass_mkstrdesc;

  dev->nconfigs = MASS_NCONFIGS;           /* Number of configurations supported */
  dev->configid = MASS_CONFIGID;           /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = MASS_NINTERFACES; /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = MASS_NSTRIDS;     /* Number of Strings */

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = MASS_NUM_EPS;
}

int mass_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                     struct usbdevclass_driver_s **classdev)
{
  struct mass_softc *mass_s ;
  struct mass_dev_s *priv;
  struct mass_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  mass_s = (struct mass_softc *)malloc(sizeof(struct mass_softc));
  if (mass_s == NULL)
    {
      return -1;
    }
  g_fmass = mass_s;

  /* Convenience pointers into the allocated blob */

  priv = &mass_s->dev;
  drvr = &mass_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct mass_dev_s), 0, sizeof(struct mass_dev_s));
  spin_lock_init(&priv->lock);

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_mass_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void mass_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct mass_driver_s *mass_drvr = (struct mass_driver_s *)classdev;
  struct mass_dev_s *priv ;
  struct mass_softc *mass_s;

  if (mass_drvr == NULL)
    {
      return;
    }

  priv = mass_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  mass_s = container_of(mass_drvr, struct mass_softc, drvr);
  free(mass_s);
}

void usbdev_mass_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the UAC driver to fill in the constants we didn't
   * know here.
   */

  mass_get_composite_devdesc(dev);

  /* Overwrite and correct some values... */
  /* The callback functions for the UAC class */

  dev->classobject  = mass_classobject;
  dev->uninitialize = mass_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_mass_initialize(struct module *mod, int n, void *arg)
{
  struct composite_softc *com_s = (struct composite_softc *)arg;
  struct composite_devdesc_s dev;
  int ret;

  (void)mod;
  (void)n;
  if (com_s == NULL)
    {
      return -1;
    }

  usbdev_mass_initialize_sub(&dev, 0, DEV_MASS);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** Mass device initialized successfully! **\n");
  return 0;
}

#undef USB_DEBUG_VAR

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */