/**
 * @file inverter.h
 * @brief Device interface - an inverter or power device for RoboHub Platform.
 *
 * The `drivers/` interfaces describe a part on a bus: a temperature sensor, a
 * barometer. This is a step above them. An inverter is not read over a bus — it
 * is spoken to in Modbus registers or answered over HTTP — and what it reports is
 * a set of power and energy figures, not one number. So it is a device
 * interface, and it lives under `devices/` with the identity a gateway knows it
 * by.
 *
 * A reading is one statement about the device at one moment: how much it is
 * generating, what the load, grid and battery are doing, how full the battery
 * is, and how much energy has moved today and in total. Not every device has
 * every figure — a microinverter has no battery — so a reading carries a mask of
 * which fields are present, and a field whose bit is clear is absent rather than
 * zero. Zero is a real reading (an inverter at night generates nothing); absent
 * is a device that has no such thing.
 *
 * The concrete integrations live in `robohub-devices`: the Growatt register map
 * over Modbus, the Hoymiles figures over HTTP. This header is what they present,
 * so a program — the device node that publishes to a gateway — is written once
 * against `RoboHubInverter` and handed whichever device it is configured with.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DEVICES_INVERTER_H
#define INCLUDE_ROBOHUB_DEVICES_INVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"
#include "robohub/devices/device.h"
#include "robohub/devices/power.h"



/* -------------------------------------------------------------------------- */
/*  Interface                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief One inverter, whatever part it is and however it is reached.
 *
 * `read` is the whole of it: a device brings itself up, is read for one
 * statement about itself, and is taken down. It is deliberately not a set of
 * per-figure getters — the device is one part answering one query, and reading
 * the figures separately would be several queries to it and a disagreement
 * between them.
 */
typedef struct RoboHubInverter RoboHubInverter;

struct RoboHubInverter {
    /** The device's name, such as "growatt" or "hoymiles". */
    rh_name name;
    /** Driver state: the integration struct whose member this is. */
    void*   context;

    /** Bring the device up. Called once before any read. */
    RoboHubError (*init)(RoboHubInverter* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubInverter* self);

    /**
     * Read the device's power and energy.
     *
     * Fills in `out`, including the `valid` mask of the figures the device has
     * this time. Returns `ROBOHUB_ERR_NOT_FOUND` when the device does not answer,
     * which is the failure a caller acts on.
     */
    RoboHubError (*read)(RoboHubInverter* self, RoboHubPowerReading* out);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_INVERTER_H */
