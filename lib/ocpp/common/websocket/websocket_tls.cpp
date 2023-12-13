// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/common/websocket/websocket_tls.hpp>
#include <ocpp/common/websocket/websocket_uri.hpp>

#include <everest/logging.hpp>

#include <memory>
#include <stdexcept>
#include <string>

namespace ocpp {

static std::vector<std::string> get_subject_alt_names(const X509* x509) {
    std::vector<std::string> list;
    GENERAL_NAMES* subject_alt_names =
        static_cast<GENERAL_NAMES*>(X509_get_ext_d2i(x509, NID_subject_alt_name, NULL, NULL));
    if (subject_alt_names == nullptr) {
        return list;
    }
    for (int i = 0; i < sk_GENERAL_NAME_num(subject_alt_names); i++) {
        GENERAL_NAME* gen = sk_GENERAL_NAME_value(subject_alt_names, i);
        if (gen == nullptr) {
            continue;
        }
        if (gen->type == GEN_URI || gen->type == GEN_DNS || gen->type == GEN_EMAIL) {
            ASN1_IA5STRING* asn1_str = gen->d.uniformResourceIdentifier;
            std::string san = std::string(reinterpret_cast<const char*>(ASN1_STRING_get0_data(asn1_str)),
                                          ASN1_STRING_length(asn1_str));
            list.push_back(san);
        } else if (gen->type == GEN_IPADD) {
            unsigned char* ip = gen->d.ip->data;
            if (gen->d.ip->length == 4) { // only support IPv4 for now
                std::stringstream ip_stream;
                ip_stream << static_cast<int>(ip[0]) << '.' << static_cast<int>(ip[1]) << '.' << static_cast<int>(ip[2])
                          << '.' << static_cast<int>(ip[3]);
                list.push_back(ip_stream.str());
            }
        }
    }
    GENERAL_NAMES_free(subject_alt_names);
    return list;
}

// verify that the csms certificate's commonName matches the CSMS FQDN
bool verify_csms_cn(const std::string& hostname, bool preverified, boost::asio::ssl::verify_context& ctx) {

    /*
    FIXME(cc): This does not work, always returns false here
    if (!preverified) {
         EVLOG_error << "Could not verify CSMS server certificate";
         return false;
     }*/

    int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());
    // only check for CSMS server certificate
    if (depth == 0) {
        // Get server certificate
        X509* server_cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

        // Extract CN from csms server's certificate
        X509_NAME* subject_name = X509_get_subject_name(server_cert);
        char common_name[256];
        if (X509_NAME_get_text_by_NID(subject_name, NID_commonName, common_name, sizeof(common_name)) <= 0) {
            EVLOG_error << "Could not extract CN from CSMS server certificate";
            return false;
        }

        auto alt_names = get_subject_alt_names(server_cert);

        // Compare the extracted CN with the expected FQDN
        if (hostname == common_name) {
            EVLOG_info << "FQDN matches CN of server certificate: " << hostname;
            return true;
        }

        // If the CN does not match, go through all alternative names
        for (auto name : alt_names) {
            if (hostname == name) {
                EVLOG_info << "FQDN matches alternative name of server certificate: " << hostname;
                return true;
            }
        }

        EVLOG_info << "FQDN does not match CN or alternative names.";
        return false;
    }

    return true;
}

WebsocketTLS::WebsocketTLS(const WebsocketConnectionOptions& connection_options,
                           std::shared_ptr<EvseSecurity> evse_security) :
    WebsocketBase(), evse_security(evse_security) {

    set_connection_options(connection_options);

    EVLOG_debug << "Initialised WebsocketTLS with URI: " << this->connection_options.csms_uri.string();
}

void WebsocketTLS::set_connection_options(const WebsocketConnectionOptions& connection_options) {
    switch (connection_options.security_profile) { // `switch` used to lint on missing enum-values
    case security::SecurityProfile::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION:
    case security::SecurityProfile::UNSECURED_TRANSPORT_WITH_BASIC_AUTHENTICATION:
        throw std::invalid_argument("`security_profile` is not a TLS-profile");
    case security::SecurityProfile::TLS_WITH_BASIC_AUTHENTICATION:
    case security::SecurityProfile::TLS_WITH_CLIENT_SIDE_CERTIFICATES:
        break;
    default:
        throw std::invalid_argument("unknown `security_profile`, value = " +
                                    std::to_string(connection_options.security_profile));
    }

    set_connection_options_base(connection_options);

    this->connection_options.csms_uri.set_secure(true);
}

