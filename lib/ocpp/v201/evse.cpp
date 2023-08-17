// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utility>

#include <everest/logging.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/utils.hpp>

namespace ocpp {
namespace v201 {

Evse::Evse(const int32_t evse_id, const int32_t number_of_connectors,
           const std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)>&
               status_notification_callback,
           const std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                                    const std::optional<int32_t> reservation_id)>& transaction_meter_value_req) :
    evse_id(evse_id),
    status_notification_callback(status_notification_callback),
    transaction_meter_value_req(transaction_meter_value_req),
    transaction(nullptr) {
    for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
        this->id_connector_map.insert(std::make_pair(
            connector_id,
            std::make_unique<Connector>(connector_id, [this, connector_id](const ConnectorStatusEnum& status) {
                this->status_notification_callback(connector_id, status);
            })));
    }
}

EVSE Evse::get_evse_info() {
    EVSE evse{evse_id};
    return evse;
}

int32_t Evse::get_number_of_connectors() {
    return this->id_connector_map.size();
}

Everest::SteadyTimer& Evse::get_sampled_meter_values_timer() {
    return this->sampled_meter_values_timer;
}

void Evse::open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                            const MeterValue& meter_start, const IdToken& id_token,
                            const std::optional<IdToken>& group_id_token, const std::optional<int32_t> reservation_id,
                            const int32_t sampled_data_tx_updated_interval) {
    if (!this->id_connector_map.count(connector_id)) {
        EVLOG_AND_THROW(std::runtime_error("Attempt to start transaction at invalid connector_id"));
    }
    this->transaction = std::make_unique<EnhancedTransaction>();
    this->transaction->transactionId = transaction_id;
    this->transaction->reservation_id = reservation_id;
    this->transaction->id_token = id_token;
    this->transaction->group_id_token = group_id_token;

    transaction->meter_values.push_back(meter_start);

    if (sampled_data_tx_updated_interval > 0) {
        this->sampled_meter_values_timer.interval(
            [this] {
                const auto meter_value = this->get_meter_value();
                this->transaction->meter_values.push_back(meter_value);
                this->transaction_meter_value_req(meter_value, this->transaction->get_transaction(),
                                                  transaction->get_seq_no(), this->transaction->reservation_id);
            },
            std::chrono::seconds(sampled_data_tx_updated_interval));
    }
}

void Evse::close_transaction(const DateTime& timestamp, const MeterValue& meter_stop, const ReasonEnum& reason) {
    if (this->transaction == nullptr) {
        EVLOG_warning << "Received attempt to stop a transaction without an active transaction";
        return;
    }

    this->transaction->stoppedReason.emplace(reason);
    this->transaction->meter_values.push_back(meter_stop);
    this->sampled_meter_values_timer.stop();
}

bool Evse::has_active_transaction() {
    return this->transaction != nullptr;
}

void Evse::release_transaction() {
    this->transaction = nullptr;
}

std::unique_ptr<EnhancedTransaction>& Evse::get_transaction() {
    return this->transaction;
}

ConnectorStatusEnum Evse::get_state(const int32_t connector_id) {
    return this->id_connector_map.at(connector_id)->get_state();
}

void Evse::submit_event(const int32_t connector_id, ConnectorEvent event) {
    return this->id_connector_map.at(connector_id)->submit_event(event);
}

void Evse::trigger_status_notification_callbacks() {
    for (auto const& [connector_id, connector] : this->id_connector_map) {
        this->status_notification_callback(connector_id, connector->get_state());
    }
}

void Evse::on_meter_value(const MeterValue& meter_value) {
    std::lock_guard<std::mutex> lk(this->meter_value_mutex);
    this->meter_value = meter_value;
}

MeterValue Evse::get_meter_value() {
    std::lock_guard<std::mutex> lk(this->meter_value_mutex);
    return this->meter_value;
}

} // namespace v201
} // namespace ocpp
