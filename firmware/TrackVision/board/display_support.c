/*
 * Copyright 2019-2021, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "display_support.h"
#include "fsl_gpio.h"
#include "fsl_mipi_dsi.h"
#if (DEMO_PANEL_RK055AHD091 == DEMO_PANEL)
  #include "fsl_rm68200.h"
#elif (DEMO_PANEL_RK055IQH091 == DEMO_PANEL)
  #include "fsl_rm68191.h"
#elif (DEMO_PANEL_RK055MHD091 == DEMO_PANEL)
  #include "fsl_hx8394.h"
#elif IS_HDMI_PANEL()
  #include "fsl_adv7535.h"
#else
  #include "fsl_display.h"
#endif
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "pca6416.h"
#include "pca9530.h"

#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
#include "fsl_dc_fb_lcdifv2.h"
#else
#include "fsl_dc_fb_elcdif.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
  #if (DEMO_PANEL_RK043FN02HC == DEMO_PANEL)
    #define DEMO_LCDIF_POL_FLAGS                                                             \
        (kLCDIFV2_DataEnableActiveHigh | kLCDIFV2_VsyncActiveLow | kLCDIFV2_HsyncActiveLow | \
         kLCDIFV2_DriveDataOnRisingClkEdge)
  #else
    #define DEMO_LCDIF_POL_FLAGS                                                             \
        (kLCDIFV2_DataEnableActiveHigh | kLCDIFV2_VsyncActiveLow | kLCDIFV2_HsyncActiveLow | \
         kLCDIFV2_DriveDataOnFallingClkEdge)
  #endif

  #define DEMO_LCDIF LCDIFV2

#else
  #if (DEMO_PANEL_RK043FN02HC == DEMO_PANEL)
    #define DEMO_LCDIF_POL_FLAGS \
        (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnRisingClkEdge)
  #else
    #define DEMO_LCDIF_POL_FLAGS \
        (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnFallingClkEdge)
  #endif

  #define DEMO_LCDIF LCDIF

#endif

/* Definitions for MIPI. */
#define DEMO_MIPI_DSI          (&g_mipiDsi)
#define DEMO_MIPI_DSI_LANE_NUM 2

/*
 * The DPHY bit clock must be fast enough to send out the pixels, it should be
 * larger than:
 *
 *         (Pixel clock * bit per output pixel) / number of MIPI data lane
 *
 * Here the desired DPHY bit clock multiplied by ( 9 / 8 = 1.125) to ensure
 * it is fast enough.
 */
#define DEMO_MIPI_DPHY_BIT_CLK_ENLARGE(origin) (((origin) / 8) * 9)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Resets panel using reset pin.
 * 
 * @param pullUp sets true pin
 */
static void BOARD_PullPanelResetPin(bool pullUp);

/*!
 * @brief Resets panel power down pin.
 * 
 * @param pullUp sets true pin
 */
static void BOARD_PullPanelPowerPin(bool pullUp);

/*!
 * @brief Initializes Lcdif Clock.
 */
static void BOARD_InitLcdifClock(void);

/*!
 * @brief Initializes Mipi Dsi Clock.
 */
static void BOARD_InitMipiDsiClock(void);

#if !IS_HDMI_PANEL()
/*!
 * @brief Data transfer using blocking method.
 *
 * Perform data transfer using blocking method. This function waits until all
 * data send or received, or timeout happens.
 *
 * @param xfer pointer to the transfer structure.
 * @return kStatus_Success Data transfer finished with no error.
 * @return kStatus_Timeout Transfer failed because of timeout.
 * @return kStatus_DSI_RxDataError RX data error, user could use ref DSI_GetRxErrorStatus
 * to check the error details.
 * @return kStatus_DSI_ErrorReportReceived Error Report packet received, user could use
 *        ref DSI_GetAndClearHostStatus to check the error report status.
 * @return kStatus_DSI_NotSupported Transfer format not supported.
 * @return kStatus_DSI_Fail Transfer failed for other reasons.
 */
