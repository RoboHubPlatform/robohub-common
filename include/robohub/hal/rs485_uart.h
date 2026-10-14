/**
 * @file rs485_uart.h
 * @brief Hardware Abstraction Layer - RS485 built on a UART and a direction pin.
 *
 * RS485 is not a peripheral on most parts. It is a UART, plus a transceiver that
 * has to be told which way the line is being driven: a MAX485, a MAX3485, an
 * SN75176. The UART carries the bytes and the transceiver's DE/RE pin decides
 * whether the host is talking or the bus is. That is one extra pin, and this is
 * the one place that knows it.
 *
 * So an RS485 bus is built from a `RoboHubUART` and a `RoboHubGPIO`, exactly as
 * the common register bus is built from an I2C or SPI controller. A board that
 * fits a MAX485 binds the UART it is wired to and the pin the transceiver's DE
 * is on, and gets a `RoboHubRS485` a Modbus master or any other protocol uses
 * without knowing the transceiver exists. A host whose adapter has automatic
 * direction passes no pin and gets the same interface.
 *
 * What the pin means, and the one timing detail it carries: the direction goes
 * to transmit before the first byte and back to receive after the last, and the
 * last byte cannot be considered gone when the UART's write returns — the write
 * waits for room in the shift register, not for the register to empty. Releasing
 * the line while the final character is still going out truncates it, so the
 * wait for the last character is part of the write, taken from the timer when
 * one was given and left to the platform otherwise.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_RS485_UART_H
#define INCLUDE_ROBOHUB_HAL_RS485_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"
#include "robohub/hal/rs485.h"
#include "robohub/hal/uart.h"
#include "robohub/hal/gpio.h"
#include "robohub/hal/timer.h"

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief An RS485 bus over a UART and a transceiver direction pin.
 *
 * A value the caller owns. Its first member is the `RoboHubRS485` it presents,
 * so the address of the struct is the address of the interface and a protocol
 * may hold either. `de` may be null, which is a bus whose direction the hardware
 * handles itself — a USB-RS485 adapter with automatic direction — and then
 * `set_direction` is a call that does nothing rather than an error.
 */
typedef struct {
    RoboHubRS485  interface;      /* Must remain the first member. */
    RoboHubUART*  uart;           /* The link the transceiver is on. */
    RoboHubGPIO*  de;             /* Direction pin, or null when automatic. */
    rh_u32        de_pin;         /* Pin on that GPIO. */
    rh_bool       de_active_high; /* Whether driving high is transmit. */
    RoboHubTimer* timer;          /* For the inter-character wait and timeouts. */
    rh_u32        baudrate;       /* As the adapter was brought up. */
} RoboHubRS485Uart;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Bind a UART and a direction pin as an RS485 bus.
 *
 * `de` may be NULL for automatic direction. When it is not, `de_active_high`
 * says whether a high level drives the line (a DE pin on the part) or a low does
 * (an inverted enable). The instance is filled in and returned; the caller
 * registers it or passes `&self->interface` where a bus is wanted.
 *
 * The UART and the GPIO must outlive the bus.
 */
RoboHubError robohub_rs485_uart(RoboHubRS485Uart* self,
                                rh_name name,
                                RoboHubUART* uart,
                                RoboHubGPIO* de,
                                rh_u32 de_pin,
                                rh_bool de_active_high,
                                RoboHubTimer* timer);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_RS485_UART_H */
