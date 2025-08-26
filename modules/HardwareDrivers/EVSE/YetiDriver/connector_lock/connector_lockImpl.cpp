// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "connector_lockImpl.hpp"

namespace module {
namespace connector_lock {

void connector_lockImpl::init() {
    mod->serial.signalLockState.connect([this](bool l) { lock_state = l; });
}

void connector_lockImpl::ready() {
}

void connector_lockImpl::handle_lock() {
}

void connector_lockImpl::handle_unlock() {
    mod->serial.forceUnlock();
}

} // namespace connector_lock
} // namespace module
