// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <memory>
#include <vector>

#include <ieee2030/charger/io/can_broker_charger.hpp>
#include <ieee2030/common/messages/messages.hpp>

#include <ieee2030/charger/v20/context.hpp>
#include <ieee2030/charger/v20/control_event.hpp>
#include <ieee2030/charger/v20/states.hpp>
#include <ieee2030/fsm/fsm.hpp>

namespace ieee2030::charger {

struct SessionState {
    bool can_active{false};
    bool new_data{false};
};

class Session {
public:
    Session(std::unique_ptr<io::CanBrokerCharger>, const callback::Callbacks&);
    ~Session();

    void update(const std::vector<events::Event>&);

    bool is_session_active() const {
        return session_is_active;
    }

    // void end_session() {
    //     session_is_active = false;
    // }

private:
    std::unique_ptr<io::CanBrokerCharger> can_broker;

    std::optional<events::Event> active_event{std::nullopt};

    SessionState state;

    v20::Context ctx;

    fsm::v2::FSM<ieee2030::charger::v20::StateBase> fsm;

    bool session_is_active;

    void handle_can_event(io::CanEvent);

    messages::EV100 message_100;
    messages::EV101 message_101;
    messages::EV102 message_102;
};

} // namespace ieee2030::charger
