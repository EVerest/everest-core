// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ieee2030/charger/session/callback.hpp>

#include <ieee2030/common/detail/helper.hpp>

namespace ieee2030::charger {

Callback::Callback(callback::Callbacks callbacks_) : callbacks(std::move(callbacks_)) {
}

void Callback::signal(callback::Signal signal) const {
    call_if_available(callbacks.signal, signal);
}

void Callback::hw_signal(const callback::HwSignal& hw_signal) const {
    call_if_available(callbacks.hw_signal, hw_signal);
}

} // namespace ieee2030::charger