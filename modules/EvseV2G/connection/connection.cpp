// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest

#include "connection.hpp"
#include "log.hpp"
#include "tls_connection.hpp"
#include "tools.hpp"
#include "v2g_server.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_SOCKET_BACKLOG        3
#define DEFAULT_TCP_PORT              61341
#define DEFAULT_TLS_PORT              64109
#define ERROR_SESSION_ALREADY_STARTED 2

/*!
 * \brief connection_create_socket This function creates a tcp/tls socket
 * \param sockaddr to bind the socket to an interface
 * \return Returns \c 0 on success, otherwise \c -1
 */
static int connection_create_socket(struct sockaddr_in6* sockaddr) {
    socklen_t addrlen = sizeof(*sockaddr);
    int s, enable = 1;
    static bool error_once = false;

    /* create socket */
    s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s == -1) {
        if (!error_once) {
            dlog(DLOG_LEVEL_ERROR, "socket() failed: %s", strerror(errno));
            error_once = true;
        }
        return -1;
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) == -1) {
        if (!error_once) {
            dlog(DLOG_LEVEL_ERROR, "setsockopt(SO_REUSEPORT) failed: %s", strerror(errno));
            error_once = true;
        }
        close(s);
        return -1;
    }

    /* bind it to interface */
    if (bind(s, reinterpret_cast<struct sockaddr*>(sockaddr), addrlen) == -1) {
        if (!error_once) {
            dlog(DLOG_LEVEL_WARNING, "bind() failed: %s", strerror(errno));
            dlog(DLOG_LEVEL_WARNING,
                 "Verify that the configured interface has a valid IPv6 link local address configured.");
            error_once = true;
        }
        close(s);
        return -1;
    }

    /* listen on this socket */
    if (listen(s, DEFAULT_SOCKET_BACKLOG) == -1) {
        if (!error_once) {
            dlog(DLOG_LEVEL_ERROR, "listen() failed: %s", strerror(errno));
            error_once = true;
        }
        close(s);
        return -1;
    }

    /* retrieve the actual port number we are listening on */
    if (getsockname(s, reinterpret_cast<struct sockaddr*>(sockaddr), &addrlen) == -1) {
        if (!error_once) {
            dlog(DLOG_LEVEL_ERROR, "getsockname() failed: %s", strerror(errno));
            error_once = true;
        }
        close(s);
        return -1;
    }

    return s;
}

/*!
 * \brief check_interface This function checks the interface name. The interface name is
 * configured automatically in case it is pre-initialized to “auto.
 * \param sockaddr to bind the socket to an interface
 * \return Returns \c 0 on success, otherwise \c -1
 */
int check_interface(struct v2g_context* v2g_ctx) {
    if (v2g_ctx == nullptr || v2g_ctx->if_name == nullptr) {
        return -1;
    }

    struct ipv6_mreq mreq = {};
    std::memset(&mreq, 0, sizeof(mreq));

    if (strcmp(v2g_ctx->if_name, "auto") == 0) {
        v2g_ctx->if_name = choose_first_ipv6_interface();
    }

    if (v2g_ctx->if_name == nullptr) {
        return -1;
    }

    mreq.ipv6mr_interface = if_nametoindex(v2g_ctx->if_name);
    if (!mreq.ipv6mr_interface) {
        dlog(DLOG_LEVEL_ERROR, "No such interface: %s", v2g_ctx->if_name);
        return -1;
    }

    return (v2g_ctx->if_name == nullptr) ? -1 : 0;
}

/*!
 * \brief connection_init This function initilizes the tcp and tls interface.
 * \param v2g_context is the V2G context.
 * \return Returns \c 0 on success, otherwise \c -1
 */
