/**
 * @file temperature.h
 * @brief Driver interface - a temperature sensor for RoboHub Platform.
 *
 * The HAL describes what a chip can do; this describes what a kind of device
 * is. A temperature sensor is the first of the second sort, and the two are not
 * the same contract. A HAL interface is implemented once per platform and
 * reaches one peripheral; a driver interface is implemented once per device and
 * reaches the HAL.
 *
 * A program is written against this, not against a part number. It asks for a
 * `RoboHubTemperature`, is handed whichever sensor the board carries, and reads
 * degrees from it — a `DS18B20` on a 1-Wire bus, an `LM75` on I2C, an internal
 * sensor on a chip. Swapping the part is then a change of what is compiled in
 * and not a change of the program.
 *
 * This lives under `drivers/` rather than beside the HAL interfaces because it
 * is a different layer in the same way `interfaces` and `platform` are: a
 * driver implements it, and a platform never does. A platform that grew a
 * temperature method would be a platform that thinks it knows a device.
 *
 * The driver fills the struct in as a value the caller owns — the same shape
 * the HAL uses — so a device instance is a static with no constructor, no
 * allocation and nothing to free. The first member of a driver's own device
 * struct is this interface, which is what lets a caller hold the interface and
 * a driver recover the device behind it.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_H
#define INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_H

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
 * @brief One temperature sensor, whatever part it is.
 *
 * `name` and `context` carry what the HAL's do: the name a device was given and
 * the driver's own state. The operations are the whole of what a temperature
 * sensor is asked, and a driver that cannot answer one of them says so with
 * `ROBOHUB_ERR_NOT_SUPPORTED` rather than approximating it — a caller that is
 * told a sensor has no Fahrenheit reading can convert the one it has, where a
 * caller handed a wrong number cannot tell.
 */
typedef struct RoboHubTemperature RoboHubTemperature;

struct RoboHubTemperature {
    /** The device's name, such as "ambient" or "ds18b20". */
    rh_name name;
    /** Driver state: the device struct whose first member this is. */
    void*   context;

    /** Bring the device up. Called once before any read. */
    RoboHubError (*init)(RoboHubTemperature* self);
    /** Take the device down. The interface is unusable until init runs again. */
    RoboHubError (*deinit)(RoboHubTemperature* self);

    /** Read the temperature in degrees Celsius. */
    RoboHubError (*get_celsius)(RoboHubTemperature* self, rh_f32* celsius);
    /**
     * Read the temperature in degrees Fahrenheit.
     *
     * A driver whose device reports Celsius may leave this unset; a caller then
     * converts with `robohub_temperature_to_fahrenheit`. A driver that has the
     * Fahrenheit value natively — an infrared sensor that reports it in that
     * scale, say — fills it in and the conversion is never reached.
     */
    RoboHubError (*get_fahrenheit)(RoboHubTemperature* self, rh_f32* fahrenheit);
};

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

/* The conversions and the Fahrenheit fallback are in robohub/drivers/
 * temperature_utils.h. They are static inline helpers, and a foreign-interface
 * reader reads a header as declarations and cannot read a function body, so a
 * header a C importer parses carries no inline code. */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_H */
