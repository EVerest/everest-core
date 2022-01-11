// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"

namespace module {

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);

    // TODO(kai): support multiple EVSE with their connectors here
    this->connector = 1;

    boost::filesystem::path config_path = boost::filesystem::path(this->config.ChargePointConfigPath);

    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    json json_config = json::parse(config_file);

    std::shared_ptr<ocpp1_6::ChargePointConfiguration> configuration =
        std::make_shared<ocpp1_6::ChargePointConfiguration>(json_config, this->config.SchemasPath,
                                                            this->config.DatabasePath);

    this->charge_point = new ocpp1_6::ChargePoint(configuration);
    // TODO(kai): select appropriate EVSE based on connector
    this->charge_point->register_pause_charging_callback([this](int32_t connector) {
        return this->r_evse_manager->call_pause_charging(); //
    });
    this->charge_point->register_resume_charging_callback([this](int32_t connector) {
        return this->r_evse_manager->call_resume_charging(); //
    });
    this->charge_point->register_cancel_charging_callback([this](int32_t connector) {
        return this->r_evse_manager->call_cancel_charging(); //
    });
    this->charge_point->register_reserve_now_callback(
        [this](int32_t connector, ocpp1_6::CiString20Type idTag, std::chrono::seconds timeout) {
            return this->r_evse_manager->call_reserve_now(idTag.get(), timeout.count()); //
        });
    this->charge_point->register_cancel_reservation_callback([this](int32_t connector) {
        return this->r_evse_manager->call_cancel_reservation(); //
    });

    this->charge_point->start();

    this->r_powermeter->subscribe_powermeter([this](Object powermeter) {
        this->charge_point->receive_power_meter(this->connector, powermeter); //
    });

    this->r_evse_manager->subscribe_limits([this](Object limits) {
        auto max_current = limits["max_current"].get<double>();
        this->charge_point->receive_max_current_offered(this->connector, max_current);

        auto number_of_phases = limits["nr_of_phases_available"].get<int32_t>();
        this->charge_point->receive_number_of_phases_available(this->connector, number_of_phases);
    });

    this->r_evse_manager->subscribe_session_events([this](Object session_events) {
        auto event = session_events["event"];
        if (event == "Enabled") {
            // TODO(kai): decide if we need to inform libocpp about such an event
        } else if (event == "Disabled") {
            // TODO(kai): decide if we need to inform libocpp about such an event
        } else if (event == "SessionStarted") {
            auto session_started = session_events["session_started"];
            auto timestamp = ocpp1_6::DateTime(
                std::chrono::system_clock::time_point(std::chrono::seconds(session_started["timestamp"].get<int>())));
            auto energy_Wh_import = session_started["energy_Wh_import"].get<double>();
            this->charge_point->start_session(this->connector, timestamp, energy_Wh_import);
        } else if (event == "ChargingStarted") {
            this->charge_point->start_charging(this->connector);
        } else if (event == "ChargingPausedEV") {
            this->charge_point->suspend_charging_ev(this->connector);
        } else if (event == "ChargingPausedEVSE") {
            this->charge_point->suspend_charging_evse(this->connector);
        } else if (event == "ChargingResumed") {
            this->charge_point->resume_charging(this->connector);
        } else if (event == "SessionFinished") {
            auto session_finished = session_events["session_finished"];
            auto timestamp =
                std::chrono::system_clock::time_point(std::chrono::seconds(session_finished["timestamp"].get<int>()));
            auto energy_Wh_import = session_finished["energy_Wh_import"].get<double>();
            this->charge_point->stop_session(this->connector, ocpp1_6::DateTime(timestamp), energy_Wh_import);
        } else if (event == "Error") {
            auto error = session_events["error"];
            if (error == "OverCurrent") {
                this->charge_point->error(this->connector, ocpp1_6::ChargePointErrorCode::OverCurrentFailure);
            } else {
                this->charge_point->vendor_error(this->connector, error);
            }
        } else if (event == "PermanentFault") {
            this->charge_point->permanent_fault(this->connector);
        }
    });
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
}

} // namespace module
