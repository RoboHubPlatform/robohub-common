/**
 * @file i2c.h
 * @brief Hardware Abstraction Layer - I2C interface for RoboHub Platform.
 *
 * This header defines the abstract I2C interface used across all RoboHub
 * Platform modules. Hardware-specific details are stored in context.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */
typedef struct RoboHubI2C {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubI2C* self, rh_u32 speed_hz);
    /* addr is an unshifted 7-bit I2C address. */
    RoboHubError (*write)(struct RoboHubI2C* self,
                          rh_u8 addr,
                          const rh_u8* data,
                          rh_size len);
    RoboHubError (*read)(struct RoboHubI2C* self,
                         rh_u8 addr,
                         rh_u8* data,
                         rh_size len);
} RoboHubI2C;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif
