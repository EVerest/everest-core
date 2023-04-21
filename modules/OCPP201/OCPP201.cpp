// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>

namespace module {

const std::string INIT_SQL = "init.sql";

namespace fs = std::filesystem;

ocpp::v201::SampledValue get_sampled_value(const ocpp::v201::MeasurandEnum& measurand, const std::string& unit,
                                           const boost::optional<ocpp::v201::PhaseEnum> phase) {
    ocpp::v201::SampledValue sampled_value;
    ocpp::v201::UnitOfMeasure unit_of_measure;
    sampled_value.context = ocpp::v201::ReadingContextEnum::Sample_Periodic;
    sampled_value.location = ocpp::v201::LocationEnum::Outlet;
    sampled_value.measurand = measurand;
    unit_of_measure.unit = unit;
    sampled_value.unitOfMeasure = unit_of_measure;
    sampled_value.phase = phase;
    return sampled_value;
}

ocpp::v201::MeterValue get_meter_value(const types::powermeter::Powermeter& power_meter) {
    ocpp::v201::MeterValue meter_value;
    meter_value.timestamp = ocpp::DateTime(power_meter.timestamp);

    // Energy.Active.Import.Register
    ocpp::v201::SampledValue sampled_value =
        get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh", boost::none);
    sampled_value.value = power_meter.energy_Wh_import.total;
    meter_value.sampledValue.push_back(sampled_value);
    if (power_meter.energy_Wh_import.L1.has_value()) {
        sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh",
                                          ocpp::v201::PhaseEnum::L1);
        sampled_value.value = power_meter.energy_Wh_import.L1.value();
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L2.has_value()) {
        sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh",
                                          ocpp::v201::PhaseEnum::L2);
        sampled_value.value = power_meter.energy_Wh_import.L2.value();
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L3.has_value()) {
        sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh",
                                          ocpp::v201::PhaseEnum::L3);
        sampled_value.value = power_meter.energy_Wh_import.L3.value();
        meter_value.sampledValue.push_back(sampled_value);
    }

    // Energy.Active.Export.Register
    if (power_meter.energy_Wh_export.has_value()) {
        auto sampled_value =
            get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh", boost::none);
        sampled_value.value = power_meter.energy_Wh_export.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.energy_Wh_export.value().L1.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                              ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L2.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                              ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L3.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                              ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Active.Import
    if (power_meter.power_W.has_value()) {
        auto sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Power_Active_Import, "W", boost::none);
        sampled_value.value = power_meter.power_W.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.power_W.value().L1.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Active_Import, "W", ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L2.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Active_Import, "W", ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L3.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Active_Import, "W", ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Reactive.Import
    if (power_meter.VAR.has_value()) {
        auto sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var", boost::none);
        sampled_value.value = power_meter.VAR.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.VAR.value().L1.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var", ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L2.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var", ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L3.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var", ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Current.Import
    if (power_meter.current_A.has_value()) {
        auto sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", boost::none);
        if (power_meter.current_A.value().L1.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.current_A.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L2.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.current_A.value().L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L3.has_value()) {
            sampled_value =
                get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.current_A.value().L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().DC.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", boost::none);
            sampled_value.value = power_meter.current_A.value().DC.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().N.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Current_Import, "A", ocpp::v201::PhaseEnum::N);
            sampled_value.value = power_meter.current_A.value().N.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Voltage
    if (power_meter.voltage_V.has_value()) {
        if (power_meter.voltage_V.value().L1.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Voltage, "V", ocpp::v201::PhaseEnum::L1_N);
            sampled_value.value = power_meter.voltage_V.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L2.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Voltage, "V", ocpp::v201::PhaseEnum::L2_N);
            sampled_value.value = power_meter.voltage_V.value().L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L3.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Voltage, "V", ocpp::v201::PhaseEnum::L3_N);
            sampled_value.value = power_meter.voltage_V.value().L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().DC.has_value()) {
            sampled_value = get_sampled_value(ocpp::v201::MeasurandEnum::Voltage, "V", boost::none);
            sampled_value.value = power_meter.voltage_V.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }
    return meter_value;
}

