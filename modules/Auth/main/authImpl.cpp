// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "authImpl.hpp"

#include <everest/logging.hpp>
#include <utility>

namespace module {
namespace main {

void authImpl::init() {
}

void authImpl::ready() {
}

void authImpl::shutdown() {
}

void authImpl::handle_set_connection_timeout(int& connection_timeout){
    this->mod->set_connection_timeout(connection_timeout);
}

} // namespace main
} // namespace module
