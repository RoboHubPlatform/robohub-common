/**
 * @file rs485.h
 * @brief Hardware Abstraction Layer - RS485 interface for RoboHub Platform.
 *
 * This header defines the abstract RS485 interface used across all RoboHub
 * Platform modules. Hardware-specific details are stored in context.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_RS485_H
#define INCLUDE_ROBOHUB_HAL_RS485_H

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

/* -------------------------------------------------------------------------- */
/*  Type Definitions                                                          */
/* -------------------------------------------------------------------------- */

/** The line is driven by the transceiver: the host is talking. */
#define ROBOHUB_RS485_TX 1u
/** The line is released to the bus: the host is listening. */
#define ROBOHUB_RS485_RX 0u

typedef struct RoboHubRS485 {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubRS485* self, rh_u32 baudrate);
    /* A successful operation transfers all requested bytes. */
    RoboHubError (*write)(struct RoboHubRS485* self,
                          const rh_u8* data,
                          rh_size len);
    RoboHubError (*read)(struct RoboHubRS485* self,
                         rh_u8* data,
                         rh_size len);
    /* Read up to `len` bytes within `timeout_ms`; returns how many arrived. */
    rh_size (*read_timed)(struct RoboHubRS485* self,
                          rh_u8* data,
                          rh_size len,
                          rh_u32 timeout_ms);
    /* Drive or release the transceiver: ROBOHUB_RS485_TX or _RX. */
    RoboHubError (*set_direction)(struct RoboHubRS485* self, rh_u32 dir);
} RoboHubRS485;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_RS485_H */
