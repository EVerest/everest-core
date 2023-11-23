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

#include <ocpp/common/types.hpp>
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
    using StatusNotificationCallback = std::function<void(
        const ChargePointStatus status, const ChargePointErrorCode error_code, const ocpp::DateTime& timestamp)>;
    explicit ChargePointFSM(const StatusNotificationCallback& status_notification_callback, FSMState initial_state);

    bool handle_event(FSMEvent event, const ocpp::DateTime timestamp);
    bool handle_error(const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp);
    bool handle_fault(const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp);

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
        std::function<void(const int connector_id, const ChargePointErrorCode errorCode, const ChargePointStatus status,
                           const ocpp::DateTime& timestamp)>;
    ChargePointStates(const ConnectorStatusCallback& connector_status_callback);
    void reset(std::map<int, ChargePointStatus> connector_status_map);

    void submit_event(const int connector_id, FSMEvent event, const ocpp::DateTime& timestamp);
    void submit_fault(const int connector_id, const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp);
    void submit_error(const int connector_id, const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp);

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
