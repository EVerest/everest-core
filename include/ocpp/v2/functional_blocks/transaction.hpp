// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/v2/message_dispatcher.hpp>

namespace ocpp::v2 {
class DeviceModel;
class AuthorizationInterface;
class ConnectivityManagerInterface;
class EvseManagerInterface;
class DatabaseHandlerInterface;
class AvailabilityInterface;
class SmartChargingInterface;
class TariffAndCostInterface;

struct GetTransactionStatusRequest;

typedef std::function<void(const TransactionEventRequest& transaction_event)> TransactionEventCallback;
typedef std::function<void(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)> ResetCallback;
typedef std::function<void(const TransactionEventRequest& transaction_event,
                           const TransactionEventResponse& transaction_event_response)>
    TransactionEventResponseCallback;
typedef std::function<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
    StopTransactionCallback;
typedef std::function<void(const int32_t evse_id)> PauseChargingCallback;

class TransactionInterface : public MessageHandlerInterface {
public:
    virtual ~TransactionInterface() = default;

    /// \brief Event handler that should be called when a transaction has started
    /// \param evse_id
    /// \param connector_id
    /// \param session_id
    /// \param timestamp
    /// \param trigger_reason
    /// \param meter_start
    /// \param id_token
    /// \param group_id_token   Optional group id token
    /// \param reservation_id
    /// \param remote_start_id
    /// \param charging_state   The new charging state
    virtual void
    on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                           const DateTime& timestamp, const TriggerReasonEnum trigger_reason,
                           const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                           const std::optional<IdToken>& group_id_token, const std::optional<int32_t>& reservation_id,
                           const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state) = 0;

    /// \brief Event handler that should be called when a transaction has finished
    /// \param evse_id
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    /// \param id_token
    /// \param signed_meter_value
    /// \param charging_state
    virtual void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                         const ReasonEnum reason, const TriggerReasonEnum trigger_reason,
                                         const std::optional<IdToken>& id_token,
                                         const std::optional<std::string>& signed_meter_value,
                                         const ChargingStateEnum charging_state) = 0;

    /* OCPP message requests */

    // Functional Block E: Transactions
    virtual void transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                                       const Transaction& transaction, const TriggerReasonEnum& trigger_reason,
                                       const int32_t seq_no, const std::optional<int32_t>& cable_max_current,
                                       const std::optional<EVSE>& evse, const std::optional<IdToken>& id_token,
                                       const std::optional<std::vector<MeterValue>>& meter_value,
                                       const std::optional<int32_t>& number_of_phases_used, const bool offline,
                                       const std::optional<int32_t>& reservation_id,
                                       const bool initiated_by_trigger_message = false) = 0;

    virtual void set_remote_start_id_for_evse(const int32_t evse_id, const IdToken id_token,
                                              const int32_t remote_start_id) = 0;
    virtual void schedule_reset(const std::optional<int32_t> reset_scheduled_evseid) = 0;
};

class TransactionBlock : public TransactionInterface {
public:
    TransactionBlock(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                     ConnectivityManagerInterface& connectivity_manager, EvseManagerInterface& evse_manager,
                     MessageQueue<v2::MessageType>& message_queue, DatabaseHandlerInterface& database_handler,
                     AuthorizationInterface& authorization, AvailabilityInterface& availability,
                     SmartChargingInterface& smart_charging, TariffAndCostInterface& tariff_and_cost,
                     StopTransactionCallback stop_transaction_callback, PauseChargingCallback pause_charging_callback,
                     std::optional<TransactionEventCallback> transaction_event_callback,
                     std::optional<TransactionEventResponseCallback> transaction_event_response_callback,
                     ResetCallback reset_callback);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    void on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                                const DateTime& timestamp, const TriggerReasonEnum trigger_reason,
                                const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                                const std::optional<IdToken>& group_id_token,
                                const std::optional<int32_t>& reservation_id,
                                const std::optional<int32_t>& remote_start_id,
                                const ChargingStateEnum charging_state) override;
    void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                 const ReasonEnum reason, const TriggerReasonEnum trigger_reason,
                                 const std::optional<IdToken>& id_token,
                                 const std::optional<std::string>& signed_meter_value,
                                 const ChargingStateEnum charging_state) override;
    void transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                               const Transaction& transaction, const TriggerReasonEnum& trigger_reason,
                               const int32_t seq_no, const std::optional<int32_t>& cable_max_current,
                               const std::optional<EVSE>& evse, const std::optional<IdToken>& id_token,
                               const std::optional<std::vector<MeterValue>>& meter_value,
                               const std::optional<int32_t>& number_of_phases_used, const bool offline,
                               const std::optional<int32_t>& reservation_id,
                               const bool initiated_by_trigger_message = false) override;
    void set_remote_start_id_for_evse(const int32_t evse_id, const IdToken id_token,
                                      const int32_t remote_start_id) override;
    void schedule_reset(const std::optional<int32_t> reset_scheduled_evseid) override;

private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    ConnectivityManagerInterface& connectivity_manager;
    EvseManagerInterface& evse_manager;
    MessageQueue<v2::MessageType>& message_queue;
    DatabaseHandlerInterface& database_handler;
    AuthorizationInterface& authorization;
    AvailabilityInterface& availability;
    SmartChargingInterface& smart_charging;
    TariffAndCostInterface& tariff_and_cost;
    StopTransactionCallback stop_transaction_callback;
    PauseChargingCallback pause_charging_callback;
    std::optional<TransactionEventCallback> transaction_event_callback;
    std::optional<TransactionEventResponseCallback> transaction_event_response_callback;
    ResetCallback reset_callback;

    std::map<int32_t, std::pair<IdToken, int32_t>> remote_start_id_per_evse;
    /// \brief Used when an 'OnIdle' reset is requested, to perform the reset after the charging has stopped.
    bool reset_scheduled;
    /// \brief If `reset_scheduled` is true and the reset is for a specific evse id, it will be stored in this member.
    std::set<int32_t> reset_scheduled_evseids;

private: // Functions
    /* OCPP message handlers */

    // Functional Block E: Transaction
    void handle_transaction_event_response(const EnhancedMessage<v2::MessageType>& message);
    void handle_get_transaction_status(const Call<GetTransactionStatusRequest> call);
};
} // namespace ocpp::v2
