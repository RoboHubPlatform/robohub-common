/**
 * @file dht.h
 * @brief Hardware Abstraction Layer - DHT single-wire sensor interface.
 *
 * A DHT11 or DHT22 is reached over one data line, and the protocol on it is
 * timing and nothing else: the host pulls the line low for at least 18 ms to ask
 * for a measurement, releases it, and then the part answers with 40 bits, each
 * bit a low pulse followed by a high pulse whose length says whether it is a one
 * or a zero. There is no clock line and no address; the length of a pulse is the
 * whole of the message.
 *
 * That is why the wire is delegated rather than bit-banged in a driver, for the
 * same reason 1-Wire is. A driver that measured those pulses in software would
 * be measuring them through whatever the platform's delay and read happen to
 * cost, which is a different number on every chip and a different number again
 * once an interrupt lands inside a bit. A part's 26 µs and a part's 70 µs are
 * close enough that the wrong side of the measurement is a wrong byte.
 *
 * So the platform implements these calls against whatever makes its timing exact
 * — a PIO program, a timer-driven sampler, a capture peripheral — and returns
 * the frame the part sent. The driver speaks the frame: which bytes are humidity,
 * which are temperature, and how the checksum is computed. The platform speaks
 * the wire.
 *
 * Bytes, not bits or microseconds. A caller that had to assemble each byte from
 * eight measured pulses would be reimplementing the sampler in every driver, and
 * the timing that makes it correct would live in the driver rather than where the
 * clock is.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_DHT_H
#define INCLUDE_ROBOHUB_HAL_DHT_H

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

/** The number of bytes in a DHT measurement frame: humidity integer and
 *  decimal, temperature integer and decimal, and a checksum. */
#define ROBOHUB_DHT_FRAME_BYTES 5u

/**
 * @brief One DHT data line, as a device driver sees it.
 *
 * `context` is the platform's: the pin, the PIO, the capture channel. A platform
 * that cannot produce the timing returns `ROBOHUB_ERR_NOT_SUPPORTED` rather than
 * a frame it is not sure of.
 */
typedef struct RoboHubDHT {
    /* Instance metadata */
    rh_name name;       /* e.g. "dht0" */
    void*   context;    /* platform-specific: the pin, the sampler */

    /* HAL operations */
    RoboHubError (*init)(struct RoboHubDHT* self);
    RoboHubError (*deinit)(struct RoboHubDHT* self);
    /**
     * Perform the start signal and read one frame.
     *
     * `data` receives `len` bytes in the order they arrived, most significant
     * bit of each byte first — the frame as the part sent it. The driver checks
     * the checksum and decides what the bytes mean. A part that does not answer
     * — no response pulse after the start signal — is
     * `ROBOHUB_ERR_NOT_FOUND`, which is the failure a caller can act on.
     */
    RoboHubError (*read)(struct RoboHubDHT* self, rh_u8* data, rh_size len);
} RoboHubDHT;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_DHT_H */
