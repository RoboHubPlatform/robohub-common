/**
 * @file bus.h
 * @brief Hardware Abstraction Layer - the common register bus over I2C and SPI.
 *
 * A device driver should not have to know which wire a part is soldered to. Most
 * parts are reached either over I2C or over SPI, and what differs between the two
 * is only how a register is addressed: I2C writes the register number and then
 * talks to the same address, while SPI sets a command bit and shifts bytes under
 * a chip select. Everything above that — the device's own protocol — is the same
 * code either way.
 *
 * This is the layer that makes that true. It is one contract, `RoboHubBus`, whose
 * whole surface is a register read and a register write, and two implementations
 * behind it: one that wraps a `RoboHubI2C` the platform registered and one that
 * wraps a `RoboHubSPI`. A driver is written against `RoboHubBus` alone, so the
 * same object file runs whether the part was fitted on I2C or on SPI — a change
 * of board, not of driver.
 *
 * The layering, in the order a bus is resolved:
 *
 *   1. *Bus specific* — the backends here (`robohub_bus_i2c_*`,
 *      `robohub_bus_spi_*`). They know the wire: a register number before a read,
 *      a command bit and a chip select for SPI. This is the only place that does.
 *   2. *Registry specific* — the `*_open` calls. They look the controller up in
 *      the HAL registry by the name a board registered it under and bind it. A
 *      caller names "i2c0"; it does not reach into a platform's structs.
 *   3. *Driver* — the device's protocol, written against `RoboHubBus` and the
 *      driver interface it implements, knowing neither I2C nor SPI.
 *
 * A bus is a value the caller owns, the same shape the rest of the HAL uses:
 * the concrete forms below are declared where they are used and there is no
 * allocation and nothing to free. A driver holds a `RoboHubBus*` and never the
 * concrete form, which is what keeps it bus-agnostic.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_BUS_H
#define INCLUDE_ROBOHUB_HAL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"
#include "error.h"
#include "robohub/hal/i2c.h"
#include "robohub/hal/spi.h"
#include "robohub/hal/gpio.h"

/* -------------------------------------------------------------------------- */
/*  Limits                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * The largest single register transfer the backends assemble on the stack.
 *
 * A register write and an I2C read that has to prepend the register number are
 * built in one buffer, and it is a small one on purpose: a sensor's registers are
 * bytes, a calibration block is a couple of dozen, and a caller asking for more
 * than this has a burst and should use `write`/`read` instead. Keeping it a
 * constant rather than an allocation is what lets a bus be a value with no heap
 * in a program that runs on a part.
 */
#define ROBOHUB_BUS_MAX_XFER 64u

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief One register bus, whatever wire it is.
 *
 * The operations are a register read, a register write, and the raw byte forms
 * for a part whose protocol is not shaped like registers. `reg` is the device's
 * own register address; how it reaches the wire is the backend's business.
 *
 * A backend that cannot answer one of these — an I2C controller with no read, a
 * transfer the part does not support — returns `ROBOHUB_ERR_NOT_SUPPORTED`
 * rather than a wrong answer.
 */
typedef struct RoboHubBus RoboHubBus;

struct RoboHubBus {
    /** The name the bus was bound under, such as "i2c0" or "spi0". */
    rh_name name;
    /** Backend state: the concrete form whose first member this is. */
    void*   context;

    /** Write `len` bytes to `reg`. `len` may be zero to set the pointer. */
    RoboHubError (*write_reg)(RoboHubBus* self, rh_u8 reg, const rh_u8* data, rh_size len);
    /** Read `len` bytes starting at `reg`. */
    RoboHubError (*read_reg)(RoboHubBus* self, rh_u8 reg, rh_u8* data, rh_size len);
    /** Write `len` raw bytes, with no register framing. */
    RoboHubError (*write)(RoboHubBus* self, const rh_u8* data, rh_size len);
    /** Read `len` raw bytes, with no register framing. */
    RoboHubError (*read)(RoboHubBus* self, rh_u8* data, rh_size len);
};

