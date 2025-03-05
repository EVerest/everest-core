// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP
#define MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP

#include <memory>

#include <LpcUseCaseHandler.hpp>
#include <control_service/control_service.grpc-ext.pb.h>
#include <grpcpp/grpcpp.h>

#include <EebusCallbacks.hpp>
#include <include/UseCaseEventReader.hpp>

namespace module {

class EebusConnectionHandler {
public:
    explicit EebusConnectionHandler(std::shared_ptr<ConfigValidator> config);
    ~EebusConnectionHandler();
    EebusConnectionHandler(const EebusConnectionHandler&) = delete;
    EebusConnectionHandler& operator=(const EebusConnectionHandler&) = delete;
    EebusConnectionHandler(EebusConnectionHandler&&) = default;
    EebusConnectionHandler& operator=(EebusConnectionHandler&&) = default;

    bool initialize_connection();
    bool start_service();
    bool add_lpc_use_case(const eebus::EEBusCallbacks& callbacks);
    void subscribe_to_events();
    void stop();

private:
    static bool wait_for_channel_ready(const std::shared_ptr<grpc::Channel>& channel,
                                       std::chrono::milliseconds timeout);

    std::shared_ptr<ConfigValidator> config;
    std::unique_ptr<LpcUseCaseHandler> lpc_handler;

    std::unique_ptr<UseCaseEventReader> event_reader;

    std::shared_ptr<grpc::Channel> control_service_channel;
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub;
};

} // namespace module

#endif // MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP
