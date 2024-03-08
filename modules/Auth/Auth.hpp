// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef AUTH_HPP
#define AUTH_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/auth/Implementation.hpp>
#include <generated/interfaces/reservation/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/auth_token_provider/Interface.hpp>
#include <generated/interfaces/auth_token_validator/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <AuthHandler.hpp>
#include <memory>

using namespace types::evse_manager;
using namespace types::authorization;
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string selection_algorithm;
    int connection_timeout;
    std::string master_pass_group_id;
    bool prioritize_authorization_over_stopping_transaction;
    bool ignore_connector_faults;
};

class Auth : public Everest::ModuleBase {
public:
    Auth() = delete;
    Auth(const ModuleInfo& info, std::unique_ptr<authImplBase> p_main,
         std::unique_ptr<reservationImplBase> p_reservation,
         std::vector<std::unique_ptr<auth_token_providerIntf>> r_token_provider,
         std::vector<std::unique_ptr<auth_token_validatorIntf>> r_token_validator,
         std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, Conf& config) :
        ModuleBase(info),
        p_main(std::move(p_main)),
        p_reservation(std::move(p_reservation)),
        r_token_provider(std::move(r_token_provider)),
        r_token_validator(std::move(r_token_validator)),
        r_evse_manager(std::move(r_evse_manager)),
        config(config){};

    const std::unique_ptr<authImplBase> p_main;
    const std::unique_ptr<reservationImplBase> p_reservation;
    const std::vector<std::unique_ptr<auth_token_providerIntf>> r_token_provider;
    const std::vector<std::unique_ptr<auth_token_validatorIntf>> r_token_validator;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<AuthHandler> auth_handler;

    /**
     * @brief Set the connection timeout for the auth handler
     *
     * @param connection_timeout timeout in seconds
     */
    void set_connection_timeout(int& connection_timeout);

    /**
     * @brief Set the master pass group id for the auth handler
     *
     * @param master_pass_group_id master pass group id
     */
    void set_master_pass_group_id(const std::string& master_pass_group_id);
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

#endif // AUTH_HPP
