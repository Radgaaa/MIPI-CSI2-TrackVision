/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DISPLAY_SUPPORT_H_
#define _DISPLAY_SUPPORT_H_

#include "fsl_dc_fb.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* @TEST_ANCHOR */

#define DEMO_PANEL_RK055AHD091        1   /* 720 * 1280, RK055AHD091-CTG(RK055HDMIPI4M) */
#define DEMO_PANEL_RK055IQH091        2   /* 540 * 960,  RK055IQH091-CTG */
#define DEMO_PANEL_RK055MHD091        3   /* 720 * 1280, RK055MHD091A0-CTG(RK055HDMIPI4MA0) */
#define DEMO_PANEL_RK043FN02HC        4   /* 480 * 272 */
#define DEMO_PANEL_KD050WVTPA045      5   /* 800 * 480 */
#define DEMO_PANEL_NHD50800480TFATXL  6   /* 800 * 480,  NHD-5.0-800480TF-ATXL#-CTP */
#define DEMO_PANEL_HDMI_720X480_60   10
#define DEMO_PANEL_HDMI_720X576_50   11
#define DEMO_PANEL_HDMI_800X480_60   12
#define DEMO_PANEL_HDMI_1024X600_76  13
#define DEMO_PANEL_HDMI_1024X768_60  14
#define DEMO_PANEL_HDMI_1280X720_60  15
#define DEMO_PANEL_HDMI_1920X1080_30 16

#define DEMO_DISPLAY_CONTROLLER_ELCDIF  0
#define DEMO_DISPLAY_CONTROLLER_LCDIFV2 1

#ifndef DEMO_PANEL
#define DEMO_PANEL DEMO_PANEL_HDMI_1024X768_60
#endif

#ifndef DEMO_DISPLAY_CONTROLLER
  /* Use LCDIFV2 by default, could use ELCDIF by changing this macro. */
  #define DEMO_DISPLAY_CONTROLLER DEMO_DISPLAY_CONTROLLER_LCDIFV2
#endif

#define DEMO_BUFFER_FIXED_ADDRESS 0

#if DEMO_BUFFER_FIXED_ADDRESS
#define DEMO_BUFFER0_ADDR 0x80000000
#define DEMO_BUFFER1_ADDR 0x80200000
#endif

/*
 * Use the MIPI dumb panel
 */

/* Definitions for the frame buffer. */
#define DEMO_BUFFER_COUNT 2 /* 2 is enough for DPI interface display. */

#ifndef DEMO_USE_XRGB8888
#define DEMO_USE_XRGB8888 0
#endif

#if DEMO_USE_XRGB8888
#define DEMO_BUFFER_PIXEL_FORMAT   kVIDEO_PixelFormatXRGB8888
#define DEMO_BUFFER_BYTE_PER_PIXEL 4
#else
#define DEMO_BUFFER_PIXEL_FORMAT   kVIDEO_PixelFormatRGB565
#define DEMO_BUFFER_BYTE_PER_PIXEL 2
#endif

#if (DEMO_PANEL_RK055AHD091 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (720)
  #define DEMO_PANEL_HEIGHT (1280)
  #define DEMO_PANEL_NAME   "RK055AHD091"

  #define DEMO_HSW 8
  #define DEMO_HFP 32
  #define DEMO_HBP 32
  #define DEMO_VSW 2
  #define DEMO_VFP 16
  #define DEMO_VBP 14

#elif (DEMO_PANEL_RK055IQH091 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (540)
  #define DEMO_PANEL_HEIGHT (960)
  #define DEMO_PANEL_NAME   "RK055IQH091"

  #define DEMO_HSW 2
  #define DEMO_HFP 32
  #define DEMO_HBP 30
  #define DEMO_VSW 2
  #define DEMO_VFP 16
  #define DEMO_VBP 14

