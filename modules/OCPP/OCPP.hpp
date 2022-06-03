// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_HPP
#define OCPP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/auth_token_provider/Implementation.hpp>
#include <generated/auth_token_validator/Implementation.hpp>
#include <generated/ocpp_1_6_charge_point/Implementation.hpp>

// headers for required interface implementations
#include <generated/evse_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <mutex>
#include <ocpp1_6/charge_point.hpp>
#include <ocpp1_6/schemas.hpp>
#include <ocpp1_6/types.hpp>
struct Session {
    double energy_Wh_import;
    ocpp1_6::DateTime timestamp;
};
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ChargePointConfigPath;
    std::string DatabasePath;
    std::string SchemasPath;
};

class OCPP : public Everest::ModuleBase {
public:
    OCPP() = delete;
    OCPP(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
         std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main,
         std::unique_ptr<auth_token_validatorImplBase> p_auth_validator,
         std::unique_ptr<auth_token_providerImplBase> p_auth_provider,
         std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        p_auth_validator(std::move(p_auth_validator)),
        p_auth_provider(std::move(p_auth_provider)),
        r_evse_manager(std::move(r_evse_manager)),
        config(config){};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main;
    const std::unique_ptr<auth_token_validatorImplBase> p_auth_validator;
    const std::unique_ptr<auth_token_providerImplBase> p_auth_provider;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    ocpp1_6::ChargePoint* charge_point;
    std::map<int32_t, int32_t> res_conn_map;
    std::map<std::string, ocpp1_6::ReservationStatus> ResStatMap = {
        {std::string("Accepted"), ocpp1_6::ReservationStatus::Accepted},
        {std::string("Faulted"), ocpp1_6::ReservationStatus::Faulted},
        {std::string("Occupied"), ocpp1_6::ReservationStatus::Occupied},
        {std::string("Rejected"), ocpp1_6::ReservationStatus::Rejected},
        {std::string("Unavailable"), ocpp1_6::ReservationStatus::Unavailable}};
    std::map<bool, ocpp1_6::CancelReservationStatus> can_res_stat_map = {
        {true, ocpp1_6::CancelReservationStatus::Accepted}, {false, ocpp1_6::CancelReservationStatus::Rejected}};
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
    void start_session();
    std::thread upload_diagnostics_thread;
    std::thread update_firmware_thread;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPP_HPP
