/**
 * @file power.h
 * @brief The power and energy a metered device reports.
 *
 * An inverter and a smart plug both state the same figures: what they generate,
 * what a load draws, what the grid or a battery is doing, and the energy
 * counters. The reading is one type so a consumer does not learn a second shape
 * for a second part.
 *
 * @project RoboHub Platform
 * @license MIT
 */
#ifndef INCLUDE_ROBOHUB_DEVICES_POWER_H
#define INCLUDE_ROBOHUB_DEVICES_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
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

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_POWER_H */
