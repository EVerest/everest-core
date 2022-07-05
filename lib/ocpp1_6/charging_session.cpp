// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <mutex>

#include <everest/logging.hpp>
#include <everest/timer.hpp>

#include <ocpp1_6/charging_session.hpp>

namespace ocpp1_6 {
Transaction::Transaction(int32_t transactionId, int32_t connector,
                         std::unique_ptr<Everest::SteadyTimer> meter_values_sample_timer, CiString20Type idTag,
                         std::string start_transaction_message_id) :
    transactionId(transactionId),
    connector(connector),
    idTag(idTag),
    start_transaction_message_id(start_transaction_message_id),
    active(true),
    finished(false),
    meter_values_sample_timer(std::move(meter_values_sample_timer)) {
}

int32_t Transaction::get_connector() {
    return this->connector;
}

CiString20Type Transaction::get_id_tag() {
    return this->idTag;
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
    return this->transactionId;
}

void Transaction::set_start_transaction_message_id(const std::string message_id) {
    this->start_transaction_message_id = message_id;
}

std::string Transaction::get_start_transaction_message_id() {
    return this->start_transaction_message_id;
}

void Transaction::set_stop_transaction_message_id(const std::string message_id) {
    this->stop_transaction_message_id = message_id;
}

std::string Transaction::get_stop_transaction_message_id() {
    return this->stop_transaction_message_id;
}

void Transaction::set_transaction_id(int32_t transaction_id) {
    this->transactionId = transaction_id;
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
    this->active = false;
}

void Transaction::set_charging_profile(ChargingProfile charging_profile) {
    std::lock_guard<std::mutex> charge_point_max_profiles_lock(tx_charging_profiles_mutex);
    this->tx_charging_profiles[charging_profile.stackLevel] = charging_profile;
}

bool Transaction::transaction_active() {
    return this->active;
}

bool Transaction::is_finished() {
    return this->finished;
}

void Transaction::set_finished() {
    this->finished = true;
}

void Transaction::remove_charging_profile(int32_t stack_level) {
    std::lock_guard<std::mutex> charge_point_max_profiles_lock(tx_charging_profiles_mutex);
    this->tx_charging_profiles.erase(stack_level);
}

void Transaction::remove_charging_profiles() {
    std::lock_guard<std::mutex> charge_point_max_profiles_lock(tx_charging_profiles_mutex);
    this->tx_charging_profiles.clear();
}

std::map<int32_t, ChargingProfile> Transaction::get_charging_profiles() {
    std::lock_guard<std::mutex> charge_point_max_profiles_lock(tx_charging_profiles_mutex);
    return this->tx_charging_profiles;
}

ChargingSession::ChargingSession() :
    authorized_token(nullptr), plug_connected(false), transaction(nullptr), reservation_id(boost::none) {
}

ChargingSession::ChargingSession(std::unique_ptr<AuthorizedToken> authorized_token) :
    authorized_token(std::move(authorized_token)),
    plug_connected(false),
    transaction(nullptr),
    reservation_id(boost::none) {
}

void ChargingSession::connect_plug() {
    this->plug_connected = true;
}

void ChargingSession::disconnect_plug() {
    this->plug_connected = false;
}

bool ChargingSession::authorized_token_available() {
    return this->authorized_token != nullptr;
}

bool ChargingSession::add_authorized_token(std::unique_ptr<AuthorizedToken> authorized_token) {
    this->authorized_token = std::move(authorized_token);
    return true;
}

bool ChargingSession::add_start_energy_wh(std::shared_ptr<StampedEnergyWh> start_energy_wh) {
    if (this->start_energy_wh != nullptr) {
        return false;
    }
    this->start_energy_wh = start_energy_wh;
    return true;
}

std::shared_ptr<StampedEnergyWh> ChargingSession::get_start_energy_wh() {
    return this->start_energy_wh;
}

bool ChargingSession::add_stop_energy_wh(std::shared_ptr<StampedEnergyWh> stop_energy_wh) {
    if (this->stop_energy_wh != nullptr) {
        return false;
    }
    if (this->transaction != nullptr) {
        this->transaction->stop();
    }
    this->stop_energy_wh = stop_energy_wh;
    return true;
}

std::shared_ptr<StampedEnergyWh> ChargingSession::get_stop_energy_wh() {
    return this->stop_energy_wh;
}

bool ChargingSession::ready() {
    return this->plug_connected && this->authorized_token_available() && this->start_energy_wh != nullptr;
}

bool ChargingSession::running() {
    return this->transaction != nullptr && this->transaction->transaction_active();
}

boost::optional<CiString20Type> ChargingSession::get_authorized_id_tag() {
    if (this->authorized_token == nullptr) {
        return boost::none;
    }
    return this->authorized_token->idTag;
}

bool ChargingSession::add_transaction(std::shared_ptr<Transaction> transaction) {
    if (this->transaction != nullptr) {
        return false;
    }
    this->transaction = transaction;
    return true;
}

std::shared_ptr<Transaction> ChargingSession::get_transaction() {
    return this->transaction;
}

bool ChargingSession::change_meter_values_sample_interval(int32_t interval) {
    if (this->transaction == nullptr) {
        return false;
    }
    return this->transaction->change_meter_values_sample_interval(interval);
}

void ChargingSession::add_meter_value(MeterValue meter_value) {
    if (this->transaction != nullptr) {
        this->transaction->add_meter_value(meter_value);
    }
}

std::vector<MeterValue> ChargingSession::get_meter_values() {
    if (this->transaction == nullptr) {
        return {};
    }
    return this->transaction->get_meter_values();
}

void ChargingSession::add_reservation_id(int32_t reservation_id) {
    this->reservation_id.emplace(reservation_id);
}

boost::optional<int32_t> ChargingSession::get_reservation_id() {
    if (this->reservation_id == boost::none) {
        return boost::none;
    }
    return this->reservation_id;
}

bool ChargingSessionHandler::valid_connector(int32_t connector) {
    if (connector < 0 || connector > static_cast<int32_t>(this->active_charging_sessions.size())) {
        return false;
    }
    return true;
}

ChargingSessionHandler::ChargingSessionHandler(int32_t number_of_connectors) :
    number_of_connectors(number_of_connectors) {
    for (int32_t i = 0; i < number_of_connectors + 1; i++) {
        this->active_charging_sessions.push_back(nullptr);
    }
}

void ChargingSessionHandler::add_stopped_transaction(std::shared_ptr<Transaction> stopped_transaction) {
    this->stopped_transactions.push_back(stopped_transaction);
}

int32_t ChargingSessionHandler::add_authorized_token(CiString20Type idTag, IdTagInfo idTagInfo) {
    return this->add_authorized_token(0, idTag, idTagInfo);
}

int32_t ChargingSessionHandler::add_authorized_token(int32_t connector, CiString20Type idTag, IdTagInfo idTagInfo) {
    if (!this->valid_connector(connector)) {
        return -1;
    }

    auto authorized_token = std::make_unique<AuthorizedToken>(idTag, idTagInfo);

    if (connector == 0) {
        std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
        // check if a charging session is missing an authorized token
        bool moved = false;
        int32_t index = 0;
        for (auto& charging_session : this->active_charging_sessions) {
            if (charging_session != nullptr && charging_session->get_authorized_id_tag() == boost::none) {
                charging_session->add_authorized_token(std::move(authorized_token));
                moved = true;
                connector = index;
                break;
            }
            index += 1;
        }
        if (!moved) {
            this->active_charging_sessions.at(0) = std::make_unique<ChargingSession>(std::move(authorized_token));
        }
    } else {
        std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
        if (this->active_charging_sessions.at(connector) == nullptr) {
            this->active_charging_sessions.at(connector) =
                std::make_unique<ChargingSession>(std::move(authorized_token));
        } else {
            if (this->active_charging_sessions.at(connector)->running()) {
                // do not add a authorized token to a running charging session
                return -1;
            }
            this->active_charging_sessions.at(connector)->add_authorized_token(std::move(authorized_token));
        }
    }
    return connector;
}

bool ChargingSessionHandler::add_start_energy_wh(int32_t connector, std::shared_ptr<StampedEnergyWh> start_energy_wh) {
    if (!this->valid_connector(connector)) {
        return false;
    }
    if (connector == 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return false;
    }
    this->active_charging_sessions.at(connector)->add_start_energy_wh(start_energy_wh);
    return true;
}

std::shared_ptr<StampedEnergyWh> ChargingSessionHandler::get_start_energy_wh(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return nullptr;
    }
    if (connector == 0) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return nullptr;
    }
    return this->active_charging_sessions.at(connector)->get_start_energy_wh();
}

