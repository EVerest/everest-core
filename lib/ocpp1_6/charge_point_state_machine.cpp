// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <ocpp1_6/charge_point_state_machine.hpp>

namespace ocpp1_6 {
ChargePointStateMachine::ChargePointStateMachine(
    const std::function<void(ChargePointStatus status)>& status_notification_callback) :
    status_notification_callback(status_notification_callback) {
    this->sd_available.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::A2_UsageInitiated:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::A3_UsageInitiatedWithoutAuthorization:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::A4_UsageInitiatedEVDoesNotStartCharging:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::A5_UsageInitiatedEVSEDoesNotAllowCharging:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::A7_ReserveNowReservesConnector:
            return trans.set(this->sd_reserved);
        case ChargePointStatusTransition::A8_ChangeAvailabilityToUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::A9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_available.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::Available);
    };

    this->sd_preparing.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::B1_IntendedUsageIsEnded:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::B3_PrerequisitesForChargingMetAndChargingStarts:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::B4_PrerequisitesForChargingMetEVDoesNotStartCharging:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::B6_TimedOut:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::B9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_preparing.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::Preparing);
    };

    this->sd_charging.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::C1_ChargingSessionEndsNoUserActionRequired:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::C4_ChargingStopsUponEVRequest:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::C5_ChargingStopsUponEVSERequest:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::C6_TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::
            C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::C9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_charging.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::Charging);
    };

    this->sd_suspended_ev.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::D1_ChargingSessionEndsNoUserActionRequired:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::D3_ChargingResumesUponEVRequest:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::D5_ChargingSuspendedByEVSE:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::D6_TransactionStoppedNoUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::
            D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::D9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_suspended_ev.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::SuspendedEV);
    };

    this->sd_suspended_evse.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::E1_ChargingSessionEndsNoUserActionRequired:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::E3_ChargingResumesEVSERestrictionLifted:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::E4_EVSERestrictionLiftedEVDoesNotStartCharging:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::E6_TransactionStoppedAndUserActionRequired:
            return trans.set(this->sd_finishing);
        case ChargePointStatusTransition::
            E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::E9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_suspended_evse.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::SuspendedEVSE);
    };

    this->sd_finishing.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::F1_AllUserActionsCompleted:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::F2_UsersRestartChargingSession:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::F9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_finishing.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::Finishing);
    };

    this->sd_reserved.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::G1_ReservationExpiresOrCancelReservationReceived:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::G2_ReservationIdentityPresented:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::
            G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable:
            return trans.set(this->sd_unavailable);
        case ChargePointStatusTransition::G9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_reserved.state_fun = [this](FSMContextType& ctx) {
        this->status_notification_callback(ChargePointStatus::Reserved);
    };

    this->sd_unavailable.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ChargePointStatusTransition::H1_ConnectorSetAvailableByChangeAvailability:
            return trans.set(this->sd_available);
        case ChargePointStatusTransition::H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint:
            return trans.set(this->sd_preparing);
        case ChargePointStatusTransition::H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging:
            return trans.set(this->sd_charging);
        case ChargePointStatusTransition::H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging:
            return trans.set(this->sd_suspended_ev);
        case ChargePointStatusTransition::H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging:
            return trans.set(this->sd_suspended_evse);
        case ChargePointStatusTransition::H9_FaultDetected:
            return trans.set(this->sd_faulted);
        default:
            return;
        }
    };
    this->sd_unavailable.state_fun = [this](FSMContextType& ctx) {
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
        this->status_notification_callback(ChargePointStatus::Faulted);
    };
}

ChargePointStates::ChargePointStates(
    int32_t number_of_connectors,
    const std::function<void(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status)>&
        status_notification_callback) :
    status_notification_callback(status_notification_callback) {
    for (size_t connector = 0; connector < number_of_connectors + 1; connector++) {
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
    if (connector > 0 && connector < this->state_machines.size()) {
        this->state_machines.at(connector)->controller->submit_event(event);
    }
}

} // namespace ocpp1_6
