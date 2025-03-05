// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EebusConnectionHandler.hpp>

#include <ConfigValidator.hpp>
#include <everest/logging.hpp>

namespace module {

namespace {
constexpr auto CHANNEL_READY_TIMEOUT = std::chrono::seconds(60);
constexpr auto HEARTBEAT_TIMEOUT_SECONDS = 60;
} // namespace

EebusConnectionHandler::EebusConnectionHandler(std::shared_ptr<ConfigValidator> config) : config(std::move(config)) {
}

EebusConnectionHandler::~EebusConnectionHandler() {
    this->stop();
}

bool EebusConnectionHandler::initialize_connection() {
    const auto address = "localhost:" + std::to_string(this->config->get_grpc_port());
    this->control_service_channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    if (!EebusConnectionHandler::wait_for_channel_ready(this->control_service_channel, CHANNEL_READY_TIMEOUT)) {
        EVLOG_error << "Connection to EEBUS gRPC server failed.";
        return false;
    }
    this->control_service_stub = control_service::ControlService::NewStub(this->control_service_channel);

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
        return false;
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

bool EebusConnectionHandler::start_service() {
    control_service::EmptyRequest request;
    control_service::EmptyResponse response;
    auto status = control_service::CallStartService(this->control_service_stub, request, &response);
    if (!status.ok()) {
        EVLOG_error << "start_service failed: " << status.error_message();
        return false;
    }

    this->lpc_handler->start();

    return true;
}

bool EebusConnectionHandler::add_lpc_use_case(const eebus::EEBusCallbacks& callbacks) {
    this->lpc_handler = std::make_unique<LpcUseCaseHandler>(this->config, callbacks);

    control_service::UseCase use_case = LpcUseCaseHandler::get_use_case_info();
    common_types::EntityAddress entity_address = common_types::CreateEntityAddress({1});
    control_service::AddUseCaseRequest request = control_service::CreateAddUseCaseRequest(&entity_address, &use_case);
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

    return true;
}

void EebusConnectionHandler::subscribe_to_events() {
    this->event_reader = std::make_unique<UseCaseEventReader>(
        this->control_service_stub, [this](const auto& event) { this->lpc_handler->handle_event(event); });

    common_types::EntityAddress entity_address = common_types::CreateEntityAddress({1});
    control_service::UseCase use_case = LpcUseCaseHandler::get_use_case_info();
    this->event_reader->start(entity_address, use_case);
}

void EebusConnectionHandler::stop() {
    if (this->event_reader) {
        this->event_reader->stop();
    }
    if (this->lpc_handler) {
        this->lpc_handler->stop();
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
