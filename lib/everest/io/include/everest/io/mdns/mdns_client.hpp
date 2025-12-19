// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/mdns/mdns_socket.hpp>

namespace everest::lib::io::mdns {

/**
 * @var mdns_client
 * @brief Client for MDNS discovery implemented in terms of \ref event::fd_event_client
 * and \ref mdns::mdns_socket
 */
using mdns_client = event::fd_event_client<mdns_socket>::type;

} // namespace everest::lib::io::mdns
