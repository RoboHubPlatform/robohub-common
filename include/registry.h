/**
 * @file registry.h
 * @brief HAL instance registry contract for RoboHub Platform.
 *
 * This header defines the registry API used to store and retrieve HAL
 * instances (I2C, SPI, UART, GPIO, RS485, Timer) by name. Each board
 * registers its own instances during platform initialization.
 *
 * @project RoboHub Platform
 * @author Aurel
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_REGISTRY_H
#define INCLUDE_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"

#include "robohub/hal/i2c.h"
#include "robohub/hal/spi.h"
#include "robohub/hal/uart.h"
#include "robohub/hal/gpio.h"
#include "robohub/hal/rs485.h"
#include "robohub/hal/timer.h"
#include "robohub/hal/onewire.h"

/* -------------------------------------------------------------------------- */
/*  Initialization                                                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize all subsystem registries.
 *
 * May be called repeatedly; each call clears previous registrations.
 */
void robohub_registry_init(void);

/**
 * @brief Release registry-owned names and reset every registry.
 *
 * Registered HAL instances remain owned by the caller.
 */
void robohub_registry_deinit(void);

/* -------------------------------------------------------------------------- */
/*  Registration API                                                          */
/* -------------------------------------------------------------------------- */

RoboHubError robohub_register_i2c(rh_name name, RoboHubI2C* instance);
RoboHubError robohub_register_spi(rh_name name, RoboHubSPI* instance);
RoboHubError robohub_register_uart(rh_name name, RoboHubUART* instance);
RoboHubError robohub_register_gpio(rh_name name, RoboHubGPIO* instance);
RoboHubError robohub_register_rs485(rh_name name, RoboHubRS485* instance);
RoboHubError robohub_register_timer(rh_name name, RoboHubTimer* instance);
RoboHubError robohub_register_onewire(rh_name name, RoboHubOneWire* instance);

/* -------------------------------------------------------------------------- */
/*  Lookup API                                                                */
/* -------------------------------------------------------------------------- */

RoboHubI2C*  robohub_get_i2c(rh_name name);
RoboHubSPI*  robohub_get_spi(rh_name name);
RoboHubUART* robohub_get_uart(rh_name name);
RoboHubGPIO* robohub_get_gpio(rh_name name);
RoboHubRS485* robohub_get_rs485(rh_name name);
RoboHubTimer* robohub_get_timer(rh_name name);
RoboHubOneWire* robohub_get_onewire(rh_name name);

/* -------------------------------------------------------------------------- */
/*  Counts                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief How many instances of each kind are registered.
 *
 * A smoke asks whether the platform brought a bus up at all — "is a OneWire bus
 * registered" — without being configured with its name, and a board has one name
 * per bus and no way to enumerate them from the outside.
 */
rh_size robohub_count_i2c(void);
rh_size robohub_count_spi(void);
rh_size robohub_count_uart(void);
rh_size robohub_count_gpio(void);
rh_size robohub_count_rs485(void);
rh_size robohub_count_timer(void);
rh_size robohub_count_onewire(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_REGISTRY_H */
