// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHARGE_POINT_STATE_MACHINE_HPP
#define OCPP1_6_CHARGE_POINT_STATE_MACHINE_HPP

#include <fsm/async.hpp>
#include <fsm/specialization/async/pthread.hpp>
#include <fsm/utils/Identifiable.hpp>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

enum class ChargePointStatusTransition
{
    A2_UsageInitiated,
    A3_UsageInitiatedWithoutAuthorization,
    A4_UsageInitiatedEVDoesNotStartCharging,
    A5_UsageInitiatedEVSEDoesNotAllowCharging,
    A7_ReserveNowReservesConnector,
    A8_ChangeAvailabilityToUnavailable,
    A9_FaultDetected,
    B1_IntendedUsageIsEnded,
    B3_PrerequisitesForChargingMetAndChargingStarts,
    B4_PrerequisitesForChargingMetEVDoesNotStartCharging,
    B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging,
    B6_TimedOut, // Usage was initiated but ID Tag was not presented within timeout
    B9_FaultDetected,
    C1_ChargingSessionEndsNoUserActionRequired, // e.g. fixed cable removed on EV side
    C4_ChargingStopsUponEVRequest,
    C5_ChargingStopsUponEVSERequest,
    C6_TransactionStoppedAndUserActionRequired, // e.g. remove cable leave parking bay
    C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    C9_FaultDetected,
    D1_ChargingSessionEndsNoUserActionRequired,
    D3_ChargingResumesUponEVRequest,
    D5_ChargingSuspendedByEVSE,
    D6_TransactionStoppedNoUserActionRequired,
    D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    D9_FaultDetected,
    E1_ChargingSessionEndsNoUserActionRequired,
    E3_ChargingResumesEVSERestrictionLifted,
    E4_EVSERestrictionLiftedEVDoesNotStartCharging,
    E6_TransactionStoppedAndUserActionRequired,
    E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    E9_FaultDetected,
    F1_AllUserActionsCompleted,
    F2_UsersRestartChargingSession, // cable reconnect, ID Tag presented again -> creates new transaction
    F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable,
    F9_FaultDetected,
    G1_ReservationExpiresOrCancelReservationReceived,
    G2_ReservationIdentityPresented,
    G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable,
    G9_FaultDetected,
    H1_ConnectorSetAvailableByChangeAvailability,
    H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint,
    H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging,
    H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging,
    H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging,
    H9_FaultDetected,
    I1_ReturnToAvailable,
    I2_ReturnToPreparing,
    I3_ReturnToCharging,
    I4_ReturnToSuspendedEV,
    I5_ReturnToSuspendedEVSE,
    I6_ReturnToFinishing,
    I7_ReturnToReserved,
    I8_ReturnToUnavailable,
};

using EventTypeFactory = fsm::utils::IdentifiableTypeFactory<ChargePointStatusTransition>;

using Event_A2_UsageInitiated = EventTypeFactory::Derived<ChargePointStatusTransition::A2_UsageInitiated>;
using Event_A3_UsageInitiatedWithoutAuthorization =
    EventTypeFactory::Derived<ChargePointStatusTransition::A3_UsageInitiatedWithoutAuthorization>;
using Event_A4_UsageInitiatedEVDoesNotStartCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::A4_UsageInitiatedEVDoesNotStartCharging>;
using Event_A5_UsageInitiatedEVSEDoesNotAllowCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::A5_UsageInitiatedEVSEDoesNotAllowCharging>;
using Event_A7_ReserveNowReservesConnector =
    EventTypeFactory::Derived<ChargePointStatusTransition::A7_ReserveNowReservesConnector>;
using Event_A8_ChangeAvailabilityToUnavailable =
    EventTypeFactory::Derived<ChargePointStatusTransition::A8_ChangeAvailabilityToUnavailable>;
using Event_A9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::A9_FaultDetected>;
using Event_B1_IntendedUsageIsEnded = EventTypeFactory::Derived<ChargePointStatusTransition::B1_IntendedUsageIsEnded>;
using Event_B3_PrerequisitesForChargingMetAndChargingStarts =
    EventTypeFactory::Derived<ChargePointStatusTransition::B3_PrerequisitesForChargingMetAndChargingStarts>;
