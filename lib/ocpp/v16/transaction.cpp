// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <mutex>

#include <everest/logging.hpp>

#include <ocpp/v16/transaction.hpp>

namespace ocpp {
namespace v16 {

Transaction::Transaction(const int32_t& connector, const std::string& session_id, const CiString<20>& id_token,
                         const int32_t& meter_start, std::optional<int32_t> reservation_id,
                         const ocpp::DateTime& timestamp,
                         std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer) :
    transaction_id(-1),
    connector(connector),
    session_id(session_id),
    id_token(id_token),
    start_energy_wh(std::make_shared<StampedEnergyWh>(timestamp, meter_start)),
    reservation_id(reservation_id),
    active(true),
    finished(false),
    meter_values_sample_timer(std::move(meter_values_sample_timer)),
    start_transaction_message_id(""),
    stop_transaction_message_id("") {
}

int32_t Transaction::get_connector() {
    return this->connector;
}

CiString<20> Transaction::get_id_tag() {
    return this->id_token;
}

void Transaction::add_meter_value(MeterValue meter_value) {
    if (this->active) {
        std::lock_guard<std::mutex> lock(this->meter_values_mutex);
        this->meter_values.push_back(meter_value);
    }
}

std::vector<MeterValue> Transaction::get_meter_values() {
    std::lock_guard<std::mutex> lock(this->meter_values_mutex);
    return this->meter_values;
}

bool Transaction::change_meter_values_sample_interval(int32_t interval) {
    this->meter_values_sample_timer->interval(std::chrono::seconds(interval));
    return true;
}

int32_t Transaction::get_transaction_id() {
    return this->transaction_id;
}

std::string Transaction::get_session_id() {
    return this->session_id;
}

void Transaction::set_start_transaction_message_id(const std::string& message_id) {
    this->start_transaction_message_id = message_id;
}

std::string Transaction::get_start_transaction_message_id() {
    return this->start_transaction_message_id;
}

void Transaction::set_stop_transaction_message_id(const std::string& message_id) {
    this->stop_transaction_message_id = message_id;
}

std::string Transaction::get_stop_transaction_message_id() {
    return this->stop_transaction_message_id;
}

void Transaction::set_transaction_id(int32_t transaction_id) {
    this->transaction_id = transaction_id;
}

std::vector<TransactionData> Transaction::get_transaction_data() {
    std::vector<TransactionData> transaction_data_vec;
    for (auto meter_value : this->get_meter_values()) {
        TransactionData transaction_data;
        transaction_data.timestamp = meter_value.timestamp;
        transaction_data.sampledValue = meter_value.sampledValue;
        transaction_data_vec.push_back(transaction_data);
    }
    return transaction_data_vec;
}

void Transaction::stop() {
    if (this->meter_values_sample_timer != nullptr) {
        this->meter_values_sample_timer->stop();
    }
    this->active = false;
}

bool Transaction::is_active() {
    return this->active;
}

bool Transaction::is_finished() {
    return this->finished;
}

void Transaction::set_finished() {
    this->finished = true;
}

std::shared_ptr<StampedEnergyWh> Transaction::get_start_energy_wh() {
    return this->start_energy_wh;
}

void Transaction::add_stop_energy_wh(std::shared_ptr<StampedEnergyWh> stop_energy_wh) {
    this->stop_energy_wh = stop_energy_wh;
    this->stop();
}

std::shared_ptr<StampedEnergyWh> Transaction::get_stop_energy_wh() {
    return this->stop_energy_wh;
}

std::optional<int32_t> Transaction::get_reservation_id() {
    return this->reservation_id;
}

TransactionHandler::TransactionHandler(int32_t number_of_connectors) : number_of_connectors(number_of_connectors) {
    for (int32_t i = 0; i < number_of_connectors + 1; i++) {
        this->active_transactions.push_back(nullptr);
    }
}

void TransactionHandler::add_transaction(std::shared_ptr<Transaction> transaction) {
    std::lock_guard<std::mutex> lk(this->active_transactions_mutex);
    this->active_transactions.at(transaction->get_connector()) = std::move(transaction);
}

void TransactionHandler::add_stopped_transaction(int32_t connector) {
    this->stopped_transactions.push_back(std::move(this->active_transactions.at(connector)));
}

bool TransactionHandler::remove_active_transaction(int32_t connector) {
    if (connector == 0) {
        EVLOG_warning << "Attempting to remove a transaction on connector 0, this is not supported.";
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
        this->active_transactions.at(connector) = nullptr;
    }
    return true;
}

void TransactionHandler::erase_stopped_transaction(std::string stop_transaction_message_id) {
    this->stopped_transactions.erase(
        std::remove_if(this->stopped_transactions.begin(), this->stopped_transactions.end(),
                       [stop_transaction_message_id](std::shared_ptr<Transaction>& transaction) {
                           return transaction->get_stop_transaction_message_id() == stop_transaction_message_id;
                       }),
        this->stopped_transactions.end());
}

std::shared_ptr<Transaction> TransactionHandler::get_transaction(int32_t connector) {
    std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
    if (this->active_transactions.at(connector) == nullptr) {
        return nullptr;
    }
    return this->active_transactions.at(connector);
}

std::shared_ptr<Transaction> TransactionHandler::get_transaction(const std::string& transaction_message_id) {
    std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
    for (const auto& transaction : this->active_transactions) {
        if (transaction != nullptr && (transaction->get_start_transaction_message_id() == transaction_message_id or
                                       transaction->get_stop_transaction_message_id() == transaction_message_id)) {
            return transaction;
        }
    }
    for (auto transaction : this->stopped_transactions) {
        if (transaction->get_start_transaction_message_id() == transaction_message_id or
            transaction->get_stop_transaction_message_id() == transaction_message_id) {
            return transaction;
        }
    }
    return nullptr;
}

int32_t TransactionHandler::get_connector_from_transaction_id(int32_t transaction_id) {
    std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
    int32_t index = 0;
    for (auto& transaction : this->active_transactions) {
        if (transaction != nullptr) {
            if (transaction->get_transaction_id() == transaction_id) {
                return index;
            }
        }
        index += 1;
    }
    return -1;
}

void TransactionHandler::add_meter_value(int32_t connector, const MeterValue& meter_value) {
    std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
    if (this->active_transactions.at(connector) == nullptr) {
        return;
    }
    this->active_transactions.at(connector)->add_meter_value(meter_value);
}

void TransactionHandler::change_meter_values_sample_intervals(int32_t interval) {
    std::lock_guard<std::mutex> lock(this->active_transactions_mutex);
    for (auto& transaction : this->active_transactions) {
        if (transaction != nullptr && transaction->is_active()) {
            transaction->change_meter_values_sample_interval(interval);
        }
    }
}

std::optional<CiString<20>>
TransactionHandler::get_authorized_id_tag(const std::string& stop_transaction_message_id) {
    for (const auto& transaction : this->stopped_transactions) {
        if (transaction->get_stop_transaction_message_id() == stop_transaction_message_id) {
            transaction->get_id_tag();
        }
    }
    return std::nullopt;
}

bool TransactionHandler::transaction_active(int32_t connector) {
    return this->get_transaction(connector) != nullptr && this->get_transaction(connector)->is_active();
}

} // namespace v16
} // namespace ocpp
