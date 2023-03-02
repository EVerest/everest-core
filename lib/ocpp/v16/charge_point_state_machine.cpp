// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/enums.hpp>

#include <cstdint>
#include <stddef.h>
#include <stdexcept>
#include <utility>
#include <everest/logging.hpp>

#include <fsm/async.hpp>

namespace ocpp {
namespace v16 {

ChargePointStateMachine::ChargePointStateMachine(const StatusNotificationCallback& status_notification_callback) :
    status_notification_callback(status_notification_callback) {
    this->sd_available.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::UsageInitiated:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::StartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::PauseChargingEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::PauseChargingEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::ReserveConnector:
            return trans.set(this->sd_reserved);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_available.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Available;
        this->status_notification_callback(ChargePointStatus::Available, ChargePointErrorCode::NoError);
    };

    this->sd_preparing.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::StartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::PauseChargingEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::PauseChargingEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_preparing.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Preparing;
        this->status_notification_callback(ChargePointStatus::Preparing, ChargePointErrorCode::NoError);
    };

    this->sd_charging.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::PauseChargingEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::PauseChargingEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_charging.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Charging;
        this->status_notification_callback(ChargePointStatus::Charging, ChargePointErrorCode::NoError);
    };

    this->sd_suspended_ev.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::StartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::PauseChargingEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_suspended_ev.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::SuspendedEV;
        this->status_notification_callback(ChargePointStatus::SuspendedEV, ChargePointErrorCode::NoError);
    };

    this->sd_suspended_evse.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::StartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::PauseChargingEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_suspended_evse.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::SuspendedEVSE;
        this->status_notification_callback(ChargePointStatus::SuspendedEVSE, ChargePointErrorCode::NoError);
    };

    this->sd_finishing.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::UsageInitiated: // user restarts charging session
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_finishing.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Finishing;
        this->status_notification_callback(ChargePointStatus::Finishing, ChargePointErrorCode::NoError);
    };

    this->sd_reserved.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::UsageInitiated:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_reserved.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Reserved;
        this->status_notification_callback(ChargePointStatus::Reserved, ChargePointErrorCode::NoError);
    };

    this->sd_unavailable.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::BecomeAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::UsageInitiated:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::StartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::PauseChargingEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::PauseChargingEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::FaultDetected:
            return trans.set<Event_FaultDetected, ChargePointStateMachine, &ChargePointStateMachine::t_faulted>(ev,
                                                                                                                this);
        default:
            return;
        }
    };
    this->sd_unavailable.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Unavailable;
        this->status_notification_callback(ChargePointStatus::Unavailable, ChargePointErrorCode::NoError);
    };

    this->sd_faulted.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::I1_ReturnToAvailable:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::I2_ReturnToPreparing:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::I3_ReturnToCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::I4_ReturnToSuspendedEV:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::I5_ReturnToSuspendedEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::I6_ReturnToFinishing:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::I7_ReturnToReserved:
            return trans.set(this->sd_reserved);
        case ChargePointStatusTransition::I8_ReturnToUnavailable:
            return trans.set(this->sd_unavailable);
        default:
            return;
        }
    };
    this->sd_faulted.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Faulted;
        this->status_notification_callback(ChargePointStatus::Faulted, sd_faulted.error_code);
    };
}

ChargePointStatus ChargePointStateMachine::get_state() {
    return this->state;
}

// custom transitions
ChargePointStateMachine::StateHandleType& ChargePointStateMachine::t_faulted(const Event_FaultDetected& fault_ev) {
    sd_faulted.error_code = fault_ev.data;
    return sd_faulted;
}

ChargePointStates::ChargePointStates(
    int32_t number_of_connectors,
    const std::function<void(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status)>&
        status_notification_callback) :
    status_notification_callback(status_notification_callback) {
    for (int32_t connector = 0; connector < number_of_connectors + 1; connector++) {
        // TODO special state machine for connector 0
        auto state_machine = std::make_shared<ChargePointStateMachine>(
            [this, connector](ChargePointStatus status, ChargePointErrorCode error_code) {
                this->status_notification_callback(connector, error_code, status);
            });
        auto controller = std::make_shared<ChargePointStateMachineController>();
        this->state_machines.push_back(
            std::make_shared<ChargePointStateMachineWithController>(state_machine, controller));
    }
}

void ChargePointStates::run(std::map<int32_t, v16::AvailabilityType> connector_availability) {
    if (this->state_machines.size() != connector_availability.size()) {
        throw std::runtime_error("Could not initialize charge point state machine, number of initial states given does "
                                 "not match number of state machines.");
    }
    size_t count = 0;
    for (auto& state_machine : this->state_machines) {
        if (connector_availability.at(count) == AvailabilityType::Operative) {
            state_machine->controller->run(state_machine->state_machine->sd_available);
        } else {
            state_machine->controller->run(state_machine->state_machine->sd_unavailable);
        }
        count += 1;
    }
}

void ChargePointStates::submit_event(int32_t connector, const EventBaseType& event) {

    if (connector > 0 && connector < static_cast<int32_t>(this->state_machines.size())) {
        try {
            this->state_machines.at(connector)->controller->submit_event(event);
        } catch (const std::exception &e) {
            EVLOG_warning << "Could not submit event to state machine at connector# " << connector;
        }
    }
}

ChargePointStatus ChargePointStates::get_state(int32_t connector) {
    if (connector > 0 && connector < static_cast<int32_t>(this->state_machines.size())) {
        return this->state_machines.at(connector)->state_machine->get_state();
    } else if (connector == 0) {
        return ChargePointStatus::Available;
    }
    return ChargePointStatus::Unavailable;
}

} // namespace v16
} // namespace ocpp
