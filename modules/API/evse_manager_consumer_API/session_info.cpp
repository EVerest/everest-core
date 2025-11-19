// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_info.hpp"
#include <everest/logging.hpp>
#include <utils/date.hpp>
namespace module {

SessionInfo::SessionInfo() :
    publish_cb([](auto) {}),
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0),
    latest_total_w(0),
    state(State::Unknown) {
    this->session_start_time_point = date::utc_clock::now();
    this->session_end_time_point = this->session_start_time_point;
    this->transaction_start_time_point = this->session_start_time_point;
    this->transaction_end_time_point = this->session_start_time_point;
}

void SessionInfo::set_publish_callback(PublishCallback cb) {
    this->publish_cb = cb;
}

everest::lib::API::V1_0::types::evse_manager::SessionInfo SessionInfo::to_external_api() {
    everest::lib::API::V1_0::types::evse_manager::SessionInfo result;

    result.state = to_external_api(state);
    result.charged_energy_wh = charged_energy_wh;
    result.discharged_energy_wh = discharged_energy_wh;
    result.latest_total_w = static_cast<int32_t>(this->latest_total_w);
    result.session_duration_s = session_duration_s.count();
    result.transaction_duration_s = transaction_duration_s.count();
    result.selected_protocol = this->selected_protocol;
    result.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    return result;
}

void SessionInfo::update_state(const types::evse_manager::SessionEvent& session_event) {
    using Event = types::evse_manager::SessionEventEnum;

    try {
        switch (session_event.event) {
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
            this->handle_session_started(session_event);
            this->state = State::Preparing;
            break;
        case Event::PrepareCharging:
            this->state = State::Preparing;
            break;
        case Event::TransactionStarted:
            this->handle_transaction_started(session_event);
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
            this->handle_transaction_finished(session_event);
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
        publish_cb(this->to_external_api());
    } catch (const std::exception& e) {
        EVLOG_warning << "Session event handling failed with -> " << e.what();
    }
}

void SessionInfo::update_powermeter(const types::powermeter::Powermeter& powermeter) {
    try {
        this->set_latest_energy_import_wh(powermeter.energy_Wh_import.total);
        if (powermeter.energy_Wh_export.has_value()) {
            this->set_latest_energy_export_wh(powermeter.energy_Wh_export.value().total);
        }

        if (powermeter.power_W.has_value()) {
            this->latest_total_w = powermeter.power_W.value().total;
        }

        if (this->is_session_running()) {
            publish_cb(this->to_external_api());
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Powermeter update handling failed with -> " << e.what();
    }
}

void SessionInfo::update_selected_protocol(const std::string& protocol) {
    try {
        this->selected_protocol = protocol;
        if (this->is_session_running()) {
            publish_cb(this->to_external_api());
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Selected protocol update handling failed with -> " << e.what();
    }
}

void SessionInfo::handle_session_started(const types::evse_manager::SessionEvent& session_event) {
    this->state = State::Unknown;
    this->session_start_time_point = Everest::Date::from_rfc3339(session_event.timestamp);
    this->session_end_time_point = this->session_start_time_point;
    this->start_energy_import_wh = this->end_energy_import_wh;
    this->start_energy_export_wh = this->end_energy_export_wh;
    this->charged_energy_wh = 0;
    this->discharged_energy_wh = 0;
    this->session_duration_s = std::chrono::seconds(0);
    this->transaction_duration_s = std::chrono::seconds(0);
    this->latest_total_w = 0;
    this->selected_protocol = "";
}

void SessionInfo::handle_transaction_started(const types::evse_manager::SessionEvent& session_event) {
    this->state = State::Preparing;
    this->transaction_running = true;

    if (!session_event.transaction_started.has_value()) {
        return;
    }

    auto transaction_started = session_event.transaction_started.value();
    this->transaction_start_time_point = Everest::Date::from_rfc3339(transaction_started.meter_value.timestamp);
    this->transaction_end_time_point = this->transaction_start_time_point;
    this->start_energy_import_wh = transaction_started.meter_value.energy_Wh_import.total;

    this->end_energy_import_wh = this->start_energy_import_wh;
    this->transaction_end_time_point = this->transaction_start_time_point;

    if (transaction_started.meter_value.energy_Wh_export.has_value()) {
        auto energy_Wh_export = transaction_started.meter_value.energy_Wh_export.value().total;
        this->start_energy_export_wh = energy_Wh_export;
        this->end_energy_export_wh = energy_Wh_export;
        this->start_energy_export_wh_was_set = true;
    } else {
        this->start_energy_export_wh_was_set = false;
    }
}

void SessionInfo::handle_transaction_finished(const types::evse_manager::SessionEvent& session_event) {
    this->state = State::Finished;

    if (!session_event.transaction_finished.has_value()) {
        return;
    }

    auto transaction_finished = session_event.transaction_finished.value();

    if (transaction_finished.reason == types::evse_manager::StopTransactionReason::Local) {
        this->state = State::FinishedEVSE;
    }

    auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
    this->end_energy_import_wh = energy_Wh_import;
    this->transaction_end_time_point = Everest::Date::from_rfc3339(transaction_finished.meter_value.timestamp);
    this->transaction_running = false;

    if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
        auto energy_Wh_export = transaction_finished.meter_value.energy_Wh_export.value().total;
        this->end_energy_export_wh = energy_Wh_export;
        this->end_energy_export_wh_was_set = true;
    } else {
        this->end_energy_export_wh_was_set = false;
    }
}

void SessionInfo::set_latest_energy_import_wh(int32_t latest_energy_wh_import) {
    this->charged_energy_wh = this->end_energy_import_wh - this->start_energy_import_wh;
    if (this->start_energy_export_wh_was_set && this->end_energy_export_wh_was_set) {
        this->discharged_energy_wh = this->end_energy_export_wh - this->start_energy_export_wh;
    }

    this->session_duration_s =
        std::chrono::duration_cast<std::chrono::seconds>(this->session_end_time_point - this->session_start_time_point);
    this->session_end_time_point = date::utc_clock::now();

    if (transaction_running) {
        this->transaction_duration_s = std::chrono::duration_cast<std::chrono::seconds>(
            this->transaction_end_time_point - this->transaction_start_time_point);
        this->transaction_end_time_point = this->session_end_time_point;
        this->end_energy_import_wh = latest_energy_wh_import;
    }
}

void SessionInfo::set_latest_energy_export_wh(int32_t latest_export_energy_wh) {
    this->end_energy_export_wh = latest_export_energy_wh;
    this->end_energy_export_wh_was_set = true;
}

bool SessionInfo::is_session_running() {
    return this->state != State::Unplugged && this->state != State::Disabled and this->state != State::Unknown;
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