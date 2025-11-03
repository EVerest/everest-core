// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/can/socket_can_handler.hpp>
#include <everest/io/event/fd_event_client.hpp>

namespace everest::lib::io::can {

/**
 * @var socket_can
 * @brief Client for socket_can implemented in terms of \ref event::fd_event_client
 * and \ref can::socket_can_handler
 */
using socket_can = event::fd_event_client<socket_can_handler>::type;

} // namespace everest::lib::io::can
