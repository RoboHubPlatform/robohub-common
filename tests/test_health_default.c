/*
 * test_health_default.c — health on a platform with none of the hardware: no
 * watchdog, no reset flags, no retained memory. The program links, says plainly
 * that there is no watchdog, and every boot is the first.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/health.h"

#include <assert.h>
#include <string.h>

// @verifies COMM-0010, COMM-0011, COMM-0012
int main(void)
{
    rh_watchdog_caps caps;
    memset(&caps, 0xff, sizeof caps);
    rh_watchdog_caps_get(&caps);
    assert(!caps.present && !caps.stoppable && !caps.early_warning);

    uint32_t chosen = 99;
    assert(rh_watchdog_start(1000, &chosen) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(chosen == 0);
    rh_watchdog_feed();
    assert(rh_watchdog_stop() == ROBOHUB_ERR_NOT_SUPPORTED);

    assert(rh_health_reset_reason() == RH_RESET_UNKNOWN);
    assert(rh_health_boot_count() == 1);

    rh_fault_record r;
    assert(!rh_health_fault_take(&r));
    assert(r.code == 0 && r.task == RH_HEALTH_NO_TASK);

    assert(strcmp(rh_reset_reason_name(RH_RESET_BROWN_OUT), "brown-out") == 0);
    assert(strcmp(rh_reset_reason_name((rh_reset_reason)42), "unknown") == 0);
    return 0;
}
