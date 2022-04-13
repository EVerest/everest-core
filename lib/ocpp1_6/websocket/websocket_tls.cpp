// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/websocket/websocket_tls.hpp>

namespace ocpp1_6 {

WebsocketTLS::WebsocketTLS(std::shared_ptr<ChargePointConfiguration> configuration) :
    WebsocketBase(configuration), reconnect_timer(nullptr) {
    this->reconnect_interval_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::seconds(configuration->getWebsocketReconnectInterval()))
                                      .count();
}

bool WebsocketTLS::connect() {
    if (!this->initialized()) {
        return false;
    }
    this->uri = this->configuration->getCentralSystemURI();
    EVLOG(info) << "Connecting to uri: " << this->uri;

    EVLOG(info) << "Connecting TLS websocket, hostname: " << this->get_hostname(uri);
    this->wss_client.clear_access_channels(websocketpp::log::alevel::all);
    this->wss_client.clear_error_channels(websocketpp::log::elevel::all);
    this->wss_client.init_asio();
    this->wss_client.start_perpetual();

    this->wss_client.set_tls_init_handler(websocketpp::lib::bind(
        &WebsocketTLS::on_tls_init, this, this->get_hostname(this->uri), websocketpp::lib::placeholders::_1));

    std::string authentication_header = "";
    auto authorization_key = this->configuration->getAuthorizationKey();
    if (authorization_key != boost::none) {
        EVLOG(debug) << "AuthorizationKey present, encoding authentication header";
        std::string auth_header = this->configuration->getChargePointId() + ":" + authorization_key.value();
        authentication_header = std::string("Basic ") + websocketpp::base64_encode(auth_header);
    }

    websocket_thread.reset(new websocketpp::lib::thread(&tls_client::run, &this->wss_client));

    this->reconnect_callback = [this, authentication_header](const websocketpp::lib::error_code& ec) {
        EVLOG(info) << "Reconnecting TLS websocket...";
        {
            std::lock_guard<std::mutex> lk(this->reconnect_mutex);
            if (this->reconnect_timer) {
                this->reconnect_timer.get()->cancel();
            }
            this->reconnect_timer = nullptr;
        }
        this->connect_tls(authentication_header);
    };

    this->connect_tls(authentication_header);
    return true;
}

void WebsocketTLS::disconnect() {
    if (!this->initialized()) {
        EVLOG(error) << "Cannot disconnect a websocket that was not initialized";
        return;
    }
    this->shutting_down = true; // FIXME(kai): this makes the websocket inoperable after a disconnect, however this
                                // might not be a bad thing.
    if (this->reconnect_timer) {
        this->reconnect_timer.get()->cancel();
    }
    EVLOG(info) << "Disconnecting TLS websocket...";

    this->close_tls(websocketpp::close::status::normal, "");
}

bool WebsocketTLS::send(const std::string& message) {
    if (!this->initialized()) {
        EVLOG(error) << "Could not send message because websocket is not properly initialized.";
        return false;
    }

    websocketpp::lib::error_code ec;

    this->wss_client.send(this->handle, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        EVLOG(error) << "Error sending message over TLS websocket: " << ec.message();

        this->reconnect(ec);
        EVLOG(info) << "(TLS) Called reconnect()";
        return false;
    }

    EVLOG(debug) << "Sent message over TLS websocket: " << message;

    return true;
}

