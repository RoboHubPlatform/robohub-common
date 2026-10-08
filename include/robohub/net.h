/**
 * @file net.h
 * @brief Network interface and socket abstraction for the RoboHub Platform.
 *
 * A program that speaks MQTT, HTTP or a websocket should not carry a vendor's
 * networking stack in its source. This is the contract those programs are
 * written against: bring an interface up, get its address, open a socket, send
 * and receive with a timeout. Which stack is beneath — esp_netif on an ESP32,
 * cyw43_arch and LwIP on a Pico W, POSIX on a Linux gateway — is the platform's,
 * and a protocol built on this compiles for any of them without a line changed.
 *
 * Every function has a weak default in this module. A program that never opens a
 * socket, on a board that never brings an interface up, links and runs with no
 * networking at all: the default answers `ROBOHUB_ERR_NOT_SUPPORTED`, or a null
 * socket, or a closed interface. A platform that implements the contract defines
 * these functions strongly and the linker prefers them, which is the whole of
 * how the stack is chosen.
 *
 * Receives take a timeout and are not allowed to block a scheduler for longer
 * than it says. A network read that waits forever is a control loop that stops,
 * and on a board the two share a core — so a platform implements the wait as a
 * non-blocking poll between OSAL sleeps rather than a blocking `recv`.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_NET_H
#define INCLUDE_ROBOHUB_NET_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "types.h"
#include "error.h"

/* -------------------------------------------------------------------------- */
/*  Types                                                                     */
/* -------------------------------------------------------------------------- */

/** An open socket, or null. Opaque: the platform decides what it points at. */
typedef void* RoboHubSocket;

/** Where a network interface is. */
typedef enum {
    ROBOHUB_NET_DISCONNECTED = 0,
    ROBOHUB_NET_CONNECTING,
    ROBOHUB_NET_CONNECTED,
    ROBOHUB_NET_ERROR
} RoboHubNetStatus;

/** What a socket carries. */
typedef enum {
    ROBOHUB_SOCKET_TCP = 0,
    ROBOHUB_SOCKET_UDP
} RoboHubSocketType;

/** A `send` or `recv` that failed. A successful one returns a byte count. */
#define ROBOHUB_SOCKET_ERROR (-1)
/** A `recv` that ran out its timeout with nothing to read. */
#define ROBOHUB_SOCKET_TIMEOUT (-2)

/**
 * @brief What an interface is asked to join.
 *
 * The strings are read during the call and not kept, so a caller may build them
 * on its own stack.
 */
typedef struct {
    const char* ssid;
    const char* password;
    rh_u32      timeout_ms;
} RoboHubWiFiConfig;

/* -------------------------------------------------------------------------- */
/*  Interface                                                                 */
/* -------------------------------------------------------------------------- */

/** Bring an interface up. `timeout_ms` bounds how long it may take. */
RoboHubError robohub_net_wifi_connect(const RoboHubWiFiConfig* config);

/** Take the interface down. */
RoboHubError robohub_net_wifi_disconnect(void);

/** Where the interface is now. */
RoboHubNetStatus robohub_net_status(void);

/** Write the interface's address as text into `out_ip`, NUL-terminated. */
RoboHubError robohub_net_ip(char* out_ip, rh_size max_len);

/* -------------------------------------------------------------------------- */
/*  Sockets                                                                   */
/* -------------------------------------------------------------------------- */

/** Open a socket, or return null. */
RoboHubSocket robohub_socket_create(RoboHubSocketType type);

/** Resolve and connect to a host and port. */
RoboHubError robohub_socket_connect(RoboHubSocket socket, const char* host, rh_u16 port);

/** Send; returns the bytes sent or `ROBOHUB_SOCKET_ERROR`. */
int robohub_socket_send(RoboHubSocket socket, const void* data, rh_size len);

/**
 * @brief Receive up to `len` bytes, waiting at most `timeout_ms`.
 *
 * Returns the bytes read, `ROBOHUB_SOCKET_TIMEOUT` when the timeout passed with
 * nothing to read, or `ROBOHUB_SOCKET_ERROR` on failure. A timeout is not an
 * error: a caller polls again, and the scheduler it shares a core with keeps
 * running in between.
 */
int robohub_socket_recv(RoboHubSocket socket, void* data, rh_size len, rh_u32 timeout_ms);

/** Close a socket. A null socket is not an error. */
void robohub_socket_close(RoboHubSocket socket);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_NET_H */