#elif (DEMO_PANEL_RK055MHD091 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (720)
  #define DEMO_PANEL_HEIGHT (1280)
  #define DEMO_PANEL_NAME   "RK055MHD091"

  #define DEMO_HSW 6
  #define DEMO_HFP 12
  #define DEMO_HBP 24
  #define DEMO_VSW 2
  #define DEMO_VFP 16
  #define DEMO_VBP 14

#elif (DEMO_PANEL_RK043FN02HC == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (480)
  #define DEMO_PANEL_HEIGHT (272)
  #define DEMO_PANEL_NAME   "RK043FN02HC"

  #define DEMO_HSW 2
  #define DEMO_HFP 32
  #define DEMO_HBP 30
  #define DEMO_VSW 2
  #define DEMO_VFP 16
  #define DEMO_VBP 14

#elif (DEMO_PANEL_KD050WVTPA045 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (800)
  #define DEMO_PANEL_HEIGHT (480)
  #define DEMO_PANEL_NAME   "KD050WVTPA045"

  #define DEMO_HSW 48
  #define DEMO_HFP 40
  #define DEMO_HBP 40
  #define DEMO_VSW 3
  #define DEMO_VFP 141
  #define DEMO_VBP 29

#elif (DEMO_PANEL_NHD50800480TFATXL == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (800)
  #define DEMO_PANEL_HEIGHT (480)
  #define DEMO_PANEL_NAME   "NHD50800480TFATXL"

  #define DEMO_HSW 48
  #define DEMO_HFP 40
  #define DEMO_HBP 40
  #define DEMO_VSW 3
  #define DEMO_VFP 141
  #define DEMO_VBP 29

#elif (DEMO_PANEL_HDMI_720X480_60 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (720)
  #define DEMO_PANEL_HEIGHT (480)
  #define DEMO_PANEL_NAME   "HDMI_720X480_60"

  #define DEMO_HSW 62
  #define DEMO_HBP 60
  #define DEMO_HFP 16
  #define DEMO_VSW 6
  #define DEMO_VBP 30
  #define DEMO_VFP 9

  #define DEMO_VIDEO_FREQ_KHZ    594000
  #define DEMO_PIXEL_CLOCK_DIV   22
  #define DEMO_MIPI_DPY_REF_DIV  22

#elif (DEMO_PANEL_HDMI_720X576_50 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (720)
  #define DEMO_PANEL_HEIGHT (576)
  #define DEMO_PANEL_NAME   "HDMI_720X576_50"

  #define DEMO_HSW 64
  #define DEMO_HBP 68
  #define DEMO_HFP 12
  #define DEMO_VSW 5
  #define DEMO_VBP 39
  #define DEMO_VFP 5

  #define DEMO_VIDEO_FREQ_KHZ    594000
  #define DEMO_PIXEL_CLOCK_DIV   22
  #define DEMO_MIPI_DPY_REF_DIV  22

#elif (DEMO_PANEL_HDMI_800X480_60 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (800)
  #define DEMO_PANEL_HEIGHT (480)
  #define DEMO_PANEL_NAME   "HDMI_800X480_60"

  #define DEMO_HSW 20
  #define DEMO_HFP 70
  #define DEMO_HBP 26
  #define DEMO_VSW 2
  #define DEMO_VFP 7
  #define DEMO_VBP 21

  #define DEMO_VIDEO_FREQ_KHZ    594000
  #define DEMO_PIXEL_CLOCK_DIV   22
  #define DEMO_MIPI_DPY_REF_DIV  22

#elif (DEMO_PANEL_HDMI_1024X600_76 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (1024)
  #define DEMO_PANEL_HEIGHT (600)
  #define DEMO_PANEL_NAME   "HDMI_1024X600_76"

  #define DEMO_HSW 136
  #define DEMO_HBP 160
  #define DEMO_HFP 24
  #define DEMO_VSW 6
  #define DEMO_VBP 29
  #define DEMO_VFP 3

  #define DEMO_VIDEO_FREQ_KHZ    260000
  #define DEMO_PIXEL_CLOCK_DIV   4
  #define DEMO_MIPI_DPY_REF_DIV  10

