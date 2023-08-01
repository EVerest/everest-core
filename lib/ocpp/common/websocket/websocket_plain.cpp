// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/websocket/websocket_plain.hpp>

#include <boost/optional/optional.hpp>

namespace ocpp {

WebsocketPlain::WebsocketPlain(const WebsocketConnectionOptions& connection_options) :
    WebsocketBase(connection_options) {
}

bool WebsocketPlain::connect() {
    if (!this->initialized()) {
        return false;
    }
    const auto uri = this->connection_options.cs_uri.insert(0, "ws://");

    EVLOG_info << "Connecting to plain websocket at uri: " << uri
               << " with profile: " << this->connection_options.security_profile;

    this->ws_client.clear_access_channels(websocketpp::log::alevel::all);
    this->ws_client.clear_error_channels(websocketpp::log::elevel::all);
    this->ws_client.init_asio();
    this->ws_client.start_perpetual();
    this->uri = uri;

    websocket_thread.reset(new websocketpp::lib::thread(&client::run, &this->ws_client));

    this->reconnect_callback = [this](const websocketpp::lib::error_code& ec) {
        EVLOG_info << "Reconnecting to plain websocket at uri: " << this->connection_options.cs_uri
                   << " with profile: " << this->connection_options.security_profile;

        // close connection before reconnecting
        if (this->m_is_connected) {
            try {
                EVLOG_info << "Closing websocket connection before reconnecting";
                this->ws_client.close(this->handle, websocketpp::close::status::normal, "");
            } catch (std::exception& e) {
                EVLOG_error << "Error on plain close: " << e.what();
            }
        }

        this->cancel_reconnect_timer();
        this->connect_plain();
    };

    this->connect_plain();
    return true;
}

bool WebsocketPlain::send(const std::string& message) {
    if (!this->initialized()) {
        EVLOG_error << "Could not send message because websocket is not properly initialized.";
        return false;
    }

    websocketpp::lib::error_code ec;

    this->ws_client.send(this->handle, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        EVLOG_error << "Error sending message over plain websocket: " << ec.message();

        this->reconnect(ec, this->get_reconnect_interval());
        EVLOG_info << "(plain) Called reconnect()";
        return false;
    }

    EVLOG_debug << "Sent message over plain websocket: " << message;

    return true;
}

void WebsocketPlain::reconnect(std::error_code reason, long delay) {
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
                this->ws_client.close(this->handle, websocketpp::close::status::normal, "");
            } catch (std::exception& e) {
                EVLOG_error << "Error on plain close: " << e.what();
            }
        }

        if (!this->reconnect_timer) {
            EVLOG_info << "Reconnecting in: " << delay << "ms"
                       << ", attempt: " << this->connection_attempts;
            this->reconnect_timer = this->ws_client.set_timer(delay, this->reconnect_callback);
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

void WebsocketPlain::connect_plain() {

    websocketpp::lib::error_code ec;

    client::connection_ptr con = this->ws_client.get_connection(this->uri, ec);

    if (ec) {
        EVLOG_error << "Connection initialization error for plain websocket: " << ec.message();
    }

    if (this->connection_options.security_profile == 0) {
        EVLOG_debug << "Connecting with security profile: 0";
    } else if (this->connection_options.security_profile == 1) {
        EVLOG_debug << "Connecting with security profile: 1";
        std::optional<std::string> authorization_header = this->getAuthorizationHeader();
        if (authorization_header) {
            con->append_header("Authorization", authorization_header.value());
        } else {
            throw std::runtime_error("No authorization key provided when connecting with security profile: 1");
        }
    } else {
        throw std::runtime_error("Cannot connect with plain websocket with security profile > 1");
    }

    this->handle = con->get_handle();

    con->set_open_handler(websocketpp::lib::bind(&WebsocketPlain::on_open_plain, this, &this->ws_client,
                                                 websocketpp::lib::placeholders::_1));
    con->set_fail_handler(websocketpp::lib::bind(&WebsocketPlain::on_fail_plain, this, &this->ws_client,
                                                 websocketpp::lib::placeholders::_1));
    con->set_close_handler(websocketpp::lib::bind(&WebsocketPlain::on_close_plain, this, &this->ws_client,
                                                  websocketpp::lib::placeholders::_1));
    con->set_message_handler(websocketpp::lib::bind(&WebsocketPlain::on_message_plain, this,
                                                    websocketpp::lib::placeholders::_1,
                                                    websocketpp::lib::placeholders::_2));

    con->add_subprotocol(conversions::ocpp_protocol_version_to_string(this->connection_options.ocpp_version));
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->ws_client.connect(con);
}

void WebsocketPlain::on_open_plain(client* c, websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    (void)c;                       // client is not used in this function
    EVLOG_info << "OCPP client successfully connected to plain websocket server";
    this->connection_attempts = 1; // reset connection attempts
    this->m_is_connected = true;
    this->set_websocket_ping_interval(this->connection_options.ping_interval_s);
    this->connected_callback(this->connection_options.security_profile);
}

void WebsocketPlain::on_message_plain(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    if (!this->initialized()) {
        EVLOG_error << "Message received but plain websocket has not been correctly initialized. Discarding message.";
        return;
    }
    try {
        auto message = msg->get_payload();
        this->message_callback(message);
    } catch (websocketpp::exception const& e) {
        EVLOG_error << "Plain websocket exception on receiving message: " << e.what();
    }
}

void WebsocketPlain::on_close_plain(client* c, websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->m_is_connected = false;
    this->cancel_reconnect_timer();
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    EVLOG_info << "Closed plain websocket connection with code: " << error_code << " ("
               << websocketpp::close::status::get_string(con->get_remote_close_code())
               << "), reason: " << con->get_remote_close_reason();
    // dont reconnect on service restart code
    if (con->get_remote_close_code() != websocketpp::close::status::normal) {
        this->reconnect(error_code, this->get_reconnect_interval());
    } else {
        this->closed_callback();
    }
}

void WebsocketPlain::on_fail_plain(client* c, websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->connection_attempts += 1;
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    const auto ec = con->get_ec();
    this->log_on_fail(ec, con->get_transport_ec(), con->get_response_code());

    // -1 indicates to always attempt to reconnect
    if (this->connection_options.max_connection_attempts == -1 or
        this->connection_attempts < this->connection_options.max_connection_attempts) {
        this->reconnect(ec, this->get_reconnect_interval());
    } else {
        this->close(websocketpp::close::status::normal, "Connection failed");
    }
}

void WebsocketPlain::close(websocketpp::close::status::value code, const std::string& reason) {
    EVLOG_info << "Closing plain websocket.";
    websocketpp::lib::error_code ec;
    this->cancel_reconnect_timer();

    this->ws_client.stop_perpetual();
    this->ws_client.close(this->handle, code, reason, ec);
    if (ec) {
        EVLOG_error << "Error initiating close of plain websocket: " << ec.message();
        // on_close_tls wont be called here so we have to call the closed_callback manually
        this->closed_callback();
    } else {
        EVLOG_info << "Closed plain websocket successfully.";
    }
}

void WebsocketPlain::ping() {
    if (this->m_is_connected) {
        auto con = this->ws_client.get_con_from_hdl(this->handle);
        websocketpp::lib::error_code error_code;
        con->ping(this->connection_options.ping_payload, error_code);
    }
}

} // namespace ocpp
