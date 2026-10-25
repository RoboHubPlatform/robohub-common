/**
 * @file display.h
 * @brief Hardware Abstraction Layer - Display interface for RoboHub Platform.
 *
 * This header defines the abstract display interface used across all RoboHub
 * Platform modules. Hardware-specific details (resolution, color depth,
 * communication bus) are stored in platform/board-specific context structures.
 *
 * The interface is intentionally open: a display may be a matrix of LEDs, a
 * small LCD, an OLED panel, or a segment display, and the contract says only
 * what every display can do. A platform or driver that brings a richer
 * capability declares it in its own header and a program that needs it asks
 * for that rather than for this.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_DISPLAY_H
#define INCLUDE_ROBOHUB_HAL_DISPLAY_H

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
/*  Pixel formats                                                             */
/* -------------------------------------------------------------------------- */
/* What a pixel is. A display that is one colour needs only one bit; a display
 * that is many colours needs more. A program that writes a pixel says which
 * format it means, and a display that cannot do that format says so. */
#define ROBOHUB_DISPLAY_FORMAT_MONO  0u  /* one bit per pixel */
#define ROBOHUB_DISPLAY_FORMAT_GRAY  1u  /* eight bits, grey */
#define ROBOHUB_DISPLAY_FORMAT_RGB565 2u /* sixteen bits, RGB */

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */
typedef struct RoboHubDisplay {
    /* Instance metadata */
    rh_name name;       /* e.g. "user_display", "led_matrix" */
    void*   context;    /* platform/board-specific data */

    /* Display geometry */
    rh_u32 width;       /* width in pixels */
    rh_u32 height;      /* height in pixels */
    rh_u32 format;      /* pixel format, one of ROBOHUB_DISPLAY_FORMAT_* */

    /* HAL operations */
    RoboHubError (*init)(struct RoboHubDisplay* self);
    RoboHubError (*deinit)(struct RoboHubDisplay* self);
    RoboHubError (*clear)(struct RoboHubDisplay* self);
    RoboHubError (*set_pixel)(struct RoboHubDisplay* self, rh_u32 x, rh_u32 y, rh_u32 value);
    RoboHubError (*get_pixel)(struct RoboHubDisplay* self, rh_u32 x, rh_u32 y, rh_u32* value);
    RoboHubError (*write_text)(struct RoboHubDisplay* self, const char* text, rh_u32 x, rh_u32 y);
    RoboHubError (*refresh)(struct RoboHubDisplay* self);
    RoboHubError (*set_brightness)(struct RoboHubDisplay* self, rh_u32 level);
} RoboHubDisplay;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_DISPLAY_H */