static status_t BOARD_DSI_Transfer(dsi_transfer_t *xfer);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint32_t mipiDsiTxEscClkFreq_Hz;
static uint32_t mipiDsiDphyBitClkFreq_Hz;
static uint32_t mipiDsiDphyRefClkFreq_Hz;
static uint32_t mipiDsiDpiClkFreq_Hz;

const MIPI_DSI_Type g_mipiDsi = {
    .host = DSI_HOST,
    .apb  = DSI_HOST_APB_PKT_IF,
    .dpi  = DSI_HOST_DPI_INTFC,
    .dphy = DSI_HOST_DPHY_INTFC,
};

#if (DEMO_PANEL == DEMO_PANEL_RK055AHD091)

static mipi_dsi_device_t dsiDevice = {
    .virtualChannel = 0,
    .xferFunc       = BOARD_DSI_Transfer,
};

static const rm68200_resource_t rm68200Resource = {
    .dsiDevice    = &dsiDevice,
    .pullResetPin = BOARD_PullPanelResetPin,
    .pullPowerPin = BOARD_PullPanelPowerPin,
};

static display_handle_t rm68200Handle = {
    .resource = &rm68200Resource,
    .ops      = &rm68200_ops,
};

#elif (DEMO_PANEL_RK055IQH091 == DEMO_PANEL)

static mipi_dsi_device_t dsiDevice = {
    .virtualChannel = 0,
    .xferFunc       = BOARD_DSI_Transfer,
};

static const rm68191_resource_t rm68191Resource = {
    .dsiDevice    = &dsiDevice,
    .pullResetPin = BOARD_PullPanelResetPin,
    .pullPowerPin = BOARD_PullPanelPowerPin,
};

static display_handle_t rm68191Handle = {
    .resource = &rm68191Resource,
    .ops      = &rm68191_ops,
};

#elif (DEMO_PANEL == DEMO_PANEL_RK055MHD091)

static mipi_dsi_device_t dsiDevice = {
    .virtualChannel = 0,
    .xferFunc       = BOARD_DSI_Transfer,
};

static const hx8394_resource_t hx8394Resource = {
    .dsiDevice    = &dsiDevice,
    .pullResetPin = BOARD_PullPanelResetPin,
    .pullPowerPin = BOARD_PullPanelPowerPin,
};

static display_handle_t hx8394Handle = {
    .resource = &hx8394Resource,
    .ops      = &hx8394_ops,
};

#elif IS_PARALLEL_RGB_PANEL()

  /* Will use the Parallel RGB interface instead */

#else

#define  ADV7533_MAIN_I2C_ADDR           ((uint8_t)0x7A >> 1)
#define  ADV7533_CEC_DSI_I2C_ADDR        ((uint8_t)0x78 >> 1)

static adv7535_resource_t adv7535Resource = {
		.i2cAddr = ADV7533_MAIN_I2C_ADDR,
		.i2cAddrCecDsi = ADV7533_CEC_DSI_I2C_ADDR,
	    .pullResetPin = BOARD_PullPanelResetPin,
	    .pullPowerPin = BOARD_PullPanelPowerPin,
};

static display_handle_t adv7535Handle = {
    .resource = &adv7535Resource,
    .ops      = &adv7535_ops,
};

#endif

#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)

static dc_fb_lcdifv2_handle_t s_dcFbLcdifv2Handle = {0};

static const dc_fb_lcdifv2_config_t s_dcFbLcdifv2Config = {
    .lcdifv2       = DEMO_LCDIF,
    .width         = DEMO_PANEL_WIDTH,
    .height        = DEMO_PANEL_HEIGHT,
    .hsw           = DEMO_HSW,
    .hfp           = DEMO_HFP,
    .hbp           = DEMO_HBP,
    .vsw           = DEMO_VSW,
    .vfp           = DEMO_VFP,
    .vbp           = DEMO_VBP,
    .polarityFlags = DEMO_LCDIF_POL_FLAGS,
    .lineOrder     = kLCDIFV2_LineOrderRGB,
/* CM4 is domain 1, CM7 is domain 0. */
#if (__CORTEX_M <= 4)
    .domain = 1,
#else
    .domain = 0,
#endif
};

