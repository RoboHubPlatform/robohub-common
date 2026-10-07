/**
 * @file osal.h
 * @brief Operating-system abstraction: threads, time and mutual exclusion.
 *
 * The runtime is written against this and not against an RTOS. A program that
 * links robohub-freertos gets real tasks, a real delay and a real mutex; a
 * program that links nothing gets the defaults robohub-core carries, where a
 * thread is refused, a delay is the hardware timer, and a mutex is a no-op
 * because there is only ever one thing running. The program above is the same
 * in both cases.
 *
 * Every function has a weak default in robohub-core. A middleware that
 * implements this contract defines the functions strongly, and the linker picks
 * the strong definition over the weak one — which is why "remove the RTOS" is
 * removing a dependency and not editing code.
 *
 * Time is milliseconds throughout. A tick is a millisecond here even where the
 * kernel beneath counts in something else; converting at the boundary is the
 * middleware's job, not every caller's.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_OSAL_H
#define INCLUDE_ROBOHUB_OSAL_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"

/* -------------------------------------------------------------------------- */
/*  Types                                                                     */
/* -------------------------------------------------------------------------- */

/** An opaque running thread. */
typedef void* robohub_thread_t;
/** An opaque mutex. */
typedef void* robohub_mutex_t;

/**
 * @brief What a thread is asked for when it is created.
 *
 * `stack_size` is in bytes, because that is what a caller can reason about;
 * a kernel that wants a count of stack words converts. `priority` is the
 * kernel's own scale, passed through: a runtime that invented one would be a
 * runtime that has to know which kernel it is on.
 */
typedef struct robohub_osal_thread_config {
    const char* name;
    void (*entry)(void* arg);
    void* arg;
    rh_u32 stack_size;
    rh_u32 priority;
} robohub_osal_thread_config;

/* -------------------------------------------------------------------------- */
/*  Runtime capability                                                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief Whether an operating system is beneath this.
 *
 * The one question the runtime asks about the platform it is on: with a kernel
 * it structures work as threads, and without one it runs a single cooperative
 * loop. The default answers false; a middleware answers true.
 */
rh_bool robohub_osal_rtos_present(void);

/**
 * @brief Hand control to the operating system.
 *
 * On a kernel this starts the scheduler and does not return. With no kernel
 * there is nothing to start and it returns at once, and the caller carries on —
 * which is what lets the runtime call it in both cases without a branch of its
 * own.
 */
void robohub_osal_start(void);

/* -------------------------------------------------------------------------- */
/*  Threads                                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Create a thread and start it.
 *
 * Returns 0 on success and -1 on failure. With no kernel there are no threads
 * and it returns -1, which is how a caller knows the cooperative path is the
 * one it is on.
 */
int robohub_osal_thread_create(robohub_thread_t* thread,
                               const robohub_osal_thread_config* config);

/* -------------------------------------------------------------------------- */
/*  Time                                                                      */
/* -------------------------------------------------------------------------- */

/** Sleep for at least this long. */
void robohub_osal_sleep_ms(rh_u32 ms);

/** Milliseconds since boot, from the same clock the delay uses. */
rh_u32 robohub_osal_now_ms(void);

/* -------------------------------------------------------------------------- */
/*  Mutual exclusion                                                          */
/* -------------------------------------------------------------------------- */
/* A mutex is recursive: a lock taken twice by the same thread must be released
 * twice, and a handler that publishes an event while handling one must not
 * deadlock against the bus's own lock. A plain mutex cannot promise that, so
 * the contract asks for the recursive one and a bare-metal no-op is trivial. */

/** Create a mutex, or a handle that locks nothing when there is no kernel. */
robohub_mutex_t robohub_osal_mutex_create(void);

/** Release a mutex's resources. */
void robohub_osal_mutex_destroy(robohub_mutex_t mutex);

/** Take a mutex, waiting until it is free. */
void robohub_osal_mutex_lock(robohub_mutex_t mutex);

/** Give a mutex back. */
void robohub_osal_mutex_unlock(robohub_mutex_t mutex);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_OSAL_H */