bool ChargingSessionHandler::add_stop_energy_wh(int32_t connector, std::shared_ptr<StampedEnergyWh> stop_energy_wh) {
    if (!this->valid_connector(connector)) {
        return false;
    }
    if (connector == 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return false;
    }
    this->active_charging_sessions.at(connector)->add_stop_energy_wh(stop_energy_wh);
    return true;
}

std::shared_ptr<StampedEnergyWh> ChargingSessionHandler::get_stop_energy_wh(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return nullptr;
    }
    if (connector == 0) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return nullptr;
    }
    return this->active_charging_sessions.at(connector)->get_stop_energy_wh();
}

bool ChargingSessionHandler::initiate_session(int32_t connector) {
    // TODO(kai): think about supporting connector 0 here, meaning "any connector"
    if (connector == 0) {
        EVLOG_warning << "Attempting to start a charging session on connector 0, this is not supported at the moment";
        return false;
    }
    if (!this->valid_connector(connector)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
        if (this->active_charging_sessions.at(connector) == nullptr) {
            if (this->active_charging_sessions.at(0) != nullptr) {
                this->active_charging_sessions.at(connector) = std::move(this->active_charging_sessions.at(0));
            } else {
                this->active_charging_sessions.at(connector) = std::make_unique<ChargingSession>();
            }
        }
        if (this->active_charging_sessions.at(connector)->running()) {
            return false;
        }
        this->active_charging_sessions.at(connector)->connect_plug();
    }
    return true;
}

