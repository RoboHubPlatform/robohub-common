/**
 * @file rs485.h
 * @brief Hardware Abstraction Layer - RS485 interface for RoboHub Platform.
 *
 * This header defines the abstract RS485 interface used across all RoboHub
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
typedef struct RoboHubRS485 {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubRS485* self, rh_u32 baudrate);
    /* A successful operation transfers all requested bytes. */
    RoboHubError (*write)(struct RoboHubRS485* self,
                          const rh_u8* data,
                          rh_size len);
    RoboHubError (*read)(struct RoboHubRS485* self,
                         rh_u8* data,
                         rh_size len);
    RoboHubError (*set_direction)(struct RoboHubRS485* self, rh_u32 dir);
} RoboHubRS485;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
