/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LiteOS USB Driver UVC Protocol
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
#include "gadget/hicamera_control.h"
#include "implementation/global_implementation.h"
#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otg_pcd.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef F_UVC_DEBUG
#define FUVC_DEBUG(x...) dprintf(x)
#else
#define FUVC_DEBUG(x...) do {} while (0)
#endif

#define FUVC_BULK_TO_ISO_CONVERT 1 /* 0: Bulk mode, 1: ISO mode */

#if FUVC_BULK_TO_ISO_CONVERT
#define STREAM_BUF_SIZE 3072
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define FRAME_BUF_SIZE_MAX 4147200
#else
#define FRAME_BUF_SIZE_MAX 1843200
#endif
#else
#define STREAM_BUF_SIZE 16384
#define FRAME_BUF_SIZE_MAX 1843200
#endif

int usbdev_uvc_initialize(struct module *mod, int n, void *arg);

EVENT_CB_S g_uvc_event;

static const driver_t g_fuvc_driver =
{
  .name    = "fuvc",
  .methods = NULL,
  .size    = sizeof(struct uvc_softc)
};

/* private device class information */

static devclass_t g_fuvc_devclass;

DRIVER_MODULE(fuvc, simple, g_fuvc_driver, g_fuvc_devclass, usbdev_uvc_initialize, 0);

static int usbclass_uvc_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uvc_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_uvc_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_uvc_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_uvc_driverops =
{
  usbclass_uvc_bind,
  usbclass_uvc_unbind,
  usbclass_uvc_setup,
  usbclass_uvc_disconnect,
  NULL,
  NULL
};

static const char g_fuvc_str_lang[] =
{
  4, UDESC_STRING,
  0x09, 0x04
};

#define UVC_STR_IDX_MAN   1
static const char g_fuvc_str_manufacturer[] =
{
  14, UDESC_STRING,
  'H', 0, 'U', 0, 'A', 0, 'W', 0, 'E', 0, 'I', 0
};

#define UVC_STR_IDX_PRODUCT   2
static const char g_fuvc_str_product[] =
{
  14, UDESC_STRING,
  'L', 0, 'i', 0, 't', 0, 'e', 0, 'O', 0, 'S', 0
};

#define UVC_STR_IDX_CONFIG   5
static const char g_fuvc_str_config[] =
{
  22, UDESC_STRING,
  'U', 0, 'V', 0, 'C', 0, ' ', 0, 'C', 0, 'A', 0,
  'M', 0, 'E', 0, 'R', 0, 'A', 0
};

#define UVC_STR_IDX_VIDEO   4
static const char g_fuvc_str_video[] =
{
  12, UDESC_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0
};

#define UVC_STR_IDX_INTERFACE 6
static const char g_fuvc_str_interface[] =
{
  32, UDESC_STRING,
  'V', 0, 'i', 0, 'd', 0, 'e', 0, 'o', 0, ' ', 0, 'S', 0, 't', 0,
  'r', 0, 'e', 0, 'a', 0, 'm', 0, 'i', 0, 'n', 0, 'g', 0
};

/*
 * USB Video Class, Device Descriptor, for the details,
 * please refer to USB 2.0 Specification, section 9.6.1 and
 * USB Video Example 1.5, section 2.3.1
 */

static const struct usb_device_descriptor g_fuvc_device_desc =
{
  .bLength            = sizeof(struct usb_device_descriptor),
  .bDescriptorType    = UDESC_DEVICE,        /* Constant for device descriptor */
  HSETW(.bcdUSB, UD_BCD_USB),                /* USB version required: 2.0 */
  .bDeviceClass       = UICLASS_IAD,         /* Miscellaneous Device Class */
  .bDeviceSubClass    = 0x02,                /* Common Class */
  .bDeviceProtocol    = 0x01,                /* Interface Association Descriptor */
  .bMaxPacketSize     = UD_USB_MPS,          /* Control Endpoint packet size */
  HSETW(.idVendor,    0x1d6b),               /* Vendor ID of Huawei Technologies */
  HSETW(.idProduct,   0x0102),               /* Product ID, webcamera? */
  HSETW(.bcdDevice,   0x0318),               /* Device release code */
  .iManufacturer      = UVC_STR_IDX_MAN,     /* Manufacturer name, string index */
  .iProduct           = UVC_STR_IDX_PRODUCT, /* Product name, string index */
  .iSerialNumber      = 0,                   /* Not Used */
  .bNumConfigurations = 1                    /* One Configuration */
};

/*
 * USB Video Class, Configuration Descriptor, for the details,
 * please refer to USB 2.0 Specification, section 9.6.3 and
 * USB Video Example 1.5, section 2.3.2
 */

static struct usb_config_descriptor g_fuvc_config_desc =
{
  .bLength              = sizeof(struct usb_config_descriptor),
  .bDescriptorType      = UDESC_CONFIG,
  HSETW(.wTotalLength,  0),             /* Size of all descriptors, set later */
  .bNumInterface        = 0x2,          /* Two Interfaces */
  .bConfigurationValue  = 0x1,          /* ID of this configuration */
  .iConfiguration       = 0x4,          /* Index of string descriptor */
  .bmAttributes         = 0x80 | UC_SELF_POWERED, /* Self-powered */
  .bMaxPower            = 1             /* Maximum power consumption from the bus */
};

/*
 * USB Interface Association Descriptor, for the details,
 * please refer to USB Video Class specification 1.5, section 3.6 or
 * USB Video Example 1.5, section 2.3.3
 */

static struct usb_interface_assoc_descriptor g_fuvc_iad =
{
  .bLength              = sizeof(struct usb_interface_assoc_descriptor),
  .bDescriptorType      = UDESC_IFACE_ASSOC,
  .bFirstInterface      = 0,   /* Interface number of VideoControl interface */
  .bInterfaceCount      = 2,   /* Number of contiguous Video interfaces */
  .bFunctionClass       = UICLASS_VIDEO,
  .bFunctionSubClass    = UICLASS_VIDEO_INTERFACE_COLLECTION,
  .bFunctionProtocol    = UICLASS_VIDEO_PROTOCOL_UNDEFINED,
  .iFunction            = UVC_STR_IDX_CONFIG  /* index of string descriptor */
};

/*
 * USB VideoControl Interface Descriptor, for the details,
 * please refer to USB Video Class specification 1.5, section 3.7.1
 * or USB Video Example 1.5, section 2.3.4.1
 */

static struct usb_interface_descriptor g_fuvc_vc_intf_desc =
{
  .bLength              = sizeof(struct usb_interface_descriptor),
  .bDescriptorType      = UDESC_INTERFACE,
  .bInterfaceNumber     = 0,   /* index number of this interface */
  .bAlternateSetting    = 0,   /* index of this settings */
  .bNumEndpoints        = 1,   /* one endpoint */
  .bInterfaceClass      = UICLASS_VIDEO,
  .bInterfaceSubClass   = UICLASS_VIDEO_CONTROL,
  .bInterfaceProtocol   = UICLASS_VIDEO_PROTOCOL_UNDEFINED,
  .iInterface           = UVC_STR_IDX_CONFIG    /* index of string descriptor */
};

/* UVC Class-specific VC interface header descriptor, UVC Example 1.5, section 2.3.4.2 */

static struct uvc_vc_header_descriptor g_fuvc_vc_head_desc =
{
  .bLength              = sizeof(struct uvc_vc_header_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = UVC_VC_HEADER,
  .bcdUVC               = 0x0110,     /* UVC specification version, 1.1 */
  .wTotalLength         = 0,          /* total length, currently not set */
  .dwClockFrequency     = 48000000,   /* clock frequency */
  .bInCollection        = 0x1,        /* Number of streaming interfaces */
  .bInterfaceNr         = 0x1         /* Associated Video Streaming Interface */
};

#define UNIT_ID_CAMERA               1
#define UNIT_ID_PROCESSING           2
#define UNIT_ID_OUTPUT               3
#define UNIT_ID_H264_EXTENSION       10
#define UNIT_ID_HICAMERA_EXTENSION   0x11
#define ENABLE_H264                  1

/* UVC input terminal descriptor, camera; UVC Example 1.5, section 2.3.4.3 */

static const struct uvc_ct_descriptor g_fuvc_vc_camera =
{
  .bLength                  = sizeof(struct uvc_ct_descriptor),
  .bDescriptorType          = USB_UVC_CS_INTERFACE, /* CS_INTERFACE */
  .bDescriptorSubtype       = UVC_VC_INPUT_TERMINAL,
  .bTerminalID              = UNIT_ID_CAMERA,       /* ID of this input terminal */
  .wTerminalType            = USB_UVC_ITT_CAMERA,
  .bAssocTerminal           = 0,                    /* No Association */
  .iTerminal                = 0,                    /* Not used */
  .wObjectiveFocalLengthMin = 0,                    /* No optical zoom used */
  .wObjectiveFocalLengthMax = 0,                    /* No optical zoom used */
  .wOcularFocalLength       = 0,                    /* No optical zoom used */
  .bControlSize             = 3,                    /* Last Three bytes */
  .bmControls               = { 0x0e, 0x0a, 0x00 }
};

/* UVC Example 1.5, section 2.3.4.7 */

static const struct uvc_processing_descriptor g_fuvc_proc_desc =
{
  .bLength              = sizeof(struct uvc_processing_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,  /* CS_INTERFACE */
  .bDescriptorSubtype   = UVC_VC_PROCESSING_UNIT,
  .bUnitID              = UNIT_ID_PROCESSING,    /* ID of this unit */
  .bSourceID            = 0x01,                  /* input pin is connected to the output pin of unit 1 */
  .wMaxMultiplier       = 16 * 1024,
  .bControlSize         = 0x3,                   /* sizeof `bmControls fields */
  .bmControls           = { 0x5b, 0x17, 0x00 },
  .iProcessing          = 0,                     /* no string index descriptor */
  .bmVideoStandards     = 0                      /* ignored */
};

/* UVC Example 1.5, section 2.3.4.5 */

static const struct uvc_ot_descriptor g_fuvc_vc_output_desc =
{
  .bLength              = sizeof(struct uvc_ot_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,  /* CS_INTERFACE */
  .bDescriptorSubtype   = UVC_VC_OUTPUT_TERMINAL,
  .bTerminalID          = UNIT_ID_OUTPUT,        /* ID of this output terminal */
  .wTerminalType        = USB_UVC_TT_STREAMING,
  .bAssocTerminal       = 0,                     /* No Association */
  .bSourceID            = UNIT_ID_PROCESSING,    /* input pin connected to the output pin of unit 2 */
  .iTerminal            = 0                      /* Not used */
};

#if ENABLE_H264
static const struct uvc_extension_unit_descriptor1x2 g_fuvc_h264_ext_desc =
{
  .bLength              = sizeof(struct uvc_extension_unit_descriptor1x2),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = UVC_VC_EXTENSION_UNIT,
  .bUnitID              = UNIT_ID_H264_EXTENSION,
  .guidExtensionCode    =
  {
    /* refer UVC spec. 1.1, "USB_Video_Payload_H 264_1 0", Appendix-A */

    0x41, 0x76, 0x9e, 0xa2, 0x04, 0xde, 0xe3, 0x47,
    0x8b, 0x2b, 0xf4, 0x34, 0x1a, 0xff, 0x00, 0x3b
  },
  .bNumControls         = 15,
  .bNrInPins            = 0x1,
  .baSourceID           = {0x02},
  .bControlSize         = 2,
  .bmControls           = { 0xff, 0xff },
  .iExtension           = 0
};
#endif

static const struct uvc_extension_unit_descriptor1x2 g_fuvc_hicamera_ext_desc =
{
  .bLength              = sizeof(struct uvc_extension_unit_descriptor1x2),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = UVC_VC_EXTENSION_UNIT,
  .bUnitID              = UNIT_ID_HICAMERA_EXTENSION,
  .guidExtensionCode    =
  {
    /* refer UVC spec. 1.1, "USB_Video_Payload_H 264_1 0", Appendix-A */

    0x91, 0x72, 0x1e, 0x9a, 0x43, 0x68, 0x83, 0x46,
    0x6d, 0x92, 0x39, 0xbc, 0x79, 0x06, 0xee, 0x49
  },
  .bNumControls         = 0x15,
  .bNrInPins            = 0x1,
  .baSourceID           = {0x01},
  .bControlSize         = 2,
  .bmControls           = { 0xff, 0xff },
  .iExtension           = 0
};

#if UVC_USE_CTRL_EP
static struct usb_endpoint_descriptor g_fuvc_ctrl_ep_desc =
{
  .bLength              = sizeof(struct usb_endpoint_descriptor),
  .bDescriptorType      = UDESC_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .bEndpointAddress     = UE_DIR_IN | 0x3,
  .bmAttributes         = UE_XFERTYPE,
  HSETW(.wMaxPacketSize, 16),
  .bInterval            = 8
};
#endif

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
static const struct usb_endpoint_ss_comp_descriptor g_fuvc_ctrl_ss_comp_desc =
{
  .bLength              = sizeof(struct usb_endpoint_ss_comp_descriptor),
  .bDescriptorType      = 0x30,
  .bMaxBurst            = 0,
  .bmAttributes         = 0,
  HSETW(.wBytesPerInterval, 0x0010)
};
#endif

static const struct uvc_vc_int_endpoint_descriptor g_fuvc_int_ep_desc =
{
  .bLength              = sizeof(struct uvc_vc_int_endpoint_descriptor),
  .bDescriptorType      = USB_UVC_CS_ENDPOINT,
  .bDescriptorSubtype   = UVC_EP_INTERRUPT,
  .wMaxTransferSize     = 16
};

static struct usb_interface_descriptor g_fuvc_vs_intf_alt0 =
{
  .bLength              = sizeof(struct usb_interface_descriptor),
  .bDescriptorType      = UDESC_INTERFACE,
  .bInterfaceNumber     = 0x1,
  .bAlternateSetting    = 0,
#if FUVC_BULK_TO_ISO_CONVERT
  .bNumEndpoints        = 0,  /* Isoc must be 0 */
#else
  .bNumEndpoints        = 1,  /* Bulk must be 1 */
#endif
  .bInterfaceClass      = UICLASS_VIDEO,
  .bInterfaceSubClass   = UICLASS_VIDEO_STREAMING,
  .bInterfaceProtocol   = UICLASS_VIDEO_PROTOCOL_UNDEFINED,
  .iInterface           = UVC_STR_IDX_INTERFACE
};

/* UVC Example 1.5, section 2.3.5.1.2 */

static struct uvc_vs_header_descriptor g_fuvc_vs_head_desc =
{
  .bLength              = sizeof(struct uvc_vs_header_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_INPUT_HEADER,
  .bNumFormats          = 0x2 + ENABLE_H264, /* YUYV & H264 & MJPG */
  .wTotalLength         = 0,                 /* total length, set later */
  .bmInfo               = 0,                 /* do not support dynamic format change */
  .bTerminalLink        = UNIT_ID_OUTPUT,    /* terminal ID */
  .bStillCaptureMethod  = 0,                 /* do not support still image capture */
  .bTriggerSupport      = 0,                 /* do not support trigger */
  .bTriggerUsage        = 0,                 /* ignored */
  .bControlSize         = 1,
  .bmaControls          = { {0x00}, {0x00}, {0x00} }
};

static struct uvc_uncompressed_format_descriptor g_fuvc_format_yuv =
{
  .bLength              = sizeof(struct uvc_uncompressed_format_descriptor),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 0x1,      /* currently support only one format */
  .bNumFrameDescriptors = 0x6,      /* only one frame descriptor follows */
  .guidFormat           =
  {
    'Y', 'U', 'Y', '2', 0x00, 0x00, 0x10, 0x00, 0x80,
    0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
  },
  .bBitsPerPixel        = 16,       /* two pixels share 4 bytes? */
  .bDefaultFrameIndex   = 1,        /* optimum Frame Index */
  .bAspectRatioX        = 0,        /* No Aspect Ratio */
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,        /* No interlacing */
  .bCopyProtect         = 0         /* No copy protect */
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_144p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 1, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000 }
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_240p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 2, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000 }
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_288p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 3, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000 }
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_480p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 4, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwMaxVideoFrameBufferSize = 460800,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000 }
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_720p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 5, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 1843200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000}
};

