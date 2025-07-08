// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/tbd_controller.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>

#include <iso15118/io/connection_plain.hpp>
#include <iso15118/io/connection_ssl.hpp>
#include <iso15118/session/iso.hpp>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/socket_helper.hpp>

namespace iso15118 {

TbdController::TbdController(TbdConfig config_, session::feedback::Callbacks callbacks_, d20::EvseSetupConfig setup_) :
    config(std::move(config_)),
    callbacks(std::move(callbacks_)),
    evse_setup(std::move(setup_)),
    interface_name(config.interface_name) {

    const auto result_interface_check = io::check_and_update_interface(interface_name);
    if (result_interface_check) {
        logf_info("Using ethernet interface: %s", interface_name.c_str());
    } else {
        throw std::runtime_error("Ethernet interface was not found!");
    }

    if (config.enable_sdp_server) {
        sdp_server = std::make_unique<io::SdpServer>(interface_name);
        poll_manager.register_fd(sdp_server->get_fd(), [this]() { handle_sdp_server_input(); });
    }
}

void TbdController::loop() {
    static constexpr auto POLL_MANAGER_TIMEOUT_MS = 50;

    if (not config.enable_sdp_server) {
        auto connection = std::make_unique<io::ConnectionPlain>(poll_manager, interface_name);
        session =
            std::make_unique<Session>(std::move(connection), d20::SessionConfig(evse_setup), callbacks, pause_ctx);
    }

    auto next_event = get_current_time_point();

    while (true) {
        const auto poll_timeout_ms = get_timeout_ms_until(next_event, POLL_MANAGER_TIMEOUT_MS);
        poll_manager.poll(poll_timeout_ms);

        next_event = offset_time_point_by_ms(get_current_time_point(), POLL_MANAGER_TIMEOUT_MS);

        if (session) {
            const auto next_session_event = session->poll();
            next_event = std::min(next_event, next_session_event);
            if (session->is_finished()) {
                session.reset();

                if (not config.enable_sdp_server) {
                    auto connection = std::make_unique<io::ConnectionPlain>(poll_manager, interface_name);
                    session = std::make_unique<Session>(std::move(connection), d20::SessionConfig(evse_setup),
                                                        callbacks, pause_ctx);
                }
            }
        }
    }
}

void TbdController::send_control_event(const d20::ControlEvent& event) {
    if (session) {
        session->push_control_event(event);
    }
}

void TbdController::update_authorization_services(const std::vector<message_20::datatypes::Authorization>& services,
                                                  bool cert_install_service) {

    evse_setup.enable_certificate_install_service = cert_install_service;

    if (services.empty()) {
        logf_warning("The authorization services are not updated because services are empty!");
        return;
    }
    evse_setup.authorization_services = services;
}

void TbdController::update_dc_limits(const d20::DcTransferLimits& limits) {

    evse_setup.dc_limits = limits;

    if (session) {
        session->push_control_event(limits);
    }
}

void TbdController::update_energy_modes(const std::vector<message_20::datatypes::ServiceCategory>& modes) {
    evse_setup.supported_energy_services = modes;

    if (session) {
        session->push_control_event(modes);
    }
}

void TbdController::update_supported_vas_services(const d20::SupportedVASs& vas_services) {

    evse_setup.supported_vas_services = vas_services;

    if (session) {
        session->push_control_event(vas_services);
    }
}

void TbdController::handle_sdp_server_input() {
    auto request = sdp_server->get_peer_request();

    if (not request) {
        return;
    }

    switch (config.tls_negotiation_strategy) {
    case config::TlsNegotiationStrategy::ACCEPT_CLIENT_OFFER:
        // nothing to change
        break;
    case config::TlsNegotiationStrategy::ENFORCE_TLS:
        request.security = io::v2gtp::Security::TLS;
        break;
    case config::TlsNegotiationStrategy::ENFORCE_NO_TLS:
        request.security = io::v2gtp::Security::NO_TRANSPORT_SECURITY;
        break;
    }

    auto connection = [this](bool secure_connection) -> std::unique_ptr<io::IConnection> {
        if (secure_connection) {
            return std::make_unique<io::ConnectionSSL>(poll_manager, interface_name, config.ssl);
        } else {
            return std::make_unique<io::ConnectionPlain>(poll_manager, interface_name);
        }
    }(request.security == io::v2gtp::Security::TLS);

    const auto ipv6_endpoint = connection->get_public_endpoint();

    session = std::make_unique<Session>(std::move(connection), d20::SessionConfig(evse_setup), callbacks, pause_ctx);

    sdp_server->send_response(request, ipv6_endpoint);
}

} // namespace iso15118