/**
 * @brief The I2C backend of a bus. Declare one where the bus is used.
 *
 * Its first member is the `RoboHubBus` it presents, so the address of the struct
 * is the address of the interface and a driver may hold either. The address is
 * the part's unshifted 7-bit I2C address.
 */
typedef struct {
    RoboHubBus  bus;    /* Must remain the first member. */
    RoboHubI2C* i2c;    /* The controller, from the platform. */
    rh_u8       addr;   /* The part's 7-bit address. */
} RoboHubBusI2C;

/**
 * @brief The SPI backend of a bus. Declare one where the bus is used.
 *
 * `read_flag` is the bit a command byte sets to ask for a read; the driver's
 * register number is masked with its complement for a write. 0x80 is the
 * convention most parts use and is the default when a caller passes 0. `cs` and
 * `cs_pin` are the chip select; when `cs` is NULL the controller is assumed to
 * drive it itself, which is what a platform that owns the select does.
 */
typedef struct {
    RoboHubBus    bus;        /* Must remain the first member. */
    RoboHubSPI*   spi;        /* The controller, from the platform. */
    RoboHubGPIO*  cs;         /* Chip select, or NULL when the controller drives it. */
    rh_u32        cs_pin;     /* Pin on the chip-select GPIO. */
    rh_u8         read_flag;  /* Read bit in the command byte. 0 means 0x80. */
} RoboHubBusSPI;

/* -------------------------------------------------------------------------- */
/*  Bus-specific backends                                                     */
/* -------------------------------------------------------------------------- */
/* The wire is known here and nowhere above. These bind a HAL controller the
 * caller already has; the registry-specific calls below are what a board uses
 * when all it has is the name a platform registered the controller under. */

/**
 * @brief Bind an I2C controller as a register bus.
 *
 * Calls the controller's own `init` when it has one, so a caller may bind and use
 * a bus without a separate bring-up. `speed_hz` of zero means "leave it as the
 * platform configured it".
 */
RoboHubError robohub_bus_i2c(RoboHubBusI2C* self,
                             rh_name name,
                             RoboHubI2C* i2c,
                             rh_u8 addr,
                             rh_u32 speed_hz);

/**
 * @brief Bind an SPI controller as a register bus.
 *
 * `cs` may be NULL, in which case the controller is assumed to drive its own
 * chip select. `read_flag` of zero means 0x80.
 */
RoboHubError robohub_bus_spi(RoboHubBusSPI* self,
                             rh_name name,
                             RoboHubSPI* spi,
                             RoboHubGPIO* cs,
                             rh_u32 cs_pin,
                             rh_u8 read_flag,
                             rh_u32 speed_hz);

/* -------------------------------------------------------------------------- */
/*  Registry-specific binding                                                 */
/* -------------------------------------------------------------------------- */
/* The middle layer: a caller names a controller the way the board registered it
 * and gets a bound bus. This is the whole of what a board's support package
 * needs to give a driver a bus without either of them naming a pin. */

/**
 * @brief Bind the I2C controller registered under `controller` as a bus.
 *
 * Returns `ROBOHUB_ERR_NOT_FOUND` when no controller is registered under that
 * name, which is the failure a program acts on: the platform did not bring the
 * bus up.
 */
RoboHubError robohub_bus_i2c_open(RoboHubBusI2C* self,
                                  rh_name controller,
                                  rh_u8 addr,
                                  rh_u32 speed_hz);

/** The SPI counterpart of `robohub_bus_i2c_open`. */
RoboHubError robohub_bus_spi_open(RoboHubBusSPI* self,
                                  rh_name controller,
                                  RoboHubGPIO* cs,
                                  rh_u32 cs_pin,
                                  rh_u8 read_flag,
                                  rh_u32 speed_hz);

/* -------------------------------------------------------------------------- */
/*  Convenience                                                               */
/* -------------------------------------------------------------------------- */
/* A register that is one byte is written and read through the same calls as any
 * other, with a one-byte buffer; a helper for it would be a body in a header a
 * foreign-interface reader has to read, which is the reason the temperature
 * conversions live in their own header rather than in the interface. */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_BUS_H */
