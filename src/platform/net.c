/*
 * net.c — the network contract with nothing beneath it.
 *
 * Every function here is weak, so a platform that implements the contract
 * strongly replaces it and the linker never looks back. With no platform the
 * answer to every question is the honest one: there is no interface and no
 * socket, so bringing an interface up is not supported, opening a socket
 * returns null, and a send or receive fails. A program that never opens a socket
 * pays for none of it.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/net.h"

#if defined(__GNUC__) || defined(__clang__)
#define ROBOHUB_WEAK __attribute__((weak))
#else
#define ROBOHUB_WEAK
#endif

ROBOHUB_WEAK RoboHubError robohub_net_wifi_connect(const RoboHubWiFiConfig* config)
{
    (void)config;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

ROBOHUB_WEAK RoboHubError robohub_net_wifi_disconnect(void)
{
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

ROBOHUB_WEAK RoboHubNetStatus robohub_net_status(void)
{
    return ROBOHUB_NET_DISCONNECTED;
}

ROBOHUB_WEAK RoboHubError robohub_net_ip(char* out_ip, rh_size max_len)
{
    (void)out_ip;
    (void)max_len;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

ROBOHUB_WEAK RoboHubSocket robohub_socket_create(RoboHubSocketType type)
{
    (void)type;
    return 0;
}

ROBOHUB_WEAK RoboHubError robohub_socket_connect(RoboHubSocket socket, const char* host, rh_u16 port)
{
    (void)socket;
    (void)host;
    (void)port;
    return ROBOHUB_ERR_NOT_SUPPORTED;
}

ROBOHUB_WEAK int robohub_socket_send(RoboHubSocket socket, const void* data, rh_size len)
{
    (void)socket;
    (void)data;
    (void)len;
    return ROBOHUB_SOCKET_ERROR;
}

ROBOHUB_WEAK int robohub_socket_recv(RoboHubSocket socket, void* data, rh_size len, rh_u32 timeout_ms)
{
    (void)socket;
    (void)data;
    (void)len;
    (void)timeout_ms;
    return ROBOHUB_SOCKET_ERROR;
}

ROBOHUB_WEAK void robohub_socket_close(RoboHubSocket socket)
{
    (void)socket;
}
