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

namespace {
void start_eebus_grpc_api(const std::filesystem::path& binary_path, int port, const std::filesystem::path& cert_file,
                          const std::filesystem::path& key_file) {
    try {
        std::vector<std::string> args;
        constexpr int num_args = 6;
        args.reserve(num_args);
        args.emplace_back("-port");
        args.emplace_back(std::to_string(port));
        args.emplace_back("-certificate-path");
        args.emplace_back(cert_file.string());
        args.emplace_back("-private-key-path");
        args.emplace_back(key_file.string());
        everest::run_application::CmdOutput output =
            everest::run_application::run_application(binary_path.string(), args);
        EVLOG_info << "eebus-grpc output: " << output.output;
        EVLOG_info << "eebus-grpc exit code: " << output.exit_code;
    } catch (const std::exception& e) {
        EVLOG_critical << "start_eebus_grpc_api thread caught exception: " << e.what();
    } catch (...) {
        EVLOG_critical << "start_eebus_grpc_api thread caught unknown exception.";
    }
}
} // namespace

EEBUS::~EEBUS() {
    if (this->connection_handler) {
        this->connection_handler->stop();
    }
    if (this->config.manage_eebus_grpc_api_binary && this->eebus_grpc_api_thread_active.exchange(false)) {
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
        if (config_validator->validate()) {
            this->eebus_grpc_api_thread_active.store(true);
            this->eebus_grpc_api_thread =
                std::thread(start_eebus_grpc_api, config_validator->get_eebus_grpc_api_binary_path(),
                            config_validator->get_grpc_port(), config_validator->get_certificate_path(),
                            config_validator->get_private_key_path());
        } else {
            EVLOG_critical << "Could not validate config for starting eebus_grpc_api binary";
        }
    }

    this->connection_handler = std::make_unique<EebusConnectionHandler>(config_validator);

    if (!this->connection_handler->initialize_connection()) {
        EVLOG_critical << "Failed to initialize connection to EEBUS service";
        return;
    }

    if (!this->connection_handler->add_lpc_use_case(this->callbacks)) {
        EVLOG_critical << "Failed to add LPC use case";
        return;
    }

    this->connection_handler->subscribe_to_events();
}

void EEBUS::ready() {
    invoke_ready(*p_main);
    if (!this->connection_handler->start_service()) {
        EVLOG_critical << "Failed to start EEBUS service";
        return;
    }
}

} // namespace module
