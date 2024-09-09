// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "connector_lockImpl.hpp"

namespace module {
namespace connector_lock {

void connector_lockImpl::init() {
}

void connector_lockImpl::ready() {
}

void connector_lockImpl::handle_lock() {
    EVLOG_info << "Lock connector";
}

void connector_lockImpl::handle_unlock() {
    EVLOG_info << "Unlock connector";
}

} // namespace connector_lock
} // namespace module
