/*
 * test_health.c — the retained record across real restarts.
 *
 * Each boot is a child process, so everything a boot keeps in ordinary memory
 * starts fresh, as on a board; the retained block is a shared mapping the parent
 * owns, which survives a child the way `.noinit` RAM survives a reset. Losing
 * power is the parent filling the block with noise. The platform's reset flags
 * are a value the parent sets before each boot.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#define _DEFAULT_SOURCE
#include "robohub/health.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

static uint32_t *g_retained;
static rh_reset_reason *g_flags;

/* The platform, as a board provides it: strong over the weak defaults. */
void *rh_platform_retained(void) { return g_retained; }
rh_reset_reason rh_platform_reset_reason(void) { return *g_flags; }

typedef void (*boot_fn)(void);

/* Run one boot in a child; fail the test if the child failed. */
static void boot(rh_reset_reason flags, boot_fn fn)
{
    *g_flags = flags;
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        fn();
        _exit(0);
    }
    int status = 0;
    assert(waitpid(pid, &status, 0) == pid);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "a boot failed\n");
        exit(1);
    }
}

static void power_loss(void)
{
    memset(g_retained, 0xa5, RH_HEALTH_RETAINED_SIZE);
}

/* First boot after power: count 1, no record; then a task faults. */
static void boot_first_then_fault(void)
{
    rh_health_boot();
    assert(rh_health_boot_count() == 1);
    assert(rh_health_reset_reason() == RH_RESET_POWER_ON);
    rh_fault_record r;
    assert(!rh_health_fault_take(&r));
    rh_health_note(3, 12345);
    rh_health_fault(RH_FAULT_HARD, 0x10001234u);
}

/* The fault handler reset through the software path: the reason is a fault, the
   record is there once, and the reason stays the same after taking it. */
static void boot_after_fault(void)
{
    assert(rh_health_boot_count() == 2);
    assert(rh_health_reset_reason() == RH_RESET_FAULT);
    rh_fault_record r;
    assert(rh_health_fault_take(&r));
    assert(r.code == RH_FAULT_HARD && r.task == 3 && r.pc == 0x10001234u && r.uptime_ms == 12345);
    assert(!rh_health_fault_take(&r));
    assert(rh_health_reset_reason() == RH_RESET_FAULT);
}

/* Taken last boot: not there again. A software reset with no record stays software. */
static void boot_clean_software(void)
{
    assert(rh_health_boot_count() == 3);
    assert(rh_health_reset_reason() == RH_RESET_SOFTWARE);
    rh_fault_record r;
    assert(!rh_health_fault_take(&r));
    /* Between tasks; the watchdog's early warning writes its record. */
    rh_health_note(RH_HEALTH_NO_TASK, 900);
    rh_health_fault(RH_FAULT_WATCHDOG, 0);
}

/* The watchdog reset: the platform says so, and that is kept even with a record. */
static void boot_after_watchdog(void)
{
    assert(rh_health_boot_count() == 4);
    assert(rh_health_reset_reason() == RH_RESET_WATCHDOG);
    rh_fault_record r;
    assert(rh_health_fault_take(&r));
    assert(r.code == RH_FAULT_WATCHDOG && r.task == RH_HEALTH_NO_TASK && r.uptime_ms == 900);
    /* A record left now is lost with the power below. */
    rh_health_fault(RH_FAULT_ASSERT, 1);
}

static void boot_after_power_loss(void)
{
    assert(rh_health_boot_count() == 1);
    rh_fault_record r;
    assert(!rh_health_fault_take(&r));
}

/* A handler that runs before anything booted, on noise: the record survives. */
static void boot_fault_before_boot(void)
{
    rh_health_fault(RH_FAULT_STACK, 7);
}

static void boot_sees_early_fault(void)
{
    assert(rh_health_boot_count() == 1);
    rh_fault_record r;
    assert(rh_health_fault_take(&r));
    assert(r.code == RH_FAULT_STACK && r.pc == 7);
}

// @verifies COMM-0011, COMM-0012
int main(void)
{
    g_retained = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    assert(g_retained != MAP_FAILED);
    g_flags = (rh_reset_reason *)(g_retained + 64);

    power_loss();
    boot(RH_RESET_POWER_ON, boot_first_then_fault);
    boot(RH_RESET_SOFTWARE, boot_after_fault);
    boot(RH_RESET_SOFTWARE, boot_clean_software);
    boot(RH_RESET_WATCHDOG, boot_after_watchdog);
    power_loss();
    boot(RH_RESET_POWER_ON, boot_after_power_loss);
    power_loss();
    boot(RH_RESET_UNKNOWN, boot_fault_before_boot);
    boot(RH_RESET_PIN, boot_sees_early_fault);
    return 0;
}
