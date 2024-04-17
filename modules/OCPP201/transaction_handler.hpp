// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

#include <ocpp/v201/ocpp_types.hpp>

namespace module {

/// \brief TxStartStopPoint of OCPP2.0.1
enum class TxStartStopPoint {
    ParkingBayOccupancy,
    EVConnected,
    Authorized,
    PowerPathClosed,
    EnergyTransfer,
    DataSigned
};

/// \brief TxEvents that influence if conditions for TxStartStopPoints are fullfilled
enum class TxEvent {
    NONE,
    EV_CONNECTED,
    EV_DISCONNECTED,
    AUTHORIZED,
    DEAUTHORIZED,
    PARKING_BAY_OCCUPIED,
    PARKING_BAY_UNOCCUPIED,
    ENERGY_TRANSFER_STARTED,
    ENERGY_TRANSFER_STOPPED,
    SIGNED_START_DATA_RECEIVED
};

/// \brief Effect to an TxEvent. An effect can either trigger the start of a Transaction, the stop of a transaction or
/// it can have no effect
enum class TxEventEffect {
    START_TRANSACTION,
    STOP_TRANSACTION,
    NONE
};

/// \brief Struct that contains boolean conditions from which a TxEventEffect can be derived
struct TxStartStopConditions {
    bool is_authorized = false;
    bool is_ev_connected = false;
    bool is_parking_bay_occupied = false;
    bool is_energy_transfered = false;
    bool is_signed_data_received = false;

    void submit_event(const TxEvent tx_event) {

        switch (tx_event) {
        case TxEvent::EV_CONNECTED:
            is_ev_connected = true;
            break;
        case TxEvent::EV_DISCONNECTED:
            is_ev_connected = false;
            is_energy_transfered = false;
            break;
        case TxEvent::AUTHORIZED:
            is_authorized = true;
            break;
        case TxEvent::DEAUTHORIZED:
            is_authorized = false;
            break;
        case TxEvent::PARKING_BAY_OCCUPIED:
            is_parking_bay_occupied = true;
            break;
        case TxEvent::PARKING_BAY_UNOCCUPIED:
            is_parking_bay_occupied = false;
            break;
        case TxEvent::ENERGY_TRANSFER_STARTED:
            is_energy_transfered = true;
            break;
        case TxEvent::ENERGY_TRANSFER_STOPPED:
            is_energy_transfered = false;
            break;
        case TxEvent::SIGNED_START_DATA_RECEIVED:
            is_signed_data_received = true;
            break;
        }
    };

    bool is_start_condition_fullfilled(const TxStartStopPoint tx_start_point) {
        switch (tx_start_point) {
        case TxStartStopPoint::ParkingBayOccupancy:
            return is_parking_bay_occupied;
        case TxStartStopPoint::EVConnected:
            return is_ev_connected;
        case TxStartStopPoint::Authorized:
            return is_authorized;
        case TxStartStopPoint::PowerPathClosed:
            return is_authorized and is_ev_connected;
        case TxStartStopPoint::EnergyTransfer:
            return is_energy_transfered;
        case TxStartStopPoint::DataSigned:
            return is_signed_data_received;
        default:
            return false;
        }
    };

    bool is_stop_condition_fullfilled(const TxStartStopPoint tx_stop_point) {
        switch (tx_stop_point) {
        case TxStartStopPoint::ParkingBayOccupancy:
            return !is_parking_bay_occupied;
        case TxStartStopPoint::EVConnected:
            return !is_ev_connected;
        case TxStartStopPoint::Authorized:
            return !is_authorized;
        case TxStartStopPoint::PowerPathClosed:
            return !is_authorized or !is_ev_connected;
        case TxStartStopPoint::EnergyTransfer:
            return !is_energy_transfered;
        default:
            return false;
        }
    };
};

/// \brief Contains information that is required for a TransactionEvent (Started / Stopped) message in OCPP2.0.1.
struct TransactionData {
    bool started = false;
    int32_t connector_id;
    std::string session_id;
    ocpp::DateTime timestamp;
    ocpp::v201::TriggerReasonEnum trigger_reason;
    ocpp::v201::MeterValue meter_value;
    ocpp::v201::ChargingStateEnum charging_state;
    ocpp::v201::ReasonEnum stop_reason = ocpp::v201::ReasonEnum::Other;
    std::optional<ocpp::v201::IdToken> id_token;
    std::optional<ocpp::v201::IdToken> group_id_token;
    std::optional<int32_t> reservation_id;
    std::optional<int32_t> remote_start_id;

    TransactionData(const int32_t connector_id, const std::string& session_id, const ocpp::DateTime timestamp,
                    const ocpp::v201::TriggerReasonEnum trigger_reason,
                    const ocpp::v201::ChargingStateEnum charging_state) :
        connector_id(connector_id),
        session_id(session_id),
        timestamp(timestamp),
        trigger_reason(trigger_reason),
        charging_state(charging_state){};
};

/// \brief OCPP2.0.1 defines configurable TxStartPoints and TxStopPoints. This class handles TxEvents to determine when
/// OCPP2.0.1 transactions shall start and stop.
class TransactionHandler {
public:
    TransactionHandler(const int32_t nr_of_evses, const std::set<TxStartStopPoint>& tx_start_points,
                       const std::set<TxStartStopPoint>& tx_stop_points);

    /// \brief Submits the given \p tx_event at the given \p evse_id . Based on the configured \ref tx_start_points and
    /// \ref tx_stop_points, this function determines if a Transaction shall start or stop. The effect of the event is
    /// returned
    /// \param evse_id
    /// \param tx_event
    /// \return
    TxEventEffect submit_event(const int32_t evse_id, const TxEvent tx_event);

    /// \brief Sets the given \p tx_start_points
    void set_tx_start_points(const std::set<TxStartStopPoint>& tx_start_points);

    /// \brief Sets the given \p tx_stop_points
    void set_tx_stop_points(const std::set<TxStartStopPoint>& tx_stop_points);

    /// \brief Adds \p transaction_data to \ref evse_id_transaction_data_map for the given \p evse_id
    /// \param evse_id
    /// \param transaction_data
    void add_transaction_data(const int32_t evse_id, const std::shared_ptr<TransactionData>& transaction_data);

    /// \brief Gets transaction_data for the given \p evse_id from the \ref evse_id_transaction_data_map
    std::shared_ptr<TransactionData> get_transaction_data(const int32_t evse_id);

    /// \brief Resets transaction_data for the given \p evse_id in the \ref evse_id_transaction_data_map. It efectively
    /// sets the value for the \p evse_id to nullptr
    void reset_transaction_data(const int32_t evse_id);

private:
    std::set<TxStartStopPoint> tx_start_points;
    std::set<TxStartStopPoint> tx_stop_points;

    // map that holds information about states that are relevant in order to determine start and stop of OCPP2.0.1
    // transactions
    std::map<int32_t, TxStartStopConditions> tx_start_stop_conditions;

    // map that holds transaction_data for each evse_id . The transaction_data shall be updated based on the present
    // information available on the charge point
    std::map<int32_t, std::shared_ptr<TransactionData>> evse_id_transaction_data_map;

    /// \brief Determines if a transaction shall start for the given \p evse_id based on the present \ref
    /// tx_start_stop_conditions for this \p evse_id
    bool should_transaction_start(const int32_t evse_id);

    /// \brief Determines if a transaction shall stop for the given \p evse_id based on the present \ref
    /// tx_start_stop_conditions for this \p evse_id
    bool should_transaction_stop(const int32_t evse_id);
};

} // namespace module
