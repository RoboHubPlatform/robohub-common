/**
 * @file health.h
 * @brief A board's health: its watchdog, why it last reset, and what it was
 *        doing when it last failed.
 *
 * Three contracts a platform implements and one portable record this module
 * keeps for it:
 *
 * - **The watchdog** (COMM-0010). Whether the board has one, starting it with a
 *   period (the platform picks the nearest it supports and says which), feeding
 *   it, and stopping it where the hardware allows. A platform without a watchdog
 *   links the weak defaults here, which answer that there is none.
 *
 * - **The reset reason** (COMM-0011). A platform reads its own reset flags in
 *   rh_platform_reset_reason(); rh_health_reset_reason() asks it once, at the
 *   first call after boot, and answers the same thing on every later call even
 *   when reading the flags cleared them.
 *
 * - **The fault record and the boot count** (COMM-0012). A small block of memory
 *   that a reset keeps and a power loss does not (rh_platform_retained(): a
 *   `.noinit` section, a backup register file, RTC memory). A fault handler or a
 *   watchdog early-warning handler writes a record into it with
 *   rh_health_fault(); the next boot takes it, once. The boot count lives beside
 *   it, raised at every boot. A platform with no such memory links the weak
 *   default, an ordinary static block, so every boot looks like a power-on and
 *   nothing breaks.
 *
 * rh_health_fault() runs in a fault handler. It calls nothing that can fault
 * again: no allocation, no locks, no I/O, no OSAL — it reads two variables the
 * runtime keeps current (rh_health_note()) and writes the record.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HEALTH_H
#define INCLUDE_ROBOHUB_HEALTH_H

#include <stdbool.h>
#include <stdint.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*  The watchdog (COMM-0010)                                                  */
/* -------------------------------------------------------------------------- */

/** What a platform's watchdog can do. All zero when it has none. */
typedef struct {
    bool     present;        /**< there is a watchdog to start */
    bool     stoppable;      /**< it can be stopped once started */
    bool     early_warning;  /**< it interrupts before it resets (a record can be written) */
    uint32_t min_ms;         /**< the shortest period it supports */
    uint32_t max_ms;         /**< the longest period it supports */
} rh_watchdog_caps;

/** What this platform's watchdog can do. The weak default: nothing (`present` false). */
void rh_watchdog_caps_get(rh_watchdog_caps *caps);

/**
 * Start the watchdog with the period nearest `period_ms` that the hardware
 * supports, and write that period to `chosen_ms` (when not NULL).
 *
 * ROBOHUB_OK when started; ROBOHUB_ERR_NOT_SUPPORTED when the platform has no
 * watchdog (the weak default); ROBOHUB_ERR_INVALID_ARG for a period of 0.
 * Starting it again changes its period where the hardware allows that, and is
 * ROBOHUB_ERR_STATE where it does not.
 */
RoboHubError rh_watchdog_start(uint32_t period_ms, uint32_t *chosen_ms);

/** Feed the watchdog. Does nothing when it has not been started or there is none. */
void rh_watchdog_feed(void);

/**
 * Stop the watchdog. ROBOHUB_ERR_NOT_SUPPORTED when the hardware cannot stop it
 * once started (`stoppable` false) or there is none.
 */
RoboHubError rh_watchdog_stop(void);

/* -------------------------------------------------------------------------- */
/*  The reset reason (COMM-0011)                                              */
/* -------------------------------------------------------------------------- */

/** Why a board last restarted. The values are on the wire (HEALTH, PROTO-0007). */
typedef enum {
    RH_RESET_UNKNOWN   = 0,  /**< the platform cannot tell */
    RH_RESET_POWER_ON  = 1,  /**< power was applied */
    RH_RESET_WATCHDOG  = 2,  /**< the watchdog was not fed */
    RH_RESET_SOFTWARE  = 3,  /**< the program asked for it */
    RH_RESET_BROWN_OUT = 4,  /**< the supply dropped too low */
    RH_RESET_PIN       = 5,  /**< the external reset pin */
    RH_RESET_FAULT     = 6,  /**< a fault handler reset the board */
} rh_reset_reason;

