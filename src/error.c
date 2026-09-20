/**
 * @file error.c
 * @brief Generic error utilities for RoboHub Platform.
 *
 * Implements conversion of RoboHubError codes to human-readable strings.
 * Platform-specific error mappers (Pico, ESP32, STM32, Linux) should be
 * implemented in their respective BSP directories.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "error.h"

const char* robohub_error_to_string(RoboHubError err)
{
    switch (err) {
        case ROBOHUB_OK:
            return "OK";

        case ROBOHUB_ERR_FAIL:
            return "Generic failure";

        case ROBOHUB_ERR_INVALID_ARG:
            return "Invalid argument";

        case ROBOHUB_ERR_TIMEOUT:
            return "Operation timed out";

        case ROBOHUB_ERR_NO_MEM:
            return "Out of memory";

        case ROBOHUB_ERR_NOT_SUPPORTED:
            return "Feature not supported";

        case ROBOHUB_ERR_BUSY:
            return "Resource busy";

        case ROBOHUB_ERR_NOT_FOUND:
            return "Device or resource not found";

        case ROBOHUB_ERR_IO:
            return "I/O error";

        case ROBOHUB_ERR_PROTOCOL:
            return "Protocol error";

        case ROBOHUB_ERR_STATE:
            return "Invalid state for operation";

        case ROBOHUB_ERR_UNKNOWN:
        default:
            return "Unknown error";
    }
}
