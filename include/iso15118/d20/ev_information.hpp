// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/supported_app_protocol.hpp>

namespace iso15118::d20 {

using EVSupportedAppProtocols = std::vector<message_20::SupportedAppProtocol>;

// Holds information about the EV
struct EVInformation {
    EVSupportedAppProtocols ev_supported_app_protocols;
    message_20::SupportedAppProtocol selected_app_protocol;
    std::string evcc_id;
    std::optional<std::string> ev_tls_leaf_cert;
    std::optional<std::string> ev_tls_sub_ca_1_cert;
    std::optional<std::string> ev_tls_sub_ca_2_cert;
    std::optional<std::string> ev_tls_root_cert;
};

} // namespace iso15118::d20
