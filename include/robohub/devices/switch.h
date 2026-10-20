/**
 * @file switch.h
 * @brief Device interface - a switch a program can drive for RoboHub Platform.
 *
 * The other device interfaces are read: a temperature sensor, an inverter. This
 * is one a program can act on — a relay, a smart plug's outlet, anything that is
 * on or off. It is the same shape, so a program holds a `RoboHubSwitch` and does
 * not know whether the thing behind it is a GPIO, a Modbus coil or a command over
 * a network.
 *
 * `set` is the state asked for and `get` is the state the device is in: they are
 * separate because a switch may refuse, may take time, or may be changed by
 * something else — a bypassed relay, a local button. A caller that has just
 * commanded a switch reads it back to know the state rather than assuming the
 * command took, and a node reports the read-back state to its gateway.
 *
 * A device that is only ever read does not present this interface, and a program
 * that has no switch does not link it; the interface is present exactly where
 * there is something to drive.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DEVICES_SWITCH_H
#define INCLUDE_ROBOHUB_DEVICES_SWITCH_H

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
 * @brief One switch, whatever drives it.
 *
 * A device that cannot answer a call says so with `ROBOHUB_ERR_NOT_SUPPORTED`
 * rather than guessing — a switch that cannot be read reports so, and one that
 * cannot be driven is only read.
 */
typedef struct RoboHubSwitch RoboHubSwitch;

struct RoboHubSwitch {
    /** The device's name, such as "relay" or "outlet". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any use. */
    RoboHubError (*init)(RoboHubSwitch* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubSwitch* self);

    /** Read whether the switch is on. */
    RoboHubError (*get)(RoboHubSwitch* self, rh_bool* on);
    /** Drive the switch on or off. */
    RoboHubError (*set)(RoboHubSwitch* self, rh_bool on);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_SWITCH_H */
