// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_STATE_MACHINE_HPP
#define OCPP_V16_CHARGE_POINT_STATE_MACHINE_HPP

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <ocpp/v16/enums.hpp>

namespace ocpp {
namespace v16 {

enum class FSMEvent {
    BecomeAvailable,
    UsageInitiated,
    StartCharging,
    PauseChargingEV,
    PauseChargingEVSE,
    ReserveConnector,
    TransactionStoppedAndUserActionRequired,
    ChangeAvailabilityToUnavailable,
    // FaultDetected - note: this event is handled via a separate function
    I1_ReturnToAvailable,
    I2_ReturnToPreparing,
    I3_ReturnToCharging,
    I4_ReturnToSuspendedEV,
    I5_ReturnToSuspendedEVSE,
    I6_ReturnToFinishing,
    I7_ReturnToReserved,
    I8_ReturnToUnavailable,
};

using FSMState = ChargePointStatus;

using FSMStateTransitions = std::map<FSMEvent, FSMState>;

using FSMDefinition = std::map<FSMState, FSMStateTransitions>;

class ChargePointFSM {
public:
    using StatusNotificationCallback = std::function<void(ChargePointStatus status, ChargePointErrorCode error_code)>;
    explicit ChargePointFSM(const StatusNotificationCallback& status_notification_callback, FSMState initial_state);

    bool handle_event(FSMEvent event);
    bool handle_error(const ChargePointErrorCode& error_code);
    bool handle_fault(const ChargePointErrorCode& error_code);

    FSMState get_state() const;

private:
    StatusNotificationCallback status_notification_callback;
    // track current state

    bool faulted;
    ChargePointErrorCode error_code;
    FSMState state;
};

class ChargePointStates {
public:
    using ConnectorStatusCallback =
        std::function<void(int connector_id, ChargePointErrorCode errorCode, ChargePointStatus status)>;
    ChargePointStates(const ConnectorStatusCallback& connector_status_callback);
    void reset(std::map<int, ChargePointStatus> connector_status_map);

    void submit_event(int connector_id, FSMEvent event);
    void submit_fault(int connector_id, const ChargePointErrorCode& error_code);
    void submit_error(int connector_id, const ChargePointErrorCode& error_code);

    ChargePointStatus get_state(int connector_id);

private:
    ConnectorStatusCallback connector_status_callback;

    std::unique_ptr<ChargePointFSM> state_machine_connector_zero;
    std::vector<ChargePointFSM> state_machines;
    std::mutex state_machines_mutex;
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHARGE_POINT_STATE_MACHINE_HPP
