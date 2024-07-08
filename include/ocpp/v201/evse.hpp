// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <functional>
#include <map>
#include <memory>

#include <ocpp/v201/average_meter_values.hpp>
#include <ocpp/v201/component_state_manager.hpp>
#include <ocpp/v201/connector.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/transaction.hpp>

namespace ocpp {
namespace v201 {

enum class CurrentPhaseType {
    AC,
    DC,
    Unknown,
};

class EvseInterface {
public:
    virtual ~EvseInterface();

    /// \brief Return the evse_id of this EVSE
    /// \return
    virtual int32_t get_id() const = 0;

    /// \brief Returns the number of connectors of this EVSE
    /// \return
    virtual uint32_t get_number_of_connectors() const = 0;

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
    virtual void open_transaction(const std::string& transaction_id, const int32_t connector_id,
                                  const DateTime& timestamp, const MeterValue& meter_start,
                                  const std::optional<IdToken>& id_token, const std::optional<IdToken>& group_id_token,
                                  const std::optional<int32_t> reservation_id,
                                  const std::chrono::seconds sampled_data_tx_updated_interval,
                                  const std::chrono::seconds sampled_data_tx_ended_interval,
                                  const std::chrono::seconds aligned_data_tx_updated_interval,
                                  const std::chrono::seconds aligned_data_tx_ended_interval) = 0;

    /// \brief Closes the transaction on this evse by adding the given \p timestamp \p meter_stop and \p reason .
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    virtual void close_transaction(const DateTime& timestamp, const MeterValue& meter_stop,
                                   const ReasonEnum& reason) = 0;

    /// \brief Start checking if the max energy on invalid id has exceeded.
    ///        Will call pause_charging_callback when that happens.
    virtual void start_checking_max_energy_on_invalid_id() = 0;

    /// \brief Indicates if a transaction is active at this evse
    /// \return
    virtual bool has_active_transaction() const = 0;

    /// \brief Indicates if a transaction is active at this evse at the given \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \return
    virtual bool has_active_transaction(const int32_t connector_id) const = 0;

    /// \brief Releases the reference of the transaction on this evse
    virtual void release_transaction() = 0;

    /// \brief Returns a pointer to the EnhancedTransaction of this evse
    /// \return pointer to transaction (nullptr if no transaction is active)
    virtual std::unique_ptr<EnhancedTransaction>& get_transaction() = 0;

    /// \brief Submits the given \p event to the state machine controller of the connector with the given
    /// \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \param event
    virtual void submit_event(const int32_t connector_id, ConnectorEvent event) = 0;

    /// \brief Event handler that should be called when a new meter_value for this evse is present
    /// \param meter_value
    virtual void on_meter_value(const MeterValue& meter_value) = 0;

    /// \brief Returns the last present meter value for this evse
    /// \return
    virtual MeterValue get_meter_value() = 0;

    /// @brief Return the idle meter values for this evse
    /// \return MeterValue type
    virtual MeterValue get_idle_meter_value() = 0;

    /// @brief Clear the idle meter values for this evse
    virtual void clear_idle_meter_values() = 0;

    /// \brief Returns a pointer to the connector with ID \param connector_id in this EVSE.
    virtual Connector* get_connector(int32_t connector_id) = 0;

    /// \brief Gets the effective Operative/Inoperative status of this EVSE
    virtual OperationalStatusEnum get_effective_operational_status() = 0;

    /// \brief Switches the operative status of the EVSE
    /// \param new_status The operative status to switch to
    /// \param persist True the updated operative state should be persisted
    virtual void set_evse_operative_status(OperationalStatusEnum new_status, bool persist) = 0;

    /// \brief Switches the operative status of a connector within this EVSE
    /// \param connector_id The ID of the connector
    /// \param new_status The operative status to switch to
    /// \param persist True the updated operative state should be persisted
    virtual void set_connector_operative_status(int32_t connector_id, OperationalStatusEnum new_status,
                                                bool persist) = 0;

    /// \brief Restores the operative status of a connector within this EVSE to the persisted status and recomputes its
    /// effective status \param connector_id The ID of the connector
    virtual void restore_connector_operative_status(int32_t connector_id) = 0;

