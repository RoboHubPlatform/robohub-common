/*
 * test_update_slot_default.c — the update slot with no platform: not updatable,
 * every call says so, and the program links.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/update_slot.h"

#include <assert.h>
#include <string.h>

// @verifies COMM-0013, COMM-0014
int main(void)
{
    rh_update_caps caps;
    memset(&caps, 0xff, sizeof caps);
    rh_update_caps_get(&caps);
    assert(!caps.updatable && caps.slot_size == 0u);

    uint8_t byte = 0;
    rh_update_info info = { 1u, 2u, 3u };
    assert(rh_update_begin(16u) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(rh_update_write(0u, &byte, 1u) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(rh_update_read(0u, &byte, 1u) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(rh_update_boot_once(&info) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(!rh_update_pending(&info));
    assert(rh_update_confirm() == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(rh_update_reject() == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(rh_update_take_outcome(&info) == RH_UPDATE_NONE);
    assert(rh_update_security_version() == 0u);
    assert(rh_update_running_slot() == -1);
    return 0;
}
