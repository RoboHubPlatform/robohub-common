/**
 * @file humidity.h
 * @brief Driver interface - a relative-humidity sensor for RoboHub Platform.
 *
 * The HAL describes what a chip can do; this describes what a kind of device is.
 * A humidity sensor is one of the driver interfaces, the same shape as
 * temperature and pressure: a program is written against this and not against a
 * part number, asks for a `RoboHubHumidity`, and reads the humidity from whichever
 * part the board carries.
 *
 * The value is relative humidity as a percentage, in `rh_f32`: 0 is dry air and
 * 100 is saturation. It is relative and not absolute because that is what these
 * parts measure — the same moisture in the air reads differently as the
 * temperature changes, which is why a part that reports both carries a
 * temperature as well.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_HUMIDITY_H
#define INCLUDE_ROBOHUB_DRIVERS_HUMIDITY_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief One humidity sensor, whatever part it is.
 */
typedef struct RoboHubHumidity RoboHubHumidity;

struct RoboHubHumidity {
    /** The device's name, such as "ambient" or "dht11". */
    rh_name name;
    /** Driver state: the device struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any read. */
    RoboHubError (*init)(RoboHubHumidity* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubHumidity* self);

    /** Read the relative humidity as a percentage, 0 to 100. */
    RoboHubError (*get_humidity)(RoboHubHumidity* self, rh_f32* percent);
};

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_HUMIDITY_H */