const dc_fb_t g_dc = {
    .ops     = &g_dcFbOpsLcdifv2,
    .prvData = &s_dcFbLcdifv2Handle,
    .config  = &s_dcFbLcdifv2Config,
};

#else

dc_fb_elcdif_handle_t s_dcFbElcdifHandle = {0}; /* The handle must be initialized to 0. */

const dc_fb_elcdif_config_t s_dcFbElcdifConfig = {
    .elcdif        = DEMO_LCDIF,
    .width         = DEMO_PANEL_WIDTH,
    .height        = DEMO_PANEL_HEIGHT,
    .hsw           = DEMO_HSW,
    .hfp           = DEMO_HFP,
    .hbp           = DEMO_HBP,
    .vsw           = DEMO_VSW,
    .vfp           = DEMO_VFP,
    .vbp           = DEMO_VBP,
    .polarityFlags = DEMO_LCDIF_POL_FLAGS,
#if IS_PARALLEL_RGB_PANEL()
    .dataBus       = kELCDIF_DataBus16Bit,
#elif (!DEMO_USE_XRGB8888) && (DEMO_USE_LUT8)
    .dataBus       = kELCDIF_DataBus8Bit,
#else
    .dataBus       = kELCDIF_DataBus24Bit,
#endif
};

const dc_fb_t g_dc = {
    .ops     = &g_dcFbOpsElcdif,
    .prvData = &s_dcFbElcdifHandle,
    .config  = &s_dcFbElcdifConfig,
};
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

static void BOARD_PullPanelResetPin(bool pullUp)
{
    if (pullUp)
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_RST_GPIO, BOARD_MIPI_PANEL_RST_PIN, 1);
        PCA6416_SetPins(PCA_LCD_DISP_MIPI_RST);
    }
    else
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_RST_GPIO, BOARD_MIPI_PANEL_RST_PIN, 0);
        PCA6416_ClearPins(PCA_LCD_DISP_MIPI_RST);
    }
}

static void BOARD_PullPanelPowerPin(bool pullUp)
{
    if (pullUp)
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_POWER_GPIO, BOARD_MIPI_PANEL_POWER_PIN, 1);
        PCA6416_SetPins(PCA_LCD_BL_PWR);
    }
    else
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_POWER_GPIO, BOARD_MIPI_PANEL_POWER_PIN, 0);
        PCA6416_ClearPins(PCA_LCD_BL_PWR);
    }
}

#if !IS_HDMI_PANEL()
static status_t BOARD_DSI_Transfer(dsi_transfer_t *xfer)
{
    return DSI_TransferBlocking(DEMO_MIPI_DSI, xfer);
}
#endif

static void BOARD_InitLcdifClock(void)
{
    /*
     * The pixel clock is (height + VSW + VFP + VBP) * (width + HSW + HFP + HBP) * frame rate.
     *
     * For 60Hz frame rate, the RK055IQH091 pixel clock should be 36MHz.
     * the RK055AHD091 pixel clock should be 62MHz.
     */
    const clock_root_config_t lcdifClockConfig = {
        .clockOff = false,
#if IS_HDMI_PANEL()
        .mux      = kCLOCK_LCDIFV2_ClockRoot_MuxVideoPllOut,
        .div      = DEMO_PIXEL_CLOCK_DIV,
#else
        .mux      = 4, /*!< PLL_528. */
  #if ((DEMO_PANEL == DEMO_PANEL_RK055AHD091) || (DEMO_PANEL_RK055MHD091 == DEMO_PANEL))
        .div = 9,
  #elif (DEMO_PANEL == DEMO_PANEL_RK043FN02HC)
        .div = 59,  
  #elif ((DEMO_PANEL == DEMO_PANEL_KD050WVTPA045) || (DEMO_PANEL == DEMO_PANEL_NHD50800480TFATXL))
        .div = 16,
  #else
        .div = 15,
  #endif
#endif
    };

#if IS_HDMI_PANEL()
    CLOCK_InitVideoPllWithFreqKHz(DEMO_VIDEO_FREQ_KHZ, false, 0, 0);
#endif

#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
    CLOCK_SetRootClock(kCLOCK_Root_Lcdifv2, &lcdifClockConfig);

    mipiDsiDpiClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Lcdifv2);

