// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <functional>
#include <map>
#include <memory>

#include <everest/timer.hpp>

#include <ocpp/v201/connector.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/transaction.hpp>

namespace ocpp {
namespace v201 {

/// \brief Represents an EVSE. An EVSE can contain multiple Connector objects, but can only supply energy to one of
/// them.
class Evse {

private:
    int32_t evse_id;
    std::map<int32_t, std::unique_ptr<Connector>> id_connector_map;
    std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)> status_notification_callback;
    std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                       const std::optional<int32_t> reservation_id)>
        transaction_meter_value_req;
    std::unique_ptr<EnhancedTransaction> transaction; // pointer to active transaction (can be nullptr)
    MeterValue meter_value;                           // represents current meter value
    std::mutex meter_value_mutex;
    Everest::SteadyTimer sampled_meter_values_timer;

public:
    /// \brief Construct a new Evse object
    /// \param evse_id id of the evse
    /// \param number_of_connectors of the evse
    /// \param status_notification_callback that is called when the status of a connector changes
    Evse(const int32_t evse_id, const int32_t number_of_connectors,
         const std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)>&
             status_notification_callback,
         const std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                                  const std::optional<int32_t> reservation_id)>& transaction_meter_value_req);

    /// \brief Returns an OCPP2.0.1 EVSE type
    /// \return
    EVSE get_evse_info();

    /// \brief Returns the number of connectors of this EVSE
    /// \return 
    int32_t get_number_of_connectors();

    /// \brief Returns a reference to the sampled meter values timer
    /// \return
    Everest::SteadyTimer& get_sampled_meter_values_timer();

    /// \brief Opens a new transaction
    /// \param transaction_id id of the transaction
    /// \param connector_id id of the connector
    /// \param timestamp timestamp of the start of the transaction
    /// \param meter_start start meter value of the transaction
    /// \param id_token id_token with which the transaction was authorized / started
    /// \param group_id_token optional group id_token
    /// \param reservation optional reservation_id if evse was reserved
    /// \param sampled_data_tx_updated_interval Interval between sampling of metering (or other) data, intended to be
    /// transmitted via TransactionEventRequest (eventType = Updated) messages
    void open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                          const MeterValue& meter_start, const IdToken& id_token, const std::optional<IdToken> &group_id_token,
                          const std::optional<int32_t> reservation_id,
                          const int32_t sampled_data_tx_updated_interval);

    /// \brief Closes the transaction on this evse by adding the given \p timestamp \p meter_stop and \p reason .
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    void close_transaction(const DateTime& timestamp, const MeterValue& meter_stop, const ReasonEnum& reason);

    /// \brief Indicates if a transaction is active at this evse
    /// \return
    bool has_active_transaction();

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

    /// \brief Event handler that should be called when a new meter_value for this evse is present
    /// \param meter_value
    void on_meter_value(const MeterValue& meter_value);

    /// \brief Returns the last present meter value for this evse
    /// \return
    MeterValue get_meter_value();
};

} // namespace v201
} // namespace ocpp
