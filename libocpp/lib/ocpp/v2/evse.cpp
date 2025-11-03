// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <cmath>
#include <optional>
#include <utility>

#include <everest/database/exceptions.hpp>
#include <everest/logging.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/evse.hpp>

using namespace std::chrono_literals;
using QueryExecutionException = everest::db::QueryExecutionException;

namespace ocpp {
namespace v2 {

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
           const std::function<void(const MeterValue& meter_value, EnhancedTransaction& transaction)>&
               transaction_meter_value_req,
           const std::function<void(int32_t evse_id)>& pause_charging_callback) :
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

    if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::ResumeTransactionsOnBoot)
            .value_or(false)) {
        this->try_resume_transaction();
    } else {
        this->delete_database_transaction();
    }
}

Evse::~Evse() {
    if (this->trigger_metervalue_at_time_timer != nullptr) {
        this->trigger_metervalue_at_time_timer->stop();
        this->trigger_metervalue_at_time_timer = nullptr;
    }
}

int32_t Evse::get_id() const {
    return this->evse_id;
}

uint32_t Evse::get_number_of_connectors() const {
    return static_cast<uint32_t>(this->id_connector_map.size());
}

bool Evse::does_connector_exist(const CiString<20> connector_type) const {
    const uint32_t number_of_connectors = this->get_number_of_connectors();
    if (number_of_connectors == 0) {
        return false;
    }

    if (connector_type == ConnectorEnumStringType::Unknown) {
        return true;
    }

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        Connector* connector;
        try {
            connector = this->get_connector(static_cast<int32_t>(i));
        } catch (const std::logic_error&) {
            EVLOG_error << "Connector with id " << i << " does not exist";
            continue;
        }

        if (connector == nullptr) {
            EVLOG_error << "Connector with id " << i << " does not exist";
            continue;
        }

        const CiString<20> type = this->get_evse_connector_type(i).value_or(ConnectorEnumStringType::Unknown);
        if (type == ConnectorEnumStringType::Unknown || type == connector_type) {
            return true;
        }
    }

    return false;
}

std::optional<ConnectorStatusEnum> Evse::get_connector_status(std::optional<CiString<20>> connector_type) {
    bool type_found = false;
    ConnectorStatusEnum found_status = ConnectorStatusEnum::Unavailable;
    const uint32_t number_of_connectors = this->get_number_of_connectors();
    if (number_of_connectors == 0) {
        return std::nullopt;
    }

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        Connector* connector;
        try {
            connector = this->get_connector(static_cast<int32_t>(i));
        } catch (const std::logic_error&) {
            EVLOG_error << "Connector with id " << i << " does not exist";
            continue;
        }

        if (connector == nullptr) {
            EVLOG_error << "Connector with id " << i << " does not exist";
            continue;
        }

        const ConnectorStatusEnum connector_status = connector->get_effective_connector_status();

        const CiString<20> evse_connector_type =
            this->get_evse_connector_type(i).value_or(ConnectorEnumStringType::Unknown);
        const CiString<20> input_connector_type = connector_type.value_or(ConnectorEnumStringType::Unknown);
        const bool connector_type_unknown = evse_connector_type == ConnectorEnumStringType::Unknown ||
                                            input_connector_type == ConnectorEnumStringType::Unknown;

        if (connector_type_unknown || evse_connector_type == input_connector_type) {
            type_found = true;
            // We found an available connector, also store the status.
            found_status = connector_status;
            if (found_status == ConnectorStatusEnum::Available) {
                // There is an available connector with the correct type and status: we don't have to search
                // any further.
                return found_status;
            }

            // If status is not available, we keep on searching. If no connector is available, the status of
            // (at least one of) the connectors is stored to return that later if no available connector is found.
        }
    }

    if (!type_found) {
        return std::nullopt;
    }

    return found_status;
}

void Evse::try_resume_transaction() {
    // Get an open transactions from the database and resume it if there is one
    auto transaction = this->database_handler->transaction_get(evse_id);
    if (transaction == nullptr) {
        return;
    }

    if (this->id_connector_map.count(transaction->connector_id) != 0) {
        this->transaction = std::move(transaction);
        this->start_metering_timers(this->transaction->start_time);
    } else {
        EVLOG_error << "Can't resume transaction on evse_id " << evse_id << " for non existent connector "
                    << transaction->connector_id;

        try {
            this->database_handler->transaction_delete(transaction->transactionId);
        } catch (const QueryExecutionException& e) {
            EVLOG_error << "Can't delete transaction: " << e.what();
        }

        // Todo: Can we drop the transaction like this or do we still want to transmit an ended message?
    }
}

void Evse::delete_database_transaction() {
    try {
        auto transaction = this->database_handler->transaction_get(evse_id);
        if (transaction != nullptr) {
            this->database_handler->transaction_delete(transaction->transactionId);
        }
    } catch (const QueryExecutionException& e) {
        EVLOG_error << "Can't delete transaction: " << e.what();
    }
}

