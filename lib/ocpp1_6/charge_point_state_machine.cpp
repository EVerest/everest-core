// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <ocpp1_6/charge_point_state_machine.hpp>
#include <ocpp1_6/enums.hpp>

#include <cstdint>
#include <stddef.h>
#include <stdexcept>
#include <utility>

#include <fsm/async.hpp>

namespace ocpp1_6 {
ChargePointStateMachine::ChargePointStateMachine(
    const std::function<void(ChargePointStatus status)>& status_notification_callback) :
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_available.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Available;
        this->status_notification_callback(ChargePointStatus::Available);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_preparing.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Preparing;
        this->status_notification_callback(ChargePointStatus::Preparing);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_charging.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Charging;
        this->status_notification_callback(ChargePointStatus::Charging);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_suspended_ev.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::SuspendedEV;
        this->status_notification_callback(ChargePointStatus::SuspendedEV);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_suspended_evse.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::SuspendedEVSE;
        this->status_notification_callback(ChargePointStatus::SuspendedEVSE);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_finishing.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Finishing;
        this->status_notification_callback(ChargePointStatus::Finishing);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_reserved.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Reserved;
        this->status_notification_callback(ChargePointStatus::Reserved);
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
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_unavailable.state_fun = [this](FSMContextType& ctx) {
        this->state = ChargePointStatus::Unavailable;
        this->status_notification_callback(ChargePointStatus::Unavailable);
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
        this->status_notification_callback(ChargePointStatus::Faulted);
    };
}

ChargePointStatus ChargePointStateMachine::get_state() {
    return this->state;
}

ChargePointStates::ChargePointStates(
    int32_t number_of_connectors,
    const std::function<void(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status)>&
        status_notification_callback) :
    status_notification_callback(status_notification_callback) {
    for (int32_t connector = 0; connector < number_of_connectors + 1; connector++) {
        // TODO special state machine for connector 0
        auto state_machine = std::make_shared<ChargePointStateMachine>([this, connector](ChargePointStatus status) {
            this->status_notification_callback(connector, ChargePointErrorCode::NoError, status);
        });
        auto controller = std::make_shared<ChargePointStateMachineController>();
        this->state_machines.push_back(
            std::make_shared<ChargePointStateMachineWithController>(state_machine, controller));
    }
}

void ChargePointStates::run(std::map<int32_t, ocpp1_6::AvailabilityType> connector_availability) {
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

void ChargePointStates::submit_event(int32_t connector, EventBaseType event) {
    if (connector > 0 && connector < static_cast<int32_t>(this->state_machines.size())) {
        this->state_machines.at(connector)->controller->submit_event(event);
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

} // namespace ocpp1_6
