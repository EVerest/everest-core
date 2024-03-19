// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_CORE_MODULE_LEM_DCBM_TIME_SYNC_HELPER_H
#define EVEREST_CORE_MODULE_LEM_DCBM_TIME_SYNC_HELPER_H

#include "http_client_interface.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace module::main {

struct timing_config {
    // This is the time after powerup after which the DCBM guarantees that writing to settings will be reliable
    // Make sure to re-attempt any settings writes after this time has passed
    std::chrono::seconds min_time_before_setting_write_is_safe = std::chrono::minutes(2);
    // When performing regular syncs (e.g. as part of the livemeasure loop), this is the minimum duration between
    // retries.
    std::chrono::seconds min_time_between_sync_retries = std::chrono::minutes(1);
    // When a sync is successful, advance the deadline for regular syncs by this much:
    std::chrono::seconds deadline_increment_after_sync = std::chrono::hours(24);
};

struct ntp_server_spec {
    const std::string ip_addr_1;
    const int port_1 = 123;
    const std::string ip_addr_2;
    const int port_2 = 123;
    const bool ntp_enabled = !ip_addr_1.empty();
};

class LemDCBMTimeSyncHelper {

public:
    LemDCBMTimeSyncHelper() = delete;

    explicit LemDCBMTimeSyncHelper(ntp_server_spec ntp_spec) :
        LemDCBMTimeSyncHelper(std::move(ntp_spec), timing_config{}) {
    }

    explicit LemDCBMTimeSyncHelper(ntp_server_spec ntp_spec, timing_config tc) :
        timing_constants(tc), ntp_spec(std::move(ntp_spec)), unsafe_period_start_time({}) {
    }

    virtual ~LemDCBMTimeSyncHelper() = default;

    virtual void sync_if_deadline_expired(const HttpClientInterface& httpClient);

    virtual void sync(const HttpClientInterface& httpClient);

    virtual void restart_unsafe_period();

private:
    // CONFIGURATION VARIABLES
    const ntp_server_spec ntp_spec;
    // Timing constants (can be overridden in a special constructor, e.g. during testing)
    const timing_config timing_constants;

    // RUNNING VARIABLES
    // The helper can be accessed by multiple threads, so we use a mutex to protect the data below
    std::recursive_mutex time_sync_state_lock;
    std::chrono::time_point<std::chrono::steady_clock> deadline_for_next_sync;
    std::optional<std::chrono::time_point<std::chrono::steady_clock>> unsafe_period_start_time;
    // True whenever the NTP config is successfully written to the device after min_time_before_setting_write_is_safe
    // has passed
    bool dcbm_ntp_settings_saved = false;

    // sync_is_too_early is set if this is done earlier than MIN_TIME_BEFORE_SETTING_WRITE_IS_SAFE after init
    void set_ntp_settings_on_device(const HttpClientInterface& httpClient);

    // sync_is_too_early is set if this is done earlier than MIN_TIME_BEFORE_SETTING_WRITE_IS_SAFE after init
    void sync_system_time(const HttpClientInterface& httpClient);

    std::string generate_dcbm_ntp_config();
    [[nodiscard]] bool is_setting_write_safe() const;
};

} // namespace module::main

#endif // EVEREST_CORE_MODULE_LEM_DCBM_TIME_SYNC_HELPER_H