#else

    CLOCK_SetRootClock(kCLOCK_Root_Lcdif, &lcdifClockConfig);

    mipiDsiDpiClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Lcdif);
#endif
}

static void BOARD_InitMipiDsiClock(void)
{
    uint32_t mipiDsiEscClkFreq_Hz;

    /* RxClkEsc max 60MHz, TxClkEsc 12 to 20MHz. */
    /* RxClkEsc = 528MHz / 11 = 48MHz. */
    /* TxClkEsc = 528MHz / 11 / 4 = 16MHz. */
    const clock_root_config_t mipiEscClockConfig = {
        .clockOff = false,
        .mux      = 4, /*!< PLL_528. */
        .div      = 11,
    };

    CLOCK_SetRootClock(kCLOCK_Root_Mipi_Esc, &mipiEscClockConfig);

    mipiDsiEscClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Mipi_Esc);

    const clock_group_config_t mipiEscClockGroupConfig = {
        .clockOff = false,
        .resetDiv = 2,
        .div0     = 2, /* TX esc clock. */
    };

    CLOCK_SetGroupConfig(kCLOCK_Group_MipiDsi, &mipiEscClockGroupConfig);

    mipiDsiTxEscClkFreq_Hz = mipiDsiEscClkFreq_Hz / 3;

#if IS_HDMI_PANEL()
    /* DPHY reference clock, use Video PLL. */
    const clock_root_config_t mipiDphyRefClockConfig = {
        .clockOff = false,
        .mux      = kCLOCK_MIPI_REF_ClockRoot_MuxVideoPllOut,
        .div      = DEMO_MIPI_DPY_REF_DIV,
    };

    CLOCK_SetRootClock(kCLOCK_Root_Mipi_Ref, &mipiDphyRefClockConfig);

    mipiDsiDphyRefClkFreq_Hz = CLOCK_GetRootClockFreq(kCLOCK_Root_Mipi_Ref);
#else
    /* DPHY reference clock, use OSC 24MHz clock. */
    const clock_root_config_t mipiDphyRefClockConfig = {
        .clockOff = false,
        .mux      = 1, /*!< OSC_24M. */
        .div      = 1,
    };

    CLOCK_SetRootClock(kCLOCK_Root_Mipi_Ref, &mipiDphyRefClockConfig);

    mipiDsiDphyRefClkFreq_Hz = BOARD_XTAL0_CLK_HZ;
#endif
}

/*!
 * @brief Initializes Lcd panel.
 */
static status_t BOARD_InitLcdPanel(void)
{
    status_t status = kStatus_Success;

    //const gpio_pin_config_t pinConfig = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    const display_config_t displayConfig = {
        .resolution   = FSL_VIDEO_RESOLUTION(DEMO_PANEL_WIDTH, DEMO_PANEL_HEIGHT),
        .hsw          = DEMO_HSW,
        .hfp          = DEMO_HFP,
        .hbp          = DEMO_HBP,
        .vsw          = DEMO_VSW,
        .vfp          = DEMO_VFP,
        .vbp          = DEMO_VBP,
        .controlFlags = 0,
        .dsiLanes     = DEMO_MIPI_DSI_LANE_NUM,
    };

    //GPIO_PinInit(BOARD_MIPI_PANEL_POWER_GPIO, BOARD_MIPI_PANEL_POWER_PIN, &pinConfig);
    //GPIO_PinInit(BOARD_MIPI_PANEL_BL_GPIO, BOARD_MIPI_PANEL_BL_PIN, &pinConfig);
    //GPIO_PinInit(BOARD_MIPI_PANEL_RST_GPIO, BOARD_MIPI_PANEL_RST_PIN, &pinConfig);
