/**
 * @file temperature_utils.h
 * @brief Driver interface - helpers over the temperature contract.
 *
 * The conversions and the fallback that asks a sensor for Fahrenheit whichever
 * method it offers. They are kept out of `temperature.h` because that header is
 * read by a C importer — `rho cimport` writes a foreign interface from it — and
 * a header a C importer reads is declarations: a `static inline` function is a
 * body, and a reader that reads declarations cannot read one.
 *
 * A C program includes this when it wants the helpers. A device driver does not
 * have to: it implements the interface and computes whatever it needs.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_UTILS_H
#define INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stddef.h>

#include "types.h"
#include "error.h"
#include "robohub/drivers/temperature.h"

/* Degrees Fahrenheit from degrees Celsius. */
static inline rh_f32 robohub_celsius_to_fahrenheit(rh_f32 celsius)
{
    return celsius * 9.0f / 5.0f + 32.0f;
}

/* Degrees Celsius from degrees Fahrenheit. */
static inline rh_f32 robohub_fahrenheit_to_celsius(rh_f32 fahrenheit)
{
    return (fahrenheit - 32.0f) * 5.0f / 9.0f;
}

/* Read a sensor in Fahrenheit, using whichever path it offers. A device that
 * implements the method is used directly; one that does not is read in Celsius
 * and converted here, so a caller is written once and works for both. */
static inline RoboHubError
robohub_temperature_read_fahrenheit(RoboHubTemperature* self, rh_f32* fahrenheit)
{
    if (self == NULL || fahrenheit == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    if (self->get_fahrenheit != NULL) {
        return self->get_fahrenheit(self, fahrenheit);
    }
    if (self->get_celsius == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    rh_f32 celsius = 0.0f;
    RoboHubError err = self->get_celsius(self, &celsius);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    *fahrenheit = robohub_celsius_to_fahrenheit(celsius);
    return ROBOHUB_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_TEMPERATURE_UTILS_H */
