// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <memory>
#include <optional>

#include <ieee2030/charger/session/callback.hpp>
#include <ieee2030/charger/v20/control_event.hpp>
#include <ieee2030/common/io/time.hpp>
#include <ieee2030/common/messages/messages.hpp>

namespace ieee2030::charger::v20 {

struct CanBrokerContext {
    std::function<void()> enable_can;
    std::function<void()> disable_can;
    std::function<void(defs::ChargerStatusError, bool)> update_status_error;
    std::function<void(uint16_t)> update_reamining_time;
};

class StateBase;
using BasePointerType = std::unique_ptr<StateBase>;

class Context {
public:
    Context(const std::optional<events::Event>&, callback::Callbacks, const messages::EV100&, const messages::EV101&,
            const messages::EV102&);

    template <typename StateType, typename... Args> BasePointerType create_state(Args&&... args) {
        return std::make_unique<StateType>(*this, std::forward<Args>(args)...);
    }

    const auto& get_event() {
        return current_event;
    }

    template <typename T> T const* get_event() {
        if (not current_event.has_value()) {
            return nullptr;
        }

        if (not std::holds_alternative<T>(*current_event)) {
            return nullptr;
        }

        return &std::get<T>(*current_event);
    }

    const messages::EV100& message_100;
    const messages::EV101& message_101;
    const messages::EV102& message_102;

    void set_can_broker_callback(const CanBrokerContext& callbacks_) {
        can_broker_callbacks = callbacks_;
    }

    void enable_can() {
        can_broker_callbacks.enable_can();
    }
    void disable_can() {
        can_broker_callbacks.disable_can();
    }

    void update_status_error(defs::ChargerStatusError status, bool active) {
        can_broker_callbacks.update_status_error(status, active);
    }
    void update_reaminig_time_s(uint16_t seconds) {
        can_broker_callbacks.update_reamining_time(seconds);
    }

    const Callback callbacks;

    // bool& log; // Todo: Adding log file logic and write to log file

    ieee2030::io::Timeout timeout;

private:
    const std::optional<events::Event>& current_event;

    CanBrokerContext can_broker_callbacks;
};

} // namespace ieee2030::charger::v20
