// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EEBUS.hpp"

#include <filesystem>
#include <string>

// everest framework
#include <everest/logging.hpp>

// everest core deps
#include <grpcpp/grpcpp.h>

#include <everest/run_application/run_application.hpp>

// module internal
#include <helper.hpp>

constexpr int eebus_retry_interval = 100; // ms
constexpr int eebus_one_minute = 60;
constexpr int grpc_control_service_channel_port = 50051;

namespace module {

namespace {
void start_eebus_grpc_api(const std::filesystem::path& binary_path, int port, const std::filesystem::path& cert_file,
                          const std::filesystem::path& key_file) {
    std::vector<std::string> args;
    constexpr int num_args = 6;
    args.reserve(num_args);
    args.emplace_back("-port");
    args.emplace_back(std::to_string(port));
    args.emplace_back("-certificate-path");
    args.emplace_back(cert_file.string());
    args.emplace_back("-private-key-path");
    args.emplace_back(key_file.string());
    everest::run_application::CmdOutput output = everest::run_application::run_application(binary_path.string(), args);
    EVLOG_error << "eebus-grpc output: " << output.output;
    EVLOG_error << "eebus-grpc exit code: " << output.exit_code;
}

bool wait_for_channel_ready(const std::shared_ptr<grpc::Channel>& channel, std::chrono::milliseconds timeout) {
    const std::chrono::milliseconds retry_interval = std::chrono::milliseconds(eebus_retry_interval);

    const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    grpc_connectivity_state channel_state = channel->GetState(true);

    while (channel_state != GRPC_CHANNEL_READY && std::chrono::system_clock::now() - start < timeout) {
        EVLOG_debug << "Channel is not ready, retrying...";
        std::this_thread::sleep_for(retry_interval);
        channel_state = channel->GetState(true);
    }
    switch (channel_state) {
    case GRPC_CHANNEL_IDLE:
        EVLOG_error << "Channel is still in idle, but timeout is reached";
        return false;
    case GRPC_CHANNEL_CONNECTING:
        EVLOG_error << "Channel is still connecting, but timeout is reached";
        return false;
    case GRPC_CHANNEL_READY:
        // no need to log, as this is the expected state
        return true;
    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        EVLOG_error << "Channel is in transient failure, but timeout is reached";
        return false;
    case GRPC_CHANNEL_SHUTDOWN:
        EVLOG_error << "Channel is shutdown and timeout is reached";
        return false;
    }
    return false;
}

} // namespace

EEBUS::~EEBUS() {
    if (this->config.manage_eebus_grpc_api_binary and this->eebus_grpc_api_thread_active.exchange(false)) {
        this->eebus_grpc_api_thread.join();
    }
}

void EEBUS::init() {
    invoke_init(*p_main);
    this->failed = false;

    this->config_validator =
        std::make_unique<ConfigValidator>(this->config, this->info.paths.etc, this->info.paths.libexec);
    if (!this->config_validator->validate()) {
        EVLOG_error << "EEBUS module configuration is invalid";
        this->failed = true;
        return;
    }

    if (this->config.manage_eebus_grpc_api_binary) {
        this->eebus_grpc_api_thread_active.store(true);
        this->eebus_grpc_api_thread =
            std::thread(start_eebus_grpc_api, this->config_validator->get_eebus_grpc_api_binary_path(),
                        this->config.control_service_rpc_port, this->config_validator->get_certificate_path(),
                        this->config_validator->get_private_key_path());
    }

    // Setup callbacks
    this->callbacks.update_limits_callback = [this](types::energy::ExternalLimits new_limits) {
        this->r_eebus_energy_sink->call_set_external_limits(std::move(new_limits));
    };

    // Start control service
    std::string control_service_grpc_channel = "localhost:" + std::to_string(grpc_control_service_channel_port);
    const std::shared_ptr<grpc::Channel> channel =
        grpc::CreateChannel(control_service_grpc_channel, grpc::InsecureChannelCredentials());
    this->failed = !wait_for_channel_ready(channel, std::chrono::seconds(eebus_one_minute));
    if (this->failed) {
        EVLOG_error << "control service channel is not ready";
        return;
    }
    this->control_service_stub = control_service::ControlService::NewStub(channel);

    this->cs_calls = std::make_unique<grpc_calls::ControlServiceCalls>(this->control_service_stub);
    this->cs_calls->call_set_config(
        this->config_validator->get_control_service_port(), this->config_validator->get_vendor_code(),
        this->config_validator->get_device_brand(), this->config_validator->get_device_model(),
        this->config_validator->get_serial_number());
    this->cs_calls->call_start_setup();
    this->cs_calls->call_register_remote_ski(this->config.eebus_ems_ski);
    std::string endpoint = this->cs_calls->call_add_use_case(control_service::CreateUseCase(
        control_service::UseCase_ActorType_Enum::UseCase_ActorType_Enum_ControllableSystem,
        control_service::UseCase_NameType_Enum::UseCase_NameType_Enum_limitationOfPowerConsumption));

    // Open channel for LPC use case
    std::shared_ptr<grpc::Channel> channel2 = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
    this->failed = !wait_for_channel_ready(channel2, std::chrono::seconds(eebus_one_minute));
    if (this->failed) {
        EVLOG_error << "use case channel is not ready";
        return;
    }
    this->cs_lpc_stub = cs_lpc::ControllableSystemLPCControl::NewStub(channel2);

    // Setup defaults for LPC use case
    this->cs_lpc_calls = std::make_unique<grpc_calls::ControllableSystemLPCControlCalls>(this->cs_lpc_stub);
    this->cs_lpc_calls->call_set_consumption_nominal_max(this->config.max_nominal_power);
    this->cs_lpc_calls->call_set_consumption_limit(this->config.failsafe_control_limit);
    this->cs_lpc_calls->call_set_failsafe_consumption_active_power_limit(this->config.failsafe_control_limit);
    this->cs_lpc_calls->call_set_failsafe_duration_minimum();

    this->cs_calls->subscribe_use_case_events(this->callbacks, this->cs_lpc_stub);

    if (this->failed and this->eebus_grpc_api_thread_active.exchange(false)) {
        EVLOG_error << "EEBUS module failed to initialize";
        this->eebus_grpc_api_thread.join();
        return;
    }
}

void EEBUS::ready() {
    invoke_ready(*p_main);

    this->cs_calls->call_start_service();

    if (this->config.manage_eebus_grpc_api_binary and this->eebus_grpc_api_thread_active.exchange(false)) {
        this->eebus_grpc_api_thread.join();
    }
}

} // namespace module
