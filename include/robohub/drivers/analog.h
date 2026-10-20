/**
 * @file analog.h
 * @brief Driver interface - an analog input for RoboHub Platform.
 *
 * Some parts say a voltage rather than a bit: a magnetometer, a hall sensor
 * that follows a field, a potentiometer, a strain gauge. What they have in
 * common is that the reading is a level, and a program is written against the
 * level rather than against the part that produced it.
 *
 * The level runs from 0 to 1 of the converter's full scale, so a caller compares
 * it to a threshold and never asks how many bits the converter had. A part whose
 * output is not centred — a hall sensor sitting at half scale with no field —
 * reports its own range through the driver that carries it.
 *
 * A sensor with a named quantity gets its own interface — light is light, a
 * temperature is a temperature — and this is for the ones that do not.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_ANALOG_H
#define INCLUDE_ROBOHUB_DRIVERS_ANALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One analog input, whatever produces it.
 */
typedef struct RoboHubAnalog RoboHubAnalog;

struct RoboHubAnalog {
    /** The part's name, such as "hall" or "pot". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubAnalog* self);
    RoboHubError (*deinit)(RoboHubAnalog* self);
    /** Read the level, 0 (lowest) to 1 (full scale of the part). */
    RoboHubError (*get_level)(RoboHubAnalog* self, rh_f32* level);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_ANALOG_H */
