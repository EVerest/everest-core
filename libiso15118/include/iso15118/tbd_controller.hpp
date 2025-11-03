// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "config.hpp"
#include <iso15118/d20/config.hpp>
#include <iso15118/d20/control_event.hpp>
#include <iso15118/d20/limits.hpp>
#include <iso15118/io/poll_manager.hpp>
#include <iso15118/io/sdp_server.hpp>
#include <iso15118/message/common_types.hpp>
#include <iso15118/session/feedback.hpp>
#include <iso15118/session/iso.hpp>

namespace iso15118 {

struct TbdConfig {
    config::SSLConfig ssl{config::CertificateBackend::EVEREST_LAYOUT, {}, {}, {}, {}, {}, {}};
    std::string interface_name;
    config::TlsNegotiationStrategy tls_negotiation_strategy{config::TlsNegotiationStrategy::ACCEPT_CLIENT_OFFER};
    bool enable_sdp_server{true};
};

class TbdController {
public:
    TbdController(TbdConfig, session::feedback::Callbacks, d20::EvseSetupConfig);

    void loop();

    void send_control_event(const d20::ControlEvent&);

    void update_authorization_services(const std::vector<message_20::datatypes::Authorization>& services,
                                       bool cert_install_service);
    void update_dc_limits(const d20::DcTransferLimits&);
    void update_energy_modes(const std::vector<message_20::datatypes::ServiceCategory>&);
    void update_ac_limits(const d20::AcTransferLimits&);

    void update_supported_vas_services(const std::vector<uint16_t>& vas_services);

private:
    io::PollManager poll_manager;
    std::unique_ptr<io::SdpServer> sdp_server;

    std::unique_ptr<Session> session;

    // callbacks for sdp server
    void handle_sdp_server_input();

    const TbdConfig config;
    const session::feedback::Callbacks callbacks;

    d20::EvseSetupConfig evse_setup;

    std::string interface_name;

    std::optional<d20::PauseContext> pause_ctx{std::nullopt};
};

} // namespace iso15118
