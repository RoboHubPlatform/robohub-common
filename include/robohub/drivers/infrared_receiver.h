/**
 * @file infrared_receiver.h
 * @brief Driver interface - an infrared receiver for RoboHub Platform.
 *
 * The other end of `RoboHubInfrared`: a module that receives the codes a remote
 * sends and says what it heard. What arrives is a burst of pulses, and a
 * protocol — NEC, RC5, whichever the remote speaks — is what gives the burst a
 * meaning, so the driver decodes and the program reads a code.
 *
 * A code is a number and a protocol says how it was made, so both are reported:
 * the code a caller compares with the remote it expects, and the protocol that
 * decoded it. A code that was not understood is reported as none, which is
 * different from a code that was.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_INFRARED_RECEIVER_H
#define INCLUDE_ROBOHUB_DRIVERS_INFRARED_RECEIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/** The protocols a driver may report having decoded a code in. */
#define ROBOHUB_INFRARED_PROTOCOL_NONE 0u
#define ROBOHUB_INFRARED_PROTOCOL_NEC   1u
#define ROBOHUB_INFRARED_PROTOCOL_RC5   2u

/**
 * @brief One infrared receiver, whatever protocol it decodes.
 */
typedef struct RoboHubInfraredReceiver RoboHubInfraredReceiver;

struct RoboHubInfraredReceiver {
    /** The part's name, such as "ir_rx". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubInfraredReceiver* self);
    RoboHubError (*deinit)(RoboHubInfraredReceiver* self);
    /**
     * Read the code received since the last read.
     *
     * `received` says whether a code was understood at all; `code` is what it
     * was. A code that was not decoded leaves `received` false and the code
     * zero.
     */
    RoboHubError (*read)(RoboHubInfraredReceiver* self, rh_u32* code, rh_bool* received);
    /** The protocol the last code was decoded in. */
    rh_u8 (*protocol)(RoboHubInfraredReceiver* self);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_INFRARED_RECEIVER_H */
