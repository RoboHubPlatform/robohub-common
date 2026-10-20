/**
 * @file light.h
 * @brief Driver interface - a light sensor for RoboHub Platform.
 *
 * A photoresistor, a photodiode, a light-to-frequency part: a device that
 * reports how much light reaches it. A program is written against this and not
 * against a part number, so the same code reads day from night on whichever
 * sensor the board carries.
 *
 * The reading is a level from 0 to 1, dark to full scale of the part, rather
 * than lux. Lux is a calibrated quantity, and a part that reports it carries a
 * calibration that belongs to that part; a level is what an uncalibrated sensor
 * actually gives, and a caller that wants a threshold compares it to one. A
 * driver for a calibrated part may report lux as its full-scale level and say so.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_LIGHT_H
#define INCLUDE_ROBOHUB_DRIVERS_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One light sensor, whatever part it is.
 */
typedef struct RoboHubLight RoboHubLight;

struct RoboHubLight {
    /** The part's name, such as "photoresistor" or "ambient". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubLight* self);
    RoboHubError (*deinit)(RoboHubLight* self);
    /** Read the light level, 0 (dark) to 1 (full scale of the part). */
    RoboHubError (*get_level)(RoboHubLight* self, rh_f32* level);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_LIGHT_H */
