// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <mutex>
#include <string>

#include <everest_api_types/evse_manager/API.hpp>
#include <generated/types/evse_manager.hpp>
namespace module {

/// \brief Session information for EVSE sessions
class SessionInfo {
public:
    SessionInfo();

    /// \brief Converts the internal session info to the external API representation
    everest::lib::API::V1_0::types::evse_manager::SessionInfo to_external_api();

    bool start_energy_export_wh_was_set{
        false}; ///< Indicate if start export energy value (optional) has been received or not
    bool end_energy_export_wh_was_set{
        false}; ///< Indicate if end export energy value (optional) has been received or not

    void update_state(const types::evse_manager::SessionEvent event);
    void set_start_energy_import_wh(int32_t start_energy_import_wh);
    void set_end_energy_import_wh(int32_t end_energy_import_wh);
    void set_latest_energy_import_wh(int32_t latest_energy_wh);
    void set_start_energy_export_wh(int32_t start_energy_export_wh);
    void set_end_energy_export_wh(int32_t end_energy_export_wh);
    void set_latest_energy_export_wh(int32_t latest_export_energy_wh);
    void set_latest_total_w(double latest_total_w);
    void set_selected_protocol(const std::string& protocol);
    bool is_session_running();

private:
    std::mutex session_info_mutex;

    int32_t start_energy_import_wh; ///< Energy reading (import) at the beginning of this charging session in Wh
    int32_t end_energy_import_wh;   ///< Energy reading (import) at the end of this charging session in Wh
    int32_t start_energy_export_wh; ///< Energy reading (export) at the beginning of this charging session in Wh
    int32_t end_energy_export_wh;   ///< Energy reading (export) at the end of this charging session in Wh
    std::chrono::time_point<date::utc_clock> start_time_point; ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> end_time_point;   ///< End of the charging session
    double latest_total_w;                                     ///< Latest total power reading in W
    std::string selected_protocol;                             /// < Selected charging protocol of the session

    enum class State {
        Unknown,
        Unplugged,
        Disabled,
        Preparing,
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

    void reset();
    bool is_state_charging(const State current_state);
    everest::lib::API::V1_0::types::evse_manager::EvseStateEnum to_external_api(State state);
};

} // namespace module
