/**
 * @file tone.h
 * @brief Driver interface - a tone output for RoboHub Platform.
 *
 * A passive buzzer, a small speaker, a piezo: a part that makes a sound whose
 * pitch you choose. It is not the same as a switch — the point is the frequency,
 * not on or off — so it is its own interface, and a program is written against it
 * whether the part is driven by a PWM channel or by a pin toggled at the
 * frequency asked for.
 *
 * `play` starts a tone and returns; the drone of a frequency is not something to
 * wait on. `stop` silences it. A driver whose part has no volume control ignores
 * that; the interface carries a frequency and nothing else.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_TONE_H
#define INCLUDE_ROBOHUB_DRIVERS_TONE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One tone output, whatever makes the sound.
 */
typedef struct RoboHubTone RoboHubTone;

struct RoboHubTone {
    /** The part's name, such as "buzzer". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubTone* self);
    RoboHubError (*deinit)(RoboHubTone* self);
    /** Start a tone at a frequency in hertz. */
    RoboHubError (*play)(RoboHubTone* self, rh_u32 frequency_hz);
    /** Stop the tone. */
    RoboHubError (*stop)(RoboHubTone* self);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_TONE_H */
