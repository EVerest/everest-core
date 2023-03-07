// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest

#include "connection.hpp"
#include "log.hpp"
#include "tools.hpp"
#include "v2g_server.hpp"

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <mbedtls/debug.h>
#include <mbedtls/error.h>
#include <mbedtls/sha1.h>
#include <mbedtls/ssl_cache.h>
#include <mbedtls/ssl_internal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif

#define DEFAULT_PKI_PATH SYSCONFDIR "/secc/pki/certs"
#define DEFAULT_KEY_PATH SYSCONFDIR "/secc/pki/keys"

#define DEFAULT_SOCKET_BACKLOG        3
#define DEFAULT_TCP_PORT              61341
#define DEFAULT_TLS_PORT              64109
#define ERROR_SESSION_ALREADY_STARTED 2

#define MBEDTLS_DEBUG_LEVEL_VERBOSE  4
#define MBEDTLS_DEBUG_LEVEL_NO_DEBUG 0

mbedtls_ctr_drbg_context ctr_drbg;
#if defined(MBEDTLS_SSL_CACHE_C)
mbedtls_ssl_cache_context cache;
#endif

static const int v2g_cipher_suites[] = {MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
                                        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, 0};

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

    struct ipv6_mreq mreq = {{0}, 0};

    if (strcmp(v2g_ctx->if_name, "auto") == 0) {
        v2g_ctx->if_name = choose_first_ipv6_interface();
    }

    mreq.ipv6mr_interface = if_nametoindex(v2g_ctx->if_name);
    if (!mreq.ipv6mr_interface) {
        dlog(DLOG_LEVEL_ERROR, "No such interface: %s", v2g_ctx->if_name);
        return -1;
    }

    return (v2g_ctx->if_name == NULL) ? -1 : 0;
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
        if (v2g_ctx->local_tcp_addr == NULL) {
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
            if (inet_ntop(AF_INET6, &v2g_ctx->local_tcp_addr->sin6_addr, buffer, sizeof(buffer)) != NULL) {
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

            if (inet_ntop(AF_INET6, &v2g_ctx->local_tls_addr->sin6_addr, buffer, sizeof(buffer)) != NULL) {
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
    return 0;
}

/*!
 * \brief is_sequence_timeout This function checks if a sequence timeout has occured.
 * \param ts_start Is the time after waiting of the next request message.
 * \param ctx is the V2G context.
 * \return Returns \c true if a timeout has occured, otherwise \c false
 */
static bool is_sequence_timeout(struct timespec ts_start, struct v2g_context* ctx) {
    struct timespec ts_current;
    int sequence_timeout = V2G_SEQUENCE_TIMEOUT_60S;

    if (((clock_gettime(CLOCK_MONOTONIC, &ts_current)) != 0) ||
        (timespec_to_ms(timespec_sub(ts_current, ts_start)) > sequence_timeout)) {
        dlog(DLOG_LEVEL_ERROR, "Sequence timeout has occured (message: %s)", v2g_msg_type[ctx->current_v2g_msg]);
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
 * \return Returns \c true if a timeout has occured, otherwise \c false
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
            num_of_bytes = mbedtls_ssl_read(&conn->conn.ssl.ssl_context, &buf[bytes_read], count - bytes_read);

            if (num_of_bytes == MBEDTLS_ERR_SSL_WANT_READ || num_of_bytes == MBEDTLS_ERR_SSL_WANT_WRITE ||
                num_of_bytes == MBEDTLS_ERR_SSL_TIMEOUT)
                continue;

            if (num_of_bytes == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || num_of_bytes == MBEDTLS_ERR_SSL_CONN_EOF)
                return bytes_read;

            if (num_of_bytes < 0) {
                char error_buf[100];
                mbedtls_strerror(num_of_bytes, error_buf, sizeof(error_buf));
                dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_read() error: %s", error_buf);

                return -1;
            }
        } else {
            /* use select for timeout handling */
            struct timeval tv;
            fd_set read_fds;

            FD_ZERO(&read_fds);
            FD_SET(conn->conn.socket_fd, &read_fds);

            tv.tv_sec = conn->ctx->network_read_timeout / 1000;
            tv.tv_usec = (conn->ctx->network_read_timeout % 1000) * 1000;

            num_of_bytes = select(conn->conn.socket_fd + 1, &read_fds, NULL, NULL, &tv);

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
 * \return Returns \c true if a timeout has occured, otherwise \c false
 */
ssize_t connection_write(struct v2g_connection* conn, unsigned char* buf, size_t count) {
    int bytes_written = 0;

    /* loop until we got all requested bytes out */
    while (bytes_written < count) {
        int num_of_bytes;

        if (conn->is_tls_connection) {
            num_of_bytes = mbedtls_ssl_write(&conn->conn.ssl.ssl_context, &buf[bytes_written], count - bytes_written);

            if (num_of_bytes == MBEDTLS_ERR_SSL_WANT_READ || num_of_bytes == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if (num_of_bytes < 0)
                return -1;

        } else {
            num_of_bytes = (int)write(conn->conn.socket_fd, &buf[bytes_written], count - bytes_written);

            if (num_of_bytes == -1) {
                if (errno == EINTR)
                    continue;

                return -1;
            }
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
static void connection_teardown(struct v2g_connection* conn) {
    if (conn->ctx->session.is_charging == true) {
        conn->ctx->p_charger->publish_currentDemand_Finished(boost::blank{});

        if (conn->ctx->is_dc_charger == true) {
            conn->ctx->p_charger->publish_DC_Open_Contactor(true);
        } else {
            conn->ctx->p_charger->publish_AC_Open_Contactor(true);
        }
    }

    /* init charging session */
    v2g_ctx_init_charging_session(conn->ctx, true);

    /* stop timer */
    stop_timer(&conn->ctx->com_setup_timeout, NULL, conn->ctx);

    /* print dlink status */
    switch (conn->dlink_action) {
    case MQTT_DLINK_ACTION_ERROR:
        dlog(DLOG_LEVEL_TRACE, "d_link/error");
        break;
    case MQTT_DLINK_ACTION_TERMINATE:
        dlog(DLOG_LEVEL_TRACE, "d_link/terminate");
        break;
    case MQTT_DLINK_ACTION_PAUSE:
        dlog(DLOG_LEVEL_TRACE, "d_link/pause");
        break;
    }
}

static bool connection_init_tls(struct v2g_context* ctx) {

    int rv;

    char* file_path = static_cast<char*>(
        malloc(max(std::string(DEFAULT_PKI_PATH).size(), std::string(DEFAULT_KEY_PATH).size()) + MAX_FILE_NAME_LENGTH));
    uint8_t max_idx = 0;
    mbedtls_x509_crt* root_crt = NULL;

    /* Load supported v2g root certificates */

    // Determine number of v2g-folders
    DIR* d = opendir(DEFAULT_PKI_PATH); // open the path
    struct dirent* dir;                 // for the directory entries

    if (d == NULL) {
        dlog(DLOG_LEVEL_ERROR, "Unable to open pki file path: %s", DEFAULT_PKI_PATH);
        goto error_out;
    }

    while ((dir = readdir(d)) != NULL)
        if ((dir->d_type == DT_DIR) && (std::string(dir->d_name).size() && isdigit((int)dir->d_name[0])) &&
            ((atoi(dir->d_name) <= UINT8_MAX))) {
            max_idx = max(max_idx, (uint8_t)(atoi(dir->d_name)));
        }

    closedir(d);

    dlog(DLOG_LEVEL_INFO, "PKI folders found: %i", max_idx + 1);

    ctx->num_of_tls_crt = 0;
    ctx->evseTlsCrt = static_cast<mbedtls_x509_crt*>(malloc(sizeof(mbedtls_x509_crt) * (max_idx + 1)));
    ctx->evse_tls_crt_key = static_cast<mbedtls_pk_context*>(malloc(sizeof(mbedtls_pk_context) * (max_idx + 1)));
    root_crt = &ctx->v2g_root_crt;

    for (uint8_t pki_idx = 0; pki_idx <= max_idx; pki_idx++) {
        mbedtls_x509_crt_init(&ctx->evseTlsCrt[pki_idx]);
        char v2g_root_file_name[std::string(V2G_ROOT_CRT_NAME).size() + std::string(".xxx").size() + 1];
        sprintf(file_path, "%s/%u", DEFAULT_PKI_PATH, pki_idx);
        bool got_file_name =
            get_dir_filename(v2g_root_file_name, sizeof(v2g_root_file_name), file_path, V2G_ROOT_CRT_NAME);
        if (got_file_name) {
            sprintf(file_path, "%s/%u/%s", DEFAULT_PKI_PATH, pki_idx, v2g_root_file_name);
            dlog(DLOG_LEVEL_TRACE, "Checking PKI root file %s", file_path);
            if ((rv = mbedtls_x509_crt_parse_file(&ctx->v2g_root_crt, file_path)) != 0) {
                char error_buf[100];
                mbedtls_strerror(rv, error_buf, sizeof(error_buf));
                dlog(DLOG_LEVEL_ERROR, "Unable to parse v2g root certificate %s (err: -0x%04x - %s)", file_path, -rv,
                     error_buf);
                goto error_out;
            }
        } else {
            dlog(DLOG_LEVEL_ERROR, "No file matching %s found in path %s", V2G_ROOT_CRT_NAME, file_path);
            goto error_out;
        }

#ifdef MBEDTLS_SSL_TRUSTED_CA_KEYS
        /* Configure trusted ca certificates */
        unsigned char trusted_id[20];
        if (root_crt->next != NULL)
            root_crt = root_crt->next;
        mbedtls_sha1(root_crt->raw.p, root_crt->raw.len, trusted_id);
        mbedtls_ssl_conf_trusted_authority(&ctx->ssl_config, trusted_id, sizeof(trusted_id),
                                           MBEDTLS_SSL_CA_ID_TYPE_CERT_SHA1_HASH);
        // dlog(DLOG_LEVEL_ERROR, "rootCrtrootCrtrootCrtrootCrt %s", rootCrt->issuer.val.p);
#endif // MBEDTLS_SSL_TRUSTED_CA_KEYS

        sprintf(file_path, "%s/%u/%s", DEFAULT_KEY_PATH, pki_idx, EVSE_LEAF_KEY_FILE_NAME);
        mbedtls_pk_init(&ctx->evse_tls_crt_key[pki_idx]);
        rv = mbedtls_pk_parse_keyfile(&ctx->evse_tls_crt_key[pki_idx], file_path, ctx->basic_config.keyFilePw[0]);
        if (rv != 0) {
            char error_buf[100];
            mbedtls_strerror(rv, error_buf, sizeof(error_buf));
            dlog(DLOG_LEVEL_ERROR, "mbedtls_pk_parse_keyfile returned -0x%04x - %s", -rv, error_buf);
            goto error_out;
        }

        sprintf(file_path, "%s/%u", DEFAULT_PKI_PATH, pki_idx);
        char file_names[MAX_PKI_CA_LENGTH][MAX_FILE_NAME_LENGTH] = {0};
        uint8_t num_of_files = get_dir_numbered_file_names(file_names, file_path, "CPO_", "", 0, MAX_PKI_CA_LENGTH);

        for (uint8_t idx = 0; idx < num_of_files; idx++) {
            sprintf(file_path, "%s/%u/%s", DEFAULT_PKI_PATH, pki_idx, file_names[idx]);
            dlog(DLOG_LEVEL_TRACE, "Checking PKI file %s", file_path);

            if ((rv = mbedtls_x509_crt_parse_file(&ctx->evseTlsCrt[pki_idx], file_path)) != 0) {
                char error_buf[100];
                mbedtls_strerror(rv, error_buf, sizeof(error_buf));
                dlog(DLOG_LEVEL_ERROR, "Unable to parse evse-leaf certficate %s (err: -0x%04x - %s idx: %i)", file_path,
                     -rv, error_buf, idx);
                mbedtls_pk_free(&ctx->evse_tls_crt_key[pki_idx]);
                goto error_out;
            }
        }

        ctx->num_of_tls_crt++;

        if ((rv = mbedtls_ssl_conf_own_cert(&ctx->ssl_config, &ctx->evseTlsCrt[pki_idx],
                                            &ctx->evse_tls_crt_key[pki_idx])) != 0) {
            char error_buf[100];
            mbedtls_strerror(rv, error_buf, sizeof(error_buf));
            dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_conf_own_cert returned -0x%04x - %s", -rv, error_buf);
            goto error_out;
        }
    }

    if ((rv = mbedtls_ssl_config_defaults(&ctx->ssl_config, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        char error_buf[100];
        mbedtls_strerror(rv, error_buf, sizeof(error_buf));
        dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_config_defaults returned -0x%04x - %s", -rv, error_buf);
        goto error_out;
    }

    mbedtls_ssl_conf_authmode(&ctx->ssl_config, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&ctx->ssl_config, mbedtls_ctr_drbg_random, &ctr_drbg);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_conf_session_cache(&ctx->ssl_config, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
#endif
    mbedtls_ssl_conf_ciphersuites(&ctx->ssl_config, v2g_cipher_suites);
    mbedtls_ssl_conf_read_timeout(&ctx->ssl_config, ctx->network_read_timeout);

    if (file_path != NULL)
        free(file_path);
    return true;

error_out:

    if (file_path != NULL)
        free(file_path);
    return false;
}

/*!
 * \brief ssl_key_log_debug_callback This static function is the example code to produce key log file compatible with
 *  Wireshark from https://github.com/Lekensteyn/mbedtls/commit/68aea15833e1ac9290b8f52a4223fb4585fb3986.
 * \param ACtx is the context of the call back.
 * \param ALevel is the debug level.
 * \param AFile
 * \param ALine
 * \param AStr
 */
static void ssl_key_log_debug_callback(void* ACtx, int ALevel, const char* AFile, int ALine, const char* AStr) {
    keylogDebugCtx* ctx = static_cast<keylogDebugCtx*>(ACtx);

    ((void)ALevel);
    ((void)AFile);
    ((void)ALine);

    if (strstr(AStr, "dumping 'client hello, random bytes' (32 bytes)")) {
        ctx->inClientRandom = true;
        ctx->hexdumpLinesToProcess = 2;
        fputs("CLIENT_RANDOM ", ctx->file);
        return;
    } else if (strstr(AStr, "dumping 'master secret' (48 bytes)")) {
        ctx->inMasterSecret = true;
        ctx->hexdumpLinesToProcess = 3;
        fputc(' ', ctx->file);
        return;
    } else if (((ctx->inClientRandom == false) && (ctx->inMasterSecret == false)) ||
               (ctx->hexdumpLinesToProcess == 0)) {
        if ((ctx->inClientRandom == true) && (ctx->inMasterSecret == true)) {
        }
        return;
    }
    /* Parse "0000:  64 df 18 71 ca 4a 4b e4 63 87 2a ef 5f 29 ca ff  ..." */
    AStr = strstr(AStr, ":  ");
    if (!AStr || std::string(AStr).size() < 3 + 3 * 16) {
        goto reset; /* not the expected hex buffer */
    }
    AStr += 3; /* skip over ":  " */
    /* Process sequences of "hh " */
    for (int i = 0; i < (3 * 16); i += 3) {
        char c1 = AStr[i], c2 = AStr[i + 1], c3 = AStr[i + 2];
        if ((('0' <= c1 && c1 <= '9') || ('a' <= c1 && c1 <= 'f')) &&
            (('0' <= c2 && c2 <= '9') || ('a' <= c2 && c2 <= 'f')) && c3 == ' ') {
            fputc(c1, ctx->file);
            fputc(c2, ctx->file);
        } else {
            goto reset; /* unexpected non-hex char */
        }
    }
    if (((--ctx->hexdumpLinesToProcess) != 0) || (false == ctx->inMasterSecret)) {
        return; /* line is not yet finished. */
    }
reset:
    ctx->hexdumpLinesToProcess = 0;
    ctx->inClientRandom = false;
    ctx->inMasterSecret = false;
    fputc('\n', ctx->file); /* finish key log line */
    fflush(ctx->file);
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

    if (shutdown(conn->conn.socket_fd, SHUT_RDWR) == -1) {
        dlog(DLOG_LEVEL_ERROR, "shutdown() failed: %s", strerror(errno));
    }
    if (close(conn->conn.socket_fd) == -1) {
        dlog(DLOG_LEVEL_ERROR, "close() failed: %s", strerror(errno));
    }
    dlog(DLOG_LEVEL_INFO, "TCP connection closed gracefully");

    if (rv != ERROR_SESSION_ALREADY_STARTED) {
        /* cleanup and notify lower layers */
        connection_teardown(conn);
    }

    free(conn);

    return NULL;
}

/**
 * This is the 'main' function of a thread, which handles a TLS connection.
 */
static void* connection_handle_tls(void* data) {
    struct v2g_connection* conn = static_cast<v2g_connection*>(data);
    mbedtls_ssl_config* ssl_config = conn->conn.ssl.ssl_config;
    mbedtls_net_context* client_fd = &conn->conn.ssl.tls_client_fd;
    mbedtls_ssl_context* ssl = &conn->conn.ssl.ssl_context;

    mbedtls_x509_crt_init(&conn->ctx->v2g_root_crt);
    mbedtls_ssl_config_init(&conn->ctx->ssl_config);
    conn->ctx->num_of_tls_crt = 0;
    conn->ctx->evseTlsCrt = NULL;
    conn->ctx->evse_tls_crt_key = NULL;

    if (NULL != conn->ctx->privateKeyFilePath)
        free(conn->ctx->privateKeyFilePath);
    if (NULL != conn->ctx->certFilePath)
        free(conn->ctx->certFilePath);
    conn->ctx->certFilePath = static_cast<char*>(malloc(std::string(DEFAULT_PKI_PATH).size() + 3));
    conn->ctx->privateKeyFilePath = static_cast<char*>(malloc(std::string(DEFAULT_KEY_PATH).size() + 3));

    int rv = -1;

    dlog(DLOG_LEVEL_INFO, "Started new TLS connection thread");

    if (connection_init_tls(conn->ctx) == false) {
        goto thread_exit;
    }

    /* init new SSL context */
    mbedtls_ssl_init(ssl);

    /* Code to start the ssl-key-log-tracing */
    // Activate full debugging to receive the demanded  key-log-msgs

    if (NULL != conn->ctx->tls_log_ctx.file) {
        mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL_VERBOSE);
        conn->ctx->tls_log_ctx.inClientRandom = false;
        conn->ctx->tls_log_ctx.inMasterSecret = false;
        conn->ctx->tls_log_ctx.hexdumpLinesToProcess = 0;
        mbedtls_ssl_conf_dbg(ssl_config, ssl_key_log_debug_callback, &conn->ctx->tls_log_ctx);

    } else {
        mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL_NO_DEBUG);
    }

    /* get the SSL context ready */
    if ((rv = mbedtls_ssl_setup(ssl, ssl_config)) != 0) {
        dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_setup returned -0x%04x", -rv);
        goto thread_exit;
    }
    //    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL_VERBOSE);
    //    mbedtls_ssl_conf_dbg(ssl_config, mbedDlogCallback, &conn->ctx->tls_log_ctx);

    mbedtls_ssl_set_bio(ssl, client_fd, mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

    /* TLS handshake */
    dlog(DLOG_LEVEL_INFO, "Performing TLS handshake");

    rv = 0;
    do {
        if (ssl == NULL || ssl->conf == NULL) {
            goto thread_exit;
        }

        while (ssl->state != MBEDTLS_SSL_HANDSHAKE_OVER) {
            rv = mbedtls_ssl_handshake_step(ssl);

            /* Determine used v2g-root certificate */
            if (ssl->state == MBEDTLS_SSL_SERVER_HELLO_DONE) {
                mbedtls_x509_crt* caChain = ssl_config->key_cert->cert;
                uint8_t pkiIdx;

                for (pkiIdx = 0; caChain != NULL && (caChain != mbedtls_ssl_own_cert(ssl)); pkiIdx++) {
                    caChain = ssl_config->key_cert->next->cert;
                }
                sprintf(conn->ctx->certFilePath, "%s/%u", DEFAULT_PKI_PATH, pkiIdx);
                sprintf(conn->ctx->privateKeyFilePath, "%s/%u", DEFAULT_KEY_PATH, pkiIdx);

                dlog(DLOG_LEVEL_INFO, "Using v2g-root cert of index #%u for tls-handshake", pkiIdx);
            }

            if (rv != 0) {
                if (((rv != MBEDTLS_ERR_SSL_WANT_READ) && (rv != MBEDTLS_ERR_SSL_WANT_WRITE) &&
                     (rv != MBEDTLS_ERR_SSL_TIMEOUT)) ||
                    (NULL == conn->ctx->com_setup_timeout)) {
                    dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_handshake returned -0x%04x", -rv);
                    goto thread_exit;
                }
                break;
            }
        }
    } while (rv != 0);

    dlog(DLOG_LEVEL_INFO, "TLS handshake succeeded");

    /* Deactivate tls-debug-mode after the tls-handshake, because of performance reasons */
    if (conn->ctx->tls_log_ctx.file != NULL)
        mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL_NO_DEBUG);

    /* check if the v2g-session is already running in another thread, if not handle v2g-connection */
    if (conn->ctx->state == 0) {
        rv = v2g_handle_connection(conn);
        dlog(DLOG_LEVEL_INFO, "v2g_dispatch_connection exited with %d", rv);
    } else {
        rv = ERROR_SESSION_ALREADY_STARTED;
        dlog(DLOG_LEVEL_INFO, "%s", "Closing tls-connection. v2g-session is already running");
    }

    /* tear down TLS connection gracefully */
    dlog(DLOG_LEVEL_INFO, "Closing TLS connection");
    while ((rv = mbedtls_ssl_close_notify(ssl)) < 0) {
        if ((rv != MBEDTLS_ERR_SSL_WANT_READ) && (rv != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            dlog(DLOG_LEVEL_ERROR, "mbedtls_ssl_close_notify returned -0x%04x", -rv);
            goto thread_exit;
        }
    }
    dlog(DLOG_LEVEL_INFO, "TLS connection closed gracefully");

    rv = 0;

thread_exit:
    dlog(DLOG_LEVEL_INFO, "Closing TLS connection thread");

    if ((conn->ctx->end_tls_debug_by_sessionStop == true) && (conn->ctx->tls_log_ctx.file != NULL)) {
        fclose(conn->ctx->tls_log_ctx.file);
        memset(&conn->ctx->tls_log_ctx, 0, sizeof(conn->ctx->tls_log_ctx));
        dlog(DLOG_LEVEL_INFO, "TLS-debug-mode is now disabled!");
    }

    if (rv != 0) {
        char error_buf[100];
        mbedtls_strerror(rv, error_buf, sizeof(error_buf));
        dlog(DLOG_LEVEL_ERROR, "Last TLS error was: -0x%04x - %s", -rv, error_buf);
    }

    mbedtls_net_free(client_fd);
    mbedtls_ssl_free(ssl);

    for (uint8_t idx = 0; idx < conn->ctx->num_of_tls_crt; idx++) {
        mbedtls_pk_free(&conn->ctx->evse_tls_crt_key[idx]);
        mbedtls_x509_crt_free(&conn->ctx->evseTlsCrt[idx]);
    }
    conn->ctx->num_of_tls_crt = 0;
    if (conn->ctx->evseTlsCrt != NULL) {
        free(conn->ctx->evseTlsCrt);
        conn->ctx->evseTlsCrt = NULL;
    }
    if (conn->ctx->evse_tls_crt_key != NULL) {
        free(conn->ctx->evse_tls_crt_key);
        conn->ctx->evse_tls_crt_key = NULL;
    }

    mbedtls_x509_crt_free(&conn->ctx->v2g_root_crt);
    mbedtls_ssl_config_free(&conn->ctx->ssl_config);

    if (conn->ctx->privateKeyFilePath != NULL) {
        free(conn->ctx->privateKeyFilePath);
        conn->ctx->privateKeyFilePath = NULL;
    }
    if (conn->ctx->certFilePath != NULL) {
        free(conn->ctx->certFilePath);
        conn->ctx->certFilePath = NULL;
    }

    /* cleanup and notify lower layers */
    connection_teardown(conn);

    free(conn);

    return NULL;
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

        /* if this thread is the TLS thread, then connections are TLS secured;
         * return code is non-zero if equal so align it
         */
        conn->is_tls_connection = !!pthread_equal(pthread_self(), ctx->tls_thread);

        /* wait for an incoming connection */
        if (conn->is_tls_connection) {
            conn->conn.ssl.ssl_config = &ctx->ssl_config;

            /* at the moment, this is simply resetting the fd to -1; kept for upwards compatibility */
            mbedtls_net_init(&conn->conn.ssl.tls_client_fd);

            conn->conn.ssl.tls_client_fd.fd = accept(ctx->tls_socket.fd, (struct sockaddr*)&addr, &addrlen);
            if (conn->conn.ssl.tls_client_fd.fd == -1) {
                dlog(DLOG_LEVEL_ERROR, "Accept(tls) failed: %s", strerror(errno));
                continue;
            }
        } else {
            conn->conn.socket_fd = accept(ctx->tcp_socket, (struct sockaddr*)&addr, &addrlen);
            if (conn->conn.socket_fd == -1) {
                dlog(DLOG_LEVEL_ERROR, "Accept(tcp) failed: %s", strerror(errno));
                continue;
            }
        }

        if (inet_ntop(AF_INET6, &addr, client_addr, sizeof(client_addr)) != NULL) {
            dlog(DLOG_LEVEL_INFO, "Incoming connection on %s from [%s]:%" PRIu16, ctx->if_name, client_addr,
                 ntohs(addr.sin6_port));
        } else {
            dlog(DLOG_LEVEL_ERROR, "Incoming connection on %s, but inet_ntop failed: %s", ctx->if_name,
                 strerror(errno));
        }

        if (pthread_create(&conn->thread_id, &attr,
                           conn->is_tls_connection ? connection_handle_tls : connection_handle_tcp, conn) != 0) {
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
        rv = pthread_create(&ctx->tls_thread, NULL, connection_server, ctx);
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
