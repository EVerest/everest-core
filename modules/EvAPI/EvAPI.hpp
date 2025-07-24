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

#include <generated/types/board_support_common.hpp>

namespace module {

class LimitDecimalPlaces;

class EvSessionInfo {
public:
    EvSessionInfo(){};

    void reset();
    void update_event(const types::board_support_common::Event& event);

    /// \brief Converts this struct into a serialized json object
    operator std::string();

private:
    std::mutex session_info_mutex;
    std::optional<types::board_support_common::Event> event;
};
} // namespace module
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class EvAPI : public Everest::ModuleBase {
public:
    EvAPI() = delete;
    EvAPI(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
          std::vector<std::unique_ptr<ev_managerIntf>> r_ev_manager, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_ev_manager(std::move(r_ev_manager)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::vector<std::unique_ptr<ev_managerIntf>> r_ev_manager;
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

    std::list<std::unique_ptr<EvSessionInfo>> info;

    const std::string api_base = "everest_api/";
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVAPI_HPP
