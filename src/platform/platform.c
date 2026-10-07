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

/**
 * The board's registration hook, when no board supplies one.
 *
 * A board support package defines this strongly and the linker prefers that
 * definition over this one. This weak default exists so the library links on
 * its own — a host test, a simulation, a board whose BSP is not built yet — and
 * so that this library and a platform's may refer to each other without a
 * circular dependency between two archives. A platform that provides no board
 * registers nothing, which is the right answer for a generic target.
 *
 * Weak only where the compiler has weak symbols, which is every toolchain this
 * platform is built with; elsewhere it is left undefined, as it was before.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) RoboHubError robohub_board_register(void)
{
    return ROBOHUB_OK;
}
#endif

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
