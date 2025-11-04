// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef BUOCPP_CONSUMER_HPP
#define BUOCPP_CONSUMER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ocpp/Implementation.hpp>
#include <generated/interfaces/ocpp_data_transfer/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class BUOcppConsumer : public Everest::ModuleBase {
public:
    BUOcppConsumer() = delete;
    BUOcppConsumer(const ModuleInfo& info, std::unique_ptr<ocppImplBase> p_ocpp,
                   std::unique_ptr<ocpp_data_transferImplBase> p_ocpp_data_transfer, Conf& config) :
        ModuleBase(info),
        p_ocpp(std::move(p_ocpp)),
        p_ocpp_data_transfer(std::move(p_ocpp_data_transfer)),
        config(config){};

    const std::unique_ptr<ocppImplBase> p_ocpp;
    const std::unique_ptr<ocpp_data_transferImplBase> p_ocpp_data_transfer;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    void event(const types::ocpp::GetVariableResult& ev);
    void event(const std::string& value, const types::ocpp::SetVariableResult& ev);
    void event_monitor_variable(const std::string& name);
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
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // BUOCPP_CONSUMER_HPP