static struct uvc_uncompressed_frame_descriptor3 g_fuvc_frame_yuv_1080p =
{
  .bLength                   = sizeof(struct uvc_uncompressed_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 6, /* only one frame descriptor */
  .bmCapabilities            = 0,
  .wWidth                    = 1920,
  .wHeight                   = 1080,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwMaxVideoFrameBufferSize = 1843200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3,
  .dwFrameInterval           = { 333333, 400000, 500000 }
};

static const struct uvc_color_matching_descriptor g_fuvc_color_matching_descriptor =
{
  .bLength                   = sizeof(struct uvc_color_matching_descriptor),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_COLOR_FORMAT,
  .bColorPrimaries           = UVC_COLOR_BT709_SRGB,
  .bTransferCharacteristics  = UVC_COLOR_BT709_SRGB,
  .bMatrixCoefficients       = UVC_COLOR_MC_SMPTE_170M
};

#if ENABLE_H264
static struct uvc_frame_based_format_descriptor g_fuvc_format_h264 =
{
  .bLength                   = sizeof(struct uvc_frame_based_format_descriptor),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FORMAT_FRAME_BASED,
  .bFormatIndex              = 2,
  .bNumFrameDescriptors      = 6,
  .guidFormat                =
  {
    'H', '2', '6', '4', 0x00, 0x00, 0x10, 0x00, 0x80,
    0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
  },
  .bBitsPerPixel             = 16,
  .bDefaultFrameIndex        = 1,
  .bAspectRatioX             = 0,
  .bAspectRatioY             = 0,
  .bmInterlaceFlags          = 0,
  .bCopyProtect              = 0,
  .bVariableSize             = 1
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_1080p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 6,
  .bmCapabilities            = 0,
  .wWidth                    = UVC_VIDEO_WIDTH_DEF,
  .wHeight                   = UVC_VIDEO_HEIGHT_DEF,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_720p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 5,
  .bmCapabilities            = 0,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 55296000,
  .dwMaxBitRate              = 55296000,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_480p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 4,
  .bmCapabilities            = 0,
  .wWidth                    = 640,
  .wHeight                   = 480,
  .dwMinBitRate              = 29491200,
  .dwMaxBitRate              = 29491200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_288p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 3,
  .bmCapabilities            = 0,
  .wWidth                    = 352,
  .wHeight                   = 288,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 29491200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_240p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 2,
  .bmCapabilities            = 0,
  .wWidth                    = 320,
  .wHeight                   = 240,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 29491200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};

static struct uvc_frame_based_frame_descriptor3 g_fuvc_frame_h264_144p =
{
  .bLength                   = sizeof(struct uvc_frame_based_frame_descriptor3),
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_FRAME_BASED,
  .bFrameIndex               = 1,
  .bmCapabilities            = 0,
  .wWidth                    = 176,
  .wHeight                   = 144,
  .dwMinBitRate              = 18432000,
  .dwMaxBitRate              = 29491200,
  .dwDefaultFrameInterval    = 333333,
  .bFrameIntervalType        = 3, /* number of supported Frame intervals */
  .dwBytesPerLine            = 0, /* must be 0 */
  .dwFrameInterval           = { 333333, 666667, 1000000 }
};
#endif

/************************ MJPG *******************************/

static struct uvc_format_mjpeg g_fuvc_format_mjpg =
{
  .bLength                = sizeof(struct uvc_format_mjpeg),
  .bDescriptorType        = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType     = USB_UVC_VS_FORMAT_MJPEG,
  .bFormatIndex           = 3,
  .bNumFrameDescriptors   = 6,
  .bmFlags                = 0,
  .bDefaultFrameIndex     = 1,
  .bAspectRatioX          = 0,
  .bAspectRatioY          = 0,
  .bmInterfaceFlags       = 0,
  .bCopyProtect           = 0
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_144p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 1,
  .bmCapabilities             = 0,
  .wWidth                     = 176,
  .wHeight                    = 144,
  .dwMinBitRate               = 18432000,
  .dwMaxBitRate               = 55296000,
  .dwMaxVideoFrameBufferSize  = 460800,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_240p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 2,
  .bmCapabilities             = 0,
  .wWidth                     = 320,
  .wHeight                    = 240,
  .dwMinBitRate               = 18432000,
  .dwMaxBitRate               = 55296000,
  .dwMaxVideoFrameBufferSize  = 460800,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_288p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 3,
  .bmCapabilities             = 0,
  .wWidth                     = 352,
  .wHeight                    = 288,
  .dwMinBitRate               = 18432000,
  .dwMaxBitRate               = 55296000,
  .dwMaxVideoFrameBufferSize  = 460800,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_480p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 4,
  .bmCapabilities             = 0,
  .wWidth                     = 640,
  .wHeight                    = 480,
  .dwMinBitRate               = 55296000,
  .dwMaxBitRate               = 55296000,
  .dwMaxVideoFrameBufferSize  = 460800,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_720p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 5,
  .bmCapabilities             = 0,
  .wWidth                     = 1280,
  .wHeight                    = 720,
  .dwMinBitRate               = 29491200,
  .dwMaxBitRate               = 29491200,
  .dwMaxVideoFrameBufferSize  = 1843200,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

static struct uvc_mjpg_frame_descriptor1 g_fuvc_frame_mjpg_1080p =
{
  .bLength                    = sizeof(struct uvc_mjpg_frame_descriptor1),
  .bDescriptorType            = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType         = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex                = 6,
  .bmCapabilities             = 0,
  .wWidth                     = 1920,
  .wHeight                    = 1080,
  .dwMinBitRate               = 29491200,
  .dwMaxBitRate               = 29491200,
  .dwMaxVideoFrameBufferSize  = 1843200,
  .dwDefaultFrameInterval     = 333333,
  .bFrameIntervalType         = 1,
  .dwFrameInterval            = {333333}
};

/************************ MJPG *******************************/

#if FUVC_BULK_TO_ISO_CONVERT
static const struct usb_interface_descriptor g_fuvc_isoc_streaming_intf_alt1 =
{
  .bLength              = sizeof(struct usb_interface_descriptor),
  .bDescriptorType      = UDESC_INTERFACE,
  .bInterfaceNumber     = 0x1,
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = UICLASS_VIDEO,
  .bInterfaceSubClass   = UICLASS_VIDEO_STREAMING,
  .bInterfaceProtocol   = UICLASS_VIDEO_PROTOCOL_UNDEFINED,
  .iInterface           = UVC_STR_IDX_INTERFACE
};

struct usb_endpoint_descriptor g_fuvc_hs_isoc_streaming_ep =
{
  .bLength              = sizeof(struct usb_endpoint_descriptor),
  .bDescriptorType      = UDESC_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .bEndpointAddress     = UE_DIR_IN | 0x2,
  .bmAttributes         = UE_ISO_ASYNC | UE_ISOCHRONOUS,
  HSETW(.wMaxPacketSize, 0x1400),
  .bInterval            = 1
};

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
static const struct usb_endpoint_ss_comp_descriptor g_fuvc_isoc_streaming_ss_comp_desc =
{
  .bLength              = sizeof(struct usb_endpoint_ss_comp_descriptor),
  .bDescriptorType      = 0x30,
  .bMaxBurst            = 0xe,
  .bmAttributes         = 2,
  HSETW(.wBytesPerInterval, 0xa800)
};
#endif
#else

/***********************BULK MODE***************************/

struct usb_endpoint_descriptor g_fuvc_hs_bulk_streaming_ep =
{
  .bLength             = USB_DT_ENDPOINT_SIZE,
  .bDescriptorType     = UDESC_ENDPOINT,

  /* Hi3516ev200 platform needs to specify endpoint number, otherwise the camera audio works abnormally.
   * This way is compatible with other platforms.
   */

  .bEndpointAddress    = UE_DIR_IN | 0x2,
  .bmAttributes        = USB_ENDPOINT_XFER_BULK,

  /* The wMaxPacketSize and bInterval values will be initialized from
   * module parameters.
   */

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  HSETW(.wMaxPacketSize, 0x0400),
#else
  HSETW(.wMaxPacketSize, 0x0200),
#endif
  .bInterval           = 0
};

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
static struct usb_endpoint_ss_comp_descriptor g_fuvc_bulk_streaming_ss_comp_desc =
{
  .bLength              = sizeof(struct usb_endpoint_ss_comp_descriptor),
  .bDescriptorType      = 0x30,
  .bMaxBurst            = 0x0,
  .bmAttributes         = 0,
  HSETW(.wBytesPerInterval, 0)
};
#endif
#endif

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#if FUVC_BULK_TO_ISO_CONVERT
#define DESCRIPTORS_NUM (39 + UVC_USE_CTRL_EP)
#else
#define DESCRIPTORS_NUM (38 + UVC_USE_CTRL_EP)
#endif
#else
#if FUVC_BULK_TO_ISO_CONVERT
#define DESCRIPTORS_NUM (37 + UVC_USE_CTRL_EP)
#else
#define DESCRIPTORS_NUM (36 + UVC_USE_CTRL_EP)
#endif
#endif

/***********************BULK MODE***************************/

static const uint8_t *g_fuvc_descriptors_array[DESCRIPTORS_NUM] =
{
  (const uint8_t *)&g_fuvc_config_desc,
  (const uint8_t *)&g_fuvc_iad,

/*******************video control***************************/

  (const uint8_t *)&g_fuvc_vc_intf_desc,
  (const uint8_t *)&g_fuvc_vc_head_desc,
  (const uint8_t *)&g_fuvc_vc_camera,
  (const uint8_t *)&g_fuvc_proc_desc,
#if ENABLE_H264
  (const uint8_t *)&g_fuvc_h264_ext_desc,
#endif
  (const uint8_t *)&g_fuvc_hicamera_ext_desc,

  (const uint8_t *)&g_fuvc_vc_output_desc,

#if UVC_USE_CTRL_EP
  (const uint8_t *)&g_fuvc_ctrl_ep_desc,
#endif
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_ctrl_ss_comp_desc,
#endif
  (const uint8_t *)&g_fuvc_int_ep_desc,

/*******************video stream***************************/

  (const uint8_t *)&g_fuvc_vs_intf_alt0,

#if FUVC_BULK_TO_ISO_CONVERT
  (const uint8_t *)&g_fuvc_isoc_streaming_intf_alt1,
  (const uint8_t *)&g_fuvc_hs_isoc_streaming_ep,
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_isoc_streaming_ss_comp_desc,
#endif
#else
  (const uint8_t *)&g_fuvc_hs_bulk_streaming_ep,
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  (const uint8_t *)&g_fuvc_bulk_streaming_ss_comp_desc,
#endif
#endif

  (const uint8_t *)&g_fuvc_vs_head_desc,

  (const uint8_t *)&g_fuvc_format_yuv,
  (const uint8_t *)&g_fuvc_frame_yuv_144p,
  (const uint8_t *)&g_fuvc_frame_yuv_240p,
  (const uint8_t *)&g_fuvc_frame_yuv_288p,
  (const uint8_t *)&g_fuvc_frame_yuv_480p,
  (const uint8_t *)&g_fuvc_frame_yuv_720p,
  (const uint8_t *)&g_fuvc_frame_yuv_1080p,

#if ENABLE_H264
  (const uint8_t *)&g_fuvc_format_h264,
  (const uint8_t *)&g_fuvc_frame_h264_144p,
  (const uint8_t *)&g_fuvc_frame_h264_240p,
  (const uint8_t *)&g_fuvc_frame_h264_288p,
  (const uint8_t *)&g_fuvc_frame_h264_480p,
  (const uint8_t *)&g_fuvc_frame_h264_720p,
  (const uint8_t *)&g_fuvc_frame_h264_1080p,
#endif

  (const uint8_t *)&g_fuvc_format_mjpg,
  (const uint8_t *)&g_fuvc_frame_mjpg_144p,
  (const uint8_t *)&g_fuvc_frame_mjpg_240p,
  (const uint8_t *)&g_fuvc_frame_mjpg_288p,
  (const uint8_t *)&g_fuvc_frame_mjpg_480p,
  (const uint8_t *)&g_fuvc_frame_mjpg_720p,
  (const uint8_t *)&g_fuvc_frame_mjpg_1080p,
  (const uint8_t *)&g_fuvc_color_matching_descriptor,
  NULL
};

#define FRAME_NUM_MAX   7

static struct fuvc_frame_info g_fuvc_frames_yuyv[FRAME_NUM_MAX] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};

static struct fuvc_frame_info g_fuvc_frames_mjpeg[FRAME_NUM_MAX] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};

#if ENABLE_H264
static struct fuvc_frame_info g_fuvc_frames_h264[FRAME_NUM_MAX] =
{
  { 176,  144,  { 333333,  0 }, },
  { 320,  240,  { 333333,  0 }, },
  { 352,  288,  { 333333,  0 }, },
  { 640,  480,  { 333333,  0 }, },
  { 1280, 720,  { 333333,  0 }, },
  { 1920, 1080, { 333333,  0 }, },
  { 0,    0,    { 0,         }, }
};
#endif

static struct fuvc_format_info g_fuvc_formats[] =
{
  { V4L2_PIX_FMT_YUYV, g_fuvc_frames_yuyv },
#if ENABLE_H264
  { V4L2_PIX_FMT_H264, g_fuvc_frames_h264 },
#endif
  { V4L2_PIX_FMT_MJPEG, g_fuvc_frames_mjpeg }
};

