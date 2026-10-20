/**
 * @file infrared.h
 * @brief Driver interface - an infrared transmitter for RoboHub Platform.
 *
 * An IR LED sends a code as a carrier burst that is on and off in a pattern: a
 * mark of some microseconds, a space of some microseconds, again and again. The
 * pattern is the protocol's — NEC, RC5, whatever the part is remote-controlling —
 * and this interface carries the pattern and not the protocol, so a protocol
 * layer sits above it and a driver below it drives the LED.
 *
 * `send` takes a sequence of durations in microseconds, alternating mark (LED on)
 * and space (LED off), starting with a mark. A driver modulates them onto its
 * carrier — 38 kHz for most parts — and returns when the sequence has been sent.
 * A caller with a protocol builds the sequence; a caller comparing two remotes
 * captures one.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_INFRARED_H
#define INCLUDE_ROBOHUB_DRIVERS_INFRARED_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One infrared transmitter, whatever part it is.
 */
typedef struct RoboHubInfrared RoboHubInfrared;

struct RoboHubInfrared {
    /** The part's name, such as "ir_tx". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubInfrared* self);
    RoboHubError (*deinit)(RoboHubInfrared* self);
    /** The carrier frequency in hertz, such as 38000. */
    rh_u32 (*carrier_hz)(RoboHubInfrared* self);
    /**
     * Send a mark/space sequence, in microseconds, starting with a mark.
     *
     * `durations[0]` is a mark, `durations[1]` a space, and so on; an odd count
     * ends on a mark. Blocks until the sequence has been sent.
     */
    RoboHubError (*send)(RoboHubInfrared* self, const rh_u32* durations, rh_size count);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_INFRARED_H */
