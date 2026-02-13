// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/v20/context.hpp>

namespace ieee2030::charger::v20 {

Context::Context(const std::optional<events::Event>& current_event_, callback::Callbacks callbacks_,
                 const messages::EV100& message_100_, const messages::EV101& message_101_,
                 const messages::EV102& message_102_) :
    current_event{current_event_},
    callbacks(std::move(callbacks_)),
    message_100(message_100_),
    message_101(message_101_),
    message_102(message_102_) {
}

} // namespace ieee2030::charger::v20