bool WebsocketTLS::connect() {
    if (!this->initialized()) {
        return false;
    }

    EVLOG_info << "Connecting TLS websocket to uri: " << this->connection_options.csms_uri.string()
               << " with security-profile " << this->connection_options.security_profile;

    this->wss_client.clear_access_channels(websocketpp::log::alevel::all);
    this->wss_client.clear_error_channels(websocketpp::log::elevel::all);
    this->wss_client.init_asio();
    this->wss_client.start_perpetual();
    websocket_thread.reset(new websocketpp::lib::thread(&tls_client::run, &this->wss_client));

    this->wss_client.set_tls_init_handler(
        websocketpp::lib::bind(&WebsocketTLS::on_tls_init, this, this->connection_options.csms_uri.get_hostname(),
                               websocketpp::lib::placeholders::_1, this->connection_options.security_profile));

    this->reconnect_callback = [this](const websocketpp::lib::error_code& ec) {
        EVLOG_info << "Reconnecting to TLS websocket at uri: " << this->connection_options.csms_uri.string()
                   << " with security profile: " << this->connection_options.security_profile;

        // close connection before reconnecting
        if (this->m_is_connected) {
            try {
                EVLOG_info << "Closing websocket connection before reconnecting";
                this->wss_client.close(this->handle, websocketpp::close::status::normal, "");
            } catch (std::exception& e) {
                EVLOG_error << "Error on TLS close: " << e.what();
            }
        }

        this->cancel_reconnect_timer();
        this->connect_tls();
    };

    this->connect_tls();
    return true;
}

bool WebsocketTLS::send(const std::string& message) {
    if (!this->initialized()) {
        EVLOG_error << "Could not send message because websocket is not properly initialized.";
        return false;
    }

    websocketpp::lib::error_code ec;

    this->wss_client.send(this->handle, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        EVLOG_error << "Error sending message over TLS websocket: " << ec.message();

        this->reconnect(ec, this->get_reconnect_interval());
        EVLOG_info << "(TLS) Called reconnect()";
        return false;
    }

    EVLOG_debug << "Sent message over TLS websocket: " << message;

    return true;
}

void WebsocketTLS::reconnect(std::error_code reason, long delay) {
    if (this->shutting_down) {
        EVLOG_info << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    // TODO(kai): notify message queue that connection is down and a reconnect is imminent?
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        if (this->m_is_connected) {
            try {
                EVLOG_info << "Closing websocket connection before reconnecting";
                this->wss_client.close(this->handle, websocketpp::close::status::normal, "");
            } catch (std::exception& e) {
                EVLOG_error << "Error on plain close: " << e.what();
            }
        }

        if (!this->reconnect_timer) {
            EVLOG_info << "Reconnecting in: " << delay << "ms"
                       << ", attempt: " << this->connection_attempts;
            this->reconnect_timer = this->wss_client.set_timer(delay, this->reconnect_callback);
        } else {
            EVLOG_info << "Reconnect timer already running";
        }
    }

    // TODO(kai): complete error handling, especially making sure that a reconnect is only attempted in reasonable
    // circumstances
    switch (reason.value()) {
    case websocketpp::close::status::force_tcp_drop:
        /* code */
        break;

    default:
        break;
    }

    // TODO: spec-conform reconnect, refer to status codes from:
    // https://github.com/zaphoyd/websocketpp/blob/master/websocketpp/close.hpp
}

