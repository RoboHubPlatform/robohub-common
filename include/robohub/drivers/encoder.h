/**
 * @file encoder.h
 * @brief Driver interface - a rotary encoder for RoboHub Platform.
 *
 * A knob that turns: two signals whose order says which way, and a pulse for
 * each detent. A program asks how many steps passed and which way, and does not
 * read the two pins itself.
 *
 * The count is what passed since the last read, so a caller that reads rarely
 * loses nothing: the encoder does not stop turning because nobody asked. A
 * press of the knob's own switch is a separate thing — a button — and is
 * reported by the button interface when the part has one.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_ENCODER_H
#define INCLUDE_ROBOHUB_DRIVERS_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One rotary encoder, whatever produces the pulses.
 */
typedef struct RoboHubEncoder RoboHubEncoder;

struct RoboHubEncoder {
    /** The part's name, such as "knob". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubEncoder* self);
    RoboHubError (*deinit)(RoboHubEncoder* self);
    /**
     * Read the steps that passed since the last read.
     *
     * `steps` is how many detents went by, and `clockwise` which way; a step the
     * other way makes the count negative, so a caller that only wants the
     * magnitude can take the absolute value and one that wants the direction has
     * it.
     */
    RoboHubError (*read)(RoboHubEncoder* self, rh_i32* steps, rh_bool* clockwise);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_ENCODER_H */
