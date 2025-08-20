// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "connector_lockImpl.hpp"

namespace module {
namespace connector_lock_1 {

void connector_lockImpl::init() {
}

void connector_lockImpl::ready() {
}

void connector_lockImpl::handle_lock() {
    EVLOG_info << "Locking connector 1";
    mod->serial.lock(1, true);
}

void connector_lockImpl::handle_unlock() {
    EVLOG_info << "Unlocking connector 1";
    mod->serial.lock(1, false);
}

} // namespace connector_lock_1
} // namespace module
