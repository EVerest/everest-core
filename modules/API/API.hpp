// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef API_HPP
#define API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <date/date.h>
#include <date/tz.h>
#include <memory>
#include <mutex>
#include <sstream>

namespace module {

class SessionInfo {
private:
    std::mutex session_info_mutex;

    std::string state;       ///< Latest state of the EVSE
    int32_t start_energy_wh; ///< Energy reading at the beginning of this charging session in Wh
    int32_t end_energy_wh;   ///< Energy reading at the end of this charging session in Wh
    std::chrono::time_point<date::utc_clock> start_time_point; ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> end_time_point;   ///< End of the charging session
    double latest_total_w;                                     ///< Latest total power reading in W

    bool is_state_charging(const std::string& current_state);

public:
    SessionInfo();

    void reset();
    void update_state(const std::string& event);
    void set_start_energy_wh(int32_t start_energy_wh);
    void set_end_energy_wh(int32_t end_energy_wh);
    void set_latest_energy_wh(int32_t latest_energy_wh);
    void set_latest_total_w(double latest_total_w);

    /// \brief Converts this struct into a serialized json object
    operator std::string();
};
} // namespace module
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class API : public Everest::ModuleBase {
public:
    API() = delete;
    API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
        std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_evse_manager(std::move(r_evse_manager)),
        config(config){};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;

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
    std::thread datetime_thread;
    bool running = true;
    std::vector<std::unique_ptr<SessionInfo>> info;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // API_HPP
