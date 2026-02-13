// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/v20/state/state_b.hpp>
#include <ieee2030/charger/v20/state/state_c.hpp>

namespace ieee2030::charger::v20::state {

void StateB::enter() {
    // ctx.log.enter_state("State B");
    m_ctx.callbacks.hw_signal({callback::ChargerSequence::CS1, callback::Status::ON});
    m_ctx.timeout.start(6);
    // Todo: Start second timeout
}

Result StateB::feed(Event ev) {

    if (ev == Event::CAN_MESSAGE) {
        return m_ctx.create_state<StateC>();
    } else if (ev == Event::TIMEOUT) {
        // Todo: Adding Log -> Timeout error
        stop = true;
    } else if (ev == Event::EVENT) {
        if (const auto event = m_ctx.get_event<events::StopCharging>()) {
            if (*event) {
                // Todo: Adding Log -> Stop button is pressed
                stop = true;
            }
        }
    }

    if (stop) {
        m_ctx.callbacks.hw_signal({callback::ChargerSequence::CS1, callback::Status::OFF});
        // Todo: Stop the session
        return {};
    }

    return {};
}

} // namespace ieee2030::charger::v20::state