#if (DEMO_PANEL == DEMO_PANEL_RK055AHD091)
    PCA6416_ClearPins(PCA_LCD_DISP_MIPI_RST | PCA_LCD_BL_PWR);
    PCA9530_ClearPins(PCA9530_LED0);

    status = RM68200_Init(&rm68200Handle, &displayConfig);

    if (status == kStatus_Success)
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_BL_GPIO, BOARD_MIPI_PANEL_BL_PIN, 1);
        PCA9530_SetPins(PCA9530_LED0);
    }
#elif (DEMO_PANEL == DEMO_PANEL_RK055IQH091)
    PCA6416_ClearPins(PCA_LCD_DISP_MIPI_RST | PCA_LCD_BL_PWR);
    PCA9530_ClearPins(PCA9530_LED0);

    status = RM68191_Init(&rm68191Handle, &displayConfig);

    if (status == kStatus_Success)
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_BL_GPIO, BOARD_MIPI_PANEL_BL_PIN, 1);
        PCA9530_SetPins(PCA9530_LED0);
    }
#elif (DEMO_PANEL == DEMO_PANEL_RK055MHD091)
    PCA6416_ClearPins(PCA_LCD_DISP_MIPI_RST | PCA_LCD_BL_PWR);
    PCA9530_ClearPins(PCA9530_LED0);

    status = HX8394_Init(&hx8394Handle, &displayConfig);

    if (status == kStatus_Success)
    {
        //GPIO_PinWrite(BOARD_MIPI_PANEL_BL_GPIO, BOARD_MIPI_PANEL_BL_PIN, 1);
        PCA9530_SetPins(PCA9530_LED0);
    }
#elif IS_PARALLEL_RGB_PANEL()
    PCA6416_ClearPins(PCA_LCD_DISP_MIPI_RST | PCA_LCD_BL_PWR);
    PCA9530_ClearPins(PCA9530_LED0);

    /* Power on. */
    BOARD_PullPanelPowerPin(true);
    SDK_DelayAtLeastUs(1000U, SystemCoreClock);

    /* Perform reset. */
    BOARD_PullPanelResetPin(false);
    SDK_DelayAtLeastUs(1000U, SystemCoreClock);
    BOARD_PullPanelResetPin(true);
    SDK_DelayAtLeastUs(1000U, SystemCoreClock);

    PCA9530_SetPins(PCA9530_LED0);
#else
    status = ADV7535_Init(&adv7535Handle, &displayConfig);
#endif

    return status;
}

/*!
 * @brief Sets Mipi Dsi Configuration.
 */