int connection_init(struct v2g_context* v2g_ctx) {
    if (check_interface(v2g_ctx) == -1) {
        return -1;
    }

    if (v2g_ctx->tls_security != TLS_SECURITY_FORCE) {
        v2g_ctx->local_tcp_addr = static_cast<sockaddr_in6*>(calloc(1, sizeof(*v2g_ctx->local_tcp_addr)));
        if (v2g_ctx->local_tcp_addr == nullptr) {
            dlog(DLOG_LEVEL_ERROR, "Failed to allocate memory for TCP address");
            return -1;
        }
    }

    if (v2g_ctx->tls_security != TLS_SECURITY_PROHIBIT) {
        v2g_ctx->local_tls_addr = static_cast<sockaddr_in6*>(calloc(1, sizeof(*v2g_ctx->local_tls_addr)));
        if (!v2g_ctx->local_tls_addr) {
            dlog(DLOG_LEVEL_ERROR, "Failed to allocate memory for TLS address");
            return -1;
        }
    }

    while (1) {
        if (v2g_ctx->local_tcp_addr) {
            get_interface_ipv6_address(v2g_ctx->if_name, ADDR6_TYPE_LINKLOCAL, v2g_ctx->local_tcp_addr);
            if (v2g_ctx->local_tls_addr) {
                // Handle allowing TCP with TLS (TLS_SECURITY_ALLOW)
                memcpy(v2g_ctx->local_tls_addr, v2g_ctx->local_tcp_addr, sizeof(*v2g_ctx->local_tls_addr));
            }
        } else {
            // Handle forcing TLS security (TLS_SECURITY_FORCE)
            get_interface_ipv6_address(v2g_ctx->if_name, ADDR6_TYPE_LINKLOCAL, v2g_ctx->local_tls_addr);
        }

        if (v2g_ctx->local_tcp_addr) {
            char buffer[INET6_ADDRSTRLEN];

            /*
             * When we bind with port = 0, the kernel assigns a dynamic port from the range configured
             * in /proc/sys/net/ipv4/ip_local_port_range. This is on a recent Ubuntu Linux e.g.
             * $ cat /proc/sys/net/ipv4/ip_local_port_range
             * 32768   60999
             * However, in ISO15118 spec the IANA range with 49152 to 65535 is referenced. So we have the
             * problem that the kernel (without further configuration - and we want to avoid this) could
             * hand out a port which is not "range compatible".
             * To fulfill the ISO15118 standard, we simply try to bind to static port numbers.
             */
            v2g_ctx->local_tcp_addr->sin6_port = htons(DEFAULT_TCP_PORT);
            v2g_ctx->tcp_socket = connection_create_socket(v2g_ctx->local_tcp_addr);
            if (v2g_ctx->tcp_socket < 0) {
                /* retry until interface is ready */
                sleep(1);
                continue;
            }
            if (inet_ntop(AF_INET6, &v2g_ctx->local_tcp_addr->sin6_addr, buffer, sizeof(buffer)) != nullptr) {
                dlog(DLOG_LEVEL_INFO, "TCP server on %s is listening on port [%s%%%" PRIu32 "]:%" PRIu16,
                     v2g_ctx->if_name, buffer, v2g_ctx->local_tcp_addr->sin6_scope_id,
                     ntohs(v2g_ctx->local_tcp_addr->sin6_port));
            } else {
                dlog(DLOG_LEVEL_ERROR, "TCP server on %s is listening, but inet_ntop failed: %s", v2g_ctx->if_name,
                     strerror(errno));
                return -1;
            }
        }

        if (v2g_ctx->local_tls_addr) {
            char buffer[INET6_ADDRSTRLEN];

            /* see comment above for reason */
            v2g_ctx->local_tls_addr->sin6_port = htons(DEFAULT_TLS_PORT);

            v2g_ctx->tls_socket.fd = connection_create_socket(v2g_ctx->local_tls_addr);
            if (v2g_ctx->tls_socket.fd < 0) {
                if (v2g_ctx->tcp_socket != -1) {
                    /* free the TCP socket */
                    close(v2g_ctx->tcp_socket);
                }
                /* retry until interface is ready */
                sleep(1);
                continue;
            }

            if (inet_ntop(AF_INET6, &v2g_ctx->local_tls_addr->sin6_addr, buffer, sizeof(buffer)) != nullptr) {
                dlog(DLOG_LEVEL_INFO, "TLS server on %s is listening on port [%s%%%" PRIu32 "]:%" PRIu16,
                     v2g_ctx->if_name, buffer, v2g_ctx->local_tls_addr->sin6_scope_id,
                     ntohs(v2g_ctx->local_tls_addr->sin6_port));
            } else {
                dlog(DLOG_LEVEL_INFO, "TLS server on %s is listening, but inet_ntop failed: %s", v2g_ctx->if_name,
                     strerror(errno));
                return -1;
            }
        }
        /* Sockets should be ready, leave the loop */
        break;
    }

    if (v2g_ctx->local_tls_addr) {
        return tls::connection_init(v2g_ctx);
    }
    return 0;
}

