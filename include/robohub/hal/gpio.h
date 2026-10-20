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
/*  Pin modes                                                                 */
/* -------------------------------------------------------------------------- */
/* The two directions a caller asks for by name. A platform that also supports
 * alternate functions may take further values for its own pins, but these two
 * are the contract: a direction pin on a transceiver, a chip select, a reset
 * line — code that just wants to drive or sense a pin asks for one of these and
 * is not written for the chip. */
#define ROBOHUB_GPIO_MODE_INPUT 0u
#define ROBOHUB_GPIO_MODE_OUTPUT 1u
/* An input with the pull-up enabled. A part whose pin is open-drain or whose
 * line floats — a 1-Wire bus, a DHT's data line, a button to ground — needs the
 * pull-up, and which way a pin is wired is the board's to say. */
#define ROBOHUB_GPIO_MODE_INPUT_PULLUP 2u

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
