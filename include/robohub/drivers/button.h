/**
 * @file button.h
 * @brief Driver interface - a button or any discrete contact for RoboHub.
 *
 * A button, a key switch, a tilt switch, a vibration switch, a knock sensor:
 * what they have in common is one bit — the contact is closed or it is not —
 * and what differs is only the part that moves it. A program is written against
 * this and not against a part number, so the same code reads a press, a tilt and
 * a knock.
 *
 * `pressed` is the state of the contact as the driver reads it, which is the
 * switch closed, not the voltage: a driver knows whether its part is active high
 * or low, and a caller does not have to.
 *
 * A momentary part — a knock, a tap — is read as pressed only while it is
 * closed, and a caller that must not miss a short one polls faster than the
 * contact or uses a driver that latches it. The interface reports the contact;
 * catching an edge is the caller's or a driver's above it.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DRIVERS_BUTTON_H
#define INCLUDE_ROBOHUB_DRIVERS_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/**
 * @brief One discrete contact, whatever moves it.
 */
typedef struct RoboHubButton RoboHubButton;

struct RoboHubButton {
    /** The part's name, such as "key" or "tilt". */
    rh_name name;
    /** Driver state: the struct whose member this is. */
    void*   context;

    RoboHubError (*init)(RoboHubButton* self);
    RoboHubError (*deinit)(RoboHubButton* self);
    /** Read the contact: true when it is closed. */
    RoboHubError (*get)(RoboHubButton* self, rh_bool* pressed);
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DRIVERS_BUTTON_H */
