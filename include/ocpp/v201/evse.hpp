// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <functional>
#include <map>
#include <memory>

#include <ocpp/v201/connector.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/transaction.hpp>

namespace ocpp {
namespace v201 {

/// \brief Represents an EVSE. An EVSE can contain multiple Connector objects, but can only supply energy to one of
/// them.
class Evse {

private:
    int32_t evse_id;
    DeviceModel& device_model;
    std::map<int32_t, std::unique_ptr<Connector>> id_connector_map;
    std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)> status_notification_callback;
    std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                       const std::optional<int32_t> reservation_id)>
        transaction_meter_value_req;
    std::function<void()> pause_charging_callback;
    std::unique_ptr<EnhancedTransaction> transaction; // pointer to active transaction (can be nullptr)
    MeterValue meter_value;                           // represents current meter value
    std::recursive_mutex meter_value_mutex;
    Everest::SteadyTimer sampled_meter_values_timer;
    std::shared_ptr<DatabaseHandler> database_handler;

    /// \brief gets the active import energy meter value from meter_value, normalized to Wh.
    std::optional<float> get_active_import_register_meter_value();

    /// \brief function to check if the max energy has been exceeded, calls pause_charging_callback if so.
    void check_max_energy_on_invalid_id();

public:
    /// \brief Construct a new Evse object
    /// \param evse_id id of the evse
    /// \param number_of_connectors of the evse
    /// \param device_model reference to the device model
    /// \param status_notification_callback that is called when the status of a connector changes
    /// \param pause_charging_callback that is called when the charging should be paused due to max energy on
    /// invalid id being exceeded
    Evse(const int32_t evse_id, const int32_t number_of_connectors, DeviceModel& device_model,
         std::shared_ptr<DatabaseHandler> database_handler,
         const std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)>&
             status_notification_callback,
         const std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                                  const std::optional<int32_t> reservation_id)>& transaction_meter_value_req,
         const std::function<void()> pause_charging_callback);

    /// \brief Returns an OCPP2.0.1 EVSE type
    /// \return
    EVSE get_evse_info();

    /// \brief Returns the number of connectors of this EVSE
    /// \return
    uint32_t get_number_of_connectors();

    /// \brief Opens a new transaction
    /// \param transaction_id id of the transaction
    /// \param connector_id id of the connector
    /// \param timestamp timestamp of the start of the transaction
    /// \param meter_start start meter value of the transaction
    /// \param id_token id_token with which the transaction was authorized / started
    /// \param group_id_token optional group id_token
    /// \param reservation optional reservation_id if evse was reserved
    /// \param sampled_data_tx_updated_interval Interval between sampling of metering (or other) data, intended to
    /// be transmitted via TransactionEventRequest (eventType = Updated) messages
    void open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                          const MeterValue& meter_start, const IdToken& id_token,
                          const std::optional<IdToken>& group_id_token, const std::optional<int32_t> reservation_id,
                          const std::chrono::seconds sampled_data_tx_updated_interval,
                          const std::chrono::seconds sampled_data_tx_ended_interval,
                          const std::chrono::seconds aligned_data_tx_updated_interval,
                          const std::chrono::seconds aligned_data_tx_ended_interval);

    /// \brief Closes the transaction on this evse by adding the given \p timestamp \p meter_stop and \p reason .
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    void close_transaction(const DateTime& timestamp, const MeterValue& meter_stop, const ReasonEnum& reason);

    /// \brief Start checking if the max energy on invalid id has exceeded.
    ///        Will call pause_charging_callback when that happens.
    void start_checking_max_energy_on_invalid_id();

    /// \brief Indicates if a transaction is active at this evse
    /// \return
    bool has_active_transaction();

    /// \brief Indicates if a transaction is active at this evse at the given \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \return
    bool has_active_transaction(const int32_t connector_id);

    /// \brief Releases the reference of the transaction on this evse
    void release_transaction();

    /// \brief Returns a pointer to the EnhancedTransaction of this evse
    /// \return pointer to transaction (nullptr if no transaction is active)
    std::unique_ptr<EnhancedTransaction>& get_transaction();

    /// \brief Get the state of the connector with the given \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \return ConnectorStatusEnum
    ConnectorStatusEnum get_state(const int32_t connector_id);

    /// \brief Submits the given \p event to the state machine controller of the connector with the given
    /// \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \param event
    void submit_event(const int32_t connector_id, ConnectorEvent event);

    /// \brief Triggers status notification callback for all connectors of the evse
    void trigger_status_notification_callbacks();

    /// \brief Triggers a status notification callback for connector_id of the evse
    /// \param connector_id id of the connector of the evse
    void trigger_status_notification_callback(const int32_t connector_id);

    /// \brief Event handler that should be called when a new meter_value for this evse is present
    /// \param meter_value
    void on_meter_value(const MeterValue& meter_value);

    /// \brief Returns the last present meter value for this evse
    /// \return
    MeterValue get_meter_value();
};

} // namespace v201
} // namespace ocpp
