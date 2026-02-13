// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

#include <ieee2030/charger/session/callback.hpp>
#include <ieee2030/charger/session/session.hpp>
#include <ieee2030/charger/v20/control_event.hpp>
#include <ieee2030/common/v20/event_queue.hpp>

namespace ieee2030::charger {

enum class HwSignal {
    CS1,
    CS2,
    CHARGE_PERMISSION,
    PROX,
};

class Controller {
public:
    Controller(std::string, callback::Callbacks);
    ~Controller(); // Todo: Adding destructor

    // Collection of api ideas

    // Call only if the charging session is authorized and the ev is connected
    void start_session(bool, float, float, float, defs::ProtocolNumber);

    // HW Signals
    void update_hw_signal(HwSignal, bool);

    // Update physical values
    void update_present_voltage_current(float, float);
    void update_available_voltage_current(float, float);
    void update_isolation_status(events::IsolationStatus);

    // Events
    void cable_check_finished();
    void stop();

    // Error handling
    void send_error(); // Todo: define error enums
    void reset_errors();

private:
    std::string can_interface;

    ieee2030::events::EventQueue<events::Event> event_queue;

    // HW Signals: Enable/disable CS1 & CS2 done
    // EV Target voltage & current done
    // EV min & max battery voltage
    // Debug infos (protocol, soc)
    // Fault
    // Lock enable/disable?
    //
    const callback::Callbacks callbacks;
};

} // namespace ieee2030::charger
