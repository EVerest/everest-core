// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVAPI_HPP
#define EVAPI_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/error_history/Interface.hpp>
#include <generated/interfaces/ev_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <sstream>

#include <date/date.h>
#include <date/tz.h>

namespace module {

class LimitDecimalPlaces;

class SessionInfo {
public:
    SessionInfo();

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
    void update_state(const std::string& event);
    void set_start_energy_import_wh(int32_t start_energy_import_wh);
    void set_end_energy_import_wh(int32_t end_energy_import_wh);
    void set_latest_energy_import_wh(int32_t latest_energy_wh);
    void set_start_energy_export_wh(int32_t start_energy_export_wh);
    void set_end_energy_export_wh(int32_t end_energy_export_wh);
    void set_latest_energy_export_wh(int32_t latest_export_energy_wh);
    void set_latest_total_w(double latest_total_w);
    void set_enable_disable_source(const std::string& active_source, const std::string& active_state,
                                   const int active_priority);
    void set_permanent_fault(bool f) {
        permanent_fault = f;
    }

    /// \brief Converts this struct into a serialized json object
    operator std::string();

private:
    std::mutex session_info_mutex;
    int32_t start_energy_import_wh; ///< Energy reading (import) at the beginning of this charging session in Wh
    int32_t end_energy_import_wh;   ///< Energy reading (import) at the end of this charging session in Wh
    int32_t start_energy_export_wh; ///< Energy reading (export) at the beginning of this charging session in Wh
    int32_t end_energy_export_wh;   ///< Energy reading (export) at the end of this charging session in Wh
    std::chrono::time_point<date::utc_clock> start_time_point; ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> end_time_point;   ///< End of the charging session
    double latest_total_w;                                     ///< Latest total power reading in W

    std::string state = "Unknown";

    std::string active_enable_disable_source{"Unspecified"};
    std::string active_enable_disable_state{"Enabled"};
    int active_enable_disable_priority{0};
    bool permanent_fault{false};
};
} // namespace module
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class EvAPI : public Everest::ModuleBase {
public:
    EvAPI() = delete;
    EvAPI(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
          std::vector<std::unique_ptr<ev_managerIntf>> r_ev_manager,
          std::vector<std::unique_ptr<error_historyIntf>> r_error_history, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_ev_manager(std::move(r_ev_manager)),
        r_error_history(std::move(r_error_history)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::vector<std::unique_ptr<ev_managerIntf>> r_ev_manager;
    const std::vector<std::unique_ptr<error_historyIntf>> r_error_history;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    std::vector<std::thread> api_threads;
    bool running = true;

    std::list<std::unique_ptr<SessionInfo>> info;
    std::list<std::string> hw_capabilities_str;
    std::string selected_protocol;

    const std::string api_base = "everest_api/";
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVAPI_HPP
