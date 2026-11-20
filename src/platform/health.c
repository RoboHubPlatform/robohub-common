/**
 * @file health.c
 * @brief The portable half of a board's health: the retained record, the boot
 *        count, the settled reset reason; and the weak defaults for a platform
 *        that has no watchdog, no reset flags or no retained memory.
 *
 * The retained block (RH_HEALTH_RETAINED_SIZE bytes, in memory a reset keeps):
 *
 *   offset size
 *        0    4  magic RH_HEALTH_MAGIC
 *        4    4  boot count
 *        8    4  fault code (0: no record)
 *       12    4  program counter
 *       16    4  uptime in ms
 *       20    2  task
 *       22    2  reserved, 0
 *       24    4  check: a word-wise sum over bytes 0..23, inverted
 *       28    4  reserved, 0
 *
 * Memory a power loss clears comes back as noise, so a block whose magic or
 * check does not match is taken as a power-on: cleared, boot count 0, no record.
 * The check is a sum, not a CRC, because rh_health_fault() computes it inside a
 * fault handler and must stay a handful of loads and adds.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "robohub/health.h"

#include <stddef.h>

#define RH_HEALTH_MAGIC 0x52484831u /* "RHH1" */

typedef struct {
    uint32_t magic;
    uint32_t boot_count;
    uint32_t code;
    uint32_t pc;
    uint32_t uptime_ms;
    uint16_t task;
    uint16_t reserved0;
    uint32_t check;
    uint32_t reserved1;
} rh_health_block;

typedef char rh_health_block_fits[(sizeof(rh_health_block) <= RH_HEALTH_RETAINED_SIZE) ? 1 : -1];

/* What the runtime last said it was doing; read by rh_health_fault(). */
static volatile uint16_t s_task = RH_HEALTH_NO_TASK;
static volatile uint32_t s_uptime_ms = 0;

static bool            s_booted = false;
static rh_reset_reason s_reason = RH_RESET_UNKNOWN;

static uint32_t block_check(const volatile rh_health_block *b)
{
    return ~(b->magic + b->boot_count + b->code + b->pc + b->uptime_ms +
             ((uint32_t)b->task | ((uint32_t)b->reserved0 << 16)));
}

static volatile rh_health_block *block(void)
{
    return (volatile rh_health_block *)rh_platform_retained();
}

static void block_seal(volatile rh_health_block *b)
{
    b->check = block_check(b);
}

/* -------------------------------------------------------------------------- */
/*  The weak defaults: a platform without the hardware                        */
/* -------------------------------------------------------------------------- */

#if defined(__GNUC__) || defined(__clang__)
#define RH_WEAK __attribute__((weak))
#else
#define RH_WEAK
#endif

// @req COMM-0010
RH_WEAK void rh_watchdog_caps_get(rh_watchdog_caps *caps)
{
    if (caps) {
        caps->present = false;
        caps->stoppable = false;
        caps->early_warning = false;
        caps->min_ms = 0;
        caps->max_ms = 0;
    }
}

// @req COMM-0010
RH_WEAK RoboHubError rh_watchdog_start(uint32_t period_ms, uint32_t *chosen_ms)
{
    (void)period_ms;
    if (chosen_ms) {
        *chosen_ms = 0;
    }
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0010
RH_WEAK void rh_watchdog_feed(void)
{
}

// @req COMM-0010
RH_WEAK RoboHubError rh_watchdog_stop(void)
{
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

// @req COMM-0011
RH_WEAK rh_reset_reason rh_platform_reset_reason(void)
{
    return RH_RESET_UNKNOWN;
}

/* Ordinary memory: cleared at every start, so every boot is a power-on. */
static uint32_t s_default_retained[RH_HEALTH_RETAINED_SIZE / 4u];

// @req COMM-0012
RH_WEAK void *rh_platform_retained(void)
{
    return s_default_retained;
}

/* -------------------------------------------------------------------------- */
/*  The portable record                                                       */
/* -------------------------------------------------------------------------- */

// @req COMM-0011, COMM-0012
void rh_health_boot(void)
{
    if (s_booted) {
        return;
    }
    s_booted = true;

    volatile rh_health_block *b = block();
    if (b->magic != RH_HEALTH_MAGIC || b->check != block_check(b)) {
        /* Power was lost, or the block was never written: start from nothing. */
        b->magic = RH_HEALTH_MAGIC;
        b->boot_count = 0;
        b->code = RH_FAULT_NONE;
        b->pc = 0;
        b->uptime_ms = 0;
        b->task = RH_HEALTH_NO_TASK;
        b->reserved0 = 0;
        b->reserved1 = 0;
    }
    b->boot_count++;
    block_seal(b);

    rh_reset_reason reason = rh_platform_reset_reason();
    if ((reason == RH_RESET_SOFTWARE || reason == RH_RESET_UNKNOWN) && b->code != RH_FAULT_NONE) {
        reason = RH_RESET_FAULT;
    }
    s_reason = reason;
}

// @req COMM-0011
rh_reset_reason rh_health_reset_reason(void)
{
    rh_health_boot();
    return s_reason;
}

// @req COMM-0011
const char *rh_reset_reason_name(rh_reset_reason reason)
{
    switch (reason) {
    case RH_RESET_POWER_ON:  return "power-on";
    case RH_RESET_WATCHDOG:  return "watchdog";
    case RH_RESET_SOFTWARE:  return "software";
    case RH_RESET_BROWN_OUT: return "brown-out";
    case RH_RESET_PIN:       return "reset pin";
    case RH_RESET_FAULT:     return "fault";
    case RH_RESET_UNKNOWN:
    default:                 return "unknown";
    }
}

// @req COMM-0012
uint32_t rh_health_boot_count(void)
{
    rh_health_boot();
    return block()->boot_count;
}

// @req COMM-0012
bool rh_health_fault_take(rh_fault_record *out)
{
    rh_health_boot();
    volatile rh_health_block *b = block();
    bool present = b->code != RH_FAULT_NONE;
    if (out) {
        out->code = present ? b->code : 0;
        out->task = present ? b->task : RH_HEALTH_NO_TASK;
        out->pc = present ? b->pc : 0;
        out->uptime_ms = present ? b->uptime_ms : 0;
    }
    if (present) {
        b->code = RH_FAULT_NONE;
        b->pc = 0;
        b->uptime_ms = 0;
        b->task = RH_HEALTH_NO_TASK;
        block_seal(b);
    }
    return present;
}

/* Runs in a fault handler: loads, stores and adds only. A block a power loss
   left as noise (the handler running before rh_health_boot, very early) is
   given its magic and a boot count of 0, so the record still survives. */
// @req COMM-0012
void rh_health_fault(uint32_t code, uint32_t pc)
{
    volatile rh_health_block *b = block();
    if (b->magic != RH_HEALTH_MAGIC || b->check != block_check(b)) {
        b->magic = RH_HEALTH_MAGIC;
        b->boot_count = 0;
        b->reserved0 = 0;
        b->reserved1 = 0;
    }
    b->code = code == RH_FAULT_NONE ? RH_FAULT_HARD : code;
    b->pc = pc;
    b->uptime_ms = s_uptime_ms;
    b->task = s_task;
    block_seal(b);
}

// @req COMM-0012
void rh_health_note(uint16_t task, uint32_t uptime_ms)
{
    s_task = task;
    s_uptime_ms = uptime_ms;
}
