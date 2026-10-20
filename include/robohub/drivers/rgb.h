/**
 * @file rgb.h
 * @brief Driver interface - a colour output for RoboHub Platform.
 *
 * An RGB LED, a two-colour LED, a strip of them: a part whose colour is set
 * rather than whose on or off is set. It is its own interface because the colour
 * is the point — a red, a green and a blue mixed — and a program that wants
 * amber should not be adding up three switches.
 *
 * Each channel is a byte, 0 to 255, which is how a colour is usually written and
 * how a caller thinks of one; a driver whose part has fewer levels quantises, and
 * one whose part has no blue — a two-colour LED — ignores the blue it is given.
 * A driver that cannot light a channel says so through `channels`.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_RGB_H
#define INCLUDE_ROBOHUB_DRIVERS_RGB_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One colour output, whatever part it is.
 */
typedef struct RoboHubRgb RoboHubRgb;

struct RoboHubRgb {
    /** The part's name, such as "status" or "indicator". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubRgb* self);
    RoboHubError (*deinit)(RoboHubRgb* self);
    /** How many independent colour channels the part has: 1, 2 or 3. */
    rh_u8 (*channels)(RoboHubRgb* self);
    /** Set the colour, each channel 0 to 255. A part with fewer ignores the rest. */
    RoboHubError (*set)(RoboHubRgb* self, rh_u8 red, rh_u8 green, rh_u8 blue);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_RGB_H */
