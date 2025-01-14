// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef TLS_CONNECTION_HPP_
#define TLS_CONNECTION_HPP_

#include <cstddef>
#include <unistd.h>

struct v2g_context;
struct v2g_connection;

namespace tls {

/*!
 * \param ctx v2g connection context
 * \return returns 0 on succss and -1 on error
 */
int connection_init(struct v2g_context* ctx);

/*!
 * \param ctx v2g connection context
 * \return returns 0 on succss and -1 on error
 */
int connection_start_server(struct v2g_context* ctx);

/*!
 * \brief connection_read This abstracts a read from the connection socket, so that higher level functions
 * are not required to distinguish between TCP and TLS connections.
 * \param conn v2g connection context
 * \param buf buffer to store received message sequence.
 * \param count number of read bytes.
 * \return Returns the number of read bytes if successful, otherwise returns -1 for reading errors and
 * -2 for closed connection */
ssize_t connection_read(struct v2g_connection* conn, unsigned char* buf, std::size_t count, bool read_complete);

/*!
 * \brief connection_write This abstracts a write to the connection socket, so that higher level functions
 * are not required to distinguish between TCP and TLS connections.
 * \param conn v2g connection context
 * \param buf buffer to store received message sequence.
 * \param count size of the buffer
 * \return Returns the number of read bytes if successful, otherwise returns -1 for reading errors and
 * -2 for closed connection */
ssize_t connection_write(struct v2g_connection* conn, unsigned char* buf, std::size_t count);

int connection_proxy(struct v2g_connection* conn, int proxy_fd);

} // namespace tls

#endif // TLS_CONNECTION_HPP_
