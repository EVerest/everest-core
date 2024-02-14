// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <ocpp/common/websocket/websocket_tls_tpm.hpp>

#include <evse_security/detail/openssl/openssl_types.hpp>

#include <everest/logging.hpp>

#include <libwebsockets.h>
#include <openssl/provider.h>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>

template <> class std::default_delete<lws_context> {
public:
    void operator()(lws_context* ptr) const {
        ::lws_context_destroy(ptr);
    }
};

template <> class std::default_delete<OSSL_LIB_CTX> {
public:
    void operator()(OSSL_LIB_CTX* ptr) const {
        ::OPENSSL_thread_stop_ex(ptr);
        ::OSSL_LIB_CTX_free(ptr);
    }
};

template <> class std::default_delete<SSL_CTX> {
public:
    void operator()(SSL_CTX* ptr) const {
        ::SSL_CTX_free(ptr);
    }
};

namespace ocpp {

enum class EConnectionState {
    INITIALIZE, ///< Initialization state
    CONNECTING, ///< Trying to connect
    CONNECTED,  ///< Successfully connected
    ERROR,      ///< We couldn't connect
    FINALIZED,  ///< We finalized the connection and we're never going to connect again
};

/// \brief Message to return in the callback to close the socket connection
static constexpr int LWS_CLOSE_SOCKET_RESPONSE_MESSAGE = -1;

/// \brief Per thread connection data
struct ConnectionData {
    ConnectionData() :
        is_running(true), is_marked_close(false), state(EConnectionState::INITIALIZE), wsi(nullptr), owner(nullptr) {
    }

    ~ConnectionData() {
        state = EConnectionState::FINALIZED;
        is_running = false;
        wsi = nullptr;
        owner = nullptr;
    }

    void bind_thread(std::thread::id id) {
        lws_thread_id = id;
    }

    std::thread::id get_lws_thread_id() {
        return lws_thread_id;
    }

    void update_state(EConnectionState in_state) {
        state = in_state;
    }

    void do_interrupt() {
        is_running = false;
    }

    void request_close() {
        is_marked_close = true;
    }

    bool is_interupted() {
        return (is_running == false);
    }

    bool is_connecting() {
        return (state.load() == EConnectionState::CONNECTING);
    }

    bool is_close_requested() {
        return is_marked_close;
    }

    auto get_state() {
        return state.load();
    }

    lws* get_conn() {
        return wsi;
    }

    WebsocketTlsTPM* get_owner() {
        return owner.load();
    }

    void set_owner(WebsocketTlsTPM* o) {
        owner = o;
    }

public:
    // This public block will only be used from client loop thread, no locking needed
    // Openssl context, must be destroyed in this order
    std::unique_ptr<SSL_CTX> sec_context;
    std::unique_ptr<OSSL_LIB_CTX> sec_lib_context;
    // libwebsockets state
    std::unique_ptr<lws_context> lws_ctx;

    lws* wsi;

private:
    std::atomic<WebsocketTlsTPM*> owner;

    std::thread::id lws_thread_id;

    std::atomic_bool is_running;
    std::atomic_bool is_marked_close;
    std::atomic<EConnectionState> state;
};

struct WebsocketMessage {
    WebsocketMessage() : sent_bytes(0), message_sent(false) {
    }

    virtual ~WebsocketMessage() {
    }

public:
    std::string payload;
    lws_write_protocol protocol;