#elif (DEMO_PANEL_HDMI_1024X768_60 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (1024)
  #define DEMO_PANEL_HEIGHT (768)
  #define DEMO_PANEL_NAME   "HDMI_1024X768_60"

  #define DEMO_HSW 136
  #define DEMO_HBP 160
  #define DEMO_HFP 24
  #define DEMO_VSW 6
  #define DEMO_VBP 29
  #define DEMO_VFP 3

  #define DEMO_VIDEO_FREQ_KHZ    260000
  #define DEMO_PIXEL_CLOCK_DIV   4
  #define DEMO_MIPI_DPY_REF_DIV  10

#elif (DEMO_PANEL_HDMI_1280X720_60 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (1280)
  #define DEMO_PANEL_HEIGHT (720)
  #define DEMO_PANEL_NAME   "HDMI_1280X720_60"

  #define DEMO_HSW 40
  #define DEMO_HBP 220
  #define DEMO_HFP 110
  #define DEMO_VSW 5
  #define DEMO_VBP 20
  #define DEMO_VFP 5

  #define DEMO_VIDEO_FREQ_KHZ    594000
  #define DEMO_PIXEL_CLOCK_DIV   8
  #define DEMO_MIPI_DPY_REF_DIV  22

#elif (DEMO_PANEL_HDMI_1920X1080_30 == DEMO_PANEL)

  #define DEMO_PANEL_WIDTH  (1920)
  #define DEMO_PANEL_HEIGHT (1080)
  #define DEMO_PANEL_NAME   "HDMI_1920X1080_30"

  #define DEMO_HSW 44
  #define DEMO_HBP 148
  #define DEMO_HFP 88
  #define DEMO_VSW 5
  #define DEMO_VBP 36
  #define DEMO_VFP 4

  #define DEMO_VIDEO_FREQ_KHZ    594000
  #define DEMO_PIXEL_CLOCK_DIV   8
  #define DEMO_MIPI_DPY_REF_DIV  22

#else
  #error Must set DEMO_PANEL to one of the valid DEMO_PANEL_* values
#endif

#define IS_HDMI_PANEL()         (DEMO_PANEL >= 10)
#define USE_FT5406_TOUCH()      ((DEMO_PANEL_RK043FN02HC == DEMO_PANEL) || (DEMO_PANEL_NHD50800480TFATXL == DEMO_PANEL))
#define USE_GT911_TOUCH()       ((DEMO_PANEL_RK055AHD091 == DEMO_PANEL) || (DEMO_PANEL_RK055IQH091 == DEMO_PANEL) || (DEMO_PANEL_RK055MHD091 == DEMO_PANEL))
#define IS_TOUCH_SUPPORTED()    (USE_FT5406_TOUCH() || USE_GT911_TOUCH())
#define IS_PARALLEL_RGB_PANEL() ((DEMO_PANEL_RK043FN02HC == DEMO_PANEL) || (DEMO_PANEL_KD050WVTPA045 == DEMO_PANEL) || (DEMO_PANEL_NHD50800480TFATXL == DEMO_PANEL))


#define DEMO_BUFFER_WIDTH   DEMO_PANEL_WIDTH
#define DEMO_BUFFER_HEIGHT  DEMO_PANEL_HEIGHT

/* Where the frame buffer is shown in the screen. */
#define DEMO_BUFFER_START_X 0U
#define DEMO_BUFFER_START_Y 0U

#define DEMO_BUFFER_STRIDE_BYTE (DEMO_BUFFER_WIDTH * DEMO_BUFFER_BYTE_PER_PIXEL)

/* There is not frame buffer aligned requirement, consider the 64-bit AXI data
 * bus width and 32-byte cache line size, the frame buffer alignment is set to
 * 32 byte.
 */
#define FRAME_BUFFER_ALIGN 32

extern const dc_fb_t g_dc;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

status_t BOARD_PrepareDisplayController(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _DISPLAY_SUPPORT_H_ */
