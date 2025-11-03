// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef BUPOWERMETER_HPP
#define BUPOWERMETER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string evse_id;
    std::string transaction_id;
    std::string identification_data;
    std::string tariff_text;
};

class BUPowermeter : public Everest::ModuleBase {
public:
    BUPowermeter() = delete;
    BUPowermeter(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
                 std::unique_ptr<powermeterIntf> r_powermeter, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_powermeter(std::move(r_powermeter)), config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<powermeterIntf> r_powermeter;
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
    std::string last_command_duration;
    std::chrono::time_point<std::chrono::steady_clock> last_event_received;
    std::chrono::time_point<std::chrono::steady_clock> event_received;
    std::string public_key;
    types::powermeter::Powermeter powermeter;
    types::powermeter::TransactionStartResponse tr_start;
    types::powermeter::TransactionStopResponse tr_stop;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // BUPOWERMETER_HPP