#define CLAMP(_val, _min, _max) (        \
{                                        \
  typeof(_val)__val = (_val);            \
  typeof(_min)__min = (_min);            \
  typeof(_max)__max = (_max);            \
  (void) (&__val == &__min);             \
  (void) (&__val == &__max);             \
  __val = __val < __min ? __min : __val; \
  __val > __max ? __max : __val;         \
})

#define ARRAY_SIZE(a)   ((sizeof(a) / sizeof(a[0])))

void fuvc_frame_descriptors_get(struct fuvc_format_info *format_info)
{
  uint32_t nframes;
  errno_t err;

  if (format_info == NULL)
    {
      PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
      return;
    }

  nframes = 0;

  while (format_info->frames[nframes].width != 0)
    {
      ++nframes;
    }

  if (nframes >= FRAME_NUM_MAX || nframes == 0)
    {
      PRINT_ERR("%s %d %u \n", __FUNCTION__, __LINE__, nframes);
      return;
    }

  if (format_info->fcc == V4L2_PIX_FMT_YUYV)
    {
      (void)memset_s(g_fuvc_frames_yuyv, sizeof(g_fuvc_frames_yuyv), 0, sizeof(g_fuvc_frames_yuyv));
      err = memcpy_s(g_fuvc_frames_yuyv, sizeof(g_fuvc_frames_yuyv),
                     format_info->frames, (sizeof(struct fuvc_frame_info) * nframes));
      if (err != EOK)
        {
          PRINT_ERR("%s %d Memcpy fail!\n", __FUNCTION__, __LINE__);
          return;
        }
    }
#if ENABLE_H264
  else if (format_info->fcc == V4L2_PIX_FMT_H264)
    {
      (void)memset_s(g_fuvc_frames_h264, sizeof(g_fuvc_frames_h264), 0, sizeof(g_fuvc_frames_h264));
      err = memcpy_s(g_fuvc_frames_h264, sizeof(g_fuvc_frames_h264),
                     format_info->frames, (sizeof(struct fuvc_frame_info) * nframes));
      if (err != EOK)
        {
          PRINT_ERR("%s %d Memcpy fail!\n", __FUNCTION__, __LINE__);
          return;
        }
    }
#endif
  else if (format_info->fcc == V4L2_PIX_FMT_MJPEG)
    {
      (void)memset_s(g_fuvc_frames_mjpeg, sizeof(g_fuvc_frames_mjpeg), 0, sizeof(g_fuvc_frames_mjpeg));
      err = memcpy_s(g_fuvc_frames_mjpeg, sizeof(g_fuvc_frames_mjpeg),
                     format_info->frames, (sizeof(struct fuvc_frame_info) * nframes));
      if (err != EOK)
        {
          PRINT_ERR("%s %d Memcpy fail!\n", __FUNCTION__, __LINE__);
          return;
        }
    }
  else
    {
      PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
      return;
    }
}

static void frames_descriptors_update(struct fuvc_format_info *format_info, uint32_t *index)
{
  uint32_t num = *index;
  uint8_t nframes = 0;
  const struct fuvc_format_info *pformat = format_info;

  while (pformat->frames[nframes].height != 0)
    {
      switch (pformat->frames[nframes].height)
        {
        case 144:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_144p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]   = (const uint8_t *)&g_fuvc_frame_yuv_144p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_144p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_h264_144p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_144p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_mjpg_144p;
            }
          break;

        case 240:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_240p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]   = (const uint8_t *)&g_fuvc_frame_yuv_240p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_240p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_h264_240p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_240p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_mjpg_240p;
            }
          break;

        case 288:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_288p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]   = (const uint8_t *)&g_fuvc_frame_yuv_288p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_288p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_h264_288p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_288p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_mjpg_288p;
            }
          break;

        case 480:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_480p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]   = (const uint8_t *)&g_fuvc_frame_yuv_480p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_480p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_h264_480p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_480p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_mjpg_480p;
            }
          break;

        case 720:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_720p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]   = (const uint8_t *)&g_fuvc_frame_yuv_720p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_720p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_h264_720p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_720p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_mjpg_720p;
            }
          break;

        case 1080:
          if (pformat->fcc == V4L2_PIX_FMT_YUYV)
            {
              g_fuvc_frame_yuv_1080p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]    = (const uint8_t *)&g_fuvc_frame_yuv_1080p;
            }
#if ENABLE_H264
          else if (pformat->fcc == V4L2_PIX_FMT_H264)
            {
              g_fuvc_frame_h264_1080p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]     = (const uint8_t *)&g_fuvc_frame_h264_1080p;
            }
#endif
          else
            {
              g_fuvc_frame_mjpg_1080p.bFrameIndex = nframes + 1;
              g_fuvc_descriptors_array[num++]     = (const uint8_t *)&g_fuvc_frame_mjpg_1080p;
            }
          break;

        default:
          PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
        }

      nframes++;
    }

  *index = num;
}

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#if FUVC_BULK_TO_ISO_CONVERT
#define DESC_UPDATE_INDEX (16 + UVC_USE_CTRL_EP)
#else
#define DESC_UPDATE_INDEX (15 + UVC_USE_CTRL_EP)
#endif
#else
#if FUVC_BULK_TO_ISO_CONVERT
#define DESC_UPDATE_INDEX (14 + UVC_USE_CTRL_EP)
#else
#define DESC_UPDATE_INDEX (13 + UVC_USE_CTRL_EP)
#endif
#endif

static void fuvc_descriptors_update(void)
{
  uint32_t index;
  uint8_t nframes = 0;
  struct fuvc_format_info *pformat;

  for (index = DESC_UPDATE_INDEX; index < DESCRIPTORS_NUM; index++)
    {
      g_fuvc_descriptors_array[index] = NULL;
    }

  /**************** yuyv format ***************/

  pformat = &g_fuvc_formats[UVC_VFF_YUY2];
  while (pformat->frames[nframes].width != 0)
    {
      ++nframes;
    }
  g_fuvc_format_yuv.bNumFrameDescriptors = nframes;

  index = DESC_UPDATE_INDEX;
  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_yuv;

  frames_descriptors_update(pformat, &index);
#if ENABLE_H264

  /**************** h264 format ***************/

  nframes = 0;
  pformat = &g_fuvc_formats[UVC_VFF_H264];
  while (pformat->frames[nframes].width != 0)
    {
      ++nframes;
    }
  g_fuvc_format_h264.bNumFrameDescriptors = nframes;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_h264;

  frames_descriptors_update(pformat, &index);
#endif

  /**************** mjpg format ***************/

  nframes = 0;
  pformat = &g_fuvc_formats[UVC_VFF_MJPG];
  while (pformat->frames[nframes].width != 0)
    {
      ++nframes;
    }
  g_fuvc_format_mjpg.bNumFrameDescriptors = nframes;

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_format_mjpg;

  frames_descriptors_update(pformat, &index);

  if (g_fuvc_descriptors_array[index] != NULL)
    {
      PRINT_ERR("%s %d\n", __FUNCTION__, __LINE__);
      return;
    }

  g_fuvc_descriptors_array[index++] = (const uint8_t *)&g_fuvc_color_matching_descriptor;
  return;
}

/*
 * process the descriptors, link them together
 */

#define DESC_ARRAY_INDEX (DESC_UPDATE_INDEX - 1)
uint8_t *link_fuvc_descriptors(uint8_t *prefer, uint16_t ps, uint16_t *total_size)
{
  int i;
  uint8_t *des;
  uint8_t *pdes;
  uint16_t ds = 0;
  uint16_t cs;
  errno_t err;

  (void)prefer;
  (void)ps;

  fuvc_descriptors_update();

  /* Add the length of descriptors one by one */

  for (i = 0; g_fuvc_descriptors_array[i] != NULL; ++i)
    {
      ds += (uint16_t)(*g_fuvc_descriptors_array[i]);
    }

  FUVC_DEBUG("Total Length of descriptors: %u i=%d\n", ds, i);
  if (total_size != NULL)
    {
      *total_size = ds;
    }

  des = memalign(64, SKB_DATA_ALIGN(ds));
  if (des == NULL)
    {
      usb_err("System out of memory! Descriptors length: %u\n", ds);
      return NULL;
    }
  (void)memset_s(des, SKB_DATA_ALIGN(ds), 0, SKB_DATA_ALIGN(ds));
  pdes = des;

  /* configuration descriptor needs to have the full length of rest of descriptors */

  g_fuvc_config_desc.wTotalLength[0] = (uint8_t)ds;
  g_fuvc_config_desc.wTotalLength[1] = (uint8_t)(ds >> 8);

  /* Add the length of video control descriptors */

  cs  = (uint32_t)g_fuvc_vc_head_desc.bLength;
  cs += (uint32_t)g_fuvc_vc_camera.bLength;
  cs += (uint32_t)g_fuvc_proc_desc.bLength;
  cs += (uint32_t)g_fuvc_vc_output_desc.bLength;

#if ENABLE_H264
  cs += (uint32_t)g_fuvc_h264_ext_desc.bLength;
#endif
  cs += (uint32_t)g_fuvc_hicamera_ext_desc.bLength;
  g_fuvc_vc_head_desc.wTotalLength = (uint16_t)cs;

  ds = 0;
  for (i = DESC_ARRAY_INDEX; g_fuvc_descriptors_array[i] != NULL; i++)
    {
      ds += (uint16_t)*(g_fuvc_descriptors_array[i]);
    }

  g_fuvc_vs_head_desc.wTotalLength = (uint16_t)ds;
#if FUVC_BULK_TO_ISO_CONVERT
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_isoc_streaming_ep.bEndpointAddress;
#else
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_bulk_streaming_ep.bEndpointAddress;
#endif

  for (i = 0; g_fuvc_descriptors_array[i] != NULL; i++)
    {
      const u8 *des_src = g_fuvc_descriptors_array[i];
      u8 des_len = *des_src;
      err = memcpy_s(pdes, des_len, des_src, des_len);
      if (err != EOK)
        {
          usb_err("memcpy fail!\n");
          free(des);
          return NULL;
        }
      pdes += des_len;
    }

  return des;
}

