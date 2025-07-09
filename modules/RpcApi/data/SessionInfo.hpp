// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte and Contributors to EVerest

// Description: This file defines the SessionInfoStore class, which is used to manage session information for EV charging sessions.
// It includes methods to update session state, set energy readings, and calculate charged and discharged energy.
// The code of SessionInfoStore class is mostly a copy of the EVerest API module.

#ifndef SESSIONINFO_HPP
#define SESSIONINFO_HPP

// headers for required interface implementations
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/uk_random_delay/Interface.hpp>

// insert your custom include headers here
#include <mutex>
#include <date/date.h>
#include <date/tz.h>

namespace data {

class SessionInfoStore {
public:
    SessionInfoStore();

    struct Error {
        std::string type;
        std::string description;
        std::string severity;
    };

    bool start_energy_export_wh_was_set{
        false}; ///< Indicate if start export energy value (optional) has been received or not
    bool end_energy_export_wh_was_set{
        false}; ///< Indicate if end export energy value (optional) has been received or not

    void reset();
    void update_state(const types::evse_manager::SessionEvent event);
    void set_start_energy_import_wh(int32_t start_energy_import_wh);
    void set_end_energy_import_wh(int32_t end_energy_import_wh);
    void set_latest_energy_import_wh(int32_t latest_energy_wh);
    void set_start_energy_export_wh(int32_t start_energy_export_wh);
    void set_end_energy_export_wh(int32_t end_energy_export_wh);
    void set_latest_energy_export_wh(int32_t latest_export_energy_wh);
    void set_latest_total_w(double latest_total_w);
    void set_uk_random_delay_remaining(const types::uk_random_delay::CountDown& c);
    void set_enable_disable_source(const std::string& active_source, const std::string& active_state,
                                   const int active_priority);
    void set_permanent_fault(bool f) {
        permanent_fault = f;
    }

    float get_charged_energy_wh() const;
    float get_discharged_energy_wh() const;
    std::chrono::seconds get_charging_duration_s() const;

    /// \brief Converts this struct into a serialized json object
    operator std::string();

private:
    mutable std::mutex session_info_mutex;
    int32_t start_energy_import_wh; ///< Energy reading (import) at the beginning of this charging session in Wh
    int32_t end_energy_import_wh;   ///< Energy reading (import) at the end of this charging session in Wh
    int32_t start_energy_export_wh; ///< Energy reading (export) at the beginning of this charging session in Wh
    int32_t end_energy_export_wh;   ///< Energy reading (export) at the end of this charging session in Wh
    types::uk_random_delay::CountDown uk_random_delay_remaining; ///< Remaining time of a UK smart charging regs
                                                                 ///< delay. Set to 0 if no delay is active
    std::chrono::time_point<date::utc_clock> start_time_point;   ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> end_time_point;     ///< End of the charging session
    double latest_total_w;                                       ///< Latest total power reading in W

    enum class State {
        Unknown,
        Unplugged,
        Disabled,
        Preparing,
        Reserved,
        AuthRequired,
        WaitingForEnergy,
        ChargingPausedEV,
        ChargingPausedEVSE,
        Charging,
        AuthTimeout,
        Finished,
        FinishedEVSE,
        FinishedEV
    } state;

    bool is_state_charging(const SessionInfoStore::State current_state);

    std::string active_enable_disable_source{"Unspecified"};
    std::string active_enable_disable_state{"Enabled"};
    int active_enable_disable_priority{0};
    bool permanent_fault{false};
};
} // namespace module

#endif // SESSIONINFO_HPP