tls_context WebsocketTLS::on_tls_init(std::string hostname, websocketpp::connection_hdl hdl, int32_t security_profile) {
    tls_context context = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        // FIXME(kai): choose reasonable defaults, they can probably be stricter than this set of options!
        // it is recommended to only accept TLSv1.2+
        context->set_options(boost::asio::ssl::context::default_workarounds |                                    //
                             boost::asio::ssl::context::no_sslv2 |                                               //
                             boost::asio::ssl::context::no_sslv3 |                                               //
                             boost::asio::ssl::context::no_tlsv1 |                                               //
                             boost::asio::ssl::context::no_tlsv1_1 | boost::asio::ssl::context::no_compression | //
                             boost::asio::ssl::context::single_dh_use);

        EVLOG_debug << "List of ciphers that will be accepted by this TLS connection: "
                    << this->connection_options.supported_ciphers_12 << ":"
                    << this->connection_options.supported_ciphers_13;

        auto rc =
            SSL_CTX_set_cipher_list(context->native_handle(), this->connection_options.supported_ciphers_12.c_str());
        if (rc != 1) {
            EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
            EVLOG_AND_THROW(std::runtime_error("Could not set TLSv1.2 cipher list"));
        }

        rc = SSL_CTX_set_ciphersuites(context->native_handle(), this->connection_options.supported_ciphers_13.c_str());
        if (rc != 1) {
            EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
        }

        if (security_profile == 3) {
            const auto certificate_key_pair =
                this->evse_security->get_key_pair(CertificateSigningUseEnum::ChargingStationCertificate);

            if (certificate_key_pair.has_value() && certificate_key_pair.value().password.has_value()) {
                std::string passwd = certificate_key_pair.value().password.value();
                context->set_password_callback(
                    [passwd](auto max_len, auto purpose) { return passwd.substr(0, max_len); });
            }

            if (!certificate_key_pair.has_value()) {
                EVLOG_AND_THROW(std::runtime_error(
                    "Connecting with security profile 3 but no client side certificate is present or valid"));
            }
            EVLOG_info << "Using certificate: " << certificate_key_pair.value().certificate_path;
            if (SSL_CTX_use_certificate_chain_file(context->native_handle(),
                                                   certificate_key_pair.value().certificate_path.c_str()) != 1) {
                EVLOG_AND_THROW(std::runtime_error("Could not use client certificate file within SSL context"));
            }
            EVLOG_info << "Using key file: " << certificate_key_pair.value().key_path;
            if (SSL_CTX_use_PrivateKey_file(context->native_handle(), certificate_key_pair.value().key_path.c_str(),
                                            SSL_FILETYPE_PEM) != 1) {
                EVLOG_AND_THROW(std::runtime_error("Could not set private key file within SSL context"));
            }
        }

        context->set_verify_mode(boost::asio::ssl::verify_peer);
        if (this->connection_options.verify_csms_common_name) {
            context->set_verify_callback(websocketpp::lib::bind(
                &verify_csms_cn, hostname, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

        } else {
            EVLOG_warning << "Not verifying the CSMS certificates commonName with the Fully Qualified Domain Name "
                             "(FQDN) of the server because it has been explicitly turned off via the configuration!";
        }
        if (this->evse_security->is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
            EVLOG_info << "Loading ca csms bundle to verify server certificate: "
                       << this->evse_security->get_verify_file(ocpp::CaCertificateType::CSMS);
            rc = SSL_CTX_load_verify_locations(
                context->native_handle(), this->evse_security->get_verify_file(ocpp::CaCertificateType::CSMS).c_str(),
                NULL);
        }

        if (rc != 1) {
            EVLOG_error << "Could not load CA verify locations, error: " << ERR_error_string(ERR_get_error(), NULL);
            EVLOG_AND_THROW(std::runtime_error("Could not load CA verify locations"));
        }

        if (this->connection_options.use_ssl_default_verify_paths) {
            rc = SSL_CTX_set_default_verify_paths(context->native_handle());
            if (rc != 1) {
                EVLOG_error << "Could not load default CA verify path, error: "
                            << ERR_error_string(ERR_get_error(), NULL);
                EVLOG_AND_THROW(std::runtime_error("Could not load CA verify locations"));
            }
        }

    } catch (std::exception& e) {
        EVLOG_error << "Error on TLS init: " << e.what();
        EVLOG_AND_THROW(std::runtime_error("Could not properly initialize TLS connection."));
    }
    return context;
}
void WebsocketTLS::connect_tls() {
    websocketpp::lib::error_code ec;

    const tls_client::connection_ptr con = this->wss_client.get_connection(
        std::make_shared<websocketpp::uri>(this->connection_options.csms_uri.get_websocketpp_uri()), ec);

    if (ec) {
        EVLOG_error << "Connection initialization error for TLS websocket: " << ec.message();
    }

    if (this->connection_options.hostName.has_value()) {
        EVLOG_info << "User-Host is set to " << this->connection_options.hostName.value();
        con->append_header("User-Host", this->connection_options.hostName.value());
    }

    if (this->connection_options.security_profile == 2) {
        EVLOG_debug << "Connecting with security profile: 2";
        std::optional<std::string> authorization_header = this->getAuthorizationHeader();
        if (authorization_header != std::nullopt) {
            con->append_header("Authorization", authorization_header.value());
        } else {
            EVLOG_AND_THROW(
                std::runtime_error("No authorization key provided when connecting with security profile 2 or 3."));
        }
    } else if (this->connection_options.security_profile == 3) {
        EVLOG_debug << "Connecting with security profile: 3";
    } else {
        EVLOG_AND_THROW(
            std::runtime_error("Can not connect with TLS websocket with security profile not being 2 or 3."));
    }

    this->handle = con->get_handle();

    con->set_open_handler(websocketpp::lib::bind(&WebsocketTLS::on_open_tls, this, &this->wss_client,
                                                 websocketpp::lib::placeholders::_1));
    con->set_fail_handler(websocketpp::lib::bind(&WebsocketTLS::on_fail_tls, this, &this->wss_client,
                                                 websocketpp::lib::placeholders::_1));
    con->set_close_handler(websocketpp::lib::bind(&WebsocketTLS::on_close_tls, this, &this->wss_client,
                                                  websocketpp::lib::placeholders::_1));
    con->set_message_handler(websocketpp::lib::bind(
        &WebsocketTLS::on_message_tls, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    con->set_pong_timeout(this->connection_options.pong_timeout_s * 1000); // pong timeout in ms
    con->set_pong_timeout_handler(websocketpp::lib::bind(
        &WebsocketTLS::on_pong_timeout, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    con->add_subprotocol(conversions::ocpp_protocol_version_to_string(this->connection_options.ocpp_version));

    this->wss_client.connect(con);
}
void WebsocketTLS::on_open_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    (void)c;                       // tls_client is not used in this function
    EVLOG_info << "OCPP client successfully connected to TLS websocket server";
    this->connection_attempts = 1; // reset connection attempts
    this->m_is_connected = true;
    this->reconnecting = false;
    this->set_websocket_ping_interval(this->connection_options.ping_interval_s);
    this->connected_callback(this->connection_options.security_profile);
}
void WebsocketTLS::on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg) {
    (void)hdl; // connection_hdl is not used in this function
    if (!this->initialized()) {
        EVLOG_error << "Message received but TLS websocket has not been correctly initialized. Discarding message.";
        return;
    }
    try {
        auto message = msg->get_payload();
        this->message_callback(message);
    } catch (websocketpp::exception const& e) {
        EVLOG_error << "TLS websocket exception on receiving message: " << e.what();
    }
}
void WebsocketTLS::on_close_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->m_is_connected = false;
    this->disconnected_callback();
    this->cancel_reconnect_timer();
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();

    EVLOG_info << "Closed TLS websocket connection with code: " << error_code << " ("
               << websocketpp::close::status::get_string(con->get_remote_close_code())
               << "), reason: " << con->get_remote_close_reason();
    // dont reconnect on normal close
    if (con->get_remote_close_code() != websocketpp::close::status::normal) {
        this->reconnect(error_code, this->get_reconnect_interval());
    } else {
        this->closed_callback(con->get_remote_close_code());
    }
}
void WebsocketTLS::on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->m_is_connected = false;
    this->disconnected_callback();
    this->connection_attempts += 1;
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    const auto ec = con->get_ec();
    this->log_on_fail(ec, con->get_transport_ec(), con->get_response_code());

    // TODO(piet): Trigger SecurityEvent in case InvalidCentralSystemCertificate

    // -1 indicates to always attempt to reconnect
    if (this->connection_options.max_connection_attempts == -1 or
        this->connection_attempts <= this->connection_options.max_connection_attempts) {
        this->reconnect(ec, this->get_reconnect_interval());
    } else {
        this->close(websocketpp::close::status::normal, "Connection failed");
    }
}

void WebsocketTLS::close(websocketpp::close::status::value code, const std::string& reason) {

    EVLOG_info << "Closing TLS websocket.";

    websocketpp::lib::error_code ec;
    this->cancel_reconnect_timer();

    this->wss_client.stop_perpetual();
    this->wss_client.close(this->handle, code, reason, ec);

    if (ec) {
        EVLOG_error << "Error initiating close of TLS websocket: " << ec.message();
        // on_close_tls wont be called here so we have to call the closed_callback manually
        this->closed_callback(websocketpp::close::status::abnormal_close);
    } else {
        EVLOG_info << "Closed TLS websocket successfully.";
    }
}

void WebsocketTLS::ping() {
    if (this->m_is_connected) {
        auto con = this->wss_client.get_con_from_hdl(this->handle);
        websocketpp::lib::error_code error_code;
        con->ping(this->connection_options.ping_payload, error_code);
    }
}

} // namespace ocpp
