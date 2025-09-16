// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef BUSYSTEM_HPP
#define BUSYSTEM_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/system/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class BUSystem : public Everest::ModuleBase {
public:
    BUSystem() = delete;
    BUSystem(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main, std::unique_ptr<systemIntf> r_system,
             Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_system(std::move(r_system)), config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<systemIntf> r_system;
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
    std::string last_command;
    std::string firmware_update_status;
    std::string log_status;
    std::string is_reset_allowed;
    std::string boot_reason;
    std::string firmware_update_url;
    std::string new_system_time;
    std::string log_upload_location;
    std::string upload_logs_status;
    std::string update_firmware_response;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // BUSYSTEM_HPP
