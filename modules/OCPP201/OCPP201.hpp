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
#include <generated/interfaces/ocpp/Implementation.hpp>
#include <generated/interfaces/ocpp_data_transfer/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/auth/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/evse_security/Interface.hpp>
#include <generated/interfaces/ocpp_data_transfer/Interface.hpp>
#include <generated/interfaces/system/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <tuple>

#include <ocpp/v201/charge_point.hpp>

enum class TxStartPoint {
    ParkingBayOccupancy,
    EVConnected,
    Authorized,
    PowerPathClosed,
    EnergyTransfer,
    DataSigned
};

struct TransactionStart {
    int32_t evse_id;
    int32_t connector_id;
    std::string session_id;
    ocpp::DateTime timestamp;
    ocpp::v201::TriggerReasonEnum trigger_reason;
    ocpp::v201::MeterValue meter_start;
    ocpp::v201::IdToken id_token;
    std::optional<ocpp::v201::IdToken> group_id_token;
    std::optional<int32_t> reservation_id;
    std::optional<int32_t> remote_start_id;
    ocpp::v201::ChargingStateEnum charging_state;
};
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ChargePointConfigPath;
    std::string MessageLogPath;
    std::string CertsPath;
    std::string CoreDatabasePath;
    std::string DeviceModelDatabasePath;
    bool EnableExternalWebsocketControl;
    int MessageQueueResumeDelay;
};

class OCPP201 : public Everest::ModuleBase {
public:
    OCPP201() = delete;
    OCPP201(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
            std::unique_ptr<auth_token_validatorImplBase> p_auth_validator,
            std::unique_ptr<auth_token_providerImplBase> p_auth_provider,
            std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer, std::unique_ptr<ocppImplBase> p_ocpp_generic,
            std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, std::unique_ptr<systemIntf> r_system,
            std::unique_ptr<evse_securityIntf> r_security,
            std::vector<std::unique_ptr<ocpp_data_transferIntf>> r_data_transfer, std::unique_ptr<authIntf> r_auth,
            Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        p_auth_validator(std::move(p_auth_validator)),
        p_auth_provider(std::move(p_auth_provider)),
        p_data_transfer(std::move(p_data_transfer)),
        p_ocpp_generic(std::move(p_ocpp_generic)),
        r_evse_manager(std::move(r_evse_manager)),
        r_system(std::move(r_system)),
        r_security(std::move(r_security)),
        r_data_transfer(std::move(r_data_transfer)),
        r_auth(std::move(r_auth)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<auth_token_validatorImplBase> p_auth_validator;
    const std::unique_ptr<auth_token_providerImplBase> p_auth_provider;
    const std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer;
    const std::unique_ptr<ocppImplBase> p_ocpp_generic;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::unique_ptr<systemIntf> r_system;
    const std::unique_ptr<evse_securityIntf> r_security;
    const std::vector<std::unique_ptr<ocpp_data_transferIntf>> r_data_transfer;
    const std::unique_ptr<authIntf> r_auth;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<ocpp::v201::ChargePoint> charge_point;
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
    // track the session started reasons for every EVSE+Connector combination to be able to report correct trigger
    // reason in TransactionStarted event
    std::map<std::pair<int32_t, int32_t>, types::evse_manager::StartSessionReason> session_started_reasons;
    std::map<std::pair<int32_t, int32_t>, std::optional<TransactionStart>> transaction_starts;

    TxStartPoint tx_start_point;

    std::filesystem::path ocpp_share_path;

    // key represents evse_id, value indicates if ready
    std::map<int32_t, bool> evse_ready_map;
    std::mutex evse_ready_mutex;
    std::condition_variable evse_ready_cv;
    void init_evse_ready_map();
    bool all_evse_ready();
    std::map<int32_t, int32_t> get_connector_structure();

    void transaction_start(std::int32_t evseid, std::int32_t connector, const std::string& session_id,
                           const std::optional<std::string>& transaction_id);
    void transaction_end(std::int32_t evseid, std::int32_t connector, const std::string& session_id,
                         const std::optional<std::string>& transaction_id);
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPP201_HPP
