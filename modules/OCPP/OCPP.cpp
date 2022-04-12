// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"

namespace module {

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);

    boost::filesystem::path config_path = boost::filesystem::path(this->config.ChargePointConfigPath);

    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    json json_config = json::parse(config_file);
    json_config.at("Core").at("NumberOfConnectors") = this->r_evse_manager.size();

    std::shared_ptr<ocpp1_6::ChargePointConfiguration> configuration =
        std::make_shared<ocpp1_6::ChargePointConfiguration>(json_config, this->config.SchemasPath,
                                                            this->config.DatabasePath);

    this->charge_point = new ocpp1_6::ChargePoint(configuration);
    // TODO(kai): select appropriate EVSE based on connector
    this->charge_point->register_pause_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_pause_charging();
        } else {
            return false;
        }
    });
    this->charge_point->register_resume_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_resume_charging();
        } else {
            return false;
        }
    });
    this->charge_point->register_cancel_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_cancel_charging();
        } else {
            return false;
        }
    });

    // int32_t reservation_id, CiString20Type auth_token, DateTime expiry_date, std::string parent_id
    this->charge_point->register_reserve_now_callback(
        [this](int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate, ocpp1_6::CiString20Type idTag,
               boost::optional<ocpp1_6::CiString20Type> parent_id) {
            if (ResConnMap.count(reservation_id) == 0) {
                this->ResConnMap[reservation_id] = connector;
            } else if (ResConnMap.count(reservation_id) == 1) {
                std::map<int32_t, int32_t>::iterator it;
                it = this->ResConnMap.find(reservation_id);
                this->ResConnMap.erase(it);
                this->ResConnMap[reservation_id] = connector;

            } else {
                return ocpp1_6::ReservationStatus::Faulted;
            }

            if (connector > 0 && connector <= this->r_evse_manager.size()) {

                std::string response =
                    this->r_evse_manager.at(connector - 1)
                        ->call_reserve_now(
                            reservation_id, idTag.get(), expiryDate.to_rfc3339(),
                            std::string("")); // TODO: replace empty string with parent_id.get() when evse is ready
                return this->ResStatMap.at(response);
            } else {
                return ocpp1_6::ReservationStatus::Unavailable;
            }
        });

    this->charge_point->register_cancel_reservation_callback([this](int32_t reservationId) {
        int32_t connector = this->ResConnMap[reservationId];
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->CanResStatMap.at(this->r_evse_manager.at(connector - 1)->call_cancel_reservation());
        } else {
            return this->CanResStatMap.at(false);
        }
    });

    this->charge_point->start();

    int32_t connector = 1;
    for (auto& evse : this->r_evse_manager) {
        evse->subscribe_powermeter([this, connector](Object powermeter) {
            this->charge_point->receive_power_meter(connector, powermeter); //
        });

        evse->subscribe_limits([this, connector](Object limits) {
            auto max_current = limits["max_current"].get<double>();
            this->charge_point->receive_max_current_offered(connector, max_current);

            auto number_of_phases = limits["nr_of_phases_available"].get<int32_t>();
            this->charge_point->receive_number_of_phases_available(connector, number_of_phases);
        });

        evse->subscribe_session_events([this, connector](Object session_events) {
            auto event = session_events["event"];
            if (event == "Enabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "Disabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "SessionStarted") {
                auto session_started = session_events["session_started"];
                auto timestamp = ocpp1_6::DateTime(std::chrono::system_clock::time_point(
                    std::chrono::seconds(session_started["timestamp"].get<int>())));
                auto energy_Wh_import = session_started["energy_Wh_import"].get<double>();
                this->charge_point->start_session(connector, timestamp, energy_Wh_import);
            } else if (event == "ChargingStarted") {
                this->charge_point->start_charging(connector);
            } else if (event == "ChargingPausedEV") {
                this->charge_point->suspend_charging_ev(connector);
            } else if (event == "ChargingPausedEVSE") {
                this->charge_point->suspend_charging_evse(connector);
            } else if (event == "ChargingResumed") {
                this->charge_point->resume_charging(connector);
            } else if (event == "SessionFinished") {
                auto session_finished = session_events["session_finished"];
                auto timestamp = std::chrono::system_clock::time_point(
                    std::chrono::seconds(session_finished["timestamp"].get<int>()));
                auto energy_Wh_import = session_finished["energy_Wh_import"].get<double>();
                this->charge_point->stop_session(connector, ocpp1_6::DateTime(timestamp), energy_Wh_import);
            } else if (event == "Error") {
                auto error = session_events["error"];
                if (error == "OverCurrent") {
                    this->charge_point->error(connector, ocpp1_6::ChargePointErrorCode::OverCurrentFailure);
                } else {
                    this->charge_point->vendor_error(connector, error);
                }
            } else if (event == "PermanentFault") {
                this->charge_point->permanent_fault(connector);
            }
        });

        connector += 1;
    }
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
}

} // namespace module
