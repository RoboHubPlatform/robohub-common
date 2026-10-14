/**
 * @file uart.h
 * @brief Hardware Abstraction Layer - UART interface for RoboHub Platform.
 *
 * This header defines the abstract UART interface used across all RoboHub
 * Platform modules. Hardware-specific details are stored in context.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_UART_H
#define INCLUDE_ROBOHUB_HAL_UART_H

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
typedef struct RoboHubUART {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubUART* self, rh_u32 baudrate);
    /* A successful operation transfers all requested bytes. */
    RoboHubError (*write)(struct RoboHubUART* self,
                          const rh_u8* data,
                          rh_size len);
    RoboHubError (*read)(struct RoboHubUART* self,
                         rh_u8* data,
                         rh_size len);
    /*
     * Copy whatever has already arrived into `data`, up to `max_len`, and
     * return how many bytes were copied. It never blocks and never waits: a
     * byte that has not come yet is not an error, it is simply not there. This
     * is the read a parser uses — a frame arrives over time, and a blocking read
     * of a length nobody knows yet cannot be the way to see it. A platform that
     * has no queued input returns zero.
     *
     * May be null on a platform that has only the blocking read; a caller that
     * needs availability checks for null first.
     */
    rh_size (*read_available)(struct RoboHubUART* self,
                              rh_u8* data,
                              rh_size max_len);
} RoboHubUART;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_UART_H */