/*!
 * \brief is_sequence_timeout This function checks if a sequence timeout has occurred.
 * \param ts_start Is the time after waiting of the next request message.
 * \param ctx is the V2G context.
 * \return Returns \c true if a timeout has occurred, otherwise \c false
 */
bool is_sequence_timeout(struct timespec ts_start, struct v2g_context* ctx) {
    struct timespec ts_current;
    int sequence_timeout = V2G_SEQUENCE_TIMEOUT_60S;

    if (((clock_gettime(CLOCK_MONOTONIC, &ts_current)) != 0) ||
        (timespec_to_ms(timespec_sub(ts_current, ts_start)) > sequence_timeout)) {
        dlog(DLOG_LEVEL_ERROR, "Sequence timeout has occurred (message: %s)", v2g_msg_type[ctx->current_v2g_msg]);
        return true;
    }
    return false;
}

/*!
 * \brief connection_read This function reads from socket until requested bytes are received or sequence
 * timeout is reached
 * \param conn is the v2g connection context
 * \param buf is the buffer to store the v2g message
 * \param count is the number of bytes to read
 * \return Returns \c true if a timeout has occurred, otherwise \c false
 */
ssize_t connection_read(struct v2g_connection* conn, unsigned char* buf, size_t count) {
    struct timespec ts_start;
    int bytes_read = 0;

    if (clock_gettime(CLOCK_MONOTONIC, &ts_start) == -1) {
        dlog(DLOG_LEVEL_ERROR, "clock_gettime(ts_start) failed: %s", strerror(errno));
        return -1;
    }

    /* loop until we got all requested bytes or sequence timeout DIN [V2G-DC-432]*/
    while ((bytes_read < count) && (is_sequence_timeout(ts_start, conn->ctx) == false) &&
           (conn->ctx->is_connection_terminated == false)) { // [V2G2-536]

        int num_of_bytes;

        if (conn->is_tls_connection) {
            return -1; // shouldn't be using this function
        }
        /* use select for timeout handling */
        struct timeval tv;
        fd_set read_fds;

        FD_ZERO(&read_fds);
        FD_SET(conn->conn.socket_fd, &read_fds);

        tv.tv_sec = conn->ctx->network_read_timeout / 1000;
        tv.tv_usec = (conn->ctx->network_read_timeout % 1000) * 1000;

        num_of_bytes = select(conn->conn.socket_fd + 1, &read_fds, nullptr, nullptr, &tv);

        if (num_of_bytes == -1) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        /* Zero fds ready means we timed out, so let upper loop check our sequence timeout */
        if (num_of_bytes == 0) {
            continue;
        }

        num_of_bytes = (int)read(conn->conn.socket_fd, &buf[bytes_read], count - bytes_read);

        if (num_of_bytes == -1) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        /* return when peer closed connection */
        if (num_of_bytes == 0)
            return bytes_read;

        bytes_read += num_of_bytes;
    }

    if (conn->ctx->is_connection_terminated == true) {
        dlog(DLOG_LEVEL_ERROR, "Reading from tcp-socket aborted");
        return -2;
    }

    return (ssize_t)bytes_read; // [V2G2-537] read bytes are currupted if reading from socket was interrupted
                                // (V2G_SECC_Sequence_Timeout)
}

/*!
 * \brief connection_read This function writes to socket until bytes are written to the socket
 * \param conn is the v2g connection context
 * \param buf is the buffer where the v2g message is stored
 * \param count is the number of bytes to write
 * \return Returns \c true if a timeout has occurred, otherwise \c false
 */
ssize_t connection_write(struct v2g_connection* conn, unsigned char* buf, size_t count) {
    int bytes_written = 0;

    /* loop until we got all requested bytes out */
    while (bytes_written < count) {
        int num_of_bytes = (int)write(conn->conn.socket_fd, &buf[bytes_written], count - bytes_written);
        if (conn->is_tls_connection) {
            return -1; // shouldn't be using this function
        }
        if (num_of_bytes == -1) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        /* return when peer closed connection */
        if (num_of_bytes == 0)
            return bytes_written;

        bytes_written += num_of_bytes;
    }

    return (ssize_t)bytes_written;
}

