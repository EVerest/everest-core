// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/pki_handler.hpp>
#include <ocpp/common/websocket/websocket_tls.hpp>

namespace ocpp {

WebsocketTLS::WebsocketTLS(const WebsocketConnectionOptions& connection_options,
                           std::shared_ptr<PkiHandler> pki_handler) :
    WebsocketBase(connection_options), pki_handler(pki_handler) {
    this->reconnect_interval_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::seconds(connection_options.reconnect_interval_s))
                                      .count();
    this->client_certificate_timer = std::make_unique<Everest::SteadyTimer>();
}

bool WebsocketTLS::connect(int32_t security_profile, bool try_once) {
    if (!this->initialized()) {
        return false;
    }
    this->uri = this->connection_options.cs_uri.insert(0, "wss://");
    EVLOG_info << "Connecting TLS websocket to uri: " << this->uri << " with profile " << security_profile;
    this->wss_client.clear_access_channels(websocketpp::log::alevel::all);
    this->wss_client.clear_error_channels(websocketpp::log::elevel::all);
    this->wss_client.init_asio();
    this->wss_client.start_perpetual();

    this->wss_client.set_tls_init_handler(websocketpp::lib::bind(&WebsocketTLS::on_tls_init, this,
                                                                 this->get_hostname(this->uri),
                                                                 websocketpp::lib::placeholders::_1, security_profile));

    websocket_thread.reset(new websocketpp::lib::thread(&tls_client::run, &this->wss_client));

    this->reconnect_callback = [this, security_profile, try_once](const websocketpp::lib::error_code& ec) {
        EVLOG_info << "Reconnecting TLS websocket...";

        // close connection before reconnecting
        if (this->m_is_connected) {
            try {
                EVLOG_debug << "Closing websocket connection";
                this->wss_client.close(this->handle, websocketpp::close::status::normal, "");
            } catch (std::exception& e) {
                EVLOG_error << "Error on TLS close: " << e.what();
            }
        }

        {
            std::lock_guard<std::mutex> lk(this->reconnect_mutex);
            if (this->reconnect_timer) {
                this->reconnect_timer.get()->cancel();
            }
            this->reconnect_timer = nullptr;
        }
        this->connect_tls(security_profile, try_once);
    };

    this->connect_tls(security_profile, try_once);
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

        this->reconnect(ec, this->reconnect_interval_ms);
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
        if (!this->reconnect_timer) {
            EVLOG_info << "Reconnecting in: " << delay << "ms";
            this->reconnect_timer = this->wss_client.set_timer(delay, this->reconnect_callback);
        } else {
            EVLOG_debug << "Reconnect timer already running";
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

std::string WebsocketTLS::get_hostname(std::string uri) {
    // FIXME(kai): This only works with a very limited subset of hostnames!
    std::string start = "wss://";
    std::string stop = "/";
    std::string port = ":";
    auto hostname_start_pos = start.length();
    auto hostname_end_pos = uri.find_first_of(stop, hostname_start_pos);

    auto hostname_with_port = uri.substr(hostname_start_pos, hostname_end_pos - hostname_start_pos);
    auto port_pos = hostname_with_port.find_first_of(port);
    if (port_pos != std::string::npos) {
        return hostname_with_port.substr(0, port_pos);
    }
    return hostname_with_port;
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
            throw std::runtime_error("Could not set TLSv1.2 cipher list");
        }

        rc = SSL_CTX_set_ciphersuites(context->native_handle(), this->connection_options.supported_ciphers_13.c_str());
        if (rc != 1) {
            EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
            std::shared_ptr<X509Certificate> cert = this->pki_handler->getClientCertificate();
            if (cert == nullptr) {
                throw std::runtime_error(
                    "Connecting with security profile 3 but no client side certificate is present or valid");
            }
            SSL_CTX_use_certificate(context->native_handle(), cert->x509);
        }

        context->set_verify_mode(boost::asio::ssl::verify_peer);
        rc = SSL_CTX_load_verify_locations(context->native_handle(),
                                           this->pki_handler->getFile(CS_ROOT_CA_FILE).c_str(), NULL);
        rc = SSL_CTX_set_default_verify_paths(context->native_handle());
        if (rc != 1) {
            EVLOG_error << "Could not load CA verify locations, error: " << ERR_error_string(ERR_get_error(), NULL);
            throw std::runtime_error("Could not load CA verify locations");
        }

    } catch (std::exception& e) {
        EVLOG_error << "Error on TLS init: " << e.what();
        throw std::runtime_error("Could not properly initialize TLS connection.");
    }
    return context;
}
void WebsocketTLS::connect_tls(int32_t security_profile, bool try_once) {
    websocketpp::lib::error_code ec;

    tls_client::connection_ptr con = this->wss_client.get_connection(this->uri, ec);

    if (ec) {
        EVLOG_error << "Connection initialization error for TLS websocket: " << ec.message();
    }

    if (security_profile == 2) {
        EVLOG_debug << "Connecting with security profile: 2";
        boost::optional<std::string> authorization_header = this->getAuthorizationHeader();
        if (authorization_header != boost::none) {
            con->append_header("Authorization", authorization_header.get());
        } else {
            throw std::runtime_error("No authorization key provided when connecting with security profile 2 or 3.");
        }
    } else if (security_profile == 3) {
        EVLOG_debug << "Connecting with security profile: 3";
    } else {
        throw std::runtime_error("Can not connect with TLS websocket with security profile not being 2 or 3.");
    }

    this->handle = con->get_handle();

    con->set_open_handler(websocketpp::lib::bind(&WebsocketTLS::on_open_tls, this, &this->wss_client,
                                                 websocketpp::lib::placeholders::_1, security_profile));
    con->set_fail_handler(websocketpp::lib::bind(&WebsocketTLS::on_fail_tls, this, &this->wss_client,
                                                 websocketpp::lib::placeholders::_1, try_once));
    con->set_close_handler(websocketpp::lib::bind(&WebsocketTLS::on_close_tls, this, &this->wss_client,
                                                  websocketpp::lib::placeholders::_1));
    con->set_message_handler(websocketpp::lib::bind(
        &WebsocketTLS::on_message_tls, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    con->add_subprotocol(conversions::ocpp_protocol_version_to_string(this->connection_options.ocpp_version));

    this->wss_client.connect(con);
}
void WebsocketTLS::on_open_tls(tls_client* c, websocketpp::connection_hdl hdl, int32_t security_profile) {
    EVLOG_info << "Connected to TLS websocket successfully";
    this->m_is_connected = true;
    this->connection_options.security_profile = security_profile;
    if (security_profile == 3) {
        this->client_certificate_timer->interval(
            [this]() {
                EVLOG_debug << "Checking expiry date of client certificate";
                int daysLeft = this->pki_handler->getDaysUntilClientCertificateExpires();
                if (daysLeft < 30) {
                    EVLOG_info << "Certificate is invalid in " << daysLeft
                               << " days. Requesting new certificate with certificate signing request";
                    this->sign_certificate_callback();
                }
            },
            std::chrono::hours(24));
    }
    this->set_websocket_ping_interval(this->connection_options.ping_interval_s);
    this->connected_callback(this->connection_options.security_profile);
}
void WebsocketTLS::on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg) {
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
    this->m_is_connected = false;
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();

    EVLOG_info << "Closed TLS websocket connection with code: " << error_code << " ("
               << websocketpp::close::status::get_string(con->get_remote_close_code())
               << "), reason: " << con->get_remote_close_reason();
    // dont reconnect on normal close
    if (con->get_remote_close_code() != websocketpp::close::status::service_restart) {
        this->reconnect(error_code, this->reconnect_interval_ms);
    }
    this->disconnected_callback();
}
void WebsocketTLS::on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl, bool try_once) {
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    auto transport_ec = con->get_transport_ec();
    EVLOG_error << "Failed to connect to TLS websocket server " << con->get_response_header("Server")
                << ", code: " << error_code.value() << ", reason: " << error_code.message()
                << ", response code: " << con->get_response_code();
    EVLOG_error << "Failed to connect to TLS websocket server "
                << ", code: " << transport_ec.value() << ", reason: " << transport_ec.message()
                << ", category: " << transport_ec.category().name();
    EVLOG_error << "Close code: " << con->get_local_close_code() << ", close reason: " << con->get_local_close_reason();

    // TODO(piet): Trigger SecurityEvent in case InvalidCentralSystemCertificate

    // move fallback ca to /certs if it exists
    if (boost::filesystem::exists(CS_ROOT_CA_FILE_BACKUP_FILE)) {
        EVLOG_debug << "Connection with new CA was not successful - Including fallback CA";
        boost::filesystem::path new_path =
            CS_ROOT_CA_FILE_BACKUP_FILE.parent_path() / CS_ROOT_CA_FILE_BACKUP_FILE.filename();
        std::rename(CS_ROOT_CA_FILE_BACKUP_FILE.c_str(), new_path.c_str());
    }

    if (!try_once) {
        this->reconnect(error_code, this->reconnect_interval_ms);
    } else {
        this->disconnected_callback();
    }
}

void WebsocketTLS::close(websocketpp::close::status::value code, const std::string& reason) {

    EVLOG_info << "Closing TLS websocket.";

    websocketpp::lib::error_code ec;

    this->wss_client.stop_perpetual();
    this->wss_client.close(this->handle, code, reason, ec);

    if (ec) {
        EVLOG_error << "Error initiating close of TLS websocket: " << ec.message();
    }
    EVLOG_info << "Closed TLS websocket successfully.";
}

void WebsocketTLS::ping() {
    auto con = this->wss_client.get_con_from_hdl(this->handle);
    websocketpp::lib::error_code error_code;
    con->ping(this->connection_options.ping_payload, error_code);
}

} // namespace ocpp
