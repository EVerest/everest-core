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
#include <ocpp/v16/ocpp_enums.hpp>

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

/// \brief Contains all relevant information to handle errros in OCPP1.6
struct ErrorInfo {
    std::string uuid;                // uuid
    ChargePointErrorCode error_code; /// defined by OCPP1.6
    bool is_fault; /// indicates if state should change to "Faulted", if not set, state will not change and error is
                   /// only informational
    std::optional<CiString<50>> info;              /// defined by OCPP1.6
    std::optional<CiString<255>> vendor_id;        /// defined by OCPP1.6
    std::optional<CiString<50>> vendor_error_code; /// defined by OCPP1.6
    DateTime timestamp;                            // timestamp

    ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault);
    ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
              const std::optional<std::string> info);
    ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
              const std::optional<std::string> info, const std::optional<std::string> vendor_id);
    ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
              const std::optional<std::string> info, const std::optional<std::string> vendor_id,
              const std::optional<std::string> vendor_error_code);
};

using FSMState = ChargePointStatus;

using FSMStateTransitions = std::map<FSMEvent, FSMState>;

using FSMDefinition = std::map<FSMState, FSMStateTransitions>;

class ChargePointFSM {
public:
    using StatusNotificationCallback = std::function<void(
        const ChargePointStatus status, const ChargePointErrorCode error_code, const ocpp::DateTime& timestamp,
        const std::optional<CiString<50>>& info, const std::optional<CiString<255>>& vendor_id,
        const std::optional<CiString<50>>& vendor_error_code)>;
    explicit ChargePointFSM(const StatusNotificationCallback& status_notification_callback, FSMState initial_state);

    bool handle_event(FSMEvent event, const ocpp::DateTime timestamp, const std::optional<CiString<50>>& info);
    bool handle_error(const ErrorInfo& error_info);
    bool handle_error_cleared(const std::string uuid);
    bool handle_all_errors_cleared();
    void trigger_status_notification();

    FSMState get_state();
    std::optional<ErrorInfo> get_latest_error();

private:
    StatusNotificationCallback status_notification_callback;
    // track current state

    FSMState state;
    std::unordered_map<std::string, ErrorInfo> active_errors;

    bool is_faulted();
};

class ChargePointStates {
public:
    using ConnectorStatusCallback = std::function<void(
        const int connector_id, const ChargePointErrorCode errorCode, const ChargePointStatus status,
        const ocpp::DateTime& timestamp, const std::optional<CiString<50>>& info,
        const std::optional<CiString<255>>& vendor_id, const std::optional<CiString<50>>& vendor_error_code)>;
    ChargePointStates(const ConnectorStatusCallback& connector_status_callback);
    void reset(std::map<int, ChargePointStatus> connector_status_map);

    void submit_event(const int connector_id, FSMEvent event, const ocpp::DateTime& timestamp,
                      const std::optional<CiString<50>>& info = std::nullopt);
    void submit_error(const int connector_id, const ErrorInfo& error_info);
    void submit_error_cleared(const int connector_id, const std::string uuid);
    void submit_all_errors_cleared(const int32_t connector_id);
    void trigger_status_notification(const int connector_id);
    void trigger_status_notifications();

    ChargePointStatus get_state(int connector_id);
    std::optional<ErrorInfo> get_latest_error(int connector_id);

private:
    ConnectorStatusCallback connector_status_callback;

    std::unique_ptr<ChargePointFSM> state_machine_connector_zero;
    std::vector<ChargePointFSM> state_machines;
    std::mutex state_machines_mutex;
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHARGE_POINT_STATE_MACHINE_HPP
