/**
 * @file platform.h
 * @brief Platform registration and initialization contract for RoboHub Platform.
 *
 * Each hardware platform (Pico, ESP32, STM32, Linux) must implement the
 * functions declared here. The application calls robohub_platform_init(),
 * which performs:
 *
 *  - registry initialization
 *  - board-specific HAL instance creation
 *  - HAL registration (I2C, SPI, UART, GPIO, RS485, Timer)
 *  - platform-specific initialization (clocks, pins, drivers)
 *  - conditional registration based on Kconfig/project.conf
 *
 * @project RoboHub Platform
 * @author Aurel
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_PLATFORM_H
#define INCLUDE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "error.h"

/**
 * @brief Initialize the active platform and register all HAL implementations.
 *
 * MUST be implemented by each platform:
 *  - robohub-pico
 *  - robohub-esp32
 *  - robohub-stm32
 *  - robohub-linux
 */
RoboHubError robohub_platform_init(void);

/**
 * @brief Board-specific registration hook.
 *
 * Each board (Pico, Pico W, ESP32-S3, STM32F4, Raspberry Pi) MUST implement
 * this function. It creates HAL instances, fills context structures, and
 * registers them using the registry API.
 */
RoboHubError robohub_board_register(void);

/**
 * @brief Fill `out` with `length` bytes no earlier call ever returned.
 *
 * A board's authenticated link (robohub-transport's secure.h) opens each session
 * with a nonce, and a nonce that repeats lets an old session be replayed. A part
 * with a hardware random source fills `out` from it; one without (an 8-bit part)
 * combines a boot counter kept in non-volatile memory with the time since boot,
 * which never repeats as a pair. Returns 0 when it filled `out`, and anything
 * else when it could not, in which case no session is opened.
 *
 * Each platform MUST implement this function.
 */
int robohub_platform_nonce(rh_u8* out, rh_size length);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PLATFORM_H */
