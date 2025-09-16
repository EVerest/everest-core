// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/tcp/tcp_socket.hpp>

namespace everest::lib::io::tcp {

/**
 * @var tcp_client
 * @brief Client for TCP implemented in terms of \ref event::fd_event_client
 * and \ref tcp::tcp_socket
 */
using tcp_client = event::fd_event_client<tcp_socket>::type;

} // namespace everest::lib::io::tcp
