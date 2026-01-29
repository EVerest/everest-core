// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP
#define MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP

#include <memory>

#include <LpcUseCaseHandler.hpp>
#include <control_service/control_service.grpc-ext.pb.h>
#include <grpcpp/grpcpp.h>

#include <EebusCallbacks.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/fd_event_sync_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <include/UseCaseEventReader.hpp>

namespace module {

namespace eebus {
enum class EEBusUseCase {
    LPC
};
} // namespace eebus

enum class EebusConnectionEvents {
    CONNECTED,
    DISCONNECTED,
    DONE_ADDING_USE_CASES,
    STARTED
};

enum class State {
    INIT,
    CONNECTED,
    DISCONNECTED,
    ADDING_USE_CASES,
    READY_TO_START,
    RUNNING
};

class EebusConnectionHandler : public everest::lib::io::event::fd_event_sync_interface {
public:
    explicit EebusConnectionHandler(std::shared_ptr<ConfigValidator> config);
    ~EebusConnectionHandler() override;
    EebusConnectionHandler(const EebusConnectionHandler&) = delete;
    EebusConnectionHandler& operator=(const EebusConnectionHandler&) = delete;
    EebusConnectionHandler(EebusConnectionHandler&&) = delete;
    EebusConnectionHandler& operator=(EebusConnectionHandler&&) = delete;

    everest::lib::io::event::sync_status sync() override;
    int get_poll_fd() override;

    bool add_use_case(eebus::EEBusUseCase use_case, const eebus::EEBusCallbacks& callbacks);
    void done_adding_use_case();
    void stop();

private:
    void start_service();
    void handle_event(EebusConnectionEvents event);
    bool initialize_connection();
    bool create_channel_and_stub();
    bool configure_service();
    void reconnect();
    void reset();
    static bool wait_for_channel_ready(const std::shared_ptr<grpc::Channel>& channel,
                                       std::chrono::milliseconds timeout);

    std::shared_ptr<ConfigValidator> config;
    std::unique_ptr<LpcUseCaseHandler> lpc_handler;

    std::unique_ptr<UseCaseEventReader> event_reader;

    std::shared_ptr<grpc::Channel> control_service_channel;
    std::shared_ptr<control_service::ControlService::Stub> control_service_stub;
    everest::lib::io::event::fd_event_handler m_handler;
    State state;

    everest::lib::io::event::timer_fd state_machine_timer;
    everest::lib::io::event::timer_fd reconnection_timer;

    // store last added use case for reconnection
    eebus::EEBusUseCase last_use_case;
    eebus::EEBusCallbacks last_callbacks;
    bool use_case_added{false};
};

} // namespace module

#endif // MODULES_ENERGYMANAGEMENT_EEBUS_INCLUDE_EEBUSCONNECTIONHANDLER_HPP
