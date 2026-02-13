// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/charger_controller.hpp>

#include <chrono>
#include <memory>
#include <thread>

#include <ieee2030/charger/io/can_broker_charger.hpp>

namespace ieee2030::charger {
Controller::Controller(std::string can_interface_, callback::Callbacks callbacks_) :
    can_interface(can_interface_), callbacks(std::move(callbacks_)) {
}

void Controller::start_session(bool welding_detection, float available_voltage, float available_current,
                               float threshold_voltage, defs::ProtocolNumber protocol) {

    static constexpr auto SESSION_UPDATE_TIMEOUT_MS = 25;

    auto can_broker = std::make_unique<io::CanBrokerCharger>(can_interface);
    can_broker->init_charger_messages(welding_detection, available_voltage, available_current, threshold_voltage,
                                      protocol);
    auto session = Session(std::move(can_broker), callbacks);

    while (session.is_session_active()) {

        std::vector<events::Event> current_events;
        while (auto event = event_queue.pop()) {
            current_events.push_back(event.value());
        }

        session.update(current_events);

        std::this_thread::sleep_for(std::chrono::milliseconds(SESSION_UPDATE_TIMEOUT_MS));
    }
}

// Hardware signals
void Controller::update_hw_signal(HwSignal signal, bool status) {

    switch (signal) {
    case HwSignal::CS1:
        event_queue.push(events::CS1{status});
        break;
    case HwSignal::CS2:
        event_queue.push(events::CS2{status});
        break;
    case HwSignal::PROX:
        event_queue.push(events::ProximityDetection{status});
        break;
    case HwSignal::CHARGE_PERMISSION:
        event_queue.push(events::ChargePermission{status});
        break;
    }
}

// Update physical values

void Controller::update_isolation_status(events::IsolationStatus status) {
    event_queue.push(status);
}

void Controller::update_present_voltage_current(float voltage, float current) {
    event_queue.push(events::PresentVoltageCurrent{voltage, current});
}

void Controller::update_available_voltage_current(float voltage, float current) {
    event_queue.push(events::AvailableVoltageCurrent{voltage, current});
}

// Events
void Controller::cable_check_finished() {
    event_queue.push(events::CableCheckFinished{true});
}

void Controller::stop() {
    event_queue.push(events::StopCharging{true});
}

// Errors
void Controller::send_error() {
}
void Controller::reset_errors() {
}

} // namespace ieee2030::charger