static void BOARD_SetMipiDsiConfig(void)
{
    dsi_config_t dsiConfig;
    dsi_dphy_config_t dphyConfig;

    const dsi_dpi_config_t dpiConfig = {.pixelPayloadSize = DEMO_PANEL_WIDTH,
                                        .dpiColorCoding   = kDSI_Dpi24Bit,
                                        .pixelPacket      = kDSI_PixelPacket24Bit,
#if IS_HDMI_PANEL()
										.videoMode        = kDSI_DpiNonBurstWithSyncPulse,
                                        .bllpMode         = kDSI_DpiBllpNull,
                                        .polarityFlags    = kDSI_DpiVsyncActiveLow | kDSI_DpiHsyncActiveLow,
#else
                                        .videoMode        = kDSI_DpiBurst,
                                        .bllpMode         = kDSI_DpiBllpLowPower,
                                        .polarityFlags    = kDSI_DpiVsyncActiveLow | kDSI_DpiHsyncActiveLow,
#endif
                                        .hfp              = DEMO_HFP,
                                        .hbp              = DEMO_HBP,
                                        .hsw              = DEMO_HSW,
                                        .vfp              = DEMO_VFP,
                                        .vbp              = DEMO_VBP,

                                        .panelHeight      = DEMO_PANEL_HEIGHT,
                                        .virtualChannel   = 0};

    /*
     * dsiConfig.numLanes = 4;
     * dsiConfig.enableNonContinuousHsClk = false;
     * dsiConfig.autoInsertEoTp = true;
     * dsiConfig.numExtraEoTp = 0;
     * dsiConfig.htxTo_ByteClk = 0;
     * dsiConfig.lrxHostTo_ByteClk = 0;
     * dsiConfig.btaTo_ByteClk = 0;
     */
    DSI_GetDefaultConfig(&dsiConfig);
    dsiConfig.numLanes       = DEMO_MIPI_DSI_LANE_NUM;
#if IS_HDMI_PANEL()
    dsiConfig.autoInsertEoTp = false;
#else
    dsiConfig.autoInsertEoTp = true;
#endif

    /* Init the DSI module. */
    DSI_Init(DEMO_MIPI_DSI, &dsiConfig);

    /* Init DPHY.
     *
     * The DPHY bit clock must be fast enough to send out the pixels, it should be
     * larger than:
     *
     *         (Pixel clock * bit per output pixel) / number of MIPI data lane
     *
     * Here the desired DPHY bit clock multiplied by ( 9 / 8 = 1.125) to ensure
     * it is fast enough.
     *
     * Note that the DSI output pixel is 24bit per pixel.
     */
    mipiDsiDphyBitClkFreq_Hz = mipiDsiDpiClkFreq_Hz * (24 / DEMO_MIPI_DSI_LANE_NUM);

#if !IS_HDMI_PANEL()
    mipiDsiDphyBitClkFreq_Hz = DEMO_MIPI_DPHY_BIT_CLK_ENLARGE(mipiDsiDphyBitClkFreq_Hz);
#endif

    DSI_GetDphyDefaultConfig(&dphyConfig, mipiDsiDphyBitClkFreq_Hz, mipiDsiTxEscClkFreq_Hz);

    mipiDsiDphyBitClkFreq_Hz = DSI_InitDphy(DEMO_MIPI_DSI, &dphyConfig, mipiDsiDphyRefClkFreq_Hz);

    /* Init DPI interface. */
    DSI_SetDpiConfig(DEMO_MIPI_DSI, &dpiConfig, DEMO_MIPI_DSI_LANE_NUM, mipiDsiDpiClkFreq_Hz, mipiDsiDphyBitClkFreq_Hz);
}

/*!
 * @brief Initializes Display interface.
 */