#define FUVC_FUNCNAME_SIZE   32
static const char *func_msg = "Entering function [%s]\n";

int fuvc_host_connected(struct uvc_dev_s *fuvc)
{
  if (fuvc == NULL)
    {
      return 0;
    }

  return fuvc->connected;
}

static void fuvc_vs_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req);

int fuvc_transfer_initiate(struct uvc_dev_s *fuvc)
{
  struct usbdev_req_s *req;
  struct usbdev_ep_s *vs_ep;

  fuvc->fid  ^= 1;
  vs_ep       = fuvc->vs_ep;
  req         = &(fuvc->streamreq);
  req->result = 0;

  fuvc_vs_req_complete(vs_ep, req);

  return UVC_OK;
}

static void set_probe_status(struct uvc_dev_s *fuvc, int cs, int req)
{
  if (cs == 0x01)
    {
      switch (req)
        {
        case 0x01:
          {
            fuvc->probe_status.set = 1;
          }
          break;

        case 0x81:
          {
            fuvc->probe_status.get = 1;
          }
          break;

        case 0x82:
          {
            fuvc->probe_status.min = 1;
          }
          break;

        case 0x83:
          {
            fuvc->probe_status.max = 1;
          }
          break;

        default:
          break;
        }
    }
}

static int check_probe_status(struct uvc_dev_s *fuvc)
{
  if (fuvc->probe_status.get == 1 &&
      fuvc->probe_status.set == 1 &&
      fuvc->probe_status.min == 1 &&
      fuvc->probe_status.max == 1)
    {
      return 1;
    }

  return 0;
}

uint32_t g_com_flag = 0;
volatile uint32_t g_start_transfer = 0;

static void fuvc_streaming_set(struct uvc_dev_s *fuvc,
                               struct usbdev_req_s *req,
                               const struct usb_device_request *ctrl)
{
  struct uvc_probe_commit_control1_1 *target;
  struct uvc_probe_commit_control1_1 com;
  struct uvc_probe_commit_control1_1 *pcom;
  const struct fuvc_format_info *pformat;
  const struct fuvc_frame_info *frame;
  const uint32_t *interval;
  uint8_t iformat, iframe, nframes;

  (void)ctrl;

  switch (fuvc->control)
    {
    case USB_UVC_VS_PROBE_CONTROL:
      if (g_com_flag == 0)
        {
          return;
        }
      g_com_flag = 0;

      target = &fuvc->probe;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      target = &fuvc->commit;
      break;

    default:
      usb_err("setting unknown control\n");
      return;
    }

  pcom = (struct uvc_probe_commit_control1_1 *)(req->buf);
  com.bFormatIndex    = pcom->bFormatIndex;
  com.bFrameIndex     = pcom->bFrameIndex;
  com.dwFrameInterval = pcom->dwFrameInterval;

  iformat = CLAMP((uint32_t)com.bFormatIndex, 1U,
                  (uint32_t)ARRAY_SIZE(g_fuvc_formats));

  pformat = &g_fuvc_formats[iformat - 1];

  nframes = 0;

  while (pformat->frames[nframes].width != 0)
    {
      ++nframes;
    }

  iframe   = CLAMP((uint32_t)com.bFrameIndex, 1U, (uint32_t)nframes);
  frame    = &pformat->frames[iframe - 1];
  interval = frame->intervals;

  while (interval[0] < com.dwFrameInterval && interval[1])
    {
      ++interval;
    }

  target->bFormatIndex = iformat;
  target->bFrameIndex  = iframe;

  switch (pformat->fcc)
    {
    case V4L2_PIX_FMT_YUYV:
      target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
      break;

    case V4L2_PIX_FMT_MJPEG:
    case V4L2_PIX_FMT_H264:
      if (fuvc->imgsize == 0)
        {
          dprintf("WARNING: MJPEG requested and no image loaded.\n");
        }

      target->dwMaxVideoFrameSize = 1843200;
      break;

    default:
      break;
    }

  target->dwMaxPayloadTransferSize = STREAM_BUF_SIZE;
  target->dwFrameInterval          = interval[0];
  if (fuvc->control == USB_UVC_VS_COMMIT_CONTROL && check_probe_status(fuvc))
    {
      fuvc->format_info.format = pformat->fcc;
      fuvc->format_info.width  = frame->width;
      fuvc->format_info.height = frame->height;
      fuvc->format_info.status = FORMAT_SWITCH_PENDING;
      fuvc->connected          = 0x1;
      g_start_transfer         = 1;
      fuvc->transfer_status    = STREAM_ON;
      if (!LOS_ListEmpty(&g_uvc_event.stEventList))
        {
          (void)LOS_EventWrite(&g_uvc_event, 0x01);
        }
    }

  if (fuvc->control == USB_UVC_VS_COMMIT_CONTROL)
    {
      (void)memset_s(&fuvc->probe_status, sizeof(fuvc->probe_status), 0, sizeof(fuvc->probe_status));
    }
}

static void fuvc_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uvc_dev_s *fuvc;

  (void)ep; /* ignored */

  fuvc = (struct uvc_dev_s *)req->priv;
  switch (fuvc->control)
    {
    case ~0u:
      break;

    case USB_UVC_VS_PROBE_CONTROL:
      fuvc_streaming_set(fuvc, req, NULL);
      fuvc->control = ~0;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      fuvc_streaming_set(fuvc, req, NULL);
      fuvc->control = ~0;
      break;

    default:
      fuvc->control = ~0;
      break;
    }
}

#if UVC_USE_CTRL_EP
static void fuvc_vc_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
  dprintf(func_msg, __FUNCTION__);
}
#endif

static int fuvc_vs_req_complete_sub(struct uvc_dev_s *fuvc, struct uvc_isoc_transfer *transfer,
                                    struct usbdev_req_s *req)
{
  uint8_t *temp_buf;
  int ret;

  if (req->result)
    {
      usb_err("req status is %d\n", req->result);
      req->result = 0;
      return -1;
    }

  if (fuvc->transfer_status == STREAM_OFF)
    {
      if (!LOS_ListEmpty(&g_uvc_event.stEventList))
        {
          if (LOS_EventWrite(&g_uvc_event, 0x01) == LOS_OK)
            {
              /* Write OK */

            }
        }
      return -1;
    }

  temp_buf = fuvc->stream_buf;

  /* The first two bytes of the uvc frame are the protocol headers. */

  transfer->data      = &(temp_buf[0x2]);
  transfer->length    = STREAM_BUF_SIZE - 2;
  transfer->reserved  = 0x2;
  transfer->res_next  = 0x2;

  transfer->last      = 0;
  ret = uvc_continue_transfer(fuvc, transfer);
  if (ret != UVC_OK)
    {
      /* Transfer should discontinue */

      FUVC_DEBUG("Fatal error, uvc_continue_transfer(...): %d\n", ret);
      return -1;
    }

  return 0;
}

#if FUVC_BULK_TO_ISO_CONVERT
static void fuvc_vs_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  int ret;
  uint8_t *temp_buf;
  struct uvc_dev_s *fuvc;
  struct uvc_isoc_transfer transfer;
  fuvc = (struct uvc_dev_s *)ep->priv;

  ret = fuvc_vs_req_complete_sub(fuvc, &transfer, req);
  if (ret)
    {
      usb_err("data init failed!\n");
      return;
    }

  if (transfer.length == 0)
    {
      (void)LOS_EventWrite(&g_uvc_event, 0x01);
      return;
    }

  temp_buf = fuvc->stream_buf;
  req->buf = (uint8_t *)VMM_TO_DMA_ADDR((UINTPTR)temp_buf);
  req->len = transfer.length + 0x2;

  temp_buf[0] = 0x2;
  if (transfer.last)
    {
      temp_buf[1] = (uint8_t)(UVC_STREAM_HEADER_EOH |
                             (fuvc->fid & UVC_STREAM_HEADER_FID) | UVC_STREAM_HEADER_EOF);
    }
  else
    {
      temp_buf[1] = (uint8_t)(UVC_STREAM_HEADER_EOH | (fuvc->fid & UVC_STREAM_HEADER_FID));
    }

  fuvc->last_complete = LOS_TickCountGet();
  (void)EP_SUBMIT(ep, req);
}
#else
static void fuvc_vs_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  int ret;
  uint8_t *temp_buf;
  struct uvc_dev_s *fuvc;
  struct uvc_isoc_transfer transfer;
  fuvc = (struct uvc_dev_s *)ep->priv;

  ret = fuvc_vs_req_complete_sub(fuvc, &transfer, req);
  if (ret)
    {
      usb_err("data init failed!\n");
      return;
    }

  if (fuvc->dyn_fc > 0x1)
    {
      /*
       * UVC Device Initated Dynamic Format Change Support
       * Refer to UVC spec. 1.5, section 2.4.3.6 for the details
       */
    }

  temp_buf = fuvc->stream_buf;
  if (transfer.length == 0)
    {
      if (fuvc->zero_packet_flag)
        {
          fuvc->zero_packet_flag = 0;
          req->buf               = (uint8_t *)VMM_TO_DMA_ADDR((UINTPTR)temp_buf);
          req->len               = transfer.length;
          (void)EP_SUBMIT(ep, req);
          return;
        }

      (void)LOS_EventWrite(&g_uvc_event, 0x01);

      return;
    }
  req->buf    = (uint8_t *)VMM_TO_DMA_ADDR((UINTPTR)temp_buf);
  req->len    = transfer.length + 0x2;
  temp_buf[0] = 0x2;
  temp_buf[1] = (uint8_t)(UVC_STREAM_HEADER_EOH | (fuvc->fid & UVC_STREAM_HEADER_FID));

  if (transfer.last && ((req->len % UGETW(ep->maxpacket)) == 0))
    {
      fuvc->zero_packet_flag = 1;
    }

  if (transfer.last)
    {
      if (fuvc->dyn_fc)
        {
          fuvc->dyn_fc++;
        }
    }

  fuvc->last_complete = LOS_TickCountGet();
  (void)EP_SUBMIT(ep, req);
}
#endif

