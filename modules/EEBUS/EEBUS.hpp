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

// generated
#include <control_service/control_service.grpc-ext.pb.h>
#include <usecases/cs/lpc/service.grpc-ext.pb.h>

// module internal
#include <ConfigValidator.hpp>
#include <UseCaseEventReader.hpp>
#include <grpc_calls/ControlServiceCalls.hpp>
#include <grpc_calls/ControllableSystemLPCControlCalls.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool manage_eebus_grpc_api_binary;
    int control_service_rpc_port;
    std::string eebus_ems_ski;
    std::string certificate_path;
    std::string private_key_path;
    std::string eebus_grpc_api_binary_path;
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
    void set_use_case_event_reader(std::unique_ptr<UseCaseEventReader> reader);
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
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub;
    std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub;
    std::unique_ptr<UseCaseEventReader> reader;
    std::thread eebus_grpc_api_thread;

    std::unique_ptr<grpc_calls::ControlServiceCalls> cs_calls;
    std::unique_ptr<grpc_calls::ControllableSystemLPCControlCalls> cs_lpc_calls;
    std::unique_ptr<ConfigValidator> config_validator;

    bool failed;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EEBUS_HPP
