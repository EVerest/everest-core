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

namespace ieee2030::charger::io {

enum class CanEvent {
    ACTIVE,
    NEW_DATA,
    INACTIVE,
};

using CanEventCallback = std::function<void(CanEvent)>;

class CanBrokerCharger {

    enum class SendState {
        ID_108,
        ID_109,
    };

public:
    CanBrokerCharger();
    CanBrokerCharger(const std::string&);
    ~CanBrokerCharger();

    void set_event_callback(const CanEventCallback&);

    void enable_tx_can() {
        tx_active = true;
    };
    void disable_tx_can() {
        tx_active = false;
    };

    bool rx_can_enabled() {
        return rx_active;
    }

    const messages::EV100& get_can_100_message() const {
        return message_100;
    }

    const messages::EV101& get_can_101_message() const {
        return message_101;
    }

    const messages::EV102& get_can_102_message() const {
        return message_102;
    }

    void init_charger_messages(bool welding_detection, float available_voltage, float available_current,
                               float threshold_voltage, defs::ProtocolNumber protocol) {
        message_108 = messages::Charger108(welding_detection, available_voltage, available_current, threshold_voltage);
        message_109 = messages::Charger109(protocol);
    };

    void update_present_voltage(float voltage) {
        message_109.present_voltage = voltage;
    }

    void update_present_current(float current) {
        message_109.present_current = current;
    }

    void update_available_voltage(float voltage) {
        message_108.available_voltage = voltage;
    }

    void update_available_current(float current) {
        message_108.available_current = current;
    }

    void update_threshold_voltage(float voltage) {
        message_108.threshold_voltage = voltage;
    }

    void update_status_error_flag(defs::ChargerStatusError, bool);
    void update_reamining_time_10s(uint16_t);

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

    SendState tx_state{SendState::ID_108};
};

} // namespace ieee2030::charger::io
