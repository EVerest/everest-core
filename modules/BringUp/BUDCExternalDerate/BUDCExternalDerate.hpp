// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef BUDCEXTERNAL_DERATE_HPP
#define BUDCEXTERNAL_DERATE_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/dc_external_derate/Interface.hpp>
#include <generated/interfaces/generic_error/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class BUDCExternalDerate : public Everest::ModuleBase {
public:
    BUDCExternalDerate() = delete;
    BUDCExternalDerate(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
                       std::unique_ptr<dc_external_derateIntf> r_derate, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_derate(std::move(r_derate)), config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<dc_external_derateIntf> r_derate;
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
    std::string max_export_current_A{"20.0"};
    std::string max_import_current_A{"0.0"};
    std::string max_export_power_W{"20000.0"};
    std::string max_import_power_W{"0.0"};

    double plug_temp_C{0.};
    std::mutex data_mutex;
    //
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // BUDCEXTERNAL_DERATE_HPP