bool ChargingSessionHandler::remove_active_session(int32_t connector) {
    if (connector == 0) {
        EVLOG_warning << "Attempting to remove a charging session on connector 0, this is not supported.";
        return false;
    }
    if (!this->valid_connector(connector)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
        this->active_charging_sessions.at(connector) = nullptr;
    }
    return true;
}

bool ChargingSessionHandler::remove_stopped_transaction(std::string stop_transaction_message_id) {
    int32_t index = 0;
    for (size_t i = 0; i < this->stopped_transactions.size(); i++) {
        if (this->stopped_transactions.at(i)->get_stop_transaction_message_id() == stop_transaction_message_id) {
            index = i;
        }
    }
    this->stopped_transactions.erase(this->stopped_transactions.begin() + index);
    return true;
}

bool ChargingSessionHandler::ready(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) != nullptr) {
        return this->active_charging_sessions.at(connector)->ready();
    }
    return false;
}

bool ChargingSessionHandler::add_transaction(int32_t connector, std::shared_ptr<Transaction> transaction) {
    if (!this->valid_connector(connector)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return false;
    }
    return this->active_charging_sessions.at(connector)->add_transaction(transaction);
}

std::shared_ptr<Transaction> ChargingSessionHandler::get_transaction(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return nullptr;
    }
    return this->active_charging_sessions.at(connector)->get_transaction();
}

std::shared_ptr<Transaction> ChargingSessionHandler::get_transaction(const std::string start_transaction_message_id) {

    for (const auto& session : this->active_charging_sessions) {
        if (session->get_transaction() != nullptr &&
            session->get_transaction()->get_start_transaction_message_id() == start_transaction_message_id) {
            return session->get_transaction();
        }
    }

    for (const auto transaction : this->stopped_transactions) {
        if (transaction->get_start_transaction_message_id() == start_transaction_message_id) {
            return transaction;
        }
    }
    return nullptr;
}

bool ChargingSessionHandler::transaction_active(int32_t connector) {
    return this->get_transaction(connector) != nullptr && this->get_transaction(connector)->transaction_active();
}

bool ChargingSessionHandler::all_connectors_have_active_transaction() {
    for (int connector = 1; connector <= this->number_of_connectors; connector++) {
        if (this->get_transaction(connector) == nullptr || !this->transaction_active(connector)) {
            return false;
        }
    }
    return true;
}

int32_t ChargingSessionHandler::get_connector_from_transaction_id(int32_t transaction_id) {
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    int32_t index = 0;
    for (auto& charging_session : this->active_charging_sessions) {
        if (charging_session != nullptr) {
            auto transaction = charging_session->get_transaction();
            if (transaction != nullptr) {
                if (transaction->get_transaction_id() == transaction_id) {
                    return index;
                }
            }
        }
        index += 1;
    }
    return -1;
}

bool ChargingSessionHandler::change_meter_values_sample_interval(int32_t connector, int32_t interval) {
    if (!this->valid_connector(connector)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return false;
    }
    return this->active_charging_sessions.at(connector)->change_meter_values_sample_interval(interval);
}

bool ChargingSessionHandler::change_meter_values_sample_intervals(int32_t interval) {
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    bool success = true;
    for (auto& charging_session : this->active_charging_sessions) {
        if (charging_session != nullptr && charging_session->running()) {
            if (!charging_session->change_meter_values_sample_interval(interval)) {
                success = false;
            }
        }
    }
    return success;
}

boost::optional<CiString20Type> ChargingSessionHandler::get_authorized_id_tag(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return boost::none;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return boost::none;
    }
    return this->active_charging_sessions.at(connector)->get_authorized_id_tag();
}

boost::optional<CiString20Type> ChargingSessionHandler::get_authorized_id_tag(std::string stop_transaction_message_id) {
    for (const auto& transaction : this->stopped_transactions) {
        if (transaction->get_stop_transaction_message_id() == stop_transaction_message_id) {
            transaction->get_id_tag();
        }
    }
    return boost::none;
}

void ChargingSessionHandler::add_meter_value(int32_t connector, MeterValue meter_value) {
    if (!this->valid_connector(connector)) {
        return;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return;
    }
    this->active_charging_sessions.at(connector)->add_meter_value(meter_value);
}

std::vector<MeterValue> ChargingSessionHandler::get_meter_values(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return {};
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return {};
    }
    return this->active_charging_sessions.at(connector)->get_meter_values();
}

void ChargingSessionHandler::add_reservation_id(int32_t connector, int32_t reservation_id) {
    if (!this->valid_connector(connector)) {
        return;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return;
    }
    this->active_charging_sessions.at(connector)->add_reservation_id(reservation_id);
}

boost::optional<int32_t> ChargingSessionHandler::get_reservation_id(int32_t connector) {
    if (!this->valid_connector(connector)) {
        return boost::none;
    }
    std::lock_guard<std::mutex> lock(this->active_charging_sessions_mutex);
    if (this->active_charging_sessions.at(connector) == nullptr) {
        return boost::none;
    }
    return this->active_charging_sessions.at(connector)->get_reservation_id();
}

} // namespace ocpp1_6