using Event_B4_PrerequisitesForChargingMetEVDoesNotStartCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::B4_PrerequisitesForChargingMetEVDoesNotStartCharging>;
using Event_B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging>;
using Event_B6_TimedOut = EventTypeFactory::Derived<ChargePointStatusTransition::B6_TimedOut>;
using Event_B9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::B9_FaultDetected>;
using Event_C1_ChargingSessionEndsNoUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::C1_ChargingSessionEndsNoUserActionRequired>;
using Event_C4_ChargingStopsUponEVRequest =
    EventTypeFactory::Derived<ChargePointStatusTransition::C4_ChargingStopsUponEVRequest>;
using Event_C5_ChargingStopsUponEVSERequest =
    EventTypeFactory::Derived<ChargePointStatusTransition::C5_ChargingStopsUponEVSERequest>;
using Event_C6_TransactionStoppedAndUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::C6_TransactionStoppedAndUserActionRequired>;
using Event_C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable = EventTypeFactory::Derived<
    ChargePointStatusTransition::C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable>;
using Event_C9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::C9_FaultDetected>;
using Event_D1_ChargingSessionEndsNoUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::D1_ChargingSessionEndsNoUserActionRequired>;
using Event_D3_ChargingResumesUponEVRequest =
    EventTypeFactory::Derived<ChargePointStatusTransition::D3_ChargingResumesUponEVRequest>;
using Event_D5_ChargingSuspendedByEVSE =
    EventTypeFactory::Derived<ChargePointStatusTransition::D5_ChargingSuspendedByEVSE>;
using Event_D6_TransactionStoppedNoUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::D6_TransactionStoppedNoUserActionRequired>;
using Event_D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable = EventTypeFactory::Derived<
    ChargePointStatusTransition::D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable>;
using Event_D9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::D9_FaultDetected>;
using Event_E1_ChargingSessionEndsNoUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::E1_ChargingSessionEndsNoUserActionRequired>;
using Event_E3_ChargingResumesEVSERestrictionLifted =
    EventTypeFactory::Derived<ChargePointStatusTransition::E3_ChargingResumesEVSERestrictionLifted>;
using Event_E4_EVSERestrictionLiftedEVDoesNotStartCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::E4_EVSERestrictionLiftedEVDoesNotStartCharging>;
using Event_E6_TransactionStoppedAndUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::E6_TransactionStoppedAndUserActionRequired>;
using Event_E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable = EventTypeFactory::Derived<
    ChargePointStatusTransition::E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable>;
using Event_E9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::E9_FaultDetected>;
using Event_F1_AllUserActionsCompleted =
    EventTypeFactory::Derived<ChargePointStatusTransition::F1_AllUserActionsCompleted>;
using Event_F2_UsersRestartChargingSession =
    EventTypeFactory::Derived<ChargePointStatusTransition::F2_UsersRestartChargingSession>;
using Event_F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable = EventTypeFactory::Derived<
    ChargePointStatusTransition::F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable>;
using Event_F9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::F9_FaultDetected>;
using Event_G1_ReservationExpiresOrCancelReservationReceived =
    EventTypeFactory::Derived<ChargePointStatusTransition::G1_ReservationExpiresOrCancelReservationReceived>;
using Event_G2_ReservationIdentityPresented =
    EventTypeFactory::Derived<ChargePointStatusTransition::G2_ReservationIdentityPresented>;
using Event_G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable =
    EventTypeFactory::Derived<
        ChargePointStatusTransition::
            G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable>;
using Event_G9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::G9_FaultDetected>;
using Event_H1_ConnectorSetAvailableByChangeAvailability =
    EventTypeFactory::Derived<ChargePointStatusTransition::H1_ConnectorSetAvailableByChangeAvailability>;
using Event_H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint =
    EventTypeFactory::Derived<ChargePointStatusTransition::H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint>;
using Event_H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging>;
using Event_H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging = EventTypeFactory::Derived<
    ChargePointStatusTransition::H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging>;
