/**
 * @file timer.h
 * @brief Hardware Abstraction Layer - Timer interface for RoboHub Platform.
 *
 * This header defines the abstract Timer interface used across all RoboHub
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
typedef struct RoboHubTimer {
    rh_name name;
    void*   context;

    rh_millis (*get_millis)(struct RoboHubTimer* self);
    rh_micros (*get_micros)(struct RoboHubTimer* self);
    void (*delay_ms)(struct RoboHubTimer* self, rh_u32 ms);
    void (*delay_us)(struct RoboHubTimer* self, rh_u32 us);
} RoboHubTimer;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