void fuvc_fill_streaming_control(struct uvc_dev_s *fuvc,
                                 struct uvc_probe_commit_control1_1 *ctrl,
                                 int iframe, int iformat)
{
  const struct fuvc_format_info *pformat;
  const struct fuvc_frame_info *frame;
  uint32_t nframes;

  if (iformat < 0)
    {
      iformat = ARRAY_SIZE(g_fuvc_formats) + iformat;
    }

  if (iformat < 0 || iformat >= (int)ARRAY_SIZE(g_fuvc_formats))
    {
      return;
    }

  pformat = &g_fuvc_formats[iformat];

  nframes = 0;

  while (pformat->frames[nframes].width != 0)
    {
      ++nframes;
    }

  if (iframe < 0)
    {
      iframe = nframes + iframe;
    }

  if (iframe < 0 || iframe >= (int)nframes)
    {
      return;
    }

  frame = &pformat->frames[iframe];

  (void)memset_s(ctrl, sizeof(struct uvc_probe_commit_control1_1),
                 0, sizeof(struct uvc_probe_commit_control1_1));

  ctrl->bmHint          = 1;
  ctrl->bFormatIndex    = iformat + 1; /* 1 is yuv, 2 is mjpeg */
  ctrl->bFrameIndex     = iframe + 1;  /* 360 1 720 2 */
  ctrl->dwFrameInterval = frame->intervals[0];

  switch (pformat->fcc)
    {
    case V4L2_PIX_FMT_YUYV:
      ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
      break;

    case V4L2_PIX_FMT_MJPEG:
    case V4L2_PIX_FMT_H264:
      ctrl->dwMaxVideoFrameSize = 1843200;
      break;

    default:
        break;
    }

  ctrl->dwMaxPayloadTransferSize = STREAM_BUF_SIZE;
  ctrl->bmFramingInfo            = 3;
  ctrl->bPreferedVersion         = 1;
  ctrl->bMaxVersion              = 1;
}

static void fuvc_get_default_streaming_ctrl(struct uvc_dev_s *fuvc,
                                            struct uvc_probe_commit_control1_1 *ctrl, uint32_t fint)
{
  fuvc_fill_streaming_control(fuvc, ctrl, 0, 0);
}

static void fuvc_control_unit_callback(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct uvc_dev_s *fuvc;
  int ret;

  (void)ep;

  fuvc = (struct uvc_dev_s *)req->priv;

  ret = run_cmd_func(req->buf, req->xfrd, fuvc->event_id, UVC_RC_SETCUR, fuvc->unit_id);
  if (ret < 0)
    {
      usb_err("run_cmd_func return error!\n");
    }
}

static int fuvc_handle_camera_control(struct uvc_dev_s *fuvc,
                                      struct usbdev_req_s *req,
                                      const struct usb_device_request *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  (void)fuvc; /* ignored for now */
  (void)req;
  w_value = UGETW(ctrl->wValue);

  /* The high byte of wValue contains the Control Selector
   * Refer to UVC spec. 1.5, section 4.1.1 & 4.1.2
   */

  switch (ctrl->bRequest)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_CAMERA;
      break;

    default:
      ret = run_cmd_func(req->buf, req->xfrd, w_value >> 8, ctrl->bRequest, UNIT_ID_CAMERA);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

static int fuvc_handle_process_control(struct uvc_dev_s *fuvc,
                                       struct usbdev_req_s *req, const struct usb_device_request *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  (void)fuvc;
  (void)req;
  w_value = UGETW(ctrl->wValue);

  switch (ctrl->bRequest)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_PROCESSING;
      break;

    default:
      ret = run_cmd_func(req->buf, req->xfrd, w_value >> 8, ctrl->bRequest, UNIT_ID_PROCESSING);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

#if ENABLE_H264

/* We have two UVC specifications, version 1.1 & 1.5
 * The latter added H264 stream format/frame definition,
 * but the former supports H264 stream via extension unit.
 * After careful consideration, I think we'd better support
 * version 1.1, because it's better supported by windows 7
 */

static int fuvc_handle_ext_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                   const struct usb_device_request *ctrl)
{
  uint16_t w_value;
  int ret = 0;

  w_value = UGETW(ctrl->wValue);

  switch (ctrl->bRequest)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_H264_EXTENSION;
      break;

    default:
      ret = run_cmd_func(req->buf, req->xfrd, w_value >> 8, ctrl->bRequest, UNIT_ID_H264_EXTENSION);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}
#endif

static int fuvc_handle_xu_hicamera_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                           const struct usb_device_request *ctrl)
{
  uint16_t w_value = UGETW(ctrl->wValue);
  int ret = 0;

  switch (ctrl->bRequest)
    {
    case UVC_RC_SETCUR:
      req->callback  = fuvc_control_unit_callback;
      fuvc->event_id = w_value >> 8;
      fuvc->unit_id  = UNIT_ID_HICAMERA_EXTENSION;
      break;

    default:
      ret = run_cmd_func(req->buf, req->xfrd, w_value >> 8, ctrl->bRequest, UNIT_ID_HICAMERA_EXTENSION);
      if (ret < 0)
        {
          usb_err("run_cmd_func return error!\n");
        }
      break;
    }

  return ret;
}

static void fuvc_handle_class_setup_control(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                            const struct usb_device_request *ctrl)
{
  int retval;
  errno_t err;
  uint16_t n_len;
  uint16_t w_length = UGETW(ctrl->wLength);
  uint16_t w_index  = UGETW(ctrl->wIndex);
  uint16_t w_value  = UGETW(ctrl->wValue);

  retval = 0;
  err    = memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, 64);
  if (err != EOK)
    {
      usb_err("memset fail\n");
    }

  n_len = 32;
  if (n_len > w_length)
    {
      n_len = w_length;
    }

  /* The high byte of wIndex indicates which entity the request addresses.
   * Refer to UVC spec. 1.5, section 4.1.1 & 4.1.2
   */

  switch (w_index >> 8)
    {
    case UNIT_ID_CAMERA:
      retval = fuvc_handle_camera_control(fuvc, req, ctrl);
      break;

    case UNIT_ID_PROCESSING:
      retval = fuvc_handle_process_control(fuvc, req, ctrl);
      break;

#if ENABLE_H264
    case UNIT_ID_H264_EXTENSION:
      retval = fuvc_handle_ext_control(fuvc, req, ctrl);
      break;
#endif

    case UNIT_ID_HICAMERA_EXTENSION:
      retval = fuvc_handle_xu_hicamera_control(fuvc, req, ctrl);
      break;

    default:
      dprintf("Control Requests not supported, index: %#x, w_value: %#x\n", w_index, w_value);
      break;
    }

  if (retval <= 0)
    {
      req->len = n_len;
    }
  else
    {
      req->len = (unsigned)retval;
    }
}

static int fuvc_handle_streaming_probe(struct uvc_dev_s *fuvc, struct usbdev_req_s *req,
                                       const struct usb_device_request *ctrl)
{
  int retval;
  uint8_t breq;
  errno_t errnum;
  const char *errmsg = "UVC Streaming Probe: %s\n";
  struct uvc_probe_commit_control1_1 commit1_1;

  retval = 0;
  breq   = ctrl->bRequest;
  switch (breq)
    {
    case UVC_RC_SETCUR:
      g_com_flag = 1;
      retval     = 1;
      break;

    case UVC_RC_SETCUR_ALL:
      dprintf(errmsg, "UVC_RC_SETCUR_ALL");
      break;

    case UVC_RC_GETINFO:
      dprintf(errmsg, "UVC_RC_GETINFO");
      break;

    case UVC_RC_GETCUR:
      if ((UGETW(ctrl->wValue) >> 8) == USB_UVC_VS_PROBE_CONTROL)
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                            &fuvc->probe, SKB_DATA_ALIGN(sizeof(struct uvc_probe_commit_control1_1)));
        }
      else
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                            &fuvc->commit, SKB_DATA_ALIGN(sizeof(struct uvc_probe_commit_control1_1)));
        }

      if (errnum != EOK)
        {
          usb_err("memcpy fail %d\n", errnum);
          break;
        }

      retval = 0;
      break;

    case UVC_RC_GETMIN:
    case UVC_RC_GETMAX:
    case UVC_RC_GETRES:
    case UVC_RC_GETLEN:
    case UVC_RC_GETDEF:
    case UVC_RC_GETCUR_ALL:
    case UVC_RC_GETMIN_ALL:
    case UVC_RC_GETMAX_ALL:
    case UVC_RC_GETRES_ALL:
    case UVC_RC_GETDEF_ALL:
      fuvc_get_default_streaming_ctrl(fuvc, &commit1_1, 333333);
      errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                        (void *)&commit1_1, SKB_DATA_ALIGN(sizeof(struct uvc_probe_commit_control1_1)));
      if (errnum != EOK)
        {
          usb_err("memcpy fail %d\n", errnum);
          break;
        }

      retval = 0;
      break;

    default:
      usb_err("Fatal Error, un-supported request from host: %#x\n", breq);
      break;
    }

  return retval;
}

static int fuvc_handle_class_setup_streaming(struct uvc_dev_s *fuvc,
                                             struct usbdev_req_s *req, const struct usb_device_request *ctrl)
{
  int ret;
  errno_t errnum;
  const char * errmsg = NULL;
  uint16_t n_len, w_length, w_value;

  ret   = 0;
  n_len = (uint16_t)sizeof(struct uvc_probe_commit_control1_1);

  w_value  = UGETW(ctrl->wValue);
  w_length = UGETW(ctrl->wLength);