std::optional<CiString<20>> Evse::get_evse_connector_type(const uint32_t connector_id) const {

    auto connector = this->get_connector(static_cast<int32_t>(connector_id));
    if (connector == nullptr) {
        return std::nullopt;
    }

    ComponentVariable connector_cv = ConnectorComponentVariables::get_component_variable(
        this->evse_id, connector_id, ConnectorComponentVariables::Type);

    const std::optional<std::string> connector_type =
        this->device_model.get_optional_value<std::string>(connector_cv, AttributeEnum::Actual);
    if (!connector_type.has_value()) {
        return std::nullopt;
    }

    return connector_type.value();
}

void Evse::open_transaction(const std::string& transaction_id, const int32_t connector_id, const DateTime& timestamp,
                            const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                            const std::optional<IdToken>& group_id_token, const std::optional<int32_t> reservation_id,
                            const ChargingStateEnum charging_state) {
    if (!this->id_connector_map.count(connector_id)) {
        EVLOG_AND_THROW(std::runtime_error("Attempt to start transaction at invalid connector_id"));
    }

    bool tx_database_enabled =
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::ResumeTransactionsOnBoot)
            .value_or(false);

    this->transaction = std::make_unique<EnhancedTransaction>(*this->database_handler.get(), tx_database_enabled);
    this->transaction->transactionId = transaction_id;
    this->transaction->connector_id = connector_id;
    this->transaction->id_token_sent = id_token.has_value();
    this->transaction->start_time = timestamp;
    this->transaction->active_energy_import_start_value = this->get_active_import_register_meter_value();
    this->transaction->chargingState = charging_state;

    try {
        this->database_handler->transaction_metervalues_insert(this->transaction->transactionId.get(), meter_start);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    } catch (const std::invalid_argument& e) {
        EVLOG_warning << "Could not insert transaction meter values of transaction: "
                      << this->transaction->transactionId.get() << " into database: " << e.what();
    }

    this->start_metering_timers(timestamp);

    if (tx_database_enabled) {
        try {
            this->database_handler->transaction_insert(*this->transaction.get(), this->evse_id);
        } catch (const QueryExecutionException& e) {
            // Delete previous transactions that should not exist anyway and try again. Otherwise throw to higher
            // level
            this->delete_database_transaction();
            this->database_handler->transaction_insert(*this->transaction.get(), this->evse_id);
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

bool Evse::has_active_transaction() const {
    return this->transaction != nullptr;
}

bool Evse::has_active_transaction(int32_t connector_id) const {
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
    try {
        this->database_handler->transaction_metervalues_clear(this->transaction->transactionId);
        this->database_handler->transaction_delete(this->transaction->transactionId);
    } catch (const everest::db::Exception& e) {
        EVLOG_error << "Could not clear transaction meter values: " << e.what();
    }
    this->transaction = nullptr;

    this->reset_pricing_triggers();
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
    this->send_meter_value_on_pricing_trigger(meter_value);
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

            if (std::lround(charged_energy) >= max_energy_on_invalid_id.value()) {
                this->pause_charging_callback(this->evse_id);
                transaction->check_max_active_import_energy = false; // No need to check anymore
            }
        }
    }
}

void Evse::start_metering_timers(const DateTime& timestamp) {

    this->aligned_data_updated.clear_values();
    this->aligned_data_tx_end.clear_values();

    const auto sampled_data_tx_updated_interval = std::chrono::seconds(
        this->device_model.get_value<int>(ControllerComponentVariables::SampledDataTxUpdatedInterval));
    const auto sampled_data_tx_ended_interval = std::chrono::seconds(
        this->device_model.get_value<int>(ControllerComponentVariables::SampledDataTxEndedInterval));
    const auto aligned_data_tx_updated_interval =
        std::chrono::seconds(this->device_model.get_value<int>(ControllerComponentVariables::AlignedDataInterval));
    const auto aligned_data_tx_ended_interval = std::chrono::seconds(
        this->device_model.get_value<int>(ControllerComponentVariables::AlignedDataTxEndedInterval));

    if (sampled_data_tx_updated_interval > 0s) {
        this->transaction->sampled_tx_updated_meter_values_timer.interval_starting_from(
            [this] { this->transaction_meter_value_req(this->get_meter_value(), *this->transaction); },
            sampled_data_tx_updated_interval, date::utc_clock::to_sys(timestamp.to_time_point()));
    }

    if (sampled_data_tx_ended_interval > 0s) {
        this->transaction->sampled_tx_ended_meter_values_timer.interval_starting_from(
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
        this->transaction->aligned_tx_updated_meter_values_timer.interval_starting_from(
            [this, aligned_data_tx_updated_interval] {
                if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::AlignedDataSendDuringIdle)
                        .value_or(false)) {
                    return;
                }
                auto meter_value = this->aligned_data_updated.retrieve_processed_values();

                // If empty fallback on last updated metervalue
                if (meter_value.sampledValue.empty()) {
                    meter_value = this->get_meter_value();
                }

                for (auto& item : meter_value.sampledValue) {
                    item.context = ReadingContextEnum::Sample_Clock;
                }
                if (this->device_model
                        .get_optional_value<bool>(ControllerComponentVariables::RoundClockAlignedTimestamps)
                        .value_or(false)) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, aligned_data_tx_updated_interval);
                }
                this->transaction_meter_value_req(meter_value, *this->transaction);
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
                meter_value = this->get_meter_value();
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

        // Store an extra aligned metervalue to fix the edge case where a transaction is started just before an
        // interval but this code is processed just after the interval. For example, aligned interval = 1 min,
        // transaction started at 11:59:59.500 and we get here on 12:00:00.100. There is still the expectation for
        // us to add a metervalue at timepoint 12:00:00.000 which we do with this.
        if (date::utc_clock::to_sys(timestamp.to_time_point()) <= (next_interval - aligned_data_tx_ended_interval)) {
            store_aligned_metervalue();
        }
    }
}

