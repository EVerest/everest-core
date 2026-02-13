// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <iostream>
#include <optional>

#include <ieee2030/charger/session/callback.hpp>
#include <ieee2030/charger/v20/context.hpp>
#include <ieee2030/charger/v20/control_event.hpp>
#include <ieee2030/charger/v20/states.hpp>
#include <ieee2030/common/io/logging.hpp>
#include <ieee2030/common/messages/messages.hpp>
#include <ieee2030/fsm/fsm.hpp>

using namespace ieee2030::charger;

class FsmStateHelper {
public:
    FsmStateHelper(const callback::Callbacks& callbacks) :
        ctx(active_event, callbacks, message_100, message_101, message_102) {

        ieee2030::io::set_logging_callback([](std::string message) { std::cout << message; });
    }

    v20::Context& get_context();

    void handle_can_message(const ieee2030::messages::EV100& message_100_,
                            const ieee2030::messages::EV101& message_101_,
                            const ieee2030::messages::EV102& message_102_) {

        this->message_100 = message_100_;
        this->message_101 = message_101_;
        this->message_102 = message_102_;
    }

    void handle_event(const events::Event& event) {
        active_event = event;
    }

private:
    std::optional<events::Event> active_event;

    v20::Context ctx;

    ieee2030::messages::EV100 message_100;
    ieee2030::messages::EV101 message_101;
    ieee2030::messages::EV102 message_102;
};
