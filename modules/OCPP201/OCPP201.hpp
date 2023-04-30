// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP201_HPP
#define OCPP201_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/auth_token_provider/Implementation.hpp>
#include <generated/interfaces/auth_token_validator/Implementation.hpp>
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/system/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <ocpp/v201/charge_point.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ChargePointConfigPath;
    std::string MessageLogPath;
    std::string CertsPath;
    std::string DatabasePath;
};

class OCPP201 : public Everest::ModuleBase {
public:
    OCPP201() = delete;
    OCPP201(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
            std::unique_ptr<auth_token_validatorImplBase> p_auth_validator,
            std::unique_ptr<auth_token_providerImplBase> p_auth_provider,
            std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, std::unique_ptr<systemIntf> r_system,
            Conf& config) :
        ModuleBase(info),
        p_main(std::move(p_main)),
        p_auth_validator(std::move(p_auth_validator)),
        p_auth_provider(std::move(p_auth_provider)),
        r_evse_manager(std::move(r_evse_manager)),
        r_system(std::move(r_system)),
        config(config){};

    const Conf& config;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<auth_token_validatorImplBase> p_auth_validator;
    const std::unique_ptr<auth_token_providerImplBase> p_auth_provider;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::unique_ptr<systemIntf> r_system;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<ocpp::v201::ChargePoint> charge_point;

    // holds operational states of EVSE
    std::map<int32_t, ocpp::v201::OperationalStatusEnum> operational_evse_states;
    // holds operational states of Connectors
    std::map<int32_t, ocpp::v201::OperationalStatusEnum> operational_connector_states;
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
    std::filesystem::path ocpp_share_path;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPP201_HPP
