/************************************************************************************
 * include/nuttx/usb/usbdev.h
 *
 *   Copyright (C) 2008-2010, 2012-2013, 2017 Gregory Nutt. All rights reserved.
 *   Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * NOTE:  This interface was inspired by the Linux gadget interface by
 * David Brownell. That work was very helpful in determining a usable
 * partitioning of functionality between standard class drivers and various
 * implementations of USB controller drivers.  This work, however, does
 * not derive directly from that work and is licensed differently.
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
/************************************************************************************
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations,
 * which might include those applicable to Huawei LiteOS of U.S. and the country in
 * which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in
 * compliance with such applicable export control laws and regulations.
 ************************************************************************************/

#ifndef __INCLUDE_NUTTX_USB_USBDEV_H
#define __INCLUDE_NUTTX_USB_USBDEV_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include "implementation/global_implementation.h"
#include "linux/list.h"
#include "errno.h"
#define CONFIG_USBDEV_COMPOSITE
#define CONFIG_USBDEV_DMA
#define FAR
/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Endpoint helpers *****************************************************************/

/* Configure endpoint, making it usable.  The class driver may deallocate or re-use
 * the 'desc' structure after returning:
 *
 * ep   - the struct usbdev_ep_s instance obtained from allocep()
 * desc - A struct usb_epdesc_s instance describing the endpoint
 * last - true if this this last endpoint to be configured.  Some hardware needs
 *        to take special action when all of the endpoints have been configured.
 */

#define EP_CONFIGURE(ep, desc, last) (ep)->ops->configure(ep, desc, last)

/* The endpoint will no longer be used */

#define EP_DISABLE(ep)              (ep)->ops->disable(ep)

/* Allocate/free I/O requests.  Should not be called from interrupt processing! */

#define EP_ALLOCREQ(ep)             (ep)->ops->allocreq(ep)
#define EP_FREEREQ(ep, req)         (ep)->ops->freereq(ep, req)

/* Allocate/free an I/O buffer.  Should not be called from interrupt processing! */

#ifdef CONFIG_USBDEV_DMA
#  define EP_ALLOCBUFFER(ep, nb)    (ep)->ops->allocbuffer(ep, nb)
#  define EP_FREEBUFFER(ep, buf)    (ep)->ops->freebuffer(ep, buf)
#else
#  define EP_ALLOCBUFFER(ep, nb)    malloc(nb)
#  define EP_FREEBUFFER(ep, buf)    free(buf)
#endif

/* Submit an I/O request to the endpoint */

#define EP_SUBMIT(ep, req)          (ep)->ops->submit(ep, req)

/* Cancel an I/O request previously sent to an endpoint */

#define EP_CANCEL(ep, req)          (ep)->ops->cancel(ep, req)

/* Stall or resume an endpoint */

#define EP_STALL(ep)               (ep)->ops->stall(ep, false)
#define EP_RESUME(ep)              (ep)->ops->stall(ep, true)

#define EP_FLUSH(ep)               (ep)->ops->flush(ep)

/* USB Device Driver Helpers ********************************************************/

/* Allocate an endpoint:
 *
 *   ep     - 7-bit logical endpoint number (direction bit ignored).  Zero means
 *            that any endpoint matching the other requirements will suffice.  The
 *            assigned endpoint can be found in the eplog field.
 *   in     - true: IN (device-to-host) endpoint requested
 *   eptype - Endpoint type.  One of {USB_EP_ATTR_XFER_ISOC, USB_EP_ATTR_XFER_BULK,
 *            USB_EP_ATTR_XFER_INT}
 */

#define DEV_ALLOCEP(dev, ep, desc) (dev)->ops->allocep(dev, ep, desc)

/* Release an endpoint */

#define DEV_FREEEP(dev, ep)        (dev)->ops->freeep(dev, ep)

/* Returns the current frame number */

#define DEV_GETFRAME(dev)          (dev)->ops->getframe(dev)

/* Tries to wake up the host connected to this device */

#define DEV_WAKEUP(dev)            (dev)->ops->wakeup(dev)

/* Sets the device selfpowered feature */

#define DEV_SETSELFPOWERED(dev)    (dev)->ops->selfpowered(dev, true)

/* Clears the device selfpowered feature */

#define DEV_CLRSELFPOWERED(dev)    (dev)->ops->selfpowered(dev, false)

/* Software-controlled connect to USB host. All USB class drivers need to call
 * DEV_CONNECT() when they are ready to be enumerated.  That is, (1) initially when
 * bound to the USB driver, and (2) after a USB reset.
 */

#define DEV_CONNECT(dev)           (dev)->ops->pullup ? (dev)->ops->pullup(dev, true) : -EOPNOTSUPP

/* Software-controlled disconnect from USB host */

#define DEV_DISCONNECT(dev)        (dev)->ops->pullup ? (dev)->ops->pullup(dev, false) : -EOPNOTSUPP

/* USB Class Driver Helpers *********************************************************/
/* All may be called from interrupt handling logic except bind() and unbind() */

/* Invoked when the driver is bound to a USB device driver. */

