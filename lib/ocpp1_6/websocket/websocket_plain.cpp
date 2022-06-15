// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/websocket/websocket_plain.hpp>

namespace ocpp1_6 {

WebsocketPlain::WebsocketPlain(std::shared_ptr<ChargePointConfiguration> configuration) :
    WebsocketBase(configuration), reconnect_timer(nullptr) {
    this->reconnect_interval_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::seconds(configuration->getWebsocketReconnectInterval()))
                                      .count();
}

bool WebsocketPlain::connect() {
    if (!this->initialized()) {
        return false;
    }
    auto uri = this->configuration->getCentralSystemURI();

    EVLOG_info << "Connecting plain websocket";
    this->ws_client.clear_access_channels(websocketpp::log::alevel::all);
    this->ws_client.clear_error_channels(websocketpp::log::elevel::all);
    this->ws_client.init_asio();
    this->ws_client.start_perpetual();
    this->uri = uri;

    websocket_thread.reset(new websocketpp::lib::thread(&client::run, &this->ws_client));

    this->reconnect_callback = [this](const websocketpp::lib::error_code& ec) {
        EVLOG_info << "Reconnecting plain websocket...";
        {
            std::lock_guard<std::mutex> lk(this->reconnect_mutex);
            if (this->reconnect_timer) {
                this->reconnect_timer.get()->cancel();
            }
            this->reconnect_timer = nullptr;
        }
        this->connect_plain();
    };

    this->connect_plain();
    return true;
}

void WebsocketPlain::disconnect() {
    if (!this->initialized()) {
        EVLOG_error << "Cannot disconnect a websocket that was not initialized";
        return;
    }
    this->shutting_down = true; // FIXME(kai): this makes the websocket inoperable after a disconnect, however this
                                // might not be a bad thing.
    if (this->reconnect_timer) {
        this->reconnect_timer.get()->cancel();
    }

    EVLOG_info << "Disconnecting plain websocket...";
    this->close_plain(websocketpp::close::status::normal, "");
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

        this->reconnect(ec);
        EVLOG_info << "(plain) Called reconnect()";
        return false;
    }

    EVLOG_debug << "Sent message over plain websocket: " << message;

    return true;
}

void WebsocketPlain::reconnect(std::error_code reason) {
    if (this->shutting_down) {
        EVLOG_info << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    // TODO(kai): notify message queue that connection is down and a reconnect is imminent?
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        if (!this->reconnect_timer) {
            EVLOG_info << "Reconnecting in: " << this->reconnect_interval_ms << "ms";

            this->reconnect_timer = this->ws_client.set_timer(this->reconnect_interval_ms, this->reconnect_callback);
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

void WebsocketPlain::connect_plain() {
    EVLOG_info << "Connecting to plain websocket at: " << this->uri;
    websocketpp::lib::error_code ec;

    client::connection_ptr con = this->ws_client.get_connection(this->uri, ec);

    if (ec) {
        EVLOG_error << "Connection initialization error for plain websocket: " << ec.message();
        return;
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

    con->add_subprotocol("ocpp1.6");

    this->ws_client.connect(con);
}

void WebsocketPlain::on_open_plain(client* c, websocketpp::connection_hdl hdl) {
    EVLOG_info << "Connected to plain websocket successfully. Executing connected callback";
    this->connected_callback();
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
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    EVLOG_info << "Closed plain websocket connection with code: " << error_code << " ("
                << websocketpp::close::status::get_string(con->get_remote_close_code())
                << "), reason: " << con->get_remote_close_reason();
    this->reconnect(error_code);
}

void WebsocketPlain::on_fail_plain(client* c, websocketpp::connection_hdl hdl) {
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    auto error_code = con->get_ec();
    EVLOG_error << "Failed to connect to plain websocket server " << con->get_response_header("Server")
                 << ", code: " << error_code.value() << ", reason: " << error_code.message();
    this->reconnect(error_code);
}

void WebsocketPlain::close_plain(websocketpp::close::status::value code, const std::string& reason) {
    EVLOG_info << "Closing plain websocket.";
    websocketpp::lib::error_code ec;

    this->ws_client.stop_perpetual();
    this->ws_client.close(this->handle, code, reason, ec);
    if (ec) {
        EVLOG_error << "Error initiating close of plain websocket: " << ec.message();
    }

    EVLOG_info << "Closed plain websocket successfully.";
}

} // namespace ocpp1_6
