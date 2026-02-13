// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include <ieee2030/common/messages/messages.hpp>

namespace ieee2030::ev::io {

enum class CanEvent {
    ACTIVE,
    NEW_DATA,
    INACTIVE,
};

using CanEventCallback = std::function<void(CanEvent)>;

// Todo(sl): Check if refactoring with CanBrokerCharger is possible
class CanBrokerEv {

    enum class SendState {
        ID_100,
        ID_101,
        ID_102,
    };

public:
    CanBrokerEv(){}; // Todo(sl): Check if needed
    CanBrokerEv(const std::string&);
    ~CanBrokerEv();

    void set_event_callback(const CanEventCallback&);

    void enable_tx_can() {
        tx_active = true;
    };
    void disable_tx_can() {
        tx_active = false;
    };

    const messages::Charger108& get_can_108_message() const {
        return message_108;
    };
    const messages::Charger109& get_can_109_message() const {
        return message_109;
    }

    void init_messages(); // Todo(sl): Define init arguments

    // Todo(sl): define update functions

private:
    std::atomic_bool exit_rx_loop{false};
    std::atomic_bool rx_active{false};
    void rx_loop();
    std::thread rx_loop_thread;
    void handle_can_input(uint32_t, const std::vector<uint8_t>&);

    std::atomic_bool exit_tx_loop{false};
    std::atomic_bool tx_active{false};
    void tx_loop();
    std::thread tx_loop_thread;
    void send(uint32_t, const std::vector<uint8_t>&);

    int can_fd{-1};

    CanEventCallback event_callback{nullptr};

    void publish_event(CanEvent event) {
        if (!event_callback) {
            return;
        }
        event_callback(event);
    }

    messages::EV100 message_100;
    messages::EV101 message_101;
    messages::EV102 message_102;

    messages::Charger108 message_108;
    messages::Charger109 message_109;

    SendState tx_state{SendState::ID_100};
};

} // namespace ieee2030::ev::io
