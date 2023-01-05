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
#include <generated/interfaces/auth_token_provider/Implementation.hpp>
#include <generated/interfaces/auth_token_validator/Implementation.hpp>
#include <generated/interfaces/ocpp_1_6_charge_point/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/auth/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/reservation/Interface.hpp>
#include <generated/interfaces/system/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <filesystem>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <everest/timer.hpp>
#include <mutex>
#include <ocpp/v16/charge_point.hpp>
#include <ocpp/common/schemas.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v16/types.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string OcppMainPath;
    std::string ChargePointConfigPath;
    std::string UserConfigPath;
    std::string DatabasePath;
    bool EnableExternalWebsocketControl;
    int PublishChargingScheduleIntervalS;
    int PublishChargingScheduleDurationS;
    std::string MessageLogPath;
};

class OCPP : public Everest::ModuleBase {
public:
    OCPP() = delete;
    OCPP(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
         std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main,
         std::unique_ptr<auth_token_validatorImplBase> p_auth_validator,
         std::unique_ptr<auth_token_providerImplBase> p_auth_provider,
         std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, std::unique_ptr<reservationIntf> r_reservation,
         std::unique_ptr<authIntf> r_auth, std::unique_ptr<systemIntf> r_system, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        p_auth_validator(std::move(p_auth_validator)),
        p_auth_provider(std::move(p_auth_provider)),
        r_evse_manager(std::move(r_evse_manager)),
        r_reservation(std::move(r_reservation)),
        r_auth(std::move(r_auth)),
        r_system(std::move(r_system)),
        config(config){};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main;
    const std::unique_ptr<auth_token_validatorImplBase> p_auth_validator;
    const std::unique_ptr<auth_token_providerImplBase> p_auth_provider;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::unique_ptr<reservationIntf> r_reservation;
    const std::unique_ptr<authIntf> r_auth;
    const std::unique_ptr<systemIntf> r_system;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<ocpp::v16::ChargePoint> charge_point;
    std::unique_ptr<Everest::SteadyTimer> charging_schedules_timer;
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
    void publish_charging_schedules();
    std::thread upload_diagnostics_thread;
    std::thread upload_logs_thread;
    std::thread update_firmware_thread;
    std::thread signed_update_firmware_thread;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPP_HPP