    /// \brief Returns the phase type for the EVSE based on its SupplyPhases. It can be AC, DC, or Unknown.
    virtual CurrentPhaseType get_current_phase_type() = 0;
};

/// \brief Represents an EVSE. An EVSE can contain multiple Connector objects, but can only supply energy to one of
/// them.
class Evse : public EvseInterface {

private:
    int32_t evse_id;
    DeviceModel& device_model;
    std::map<int32_t, std::unique_ptr<Connector>> id_connector_map;
    std::function<void(const MeterValue& meter_value, EnhancedTransaction& transaction)> transaction_meter_value_req;
    std::function<void(int32_t evse_id)> pause_charging_callback;
    std::unique_ptr<EnhancedTransaction> transaction; // pointer to active transaction (can be nullptr)
    MeterValue meter_value;                           // represents current meter value
    std::recursive_mutex meter_value_mutex;
    Everest::SteadyTimer sampled_meter_values_timer;
    std::shared_ptr<DatabaseHandler> database_handler;

    /// \brief gets the active import energy meter value from meter_value, normalized to Wh.
    std::optional<float> get_active_import_register_meter_value();

    /// \brief function to check if the max energy has been exceeded, calls pause_charging_callback if so.
    void check_max_energy_on_invalid_id();

    AverageMeterValues aligned_data_updated;
    AverageMeterValues aligned_data_tx_end;

    /// \brief Component responsible for maintaining and persisting the operational status of CS, EVSEs, and connectors.
    std::shared_ptr<ComponentStateManagerInterface> component_state_manager;

public:
    /// \brief Construct a new Evse object
    /// \param evse_id id of the evse
    /// \param number_of_connectors of the evse
    /// \param device_model reference to the device model
    /// \param database_handler shared_ptr to the database handler
    /// \param component_state_manager shared_ptr to the component state manager
    /// \param transaction_meter_value_req that is called to transmit a meter value request related to a transaction
    /// \param pause_charging_callback that is called when the charging should be paused due to max energy on
    /// invalid id being exceeded
    Evse(const int32_t evse_id, const int32_t number_of_connectors, DeviceModel& device_model,
         std::shared_ptr<DatabaseHandler> database_handler,
         std::shared_ptr<ComponentStateManagerInterface> component_state_manager,
         const std::function<void(const MeterValue& meter_value, EnhancedTransaction& transaction)>&
             transaction_meter_value_req,
         const std::function<void(int32_t evse_id)>& pause_charging_callback);

    int32_t get_id() const;

    uint32_t get_number_of_connectors() const;

    void open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                          const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                          const std::optional<IdToken>& group_id_token, const std::optional<int32_t> reservation_id,
                          const std::chrono::seconds sampled_data_tx_updated_interval,
                          const std::chrono::seconds sampled_data_tx_ended_interval,
                          const std::chrono::seconds aligned_data_tx_updated_interval,
                          const std::chrono::seconds aligned_data_tx_ended_interval);
    void close_transaction(const DateTime& timestamp, const MeterValue& meter_stop, const ReasonEnum& reason);

    void start_checking_max_energy_on_invalid_id();

    bool has_active_transaction() const;
    bool has_active_transaction(const int32_t connector_id) const;
    void release_transaction();
    std::unique_ptr<EnhancedTransaction>& get_transaction();

    void submit_event(const int32_t connector_id, ConnectorEvent event);

    void on_meter_value(const MeterValue& meter_value);
    MeterValue get_meter_value();

    MeterValue get_idle_meter_value();
    void clear_idle_meter_values();

    Connector* get_connector(int32_t connector_id);

    OperationalStatusEnum get_effective_operational_status();
    void set_evse_operative_status(OperationalStatusEnum new_status, bool persist);
    void set_connector_operative_status(int32_t connector_id, OperationalStatusEnum new_status, bool persist);
    void restore_connector_operative_status(int32_t connector_id);

    CurrentPhaseType get_current_phase_type();
};

} // namespace v201
} // namespace ocpp
