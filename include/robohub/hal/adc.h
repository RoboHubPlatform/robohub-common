/**
 * @file adc.h
 * @brief Hardware Abstraction Layer - Analog to Digital Converter interface.
 *
 * Some parts do not speak on a bus and are not digital: a photoresistor, a
 * thermistor, a potentiometer. What they say is a voltage, and the part of a
 * machine that hears it is an ADC. This is the contract for one, so a driver
 * that reads an analog part is written against the conversion and not against
 * the pin, the reference or the sample-and-hold.
 *
 * A reading is a level from 0 to 1 rather than a count. The number of bits a
 * converter has is the platform's — 12 on one part, 10 on another — and a driver
 * that had to know it would be a driver for one part's converter. A caller that
 * wants counts, or a voltage, is given the level and the platform says what full
 * scale is; a caller that only compares two readings wants neither.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_ADC_H
#define INCLUDE_ROBOHUB_HAL_ADC_H

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
 * @brief One analog to digital converter, as a driver sees it.
 *
 * `channel` names an input of the converter, not a pin: which pin a channel
 * reaches is the board's to say, and a driver that named a pin would be a driver
 * for one board. A converter that cannot answer a call says so with
 * `ROBOHUB_ERR_NOT_SUPPORTED` rather than returning a level it did not measure.
 */
typedef struct RoboHubADC {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubADC* self);
    RoboHubError (*deinit)(struct RoboHubADC* self);
    /** The number of input channels the converter has. */
    rh_u8 (*channels)(struct RoboHubADC* self);
    /** The converter's resolution, in bits, so a caller may state a voltage. */
    rh_u8 (*resolution_bits)(struct RoboHubADC* self);
    /** Read a channel as a level from 0 to 1 of full scale. */
    RoboHubError (*read)(struct RoboHubADC* self, rh_u8 channel, rh_f32* level);
} RoboHubADC;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_ADC_H */