/*!
 * \brief connection_teardown This function must be called on connection teardown.
 * \param conn is the V2G connection context
 */
void connection_teardown(struct v2g_connection* conn) {
    if (conn->ctx->session.is_charging == true) {
        conn->ctx->p_charger->publish_current_demand_finished(nullptr);

        if (conn->ctx->is_dc_charger == true) {
            conn->ctx->p_charger->publish_dc_open_contactor(nullptr);
        } else {
            conn->ctx->p_charger->publish_ac_open_contactor(nullptr);
        }
    }

    /* init charging session */
    v2g_ctx_init_charging_session(conn->ctx, true);

    /* print dlink status */
    switch (conn->dlink_action) {
    case MQTT_DLINK_ACTION_ERROR:
        dlog(DLOG_LEVEL_TRACE, "d_link/error");
        break;
    case MQTT_DLINK_ACTION_TERMINATE:
        conn->ctx->p_charger->publish_dlink_terminate(nullptr);
        dlog(DLOG_LEVEL_TRACE, "d_link/terminate");
        break;
    case MQTT_DLINK_ACTION_PAUSE:
        conn->ctx->p_charger->publish_dlink_pause(nullptr);
        dlog(DLOG_LEVEL_TRACE, "d_link/pause");
        break;
    }
}

/**
 * This is the 'main' function of a thread, which handles a TCP connection.
 */
static void* connection_handle_tcp(void* data) {
    struct v2g_connection* conn = static_cast<struct v2g_connection*>(data);
    int rv = 0;

    dlog(DLOG_LEVEL_INFO, "Started new TCP connection thread");

    /* check if the v2g-session is already running in another thread, if not, handle v2g-connection */
    if (conn->ctx->state == 0) {
        int rv2 = v2g_handle_connection(conn);

        if (rv2 != 0) {
            dlog(DLOG_LEVEL_INFO, "v2g_handle_connection exited with %d", rv2);
        }
    } else {
        rv = ERROR_SESSION_ALREADY_STARTED;
        dlog(DLOG_LEVEL_WARNING, "%s", "Closing tcp-connection. v2g-session is already running");
    }

    /* tear down connection gracefully */
    dlog(DLOG_LEVEL_INFO, "Closing TCP connection");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (shutdown(conn->conn.socket_fd, SHUT_RDWR) == -1) {
        dlog(DLOG_LEVEL_ERROR, "shutdown() failed: %s", strerror(errno));
    }

    // Waiting for client closing the connection
    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (close(conn->conn.socket_fd) == -1) {
        dlog(DLOG_LEVEL_ERROR, "close() failed: %s", strerror(errno));
    }
    dlog(DLOG_LEVEL_INFO, "TCP connection closed gracefully");

    if (rv != ERROR_SESSION_ALREADY_STARTED) {
        /* cleanup and notify lower layers */
        connection_teardown(conn);
    }

    free(conn);

    return nullptr;
}

static void* connection_server(void* data) {
    struct v2g_context* ctx = static_cast<v2g_context*>(data);
    struct v2g_connection* conn = NULL;
    pthread_attr_t attr;

    /* create the thread in detached state so we don't need to join every single one */
    if (pthread_attr_init(&attr) != 0) {
        dlog(DLOG_LEVEL_ERROR, "pthread_attr_init failed: %s", strerror(errno));
        goto thread_exit;
    }
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        dlog(DLOG_LEVEL_ERROR, "pthread_attr_setdetachstate failed: %s", strerror(errno));
        goto thread_exit;
    }

    while (1) {
        char client_addr[INET6_ADDRSTRLEN];
        struct sockaddr_in6 addr;
        socklen_t addrlen = sizeof(addr);

        /* cleanup old one and create new connection context */
        free(conn);
        conn = static_cast<v2g_connection*>(calloc(1, sizeof(*conn)));
        if (!conn) {
            dlog(DLOG_LEVEL_ERROR, "Calloc failed: %s", strerror(errno));
            break;
        }

        /* setup common stuff */
        conn->ctx = ctx;
        conn->read = &connection_read;
        conn->write = &connection_write;
        conn->is_tls_connection = false;

        /* wait for an incoming connection */
        conn->conn.socket_fd = accept(ctx->tcp_socket, (struct sockaddr*)&addr, &addrlen);
        if (conn->conn.socket_fd == -1) {
            dlog(DLOG_LEVEL_ERROR, "Accept(tcp) failed: %s", strerror(errno));
            continue;
        }

        if (inet_ntop(AF_INET6, &addr, client_addr, sizeof(client_addr)) != NULL) {
            dlog(DLOG_LEVEL_INFO, "Incoming connection on %s from [%s]:%" PRIu16, ctx->if_name, client_addr,
                 ntohs(addr.sin6_port));
        } else {
            dlog(DLOG_LEVEL_ERROR, "Incoming connection on %s, but inet_ntop failed: %s", ctx->if_name,
                 strerror(errno));
        }

        // store the port to create a udp socket
        conn->ctx->udp_port = ntohs(addr.sin6_port);

        if (pthread_create(&conn->thread_id, &attr, connection_handle_tcp, conn) != 0) {
            dlog(DLOG_LEVEL_ERROR, "pthread_create() failed: %s", strerror(errno));
            continue;
        }

        /* is up to the thread to cleanup conn */
        conn = NULL;
    }

