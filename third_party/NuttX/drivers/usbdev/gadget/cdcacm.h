/****************************************************************************
 * drivers/usbdev/cdcacm.h
 *
 *   Copyright (C) 2011-2012, 2015, 2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 ****************************************************************************/
/****************************************************************************
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations,
 * which might include those applicable to Huawei LiteOS of U.S. and the country in
 * which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in
 * compliance with such applicable export control laws and regulations.
 ****************************************************************************/

#ifndef __DRIVERS_USBDEV_CDCACM_H
#define __DRIVERS_USBDEV_CDCACM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "gadget/composite.h"

/* Interface IDs:
 *
 * CDCACM_NINTERFACES              Two interfaces
 * CDCACM_NOTIFID                  ID of the notifier interface
 * CDCACM_NOTALTIFID               No alternate for the notifier interface
 * CDCACM_DATAIFID                 ID of the data interface
 * CDCACM_DATAALTIFID              No alternate for the data interface
 */

#define CDCACM_NOTALTIFID          (0)
#define CDCACM_DATAALTIFID         (0)

/* Buffer big enough for any of our descriptors (the config descriptor is the
 * biggest).
 */

#define CDCACM_MXDESCLEN           (64)
#define CDCACM_MAXSTRLEN           (CDCACM_MXDESCLEN-2)

/* Device descriptor values */

#define CDCACM_VERSIONNO           (0x0101) /* Device version number 1.1 (BCD) */

/* String language */

#define CDCACM_STR_LANGUAGE        (0x0409) /* en-us */

/* Descriptor strings.  If there serial device is part of a composite device
 * then the manufacturer, product, and serial number strings will be provided
 * by the composite logic.
 */

#ifndef CONFIG_CDCACM_COMPOSITE
#define CDCACM_LASTBASESTRID     (4)
#define CDCACM_STRBASE           (0)
#else
#define CDCACM_STRBASE           CONFIG_CDCACM_STRBASE
#define CDCACM_LASTBASESTRID     CONFIG_CDCACM_STRBASE
#endif

/* These string IDs only exist if a user-defined string is provided */

#ifdef CONFIG_CDCACM_NOTIFSTR
#define CDCACM_NOTIFSTRID        (CDCACM_LASTBASESTRID+1)
#else
#define CDCACM_NOTIFSTRID        CDCACM_LASTBASESTRID
#endif

#ifdef CONFIG_CDCACM_DATAIFSTR
#define CDCACM_DATAIFSTRID       (CDCACM_NOTIFSTRID+1)
#else
#define CDCACM_DATAIFSTRID       CDCACM_NOTIFSTRID
#endif

#define CDCACM_LASTSTRID           CDCACM_DATAIFSTRID
#define CDCACM_NSTRIDS             (CDCACM_LASTSTRID - CDCACM_STRBASE)

#define CDCACM_EP_INTIN_IDX        (0)
#define CDCACM_EP_BULKIN_IDX       (1)
#define CDCACM_EP_BULKOUT_IDX      (2)

/* Endpoint configuration ****************************************************/

#define CDCACM_MKEPINTIN(desc)     ((desc)->epno[CDCACM_EP_INTIN_IDX])

#define CDCACM_MKEPBULKIN(desc)    ((desc)->epno[CDCACM_EP_BULKIN_IDX])

#define CDCACM_MKEPBULKOUT(desc)   ((desc)->epno[CDCACM_EP_BULKOUT_IDX])

/* Device driver definitions ************************************************/
/* A CDC/ACM device is specific by a minor number in the range of 0-255.
 * This maps to a character device at /dev/ttyACMx, x=0..255.
 */

#define CDCACM_DEVNAME_FORMAT      "/dev/ttyGS0"
#define CDCACM_DEVNAME_SIZE        16

/* Misc Macros **************************************************************/
/* MIN/MAX macros */

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define CDCACM_NUM_EPS             (3)

#define CDCACM_NCONFIGS            (1)      /* Number of configurations supported */

/* Configuration descriptor values */

#define CDCACM_CONFIGID            (1)      /* The only supported configuration ID */
#define CDCACM_NINTERFACES         (2)      /* Number of interfaces in the configuration */

/* Table 69: UART State Bitmap Values */

#define CDC_UART_RXCARRIER      (1 << 0) /* bRxCarrier State of receiver carrier detection
                                          * mechanism of device. This signal corresponds to
                                          * V.24 signal 109 and RS-232 signal DCD.
                                          */
#define CDC_UART_TXCARRIER      (1 << 1) /* bTxCarrier State of transmission carrier. This
                                          * signal corresponds to V.24 signal 106 and RS-232
                                          * signal DSR.
                                          */

/* CDC/ACM friendly naming */

