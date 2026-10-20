/**
 * @file pwm.h
 * @brief Hardware Abstraction Layer - Pulse Width Modulation interface.
 *
 * A pin that is on or off is a GPIO; a pin that is on for a fraction of every
 * cycle is a PWM. That fraction is what dims an LED, sets the speed of a motor,
 * and — at a fixed frequency — is the carrier an infrared remote modulates its
 * code onto. This is the contract for the part of a machine that produces it, so
 * a driver says a duty and a frequency and not how many timers or which compare
 * register.
 *
 * Duty is a fraction from 0 to 1, and frequency is hertz. Both are the driver's
 * to state and the platform's to place: which timer a channel reaches, and
 * whether a channel can hold the frequency asked for, are facts about the part.
 * A platform that cannot meet a request returns `ROBOHUB_ERR_NOT_SUPPORTED`
 * rather than a duty that is not the one asked for.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_PWM_H
#define INCLUDE_ROBOHUB_HAL_PWM_H

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
 * @brief One pulse-width-modulation peripheral, as a driver sees it.
 *
 * `channel` names an output of the peripheral, not a pin. A frequency is set on
 * the peripheral — several channels of one timer share it — and a duty is set
 * per channel.
 */
typedef struct RoboHubPWM {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubPWM* self);
    RoboHubError (*deinit)(struct RoboHubPWM* self);
    /** The number of output channels the peripheral has. */
    rh_u8 (*channels)(struct RoboHubPWM* self);
    /** Set the carrier frequency shared by the channels. */
    RoboHubError (*set_frequency)(struct RoboHubPWM* self, rh_u32 frequency_hz);
    /** Set a channel's duty as a fraction from 0 to 1. */
    RoboHubError (*set_duty)(struct RoboHubPWM* self, rh_u8 channel, rh_f32 duty);
    /** Stop a channel's output. */
    RoboHubError (*stop)(struct RoboHubPWM* self, rh_u8 channel);
} RoboHubPWM;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_PWM_H */