#define CLASS_BIND(drvr, dev)      (drvr)->ops->bind(drvr, dev)

/* Invoked when the driver is unbound from a USB device driver */

#define CLASS_UNBIND(drvr, dev)    (drvr)->ops->unbind(drvr, dev)

/* Invoked after all transfers have been stopped, when the host is disconnected. */

#define CLASS_DISCONNECT(drvr, dev) (drvr)->ops->disconnect(drvr, dev)

/* Invoked for ep0 control requests */

#define CLASS_SETUP(drvr, dev, ctrl, dataout, outlen) \
  (drvr)->ops->setup(drvr, dev, ctrl, dataout, outlen)

/* Invoked on USB suspend. */

#define CLASS_SUSPEND(drvr, dev)   \
  do { if ((drvr)->ops->suspend) (drvr)->ops->suspend(drvr, dev); } while (0)

/* Invoked on USB resume */

#define CLASS_RESUME(drvr, dev)  \
  do { if ((drvr)->ops->resume) (drvr)->ops->resume(drvr, dev); } while (0)

/* Maximum size of a request buffer */

#define USBDEV_MAXREQUEUST        UINT16_MAX

#define USBDEV_MAX_EPNUM    5

/* Request flags */

#define USBDEV_REQFLAGS_NULLPKT   1 /* Bit 0: Terminate w/short packet; null packet if necessary */
                                    /* Bits 1-7: Available */

/* USB directions (in endpoint addresses) */

#define USB_EPNO_MASK                           (0x7f)
#define USB_EPNO(addr)                          ((addr) & USB_EPNO_MASK)
#define USB_EPOUT(addr)                         ((addr) | USB_DIR_OUT)
#define USB_EPIN(addr)                          ((addr) | USB_DIR_IN)
#define USB_ISEPIN(addr)                        (((addr) & USB_DIR_MASK) == USB_DIR_IN)
#define USB_ISEPOUT(addr)                       (((addr) & USB_DIR_MASK) == USB_DIR_OUT)


/************************************************************************************
 * Public Types
 ************************************************************************************/

/* USB Controller Structures ********************************************************/

/* usbdev_devinfo_s - describes the low level bindings of an usb device */

struct usbdev_devinfo_s
{
  int ninterfaces; /* Number of interfaces in the configuration */
  int ifnobase;    /* Offset to Interface-IDs */

  int nstrings;    /* Number of Strings */
  int strbase;     /* Offset to String Numbers */

  int nendpoints;  /* Number of Endpoints referenced in the following allay */
  int epno[USBDEV_MAX_EPNUM]; /* Array holding the endpoint configuration for this device */
};

struct usbdevclass_driver_s;
struct composite_devdesc_s
{
  void (*mkdevdesc)(uint8_t *buf);
  int16_t (*mkconfdesc)(uint8_t *buf, struct usbdev_devinfo_s *devinfo);
  int (*mkstrdesc)(uint8_t id, uint8_t *buf);
  int (*classobject)(int minor, struct usbdev_devinfo_s *devinfo, struct usbdevclass_driver_s **classdev);
  void (*uninitialize)(struct usbdevclass_driver_s *classdev);

  int nconfigs;    /* Number of configurations supported */
  int configid;    /* The only supported configuration ID */

  int cfgdescsize; /* The size of the config descriptor */
  int minor;

  struct usbdev_devinfo_s devinfo;
};

/* struct usbdev_req_s - describes one i/o request */

struct usbdev_ep_s;
struct usbdev_req_s
{
  uint8_t *buf;    /* Call: Buffer used for data; Return: Unchanged */
  uint8_t  flags;  /* See USBDEV_REQFLAGS_* definitions */
  int16_t  result; /* Call: zero; Return: Result of transfer (O or -errno) */
  uint32_t len;    /* Call: Total length of data in buf; Return: Unchanged */
  uint32_t xfrd;   /* Call: zero; Return: Bytes transferred so far */

  /* Callback when the transfer completes */

  void   (*callback)(struct usbdev_ep_s *ep, struct usbdev_req_s *req);
  void    *priv;    /* Used only by callee */
  int is_complete;
  struct list_head list;

 /* Added in device3.0 */

#if defined(LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER)
  uintptr_t dma;
  uint32_t  stream_id;
  uint8_t   zero;
#endif
};

/* Endpoint-specific interface to USB controller hardware. */

struct usbdev_epops_s
{
  /* Configure/enable and disable endpoint */

  int (*configure)(struct usbdev_ep_s *ep, const usb_endpoint_descriptor_t *desc,
                   bool last);
  int (*disable)(struct usbdev_ep_s *ep);

  /* Allocate and free I/O requests */

  struct usbdev_req_s *(*allocreq)(FAR struct usbdev_ep_s *ep);
  void (*freereq)(struct usbdev_ep_s *ep, struct usbdev_req_s *req);

  /* Allocate and free I/O buffers */

  void *(*allocbuffer)(struct usbdev_ep_s *ep, uint16_t nbytes);
  void (*freebuffer)(struct usbdev_ep_s *ep, void *buf);

