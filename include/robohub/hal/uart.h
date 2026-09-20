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
} RoboHubUART;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
