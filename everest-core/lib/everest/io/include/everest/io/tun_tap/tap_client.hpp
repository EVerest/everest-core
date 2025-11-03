// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/tun_tap/tap_handler.hpp>

namespace everest::lib::io::tun_tap {

/**
 * @var tap_client
 * @brief Client for names piped and tap devices implemented in terms of \ref event::fd_event_client
 * and \ref tun_tap::tap_handler;
 */
using tap_client = event::fd_event_client<tap_handler>::type;

} // namespace everest::lib::io::tun_tap
