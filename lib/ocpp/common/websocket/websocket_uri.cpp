// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/types.hpp>
#include <ocpp/common/websocket/websocket_uri.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <stdexcept>
#include <string>
#include <string_view>

namespace ocpp {

auto path_split_last_segment(std::string path) {
    struct result {
        std::string path_without_last_segment;
        std::string last_segment;
    };

    if (path == "/") {
        return result{"/", ""};
    }
    auto pos_last_slash = path.rfind("/");

    if (pos_last_slash == path.length() - 1) {
        path.pop_back();
        pos_last_slash -= 1;
    }

    auto path_without_last_segment = std::string(path.substr(0, pos_last_slash + 1));
    auto last_segment = std::string(path.substr(pos_last_slash + 1));

    return result{path_without_last_segment, last_segment};
}

Uri Uri::parse_and_validate(std::string uri, std::string chargepoint_id, int security_profile) {
    if (uri.empty()) {
        throw std::invalid_argument("`uri`-parameter must not be empty");
    }
    if (chargepoint_id.empty()) {
        throw std::invalid_argument("`chargepoint_id`-parameter must not be empty");
    }

    // workaround for required schema in `websocketpp::uri()`
    bool scheme_added_workaround = false;
    if (uri.find("://") == std::string::npos) {
        scheme_added_workaround = true;
        uri = "ws://" + uri;
    }

    auto uri_temp = ev_uri(uri);
    if (!uri_temp.get_valid()) {
        throw std::invalid_argument("given `uri` is invalid");
    }

    if (!scheme_added_workaround) {
        switch (security_profile) { // `switch` to lint for unused enum-values
        case security::SecurityProfile::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION:
        case security::SecurityProfile::UNSECURED_TRANSPORT_WITH_BASIC_AUTHENTICATION:
            if (uri_temp.get_secure()) {
                throw std::invalid_argument(
                    "secure schema 'ws://' in URI does not fit with insecure security-profile = " +
                    std::to_string(security_profile));
            }
            break;
        case security::SecurityProfile::TLS_WITH_BASIC_AUTHENTICATION:
        case security::SecurityProfile::TLS_WITH_CLIENT_SIDE_CERTIFICATES:
            if (!uri_temp.get_secure()) {
                throw std::invalid_argument(
                    "insecure schema 'ws://' in URI does not fit with secure security-profile = " +
                    std::to_string(security_profile));
            }
            break;
        default:
            throw std::invalid_argument("`security_profile`-parameter has unknown value = " +
                                        std::to_string(security_profile));
        }
    }

    auto path = uri_temp.get_resource();

    // backwards-compatibility: remove chargepoint-ID from URI, if given as last path-segment
    const auto [path_without_base, base] = path_split_last_segment(path);
    if (base == chargepoint_id) {
        path = path_without_base;
    }

    return Uri(uri_temp.get_secure(), uri_temp.get_host(), uri_temp.get_port(), path, chargepoint_id);
}

} // namespace ocpp