/**
 * The platform's own reading of its reset flags, called once per boot by
 * rh_health_reset_reason(). May clear the flags. The weak default answers
 * RH_RESET_UNKNOWN.
 */
rh_reset_reason rh_platform_reset_reason(void);

/**
 * Why the board last restarted. Asks the platform once, then answers the same
 * on every call. When the platform says RH_RESET_SOFTWARE or RH_RESET_UNKNOWN but
 * a fault record was left by the boot before — a fault handler that reset the
 * board through the software path — the answer is RH_RESET_FAULT.
 */
rh_reset_reason rh_health_reset_reason(void);

/** A short English name for a reason ("power-on", "watchdog", ...). */
const char *rh_reset_reason_name(rh_reset_reason reason);

/* -------------------------------------------------------------------------- */
/*  The fault record and the boot count (COMM-0012)                           */
/* -------------------------------------------------------------------------- */

/** What failed. The values are on the wire (HEALTH, PROTO-0007). */
typedef enum {
    RH_FAULT_NONE        = 0,
    RH_FAULT_HARD        = 1,  /**< a CPU fault: bad access, illegal instruction */
    RH_FAULT_WATCHDOG    = 2,  /**< the watchdog's early warning: about to reset */
    RH_FAULT_STACK       = 3,  /**< a stack overflow detected */
    RH_FAULT_ALLOC       = 4,  /**< an allocation failed where none may */
    RH_FAULT_ASSERT      = 5,  /**< an assertion failed */
    RH_FAULT_APPLICATION = 0x100, /**< and above: the application's own codes */
} rh_fault_code;

/** No task was running (between tasks, in an event handler, before the loop). */
#define RH_HEALTH_NO_TASK 0xFFFFu

/** The last fault, as the next boot reads it. */
typedef struct {
    uint32_t code;       /**< an rh_fault_code, or an application's own */
    uint16_t task;       /**< the task the scheduler was running, or RH_HEALTH_NO_TASK */
    uint32_t pc;         /**< where it happened, 0 when the platform cannot give it */
    uint32_t uptime_ms;  /**< time since boot, as of the last task the runtime started */
} rh_fault_record;

/**
 * The bytes a platform's retained memory must hold: rh_platform_retained()
 * returns at least this many, aligned to 4.
 */
#define RH_HEALTH_RETAINED_SIZE 32u

/**
 * Memory a reset keeps and a power loss does not, at least
 * RH_HEALTH_RETAINED_SIZE bytes, aligned to 4. The weak default is an ordinary
 * static block: zeroed at every start, so every boot looks like a power-on.
 */
void *rh_platform_retained(void);

/**
 * Called once per boot, before anything reads the health: checks the retained
 * block (a bad check means power was lost — the block is cleared), raises the
 * boot count and settles the reset reason. Calling it again in the same boot
 * does nothing.
 */
void rh_health_boot(void);

/** Boots since power was applied, this one included (1 after a power-on). */
uint32_t rh_health_boot_count(void);

/**
 * Take the fault record the previous boot left: copy it to `out` and clear it,
 * so it is taken once. True when there was one; false, with `out` zeroed, when
 * there was not.
 */
bool rh_health_fault_take(rh_fault_record *out);

/**
 * Write a fault record into retained memory. Safe in a fault or watchdog
 * early-warning handler: it calls nothing, reads only what rh_health_note()
 * last stored, and writes the record and its check. A later fault in the same
 * boot overwrites an earlier one; the last is what the next boot sees.
 */
void rh_health_fault(uint32_t code, uint32_t pc);

/**
 * What the runtime is doing, kept for rh_health_fault(): the task about to run
 * (RH_HEALTH_NO_TASK between tasks) and the uptime. The node core calls this
 * before each task; a program without it records RH_HEALTH_NO_TASK and 0.
 */
void rh_health_note(uint16_t task, uint32_t uptime_ms);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HEALTH_H */
