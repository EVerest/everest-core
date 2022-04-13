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

enum class ChargePointStatusTransition {
    BecomeAvailable,
    UsageInitiated,
    StartCharging,
    PauseChargingEV,
    PauseChargingEVSE,
    ReserveConnector,
    TransactionStoppedAndUserActionRequired,
    ChangeAvailabilityToUnavailable,
    FaultDetected,
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

using Event_UsageInitiated = EventTypeFactory::Derived<ChargePointStatusTransition::UsageInitiated>;
using Event_StartCharging =
    EventTypeFactory::Derived<ChargePointStatusTransition::StartCharging>;
using Event_PauseChargingEV =
    EventTypeFactory::Derived<ChargePointStatusTransition::PauseChargingEV>;
using Event_PauseChargingEVSE =
    EventTypeFactory::Derived<ChargePointStatusTransition::PauseChargingEVSE>;
using Event_ReserveConnector =
    EventTypeFactory::Derived<ChargePointStatusTransition::ReserveConnector>;
using Event_ChangeAvailabilityToUnavailable =
    EventTypeFactory::Derived<ChargePointStatusTransition::ChangeAvailabilityToUnavailable>;
using Event_FaultDetected = EventTypeFactory::Derived<ChargePointStatusTransition::FaultDetected>;
using Event_BecomeAvailable = EventTypeFactory::Derived<ChargePointStatusTransition::BecomeAvailable>;
using Event_TransactionStoppedAndUserActionRequired =
    EventTypeFactory::Derived<ChargePointStatusTransition::TransactionStoppedAndUserActionRequired>;
using Event_I1_ReturnToAvailable = EventTypeFactory::Derived<ChargePointStatusTransition::I1_ReturnToAvailable>;
using Event_I2_ReturnToPreparing = EventTypeFactory::Derived<ChargePointStatusTransition::I2_ReturnToPreparing>;
using Event_I3_ReturnToCharging = EventTypeFactory::Derived<ChargePointStatusTransition::I3_ReturnToCharging>;
using Event_I4_ReturnToSuspendedEV = EventTypeFactory::Derived<ChargePointStatusTransition::I4_ReturnToSuspendedEV>;
using Event_I5_ReturnToSuspendedEVSE = EventTypeFactory::Derived<ChargePointStatusTransition::I5_ReturnToSuspendedEVSE>;
using Event_I6_ReturnToFinishing = EventTypeFactory::Derived<ChargePointStatusTransition::I6_ReturnToFinishing>;
using Event_I7_ReturnToReserved = EventTypeFactory::Derived<ChargePointStatusTransition::I7_ReturnToReserved>;
using Event_I8_ReturnToUnavailable = EventTypeFactory::Derived<ChargePointStatusTransition::I8_ReturnToUnavailable>;

using EventBufferType = std::aligned_union_t<0, Event_UsageInitiated>;
using EventBaseType = EventTypeFactory::Base;

enum class State {
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

    // track current state
    ChargePointStatus state;

    std::function<void(ChargePointStatus status)> status_notification_callback;

    explicit ChargePointStateMachine(const std::function<void(ChargePointStatus status)>& status_notification_callback);

    ChargePointStatus get_state();
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

    ChargePointStatus get_state(int32_t connector);
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHARGE_POINT_STATE_MACHINE_HPP
