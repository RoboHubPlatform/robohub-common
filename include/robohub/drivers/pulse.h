/**
 * @file pulse.h
 * @brief Driver interface - a pulse for RoboHub Platform.
 *
 * Some things are not a level but a rhythm: a heartbeat, a flicker, a knock
 * repeated. A part that reports a waveform is read here as the rate at which
 * something happened, which is the question a program actually asks of it.
 *
 * The reading is a rate in beats per minute, and whether a beat has passed since
 * the last read — so a caller that wants the rhythm and a caller that wants to
 * know something happened right now are both served by one read.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_PULSE_H
#define INCLUDE_ROBOHUB_DRIVERS_PULSE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One pulse source, whatever the rhythm is.
 */
typedef struct RoboHubPulse RoboHubPulse;

struct RoboHubPulse {
    /** The part's name, such as "heartbeat" or "flame". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubPulse* self);
    RoboHubError (*deinit)(RoboHubPulse* self);
    /**
     * Read the rate, in beats per minute, and whether a beat has passed since
     * the last read.
     *
     * `beats_per_minute` is zero when nothing has been seen yet; the rate is
     * only meaningful once there is a rhythm to measure.
     */
    RoboHubError (*read)(RoboHubPulse* self, rh_u16* beats_per_minute, rh_bool* beat);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_PULSE_H */