  switch (w_value >> 8)
    {
    case USB_UVC_VS_PROBE_CONTROL:
      fuvc->control = USB_UVC_VS_PROBE_CONTROL;
      ret = fuvc_handle_streaming_probe(fuvc, req, ctrl);
      if (ret)
        {
          errnum = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, &fuvc->probe, SKB_DATA_ALIGN(n_len));
          if (errnum != EOK)
            {
              usb_err("memcpy fail, %d\n", errnum);
              break;
            }
        }

      ret = 1;
      break;

    case USB_UVC_VS_COMMIT_CONTROL:
      fuvc->control = USB_UVC_VS_COMMIT_CONTROL;
      (void)fuvc_handle_streaming_probe(fuvc, req, ctrl);
      ret = 1;
      break;

    case USB_UVC_VS_STILL_PROBE_CONTROL:
      errmsg = "USB_UVC_VS_STILL_PROBE_CONTROL";
      break;

    case USB_UVC_VS_STILL_COMMIT_CONTROL:
      errmsg = "USB_UVC_VS_STILL_COMMIT_CONTROL";
      break;

    case USB_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL:
      errmsg = "USB_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL";
      break;

    case USB_UVC_VS_STREAM_ERROR_CODE_CONTROL:
      errmsg = "USB_UVC_VS_STREAM_ERROR_CODE_CONTROL";
      break;

    case USB_UVC_VS_GENERATE_KEY_FRAME_CONTROL:
      errmsg = "USB_UVC_VS_GENERATE_KEY_FRAME_CONTROL";
      break;

    case USB_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL:
      errmsg = "USB_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL";
      break;

    case USB_UVC_VS_SYNCH_DELAY_CONTROL:
      errmsg = "USB_UVC_VS_SYNCH_DELAY_CONTROL";
      break;

    default:
      dprintf("Invalid streaming control request: %#x\n", w_value);
      break;
    }

  if (errmsg != NULL)
    {
      dprintf("UVC stream control %s not supported!\n", errmsg);
    }
  if (ret)
    {
      req->len = (n_len > w_length) ? w_length : n_len;
    }
  return ret;
}

static void fuvc_source_free(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct uvc_driver_s *drvr = (struct uvc_driver_s *)driver;
  struct uvc_dev_s *fuvc    = drvr->dev;

  (void)EP_DISABLE(fuvc->ctrl_ep);
  (void)EP_DISABLE(fuvc->vs_ep);

  DEV_FREEEP(dev, fuvc->ctrl_ep);
  DEV_FREEEP(dev, fuvc->vs_ep);

  (void)LOS_EventDestroy(&g_uvc_event);

  /* free allocated stream buffer pointer */

  if (fuvc->stream_buf != NULL)
    {
      free(fuvc->stream_buf);
      fuvc->stream_buf = NULL;
    }
}

static int usbclass_uvc_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  int ret;
  struct usbdev_ep_s *ep;
  struct uvc_driver_s *uvc_drvr;
  struct uvc_dev_s *uvc_dev;
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }
  uvc_drvr = (struct uvc_driver_s *)driver;
  uvc_dev  = uvc_drvr->dev;
  if (uvc_dev == NULL)
    {
      return -1;
    }

  devobj = usbclass_devobj_get(cdev, DEV_UVC);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(&uvc_dev->format_info, sizeof(struct uvc_format_info), 0, sizeof(struct uvc_format_info));

#if UVC_USE_CTRL_EP

  /* initialize control endpoint */

  ep = DEV_ALLOCEP(dev, g_fuvc_ctrl_ep_desc.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fuvc_ctrl_ep_desc);
  if (ep == NULL)
    {
      goto fail;
    }
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  ep->comp_desc = &g_fuvc_ctrl_ss_comp_desc;
#endif
  ep->priv = (void *)uvc_dev;
  (void)memset_s(&(uvc_dev->vc_ctrlreq), sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uvc_dev->vc_ctrlreq.callback = fuvc_vc_request_complete;
  uvc_dev->vc_ctrlreq.priv     = (void *)uvc_dev;
  ep->handle_req               = &uvc_dev->vc_ctrlreq;
  uvc_dev->ctrl_ep             = ep;
  devinfo->epno[0]             = ep->eplog;
#endif

  /* initialize VideoStreaming endpoint */

#if FUVC_BULK_TO_ISO_CONVERT
  ep = DEV_ALLOCEP(dev, g_fuvc_hs_isoc_streaming_ep.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fuvc_hs_isoc_streaming_ep);
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_isoc_streaming_ep.bEndpointAddress;
#else
  ep = DEV_ALLOCEP(dev, g_fuvc_hs_bulk_streaming_ep.bEndpointAddress,
                   (struct usb_endpoint_descriptor *)&g_fuvc_hs_bulk_streaming_ep);
  g_fuvc_vs_head_desc.bEndpointAddress = g_fuvc_hs_bulk_streaming_ep.bEndpointAddress;
#endif
  if (ep == NULL)
    {
      goto fail;
    }

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#if FUVC_BULK_TO_ISO_CONVERT
  ep->comp_desc = &g_fuvc_isoc_streaming_ss_comp_desc;
#else
  ep->comp_desc = &g_fuvc_bulk_streaming_ss_comp_desc;
#endif
#endif
  (void)memset_s(&uvc_dev->streamreq, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  uvc_dev->streamreq.callback = fuvc_vs_req_complete;
  uvc_dev->streamreq.priv     = (void *)uvc_dev;
  uvc_dev->streamreq.buf      = NULL;

  uvc_dev->stream_buf = (void *)memalign(USB_CACHE_ALIGN_SIZE, STREAM_BUF_SIZE);
  if (uvc_dev->stream_buf == NULL)
    {
      PRINT_ERR("No memory at line: %d!\n", __LINE__);
      goto fail;
    }
  ep->priv         = (void *)uvc_dev;
  ep->handle_req   = &uvc_dev->streamreq;
  uvc_dev->vs_ep   = ep;
  devinfo->epno[1] = ep->eplog;

  /* set the descriptors */

  uvc_dev->control    = ~0;
  uvc_dev->uvc_handle = (void *)UVC_INVALID_HANDLE;
  spin_lock_init(&uvc_dev->lock);
  (void)LOS_EventInit(&g_uvc_event);

  ret = uvc_stream_init(uvc_dev);
  if (ret != 0)
    {
      goto fail;
    }

  uvc_dev->transfer_status = STREAM_OFF;

  uvc_unit_control_register();

  return 0;
fail:
  (void)usbclass_uvc_unbind(driver, dev);
  return -1;
}

static int usbclass_uvc_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct uvc_driver_s *drvr;
  struct uvc_dev_s *fuvc;
  uint32_t ret;

  if (driver == NULL || dev == NULL)
    {
      PRINT_ERR("fuvc_unbind_gadget failed!\n");
      return -1;
    }

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return -1;
    }

  ret = LOS_MuxPend(g_uvc_mutex, LOS_WAIT_FOREVER);
  if (ret != LOS_OK)
    {
      return -1;
    }

  if (uvc_is_running())
    {
      (void)LOS_MuxPost(g_uvc_mutex);
      usb_err("uvc busy, unbind fail\n");
      return -1;
    }

  hi_camera_cmd_init();
  usbclass_uvc_disconnect(driver, dev);

  cdev = dev->ep0->priv;
  if (cdev == NULL)
    {
      return -1;
    }
  devobj = usbclass_devobj_get(cdev, DEV_UVC);
  if (devobj == NULL)
    {
      (void)LOS_MuxPost(g_uvc_mutex);
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));
  fuvc_source_free(driver, dev);
  (void)LOS_MuxPost(g_uvc_mutex);
  uvc_delete_mutex();
  return 0;
}

void usbclass_uvc_stream_ep_reset(struct uvc_dev_s *fuvc)
{
  (void)EP_DISABLE(fuvc->vs_ep);
  (void)EP_FLUSH(fuvc->vs_ep);
#if FUVC_BULK_TO_ISO_CONVERT
  (void)EP_CONFIGURE(fuvc->vs_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_hs_isoc_streaming_ep, 0);
#else
  (void)EP_CONFIGURE(fuvc->vs_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_hs_bulk_streaming_ep, 0);
#endif
}

#if FUVC_BULK_TO_ISO_CONVERT
static void fuvc_streaming_on(struct uvc_dev_s *fuvc)
{
  struct usbdev_req_s *req;

  fuvc->connected       = 1;
  g_start_transfer      = 1;
  fuvc->transfer_status = STREAM_ON;

  req           = &(fuvc->streamreq);
  req->len      = 0;
  req->buf      = (uint8_t *)VMM_TO_DMA_ADDR((UINTPTR)fuvc->stream_buf);
  req->callback = fuvc->streamreq.callback;
  (void)EP_SUBMIT(fuvc->vs_ep, req);

  return;
}

static void fuvc_streaming_off(struct uvc_dev_s *fuvc)
{
  fuvc->transfer_status = STREAM_OFF;
  g_start_transfer      = 0;
  fuvc->connected       = 0;
}

