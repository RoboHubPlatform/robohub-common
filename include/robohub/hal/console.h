/**
 * @file console.h
 * @brief The console contract: where a program's output goes.
 *
 * A program on a host writes to a standard output the operating system provides.
 * A program on a board has no such thing: the only place its bytes can go is a
 * UART, and which UART is a fact about the board. So the console is a contract
 * like any other — declared here, implemented per platform — rather than
 * something a program reaches for by name.
 *
 * The implementation does two things, and the second is the reason this is a
 * contract rather than a function:
 *
 *   - it drives the board's console UART, so a program can write without knowing
 *     which peripheral that is;
 *   - it retargets the C library's `write`, so that `printf` works as it does on
 *     a host. On a board the C library's `write` is a stub that returns an error,
 *     which is why a program that prints anything at all needs this.
 *
 * The first is what a program calls. The second is what makes the ordinary C
 * library work, and is why implementing this contract is also what makes
 * `printf`, `puts` and everything built on them work.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_HAL_CONSOLE_H
#define INCLUDE_ROBOHUB_HAL_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*  Errors                                                                     */
/* -------------------------------------------------------------------------- */

/** Returned by every call here. A console that cannot be opened is a board that
 *  has no console, and a program that is told so can carry on without output
 *  rather than pretending it wrote something. */
typedef enum {
    ROBOHUB_CONSOLE_OK = 0,
    ROBOHUB_CONSOLE_ERR_INVALID = -1,   /**< A null pointer or a zero length. */
    ROBOHUB_CONSOLE_ERR_STATE = -2,     /**< The console was not opened. */
    ROBOHUB_CONSOLE_ERR_TIMEOUT = -3,   /**< The UART did not accept the data. */
    ROBOHUB_CONSOLE_ERR_UNSUPPORTED = -4 /**< This board has no console. */
} RoboHubConsoleError;

/* -------------------------------------------------------------------------- */
/*  The console                                                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief The console a program writes to.
 *
 * One, because a board has one console. It is a struct rather than an opaque
 * pointer so that a board's BSP can fill it in as a static and a program can be
 * linked against it without a constructor.
 */
typedef struct {
    /** Set once the console has been opened. */
    uint8_t ready;
    /** How many bytes have been written since it was opened. A program that
     *  prints in a loop can report it, and a board bring-up can tell a program
     *  that ran from one that hung. */
    uint64_t written;
} RoboHubConsole;

/**
 * @brief Open the console.
 *
 * Called once, from the platform's initialisation. After it, `write` works and
 * the C library's output goes to the same place.
 */
RoboHubConsoleError robohub_console_init(void);

/**
 * @brief Write to the console.
 *
 * Blocks until the bytes have been handed to the UART. On a board that is the
 * only back-pressure there is: a program that writes faster than a serial line
 * carries has to wait, and waiting is correct — dropping output would be a
 * program that reports less than it did.
 */
RoboHubConsoleError robohub_console_write(const char* data, size_t len);

/**
 * @brief Write a string, without its terminator.
 */
RoboHubConsoleError robohub_console_puts(const char* text);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_HAL_CONSOLE_H */
