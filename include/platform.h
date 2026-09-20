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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