static int usbclass_uvc_set_alt(struct uvc_dev_s *fuvc, struct usbdev_s *dev,
                                unsigned intf, unsigned alt)
{
  int ret = 0;

  if (fuvc == NULL)
    {
      return -1;
    }

  if (g_fuvc_vc_intf_desc.bInterfaceNumber == intf)
    {
#if UVC_USE_CTRL_EP
      if (fuvc->ctrl_ep->priv)
        {
          (void)EP_DISABLE(fuvc->ctrl_ep);
          fuvc->ctrl_ep->priv = NULL;
        }

      ret = EP_CONFIGURE(fuvc->ctrl_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_ctrl_ep_desc, 0);
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->ctrl_ep->priv = (void *)fuvc;
#endif
    }
  else if (g_fuvc_vs_intf_alt0.bInterfaceNumber == intf)
    {
      if (alt == 0)
        {
          if (fuvc->transfer_status != STREAM_ON)
            {
              return 0;
            }

          if (fuvc->vs_ep)
            {
              (void)EP_DISABLE(fuvc->vs_ep);
            }

          fuvc_streaming_off(fuvc);
        }
      else
        {
          if (fuvc->vs_ep->priv)
            {
              (void)EP_DISABLE(fuvc->vs_ep);
              fuvc->vs_ep->priv = NULL;
            }

          ret = EP_CONFIGURE(fuvc->vs_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_hs_isoc_streaming_ep, 0);
          if (ret)
            {
              usb_err("Error enabling endpoint: %d\n", ret);
              return -1;
            }
          fuvc->vs_ep->priv = (void *)fuvc;

          fuvc_streaming_on(fuvc);
        }
    }
  else
    {
      return -EINVAL;
    }

  return ret;
}
#else
static int usbclass_uvc_set_alt(struct uvc_dev_s *fuvc, struct usbdev_s *dev)
{
  int ret = 0;

  if (fuvc != NULL)
    {
#if UVC_USE_CTRL_EP
      if (fuvc->ctrl_ep->priv)
        {
          (void)EP_DISABLE(fuvc->ctrl_ep);
          fuvc->ctrl_ep->priv = NULL;
        }
      ret = EP_CONFIGURE(fuvc->ctrl_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_ctrl_ep_desc, 0);
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->ctrl_ep->priv = (void *)fuvc;
#endif

      if (fuvc->vs_ep->priv)
        {
          (void)EP_DISABLE(fuvc->vs_ep);
          fuvc->vs_ep->priv = NULL;
        }

#if FUVC_BULK_TO_ISO_CONVERT
      usbd_configep_byspeed(dev, &g_fuvc_hs_isoc_streaming_ep);
      ret = EP_CONFIGURE(fuvc->vs_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_hs_isoc_streaming_ep, 0);
#else
      usbd_configep_byspeed(dev, &g_fuvc_hs_bulk_streaming_ep);
      ret = EP_CONFIGURE(fuvc->vs_ep, (const usb_endpoint_descriptor_t *)&g_fuvc_hs_bulk_streaming_ep, 0);
#endif
      if (ret)
        {
          usb_err("Error enabling endpoint: %d\n", ret);
          return -1;
        }
      fuvc->vs_ep->priv = (void *)fuvc;

      if (!LOS_ListEmpty(&g_uvc_event.stEventList))
        {
          if (LOS_EventWrite(&g_uvc_event, 0x01) == LOS_OK)
            {
              /* Write OK */

            }
        }

      if (g_start_transfer && fuvc->connected)
        {
          usbclass_uvc_stream_ep_reset(fuvc);
          fuvc->transfer_status = STREAM_OFF;
          g_start_transfer      = 0;
          fuvc->connected       = 0;
        }
    }

  return ret;
}
#endif

static int usbclass_uvc_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_device_request *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  uint8_t new_req = 0;
  uint16_t w_index;
  uint16_t w_value;
  struct uvc_dev_s *fuvc;
  struct uvc_driver_s *drvr;
  struct usbdev_req_s *req;

  (void)dataout;
  (void)outlen;

  if (driver == NULL || dev == NULL || ctrl == NULL)
    {
      return -1;
    }
  w_index = UGETW(ctrl->wIndex);
  w_value = UGETW(ctrl->wValue);

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return -1;
    }

  req           = dev->ep0->handle_req;
  req->callback = fuvc_request_complete;
  req->priv     = fuvc;

  req_type = ctrl->bmRequestType;
  if (UT_GET_TYPE(req_type) == UT_STANDARD)
    {
      switch (ctrl->bRequest)
        {
        case USB_REQ_SET_CONFIGURATION:
        case USB_REQ_SET_INTERFACE:
#if FUVC_BULK_TO_ISO_CONVERT
          (void)usbclass_uvc_set_alt(fuvc, dev, w_index, w_value);
#else
          (void)usbclass_uvc_set_alt(fuvc, dev);
#endif
          break;

        default:
          break;
        }
    }
  else
    {
      /* refer to section UVC spec. 1.5, section 4.1.2 */

      if ((req_type & 0x1f) == 0x2)
        {
          /* handle request directed to an endpoint  */

          dprintf("Support request for endpoint: %#x\n", req_type);
          return 0;
        }

      /* make sure that the request is directed to an interface */

      if ((req_type & 0x1f) != 0x1)
        {
          dprintf("Fatal Error at line: %d, req_type: %#x\n", __LINE__, req_type);
          return 0;
        }

      if ((ctrl->bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
        {
          set_probe_status(fuvc, (UGETW(ctrl->wValue) >> 8), ctrl->bRequest);
        }

      w_index = UGETW(ctrl->wIndex);
      switch (w_index & 0xff)
        {
        case USB_UVC_INTERFACE_CONTROL:
          fuvc_handle_class_setup_control(fuvc, req, ctrl);
          new_req++;
          break;

        case USB_UVC_INTERFACE_STREAMING:
          if (fuvc_handle_class_setup_streaming(fuvc, req, ctrl))
            {
              new_req++;
            }
          break;

        default:
          dprintf("Unknown index in [%s]: %#x\n", __FUNCTION__, w_index);
          break;
        }

      if (new_req)
        {
          (void)EP_SUBMIT(dev->ep0, req);
        }
    }

  return 0;
}

static void usbclass_uvc_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev)
{
  struct uvc_driver_s *drvr;
  struct uvc_dev_s *fuvc;

  if (driver == NULL || dev == NULL)
    {
      return;
    }

  drvr = (struct uvc_driver_s *)driver;
  fuvc = drvr->dev;
  if (fuvc == NULL)
    {
      return;
    }

  if (fuvc->connected)
    {
      fuvc->transfer_status = STREAM_OFF;
      g_start_transfer      = 0;
    }

  fuvc->connected = 0;
}

struct usbd_string g_fuvc_device_strings[8] =
{
  { 0, g_fuvc_str_lang },
  { 1, g_fuvc_str_manufacturer },
  { 2, g_fuvc_str_product },
  { 4, g_fuvc_str_video },
  { 5, g_fuvc_str_config },
  { 6, g_fuvc_str_interface },
  USBD_DEVICE_STRINGS_END
};

void uvc_mkdevdesc(uint8_t *buf)
{
  errno_t ret;
  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fuvc_device_desc, sizeof(g_fuvc_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy fail\n");
    }
}

int16_t uvc_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  uint16_t total_len;
  uint8_t *des;
  errno_t ret;

  g_fuvc_iad.bFirstInterface           = devinfo->ifnobase;
  g_fuvc_vc_intf_desc.bInterfaceNumber = devinfo->ifnobase;
  g_fuvc_vs_intf_alt0.bInterfaceNumber = devinfo->ifnobase + 1;

  des = link_fuvc_descriptors(NULL, 0, &total_len);
  if (des != NULL)
    {
      ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, des, total_len);
      if (ret != EOK)
        {
          usb_err("memcpy_s fail!, ret:%d\n", ret);
          free(des);
          return 0;
        }
      free(des);
    }

  return (int16_t)total_len;
}

int uvc_mkstrdesc(uint8_t id, uint8_t *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fuvc_device_strings[i].s != NULL; i++)
    {
      str = g_fuvc_device_strings[i].s;
      if (g_fuvc_device_strings[i].id == id)
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

#define UVC_NCONFIGS    1
#define UVC_CONFIGID    0
#define UVC_NINTERFACES 2
#define UVC_NSTRIDS     6
#define UVC_NUM_EPS     2
void uvc_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = uvc_mkdevdesc;
  dev->mkconfdesc = uvc_mkcfgdesc;
  dev->mkstrdesc  = uvc_mkstrdesc;

  dev->nconfigs   = UVC_NCONFIGS; /* Number of configurations supported */
  dev->configid   = UVC_CONFIGID; /* The only supported configuration ID */

  /* Interfaces.
   *
   * ifnobase must be provided by board-specific logic
   */

  dev->devinfo.ninterfaces = UVC_NINTERFACES; /* Number of interfaces in the configuration */

  /* Strings.
   *
   * strbase must be provided by board-specific logic
   */

  dev->devinfo.nstrings = UVC_NSTRIDS; /* Number of Strings */

  /* Endpoints.
   *
   * Endpoint numbers must be provided by board-specific logic.
   */

  dev->devinfo.nendpoints = UVC_NUM_EPS;
}

int uvc_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                    struct usbdevclass_driver_s **classdev)
{
  struct uvc_softc *uvc_s;
  struct uvc_dev_s *priv;
  struct uvc_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  uvc_s = (struct uvc_softc *)malloc(sizeof(struct uvc_softc));
  if (uvc_s == NULL)
    {
      return -1;
    }

  /* Convenience pointers into the allocated blob */

  priv = &uvc_s->dev;
  drvr = &uvc_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct uvc_dev_s), 0, sizeof(struct uvc_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_uvc_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void uvc_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct uvc_driver_s *uvc_drvr = (struct uvc_driver_s *)classdev;
  struct uvc_dev_s *priv;
  struct uvc_softc *uvc_s;

  if (uvc_drvr == NULL)
    {
      return;
    }

  priv = uvc_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  uvc_s = container_of(uvc_drvr, struct uvc_softc, drvr);
  free(uvc_s);
}

void usbdev_uvc_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  /* Ask the UVC driver to fill in the constants we didn't
   * know here.
   */

  uvc_get_composite_devdesc(dev);

  /* Overwrite and correct some values... */
  /* The callback functions for the UVC class */

  dev->classobject  = uvc_classobject;
  dev->uninitialize = uvc_uninitialize;

  /* Interfaces */

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  /* Strings */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_uvc_initialize(struct module *mod, int n, void *arg)
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

  usbdev_uvc_initialize_sub(&dev, 0, DEV_UVC);

  ret = composite_initialize(com_s, 1, &dev);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** uvc device initialized successfully! **\n");
  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
