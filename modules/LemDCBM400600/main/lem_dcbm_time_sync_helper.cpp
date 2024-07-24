// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "lem_dcbm_time_sync_helper.hpp"
#include "http_client_interface.hpp"
#include "lem_dcbm_400600_controller.hpp"
#include <boost/algorithm/string.hpp>
#include <date/tz.h>
#include <nlohmann/json.hpp>
#include <string>
#include <utils/date.hpp>

namespace module::main {

std::string LemDCBMTimeSyncHelper::generate_dcbm_ntp_config() {
    nlohmann::ordered_json config_json = {
        {"ntp",
         {{"servers",
           {{{"ipAddress", ntp_spec.ip_addr_1}, {"port", ntp_spec.port_1}},
            {{"ipAddress", ntp_spec.ip_addr_2}, {"port", ntp_spec.port_2}}}},
          {"syncPeriod", 120},
          {"ntpActivated", true}}},
    };

    return config_json.dump();
}

void LemDCBMTimeSyncHelper::set_time_config_params(const std::string& meter_timezone, const std::string& meter_dst) {
    this->meter_timezone = meter_timezone;
    this->meter_dst = meter_dst;
}

void LemDCBMTimeSyncHelper::sync_if_deadline_expired(const HttpClientInterface& httpClient) {
    const std::lock_guard<std::recursive_mutex> lock(this->time_sync_state_lock);

    if (this->ntp_spec.ntp_enabled && this->dcbm_ntp_settings_saved) {
        return;
    }

    if (std::chrono::steady_clock::now() >= this->deadline_for_next_sync) {
        try {
            this->sync(httpClient);
        } catch (LemDCBM400600Controller::DCBMUnexpectedResponseException& error) {
            EVLOG_warning << "Failed to sync time settings: " << error.what();
            this->deadline_for_next_sync =
                std::chrono::steady_clock::now() + this->timing_constants.min_time_between_sync_retries;
        }
    }
}

void LemDCBMTimeSyncHelper::sync(const HttpClientInterface& httpClient) {
    const std::lock_guard<std::recursive_mutex> lock(this->time_sync_state_lock);

    if (this->ntp_spec.ntp_enabled && this->dcbm_ntp_settings_saved) {
        return;
    }

    if (this->ntp_spec.ntp_enabled) {
        this->set_ntp_settings_on_device(httpClient);
    } else {
        this->sync_system_time(httpClient);
        this->sync_timezone(httpClient);
        this->sync_dst(httpClient);
    }
}

bool LemDCBMTimeSyncHelper::is_setting_write_safe() const {
    if (!this->unsafe_period_start_time.has_value()) {
        EVLOG_warning << "LEM DCBM 400/600: Time sync was attempted, but the unsafe period start time is not set.";
        return false;
    }
    // According to LEM DCBM manual, no setting should be written earlier than 2 minutes after the DCBM is powered on
    bool sync_is_too_early = std::chrono::steady_clock::now() <
                             unsafe_period_start_time.value() + timing_constants.min_time_before_setting_write_is_safe;
    if (sync_is_too_early) {
        EVLOG_warning << "LEM DCBM 400/600: Time sync was performed earlier than 2 minutes after initialization. "
                         "Time will be synced regardless, but it may not be reliably saved.";
    }
    return !sync_is_too_early;
}

void LemDCBMTimeSyncHelper::set_ntp_settings_on_device(const HttpClientInterface& httpClient) {
    HttpResponse response = httpClient.put("/v1/settings", this->generate_dcbm_ntp_config());
    if (response.status_code != 200) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseCode("/v1/settings", 200, response);
    }
    bool success = nlohmann::json::parse(response.body).at("result") == 1;
    if (!success) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseBody(
            "/v1/settings", "NTP setting was rejected by the device, e.g. because of an ongoing transaction.");
    }
    if (!is_setting_write_safe()) {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.min_time_between_sync_retries;
    } else {
        this->dcbm_ntp_settings_saved = true;
    }
}

void LemDCBMTimeSyncHelper::sync_system_time(const HttpClientInterface& httpClient) {
    std::string time_update = Everest::Date::to_rfc3339(date::utc_clock::now());
    std::string payload = R"({"time":{"utc":")" + time_update + R"("}})";
    HttpResponse response = httpClient.put("/v1/settings", payload);

    if (response.status_code != 200) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseCode("/v1/settings", 200, response);
    }
    bool success = nlohmann::json::parse(response.body).at("result") == 1;
    if (!success) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseBody(
            "/v1/settings", "Time setting was rejected by the device, e.g. because of an ongoing transaction.");
    }

    if (is_setting_write_safe()) {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.deadline_increment_after_sync;
    } else {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.min_time_between_sync_retries;
    }
}

void LemDCBMTimeSyncHelper::sync_timezone(const HttpClientInterface& httpClient) {
    std::string payload = std::string(R"({"time": {"tz":")") + meter_timezone + R"("}})";
    HttpResponse response = httpClient.put("/v1/settings", payload);

    if (response.status_code != 200) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseCode("/v1/settings", 200, response);
    }
    bool success = nlohmann::json::parse(response.body).at("result") == 1;
    if (!success) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseBody(
            "/v1/settings", "Timezone setting was rejected by the device, e.g. because of an ongoing transaction.");
    }

    if (is_setting_write_safe()) {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.deadline_increment_after_sync;
    } else {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.min_time_between_sync_retries;
    }
}

void LemDCBMTimeSyncHelper::sync_dst(const HttpClientInterface& httpClient) {
    std::string payload = std::string(R"({"time": {"dst":)") + meter_dst + R"(}})";
    HttpResponse response = httpClient.put("/v1/settings", payload);

    if (response.status_code != 200) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseCode("/v1/settings", 200, response);
    }
    bool success = nlohmann::json::parse(response.body).at("result") == 1;
    if (!success) {
        throw LemDCBM400600Controller::UnexpectedDCBMResponseBody(
            "/v1/settings",
            "Daylight saving setting was rejected by the device, e.g. because of an ongoing transaction.");
    }

    if (is_setting_write_safe()) {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.deadline_increment_after_sync;
    } else {
        this->deadline_for_next_sync =
            std::chrono::steady_clock::now() + this->timing_constants.min_time_between_sync_retries;
    }
}

void LemDCBMTimeSyncHelper::restart_unsafe_period() {
    this->unsafe_period_start_time = std::chrono::steady_clock::now();
    deadline_for_next_sync = unsafe_period_start_time.value() + timing_constants.min_time_before_setting_write_is_safe;
}
} // namespace module::main
