/**
 * @file onewire.h
 * @brief Hardware Abstraction Layer - 1-Wire bus interface for RoboHub Platform.
 *
 * A 1-Wire bus is one data line, a pull-up and nothing else. What makes it a
 * contract rather than a GPIO a driver toggles itself is that its timing is the
 * protocol: a reset pulse is 480 us low, a write slot is a low pulse of 1 to 15
 * us, and a read samples the line a few microseconds after a low pulse starts.
 * A driver that measured those in software would be measuring them through
 * whatever the platform's delay happens to cost, which is a different number on
 * every chip and a different number again once an interrupt lands in the middle
 * of a slot.
 *
 * So the bus is delegated. A platform implements these calls against whatever
 * makes its timing exact — a PIO program, a timer-driven bit-bang, a UART in
 * half duplex, a hardware 1-Wire peripheral — and a device driver calls them
 * without knowing which. The driver speaks the device's protocol (SKIP ROM,
 * CONVERT T, READ SCRATCHPAD); the platform speaks the wire.
 *
 * Bytes and blocks, not slots. A device driver is written in terms of the bytes
 * of its protocol, and a caller that had to assemble each byte from eight bit
 * slots would be reimplementing the shift register in every driver. Bit-level
 * timing belongs to the implementation.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_ONEWIRE_H
#define INCLUDE_ROBOHUB_HAL_ONEWIRE_H

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
 * @brief One 1-Wire bus, as a device driver sees it.
 *
 * The operations are the whole of the wire: reset and presence, write a byte,
 * read a byte, and the block forms of the last two. A bus that can only do one
 * of them is not a bus this contract describes, and an implementation that
 * cannot provide one returns `ROBOHUB_ERR_NOT_SUPPORTED` rather than a wrong
 * answer.
 */
typedef struct RoboHubOneWire {
    /* Instance metadata */
    rh_name name;       /* e.g. "onewire0" */
    void*   context;    /* platform-specific: the pin, the PIO, the UART */

    /* HAL operations */
    RoboHubError (*init)(struct RoboHubOneWire* self);
    RoboHubError (*deinit)(struct RoboHubOneWire* self);
    /* Reset and report presence; nothing on the bus is not an error */
    RoboHubError (*reset)(struct RoboHubOneWire* self, rh_bool* present);
    RoboHubError (*write_byte)(struct RoboHubOneWire* self, rh_u8 byte);
    RoboHubError (*read_byte)(struct RoboHubOneWire* self, rh_u8* byte);
    RoboHubError (*write)(struct RoboHubOneWire* self, const rh_u8* data, rh_size len);
    RoboHubError (*read)(struct RoboHubOneWire* self, rh_u8* data, rh_size len);
} RoboHubOneWire;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_ONEWIRE_H */
