// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EEBUS_HPP
#define EEBUS_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/external_energy_limits/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

#include <thread>

#include <EebusCallbacks.hpp>
#include <EebusConnectionHandler.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool manage_eebus_grpc_api_binary;
    int eebus_service_port;
    int grpc_port;
    std::string eebus_ems_ski;
    std::string certificate_path;
    std::string private_key_path;
    std::string eebus_grpc_api_binary_path;
    std::string vendor_code;
    std::string device_brand;
    std::string device_model;
    std::string serial_number;
    int failsafe_control_limit_W;
    int max_nominal_power_W;
};

class EEBUS : public Everest::ModuleBase {
public:
    EEBUS() = delete;
    EEBUS(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
          std::unique_ptr<external_energy_limitsIntf> r_eebus_energy_sink, Conf& config) :
        ModuleBase(info),
        p_main(std::move(p_main)),
        r_eebus_energy_sink(std::move(r_eebus_energy_sink)),
        config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<external_energy_limitsIntf> r_eebus_energy_sink;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    ~EEBUS() override;
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
    std::thread eebus_grpc_api_thread;
    std::atomic<bool> eebus_grpc_api_thread_active;

    std::unique_ptr<EebusConnectionHandler> connection_handler;

    eebus::EEBusCallbacks callbacks{};
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EEBUS_HPP