void Evse::set_meter_value_pricing_triggers(
    std::optional<double> trigger_metervalue_on_power_kw, std::optional<double> trigger_metervalue_on_energy_kwh,
    std::optional<DateTime> trigger_metervalue_at_time,
    std::function<void(const std::vector<MeterValue>& meter_values)> send_metervalue_function,
    boost::asio::io_context& io_context) {

    EVLOG_debug << "Set metervalue pricing triggers: "
                << (trigger_metervalue_at_time.has_value()
                        ? "at time: " + trigger_metervalue_at_time.value().to_rfc3339()
                        : "no time pricing trigger")
                << (trigger_metervalue_on_energy_kwh.has_value()
                        ? ", on energy kWh: " + std::to_string(trigger_metervalue_on_energy_kwh.value())
                        : ", No energy kWh trigger, ")
                << (trigger_metervalue_on_power_kw.has_value()
                        ? ", on power kW: " + std::to_string(trigger_metervalue_on_power_kw.value())
                        : ", No power kW trigger");

    this->send_metervalue_function = send_metervalue_function;
    this->trigger_metervalue_on_power_kw = trigger_metervalue_on_power_kw;
    this->trigger_metervalue_on_energy_kwh = trigger_metervalue_on_energy_kwh;
    if (this->trigger_metervalue_at_time_timer != nullptr and trigger_metervalue_at_time.has_value()) {
        this->trigger_metervalue_at_time_timer->stop();
        this->trigger_metervalue_at_time_timer = nullptr;
    }

    std::chrono::time_point<date::utc_clock> trigger_timepoint = trigger_metervalue_at_time.value().to_time_point();
    const std::chrono::time_point<date::utc_clock> now = date::utc_clock::now();

    if (trigger_timepoint < now) {
        EVLOG_error << "Could not set trigger metervalue because trigger time is in the past.";
        return;
    }

    // Start a timer for the trigger 'atTime'.
    this->trigger_metervalue_at_time_timer = std::make_unique<Everest::SystemTimer>(&io_context, [this]() {
        EVLOG_error << "Sending metervalue in timer";

        const MeterValue meter_value = utils::get_meter_value_with_measurands_applied(
            this->get_meter_value(), {MeasurandEnum::Energy_Active_Import_Register});
        if (meter_value.sampledValue.empty()) {
            EVLOG_error << "Send latest meter value because of chargepoint time trigger failed";
        } else {
            const MeterValue mv = utils::set_meter_value_reading_context(meter_value, ReadingContextEnum::Other);
            this->send_metervalue_function({mv});
        }
    });
    EVLOG_error << "Set trigger metervalue at time " << trigger_timepoint;

    this->trigger_metervalue_at_time_timer->at(trigger_timepoint);
}

void Evse::reset_pricing_triggers() {
    this->last_triggered_metervalue_power_kw = std::nullopt;
    this->trigger_metervalue_on_power_kw = std::nullopt;
    this->trigger_metervalue_on_energy_kwh = std::nullopt;
    this->send_metervalue_function = nullptr;

    if (this->trigger_metervalue_at_time_timer != nullptr) {
        this->trigger_metervalue_at_time_timer->stop();
        this->trigger_metervalue_at_time_timer = nullptr;
    }
}