using Event_H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging = EventTypeFactory::Derived<
    ChargePointStatusTransition::H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging>;
using Event_H9_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::H9_FaultDetected>;
using Event_I1_ReturnToAvailable = EventTypeFactory::Derived<ChargePointStatusTransition::I1_ReturnToAvailable>;
using Event_I2_ReturnToPreparing = EventTypeFactory::Derived<ChargePointStatusTransition::I2_ReturnToPreparing>;
using Event_I3_ReturnToCharging = EventTypeFactory::Derived<ChargePointStatusTransition::I3_ReturnToCharging>;
using Event_I4_ReturnToSuspendedEV = EventTypeFactory::Derived<ChargePointStatusTransition::I4_ReturnToSuspendedEV>;
using Event_I5_ReturnToSuspendedEVSE = EventTypeFactory::Derived<ChargePointStatusTransition::I5_ReturnToSuspendedEVSE>;
using Event_I6_ReturnToFinishing = EventTypeFactory::Derived<ChargePointStatusTransition::I6_ReturnToFinishing>;
using Event_I7_ReturnToReserved = EventTypeFactory::Derived<ChargePointStatusTransition::I7_ReturnToReserved>;
using Event_I8_ReturnToUnavailable = EventTypeFactory::Derived<ChargePointStatusTransition::I8_ReturnToUnavailable>;

using EventBufferType = std::aligned_union_t<0, Event_A2_UsageInitiated>;
using EventBaseType = EventTypeFactory::Base;

enum class State
{
    Available,
    Preparing,
    Charging,
    SuspendedEV,
    SuspendedEVSE,
    Finishing,
    Reserved,
    Unavailable,
    Faulted,
};

struct StateIdType {
    const State id;
    const std::string name;
};

struct ChargePointStateMachine {
    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::async::StateHandle<EventInfoType, StateIdType>;
    using TransitionType = StateHandleType::TransitionWrapperType;
    using FSMContextType = StateHandleType::FSMContextType;

    // define state descriptors
    StateHandleType sd_available{{State::Available, "Available"}};
    StateHandleType sd_preparing{{State::Preparing, "Preparing"}};
    StateHandleType sd_charging{{State::Charging, "Charging"}};
    StateHandleType sd_suspended_ev{{State::SuspendedEV, "SuspendedEV"}};
    StateHandleType sd_suspended_evse{{State::SuspendedEVSE, "SuspendedEVSE"}};
    StateHandleType sd_finishing{{State::Finishing, "Finishing"}};
    StateHandleType sd_reserved{{State::Reserved, "Reserved"}};
    StateHandleType sd_unavailable{{State::Unavailable, "Unavailable"}};
    StateHandleType sd_faulted{{State::Faulted, "Faulted"}};

    std::function<void(ChargePointStatus status)> status_notification_callback;

    explicit ChargePointStateMachine(const std::function<void(ChargePointStatus status)>& status_notification_callback);
};

using ChargePointStateMachineController = fsm::async::PThreadController<ChargePointStateMachine::StateHandleType>;

struct ChargePointStateMachineWithController {
    std::shared_ptr<ChargePointStateMachine> state_machine;
    std::shared_ptr<ChargePointStateMachineController> controller;

    ChargePointStateMachineWithController(std::shared_ptr<ChargePointStateMachine> state_machine,
                                          std::shared_ptr<ChargePointStateMachineController> controller) {
        this->state_machine = state_machine;
        this->controller = controller;
    }
};

class ChargePointStates {
private:
    std::vector<std::shared_ptr<ChargePointStateMachineWithController>> state_machines;
    std::mutex state_machines_mutex;
    std::function<void(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status)>
        status_notification_callback;

public:
    ChargePointStates(int32_t number_of_connectors,
                      const std::function<void(int32_t connector, ChargePointErrorCode errorCode,
                                               ChargePointStatus status)>& status_notification_callback);
    void run(std::map<int32_t, ocpp1_6::AvailabilityType> connector_availability);

    void submit_event(int32_t connector, EventBaseType event);
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHARGE_POINT_STATE_MACHINE_HPP
