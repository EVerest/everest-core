// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_info.hpp"
#include <everest/logging.hpp>
#include <utils/date.hpp>
namespace module {

using namespace everest::lib::API::V1_0::types::evse_manager;

SessionInfo::SessionInfo() :
    publish_cb([](auto) {}),
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0) {
    this->session_start_time_point = date::utc_clock::now();
    this->session_end_time_point = this->session_start_time_point;
    this->transaction_start_time_point = this->session_start_time_point;
    this->transaction_end_time_point = this->session_start_time_point;
}

void SessionInfo::set_publish_callback(PublishCallback cb) {
    this->publish_cb = [cb](everest::lib::API::V1_0::types::evse_manager::SessionInfo ext) {
        ext.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
        cb(ext);
    };
}

void SessionInfo::update_state(const types::evse_manager::SessionEvent& session_event) {
    using Event = types::evse_manager::SessionEventEnum;

    try {
        switch (session_event.event) {
        case Event::Enabled:
            this->ext.state = EvseStateEnum::Unplugged;
            break;
        case Event::Disabled:
            this->ext.state = EvseStateEnum::Disabled;
            break;
        case Event::AuthRequired:
            this->ext.state = EvseStateEnum::AuthRequired;
            break;
        case Event::SessionStarted:
            this->handle_session_started(session_event);
            this->ext.state = EvseStateEnum::Preparing;
            break;
        case Event::PrepareCharging:
            this->ext.state = EvseStateEnum::Preparing;
            break;
        case Event::TransactionStarted:
            this->handle_transaction_started(session_event);
            break;
        case Event::ChargingResumed:
        case Event::ChargingStarted:
            this->ext.state = EvseStateEnum::Charging;
            break;
        case Event::ChargingPausedEV:
            this->ext.state = EvseStateEnum::ChargingPausedEV;
            break;
        case Event::ChargingPausedEVSE:
            this->ext.state = EvseStateEnum::ChargingPausedEVSE;
            break;
        case Event::WaitingForEnergy:
            this->ext.state = EvseStateEnum::WaitingForEnergy;
            break;
        case Event::ChargingFinished:
            this->ext.state = EvseStateEnum::Finished;
            break;
        case Event::StoppingCharging:
            this->ext.state = EvseStateEnum::FinishedEV;
            break;
        case Event::TransactionFinished: {
            this->handle_transaction_finished(session_event);
            break;
        }
        case Event::PluginTimeout:
            this->ext.state = EvseStateEnum::AuthTimeout;
            break;
        case Event::SessionFinished:
            this->handle_session_finished(session_event);
            break;
        case Event::ReservationStart:
        case Event::ReservationEnd:
        case Event::ReplugStarted:
        case Event::ReplugFinished:
        default:
            break;
        }
        publish_cb(this->ext);
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
            this->ext.latest_total_w = powermeter.power_W.value().total;
        }

        if (this->is_session_running()) {
            publish_cb(this->ext);
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Powermeter update handling failed with -> " << e.what();
    }
}

void SessionInfo::update_selected_protocol(const std::string& protocol) {
    try {
        this->ext.selected_protocol = protocol;
        if (this->is_session_running()) {
            publish_cb(this->ext);
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Selected protocol update handling failed with -> " << e.what();
    }
}

void SessionInfo::handle_session_started(const types::evse_manager::SessionEvent& session_event) {
    this->session_start_time_point = Everest::Date::from_rfc3339(session_event.timestamp);
    this->session_end_time_point = this->session_start_time_point;
    this->start_energy_import_wh = this->end_energy_import_wh;
    this->start_energy_export_wh = this->end_energy_export_wh;

    this->ext.state = EvseStateEnum::Unknown;
    this->ext.charged_energy_wh = 0;
    this->ext.discharged_energy_wh = 0;
    this->ext.session_duration_s = 0;
    this->ext.transaction_duration_s.reset();
    this->ext.latest_total_w = 0;
    this->ext.selected_protocol.reset();
    this->ext.transaction_start_time.reset();
    this->ext.transaction_end_time.reset();
    this->ext.session_end_time.reset();
    this->ext.session_start_time = Everest::Date::to_rfc3339(this->session_start_time_point);
}

void SessionInfo::handle_session_finished(const types::evse_manager::SessionEvent& session_event) {
    this->ext.session_end_time = session_event.timestamp;
    this->ext.state = EvseStateEnum::Unplugged;
}

void SessionInfo::handle_transaction_started(const types::evse_manager::SessionEvent& session_event) {
    this->ext.state = EvseStateEnum::Preparing;
    this->transaction_running = true;

    if (!session_event.transaction_started.has_value()) {
        return;
    }

    auto transaction_started = session_event.transaction_started.value();
    this->transaction_start_time_point = Everest::Date::from_rfc3339(session_event.timestamp);
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

    this->ext.transaction_start_time = Everest::Date::to_rfc3339(this->transaction_start_time_point);
}

void SessionInfo::handle_transaction_finished(const types::evse_manager::SessionEvent& session_event) {
    this->ext.state = EvseStateEnum::Finished;

    if (!session_event.transaction_finished.has_value()) {
        return;
    }

    auto transaction_finished = session_event.transaction_finished.value();

    if (transaction_finished.reason == types::evse_manager::StopTransactionReason::Local) {
        this->ext.state = EvseStateEnum::FinishedEVSE;
    }

    auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
    this->end_energy_import_wh = energy_Wh_import;
    this->transaction_end_time_point = Everest::Date::from_rfc3339(session_event.timestamp);
    this->transaction_running = false;

    if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
        auto energy_Wh_export = transaction_finished.meter_value.energy_Wh_export.value().total;
        this->end_energy_export_wh = energy_Wh_export;
        this->end_energy_export_wh_was_set = true;
    } else {
        this->end_energy_export_wh_was_set = false;
    }

    this->ext.transaction_end_time = Everest::Date::to_rfc3339(this->transaction_end_time_point);
}

void SessionInfo::set_latest_energy_import_wh(int32_t latest_energy_wh_import) {
    this->ext.charged_energy_wh = this->end_energy_import_wh - this->start_energy_import_wh;
    if (this->start_energy_export_wh_was_set && this->end_energy_export_wh_was_set) {
        this->ext.discharged_energy_wh = this->end_energy_export_wh - this->start_energy_export_wh;
    }

    this->ext.session_duration_s =
        std::chrono::duration_cast<std::chrono::seconds>(this->session_end_time_point - this->session_start_time_point)
            .count();
    this->session_end_time_point = date::utc_clock::now();

    if (transaction_running) {
        this->ext.transaction_duration_s = std::chrono::duration_cast<std::chrono::seconds>(
                                               this->transaction_end_time_point - this->transaction_start_time_point)
                                               .count();
        this->transaction_end_time_point = this->session_end_time_point;
        this->end_energy_import_wh = latest_energy_wh_import;
    }
}

void SessionInfo::set_latest_energy_export_wh(int32_t latest_export_energy_wh) {
    this->end_energy_export_wh = latest_export_energy_wh;
    this->end_energy_export_wh_was_set = true;
}

bool SessionInfo::is_session_running() {
    return this->ext.state != EvseStateEnum::Unplugged && this->ext.state != EvseStateEnum::Disabled and
           this->ext.state != EvseStateEnum::Unknown;
}

} // namespace module