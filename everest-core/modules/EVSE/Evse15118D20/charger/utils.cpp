// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "utils.hpp"

namespace module::charger {
types::iso15118::AppProtocol convert_app_protocol(const iso15118::message_20::SupportedAppProtocol& app_protocol) {
    types::iso15118::AppProtocol result;
    result.protocol_namespace = app_protocol.protocol_namespace;
    result.priority = app_protocol.priority;
    result.schema_id = app_protocol.schema_id;
    result.version_number_major = app_protocol.version_number_major;
    result.version_number_minor = app_protocol.version_number_minor;
    return result;
}

types::iso15118::EvInformation convert_ev_info(const iso15118::d20::EVInformation& ev_info) {
    types::iso15118::EvInformation result;
    result.evcc_id = ev_info.evcc_id;
    result.selected_protocol = convert_app_protocol(ev_info.selected_app_protocol);
    result.supported_protocols.Protocols.reserve(ev_info.ev_supported_app_protocols.size());
    for (const auto& supported_app : ev_info.ev_supported_app_protocols) {
        result.supported_protocols.Protocols.push_back(convert_app_protocol(supported_app));
    }
    result.tls_leaf_certificate = ev_info.ev_tls_leaf_cert;
    result.tls_sub_ca_1_certificate = ev_info.ev_tls_sub_ca_1_cert;
    result.tls_sub_ca_2_certificate = ev_info.ev_tls_sub_ca_2_cert;
    result.tls_root_certificate = ev_info.ev_tls_root_cert;
    return result;
}
} // namespace module::charger
