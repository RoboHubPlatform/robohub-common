/**
 * @file pressure.h
 * @brief Driver interface - a pressure sensor for RoboHub Platform.
 *
 * The HAL describes what a chip can do; this describes what a kind of device is.
 * A pressure sensor is the second of the driver interfaces, after temperature,
 * and it is the same shape: a program is written against this and not against a
 * part number, asks for a `RoboHubPressure`, and reads the pressure from whichever
 * part the board carries.
 *
 * The unit is the pascal, in `rh_f32`. It is chosen because it is the SI unit and
 * because a call site that wants a different one can convert exactly; a driver
 * that reported hectopascals or millibars would have made that choice for every
 * caller. `robohub_pressure_pa_to_hpa` and its inverse live in
 * `pressure_utils.h` for the common conversions.
 *
 * This lives under `drivers/` rather than beside the HAL interfaces because it is
 * a different layer in the same way `interfaces` and `platform` are: a driver
 * implements it, and a platform never does.
 *
 * The driver fills the struct in as a value the caller owns — no constructor, no
 * allocation, nothing to free. The first member of a driver's own device struct
 * is one driver interface, and every interface the device presents has
 * `context` pointing back at the device, which is what lets a caller hold any of
 * them and a driver recover the device behind it.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_PRESSURE_H
#define INCLUDE_ROBOHUB_DRIVERS_PRESSURE_H

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
 * @brief One pressure sensor, whatever part it is.
 *
 * A driver that cannot answer a call says so with `ROBOHUB_ERR_NOT_SUPPORTED`
 * rather than approximating it.
 */
typedef struct RoboHubPressure RoboHubPressure;

struct RoboHubPressure {
    /** The device's name, such as "barometer" or "bmp180". */
    rh_name name;
    /** Driver state: the device struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any read. */
    RoboHubError (*init)(RoboHubPressure* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubPressure* self);

    /** Read the pressure in pascals. */
    RoboHubError (*get_pressure)(RoboHubPressure* self, rh_f32* pascals);
};

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

/* The conversions are static inline helpers in pressure_utils.h, kept out of
 * this header because a foreign-interface reader reads a header as declarations
 * and cannot read a function body. */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_PRESSURE_H */