void WebsocketTLS::reconnect(std::error_code reason) {
    if (this->shutting_down) {
        EVLOG(info) << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    // TODO(kai): notify message queue that connection is down and a reconnect is imminent?
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        if (!this->reconnect_timer) {
            EVLOG(info) << "Reconnecting in: " << this->reconnect_interval_ms << "ms";
            this->reconnect_timer = this->wss_client.set_timer(this->reconnect_interval_ms, this->reconnect_callback);
        } else {
            EVLOG(debug) << "Reconnect timer already running";
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

// TLS
bool WebsocketTLS::verify_certificate(std::string hostname, bool preverified,
                                      boost::asio::ssl::verify_context& context) {
    // FIXME(kai): this does not verify anything at the moment!

    EVLOG(critical) << "Faking certificate verification, always returning true";

    return true;
}
tls_context WebsocketTLS::on_tls_init(std::string hostname, websocketpp::connection_hdl hdl) {
    tls_context context = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        // FIXME(kai): choose reasonable defaults, they can probably be stricter than this set of options!
        // it is recommended to only accept TLSv1.2+
        context->set_options(boost::asio::ssl::context::default_workarounds | //
                             boost::asio::ssl::context::no_sslv2 |            //
                             boost::asio::ssl::context::no_sslv3 |            //
                             boost::asio::ssl::context::no_tlsv1 |            //
                             boost::asio::ssl::context::no_tlsv1_1 |          //
                             boost::asio::ssl::context::single_dh_use);

        EVLOG(debug) << "List of ciphers that will be accepted by this TLS connection: "
                     << this->configuration->getSupportedCiphers();

        // FIXME(kai): the following only applies to TSLv1.2, we should support TLSv1.3 here as well
        // FIXME(kai): use SSL_CTX_set_ciphersuites for TLSv1.3
        auto set_cipher_list_ret =
            SSL_CTX_set_cipher_list(context->native_handle(), this->configuration->getSupportedCiphers().c_str());
        if (set_cipher_list_ret != 1) {
            EVLOG(critical) << "SSL_CTX_set_cipher_list return value: " << set_cipher_list_ret;
            throw std::runtime_error("Could not set TLSv1.2 cipher list");
        }

        context->set_verify_mode(boost::asio::ssl::verify_peer);
        context->set_verify_callback(websocketpp::lib::bind(&WebsocketTLS::verify_certificate, this, hostname,
                                                            websocketpp::lib::placeholders::_1,
                                                            websocketpp::lib::placeholders::_2));
        context->set_default_verify_paths();
    } catch (std::exception& e) {
        EVLOG(error) << "Error on TLS init: " << e.what();
        throw std::runtime_error("Could not properly initialize TLS connection.");
    }
    return context;
}
void WebsocketTLS::connect_tls(std::string authorization_header) {
    EVLOG(info) << "Connecting to TLS websocket at: " << this->uri;
    websocketpp::lib::error_code ec;

    tls_client::connection_ptr con = this->wss_client.get_connection(this->uri, ec);

    if (ec) {
        EVLOG(error) << "Connection initialization error for TLS websocket: " << ec.message();
        return;
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

    con->add_subprotocol("ocpp1.6");

    if (!authorization_header.empty()) {
        EVLOG(debug) << "Authorization header provided, appending to HTTP header: " << authorization_header;
        con->append_header("Authorization", authorization_header);
    }

    this->wss_client.connect(con);
}
void WebsocketTLS::on_open_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    EVLOG(info) << "Connected to TLS websocket successfully. Executing connected callback";
    this->connected_callback();
}
void WebsocketTLS::on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg) {
    if (!this->initialized()) {
        EVLOG(error) << "Message received but TLS websocket has not been correctly initialized. Discarding message.";
        return;
    }
    try {
        auto message = msg->get_payload();
        this->message_callback(message);
    } catch (websocketpp::exception const& e) {
        EVLOG(error) << "TLS websocket exception on receiving message: " << e.what();
    }
}
void WebsocketTLS::on_close_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    EVLOG(info) << "Closed TLS websocket connection with code: " << error_code << " ("
                << websocketpp::close::status::get_string(con->get_remote_close_code())
                << "), reason: " << con->get_remote_close_reason();
    this->reconnect(error_code);
}
void WebsocketTLS::on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl) {
    tls_client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    EVLOG(error) << "Failed to connect to TLS websocket server " << con->get_response_header("Server")
                 << ", code: " << error_code.value() << ", reason: " << error_code.message();
    this->reconnect(error_code);
}
void WebsocketTLS::close_tls(websocketpp::close::status::value code, const std::string& reason) {
    EVLOG(info) << "Closing TLS websocket.";
    websocketpp::lib::error_code ec;

    this->wss_client.stop_perpetual();
    this->wss_client.close(this->handle, code, reason, ec);
    if (ec) {
        EVLOG(error) << "Error initiating close of TLS websocket: " << ec.message();
    }

    EVLOG(info) << "Closed TLS websocket successfully.";
}

} // namespace ocpp1_6
