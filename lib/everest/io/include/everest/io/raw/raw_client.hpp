// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/raw/raw_socket.hpp>

namespace everest::lib::io::raw {

/**
 * @var tcp_client
 * @brief Client for RAW ETHERNET implemented in terms of \ref event::fd_event_client
 * and \ref raw::raw_socket
 */
using raw_client = event::fd_event_client<raw_socket>::type;

} // namespace everest::lib::io::raw
