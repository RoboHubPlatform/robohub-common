/**
 * @file gpio.h
 * @brief Hardware Abstraction Layer - GPIO interface for RoboHub Platform.
 *
 * This header defines the abstract GPIO interface used across all RoboHub
 * Platform modules. Hardware-specific details (ports, pins, alternate
 * functions) are stored in platform/board-specific context structures.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_GPIO_H
#define INCLUDE_ROBOHUB_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

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
typedef struct RoboHubGPIO {
    /* Instance metadata */
    rh_name name;       /* e.g. "GPIO0", "GPIO_MAIN" */
    void*   context;    /* platform/board-specific data */

    /* HAL operations */
    RoboHubError (*init)(struct RoboHubGPIO* self);
    RoboHubError (*set_mode)(struct RoboHubGPIO* self, rh_u32 pin, rh_u32 mode);
    RoboHubError (*write)(struct RoboHubGPIO* self, rh_u32 pin, rh_u32 value);
    RoboHubError (*read)(struct RoboHubGPIO* self, rh_u32 pin, rh_u32* value);
} RoboHubGPIO;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_GPIO_H */
