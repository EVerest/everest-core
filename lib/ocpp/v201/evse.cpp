// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <optional>
#include <utility>

#include <everest/logging.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/evse.hpp>

using namespace std::chrono_literals;
using QueryExecutionException = ocpp::common::QueryExecutionException;

namespace ocpp {
namespace v201 {

// Convert an energy value into Wh
static float get_normalized_energy_value(SampledValue sampled_value) {
    float value = sampled_value.value;
    // If no unit of measure is present the unit is in Wh so nothing to do
    if (sampled_value.unitOfMeasure.has_value()) {
        const auto& unit_of_measure = sampled_value.unitOfMeasure.value();
        if (unit_of_measure.unit.has_value()) {
            if (unit_of_measure.unit.value() == "kWh") {
                value *= 1000.0f;
            } else if (unit_of_measure.unit.value() == "Wh") {
                // do nothing
            } else {
                EVLOG_AND_THROW(
                    std::runtime_error("Attempt to convert an energy value which does not have a correct unit"));
            }
        }

        if (unit_of_measure.multiplier.has_value()) {
            if (unit_of_measure.multiplier.value() != 0) {
                value *= powf(10, unit_of_measure.multiplier.value());
            }
        }
    }
    return value;
}

EvseInterface::~EvseInterface() {
}

Evse::Evse(const int32_t evse_id, const int32_t number_of_connectors, DeviceModel& device_model,
           std::shared_ptr<DatabaseHandler> database_handler,
           std::shared_ptr<ComponentStateManagerInterface> component_state_manager,
           const std::function<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                                    const std::optional<int32_t> reservation_id)>& transaction_meter_value_req,
           const std::function<void()> pause_charging_callback) :
    evse_id(evse_id),
    device_model(device_model),
    transaction_meter_value_req(transaction_meter_value_req),
    pause_charging_callback(pause_charging_callback),
    database_handler(database_handler),
    component_state_manager(component_state_manager),
    transaction(nullptr) {
    for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
        this->id_connector_map.insert(
            std::make_pair(connector_id, std::make_unique<Connector>(evse_id, connector_id, component_state_manager)));
    }
}

EVSE Evse::get_evse_info() {
    EVSE evse{evse_id};
    return evse;
}

uint32_t Evse::get_number_of_connectors() {
    return static_cast<uint32_t>(this->id_connector_map.size());
}

