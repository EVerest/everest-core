// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v201/message_handler.hpp>

#include <ocpp/v201/message_dispatcher.hpp>

namespace ocpp::v201 {
class ConnectivityManagerInterface;
class DeviceModel;
class EvseManagerInterface;
class ComponentStateManagerInterface;
class EvseInterface;

class TransactionInterface;
class SmartChargingInterface;
class MeterValuesInterface;
class AvailabilityInterface;
class FirmwareUpdateInterface;
class SecurityInterface;
class ReservationInterface;
class ProvisioningInterface;

struct UnlockConnectorRequest;
struct RequestStartTransactionRequest;
struct RequestStopTransactionRequest;
struct TriggerMessageRequest;
struct UnlockConnectorResponse;

typedef std::function<UnlockConnectorResponse(const int32_t evse_id, const int32_t connecor_id)>
    UnlockConnectorCallback;
typedef std::function<RequestStartStopStatusEnum(const RequestStartTransactionRequest& request,
                                                 const bool authorize_remote_start)>
    RemoteStartTransactionCallback;
typedef std::function<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
    StopTransactionCallback;

class RemoteTransactionControlInterface : public MessageHandlerInterface {
public:
    virtual ~RemoteTransactionControlInterface() = default;
};

class RemoteTransactionControl : public RemoteTransactionControlInterface {
public:
    RemoteTransactionControl(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                             ConnectivityManagerInterface& connectivity_manager, EvseManagerInterface& evse_manager,
                             ComponentStateManagerInterface& component_state_manager, TransactionInterface& transaction,
                             SmartChargingInterface& smart_charging, MeterValuesInterface& meter_values,
                             AvailabilityInterface& availability, FirmwareUpdateInterface& firmware_update,
                             SecurityInterface& security, ReservationInterface* reservation,
                             ProvisioningInterface& provisioning, UnlockConnectorCallback unlock_connector_callback,
                             RemoteStartTransactionCallback remote_start_transaction_callback,
                             StopTransactionCallback stop_transaction_callback,
                             std::atomic<RegistrationStatusEnum>& registration_status,
                             std::atomic<UploadLogStatusEnum>& upload_log_status,
                             std::atomic<int32_t>& upload_log_status_id);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    ConnectivityManagerInterface& connectivity_manager;
    EvseManagerInterface& evse_manager;
    ComponentStateManagerInterface& component_state_manager;

    TransactionInterface& transaction;
    SmartChargingInterface& smart_charging;
    MeterValuesInterface& meter_values;
    AvailabilityInterface& availability;
    FirmwareUpdateInterface& firmware_update;
    SecurityInterface& security;
    ReservationInterface* reservation;
    ProvisioningInterface& provisioning;

    UnlockConnectorCallback unlock_connector_callback;
    RemoteStartTransactionCallback remote_start_transaction_callback;
    StopTransactionCallback stop_transaction_callback;

    std::atomic<RegistrationStatusEnum>& registration_status;
    std::atomic<UploadLogStatusEnum>& upload_log_status;
    std::atomic<int32_t>& upload_log_status_id;

private: // Functions
    /* OCPP message handlers */

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);
    void handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call);
    void handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call);
    void handle_trigger_message(Call<TriggerMessageRequest> call);

    // Helper functions
    ///
    /// \brief Check if one of the connectors of the evse is available (both connectors faulted or unavailable or on of
    ///        the connectors occupied).
    /// \param evse Evse to check.
    /// \return True if at least one connector is not faulted or unavailable.
    ///
    bool is_evse_connector_available(EvseInterface& evse) const;

    ///
    /// \brief Check if EVSE connector is reserved for another than the given id token and / or group id token.
    /// \param evse             The evse id that must be checked. Reservation will be checked for all connectors.
    /// \param id_token         The id token to check if it is reserved for that token.
    /// \param group_id_token   The group id token to check if it is reserved for that group id.
    /// \return The status of the reservation for this evse, id token and group id token.
    ///
    ReservationCheckStatus is_evse_reserved_for_other(EvseInterface& evse, const IdToken& id_token,
                                                      const std::optional<IdToken>& group_id_token) const;
};
} // namespace ocpp::v201
