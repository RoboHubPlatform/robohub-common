/**
 * @file pressure_utils.h
 * @brief Driver interface - helpers over the pressure contract.
 *
 * The conversions between the pascals a `RoboHubPressure` reports and the units
 * a person reads. They are kept out of `pressure.h` because that header is read
 * by a C importer — `rho cimport` writes a foreign interface from it — and a
 * header a C importer reads is declarations: a `static inline` function is a
 * body, and a reader that reads declarations cannot read one.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_PRESSURE_UTILS_H
#define INCLUDE_ROBOHUB_DRIVERS_PRESSURE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"

/* Hectopascals from pascals. One hectopascal is one millibar, which is the unit
 * a weather figure is quoted in. */
static inline rh_f32 robohub_pressure_pa_to_hpa(rh_f32 pascals)
{
    return pascals / 100.0f;
}

/* Pascals from hectopascals. */
static inline rh_f32 robohub_pressure_hpa_to_pa(rh_f32 hectopascals)
{
    return hectopascals * 100.0f;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_PRESSURE_UTILS_H */