void Evse::open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                            const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                            const std::optional<IdToken>& group_id_token, const std::optional<int32_t> reservation_id,
                            const std::chrono::seconds sampled_data_tx_updated_interval,
                            const std::chrono::seconds sampled_data_tx_ended_interval,
                            const std::chrono::seconds aligned_data_tx_updated_interval,
                            const std::chrono::seconds aligned_data_tx_ended_interval) {
    if (!this->id_connector_map.count(connector_id)) {
        EVLOG_AND_THROW(std::runtime_error("Attempt to start transaction at invalid connector_id"));
    }
    this->transaction = std::make_unique<EnhancedTransaction>();
    this->transaction->transactionId = transaction_id;
    this->transaction->reservation_id = reservation_id;
    this->transaction->connector_id = connector_id;
    this->transaction->id_token = id_token;
    this->transaction->group_id_token = group_id_token;
    this->transaction->active_energy_import_start_value = this->get_active_import_register_meter_value();

    try {
        this->database_handler->transaction_metervalues_insert(this->transaction->transactionId.get(), meter_start);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    } catch (const std::invalid_argument& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    }

    this->aligned_data_updated.clear_values();
    this->aligned_data_tx_end.clear_values();

    if (sampled_data_tx_updated_interval > 0s) {
        transaction->sampled_tx_updated_meter_values_timer.interval_starting_from(
            [this] {
                this->transaction_meter_value_req(this->get_meter_value(), this->transaction->get_transaction(),
                                                  transaction->get_seq_no(), this->transaction->reservation_id);
            },
            sampled_data_tx_updated_interval, date::utc_clock::to_sys(timestamp.to_time_point()));
    }

    if (sampled_data_tx_ended_interval > 0s) {
        transaction->sampled_tx_ended_meter_values_timer.interval_starting_from(
            [this] {
                try {
                    this->database_handler->transaction_metervalues_insert(this->transaction->transactionId.get(),
                                                                           this->get_meter_value());
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not insert transaction meter values of transaction: "
                                  << this->transaction->transactionId.get() << " into database: " << e.what();
                } catch (const std::invalid_argument& e) {
                    EVLOG_warning << "Could not insert transaction meter values of transaction: "
                                  << this->transaction->transactionId.get() << " into database: " << e.what();
                }
            },
            sampled_data_tx_ended_interval, date::utc_clock::to_sys(timestamp.to_time_point()));
    }

    if (aligned_data_tx_updated_interval > 0s) {
        transaction->aligned_tx_updated_meter_values_timer.interval_starting_from(
            [this, aligned_data_tx_updated_interval] {
                if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::AlignedDataSendDuringIdle)
                        .value_or(false)) {
                    return;
                }
                auto meter_value = this->aligned_data_updated.retrieve_processed_values();

                // If empty fallback on last updated metervalue
                if (meter_value.sampledValue.empty()) {
                    meter_value = get_meter_value();
                }

                for (auto& item : meter_value.sampledValue) {
                    item.context = ReadingContextEnum::Sample_Clock;
                }
                if (this->device_model
                        .get_optional_value<bool>(ControllerComponentVariables::RoundClockAlignedTimestamps)
                        .value_or(false)) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, aligned_data_tx_updated_interval);
                }
                this->transaction_meter_value_req(meter_value, this->transaction->get_transaction(),
                                                  transaction->get_seq_no(), this->transaction->reservation_id);
                this->aligned_data_updated.clear_values();
            },
            aligned_data_tx_updated_interval,
            std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));
    }

    if (aligned_data_tx_ended_interval > 0s) {
        auto store_aligned_metervalue = [this, aligned_data_tx_ended_interval] {
            auto meter_value = this->aligned_data_tx_end.retrieve_processed_values();

            // If empty fallback on last updated metervalue
            if (meter_value.sampledValue.empty()) {
                meter_value = get_meter_value();
            }

            for (auto& item : meter_value.sampledValue) {
                item.context = ReadingContextEnum::Sample_Clock;
            }
            if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::RoundClockAlignedTimestamps)
                    .value_or(false)) {
                meter_value.timestamp = utils::align_timestamp(DateTime{}, aligned_data_tx_ended_interval);
            }
            try {
                this->database_handler->transaction_metervalues_insert(this->transaction->transactionId.get(),
                                                                       meter_value);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not insert transaction meter values of transaction: "
                              << this->transaction->transactionId.get() << " into database: " << e.what();
            } catch (const std::invalid_argument& e) {
                EVLOG_warning << "Could not insert transaction meter values of transaction: "
                              << this->transaction->transactionId.get() << " into database: " << e.what();
            }
            this->aligned_data_tx_end.clear_values();
        };

        auto next_interval = transaction->aligned_tx_ended_meter_values_timer.interval_starting_from(
            store_aligned_metervalue, aligned_data_tx_ended_interval,
            std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));

        // Store an extra aligned metervalue to fix the edge case where a transaction is started just before an interval
        // but this code is processed just after the interval.
        // For example, aligned interval = 1 min, transaction started at 11:59:59.500 and we get here on 12:00:00.100.
        // There is still the expectation for us to add a metervalue at timepoint 12:00:00.000 which we do with this.
        if (date::utc_clock::to_sys(timestamp.to_time_point()) <= (next_interval - aligned_data_tx_ended_interval)) {
            store_aligned_metervalue();
        }
    }
}

void Evse::close_transaction(const DateTime& timestamp, const MeterValue& meter_stop, const ReasonEnum& reason) {
    if (this->transaction == nullptr) {
        EVLOG_warning << "Received attempt to stop a transaction without an active transaction";
        return;
    }

    this->transaction->stoppedReason.emplace(reason);

    // First stop all the timers to make sure the meter_stop is the last one in the database
    this->transaction->sampled_tx_updated_meter_values_timer.stop();
    this->transaction->sampled_tx_ended_meter_values_timer.stop();
    this->transaction->aligned_tx_updated_meter_values_timer.stop();
    this->transaction->aligned_tx_ended_meter_values_timer.stop();

    try {
        this->database_handler->transaction_metervalues_insert(this->transaction->transactionId.get(), meter_stop);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    } catch (const std::invalid_argument& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    }
    // Clear for non transaction aligned metervalues
    this->aligned_data_updated.clear_values();
}

void Evse::start_checking_max_energy_on_invalid_id() {
    if (this->transaction != nullptr) {
        this->transaction->check_max_active_import_energy = true;
        this->check_max_energy_on_invalid_id();
    } else {
        EVLOG_error << "Trying to start \"MaxEnergyOnInvalidId\" checking without an active transaction";
    }
}

bool Evse::has_active_transaction() {
    return this->transaction != nullptr;
}