void Evse::send_meter_value_on_pricing_trigger(const MeterValue& meter_value) {
    bool meter_value_sent = false;
    // Check if there is a kwh trigger and if the value is exceeded.
    if (this->trigger_metervalue_on_energy_kwh.has_value()) {
        const double trigger_energy_kwh = this->trigger_metervalue_on_energy_kwh.value();
        if (this->send_metervalue_function == nullptr) {
            EVLOG_error << "Cost and price metervalue kwh trigger: Can not send metervalue because the send metervalue "
                           "function is not set.";
            this->trigger_metervalue_on_energy_kwh.reset();
        } else {
            const std::optional<float> active_import_register_meter_value_wh = get_active_import_register_meter_value();
            if (active_import_register_meter_value_wh.has_value() and
                static_cast<double>(active_import_register_meter_value_wh.value()) >= trigger_energy_kwh * 1000) {
                const MeterValue active_import_meter_value = utils::get_meter_value_with_measurands_applied(
                    meter_value, {MeasurandEnum::Energy_Active_Import_Register, MeasurandEnum::Power_Active_Import});
                if (active_import_meter_value.sampledValue.empty()) {
                    EVLOG_error
                        << "No current active import register metervalue found. Can not send trigger metervalue.";
                } else {
                    const MeterValue to_send =
                        utils::set_meter_value_reading_context(active_import_meter_value, ReadingContextEnum::Other);
                    this->send_metervalue_function({to_send});
                    this->trigger_metervalue_on_energy_kwh.reset();
                    meter_value_sent = true;
                }
            }
        }
    }

    // Check if there is a power kw trigger and if that is triggered. For the power kw trigger, we added hysterisis
    // to prevent constant triggering.
    const std::optional<float> active_power_meter_value = utils::get_total_power_active_import(meter_value);

    if (!this->trigger_metervalue_on_power_kw.has_value() or !active_power_meter_value.has_value()) {
        return;
    }

    const double trigger_power_kw = this->trigger_metervalue_on_power_kw.value();
    if (this->send_metervalue_function == nullptr) {
        EVLOG_error << "Cost and price metervalue wh trigger: Can not send metervalue because the send metervalue "
                       "function is not set.";
        // Remove trigger because next time function is not set as well (this is probably a bug because it should be
        // set in the `set_meter_value_pricing_triggers` function together with the trigger values).
        this->trigger_metervalue_on_energy_kwh.reset();
        return;
    }

    if (this->last_triggered_metervalue_power_kw.has_value()) {
        // Hysteresis of 5% to avoid repetitive triggers when the power fluctuates around the trigger level.
        const double hysterisis_kw = trigger_power_kw * 0.05;
        const double triggered_power_kw = this->last_triggered_metervalue_power_kw.value();
        const double current_metervalue_w = static_cast<double>(active_power_meter_value.value());
        const double current_metervalue_kw = current_metervalue_w / 1000;

        if ( // Check if trigger value is crossed in upward direction.
            (triggered_power_kw < trigger_power_kw and current_metervalue_kw >= (trigger_power_kw + hysterisis_kw)) or
            // Check if trigger value is crossed in downward direction.
            (triggered_power_kw > trigger_power_kw and current_metervalue_kw <= (trigger_power_kw - hysterisis_kw))) {

            // Power threshold is crossed, send metervalues.
            if (!meter_value_sent) {
                // Only send metervalue if it is not sent yet, otherwise only the last triggered metervalue is set.
                const MeterValue mv = utils::set_meter_value_reading_context(meter_value, ReadingContextEnum::Other);
                this->send_metervalue_function({mv});
            }

            // Also when metervalue is sent, we want to set the last triggered metervalue.
            this->last_triggered_metervalue_power_kw = current_metervalue_kw;
        }
    } else {
        // Send metervalue anyway since we have no previous metervalue stored and don't know if we should send any
        if (!meter_value_sent) {
            // Only send metervalue if it is not sent yet, otherwise only the last triggered metervalue is set.
            const MeterValue mv = utils::set_meter_value_reading_context(meter_value, ReadingContextEnum::Other);
            this->send_metervalue_function({mv});
        }
        this->last_triggered_metervalue_power_kw = active_power_meter_value.value() / 1000;
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

OperationalStatusEnum Evse::get_connector_effective_operational_status(const int32_t connector_id) {
    return this->component_state_manager->get_connector_effective_operational_status(this->get_id(), connector_id);
}

OperationalStatusEnum Evse::get_effective_operational_status() {
    return this->component_state_manager->get_evse_effective_operational_status(this->evse_id);
}

Connector* Evse::get_connector(int32_t connector_id) const {
    if (connector_id <= 0 or connector_id > this->get_number_of_connectors()) {
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
    } else if (*supply_phases == 1 or *supply_phases == 3) {
        return CurrentPhaseType::AC;
    } else if (*supply_phases == 0) {
        return CurrentPhaseType::DC;
    }

    // NOTE: SupplyPhases should never be a value that isn't NULL, 1, 3, or 0.
    return CurrentPhaseType::Unknown;
}

} // namespace v2
} // namespace ocpp
