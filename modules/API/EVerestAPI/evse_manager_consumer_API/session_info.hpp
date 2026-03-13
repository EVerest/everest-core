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

/// \brief Session and transaction information for EVSE
class SessionInfo {
public:
    using PublishCallback = std::function<void(everest::lib::API::V1_0::types::evse_manager::SessionInfo)>;
    SessionInfo();

    void set_publish_callback(PublishCallback cb);

    void update_state(const types::evse_manager::SessionEvent& session_event);
    void update_powermeter(const types::powermeter::Powermeter& powermeter);
    void update_selected_protocol(const std::string& protocol);

private:
    PublishCallback publish_cb;
    /// \brief External API representation
    everest::lib::API::V1_0::types::evse_manager::SessionInfo ext;

    bool start_energy_export_wh_was_set{
        false}; ///< Indicate if start export energy value (optional) has been received or not
    bool end_energy_export_wh_was_set{
        false}; ///< Indicate if end export energy value (optional) has been received or not
    bool transaction_running{false};

    int32_t start_energy_import_wh; ///< Energy reading (import) at the beginning of this charging session in Wh
    int32_t end_energy_import_wh;   ///< Energy reading (import) at the end of this charging session in Wh
    int32_t start_energy_export_wh; ///< Energy reading (export) at the beginning of this charging session in Wh
    int32_t end_energy_export_wh;   ///< Energy reading (export) at the end of this charging session in Wh
    std::chrono::time_point<date::utc_clock> session_start_time_point;     ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> session_end_time_point;       ///< End of the charging session
    std::chrono::time_point<date::utc_clock> transaction_start_time_point; ///< Start of the transaction
    std::chrono::time_point<date::utc_clock> transaction_end_time_point;   ///< End of the transaction

    void handle_session_started(const types::evse_manager::SessionEvent& session_event);
    void handle_session_finished(const types::evse_manager::SessionEvent& session_event);
    void handle_transaction_started(const types::evse_manager::SessionEvent& session_event);
    void handle_transaction_finished(const types::evse_manager::SessionEvent& session_event);
    void set_latest_energy_import_wh(int32_t latest_energy_wh_import);
    void set_latest_energy_export_wh(int32_t latest_export_energy_wh);
    bool is_session_running();
};

} // namespace module
