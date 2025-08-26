// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <transaction_handler.hpp>

namespace module {

TransactionHandler::TransactionHandler(const int32_t nr_of_evses, const std::set<TxStartStopPoint>& tx_start_points,
                                       const std::set<TxStartStopPoint>& tx_stop_points) :
    tx_start_points(tx_start_points), tx_stop_points(tx_stop_points) {

    if (tx_start_points.empty() or tx_stop_points.empty()) {
        throw std::runtime_error(
            "Cannot construct TransactionHandler with either TxStart or TxStop points being empty");
    }

    if (nr_of_evses <= 0) {
        throw std::runtime_error("Cannot construct TransactionHandler with nr_of_evses <= 0");
    }

    for (int32_t evse_id = 1; evse_id <= nr_of_evses; evse_id++) {
        tx_start_stop_conditions[evse_id] = TxStartStopConditions();
        evse_id_transaction_data_map[evse_id] = nullptr;
    }
}

void TransactionHandler::set_tx_start_points(const std::set<TxStartStopPoint>& tx_start_points) {
    this->tx_start_points = tx_start_points;
}

void TransactionHandler::set_tx_stop_points(const std::set<TxStartStopPoint>& tx_stop_points) {
    this->tx_stop_points = tx_stop_points;
}

TxEventEffect TransactionHandler::submit_event(const int32_t evse_id, const TxEvent tx_event) {
    this->tx_start_stop_conditions[evse_id].submit_event(tx_event);

    if (this->should_transaction_start(evse_id)) {
        return TxEventEffect::START_TRANSACTION;
    }

    if (this->should_transaction_stop(evse_id)) {
        return TxEventEffect::STOP_TRANSACTION;
    }

    return TxEventEffect::NONE;
}

void TransactionHandler::add_transaction_data(const int32_t evse_id,
                                              const std::shared_ptr<TransactionData>& transaction_data) {
    if (evse_id <= 0 or evse_id > this->tx_start_stop_conditions.size()) {
        throw std::out_of_range("Can not add transaction data for evse_id <= 0 or > nr_of_evses");
    }
    this->evse_id_transaction_data_map[evse_id] = transaction_data;
}

std::shared_ptr<TransactionData> TransactionHandler::get_transaction_data(const int32_t evse_id) {
    if (!this->evse_id_transaction_data_map.count(evse_id)) {
        throw std::out_of_range("Attempt to get transaction_data for invalid evse_id");
    }
    return this->evse_id_transaction_data_map[evse_id];
}

void TransactionHandler::reset_transaction_data(const int32_t evse_id) {
    if (!this->evse_id_transaction_data_map.count(evse_id)) {
        throw std::out_of_range("Attempt to reset transaction_data for invalid evse_id");
    }
    this->evse_id_transaction_data_map[evse_id].reset();
}

bool TransactionHandler::should_transaction_start(const int32_t evse_id) {
    if (!this->evse_id_transaction_data_map.count(evse_id)) {
        throw std::out_of_range("Can not retrieve if transaction should start for invalid evse_id");
    }

    // check if transaction data is present
    if (this->evse_id_transaction_data_map[evse_id] == nullptr) {
        return false;
    }

    // check if transaction has not yet been started (in OCPP)
    if (!this->evse_id_transaction_data_map[evse_id]->started) {
        for (const auto& tx_start_condition : this->tx_start_points) {
            if (this->tx_start_stop_conditions[evse_id].is_start_condition_fullfilled(tx_start_condition)) {
                return true;
            }
        }
    }
    return false;
}

bool TransactionHandler::should_transaction_stop(const int32_t evse_id) {
    if (!this->evse_id_transaction_data_map.count(evse_id)) {
        throw std::out_of_range("Can not retrieve if transaction should stop for invalid evse_id");
    }

    // check if transaction data is present
    if (this->evse_id_transaction_data_map[evse_id] == nullptr) {
        return false;
    }

    // check if transaction is already started (in OCPP) for this evse
    if (this->evse_id_transaction_data_map[evse_id]->started) {
        for (const auto& tx_stop_condition : this->tx_stop_points) {
            if (this->tx_start_stop_conditions[evse_id].is_stop_condition_fullfilled(tx_stop_condition)) {
                return true;
            }
        }
    }
    return false;
}

} // namespace module
