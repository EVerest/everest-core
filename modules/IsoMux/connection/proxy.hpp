// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022 chargebyte GmbH
// Copyright (C) 2022 Contributors to EVerest

#ifndef ISOMUX_PROXY_H
#define ISOMUX_PROXY_H

#include <cstddef>
#include <netinet/in.h>

/*!
 * \brief connect to a local V2G server
 * \param port port to connect to
 * \return 0 on failure, otherwise the socket
 */
inline int proxy_connect(uint16_t port) {

    int sock_fd = -1;
    struct sockaddr_in6 server_addr;

    /* Create socket for communication with server */
    sock_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("socket()");
        return -1;
    }

    /* Connect to server running on localhost */
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &server_addr.sin6_addr);
    server_addr.sin6_port = htons(port);

    /* Try to do TCP handshake with server */
    int ret = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("connect()");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

#endif /* ISOMUX_PROXY_H */
