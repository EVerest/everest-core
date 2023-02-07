// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include "server.hpp"

#include <cstring>

#include <fstream>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>

#include <fmt/core.h>

#include <libwebsockets.h>

#include <everest/logging.hpp>

// FIXME (aw): naming
class WebsocketSession {
public:
    enum class OutputState {
        EMPTY,
        LAST_DATA,
        MORE_DATA,
    };
    class Output {
    public:
        OutputState state{OutputState::EMPTY};
        unsigned char* buffer{nullptr};
        size_t len{0};

        void set_data(const std::string& data) {
            len = data.size();
            d.resize(LWS_PRE + len);
            buffer = d.data() + LWS_PRE;
            memcpy(buffer, data.data(), len);
        }

    private:
        std::vector<unsigned char> d{};
    };

    void push_output_data(std::string data);
    Output pop_output();
    void add_input(const char*, size_t len);
    std::string finish_input();

private:
    std::string input;
    std::queue<std::string> output_queue;
    std::mutex output_mtx;
};

void WebsocketSession::add_input(const char* in, size_t len) {
    input.append(in, len);
}

std::string WebsocketSession::finish_input() {
    auto data = std::move(input);
    input.clear();
    return data;
}

void WebsocketSession::push_output_data(std::string data) {
    const std::lock_guard<std::mutex> lock(output_mtx);
    output_queue.emplace(std::move(data));
}

WebsocketSession::Output WebsocketSession::pop_output() {
    const std::lock_guard<std::mutex> lock(output_mtx);
    WebsocketSession::Output output;
    if (output_queue.empty()) {
        // output is empty by default
        return output;
    }

    output.set_data(output_queue.front());
    output_queue.pop();

    output.state = output_queue.empty() ? OutputState::LAST_DATA : OutputState::MORE_DATA;

    return output;
}

class Server::Impl {
public:
    Impl() {
        memset(&info, 0, sizeof(info));
    }

    void run(const Server::IncomingMessageHandler& handler, const std::string& html_origin, int port);
    void push(const nlohmann::json& msg);

private:
    static const lws_protocol_vhost_options pvo_opt;
    static const lws_protocol_vhost_options pvo;

    static const lws_protocol_vhost_options pvo_mime;
    static lws_http_mount mount;

    lws_context_creation_info info;
    lws_context* context{nullptr};

    std::mutex context_mtx;

    static int callback(struct lws* wsi, lws_callback_reasons reason, void* user, void* in, size_t len);

    void create_session(WebsocketSession* session);
    void destroy_session(WebsocketSession* session);

    IncomingMessageHandler message_in_handler;

    std::set<WebsocketSession*> sessions{};
};

const lws_protocol_vhost_options Server::Impl::pvo_opt = {nullptr, nullptr, "default", "1"};
const lws_protocol_vhost_options Server::Impl::pvo = {nullptr, &pvo_opt, "everest-controller", ""};

const lws_protocol_vhost_options Server::Impl::pvo_mime = {nullptr, nullptr, ".mp4", "application/x-mp4"};

lws_http_mount Server::Impl::mount = {
    .mountpoint = "/",
    .origin = "/",
    .def = "index.html",
    .extra_mimetypes = &pvo_mime,
    .origin_protocol = LWSMPRO_FILE,
    .mountpoint_len = 1,
};

int Server::Impl::callback(struct lws* wsi, lws_callback_reasons reason, void* user, void* in, size_t len) {
    auto session = static_cast<WebsocketSession*>(user);

    auto& instance = *static_cast<Server::Impl*>(lws_get_protocol(wsi)->user);

    if (LWS_CALLBACK_PROTOCOL_INIT == reason) {
    } else if (LWS_CALLBACK_ESTABLISHED == reason) {
        instance.create_session(session);

    } else if (LWS_CALLBACK_SERVER_WRITEABLE == reason) {
        using State = WebsocketSession::OutputState;
        auto output = session->pop_output();

        if (output.state == State::EMPTY) {
            return 0;
        }

        lws_write(wsi, output.buffer, output.len, LWS_WRITE_TEXT);

        if (output.state == State::MORE_DATA) {
            // more to write
            lws_callback_on_writable(wsi);
        }

    } else if (LWS_CALLBACK_RECEIVE == reason) {
        session->add_input(static_cast<char*>(in), len);

        if (!lws_is_final_fragment(wsi)) {
            return 0;
        }

        auto input = session->finish_input();

        const auto& retval = instance.message_in_handler(input);
        // FIXME (aw): this is blocking - do we want that?
        if (!retval.is_null()) {
            session->push_output_data(retval.dump());

            lws_callback_on_writable(wsi);
        }

        return 0;
    } else if (LWS_CALLBACK_CLOSED == reason) {
        instance.destroy_session(session);
    } else {
        return lws_callback_http_dummy(wsi, reason, user, in, len);
    }

    return 0;
}

void Server::Impl::create_session(WebsocketSession* session) {
    new (session) WebsocketSession();
    const std::lock_guard<std::mutex> lock(context_mtx);
    sessions.insert(session);
}

void Server::Impl::destroy_session(WebsocketSession* session) {
    {
        const std::lock_guard<std::mutex> lock(context_mtx);
        sessions.erase(session);
    }

    session->~WebsocketSession();
}

void Server::Impl::run(const Server::IncomingMessageHandler& handler, const std::string& html_origin, int port) {
    if (!handler) {
        throw std::runtime_error("Could not run the server with a null incoming message handler");
    }
    this->message_in_handler = handler;

    static struct lws_protocols protocols[] = {
        {"http", lws_callback_http_dummy, 0, 0, 0, NULL, 0},
        {"everest-controller", Server::Impl::callback, sizeof(WebsocketSession), 1024, 0, this, 0},
        LWS_PROTOCOL_LIST_TERM,
    };

    mount.origin = html_origin.c_str();
    info.port = port;
    info.mounts = &mount;
    info.protocols = protocols;
    info.pvo = &pvo;

    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE, [](int level, const char* line) {
        if (level == LLL_NOTICE) {
            EVLOG_debug << line;
        } else {
            // should be LLL_ERR or LLL_WARN
            EVLOG_info << line;
        }
        });

    EVLOG_info << fmt::format("Launching controller service on port {}\n", info.port);

    {
        std::lock_guard<std::mutex> lck(context_mtx);
        context = lws_create_context(&info);
    }

    while (lws_service(context, 0) >= 0) {
    }

    // FIXME (aw): check for errors and log them somehow ...
    {
        std::lock_guard<std::mutex> lck(context_mtx);
        lws_context_destroy(context);
        context = nullptr;
    }
}

void Server::Impl::push(const nlohmann::json& msg) {
    const std::lock_guard<std::mutex> lock(context_mtx);
    if (context == nullptr) {
        // context does not exist, nothing to do
        return;
    }

    for (auto session : sessions) {
        session->push_output_data(std::move(msg.dump()));
    }
    lws_cancel_service(context);
}

Server::Server() : pimpl(std::make_unique<Impl>()) {
}

Server::~Server() = default;

void Server::run(const IncomingMessageHandler& handler, const std::string& html_origin, int port) {
    pimpl->run(handler, html_origin, port);
}

void Server::push(const nlohmann::json& msg) {
    pimpl->push(msg);
}
