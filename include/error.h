/**
 * @file error.h
 * @brief Generic error codes and error utilities for RoboHub Platform.
 *
 * This header defines platform-independent error codes used across all
 * RoboHub modules (HAL, drivers, platform manager, registry, BSP).
 *
 * Platform-specific implementations (Pico, ESP32, STM32, Linux) must map
 * their native error codes to RoboHubError values.
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

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*  Error Codes (generic, platform-independent)                               */
/* -------------------------------------------------------------------------- */

typedef enum {
    ROBOHUB_OK = 0,               /* Success */

    ROBOHUB_ERR_FAIL = -1,        /* Generic failure */
    ROBOHUB_ERR_INVALID_ARG = -2, /* Invalid argument */
    ROBOHUB_ERR_TIMEOUT = -3,     /* Operation timed out */
    ROBOHUB_ERR_NO_MEM = -4,      /* Out of memory */
    ROBOHUB_ERR_NOT_SUPPORTED = -5, /* Feature not supported */
    ROBOHUB_ERR_BUSY = -6,        /* Resource busy */
    ROBOHUB_ERR_NOT_FOUND = -7,   /* Device or resource not found */
    ROBOHUB_ERR_IO = -8,          /* I/O error */
    ROBOHUB_ERR_PROTOCOL = -9,    /* Protocol-level error */
    ROBOHUB_ERR_STATE = -10,      /* Invalid state for operation */

    ROBOHUB_ERR_UNKNOWN = -100    /* Unknown error */
} RoboHubError;

/* -------------------------------------------------------------------------- */
/*  Error Object (optional, richer error reporting)                           */
/* -------------------------------------------------------------------------- */

typedef struct {
    RoboHubError code;      /* Error code */
    const char*  message;   /* Human-readable message */
    const char*  source;    /* Module or subsystem name */
} RoboHubErrorInfo;

/* -------------------------------------------------------------------------- */
/*  Utility Macros                                                            */
/* -------------------------------------------------------------------------- */

#define ROBOHUB_SUCCEEDED(err) ((err) == ROBOHUB_OK)
#define ROBOHUB_FAILED(err)    ((err) != ROBOHUB_OK)

/* -------------------------------------------------------------------------- */
/*  Error Conversion Helpers                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Convert a platform-specific error code to RoboHubError.
 *
 * Example:
 *   - Pico SDK: return pico_error_to_robohub(err);
 *   - ESP-IDF: return esp_err_to_robohub(err);
 *   - STM32 HAL: return stm32_hal_to_robohub(err);
 */
typedef RoboHubError (*RoboHubErrorMapper)(int platform_error);

/* -------------------------------------------------------------------------- */
/*  Error Message Helper                                                      */
/* -------------------------------------------------------------------------- */

const char* robohub_error_to_string(RoboHubError err);

#ifdef __cplusplus
}
#endif
