/**
 * @file altitude.h
 * @brief Driver interface - an altitude sensor for RoboHub Platform.
 *
 * Some parts measure altitude directly; the ordinary case is a barometer whose
 * altitude is computed from pressure against a reference. Either is a
 * `RoboHubAltitude`, and a program is written against that rather than against
 * the arithmetic.
 *
 * Altitude is a derived quantity, so the interface carries the one thing that
 * makes a derivation meaningful: the reference pressure the reading is relative
 * to. A driver whose part reports altitude itself may ignore `set_reference` and
 * return `ROBOHUB_ERR_NOT_SUPPORTED`, and a caller that never sets one gets the
 * driver's own default — the standard sea-level pressure, 101325 Pa, for a
 * barometric part.
 *
 * The unit is the metre, in `rh_f32`, and it is the height above the reference
 * the part was given, not above any particular place on the ground.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_ALTITUDE_H
#define INCLUDE_ROBOHUB_DRIVERS_ALTITUDE_H

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
 * @brief One altitude sensor, whatever part it is.
 */
typedef struct RoboHubAltitude RoboHubAltitude;

struct RoboHubAltitude {
    /** The device's name, such as "altimeter" or "bmp180". */
    rh_name name;
    /** Driver state: the device struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any read. */
    RoboHubError (*init)(RoboHubAltitude* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubAltitude* self);

    /**
     * Set the sea-level pressure the altitude is measured against, in pascals.
     *
     * A driver that computes altitude from pressure uses this; one whose part
     * reports altitude itself may leave it null.
     */
    RoboHubError (*set_reference)(RoboHubAltitude* self, rh_f32 sea_level_pa);

    /** Read the altitude in metres above the reference. */
    RoboHubError (*get_altitude)(RoboHubAltitude* self, rh_f32* metres);
};

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

/** The standard sea-level pressure, in pascals, a barometric part defaults to. */
#define ROBOHUB_SEA_LEVEL_PRESSURE_PA 101325.0f

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_ALTITUDE_H */
