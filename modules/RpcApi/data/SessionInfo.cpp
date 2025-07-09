#include "SessionInfo.hpp"

namespace data {

SessionInfoStore::SessionInfoStore() :
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0),
    latest_total_w(0),
    state(State::Unknown) {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;

    uk_random_delay_remaining.countdown_s = 0;
    uk_random_delay_remaining.current_limit_after_delay_A = 0.;
    uk_random_delay_remaining.current_limit_during_delay_A = 0;
}

bool SessionInfoStore::is_state_charging(const SessionInfoStore::State current_state) {
    if (current_state == State::AuthRequired || current_state == State::Charging ||
        current_state == State::ChargingPausedEV || current_state == State::ChargingPausedEVSE) {
        return true;
    }
    return false;
}

void SessionInfoStore::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = State::Unknown;
    this->start_energy_import_wh = 0;
    this->end_energy_import_wh = 0;
    this->start_energy_export_wh = 0;
    this->end_energy_export_wh = 0;
    this->start_time_point = date::utc_clock::now();
    this->latest_total_w = 0;
    this->permanent_fault = false;
}

void SessionInfoStore::update_state(const types::evse_manager::SessionEvent event) {
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
    case Event::PrepareCharging:
    case Event::SessionStarted:
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
    case Event::ReservationStart:
        this->state = State::Reserved;
        break;
    case Event::ReservationEnd:
    case Event::SessionFinished:
        this->state = State::Unplugged;
        break;
    case Event::ReplugStarted:
    case Event::ReplugFinished:
    default:
        break;
    }
}

void SessionInfoStore::set_start_energy_import_wh(int32_t start_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_import_wh = start_energy_import_wh;
    this->end_energy_import_wh = start_energy_import_wh;
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

void SessionInfoStore::set_end_energy_import_wh(int32_t end_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_import_wh = end_energy_import_wh;
    this->end_time_point = date::utc_clock::now();
}

void SessionInfoStore::set_latest_energy_import_wh(int32_t latest_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_time_point = date::utc_clock::now();
        this->end_energy_import_wh = latest_energy_wh;
    }
}

void SessionInfoStore::set_start_energy_export_wh(int32_t start_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_export_wh = start_energy_export_wh;
    this->end_energy_export_wh = start_energy_export_wh;
    this->start_energy_export_wh_was_set = true;
}

void SessionInfoStore::set_end_energy_export_wh(int32_t end_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_export_wh = end_energy_export_wh;
    this->end_energy_export_wh_was_set = true;
}

void SessionInfoStore::set_latest_energy_export_wh(int32_t latest_export_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_energy_export_wh = latest_export_energy_wh;
        this->end_energy_export_wh_was_set = true;
    }
}

void SessionInfoStore::set_latest_total_w(double latest_total_w) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->latest_total_w = latest_total_w;
}

void SessionInfoStore::set_uk_random_delay_remaining(const types::uk_random_delay::CountDown& cd) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->uk_random_delay_remaining = cd;
}

void SessionInfoStore::set_enable_disable_source(const std::string& active_source, const std::string& active_state,
                                            const int active_priority) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->active_enable_disable_source = active_source;
    this->active_enable_disable_state = active_state;
    this->active_enable_disable_priority = active_priority;
}

static void to_json(json& j, const SessionInfoStore::Error& e) {
    j = json{{"type", e.type}, {"description", e.description}, {"severity", e.severity}};
}

float SessionInfoStore::get_charged_energy_wh() const {
    return end_energy_import_wh - start_energy_import_wh;
}

float SessionInfoStore::get_discharged_energy_wh() const {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (start_energy_export_wh_was_set && end_energy_export_wh_was_set) {
        return end_energy_export_wh - start_energy_export_wh;
    }
    return 0.0f; // No discharged energy if export values are not set
}

std::chrono::seconds SessionInfoStore::get_charging_duration_s() const {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    return std::chrono::duration_cast<std::chrono::seconds>(this->end_time_point - this->start_time_point);
}
} // namespace module