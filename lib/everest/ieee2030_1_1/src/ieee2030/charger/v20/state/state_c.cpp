// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/detail/v20/state/state_c.hpp>
#include <ieee2030/charger/v20/state/state_c.hpp>

namespace ieee2030::charger::v20::state {

bool state_c_1(const messages::EV100& ev_100, const messages::EV101& ev_101, const messages::EV102& ev_102) {

    // Check ev messages if sended
    // If yes, check Battery comp check, calculate output current

    // If yes, start charger can
    // Start Timeout after starting charger can (From cs1 = on)

    return false;
};

bool state_c_2(const messages::EV102& ev_102) {

    // Start Reaming Time Check task

    // Check HW Charge Permission Signal
    // Check 102.5.0 == 1
    return false;
};

void StateC::enter() {
    // ctx.log.enter_state("State C");
}

Result StateC::feed(Event ev) {

    if (ev == Event::CAN_MESSAGE) {
        ev_100 = m_ctx.message_100;
        ev_101 = m_ctx.message_101;
        ev_102 = m_ctx.message_102;
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

    // Todo: Define in a separat file tasks.cpp in state
    //      tasks::monitoring(); -> Check ev can errors
    //      tasks::battery_compability_check();
    //      tasks::calculation_remaming_time();

    // Todo: Adding Monitoring for every main state
    // Todo: Start Battery Compability Check until 102.3 > 0
    // Todo: Start Calculation remaming charging time untule 102.3 > 0

    switch (states) {
    case state_c::InternalStates::C_1:
        if (state_c_1(ev_100, ev_101, ev_102)) {
            states = state_c::InternalStates::C_2;
        }
        break;

    case state_c::InternalStates::C_2:
        if (state_c_2(ev_102)) {
            // return sa.create_simple<StateD>(m_ctx); // Todo: Adding state D
        }
        break;
    }

    if (stop) {
        // Todo(sl): How to stop in state C
        m_ctx.callbacks.hw_signal({callback::ChargerSequence::CS1, callback::Status::OFF});
        // Todo(sl): Stopping the session
        return {};
    }

    return {};
}

} // namespace ieee2030::charger::v20::state