status_t BOARD_InitDisplayInterface(void)
{
#if IS_PARALLEL_RGB_PANEL()

    CLOCK_EnableClock(kCLOCK_Video_Mux);

  #if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
    /* LCDIF v2 output to Parallel LCDIF. */
    VIDEO_MUX->VID_MUX_CTRL.SET = VIDEO_MUX_VID_MUX_CTRL_PARA_LCD_SEL_MASK;
  #else
    /* ELCDIF output to Parallel LCDIF. */
    VIDEO_MUX->VID_MUX_CTRL.CLR = VIDEO_MUX_VID_MUX_CTRL_PARA_LCD_SEL_MASK;
  #endif

#else

    CLOCK_EnableClock(kCLOCK_Video_Mux);

  #if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
    /* LCDIF v2 output to MIPI DSI. */
    VIDEO_MUX->VID_MUX_CTRL.SET = VIDEO_MUX_VID_MUX_CTRL_MIPI_DSI_SEL_MASK;
  #else
    /* ELCDIF output to MIPI DSI. */
    VIDEO_MUX->VID_MUX_CTRL.CLR = VIDEO_MUX_VID_MUX_CTRL_MIPI_DSI_SEL_MASK;
  #endif

    /* 1. Power on and isolation off. */
    PGMC_BPC4->BPC_POWER_CTRL |= (PGMC_BPC_BPC_POWER_CTRL_PSW_ON_SOFT_MASK | PGMC_BPC_BPC_POWER_CTRL_ISO_OFF_SOFT_MASK);

    /* 2. Assert MIPI reset. */
    IOMUXC_GPR->GPR62 &=
        ~(IOMUXC_GPR_GPR62_MIPI_DSI_PCLK_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_ESC_SOFT_RESET_N_MASK |
          IOMUXC_GPR_GPR62_MIPI_DSI_BYTE_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_DPI_SOFT_RESET_N_MASK);

    /* 3. Setup clock. */
    BOARD_InitMipiDsiClock();

    /* 4. Deassert PCLK and ESC reset. */
    IOMUXC_GPR->GPR62 |=
        (IOMUXC_GPR_GPR62_MIPI_DSI_PCLK_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_ESC_SOFT_RESET_N_MASK);

    /* 5. Configures peripheral. */
    BOARD_SetMipiDsiConfig();

    /* 6. Deassert BYTE and DBI reset. */
    IOMUXC_GPR->GPR62 |=
        (IOMUXC_GPR_GPR62_MIPI_DSI_BYTE_SOFT_RESET_N_MASK | IOMUXC_GPR_GPR62_MIPI_DSI_DPI_SOFT_RESET_N_MASK);

#endif

    /* 7. Configure the panel. */
    return BOARD_InitLcdPanel();
}

/*!
 * @brief Handles LCDIFv2 IRQ.
 */
#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
void LCDIFv2_IRQHandler(void)
{
    DC_FB_LCDIFV2_IRQHandler(&g_dc);
}
#else
void eLCDIF_IRQHandler(void)
{
    DC_FB_ELCDIF_IRQHandler(&g_dc);
}
#endif

/*!
 * @brief Verifies display clock source.
 */
status_t BOARD_VerifyDisplayClockSource(void)
{
    status_t status;
    uint32_t srcClkFreq;

    /*
     * In this implementation, the SYSPLL2 (528M) clock is used as the source
     * of LCDIFV2 pixel clock and MIPI DSI ESC clock. The OSC24M clock is used
     * as the MIPI DSI DPHY PLL reference clock. This function checks the clock
     * source are valid. OSC24M is always valid, so only verify the SYSPLL2.
     */
    srcClkFreq = CLOCK_GetPllFreq(kCLOCK_PllSys2);
    if (528 != (srcClkFreq / 1000000))
    {
        status = kStatus_Fail;
    }
    else
    {
        status = kStatus_Success;
    }

    return status;
}

/*!
 * @brief Initializes display controler.
 * 
 * @return status code
 */
status_t BOARD_PrepareDisplayController(void)
{
    status_t status;

    status = BOARD_VerifyDisplayClockSource();

    if (status != kStatus_Success)
    {
        PRINTF("Error: Invalid display clock source.\r\n");
        return status;
    }

    BOARD_InitLcdifClock();

    status = BOARD_InitDisplayInterface();

    if (kStatus_Success == status)
    {
#if (DEMO_DISPLAY_CONTROLLER == DEMO_DISPLAY_CONTROLLER_LCDIFV2)
        NVIC_ClearPendingIRQ(LCDIFv2_IRQn);
        NVIC_SetPriority(LCDIFv2_IRQn, 3);
        EnableIRQ(LCDIFv2_IRQn);
#else
        NVIC_ClearPendingIRQ(eLCDIF_IRQn);
        NVIC_SetPriority(eLCDIF_IRQn, 3);
        EnableIRQ(eLCDIF_IRQn);
#endif
    }

    return kStatus_Success;
}