#define CDCACM_UART_DCD          CDC_UART_RXCARRIER
#define CDCACM_UART_DSR          CDC_UART_TXCARRIER

/* "SerialState is used like a real interrupt status register. Once a notification has been
 *  sent, the device will reset and reevaluate the different signals.  For the consistent
 *  signals like carrier detect or transmission carrier, this will mean another notification
 *  will not be generated until there is a state change.  For the irregular signals like
 *  break, the incoming ring signal, or the overrun error state, this will reset their values
 *  to zero and again will not send another notification until their state changes."
 */

#define CDC_UART_CONSISTENT     (CDC_UART_RXCARRIER | CDC_UART_TXCARRIER)

#ifndef CONFIG_CDCACM_EPBULKOUT_FSSIZE
#define CONFIG_CDCACM_EPBULKOUT_FSSIZE 0x200
#endif

#ifndef CONFIG_CDCACM_EPBULKOUT_HSSIZE
#define CONFIG_CDCACM_EPBULKOUT_HSSIZE 0x400
#endif

#ifndef CONFIG_CDCACM_EPBULKIN_FSSIZE
#define CONFIG_CDCACM_EPBULKIN_FSSIZE 0x200
#endif

#ifndef CONFIG_CDCACM_EPBULKIN_HSSIZE
#define CONFIG_CDCACM_EPBULKIN_HSSIZE 0x400
#endif

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define DWC3_USB_SERIAL
#endif

#ifdef DWC3_USB_SERIAL
#define MAX_PACKET_SIZE 0x400
#else
#define MAX_PACKET_SIZE 0x200
#endif

#define ACM_NOTIFY_SERIAL_STATE 0x20
#define ACM_GET_LINE_CODING     0x21 /* Requests current DTE rate, stop-bits, parity, and
                                      * number-of-character bits. (Optional)
                                      */
#define ACM_SET_LINE_CODING     0x20 /* Configures DTE rate, stop-bits, parity, and
                                      * number-of-character bits. (Optional)
                                      */
#define ACM_SET_CTRL_LINE_STATE 0x22 /* RS-232 signal used to tell the DCE device the
                                      * DTE device is now present. (Optional)
                                      */
#define SIZEOF_CDC_LINECODING 7

#define USB_SERIAL_READ_EVENT   0x11

/* define descriptor */

#define DEVICE_VENDOR_ID        0x0525
#define DEVICE_PRODUCT_ID       0xa4a7
#define DEVICE_VERSION          0x0100

#define RING_BUFFER_SIZE 6144U

#define STAE_BUFFER_OFFSET 1

struct cdcacm_hs_function_descriptor
{
  struct usb_interface_assoc_descriptor ifcad;
  struct usb_interface_descriptor ifcd; /* acm control interface descriptor */
  struct usb_cdc_header_descriptor cdc_desc;
  struct usb_cdc_cm_descriptor cdc_call_desc;
  struct usb_cdc_acm_descriptor cdc_acm_desc;
  struct usb_cdc_union_desc cdc_union_desc;
  struct usb_endpoint_descriptor nepd;
#ifdef DWC3_USB_SERIAL
  struct usb_endpoint_ss_comp_descriptor ncompd;
#endif
  struct usb_interface_descriptor ifdd;
  struct usb_endpoint_descriptor iepd;
#ifdef DWC3_USB_SERIAL
  struct usb_endpoint_ss_comp_descriptor icompd;
#endif
  struct usb_endpoint_descriptor oepd;
#ifdef DWC3_USB_SERIAL
  struct usb_endpoint_ss_comp_descriptor ocompd;
#endif
} __packed;

/****************************************************************************
 * Public Types
 ****************************************************************************/

enum cdcacm_epdesc_e
{
  CDCACM_EPINTIN = 0,  /* Interrupt IN endpoint descriptor */
  CDCACM_EPBULKOUT,    /* Bulk OUT endpoint descriptor */
  CDCACM_EPBULKIN      /* Bulk IN endpoint descriptor */
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern struct cdcacm_hs_function_descriptor g_cdcacm_hs_func_desc;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void cdcacm_mkdevdesc(uint8_t *buf);

/****************************************************************************
 * Name: cdcacm_mkstrdesc
 *
 * Description:
 *   Construct a string descriptor
 *
 ****************************************************************************/

int cdcacm_mkstrdesc(uint8_t id, uint8_t *buf);

/****************************************************************************
 * Name: cdcacm_mkcfgdesc
 *
 * Description:
 *   Construct the configuration descriptor
 *
 ****************************************************************************/

int16_t cdcacm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo);

void usbdev_cdcacm_initialize_sub(FAR struct composite_devdesc_s *dev, int ifnobase, int minor);
#endif /* __DRIVERS_USBDEV_CDCACM_H */