thread_exit:
    if (pthread_attr_destroy(&attr) != 0) {
        dlog(DLOG_LEVEL_ERROR, "pthread_attr_destroy failed: %s", strerror(errno));
    }

    /* clean up if dangling */
    free(conn);

    return NULL;
}

int connection_start_servers(struct v2g_context* ctx) {
    int rv, tcp_started = 0;

    if (ctx->tcp_socket != -1) {
        rv = pthread_create(&ctx->tcp_thread, NULL, connection_server, ctx);
        if (rv != 0) {
            dlog(DLOG_LEVEL_ERROR, "pthread_create(tcp) failed: %s", strerror(errno));
            return -1;
        }
        tcp_started = 1;
    }

    if (ctx->tls_socket.fd != -1) {
        rv = tls::connection_start_server(ctx);
        if (rv != 0) {
            if (tcp_started) {
                pthread_cancel(ctx->tcp_thread);
                pthread_join(ctx->tcp_thread, NULL);
            }
            dlog(DLOG_LEVEL_ERROR, "pthread_create(tls) failed: %s", strerror(errno));
            return -1;
        }
    }

    return 0;
}

int create_udp_socket(const uint16_t udp_port, const char* interface_name) {
    constexpr auto LINK_LOCAL_MULTICAST = "ff02::1";

    int udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        EVLOG_error << "Could not create socket: " << strerror(errno);
        return udp_socket;
    }

    // source setup

    // find port between 49152-65535
    auto could_bind = false;
    auto source_port = 49152;
    for (; source_port < 65535; source_port++) {
        sockaddr_in6 source_address = {AF_INET6, htons(source_port)};
        if (bind(udp_socket, reinterpret_cast<sockaddr*>(&source_address), sizeof(sockaddr_in6)) == 0) {
            could_bind = true;
            break;
        }
    }

    if (!could_bind) {
        EVLOG_error << "Could not bind: " << strerror(errno);
        return -1;
    }

    EVLOG_info << "UDP socket bound to source port: " << source_port;

    const auto index = if_nametoindex(interface_name);
    auto mreq = ipv6_mreq{};
    mreq.ipv6mr_interface = index;
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &mreq.ipv6mr_multiaddr) <= 0) {
        EVLOG_error << "Failed to setup multicast address" << strerror(errno);
        return -1;
    }
    if (setsockopt(udp_socket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        EVLOG_error << "Could not add multicast group membership: " << strerror(errno);
        return -1;
    }

    if (setsockopt(udp_socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, &index, sizeof(index)) < 0) {
        EVLOG_error << "Could not set interface name: " << interface_name << "with error: " << strerror(errno);
    }

    // destination setup
    sockaddr_in6 destination_address = {AF_INET6, htons(udp_port)};
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &destination_address.sin6_addr) <= 0) {
        EVLOG_error << "Failed to setup server address" << strerror(errno);
    }
    const auto connected =
        connect(udp_socket, reinterpret_cast<sockaddr*>(&destination_address), sizeof(sockaddr_in6)) == 0;
    if (!connected) {
        EVLOG_error << "Could not connect: " << strerror(errno);
        return -1;
    }

    return udp_socket;
}
