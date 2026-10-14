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

/* -------------------------------------------------------------------------- */
/*  Reading                                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Which figures a reading carries.
 *
 * A set of bits, one per field. A bit that is clear means the device does not
 * report that figure, which is different from reporting it as zero. The numbers
 * are the schema's `PowerField`, so the mask crosses the wire unchanged.
 */
typedef enum {
    ROBOHUB_POWER_PV_POWER        = 1u << 0, /**< generating, watts */
    ROBOHUB_POWER_LOAD_POWER      = 1u << 1, /**< household load, watts */
    ROBOHUB_POWER_GRID_POWER      = 1u << 2, /**< + import, - export, watts */
    ROBOHUB_POWER_BATTERY_POWER   = 1u << 3, /**< + charge, - discharge, watts */
    ROBOHUB_POWER_BATTERY_SOC     = 1u << 4, /**< state of charge, percent */
    ROBOHUB_POWER_ENERGY_TODAY    = 1u << 5, /**< energy today, kWh */
    ROBOHUB_POWER_ENERGY_TOTAL    = 1u << 6  /**< lifetime energy, kWh */
} RoboHubPowerField;

/**
 * @brief One device's power and energy at one moment.
 *
 * `valid` says which of the figures this reading carries; a field not named
 * there is absent and its value is not to be read. Power is signed where the
 * direction matters — grid and battery are positive in one direction and
 * negative in the other — and the sign convention is stated per field above, so
 * a consumer need not know the part.
 */
typedef struct {
    /** The device the reading is of. */
    RoboHubDeviceId device;
    /** Milliseconds on the reporting device's clock, as a duration. */
    rh_u32          timestamp_ms;
    /** What the device is. */
    RoboHubDeviceKind kind;
    /** The schema's `NodeStatus`: 0 is OK, 1 is a fault. */
    rh_u16          status;
    /** Which of the figures below are present. */
    rh_u16          valid;

    rh_f32 pv_power_w;        /**< ROBOHUB_POWER_PV_POWER */
    rh_f32 load_power_w;      /**< ROBOHUB_POWER_LOAD_POWER */
    rh_f32 grid_power_w;      /**< ROBOHUB_POWER_GRID_POWER */
    rh_f32 battery_power_w;   /**< ROBOHUB_POWER_BATTERY_POWER */
    rh_f32 battery_soc;       /**< ROBOHUB_POWER_BATTERY_SOC, percent */
    rh_f32 energy_today_kwh;  /**< ROBOHUB_POWER_ENERGY_TODAY */
    rh_f32 energy_total_kwh;  /**< ROBOHUB_POWER_ENERGY_TOTAL */
} RoboHubPowerReading;

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