bool Evse::has_active_transaction(int32_t connector_id) {
    if (!this->id_connector_map.count(connector_id)) {
        EVLOG_warning << "has_active_transaction called for invalid connector_id";
        return false;
    }

    if (this->transaction == nullptr) {
        return false;
    }

    return this->transaction->connector_id == connector_id;
}

void Evse::release_transaction() {
    this->transaction = nullptr;
}

std::unique_ptr<EnhancedTransaction>& Evse::get_transaction() {
    return this->transaction;
}

void Evse::submit_event(const int32_t connector_id, ConnectorEvent event) {
    return this->id_connector_map.at(connector_id)->submit_event(event);
}

void Evse::on_meter_value(const MeterValue& meter_value) {
    std::lock_guard<std::recursive_mutex> lk(this->meter_value_mutex);
    this->meter_value = meter_value;
    this->aligned_data_updated.set_values(meter_value);
    this->aligned_data_tx_end.set_values(meter_value);
    this->check_max_energy_on_invalid_id();
}

MeterValue Evse::get_meter_value() {
    std::lock_guard<std::recursive_mutex> lk(this->meter_value_mutex);
    return this->meter_value;
}

MeterValue Evse::get_idle_meter_value() {
    return this->aligned_data_updated.retrieve_processed_values();
}

void Evse::clear_idle_meter_values() {
    this->aligned_data_updated.clear_values();
}

std::optional<float> Evse::get_active_import_register_meter_value() {
    std::lock_guard<std::recursive_mutex> lk(this->meter_value_mutex);
    auto it = std::find_if(
        this->meter_value.sampledValue.begin(), this->meter_value.sampledValue.end(), [](const SampledValue& value) {
            return value.measurand == MeasurandEnum::Energy_Active_Import_Register and !value.phase.has_value();
        });
    if (it != this->meter_value.sampledValue.end()) {
        return get_normalized_energy_value(*it);
    }
    return std::nullopt;
}

void Evse::check_max_energy_on_invalid_id() {
    // Handle E05.02
    auto max_energy_on_invalid_id =
        this->device_model.get_optional_value<int32_t>(ControllerComponentVariables::MaxEnergyOnInvalidId);
    auto& transaction = this->transaction;
    if (transaction != nullptr and max_energy_on_invalid_id.has_value() and
        transaction->check_max_active_import_energy) {
        const auto opt_energy_value = this->get_active_import_register_meter_value();
        auto active_energy_import_start_value = transaction->active_energy_import_start_value;
        if (opt_energy_value.has_value() and active_energy_import_start_value.has_value()) {
            auto charged_energy = opt_energy_value.value() - active_energy_import_start_value.value();

            if (charged_energy > static_cast<float>(max_energy_on_invalid_id.value())) {
                this->pause_charging_callback();
                transaction->check_max_active_import_energy = false; // No need to check anymore
            }
        }
    }
}

void Evse::set_evse_operative_status(OperationalStatusEnum new_status, bool persist) {
    this->component_state_manager->set_evse_individual_operational_status(this->evse_id, new_status, persist);
}

void Evse::set_connector_operative_status(int32_t connector_id, OperationalStatusEnum new_status, bool persist) {
    this->id_connector_map.at(connector_id)->set_connector_operative_status(new_status, persist);
}

void Evse::restore_connector_operative_status(int32_t connector_id) {
    this->id_connector_map.at(connector_id)->restore_connector_operative_status();
}

OperationalStatusEnum Evse::get_effective_operational_status() {
    return this->component_state_manager->get_evse_effective_operational_status(this->evse_id);
}

Connector* Evse::get_connector(int32_t connector_id) {
    if (connector_id <= 0 || connector_id > this->get_number_of_connectors()) {
        std::stringstream err_msg;
        err_msg << "ConnectorID " << connector_id << " out of bounds for EVSE " << this->evse_id;
        throw std::logic_error(err_msg.str());
    }
    return this->id_connector_map.at(connector_id).get();
}

CurrentPhaseType Evse::get_current_phase_type() {
    ComponentVariable evse_variable =
        EvseComponentVariables::get_component_variable(this->evse_id, EvseComponentVariables::SupplyPhases);
    auto supply_phases = this->device_model.get_optional_value<int32_t>(evse_variable);
    if (supply_phases == std::nullopt) {
        return CurrentPhaseType::Unknown;
    } else if (*supply_phases == 1 || *supply_phases == 3) {
        return CurrentPhaseType::AC;
    } else if (*supply_phases == 0) {
        return CurrentPhaseType::DC;
    }

    // NOTE: SupplyPhases should never be a value that isn't NULL, 1, 3, or 0.
    return CurrentPhaseType::Unknown;
}

} // namespace v201
} // namespace ocpp