  /* Submit and cancel I/O requests */

  int (*submit)(struct usbdev_ep_s *ep, struct usbdev_req_s *req);
  int (*cancel)(struct usbdev_ep_s *ep, struct usbdev_req_s *req);

  /* Stall or resume an endpoint */

  int (*stall)(struct usbdev_ep_s *ep, bool resume);
  void (*flush)(struct usbdev_ep_s *ep);
};

/* Representation of one USB endpoint */

struct usbdev_ep_s
{
  const struct usbdev_epops_s *ops; /* Endpoint operations */
  uint8_t  eplog;                   /* Logical endpoint address */
  uint8_t  used;
  uint16_t maxpacket;               /* Maximum packet size for this endpoint */
  void    *priv;                    /* For use by class driver */
  void    *ep_link;
  struct usbdev_req_s    *handle_req;

  /* Added in device3.0 */

#if defined(LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER)
  uint16_t maxpacket_limit:16;
  uint16_t max_streams:16;
  uint16_t mult:2;
  uint16_t maxburst:5;
  const struct usb_endpoint_ss_comp_descriptor *comp_desc;
#endif
};

/* struct usbdev_s represents a usb device */

struct usbdev_s;
struct usbdev_ops_s
{
  /* Allocate and free endpoints */

  struct usbdev_ep_s *(*allocep)(struct usbdev_s *dev, uint8_t eplog, usb_endpoint_descriptor_t *desc);
  void (*freeep)(struct usbdev_s *dev, struct usbdev_ep_s *ep);

  /* Get the frame number from the last SOF */

  int (*getframe)(struct usbdev_s *dev);

  /* Hardware specific features */

  int (*wakeup)(struct usbdev_s *dev);
  int (*selfpowered)(struct usbdev_s *dev, bool selfpowered);
  int (*pullup)(struct usbdev_s *dev,  bool enable);

  /* Device-specific I/O command support */

  int (*ioctl)(struct usbdev_s *dev, unsigned code, unsigned long param);
};

struct usbdev_s
{
  const struct usbdev_ops_s *ops; /* Access to hardware specific features */
  struct usbdev_ep_s *ep0;        /* Endpoint zero */
  uint8_t speed;                  /* Current speed of the host connection */
  uint8_t dualspeed:1;            /* 1:supports high and full speed operation */
};

/* USB Device Class Implementations *************************************************/

struct usbdevclass_driverops_s
{
  int  (*bind)(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
  int  (*unbind)(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
  int  (*setup)(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);
  void (*disconnect)(struct usbdevclass_driver_s *driver,
                     struct usbdev_s *dev);
  void (*suspend)(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
  void (*resume)(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
};

struct usbdevclass_driver_s
{
  struct usbdevclass_driverops_s *ops;
  uint8_t speed; /* Highest speed that the driver handles */
};

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

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: usbdevclass_register
 *
 * Description:
 *   Register a USB device class driver. The class driver's bind() method will be
 *   called to bind it to a USB device driver.
 *
 ************************************************************************************/

int usbdev_register(struct usbdevclass_driver_s *driver);

/************************************************************************************
 * Name: usbdev_unregister
 *
 * Description:
 *   Un-register usbdev class driver.If the USB device is connected to a USB host,
 *   it will first disconnect().  The driver is also requested to unbind() and clean
 *   up any device state, before this procedure finally returns.
 *
 ************************************************************************************/

int usbdev_unregister(struct usbdevclass_driver_s *driver);

/****************************************************************************
 * Name: usbdev_dma_alloc and usbdev_dma_free
 *
 * Description:
 *   The USB class driver allocates packet I/O buffers for data transfer by
 *   calling the driver allocbuffer() and freebuffer() methods.  Those
 *   methods are only available if CONFIG_USBDEV_DMA is defined in the
 *   system configuration.
 *
 *   If CONFIG_USBDEV_DMAMEMORY is also defined in the NuttX configuration,
 *   then the driver implementations of the allocbuffer() and freebuffer()
 *   methods may use board-specific usbdev_dma_alloc() and usbdev_dma_free().
 *   If CONFIG_USBDEV_DMA and CONFIG_USBDEV_DMAMEMORY are both defined,
 *   then the board-specific logic must provide the functions
 *   usbdev_dma_alloc() and usbdev_dma_free() as prototyped below:
 *   usbdev_dma_alloc() will allocate DMA-capable memory of the specified
 *   size; usbdev_dma_free() is the corresponding function that will be
 *   called to free the DMA-capable memory.
 *
 *   This functions may be simple wrappers around gran_alloc() and
 *   gran_free() (See nuttx/mm/gran.h).  Note that the gran_free() function
 *   does require the size of the allocation to be freed; that would need
 *   to be managed in the board-specific logic.
 *
 ****************************************************************************/

#if defined(CONFIG_USBDEV_DMA) && defined(CONFIG_USBDEV_DMAMEMORY)
void *usbdev_dma_alloc(size_t size);
void usbdev_dma_free(void *memory);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __INCLUDE_NUTTX_USB_USBDEV_H */
