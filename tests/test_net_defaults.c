/*
 * test_net_defaults.c — the contract with no platform: every call answers that
 * there is no network, rather than being an unresolved symbol.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/net.h"

#include <assert.h>

int main(void)
{
    assert(robohub_net_status() == ROBOHUB_NET_DISCONNECTED);

    RoboHubWiFiConfig config = { .ssid = "ap", .password = "secret", .timeout_ms = 1000u };
    assert(robohub_net_wifi_connect(&config) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(robohub_net_wifi_disconnect() == ROBOHUB_ERR_NOT_SUPPORTED);

    char ip[32] = { 0 };
    assert(robohub_net_ip(ip, sizeof(ip)) == ROBOHUB_ERR_NOT_SUPPORTED);

    assert(robohub_socket_create(ROBOHUB_SOCKET_TCP) == 0);
    assert(robohub_socket_create(ROBOHUB_SOCKET_UDP) == 0);
    assert(robohub_socket_connect(0, "example.invalid", 80) == ROBOHUB_ERR_NOT_SUPPORTED);
    assert(robohub_socket_send(0, "x", 1u) == ROBOHUB_SOCKET_ERROR);
    assert(robohub_socket_recv(0, ip, sizeof(ip), 10u) == ROBOHUB_SOCKET_ERROR);
    robohub_socket_close(0); /* a null socket is not an error */

    return 0;
}
