/**
 * @file platform.c
 * @brief Generic platform initialization for RoboHub Platform.
 *
 * This file implements the common initialization sequence:
 *  - registry initialization
 *  - board-specific HAL registration
 *
 * Platform-specific code is implemented in robohub_board_register().
 *
 * @project RoboHub Platform
 * @author Aurel
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform.h"
#include "registry.h"

RoboHubError robohub_platform_init(void)
{
    /* Step 1: Initialize all registries */
    robohub_registry_init();

    /* Step 2: Call board-specific registration */
    RoboHubError err = robohub_board_register();
    if (err != ROBOHUB_OK) {
        robohub_registry_deinit();
        return err;
    }

    /* Step 3: Platform-level initialization (optional) */
    /* Example:
       - clock setup
       - power domains
       - system timers
       - interrupt controller
       - RTOS startup
    */

    return ROBOHUB_OK;
}
