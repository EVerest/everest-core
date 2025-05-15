// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef RPC_API_HPP
#define RPC_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/error_history/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/external_energy_limits/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <generated/types/json_rpc_api.hpp>
#include <vector>
#include "data/DataStore.hpp"
#include "rpc/RpcHandler.hpp"
#include "server/WebsocketServer.hpp"
#include "RpcApiRequestHandler.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool websocket_enabled;
    int websocket_port;
    bool websocket_tls_enabled;
    bool authentication_required;
    std::string charger_information_file;
    int max_decimal_places_power;
    int max_decimal_places_energy;
    int max_decimal_places_current;
    int max_decimal_places_voltage;
    int max_decimal_places_frequency;
    int max_decimal_places_other;
};

class RpcApi : public Everest::ModuleBase {
public:
    RpcApi() = delete;
    RpcApi(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
           std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager,
           std::vector<std::unique_ptr<error_historyIntf>> r_error_history,
           std::vector<std::unique_ptr<external_energy_limitsIntf>> r_evse_energy_sink, Conf& config) :
        ModuleBase(info),
        p_main(std::move(p_main)),
        r_evse_manager(std::move(r_evse_manager)),
        r_error_history(std::move(r_error_history)),
        r_evse_energy_sink(std::move(r_evse_energy_sink)),
        config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::vector<std::unique_ptr<error_historyIntf>> r_error_history;
    const std::vector<std::unique_ptr<external_energy_limitsIntf>> r_evse_energy_sink;
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
    data::DataStoreCharger data;
    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_handler;
    std::unique_ptr<request_interface::RequestHandlerInterface> m_request_handler;

    void subscribe_evse_manager(const std::unique_ptr<evse_managerIntf>& evse_manager, data::DataStoreEvse& evse_data);
    void meterdata_var_to_datastore(const types::powermeter::Powermeter& powermeter,
                                      data::MeterDataStore& meter_data);
    void hwcaps_var_to_datastore(const types::evse_board_support::HardwareCapabilities& hwcaps,
                                       data::HardwareCapabilitiesStore& hw_caps_data);
    bool check_evse_energy_sink_mapping();
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // RPC_API_HPP
