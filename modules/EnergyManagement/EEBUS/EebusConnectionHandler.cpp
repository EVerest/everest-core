// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EebusConnectionHandler.hpp>

#include <ConfigValidator.hpp>
#include <everest/logging.hpp>
#include <utility>

namespace module {

namespace {
constexpr auto CHANNEL_READY_TIMEOUT = std::chrono::seconds(60);
constexpr auto HEARTBEAT_TIMEOUT_SECONDS = 60;
} // namespace

EebusConnectionHandler::EebusConnectionHandler(std::shared_ptr<ConfigValidator> config) :
    config(std::move(config)), state(State::INIT) {
    if (!this->initialize_connection()) {
        this->m_handler.add_action([this] { this->handle_event(EebusConnectionEvents::DISCONNECTED); });
    }
}

everest::lib::io::event::sync_status EebusConnectionHandler::sync() {
    m_handler.run_once();
    return everest::lib::io::event::sync_status::ok;
}

int EebusConnectionHandler::get_poll_fd() {
    return m_handler.get_poll_fd();
}

EebusConnectionHandler::~EebusConnectionHandler() {
    this->stop();
}

void EebusConnectionHandler::reconnect() {
    EVLOG_info << "Attempting to reconnect to EEBUS gRPC server...";
    if (this->initialize_connection()) {
        this->m_handler.unregister_event_handler(&this->reconnection_timer);
        EVLOG_info << "Reconnected successfully.";
        if (this->use_case_added) {
            this->add_use_case(this->last_use_case, this->last_callbacks);
            this->done_adding_use_case();
        }
    } else {
        EVLOG_warning << "Reconnect attempt failed. Will try again in " << this->config->get_reconnect_delay_s()
                      << " seconds.";
        this->reconnection_timer.set_timeout(std::chrono::seconds(this->config->get_reconnect_delay_s()));
    }
}

void EebusConnectionHandler::reset() {
    if (this->event_reader) {
        this->event_reader->stop();
        this->event_reader.reset();
    }
    if (this->lpc_handler) {
        this->lpc_handler.reset();
    }
    if (this->control_service_stub) {
        this->control_service_stub.reset();
    }
    if (this->control_service_channel) {
        this->control_service_channel.reset();
    }
}

bool EebusConnectionHandler::initialize_connection() {
    if (!this->create_channel_and_stub()) {
        EVLOG_error << "Connection to EEBUS gRPC server failed, stopping.";
        return false;
    }

    if (!this->configure_service()) {
        EVLOG_error << "Failed to configure EEBUS gRPC service, stopping.";
        return false;
    }

    m_handler.add_action([this] { handle_event(EebusConnectionEvents::CONNECTED); });
    return true;
}

bool EebusConnectionHandler::create_channel_and_stub() {
    const auto address = "localhost:" + std::to_string(this->config->get_grpc_port());
    this->control_service_channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    if (!EebusConnectionHandler::wait_for_channel_ready(this->control_service_channel, CHANNEL_READY_TIMEOUT)) {
        return false;
    }
    this->control_service_stub = control_service::ControlService::NewStub(this->control_service_channel);
    return true;
}

bool EebusConnectionHandler::configure_service() {
    control_service::SetConfigRequest set_config_request = control_service::CreateSetConfigRequest(
        this->config->get_eebus_service_port(), this->config->get_vendor_code(), this->config->get_device_brand(),
        this->config->get_device_model(), this->config->get_serial_number(),
        {control_service::DeviceCategory_Enum::DeviceCategory_Enum_E_MOBILITY},
        control_service::DeviceType_Enum::DeviceType_Enum_CHARGING_STATION,
        {control_service::EntityType_Enum::EntityType_Enum_EVSE}, HEARTBEAT_TIMEOUT_SECONDS);
    control_service::EmptyResponse set_config_response;
    auto set_config_status =
        control_service::CallSetConfig(this->control_service_stub, set_config_request, &set_config_response);
    if (!set_config_status.ok()) {
        EVLOG_error << "set_config failed: " << set_config_status.error_message();
        return false;
    }

    control_service::EmptyRequest start_setup_request;
    control_service::EmptyResponse start_setup_response;
    auto start_setup_status =
        control_service::CallStartSetup(this->control_service_stub, start_setup_request, &start_setup_response);
    if (!start_setup_status.ok()) {
        EVLOG_warning << "start_setup failed: " << start_setup_status.error_message();
        // This is not considered a fatal error
    }

    control_service::RegisterRemoteSkiRequest register_ski_request;
    register_ski_request.set_remote_ski(this->config->get_eebus_ems_ski());
    control_service::EmptyResponse register_ski_response;
    auto register_ski_status = control_service::CallRegisterRemoteSki(this->control_service_stub, register_ski_request,
                                                                      &register_ski_response);
    if (!register_ski_status.ok()) {
        EVLOG_error << "register_remote_ski failed: " << register_ski_status.error_message();
        return false;
    }

    return true;
}

void EebusConnectionHandler::start_service() {
    control_service::EmptyRequest request;
    control_service::EmptyResponse response;
    auto status = control_service::CallStartService(this->control_service_stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "start_service failed: " << status.error_message();
        m_handler.add_action([this] { handle_event(EebusConnectionEvents::DISCONNECTED); });
    }

    if (lpc_handler) {
        this->lpc_handler->start();
    }

    m_handler.add_action([this] { handle_event(EebusConnectionEvents::STARTED); });
}

bool EebusConnectionHandler::add_use_case(eebus::EEBusUseCase use_case, const eebus::EEBusCallbacks& callbacks) {
    this->last_use_case = use_case;
    this->last_callbacks = callbacks;
    this->use_case_added = true;

    switch (use_case) {
    case eebus::EEBusUseCase::LPC:
        this->lpc_handler = std::make_unique<LpcUseCaseHandler>(this->config, callbacks);

        control_service::UseCase use_case_info = LpcUseCaseHandler::get_use_case_info();
        common_types::EntityAddress entity_address = common_types::CreateEntityAddress({1});
        control_service::AddUseCaseRequest request =
            control_service::CreateAddUseCaseRequest(&entity_address, &use_case_info);
        control_service::AddUseCaseResponse response;
        auto status = control_service::CallAddUseCase(this->control_service_stub, request, &response);
        std::ignore = request.release_entity_address();
        std::ignore = request.release_use_case();
        if (!status.ok() || response.endpoint().empty()) {
            EVLOG_error << "add_use_case failed: " << status.error_message();
            return false;
        }

        auto lpc_channel = grpc::CreateChannel(response.endpoint(), grpc::InsecureChannelCredentials());
        if (!EebusConnectionHandler::wait_for_channel_ready(lpc_channel, CHANNEL_READY_TIMEOUT)) {
            EVLOG_error << "Connection to LPC use case gRPC server failed.";
            return false;
        }

        this->lpc_handler->set_stub(cs_lpc::ControllableSystemLPCControl::NewStub(lpc_channel));
        this->lpc_handler->configure_use_case();
        this->event_reader =
            std::make_unique<UseCaseEventReader>(this->control_service_stub, [this](const auto& event) {
                m_handler.add_action([this, event] { this->lpc_handler->handle_event(event); });
            });

        common_types::EntityAddress entity_address_for_event_reader = common_types::CreateEntityAddress({1});
        this->event_reader->start(entity_address_for_event_reader, use_case_info);
        break;
    }

    return true;
}

void EebusConnectionHandler::handle_event(EebusConnectionEvents event) {
    switch (event) {
    case EebusConnectionEvents::CONNECTED:
        if (State::INIT == state || State::DISCONNECTED == state) {
            state = State::ADDING_USE_CASES;
        }
        break;
    case EebusConnectionEvents::DISCONNECTED:
        this->state = State::DISCONNECTED;
        this->reset();
        EVLOG_info << "Disconnected from EEBUS gRPC service. Will try to reconnect in "
                   << this->config->get_reconnect_delay_s() << " seconds.";
        this->reconnection_timer.set_timeout(std::chrono::seconds(this->config->get_reconnect_delay_s()));
        this->m_handler.register_event_handler(&this->reconnection_timer, [this](auto&) { this->reconnect(); });
        break;
    case EebusConnectionEvents::STARTED:
        if (State::READY_TO_START == state) {
            state = State::RUNNING;
            this->state_machine_timer.set_timeout(std::chrono::seconds(1));
            this->m_handler.register_event_handler(&state_machine_timer, [this](auto&) {
                if (this->lpc_handler) {
                    this->lpc_handler->run_state_machine();
                }
            });
        }
        break;
    case EebusConnectionEvents::DONE_ADDING_USE_CASES:
        if (State::ADDING_USE_CASES == state) {
            state = State::READY_TO_START;
            m_handler.add_action([this] { start_service(); });
        }
        break;
    }
}

void EebusConnectionHandler::done_adding_use_case() {
    m_handler.add_action([this] { this->handle_event(EebusConnectionEvents::DONE_ADDING_USE_CASES); });
}

void EebusConnectionHandler::stop() {
    if (this->event_reader) {
        this->event_reader->stop();
    }
}

bool EebusConnectionHandler::wait_for_channel_ready(const std::shared_ptr<grpc::Channel>& channel,
                                                    std::chrono::milliseconds timeout) {
    auto deadline = std::chrono::system_clock::now() + timeout;
    grpc_connectivity_state state = channel->GetState(true);
    while (state != GRPC_CHANNEL_READY) {
        if (!channel->WaitForStateChange(state, deadline)) {
            // timeout or channel is shutting down
            EVLOG_error << "Channel is not ready after timeout.";
            return false;
        }
        state = channel->GetState(true);
    }
    return true;
}

} // namespace module
