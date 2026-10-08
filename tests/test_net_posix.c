/*
 * test_net_posix.c — the contract implemented over POSIX, exercised against a
 * loopback socket.
 *
 * The functions below are strong, so they are reached instead of the weak
 * defaults in robohub-common. That is the same mechanism a platform uses, which
 * makes this both a test of the socket contract and a reference for the POSIX
 * (Linux edge gateway) column of the mapping.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/net.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/*  The POSIX implementation                                                  */
/* -------------------------------------------------------------------------- */

static RoboHubNetStatus posix_status = ROBOHUB_NET_DISCONNECTED;

RoboHubError robohub_net_wifi_connect(const RoboHubWiFiConfig* config)
{
    (void)config;
    posix_status = ROBOHUB_NET_CONNECTED;
    return ROBOHUB_OK;
}

RoboHubError robohub_net_wifi_disconnect(void)
{
    posix_status = ROBOHUB_NET_DISCONNECTED;
    return ROBOHUB_OK;
}

RoboHubNetStatus robohub_net_status(void)
{
    return posix_status;
}

RoboHubError robohub_net_ip(char* out_ip, rh_size max_len)
{
    if (out_ip == 0 || max_len < 10u) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    strcpy(out_ip, "127.0.0.1");
    return ROBOHUB_OK;
}

RoboHubSocket robohub_socket_create(RoboHubSocketType type)
{
    int kind = (type == ROBOHUB_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int fd = socket(AF_INET, kind, 0);
    if (fd < 0) {
        return 0;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    (void)fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return (RoboHubSocket)(intptr_t)fd;
}

RoboHubError robohub_socket_connect(RoboHubSocket socket, const char* host, rh_u16 port)
{
    if (socket == 0) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    int fd = (int)(intptr_t)socket;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &address.sin_addr) != 1) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    int result = connect(fd, (struct sockaddr*)&address, sizeof(address));
    if (result < 0 && errno != EINPROGRESS) {
        return ROBOHUB_ERR_IO;
    }
    return ROBOHUB_OK;
}

int robohub_socket_send(RoboHubSocket socket, const void* data, rh_size len)
{
    if (socket == 0) {
        return ROBOHUB_SOCKET_ERROR;
    }
    int fd = (int)(intptr_t)socket;
    ssize_t sent = send(fd, data, len, 0);
    return (sent < 0) ? ROBOHUB_SOCKET_ERROR : (int)sent;
}

int robohub_socket_recv(RoboHubSocket socket, void* data, rh_size len, rh_u32 timeout_ms)
{
    if (socket == 0) {
        return ROBOHUB_SOCKET_ERROR;
    }
    int fd = (int)(intptr_t)socket;
    fd_set readable;
    FD_ZERO(&readable);
    FD_SET(fd, &readable);
    struct timeval tv;
    tv.tv_sec = (long)(timeout_ms / 1000u);
    tv.tv_usec = (long)((timeout_ms % 1000u) * 1000u);
    int ready = select(fd + 1, &readable, 0, 0, &tv);
    if (ready == 0) {
        return ROBOHUB_SOCKET_TIMEOUT;
    }
    if (ready < 0) {
        return ROBOHUB_SOCKET_ERROR;
    }
    ssize_t got = recv(fd, data, len, 0);
    return (got < 0) ? ROBOHUB_SOCKET_ERROR : (int)got;
}

void robohub_socket_close(RoboHubSocket socket)
{
    if (socket != 0) {
        close((int)(intptr_t)socket);
    }
}

/* -------------------------------------------------------------------------- */
/*  The test                                                                  */
/* -------------------------------------------------------------------------- */

static bool wait_writable(int fd, int tries)
{
    for (int i = 0; i < tries; i++) {
        fd_set writable;
        FD_ZERO(&writable);
        FD_SET(fd, &writable);
        struct timeval tv = { 0, 10000 };
        if (select(fd + 1, 0, &writable, 0, &tv) > 0) {
            int err = 0;
            socklen_t length = sizeof(err);
            (void)getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &length);
            return err == 0;
        }
    }
    return false;
}

int main(void)
{
    /* The interface half of the contract. */
    RoboHubWiFiConfig config = { .ssid = "ap", .password = "secret", .timeout_ms = 2000u };
    assert(robohub_net_status() == ROBOHUB_NET_DISCONNECTED);
    assert(robohub_net_wifi_connect(&config) == ROBOHUB_OK);
    assert(robohub_net_status() == ROBOHUB_NET_CONNECTED);
    char ip[16] = { 0 };
    assert(robohub_net_ip(ip, sizeof(ip)) == ROBOHUB_OK);
    assert(strcmp(ip, "127.0.0.1") == 0);

    /* A listening socket, by hand, to be the far end. */
    int server = socket(AF_INET, SOCK_STREAM, 0);
    assert(server >= 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    assert(bind(server, (struct sockaddr*)&address, sizeof(address)) == 0);
    assert(listen(server, 1) == 0);
    socklen_t address_len = sizeof(address);
    assert(getsockname(server, (struct sockaddr*)&address, &address_len) == 0);
    rh_u16 port = ntohs(address.sin_port);

    /* The socket half: create, connect, send, receive with a timeout. */
    RoboHubSocket client = robohub_socket_create(ROBOHUB_SOCKET_TCP);
    assert(client != 0);
    assert(robohub_socket_connect(client, "127.0.0.1", port) == ROBOHUB_OK);
    assert(wait_writable((int)(intptr_t)client, 100));

    int peer = -1;
    for (int i = 0; i < 100 && peer < 0; i++) {
        fd_set readable;
        FD_ZERO(&readable);
        FD_SET(server, &readable);
        struct timeval tv = { 0, 10000 };
        if (select(server + 1, &readable, 0, 0, &tv) > 0) {
            peer = accept(server, 0, 0);
        }
    }
    assert(peer >= 0);

    assert(robohub_socket_send(client, "ping", 4u) == 4);
    char buffer[8] = { 0 };
    assert(recv(peer, buffer, sizeof(buffer), 0) == 4);
    assert(memcmp(buffer, "ping", 4) == 0);

    assert(send(peer, "pong", 4, 0) == 4);
    int got = robohub_socket_recv(client, buffer, sizeof(buffer), 500u);
    assert(got == 4);
    assert(memcmp(buffer, "pong", 4) == 0);

    /* Nothing more to read: a timeout is reported, not a fake zero. */
    assert(robohub_socket_recv(client, buffer, sizeof(buffer), 50u) == ROBOHUB_SOCKET_TIMEOUT);

    robohub_socket_close(client);
    close(peer);
    close(server);

    assert(robohub_net_wifi_disconnect() == ROBOHUB_OK);
    assert(robohub_net_status() == ROBOHUB_NET_DISCONNECTED);

    return 0;
}