    // How many bytes we have sent to libwebsockets, does not
    // necessarily mean that all bytes have been sent over the wire,
    // just that these were sent to libwebsockets
    size_t sent_bytes;
    // If libwebsockets has sent all the bytes through the wire
    std::atomic_bool message_sent;
};

WebsocketTlsTPM::WebsocketTlsTPM(const WebsocketConnectionOptions& connection_options,
                                 std::shared_ptr<EvseSecurity> evse_security) :
    WebsocketBase(), evse_security(evse_security) {

    set_connection_options(connection_options);

    EVLOG_debug << "Initialised WebsocketTlsTPM with URI: " << this->connection_options.csms_uri.string();
}

WebsocketTlsTPM::~WebsocketTlsTPM() {
    if (conn_data != nullptr) {
        conn_data->do_interrupt();
    }

    if (websocket_thread != nullptr) {
        websocket_thread->join();
    }

    if (recv_message_thread != nullptr) {
        recv_message_thread->join();
    }
}

void WebsocketTlsTPM::set_connection_options(const WebsocketConnectionOptions& connection_options) {
    switch (connection_options.security_profile) { // `switch` used to lint on missing enum-values
    case security::SecurityProfile::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION:
    case security::SecurityProfile::UNSECURED_TRANSPORT_WITH_BASIC_AUTHENTICATION:
        throw std::invalid_argument("`security_profile` is not a TLS-profile");
        [[fallthrough]];
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

static int callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    // Get user safely, since on some callbacks (void *user) can be different than what we set
    if (wsi != nullptr) {
        if (ConnectionData* data = reinterpret_cast<ConnectionData*>(lws_wsi_user(wsi))) {
            auto owner = data->get_owner();
            if (owner not_eq nullptr) {
                return data->get_owner()->process_callback(wsi, static_cast<int>(reason), user, in, len);
            } else {
                EVLOG_error << "callback_minimal called, but data->owner is nullptr";
            }
        }
    }

    return 0;
}

constexpr auto local_protocol_name = "lws-everest-client";
static const struct lws_protocols protocols[] = {{local_protocol_name, callback_minimal, 0, 0, 0, NULL, 0},
                                                 LWS_PROTOCOL_LIST_TERM};

static void create_sec_context(bool use_tpm, OSSL_LIB_CTX*& out_libctx, SSL_CTX*& out_ctx) {
    OSSL_LIB_CTX* libctx = OSSL_LIB_CTX_new();

    if (libctx == nullptr) {
        EVLOG_AND_THROW(std::runtime_error("Unable to create ssl lib ctx."));
    }

    out_libctx = libctx;

    if (use_tpm) {
        OSSL_PROVIDER* prov_tpm2 = nullptr;
        OSSL_PROVIDER* prov_default = nullptr;

        if ((prov_tpm2 = OSSL_PROVIDER_load(libctx, "tpm2")) == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Could not load provider tpm2."));
        }

        if (!OSSL_PROVIDER_self_test(prov_tpm2)) {
            EVLOG_AND_THROW(std::runtime_error("Could not self-test provider tpm2."));
        }

        if ((prov_default = OSSL_PROVIDER_load(libctx, "default")) == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Could not load provider default."));
        }

        if (!OSSL_PROVIDER_self_test(prov_default)) {
            EVLOG_AND_THROW(std::runtime_error("Could not self-test provider default."));
        }
    }

    const SSL_METHOD* method = SSLv23_client_method();
    SSL_CTX* ctx = SSL_CTX_new_ex(libctx, nullptr, method);

    if (ctx == nullptr) {
        EVLOG_AND_THROW(std::runtime_error("Unable to create ssl ctx."));
    }

    out_ctx = ctx;
}

void WebsocketTlsTPM::tls_init() {
    SSL_CTX* ctx = nullptr;

    if (auto* data = conn_data.get()) {
        ctx = data->sec_context.get();
    }

    if (nullptr == ctx) {
        EVLOG_AND_THROW(std::runtime_error("Invalid SSL context!"));
    }

    auto rc = SSL_CTX_set_cipher_list(ctx, this->connection_options.supported_ciphers_12.c_str());
    if (rc != 1) {
        EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
        EVLOG_AND_THROW(std::runtime_error("Could not set TLSv1.2 cipher list"));
    }

    rc = SSL_CTX_set_ciphersuites(ctx, this->connection_options.supported_ciphers_13.c_str());
    if (rc != 1) {
        EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    if (this->connection_options.security_profile == 3) {
        const char* path_key = nullptr;
        const char* path_chain = nullptr;

        const auto certificate_key_pair =
            this->evse_security->get_key_pair(CertificateSigningUseEnum::ChargingStationCertificate);

        if (!certificate_key_pair.has_value()) {
            EVLOG_AND_THROW(std::runtime_error(
                "Connecting with security profile 3 but no client side certificate is present or valid"));
        }

        path_chain = certificate_key_pair.value().certificate_path.c_str();
        path_key = certificate_key_pair.value().key_path.c_str();

        if (1 != SSL_CTX_use_certificate_chain_file(ctx, path_chain)) {
            EVLOG_AND_THROW(std::runtime_error("Could not use client certificate file within SSL context"));
        }

        if (1 != SSL_CTX_use_PrivateKey_file(ctx, path_key, SSL_FILETYPE_PEM)) {
            EVLOG_AND_THROW(std::runtime_error("Could not set private key file within SSL context"));
        }

        if (false == SSL_CTX_check_private_key(ctx)) {
            EVLOG_AND_THROW(std::runtime_error("Could not check private key within SSL context"));
        }
    }

    if (this->evse_security->is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
        std::string ca_csms = this->evse_security->get_verify_file(ocpp::CaCertificateType::CSMS);

        EVLOG_info << "Loading CA csms bundle to verify server certificate: " << ca_csms;

        rc = SSL_CTX_load_verify_locations(ctx, ca_csms.c_str(), NULL);

        if (rc != 1) {
            EVLOG_error << "Could not load CA verify locations, error: " << ERR_error_string(ERR_get_error(), NULL);
            EVLOG_AND_THROW(std::runtime_error("Could not load CA verify locations"));
        }
    }

    if (this->connection_options.use_ssl_default_verify_paths) {
        rc = SSL_CTX_set_default_verify_paths(ctx);
        if (rc != 1) {
            EVLOG_error << "Could not load default CA verify path, error: " << ERR_error_string(ERR_get_error(), NULL);
            EVLOG_AND_THROW(std::runtime_error("Could not load CA verify locations"));
        }
    }

    // Extra info
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // to verify server certificate
                                                    // SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
}

void WebsocketTlsTPM::recv_loop() {
    std::shared_ptr<ConnectionData> local_data = conn_data;
    auto data = local_data.get();

    if (data == nullptr) {
        EVLOG_error << "Failed recv loop context init!";
        return;
    }

    EVLOG_debug << "Init recv loop with ID: " << std::this_thread::get_id();

    while (false == data->is_interupted()) {
        // Process all messages
        while (true) {
            std::string message{};

            {
                std::lock_guard lk(this->recv_mutex);
                if (recv_message_queue.empty())
                    break;

                message = std::move(recv_message_queue.front());
                recv_message_queue.pop();
            }

            // Invoke our processing callback, that might trigger a send back that
            // can cause a deadlock if is not managed on a different thread
            this->message_callback(message);
        }

        // While we are empty, sleep
        {
            std::unique_lock<std::mutex> lock(this->recv_mutex);
            recv_message_cv.wait_for(lock, std::chrono::seconds(1),
                                     [&]() { return (false == recv_message_queue.empty()); });
        }
    }

    EVLOG_debug << "Exit recv loop with ID: " << std::this_thread::get_id();
}

void WebsocketTlsTPM::client_loop() {
    std::shared_ptr<ConnectionData> local_data = conn_data;
    auto data = local_data.get();

    if (data == nullptr) {
        EVLOG_error << "Failed client loop context init!";
        return;
    }

    // Bind thread for checks
    local_data->bind_thread(std::this_thread::get_id());

    // lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_PARSER | LLL_HEADER
    //					| LLL_EXT | LLL_CLIENT | LLL_LATENCY | LLL_THREAD | LLL_USER, nullptr);
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, nullptr);

    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;

    // Set reference to ConnectionData since 'data' can go away in the websocket
    info.user = data;

    info.fd_limit_per_thread = 1 + 1 + 1;

    bool use_tpm = connection_options.use_tpm_tls;

    // Setup context
    OSSL_LIB_CTX* lib_ctx;
    SSL_CTX* ssl_ctx;

    create_sec_context(use_tpm, lib_ctx, ssl_ctx);

    // Connection acquire the contexts
    conn_data->sec_lib_context = std::unique_ptr<OSSL_LIB_CTX>(lib_ctx);
    conn_data->sec_context = std::unique_ptr<SSL_CTX>(ssl_ctx);

    // Init TLS data
    tls_init();

    // Setup our context
    info.provided_client_ssl_ctx = ssl_ctx;

    lws_context* lws_ctx = lws_create_context(&info);
    if (nullptr == lws_ctx) {
        EVLOG_error << "lws init failed!";
        data->update_state(EConnectionState::FINALIZED);
    }

    // Conn acquire the lws context
    conn_data->lws_ctx = std::unique_ptr<lws_context>(lws_ctx);

    lws_client_connect_info i;
    memset(&i, 0, sizeof(lws_client_connect_info));

    int ssl_connection = LCCSCF_USE_SSL;

    // TODO: Completely remove after test
    // ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
    // ssl_connection |= LCCSCF_ALLOW_INSECURE;
    // ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
    // ssl_connection |= LCCSCF_ALLOW_EXPIRED;

    auto& uri = this->connection_options.csms_uri;

    // TODO: No idea who releases the strdup?
    i.context = lws_ctx;
    i.port = uri.get_port();
    i.address = strdup(uri.get_hostname().c_str());                       // Base address, as resolved by getnameinfo
    i.path = strdup((uri.get_path() + uri.get_chargepoint_id()).c_str()); // Path of resource
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.protocol = strdup(conversions::ocpp_protocol_version_to_string(this->connection_options.ocpp_version).c_str());
    i.local_protocol_name = local_protocol_name;
    i.pwsi = &conn_data->wsi;
    i.userdata = data; // See lws_context 'user'

    // TODO (ioan): See if we need retry policy since we handle this manually
    // i.retry_and_idle_policy = &retry;

    // Print data for debug
    EVLOG_info << "LWS connect with info "
               << "port: [" << i.port << "] address: [" << i.address << "] path: [" << i.path << "] protocol: ["
               << i.protocol << "]";

    lws* wsi = lws_client_connect_via_info(&i);

    if (nullptr == wsi) {
        EVLOG_error << "LWS connect failed!";
        data->update_state(EConnectionState::FINALIZED);
    }

    EVLOG_debug << "Init client loop with ID: " << std::this_thread::get_id();

    // Process while we're running
    int n = 0;

    while (n >= 0 && (false == data->is_interupted())) {
        // Set to -1 for continuous servicing, of required, not recommended
        n = lws_service(data->lws_ctx.get(), 0);

        bool message_queue_empty;
        {
            std::lock_guard<std::mutex> lock(this->queue_mutex);
            message_queue_empty = message_queue.empty();
        }
        if (false == message_queue_empty) {
            lws_callback_on_writable(data->get_conn());
        }
    }

    // Client loop finished for our tid
    EVLOG_debug << "Exit client loop with ID: " << std::this_thread::get_id();
}

// Will be called from external threads as well
bool WebsocketTlsTPM::connect() {
    if (!this->initialized()) {
        return false;
    }

    EVLOG_info << "Connecting tpm TLS websocket to uri: " << this->connection_options.csms_uri.string()
               << " with security-profile " << this->connection_options.security_profile
               << " with TPM keys: " << this->connection_options.use_tpm_tls;

    // Interrupt any previous connection
    if (this->conn_data) {
        this->conn_data->do_interrupt();
    }

    auto conn_data = new ConnectionData();
    conn_data->set_owner(this);

    this->conn_data.reset(conn_data);

    // Wait old thread for a clean state
    if (this->websocket_thread) {
        // Awake libwebsockets thread to quickly exit
        request_write();
        this->websocket_thread->join();
    }

    if (this->recv_message_thread) {
        // Awake the receiving message thread to finish
        recv_message_cv.notify_one();
        this->recv_message_thread->join();
    }

    // Stop any pending reconnect timer
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    // Clear any pending messages on a new connection
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        std::queue<std::shared_ptr<WebsocketMessage>> empty;
        empty.swap(message_queue);
    }

    {
        std::lock_guard<std::mutex> lock(recv_mutex);
        std::queue<std::string> empty;
        empty.swap(recv_message_queue);
    }

    // Bind reconnect callback
    this->reconnect_callback = [this](const websocketpp::lib::error_code& ec) {
        EVLOG_info << "Reconnecting to TLS websocket at uri: " << this->connection_options.csms_uri.string()
                   << " with security profile: " << this->connection_options.security_profile;

        // close connection before reconnecting
        if (this->m_is_connected) {
            EVLOG_info << "Closing websocket connection before reconnecting";
            this->close(websocketpp::close::status::abnormal_close, "Reconnect");
        }

        this->connect();
    };

    std::unique_lock<std::mutex> lock(connection_mutex);

    // Release other threads
    this->websocket_thread.reset(new std::thread(&WebsocketTlsTPM::client_loop, this));

    // TODO(ioan): remove this thread when the fix will be moved into 'MessageQueue'
    // The reason for having a received message processing thread is that because
    // if we dispatch a message receive from the client_loop thread, then the callback
    // will send back another message, and since we're waiting for that message to be
    // sent over the wire on the client_loop, not giving the opportunity to the loop to
    // advance we will have a dead-lock
    this->recv_message_thread.reset(new std::thread(&WebsocketTlsTPM::recv_loop, this));

    // Wait until connect or timeout
    bool timeouted = !conn_cv.wait_for(lock, std::chrono::seconds(60), [&]() {
        return false == conn_data->is_connecting() && EConnectionState::INITIALIZE != conn_data->get_state();
    });

    EVLOG_info << "Connect finalized with state: " << (int)conn_data->get_state() << " Timeouted: " << timeouted;
    bool connected = (conn_data->get_state() == EConnectionState::CONNECTED);

    if (false == connected) {
        EVLOG_error << "Conn failed, interrupting.";

        // Interrupt and drop the connection data
        this->conn_data->do_interrupt();
        this->conn_data.reset();
    }

    return (connected);
}

void WebsocketTlsTPM::reconnect(std::error_code reason, long delay) {
    EVLOG_info << "Attempting TLS TPM reconnect with reason: " << reason << " and delay:" << delay;

    if (this->shutting_down) {
        EVLOG_info << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    if (this->m_is_connected) {
        EVLOG_info << "Closing websocket connection before reconnecting";
        this->close(websocketpp::close::status::abnormal_close, "Reconnect");
    }

    EVLOG_info << "Reconnecting in: " << delay << "ms"
               << ", attempt: " << this->connection_attempts;

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.timeout([this]() { this->reconnect_callback(websocketpp::lib::error_code()); },
                                          std::chrono::milliseconds(delay));
    }
}

void WebsocketTlsTPM::close(websocketpp::close::status::value code, const std::string& reason) {
    EVLOG_info << "Closing TLS TPM websocket with reason: " << reason;

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    if (conn_data) {
        if (auto* data = conn_data.get()) {
            // Set the trigger from us
            data->request_close();
            data->do_interrupt();
        }

        // Release the connection data
        conn_data.reset();
    }

    this->m_is_connected = false;
    std::thread closing([this]() { this->closed_callback(websocketpp::close::status::normal); });
    closing.detach();
}

void WebsocketTlsTPM::on_conn_connected() {
    EVLOG_info << "OCPP client successfully connected to TLS websocket server";

    this->connection_attempts = 1; // reset connection attempts
    this->m_is_connected = true;
    this->reconnecting = false;

    std::thread connected([this]() { this->connected_callback(this->connection_options.security_profile); });
    connected.detach();
}

void WebsocketTlsTPM::on_conn_close() {
    EVLOG_info << "OCPP client closed connection to TLS websocket server";

    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->m_is_connected = false;
    this->disconnected_callback();
    this->cancel_reconnect_timer();

    std::thread closing([this]() { this->closed_callback(websocketpp::close::status::normal); });
    closing.detach();
}

void WebsocketTlsTPM::on_conn_fail() {
    EVLOG_error << "OCPP client connection failed to TLS websocket server";

    std::lock_guard<std::mutex> lk(this->connection_mutex);
    if (this->m_is_connected) {
        std::thread disconnect([this]() { this->disconnected_callback(); });
        disconnect.detach();
    }

    this->m_is_connected = false;
    this->connection_attempts += 1;

    // -1 indicates to always attempt to reconnect
    if (this->connection_options.max_connection_attempts == -1 or
        this->connection_attempts <= this->connection_options.max_connection_attempts) {
        this->reconnect(std::error_code(), this->get_reconnect_interval());
    } else {
        EVLOG_info << "Closed TLS websocket, reconnect attempts exhausted";
        this->close(websocketpp::close::status::normal, "Connection failed");
    }
}

void WebsocketTlsTPM::on_message(void* msg, size_t len) {
    if (!this->initialized()) {
        EVLOG_error << "Message received but TLS websocket has not been correctly initialized. Discarding message.";
        return;
    }

    std::string message(reinterpret_cast<char*>(msg), len);
    EVLOG_info << "Received message over TLS websocket polling for process: " << message;

    {
        std::lock_guard<std::mutex> lock(this->recv_mutex);
        recv_message_queue.push(std::move(message));
    }

    recv_message_cv.notify_one();
}

static bool send_internal(lws* wsi, WebsocketMessage* msg) {
    static std::vector<char> buff;

    std::string& message = msg->payload;
    size_t message_len = message.length();
    size_t buff_req_size = message_len + LWS_PRE;

    if (buff.size() < buff_req_size)
        buff.resize(buff_req_size);

    // Copy data in send buffer
    memcpy(&buff[LWS_PRE], message.data(), message_len);

    // TODO (ioan): if we require certain sending over the wire,
    // we'll have to send chunked manually something like this:

    // Ethernet MTU: ~= 1500bytes
    // size_t BUFF_SIZE = (MTU * 2);
    // char *buff = alloca(LWS_PRE + BUFF_SIZE);
    // memcpy(buff, message.data() + already_written, BUFF_SIZE);
    // int flags = lws_write_ws_flags(proto, is_start, is_end);
    // already_written += lws_write(wsi, buff + LWS_PRE, BUFF_SIZE - LWS_PRE, flags);

    auto sent = lws_write(wsi, reinterpret_cast<unsigned char*>(&buff[LWS_PRE]), message_len, msg->protocol);

    // Even if we have written all the bytes to lws, it doesn't mean that it has been sent over
    // the wire. According to the function comment (lws_write), until everything has been
    // sent, the 'LWS_CALLBACK_CLIENT_WRITEABLE' callback will be suppressed. When we received
    // another callback, it means that everything was sent and that we can mark the message
    // as certainly 'sent' over the wire
    msg->sent_bytes = sent;

    if (sent < 0) {
        // Fatal error, conn closed
        EVLOG_error << "Error sending message over TLS websocket, conn closed.";
        return false;
    }

    if (sent < message_len) {
        EVLOG_error << "Error sending message over TLS websocket. Sent bytes: " << sent
                    << " Total to send: " << message_len;
        return false;
    }

    return true;
}

void WebsocketTlsTPM::on_writable() {
    if (!this->initialized() || !this->m_is_connected) {
        EVLOG_error << "Message sending but TLS websocket has not been correctly initialized/connected.";
        return;
    }

    auto* data = conn_data.get();

    if (nullptr == data) {
        EVLOG_error << "Message sending TLS websocket with null connection data!";
        return;
    }

    if (data->is_interupted() || data->get_state() == EConnectionState::FINALIZED) {
        EVLOG_error << "Trying to write message to interrupted/finalized state!";
        return;
    }

    // Execute while we have messages that were polled
    while (true) {
        WebsocketMessage* message = nullptr;

        {
            std::lock_guard<std::mutex> lock(this->queue_mutex);

            // Break if we have en empty queue
            if (message_queue.empty())
                break;

            message = message_queue.front().get();
        }

        if (message == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Null message in queue, fatal error!"));
        }

        // This message was polled in a previous iteration
        if (message->sent_bytes >= message->payload.length()) {
            EVLOG_info << "Websocket message fully written, popping processing thread from queue!";

            // If we have written all bytes to libwebsockets it means that if we received
            // this writable callback everything is sent over the wire, mark the message
            // as 'sent' and remove it from the queue
            {
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                message->message_sent = true;
                message_queue.pop();
            }

            EVLOG_debug << "Notifying waiting thread!";
            // Notify any waiting thread to check it's state
            msg_send_cv.notify_all();
        } else {
            // If the message was not polled, we reached the first unpolled and break
            break;
        }
    }

    // If we still have message ONLY poll a single one that can be processed in the invoke of the function
    // libwebsockets is designed so that when a message is sent to the wire from the internal buffer it
    // will invoke 'on_writable' again and we can execute the code above
    bool any_message_polled;
    {
        std::lock_guard<std::mutex> lock(this->queue_mutex);
        any_message_polled = not message_queue.empty();
    }

    // Poll a single message
    if (any_message_polled) {
        EVLOG_debug << "Client writable, sending message part!";

        WebsocketMessage* message = nullptr;

        {
            std::lock_guard<std::mutex> lock(this->queue_mutex);
            message = message_queue.front().get();
        }

        if (message == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Null message in queue, fatal error!"));
        }

        if (message->sent_bytes >= message->payload.length()) {
            EVLOG_AND_THROW(std::runtime_error("Already polled message should be handled above, fatal error!"));
        }

        // Continue sending message part, for a single message only
        bool sent = send_internal(data->get_conn(), message);

        // If we failed, attempt again later
        if (false == sent) {
            message->sent_bytes = 0;
        }
    }
}

void WebsocketTlsTPM::request_write() {
    if (this->m_is_connected) {
        if (auto* data = conn_data.get()) {
            if (data->get_conn()) {
                // Notify waiting processing thread to wake up. According to docs it is ok to call from another
                // thread.
                lws_cancel_service(data->lws_ctx.get());
            }
        }
    } else {
        EVLOG_warning << "Requested write with offline TLS websocket!";
    }
}

void WebsocketTlsTPM::poll_message(const std::shared_ptr<WebsocketMessage>& msg) {

    if (std::this_thread::get_id() == conn_data->get_lws_thread_id()) {
        EVLOG_AND_THROW(std::runtime_error("Deadlock detected, polling send from client lws thread!"));
    }

    EVLOG_info << "Queueing message over TLS websocket: " << msg->payload;

    {
        std::lock_guard<std::mutex> lock(this->queue_mutex);
        message_queue.emplace(msg);
    }

    // Request a write callback
    request_write();

    {
        std::unique_lock lock(this->msg_send_cv_mutex);
        if (msg_send_cv.wait_for(lock, std::chrono::seconds(20), [&] { return (true == msg->message_sent); })) {
            EVLOG_info << "Successfully sent last message over TLS websocket!";
        } else {
            EVLOG_warning << "Could not send last message over TLS websocket!";
        }
    }
}

// Will be called from external threads
bool WebsocketTlsTPM::send(const std::string& message) {
    if (!this->initialized()) {
        EVLOG_error << "Could not send message because websocket is not properly initialized.";
        return false;
    }

    auto msg = std::make_shared<WebsocketMessage>();
    msg->payload = std::move(message);
    msg->protocol = LWS_WRITE_TEXT;

    poll_message(msg);

    return msg->message_sent;
}

void WebsocketTlsTPM::ping() {
    if (!this->initialized()) {
        EVLOG_error << "Could not send ping because websocket is not properly initialized.";
    }

    auto msg = std::make_shared<WebsocketMessage>();
    msg->payload = this->connection_options.ping_payload;
    msg->protocol = LWS_WRITE_PING;

    poll_message(msg);
}

int WebsocketTlsTPM::process_callback(void* wsi_ptr, int callback_reason, void* user, void* in, size_t len) {
    enum lws_callback_reasons reason = static_cast<lws_callback_reasons>(callback_reason);

    lws* wsi = reinterpret_cast<lws*>(wsi_ptr);

    // The ConnectionData is thread bound, so that if we clear it in the 'WebsocketTlsTPM'
    // we still have a chance to close the connection here
    ConnectionData* data = reinterpret_cast<ConnectionData*>(lws_wsi_user(wsi));

    // If we are in the process of deletion, just close socket and return
    if (nullptr == data) {
        return LWS_CLOSE_SOCKET_RESPONSE_MESSAGE;
    }

    switch (reason) {
    // TODO: If required in the future
    case LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION:
        break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
        break;

    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: {
        unsigned char **ptr = reinterpret_cast<unsigned char**>(in), *end_header = (*ptr) + len;

        if (this->connection_options.hostName.has_value()) {
            auto& str = this->connection_options.hostName.value();
            EVLOG_info << "User-Host is set to " << str;

            if (0 != lws_add_http_header_by_name(wsi, reinterpret_cast<const unsigned char*>("User-Host"),
                                                 reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), ptr,
                                                 end_header)) {
                EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
            }
        }

        if (this->connection_options.security_profile == 2) {
            std::optional<std::string> authorization_header = this->getAuthorizationHeader();

            if (authorization_header != std::nullopt) {
                auto& str = authorization_header.value();
                if (0 != lws_add_http_header_by_name(wsi, reinterpret_cast<const unsigned char*>("Authorization"),
                                                     reinterpret_cast<const unsigned char*>(str.c_str()), str.length(),
                                                     ptr, end_header)) {
                    EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
                }
            } else {
                EVLOG_AND_THROW(
                    std::runtime_error("No authorization key provided when connecting with security profile 2 or 3."));
            }
        }

        return 0;
    } break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        EVLOG_error << "CLIENT_CONNECTION_ERROR: " << (in ? reinterpret_cast<char*>(in) : "(null)");

        if (data->get_state() == EConnectionState::CONNECTING) {
            data->update_state(EConnectionState::ERROR);
            conn_cv.notify_one();
        }

        on_conn_fail();
        return -1;

    case LWS_CALLBACK_CONNECTING:
        EVLOG_info << "Client connecting...";
        data->update_state(EConnectionState::CONNECTING);
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        if (data->get_state() == EConnectionState::CONNECTING) {
            data->update_state(EConnectionState::CONNECTED);
            conn_cv.notify_one();
        }

        on_conn_connected();

        // Attempt first write after connection
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: {
        std::string close_reason(reinterpret_cast<char*>(in), len);
        unsigned char* pp = reinterpret_cast<unsigned char*>(in);
        unsigned short close_code = (unsigned short)((pp[0] << 8) | pp[1]);

        EVLOG_info << "Unsolicited client close reason: " << close_reason << " close code: " << close_code;

        if (close_code != LWS_CLOSE_STATUS_NORMAL) {
            data->update_state(EConnectionState::ERROR);
            data->do_interrupt();
            on_conn_fail();
        }

        // Return 0 to print peer close reason
        return 0;
    }

    case LWS_CALLBACK_CLIENT_CLOSED:
        EVLOG_info << "Client closed, was requested: " << data->is_close_requested();

        // Determine if the close connection was requested or if the server went away
        if (data->is_close_requested()) {
            data->update_state(EConnectionState::FINALIZED);
            data->do_interrupt();
            on_conn_close();
        } else {
            // It means the server went away, attempt to reconnect
            data->update_state(EConnectionState::ERROR);
            data->do_interrupt();
            on_conn_fail();
        }

        break;

    case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        on_writable();
        {
            bool message_queue_empty;
            {
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                message_queue_empty = message_queue.empty();
            }
            if (false == message_queue_empty) {
                lws_callback_on_writable(wsi);
            }
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE_PONG: {
        bool message_queue_empty;
        {
            std::lock_guard<std::mutex> lock(this->queue_mutex);
            message_queue_empty = message_queue.empty();
        }
        if (false == message_queue_empty) {
            lws_callback_on_writable(data->get_conn());
        }
    } break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        on_message(in, len);
        {
            bool message_queue_empty;
            {
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                message_queue_empty = message_queue.empty();
            }
            if (false == message_queue_empty) {
                lws_callback_on_writable(data->get_conn());
            }
        }
        break;

    case LWS_CALLBACK_EVENT_WAIT_CANCELLED: {
        bool message_queue_empty;
        {
            std::lock_guard<std::mutex> lock(this->queue_mutex);
            message_queue_empty = message_queue.empty();
        }
        if (false == message_queue_empty) {
            lws_callback_on_writable(data->get_conn());
        }
    } break;

    default:
        EVLOG_info << "Callback with unhandled reason: " << reason;
        break;
    }

    // If we are interrupted, close the socket cleanly
    if (data->is_interupted()) {
        EVLOG_info << "Conn interrupted/closed, closing socket!";
        return LWS_CLOSE_SOCKET_RESPONSE_MESSAGE;
    }

    // Return -1 on fatal error (-1 is request to close the socket)
    return 0;
}

} // namespace ocpp