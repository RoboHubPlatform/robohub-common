/**
 * @file plug.h
 * @brief Device interface - a switch with a power meter, for RoboHub Platform.
 *
 * A smart plug is two statements about one part: whether the outlet is on and
 * what it is drawing. Keeping them in one interface lets a caller treat a
 * Shelly, a metered relay or a future plug the same way, and lets a device that
 * has no meter report `ROBOHUB_ERR_NOT_SUPPORTED` for the reading alone.
 *
 * The switch half is the same shape as `RoboHubSwitch`; the power half is the
 * same reading an inverter produces.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DEVICES_PLUG_H
#define INCLUDE_ROBOHUB_DEVICES_PLUG_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"
#include "robohub/devices/power.h"

typedef struct RoboHubPlug RoboHubPlug;

struct RoboHubPlug {
    /** The device's name, such as "shelly-plug". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubPlug* self);
    RoboHubError (*deinit)(RoboHubPlug* self);

    /** Read whether the outlet is on. */
    RoboHubError (*get)(RoboHubPlug* self, rh_bool* on);
    /** Drive the outlet on or off. */
    RoboHubError (*set)(RoboHubPlug* self, rh_bool on);

    /** Read the meter; a plug without one returns `ROBOHUB_ERR_NOT_SUPPORTED`. */
    RoboHubError (*read_power)(RoboHubPlug* self, RoboHubPowerReading* out);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_PLUG_H */
