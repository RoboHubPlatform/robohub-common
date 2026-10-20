/**
 * @file actuator.h
 * @brief Device interface - an actuator with a value, for RoboHub Platform.
 *
 * A switch is on or off; this is the device that takes a number — a thermostat's
 * setpoint, a charger's current limit, a dimmer, a valve. Like the switch it is
 * written, not only read, and `get` reports the value the device is at rather
 * than the value it was told, because the two can differ.
 *
 * The value is an `rh_f32` and its meaning is the device's: degrees for a
 * thermostat, amperes for a charger, percent for a dimmer. What a value means is
 * documented by the concrete device, not carried here, because a program that
 * holds a `RoboHubActuator` has been given it for a purpose and knows the
 * purpose; the interface carries the mechanism and not the units.
 *
 * A caller that wants a range trusted to the device drives it and reads it back.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DEVICES_ACTUATOR_H
#define INCLUDE_ROBOHUB_DEVICES_ACTUATOR_H

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
 * @brief One actuator, whatever it moves.
 */
typedef struct RoboHubActuator RoboHubActuator;

struct RoboHubActuator {
    /** The device's name, such as "setpoint" or "dimmer". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any use. */
    RoboHubError (*init)(RoboHubActuator* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubActuator* self);

    /** Read the value the device is at. */
    RoboHubError (*get)(RoboHubActuator* self, rh_f32* value);
    /** Ask the device to move to a value. */
    RoboHubError (*set)(RoboHubActuator* self, rh_f32 value);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_ACTUATOR_H */
