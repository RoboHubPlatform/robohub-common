/**
 * @file spi.h
 * @brief Hardware Abstraction Layer - SPI interface for RoboHub Platform.
 *
 * This header defines the abstract SPI interface used across all RoboHub
 * Platform modules. Hardware-specific details are stored in context.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_SPI_H
#define INCLUDE_ROBOHUB_HAL_SPI_H

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
typedef struct RoboHubSPI {
    rh_name name;
    void*   context;

    RoboHubError (*init)(struct RoboHubSPI* self, rh_u32 speed_hz);
    /* tx and rx may be NULL only when the corresponding transfer is unused. */
    RoboHubError (*transfer)(struct RoboHubSPI* self,
                             const rh_u8* tx,
                             rh_u8* rx,
                             rh_size len);
} RoboHubSPI;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_SPI_H */
