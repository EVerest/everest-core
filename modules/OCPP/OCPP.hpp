// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_HPP
#define OCPP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/auth_token_provider/Implementation.hpp>
#include <generated/interfaces/auth_token_validator/Implementation.hpp>
#include <generated/interfaces/ocpp/Implementation.hpp>
#include <generated/interfaces/ocpp_1_6_charge_point/Implementation.hpp>
#include <generated/interfaces/ocpp_data_transfer/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/auth/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/evse_security/Interface.hpp>
#include <generated/interfaces/external_energy_limits/Interface.hpp>
#include <generated/interfaces/ocpp_data_transfer/Interface.hpp>
#include <generated/interfaces/reservation/Interface.hpp>
#include <generated/interfaces/system/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <everest/timer.hpp>
#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/charge_point.hpp>
#include <ocpp/v16/types.hpp>
#include <ocpp/v201/ocpp_types.hpp>

using EvseConnectorMap = std::map<int32_t, std::map<int32_t, int32_t>>;
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ChargePointConfigPath;
    std::string UserConfigPath;
    std::string DatabasePath;
    bool EnableExternalWebsocketControl;
    int PublishChargingScheduleIntervalS;
    int PublishChargingScheduleDurationS;
    std::string MessageLogPath;
    int MessageQueueResumeDelay;
};

class OCPP : public Everest::ModuleBase {
public:
    OCPP() = delete;
    OCPP(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
         std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main,
         std::unique_ptr<auth_token_validatorImplBase> p_auth_validator,
         std::unique_ptr<auth_token_providerImplBase> p_auth_provider,
         std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer, std::unique_ptr<ocppImplBase> p_ocpp_generic,
         std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager,
         std::vector<std::unique_ptr<external_energy_limitsIntf>> r_connector_zero_sink,
         std::unique_ptr<reservationIntf> r_reservation, std::unique_ptr<authIntf> r_auth,
         std::unique_ptr<systemIntf> r_system, std::unique_ptr<evse_securityIntf> r_security,
         std::vector<std::unique_ptr<ocpp_data_transferIntf>> r_data_transfer, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        p_auth_validator(std::move(p_auth_validator)),
        p_auth_provider(std::move(p_auth_provider)),
        p_data_transfer(std::move(p_data_transfer)),
        p_ocpp_generic(std::move(p_ocpp_generic)),
        r_evse_manager(std::move(r_evse_manager)),
        r_connector_zero_sink(std::move(r_connector_zero_sink)),
        r_reservation(std::move(r_reservation)),
        r_auth(std::move(r_auth)),
        r_system(std::move(r_system)),
        r_security(std::move(r_security)),
        r_data_transfer(std::move(r_data_transfer)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<ocpp_1_6_charge_pointImplBase> p_main;
    const std::unique_ptr<auth_token_validatorImplBase> p_auth_validator;
    const std::unique_ptr<auth_token_providerImplBase> p_auth_provider;
    const std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer;
    const std::unique_ptr<ocppImplBase> p_ocpp_generic;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::vector<std::unique_ptr<external_energy_limitsIntf>> r_connector_zero_sink;
    const std::unique_ptr<reservationIntf> r_reservation;
    const std::unique_ptr<authIntf> r_auth;
    const std::unique_ptr<systemIntf> r_system;
    const std::unique_ptr<evse_securityIntf> r_security;
    const std::vector<std::unique_ptr<ocpp_data_transferIntf>> r_data_transfer;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<ocpp::v16::ChargePoint> charge_point;
    std::unique_ptr<Everest::SteadyTimer> charging_schedules_timer;
    bool ocpp_stopped = false;

    // Return the OCPP connector id from a pair of EVerest EVSE id and connector id
    int32_t get_ocpp_connector_id(int32_t evse_id, int32_t connector_id);
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
    void set_external_limits(const std::map<int32_t, ocpp::v16::EnhancedChargingSchedule>& charging_schedules);
    void publish_charging_schedules(const std::map<int32_t, ocpp::v16::EnhancedChargingSchedule>& charging_schedules);

    void init_evse_subscriptions(); // initialize subscriptions to all EVSEs provided by r_evse_manager
    void init_evse_connector_map();
    void init_evse_ready_map();
    EvseConnectorMap evse_connector_map; // provides access to OCPP connector id by using EVerests evse and connector id
    std::map<int32_t, int32_t>
        connector_evse_index_map; // provides access to r_evse_manager index by using OCPP connector id
    std::map<int32_t, bool> evse_ready_map;
    std::mutex evse_ready_mutex;
    std::condition_variable evse_ready_cv;
    bool all_evse_ready();

    std::atomic_bool started{false};
    std::mutex session_event_mutex;
    std::map<int32_t, std::queue<types::evse_manager::SessionEvent>> session_event_queue;
    void process_session_event(int32_t evse_id, const types::evse_manager::SessionEvent& session_event);
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
/// \brief Contains information about an error
struct ErrorInfo {
    ocpp::v16::ChargePointErrorCode ocpp_error_code;
    std::optional<std::string> info;
    std::optional<std::string> vendor_id;
    std::optional<std::string> vendor_error_code;
};
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPP_HPP
