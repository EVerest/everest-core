// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_URI_HPP
#define OCPP_WEBSOCKET_URI_HPP

#include <cstdint>
#include <string>
#include <string_view>

#include <websocketpp_utils/uri.hpp>

namespace ocpp {

using ev_uri = ocpp::uri;
class Uri {
public:
    Uri() = default;

    // clang-format off
    /// \brief parse_and_validate parses the \p uri and checks
    ///   1. the general validity of it and
    ///   2. if optional scheme fits to given \p security_profile
    ///
    /// \param uri  The whole URI with optional scheme and \p chargepoint_id as last segment (as backward-compatibility).
    /// \param chargepoint_id  The identifier unique to the CSMS.
    /// \param security_profile The security-profile.
    /// \returns Uri
    /// \throws std::invalid_argument for several checks
    // clang-format on
    static Uri parse_and_validate(std::string uri, std::string chargepoint_id, int security_profile);

    /// \brief set_secure defines if the connection is done via TLS
    ///
    /// \param secure true: connect via TLS; false: connect as plaintext
    void set_secure(bool secure) {
        this->secure = secure;
    }

    std::string get_hostname() {
        return this->host;
    }
    std::string get_chargepoint_id() {
        return this->chargepoint_id;
    }

    std::string get_path() {
        return this->path_without_chargepoint_id;
    }

    uint16_t get_port() const {
        return this->port;
    }

    std::string string() {
        auto uri = get_websocketpp_uri();
        return uri.str();
    }

    ev_uri get_websocketpp_uri() { // FIXME: wrap needed `websocketpp:uri` functionality inside `Uri`
        return ev_uri(this->secure, this->host, this->port,
                      this->path_without_chargepoint_id /* is normalized with ending slash */ + this->chargepoint_id);
    }

private:
    Uri(bool secure, const std::string& host, uint16_t port, const std::string& path_without_chargepoint_id,
        const std::string& chargepoint_id) :
        secure(secure),
        host(host),
        port(port),
        path_without_chargepoint_id(path_without_chargepoint_id),
        chargepoint_id(chargepoint_id) {
    }

    bool secure = false;
    std::string host;
    uint16_t port = 0;
    std::string path_without_chargepoint_id;
    std::string chargepoint_id;
};

} // namespace ocpp

#endif // OCPP_WEBSOCKET_URI_HPP */