void OCPP201::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_provider);
    invoke_init(*p_auth_validator);

    this->ocpp_share_path = this->info.paths.share;

    const auto etc_certs_path = [&]() {
        if (this->config.CertsPath.empty()) {
            return fs::path(this->info.everest_prefix) / EVEREST_ETC_CERTS_PATH;
        } else {
            return fs::path(this->config.CertsPath);
        }
    }();
    EVLOG_info << "OCPP certificates path: " << etc_certs_path.string();

    auto configured_config_path = fs::path(this->config.ChargePointConfigPath);

    // try to find the config file if it has been provided as a relative path
    if (!fs::exists(configured_config_path) && configured_config_path.is_relative()) {
        configured_config_path = this->ocpp_share_path / configured_config_path;
    }
    if (!fs::exists(configured_config_path)) {
        EVLOG_AND_THROW(Everest::EverestConfigError(
            fmt::format("OCPP config file is not available at given path: {} which was resolved to: {}",
                        this->config.ChargePointConfigPath, configured_config_path.string())));
    }
    EVLOG_info << "OCPP config: " << configured_config_path.string();

    std::ifstream ifs(configured_config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    auto json_config = json::parse(config_file);

    if (!fs::exists(this->config.MessageLogPath)) {
        try {
            fs::create_directory(this->config.MessageLogPath);
        } catch (const fs::filesystem_error& e) {
            EVLOG_AND_THROW(e);
        }
    }

    ocpp::v201::Callbacks callbacks;
    callbacks.is_reset_allowed_callback = [this](const ocpp::v201::ResetEnum& type) {
        try {
            const auto reset_type =
                types::system::string_to_reset_type(ocpp::v201::conversions::reset_enum_to_string(type));
            return this->r_system->call_is_reset_allowed(reset_type);
        } catch (std::out_of_range& e) {
            EVLOG_warning
                << "Could not convert OCPP ResetEnum to EVerest ResetType while executing is_reset_allowed_callback.";
            return false;
        }
    };
    callbacks.reset_callback = [this](const ocpp::v201::ResetEnum& type) {
        try {
            const auto reset_type =
                types::system::string_to_reset_type(ocpp::v201::conversions::reset_enum_to_string(type));
            this->r_system->call_reset(types::system::ResetType::Soft); // FIXME(piet): fix reset type in system module
                                                                        // according to v201 reset type
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert OCPP ResetEnum to EVerest ResetType while executing reset_callack. No "
                             "reset will be executed.";
        }
    };

    this->charge_point = std::make_unique<ocpp::v201::ChargePoint>(
        json_config, this->ocpp_share_path.string(), this->config.MessageLogPath, etc_certs_path, callbacks);

    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        evse->subscribe_session_event([this, evse_id](types::evse_manager::SessionEvent session_event) {
            switch (session_event.event) {
            case types::evse_manager::SessionEventEnum::SessionStarted: {
                this->charge_point->on_session_started(evse_id, 1);
                break;
            }
            case types::evse_manager::SessionEventEnum::SessionFinished: {
                this->charge_point->on_session_finished(evse_id, 1);
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionStarted: {
                const auto transaction_started = session_event.transaction_started.value();
                const auto timestamp = ocpp::DateTime(transaction_started.timestamp);
                const auto meter_start = transaction_started.energy_Wh_import;
                const auto session_id = session_event.uuid;
                const auto signed_meter_value = transaction_started.signed_meter_value;
                const auto reservation_id = transaction_started.reservation_id;

                // meter start
                ocpp::v201::MeterValue meter_value;
                ocpp::v201::SampledValue sampled_value;
                sampled_value.context = ocpp::v201::ReadingContextEnum::Transaction_Begin;
                sampled_value.location = ocpp::v201::LocationEnum::Outlet;
                sampled_value.measurand = ocpp::v201::MeasurandEnum::Energy_Active_Import_Register;
                sampled_value.value = meter_start;
                meter_value.sampledValue.push_back(sampled_value);
                meter_value.timestamp = timestamp;

                ocpp::v201::IdToken id_token;
                id_token.idToken = transaction_started.id_tag;
                id_token.type = ocpp::v201::IdTokenEnum::ISO14443;

                this->charge_point->on_transaction_started(evse_id, 1, session_id, timestamp, meter_value, id_token,
                                                           reservation_id);
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionFinished: {
                const auto transaction_finished = session_event.transaction_finished.value();
                const auto timestamp = ocpp::DateTime(transaction_finished.timestamp);
                const auto meter_stop = transaction_finished.energy_Wh_import;
                ocpp::v201::ReasonEnum reason;
                try {
                    reason = ocpp::v201::conversions::string_to_reason_enum(
                        types::evse_manager::stop_transaction_reason_to_string(transaction_finished.reason.value()));
                } catch (std::out_of_range &e) {
                    reason = ocpp::v201::ReasonEnum::Other;
                }
                const auto signed_meter_value = transaction_finished.signed_meter_value;
                const auto id_token = transaction_finished.id_tag;

                ocpp::v201::MeterValue meter_value;
                ocpp::v201::SampledValue sampled_value;
                sampled_value.context = ocpp::v201::ReadingContextEnum::Transaction_End;
                sampled_value.location = ocpp::v201::LocationEnum::Outlet;
                sampled_value.measurand = ocpp::v201::MeasurandEnum::Energy_Active_Import_Register;
                sampled_value.value = meter_stop;
                meter_value.sampledValue.push_back(sampled_value);
                meter_value.timestamp = timestamp;

                this->charge_point->on_transaction_finished(evse_id, timestamp, meter_value, reason, id_token,
                                                            signed_meter_value);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingStarted: {
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingResumed: {
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingFinished: {
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingPausedEV: {
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingPausedEVSE: {
                break;
            }
            case types::evse_manager::SessionEventEnum::Disabled: {
                break;
            }
            case types::evse_manager::SessionEventEnum::Enabled: {
                break;
            }
            }
        });

        evse->subscribe_powermeter([this, evse_id](const types::powermeter::Powermeter& power_meter) {
            const auto meter_value = get_meter_value(power_meter);
            this->charge_point->on_meter_value(evse_id, meter_value);
        });
        evse_id++;
    }

    this->charge_point->start();
}

void OCPP201::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_provider);
    invoke_ready(*p_auth_validator);
}

} // namespace module
