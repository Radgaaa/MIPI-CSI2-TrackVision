/*
 * Copyright 2017-2018, 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ADV7535_H_
#define _FSL_ADV7535_H_

#include "fsl_display.h"
#include "fsl_mipi_dsi_cmd.h"

/*
 * Change log:
 *
 *   1.0.1
 *     - Fix MISRA-C 2012 issues.
 *
 *   1.0.0
 *     - Initial version
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief ADV7535 resource.
 *
 * The I2C instance should be initialized before calling @ref ADV7535_Init.
 */
typedef struct _adv7535_resource
{
    void (*pullResetPin)(bool pullUp); /*!< Function to pull reset pin high or low. */
    void (*pullPowerPin)(bool pullUp); /*!< Function to pull power pin high or low. */
    uint8_t i2cAddr;                   /*!< I2C address for the main memory. */
    uint8_t i2cAddrCecDsi;             /*!< I2C address for DSI and CEC Address */
} adv7535_resource_t;

extern const display_operations_t adv7535_ops;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

status_t ADV7535_Init(display_handle_t *handle, const display_config_t *config);

status_t ADV7535_Deinit(display_handle_t *handle);

status_t ADV7535_Start(display_handle_t *handle);

status_t ADV7535_Stop(display_handle_t *handle);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_ADV7535_H_ */
