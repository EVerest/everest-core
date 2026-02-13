// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/udp/udp_socket.hpp>

namespace everest::lib::io::udp {

/**
 * @var udp_client
 * @brief Client for UDP implemented in terms of \ref event::fd_event_client
 * and \ref udp::udp_client_socket
 */
using udp_client = event::fd_event_client<udp_client_socket>::type;

/**
 * @var udp_server
 * @brief Server for UDP implemented in terms of \ref event::fd_event_client
 * and \ref udp::udp_server_socket
 */
using udp_server = event::fd_event_client<udp_server_socket>::type;

} // namespace everest::lib::io::udp
