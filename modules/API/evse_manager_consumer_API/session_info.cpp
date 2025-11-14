// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_info.hpp"
#include <utils/date.hpp>

namespace module {

SessionInfo::SessionInfo() :
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0),
    latest_total_w(0),
    state(State::Unknown) {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

everest::lib::API::V1_0::types::evse_manager::SessionInfo SessionInfo::to_external_api() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    everest::lib::API::V1_0::types::evse_manager::SessionInfo result;

    auto charged_energy_wh = this->end_energy_import_wh - this->start_energy_import_wh;
    int32_t discharged_energy_wh{0};
    if ((this->start_energy_export_wh_was_set == true) && (this->end_energy_export_wh_was_set == true)) {
        discharged_energy_wh = this->end_energy_export_wh - this->start_energy_export_wh;
    }

    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(this->end_time_point - this->start_time_point);

    result.state = to_external_api(state);
    result.charged_energy_wh = charged_energy_wh;
    result.discharged_energy_wh = discharged_energy_wh;
    result.latest_total_w = static_cast<int32_t>(this->latest_total_w);
    result.duration_s = duration_s.count();
    result.selected_protocol = this->selected_protocol;
    result.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    return result;
}

void SessionInfo::reset() {
    this->state = State::Unknown;
    this->start_energy_import_wh = 0;
    this->end_energy_import_wh = 0;
    this->start_energy_export_wh = 0;
    this->end_energy_export_wh = 0;
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
    this->latest_total_w = 0;
    this->selected_protocol = "";
}

void SessionInfo::set_start_energy_import_wh(int32_t start_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_import_wh = start_energy_import_wh;
    this->end_energy_import_wh = start_energy_import_wh;
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

void SessionInfo::set_end_energy_import_wh(int32_t end_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_import_wh = end_energy_import_wh;
    this->end_time_point = date::utc_clock::now();
}

void SessionInfo::set_latest_energy_import_wh(int32_t latest_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_time_point = date::utc_clock::now();
        this->end_energy_import_wh = latest_energy_wh;
    }
}

void SessionInfo::set_start_energy_export_wh(int32_t start_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_export_wh = start_energy_export_wh;
    this->end_energy_export_wh = start_energy_export_wh;
    this->start_energy_export_wh_was_set = true;
}

void SessionInfo::set_end_energy_export_wh(int32_t end_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_export_wh = end_energy_export_wh;
    this->end_energy_export_wh_was_set = true;
}

void SessionInfo::set_latest_energy_export_wh(int32_t latest_export_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_energy_export_wh = latest_export_energy_wh;
        this->end_energy_export_wh_was_set = true;
    }
}

void SessionInfo::set_latest_total_w(double latest_total_w) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->latest_total_w = latest_total_w;
}

void SessionInfo::set_selected_protocol(const std::string& protocol) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->selected_protocol = protocol;
    }
}

bool SessionInfo::is_session_running() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    return this->state != State::Unplugged && this->state != State::Disabled && this->state != State::Finished &&
           this->state != State::FinishedEVSE && this->state != State::FinishedEV and this->state != State::AuthTimeout;
}

void SessionInfo::update_state(const types::evse_manager::SessionEvent event) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    using Event = types::evse_manager::SessionEventEnum;

    // using switch since some code analysis tools can detect missing cases
    // (when new events are added)
    switch (event.event) {
    case Event::Enabled:
        this->state = State::Unplugged;
        break;
    case Event::Disabled:
        this->state = State::Disabled;
        break;
    case Event::AuthRequired:
        this->state = State::AuthRequired;
        break;
    case Event::SessionStarted:
        this->reset();
        this->state = State::Preparing;
        break;
    case Event::PrepareCharging:
    case Event::TransactionStarted:
        this->state = State::Preparing;
        break;
    case Event::ChargingResumed:
    case Event::ChargingStarted:
        this->state = State::Charging;
        break;
    case Event::ChargingPausedEV:
        this->state = State::ChargingPausedEV;
        break;
    case Event::ChargingPausedEVSE:
        this->state = State::ChargingPausedEVSE;
        break;
    case Event::WaitingForEnergy:
        this->state = State::WaitingForEnergy;
        break;
    case Event::ChargingFinished:
        this->state = State::Finished;
        break;
    case Event::StoppingCharging:
        this->state = State::FinishedEV;
        break;
    case Event::TransactionFinished: {
        if (event.transaction_finished->reason == types::evse_manager::StopTransactionReason::Local) {
            this->state = State::FinishedEVSE;
        } else {
            this->state = State::Finished;
        }
        break;
    }
    case Event::PluginTimeout:
        this->state = State::AuthTimeout;
        break;
    case Event::SessionFinished:
        this->state = State::Unplugged;
        break;
    case Event::ReservationStart:
    case Event::ReservationEnd:
    case Event::ReplugStarted:
    case Event::ReplugFinished:
    default:
        break;
    }
}

bool SessionInfo::is_state_charging(const SessionInfo::State current_state) {
    if (current_state == State::Charging || current_state == State::ChargingPausedEV ||
        current_state == State::ChargingPausedEVSE) {
        return true;
    }
    return false;
}

everest::lib::API::V1_0::types::evse_manager::EvseStateEnum SessionInfo::to_external_api(State state) {
    switch (state) {
    case State::Unknown:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Unknown;
    case State::Unplugged:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Unplugged;
    case State::Disabled:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Disabled;
    case State::Preparing:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Preparing;
    case State::AuthRequired:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::AuthRequired;
    case State::WaitingForEnergy:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::WaitingForEnergy;
    case State::ChargingPausedEV:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::ChargingPausedEV;
    case State::ChargingPausedEVSE:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::ChargingPausedEVSE;
    case State::Charging:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Charging;
    case State::AuthTimeout:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::AuthTimeout;
    case State::Finished:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Finished;
    case State::FinishedEVSE:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::FinishedEVSE;
    case State::FinishedEV:
        return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::FinishedEV;
    }
    return everest::lib::API::V1_0::types::evse_manager::EvseStateEnum::Unknown;
}

} // namespace module