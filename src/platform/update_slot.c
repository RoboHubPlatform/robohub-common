/**
 * @file update_slot.c
 * @brief The update slot when a platform has none: not updatable, and every
 *        call says so (COMM-0013).
 *
 * A platform with a second image defines these strongly and the linker prefers
 * those. See robohub/update_slot.h.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "robohub/update_slot.h"

#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)
#define RH_WEAK __attribute__((weak))
#else
#define RH_WEAK
#endif

// @req COMM-0013
RH_WEAK void rh_update_caps_get(rh_update_caps *caps)
{
    if (caps) {
        caps->updatable = false;
        caps->slot_size = 0u;
    }
}

// @req COMM-0013
RH_WEAK RoboHubError rh_update_begin(uint32_t size)
{
    (void)size;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0013
RH_WEAK RoboHubError rh_update_write(uint32_t offset, const uint8_t *data, uint32_t length)
{
    (void)offset; (void)data; (void)length;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0013
RH_WEAK RoboHubError rh_update_read(uint32_t offset, uint8_t *out, uint32_t length)
{
    (void)offset; (void)out; (void)length;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0013
RH_WEAK RoboHubError rh_update_boot_once(const rh_update_info *info)
{
    (void)info;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0013
RH_WEAK bool rh_update_pending(rh_update_info *out)
{
    (void)out;
    return false;
}

// @req COMM-0013
RH_WEAK RoboHubError rh_update_confirm(void)
{
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0014
RH_WEAK RoboHubError rh_update_reject(void)
{
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0013
RH_WEAK rh_update_outcome rh_update_take_outcome(rh_update_info *out)
{
    (void)out;
    return RH_UPDATE_NONE;
}

// @req COMM-0013
RH_WEAK uint32_t rh_update_security_version(void)
{
    return 0u;
}

// @req COMM-0013
RH_WEAK int rh_update_running_slot(void)
{
    return -1;
}
