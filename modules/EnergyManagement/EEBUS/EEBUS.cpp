// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EEBUS.hpp>

#include <filesystem>
#include <string>

#include <everest/logging.hpp>
#include <everest/run_application/run_application.hpp>

#include <ConfigValidator.hpp>
#include <EebusConnectionHandler.hpp>

namespace module {

EEBUS::~EEBUS() {
    if (this->connection_handler) {
        this->connection_handler->stop();
    }
    if (this->config.manage_eebus_grpc_api_binary) {
        this->eebus_grpc_api_thread_active.store(false);
        if (this->eebus_grpc_api_thread.joinable()) {
            this->eebus_grpc_api_thread.join();
        }
    }
}

void EEBUS::init() {
    invoke_init(*p_main);

    // Setup callbacks
    this->callbacks.update_limits_callback = [this](types::energy::ExternalLimits new_limits) {
        this->r_eebus_energy_sink->call_set_external_limits(std::move(new_limits));
    };

    auto config_validator =
        std::make_shared<ConfigValidator>(this->config, this->info.paths.etc, this->info.paths.libexec);
    if (!config_validator->validate()) {
        EVLOG_critical << "EEBUS module configuration is invalid";
        return;
    }

    if (this->config.manage_eebus_grpc_api_binary) {
        this->eebus_grpc_api_thread_active.store(true);
        this->eebus_grpc_api_thread = std::thread(
            &EEBUS::start_eebus_grpc_api, this, config_validator->get_eebus_grpc_api_binary_path(),
            config_validator->get_grpc_port(), config_validator->get_certificate_path(),
            config_validator->get_private_key_path(), config_validator->get_restart_delay_s());
    }

    this->connection_handler = std::make_unique<EebusConnectionHandler>(config_validator);
    event_handler.register_event_handler(this->connection_handler.get());

    this->connection_handler->add_use_case(eebus::EEBusUseCase::LPC, this->callbacks);
    this->connection_handler->done_adding_use_case();
}

void EEBUS::start_eebus_grpc_api(const std::filesystem::path& binary_path, int port,
                                 const std::filesystem::path& cert_file, const std::filesystem::path& key_file,
                                 int restart_delay_s) {
    std::vector<std::string> args;
    constexpr int num_args = 6;
    args.reserve(num_args);
    args.emplace_back("-port");
    args.emplace_back(std::to_string(port));
    args.emplace_back("-certificate-path");
    args.emplace_back(cert_file.string());
    args.emplace_back("-private-key-path");
    args.emplace_back(key_file.string());

    while (this->eebus_grpc_api_thread_active) {
        try {
            EVLOG_info << "Starting eebus_grpc_api binary...";
            everest::run_application::CmdOutput output = everest::run_application::run_application(
                binary_path.string(), args, [this](const std::string& output_line) {
                    if (!output_line.empty()) {
                        EVLOG_debug << "eebus-grpc: " << output_line;
                    }
                    if (not this->eebus_grpc_api_thread_active) {
                        return everest::run_application::CmdControl::Terminate;
                    }
                    return everest::run_application::CmdControl::Continue;
                });
            EVLOG_warning << "eebus-grpc process exited with code: " << output.exit_code;
        } catch (const std::exception& e) {
            EVLOG_critical << "start_eebus_grpc_api thread caught exception: " << e.what();
        } catch (...) {
            EVLOG_critical << "start_eebus_grpc_api thread caught unknown exception.";
        }

        if (this->eebus_grpc_api_thread_active) {
            EVLOG_info << "Restarting eebus_grpc_api binary in " << restart_delay_s << " seconds...";
            std::this_thread::sleep_for(std::chrono::seconds(restart_delay_s));
        }
    }
    EVLOG_info << "eebus_grpc_api monitoring thread is stopping.";
}

void EEBUS::ready() {
    invoke_ready(*p_main);

    // Start the event handler in its own thread
    event_handler_thread = std::thread([this]() { event_handler.run(running_flag); });

    // Main application loop
    while (running_flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

} // namespace module
