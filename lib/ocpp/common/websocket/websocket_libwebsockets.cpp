// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <evse_security/crypto/openssl/openssl_tpm.hpp>
#include <ocpp/common/websocket/websocket_libwebsockets.hpp>

#include <everest/logging.hpp>

#include <libwebsockets.h>

#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>

#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

#define USING_OPENSSL_3 (OPENSSL_VERSION_NUMBER >= 0x30000000L)

#if USING_OPENSSL_3
#include <openssl/provider.h>
#else
#define SSL_CTX_new_ex(LIB, PROP, METHOD) SSL_CTX_new(METHOD)
#endif

template <> class std::default_delete<lws_context> {
public:
    void operator()(lws_context* ptr) const {
        ::lws_context_destroy(ptr);
    }
};

template <> class std::default_delete<SSL_CTX> {
public:
    void operator()(SSL_CTX* ptr) const {
        ::SSL_CTX_free(ptr);
    }
};

namespace ocpp {

using evse_security::is_tpm_key_file;
using evse_security::OpenSSLProvider;

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
        wsi(nullptr), owner(nullptr), is_running(true), is_marked_close(false), state(EConnectionState::INITIALIZE) {
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

static bool verify_csms_cn(const std::string& hostname, bool preverified, const X509_STORE_CTX* ctx,
                           bool allow_wildcards) {

    // Error depth gives the depth in the chain (with 0 = leaf certificate) where
    // a potential (!) error occurred; error here means current error code and can also be "OK".
    // This thus gives also the position (in the chain)  of the currently to be verified certificate.
    // If depth is 0, we need to check the leaf certificate;
    // If depth > 0, we are verifying a CA (or SUB-CA) certificate and thus trust "preverified"
    int depth = X509_STORE_CTX_get_error_depth(ctx);

    if (!preverified) {
        int error = X509_STORE_CTX_get_error(ctx);
        EVLOG_warning << "Invalid certificate error '" << X509_verify_cert_error_string(error) << "' (at chain depth '"
                      << depth << "')";
    }

    // only check for CSMS server certificate
    if (depth == 0 and preverified) {
        // Get server certificate
        X509* server_cert = X509_STORE_CTX_get_current_cert(ctx);

        // TODO (ioan): this manual verification is done because libwebsocket does not take into account
        // the host parameter that we are setting during 'tls_init'. This function should be removed
        // when we can make libwebsocket take custom verification parameter

        // Verify host-name manually
        int result;

        if (allow_wildcards) {
            result = X509_check_host(server_cert, hostname.c_str(), hostname.length(),
                                     X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS, nullptr);
        } else {
            result = X509_check_host(server_cert, hostname.c_str(), hostname.length(), X509_CHECK_FLAG_NO_WILDCARDS,
                                     nullptr);
        }

        if (result != 1) {
            X509_NAME* subject_name = X509_get_subject_name(server_cert);
            char common_name[256];

            if (X509_NAME_get_text_by_NID(subject_name, NID_commonName, common_name, sizeof(common_name)) <= 0) {
                EVLOG_error << "Failed to verify server certificate cn with hostname: " << hostname
                            << " and with server certificate cs: " << common_name
                            << " with wildcards: " << allow_wildcards;
            }

            return false;
        }
    }

    return preverified;
}

WebsocketTlsTPM::WebsocketTlsTPM(const WebsocketConnectionOptions& connection_options,
                                 std::shared_ptr<EvseSecurity> evse_security) :
    WebsocketBase(), evse_security(evse_security) {

    set_connection_options(connection_options);

    EVLOG_debug << "Initialised WebsocketTlsTPM with URI: " << this->connection_options.csms_uri.string();
}

WebsocketTlsTPM::~WebsocketTlsTPM() {
    std::shared_ptr<ConnectionData> local_data = conn_data;
    if (local_data != nullptr) {
        local_data->do_interrupt();
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
    case security::SecurityProfile::TLS_WITH_BASIC_AUTHENTICATION:
    case security::SecurityProfile::TLS_WITH_CLIENT_SIDE_CERTIFICATES:
        break;
    default:
        throw std::invalid_argument("unknown `security_profile`, value = " +
                                    std::to_string(connection_options.security_profile));
    }

    set_connection_options_base(connection_options);

    // Set secure URI only if it is in TLS mode
    if (connection_options.security_profile >
        security::SecurityProfile::UNSECURED_TRANSPORT_WITH_BASIC_AUTHENTICATION) {
        this->connection_options.csms_uri.set_secure(true);
    }
}

static int callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    // Get user safely, since on some callbacks (void *user) can be different than what we set
    if (wsi != nullptr) {
        if (ConnectionData* data = reinterpret_cast<ConnectionData*>(lws_wsi_user(wsi))) {
            auto owner = data->get_owner();
            if (owner not_eq nullptr) {
                return data->get_owner()->process_callback(wsi, static_cast<int>(reason), user, in, len);
            } else {
                EVLOG_warning << "callback_minimal called, but data->owner is nullptr. Reason: " << reason;
            }
        }
    }

    return 0;
}

constexpr auto local_protocol_name = "lws-everest-client";
static const struct lws_protocols protocols[] = {{local_protocol_name, callback_minimal, 0, 0, 0, NULL, 0},
                                                 LWS_PROTOCOL_LIST_TERM};

void WebsocketTlsTPM::tls_init(SSL_CTX* ctx, const std::string& path_chain, const std::string& path_key, bool tpm_key,
                               std::optional<std::string>& password) {
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
        if ((path_chain.empty()) || (path_key.empty())) {
            EVLOG_error << "Cert chain: [" << path_chain << "] key: " << path_key << "]";
            EVLOG_AND_THROW(std::runtime_error("No certificate and key for SSL"));
        }

        if (1 != SSL_CTX_use_certificate_chain_file(ctx, path_chain.c_str())) {
            ERR_print_errors_fp(stderr);
            EVLOG_AND_THROW(std::runtime_error("Could not use client certificate file within SSL context"));
        }

        SSL_CTX_set_default_passwd_cb_userdata(ctx, reinterpret_cast<void*>(password.value_or("").data()));

        if (1 != SSL_CTX_use_PrivateKey_file(ctx, path_key.c_str(), SSL_FILETYPE_PEM)) {
            ERR_print_errors_fp(stderr);
            EVLOG_AND_THROW(std::runtime_error("Could not set private key file within SSL context"));
        }

        if (false == SSL_CTX_check_private_key(ctx)) {
            ERR_print_errors_fp(stderr);
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

    // TODO (ioan): libwebsockets seems not to take this parameters into account
    // and this code should be re-introduced after the issue is solved. At the moment a
    // manual work-around is used, the check is manually done using 'X509_check_host'

    /*
    if (this->connection_options.verify_csms_common_name) {
        // Verify hostname
        X509_VERIFY_PARAM* param = X509_VERIFY_PARAM_new();

        if (this->connection_options.verify_csms_allow_wildcards) {
            X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        } else {
            X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_WILDCARDS);
        }

        // Set the host and parameter check
        EVLOG_error << "Set wrong host param!";
        if(1 != X509_VERIFY_PARAM_set1_host(param, this->connection_options.csms_uri.get_hostname().c_str(),
                                            this->connection_options.csms_uri.get_hostname().length())) {
            EVLOG_error << "Could not set host name: " << this->connection_options.csms_uri.get_hostname();
            EVLOG_AND_THROW(std::runtime_error("Could not set verification hostname!"));
        }

        SSL_CTX_set1_param(ctx, param);

        X509_VERIFY_PARAM_free(param);
    } else {
        EVLOG_warning << "Not verifying the CSMS certificates commonName with the Fully Qualified Domain Name "
                         "(FQDN) of the server because it has been explicitly turned off via the configuration!";
    }
    */

    // Extra info
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // to verify server certificate
}

void WebsocketTlsTPM::recv_loop() {
    std::shared_ptr<ConnectionData> local_data = conn_data;

    if (local_data == nullptr) {
        EVLOG_error << "Failed recv loop context init!";
        return;
    }

    EVLOG_debug << "Init recv loop with ID: " << std::hex << std::this_thread::get_id();

    while (!local_data->is_interupted()) {
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

    EVLOG_debug << "Exit recv loop with ID: " << std::hex << std::this_thread::get_id();
}

void WebsocketTlsTPM::client_loop() {
    std::shared_ptr<ConnectionData> local_data = conn_data;

    if (local_data == nullptr) {
        EVLOG_error << "Failed client loop context init!";
        return;
    }

    // Bind thread for checks
    local_data->bind_thread(std::this_thread::get_id());

    // lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_PARSER | LLL_HEADER | LLL_EXT |
    //                          LLL_CLIENT | LLL_LATENCY | LLL_THREAD | LLL_USER, nullptr);
    lws_set_log_level(LLL_ERR, nullptr);

    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;

    if (this->connection_options.iface.has_value()) {
        EVLOG_info << "Using network iface: " << this->connection_options.iface.value().c_str();

        info.iface = this->connection_options.iface.value().c_str();
        info.bind_iface = 1;
    }

    // Set reference to ConnectionData since 'data' can go away in the websocket
    info.user = local_data.get();

    info.fd_limit_per_thread = 1 + 1 + 1;

    if (this->connection_options.security_profile == 2 || this->connection_options.security_profile == 3) {
        // Setup context - need to know the key type first
        std::string path_key;
        std::string path_chain;
        std::optional<std::string> password;

        if (this->connection_options.security_profile == 3) {
            const auto certificate_response =
                this->evse_security->get_leaf_certificate_info(CertificateSigningUseEnum::ChargingStationCertificate);

            if (certificate_response.status != ocpp::GetCertificateInfoStatus::Accepted or
                !certificate_response.info.has_value()) {
                EVLOG_AND_THROW(std::runtime_error(
                    "Connecting with security profile 3 but no client side certificate is present or valid"));
            }

            const auto& certificate_info = certificate_response.info.value();

            if (certificate_info.certificate_path.has_value()) {
                path_chain = certificate_info.certificate_path.value();
            } else if (certificate_info.certificate_single_path.has_value()) {
                path_chain = certificate_info.certificate_single_path.value();
            } else {
                EVLOG_AND_THROW(std::runtime_error(
                    "Connecting with security profile 3 but no client side certificate is present or valid"));
            }

            path_key = certificate_info.key_path;
            password = certificate_info.password;
        }

        SSL_CTX* ssl_ctx = nullptr;
        bool tpm_key = false;

        if (!path_key.empty()) {
            tpm_key = is_tpm_key_file(path_key);
        }

        OpenSSLProvider provider;

        if (tpm_key) {
            provider.set_tls_mode(OpenSSLProvider::mode_t::tpm2_provider);
        } else {
            provider.set_tls_mode(OpenSSLProvider::mode_t::default_provider);
        }

        const SSL_METHOD* method = SSLv23_client_method();
        ssl_ctx = SSL_CTX_new_ex(provider, provider.propquery_tls_str(), method);

        if (ssl_ctx == nullptr) {
            ERR_print_errors_fp(stderr);
            EVLOG_AND_THROW(std::runtime_error("Unable to create ssl ctx."));
        }

        // Init TLS data
        tls_init(ssl_ctx, path_chain, path_key, tpm_key, password);

        // Setup our context
        info.provided_client_ssl_ctx = ssl_ctx;

        // Connection acquire the contexts
        local_data->sec_context = std::unique_ptr<SSL_CTX>(ssl_ctx);
    }

    lws_context* lws_ctx = lws_create_context(&info);
    if (nullptr == lws_ctx) {
        EVLOG_error << "lws init failed!";
        local_data->update_state(EConnectionState::FINALIZED);
    }

    // Conn acquire the lws context
    local_data->lws_ctx = std::unique_ptr<lws_context>(lws_ctx);

    lws_client_connect_info i;
    memset(&i, 0, sizeof(lws_client_connect_info));

    // No SSL
    int ssl_connection = 0;

    if (this->connection_options.security_profile == 2 || this->connection_options.security_profile == 3) {
        ssl_connection = LCCSCF_USE_SSL;

        // Skip server hostname check
        if (this->connection_options.verify_csms_common_name == false) {
            ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
        }

        // TODO: Completely remove after test
        // ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
        // ssl_connection |= LCCSCF_ALLOW_INSECURE;
        // ssl_connection |= LCCSCF_ALLOW_EXPIRED;
    }

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
    i.pwsi = &local_data->wsi;
    i.userdata = local_data.get(); // See lws_context 'user'

    if (this->connection_options.iface.has_value()) {
        i.iface = this->connection_options.iface.value().c_str();
    }

    // Print data for debug
    EVLOG_info << "LWS connect with info "
               << "port: [" << i.port << "] address: [" << i.address << "] path: [" << i.path << "] protocol: ["
               << i.protocol << "]";

    if (lws_client_connect_via_info(&i) == nullptr) {
        EVLOG_error << "LWS connect failed!";
        // This condition can occur when connecting fails to an IP address
        // retries need to be attempted
        local_data->update_state(EConnectionState::ERROR);
        on_conn_fail();

        // Notify conn waiter
        conn_cv.notify_one();
    } else {
        EVLOG_debug << "Init client loop with ID: " << std::hex << std::this_thread::get_id();

        // Process while we're running
        int n = 0;

        while (n >= 0 && (!local_data->is_interupted())) {
            // Set to -1 for continuous servicing, of required, not recommended
            n = lws_service(local_data->lws_ctx.get(), 0);

            bool message_queue_empty;
            {
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                message_queue_empty = message_queue.empty();
            }
            if (!message_queue_empty) {
                lws_callback_on_writable(local_data->get_conn());
            }
        }

        // Client loop finished for our tid
        EVLOG_debug << "Exit client loop with ID: " << std::hex << std::this_thread::get_id();
    }
}

// Will be called from external threads as well
bool WebsocketTlsTPM::connect() {
    if (!this->initialized()) {
        return false;
    }

    EVLOG_info << "Connecting to uri: " << this->connection_options.csms_uri.string() << " with security-profile "
               << this->connection_options.security_profile
               << (this->connection_options.use_tpm_tls ? " with TPM keys" : "");

    // new connection context
    std::shared_ptr<ConnectionData> local_data = std::make_shared<ConnectionData>();
    local_data->set_owner(this);

    // Interrupt any previous connection
    std::shared_ptr<ConnectionData> tmp_data = conn_data;
    if (tmp_data != nullptr) {
        tmp_data->do_interrupt();
    }

    // use new connection context
    conn_data = local_data;

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
    this->reconnect_callback = [this]() {
        // close connection before reconnecting
        if (this->m_is_connected) {
            this->close(WebsocketCloseReason::AbnormalClose, "before reconnecting");
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
        return !local_data->is_connecting() && EConnectionState::INITIALIZE != local_data->get_state();
    });

    bool connected = (local_data->get_state() == EConnectionState::CONNECTED);

    if (!connected) {
        EVLOG_info << "Connect failed with state: " << (int)local_data->get_state() << " Timeouted: " << timeouted;

        // If we timeouted the on_conn_fail was not dispatched, since it did not had the chance
        if (timeouted && local_data->get_state() != EConnectionState::ERROR) {
            EVLOG_error << "Conn failed with timeout, without disconnect dispatch, dispatching manually.";
            on_conn_fail();
        }

        // Interrupt and drop the connection data
        local_data->do_interrupt();

        // Also interrupt the latest conenction, if it was set by a parallel thread
        auto local = conn_data;

        if (local != nullptr) {
            local->do_interrupt();
        }

        conn_data.reset();
    }

    return (connected);
}

void WebsocketTlsTPM::reconnect(long delay) {
    if (this->shutting_down) {
        EVLOG_info << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    EVLOG_info << "Reconnecting in: " << delay << "ms"
               << ", attempt: " << this->connection_attempts;

    if (this->m_is_connected) {
        this->close(WebsocketCloseReason::AbnormalClose, "before reconnecting");
    }

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.timeout([this]() { this->reconnect_callback(); }, std::chrono::milliseconds(delay));
    }
}

void WebsocketTlsTPM::close(const WebsocketCloseReason code, const std::string& reason) {
    EVLOG_info << "Closing websocket: " << reason;

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    std::shared_ptr<ConnectionData> local_data = conn_data;
    if (local_data != nullptr) {
        // Set the trigger from us
        local_data->request_close();
        local_data->do_interrupt();
    }
    // Release the connection data
    conn_data.reset();

    this->m_is_connected = false;

    // Notify any message senders that are waiting, since we can't send messages any more
    msg_send_cv.notify_all();

    // Clear any irrelevant data after a DC
    recv_buffered_message.clear();

    std::thread closing([this]() {
        this->closed_callback(WebsocketCloseReason::Normal);
        this->disconnected_callback();
    });
    closing.detach();
}

void WebsocketTlsTPM::on_conn_connected() {
    EVLOG_info << "OCPP client successfully connected to server";

    this->connection_attempts = 1; // reset connection attempts
    this->m_is_connected = true;
    this->reconnecting = false;

    // Clear any irrelevant data after a DC
    recv_buffered_message.clear();

    std::thread connected([this]() { this->connected_callback(this->connection_options.security_profile); });
    connected.detach();
}

void WebsocketTlsTPM::on_conn_close() {
    EVLOG_info << "OCPP client closed connection to server";

    std::lock_guard<std::mutex> lk(this->connection_mutex);
    this->m_is_connected = false;
    this->disconnected_callback();
    this->cancel_reconnect_timer();

    // Notify any message senders that are waiting, since we can't send messages any more
    msg_send_cv.notify_all();

    // Clear any irrelevant data after a DC
    recv_buffered_message.clear();

    std::thread closing([this]() {
        this->closed_callback(WebsocketCloseReason::Normal);
        this->disconnected_callback();
    });
    closing.detach();
}

void WebsocketTlsTPM::on_conn_fail() {
    EVLOG_error << "OCPP client connection to server failed";

    std::lock_guard<std::mutex> lk(this->connection_mutex);
    if (this->m_is_connected) {
        std::thread disconnect([this]() { this->disconnected_callback(); });
        disconnect.detach();
    }

    this->m_is_connected = false;
    recv_buffered_message.clear();

    // -1 indicates to always attempt to reconnect
    if (this->connection_options.max_connection_attempts == -1 or
        this->connection_attempts <= this->connection_options.max_connection_attempts) {
        this->reconnect(this->get_reconnect_interval());

        // Increment reconn attempts
        this->connection_attempts += 1;
    } else {
        this->close(WebsocketCloseReason::Normal, "reconnect attempts exhausted");
    }
}

void WebsocketTlsTPM::on_message(std::string&& message) {
    if (!this->initialized()) {
        EVLOG_error << "Message received but TLS websocket has not been correctly initialized. Discarding message.";
        return;
    }

    EVLOG_debug << "Received message over TLS websocket polling for process: " << message;

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

    if (sent < 0) {
        // Fatal error, conn closed
        EVLOG_error << "Error sending message over TLS websocket, conn closed.";
        msg->sent_bytes = 0;
        return false;
    }

    // Even if we have written all the bytes to lws, it doesn't mean that it has been sent over
    // the wire. According to the function comment (lws_write), until everything has been
    // sent, the 'LWS_CALLBACK_CLIENT_WRITEABLE' callback will be suppressed. When we received
    // another callback, it means that everything was sent and that we can mark the message
    // as certainly 'sent' over the wire
    msg->sent_bytes = sent;

    if (static_cast<size_t>(sent) < message_len) {
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

    std::shared_ptr<ConnectionData> local_data = conn_data;

    if (local_data == nullptr) {
        EVLOG_error << "Message sending TLS websocket with null connection data!";
        return;
    }

    if (local_data->is_interupted() || local_data->get_state() == EConnectionState::FINALIZED) {
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
            EVLOG_debug << "Websocket message fully written, popping processing thread from queue!";

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
        bool sent = send_internal(local_data->get_conn(), message);

        // If we failed, attempt again later
        if (!sent) {
            message->sent_bytes = 0;
        }
    }
}

void WebsocketTlsTPM::request_write() {
    std::shared_ptr<ConnectionData> local_data = conn_data;
    if (this->m_is_connected) {
        if (local_data != nullptr) {
            if (local_data->get_conn()) {
                // Notify waiting processing thread to wake up. According to docs it is ok to call from another
                // thread.
                lws_cancel_service(local_data->lws_ctx.get());
            }
        }
    } else {
        EVLOG_debug << "Requested write with offline TLS websocket!";
    }
}

void WebsocketTlsTPM::poll_message(const std::shared_ptr<WebsocketMessage>& msg) {
    if (this->m_is_connected == false) {
        EVLOG_debug << "Trying to poll message without being connected!";
        return;
    }

    std::shared_ptr<ConnectionData> local_data = conn_data;

    if (local_data != nullptr) {
        auto cd_tid = local_data->get_lws_thread_id();

        if (std::this_thread::get_id() == cd_tid) {
            EVLOG_AND_THROW(std::runtime_error("Deadlock detected, polling send from client lws thread!"));
        }

        // If we are interupted or finalized
        if (local_data->is_interupted() || local_data->get_state() == EConnectionState::FINALIZED) {
            EVLOG_warning << "Trying to poll message to interrupted/finalized state!";
            return;
        }
    }

    EVLOG_debug << "Queueing message over TLS websocket: " << msg->payload;

    {
        std::lock_guard<std::mutex> lock(this->queue_mutex);
        message_queue.emplace(msg);
    }

    // Request a write callback
    request_write();

    {
        std::unique_lock lock(this->msg_send_cv_mutex);
        if (msg_send_cv.wait_for(lock, std::chrono::seconds(20), [&] { return (true == msg->message_sent); })) {
            EVLOG_debug << "Successfully sent last message over TLS websocket!";
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
    case LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION:

        // TODO (ioan): remove this option after we figure out why libwebsockets does not take the param set
        // at 'tls_init' into account
        if (this->connection_options.verify_csms_common_name) {
            // 'user' is X509_STORE and 'len' is preverify_ok (1) in case the pre-verification was successful
            EVLOG_debug << "Verifying server certs!";

            if (false == verify_csms_cn(this->connection_options.csms_uri.get_hostname(), (len == 1),
                                        reinterpret_cast<X509_STORE_CTX*>(user),
                                        this->connection_options.verify_csms_allow_wildcards)) {
                // Return 1 to fail the cert
                return 1;
            }
        }

        break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
        break;

    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: {
        EVLOG_debug << "Handshake with security profile: " << this->connection_options.security_profile;

        unsigned char **ptr = reinterpret_cast<unsigned char**>(in), *end_header = (*ptr) + len;

        if (this->connection_options.hostName.has_value()) {
            auto& str = this->connection_options.hostName.value();
            EVLOG_info << "User-Host is set to " << str;

            if (0 != lws_add_http_header_by_token(wsi, lws_token_indexes::WSI_TOKEN_HOST,
                                                  reinterpret_cast<const unsigned char*>(str.c_str()), str.length(),
                                                  ptr, end_header)) {
                EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
            }

            /*
            if (0 != lws_add_http_header_by_name(wsi, reinterpret_cast<const unsigned char*>("User-Host"),
                                                 reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), ptr,
                                                 end_header)) {
                EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
            }
            */
        }

        if (this->connection_options.security_profile == 1 || this->connection_options.security_profile == 2) {
            std::optional<std::string> authorization_header = this->getAuthorizationHeader();

            if (authorization_header != std::nullopt) {
                auto& str = authorization_header.value();

                if (0 != lws_add_http_header_by_token(wsi, lws_token_indexes::WSI_TOKEN_HTTP_AUTHORIZATION,
                                                      reinterpret_cast<const unsigned char*>(str.c_str()), str.length(),
                                                      ptr, end_header)) {
                    EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
                }

                /*
                // TODO: See if we need to switch back here
                if (0 != lws_add_http_header_by_name(wsi, reinterpret_cast<const unsigned char*>("Authorization"),
                                                     reinterpret_cast<const unsigned char*>(str.c_str()), str.length(),
                                                     ptr, end_header)) {
                    EVLOG_AND_THROW(std::runtime_error("Could not append authorization header."));
                }
                */
            } else {
                EVLOG_AND_THROW(
                    std::runtime_error("No authorization key provided when connecting with security profile 1 or 2."));
            }
        }

        return 0;
    } break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        EVLOG_error << "CLIENT_CONNECTION_ERROR: " << (in ? reinterpret_cast<char*>(in) : "(null)");
        ERR_print_errors_fp(stderr);

        if (data->get_state() == EConnectionState::CONNECTING) {
            data->update_state(EConnectionState::ERROR);
            conn_cv.notify_one();
        }

        on_conn_fail();
        return -1;

    case LWS_CALLBACK_CONNECTING:
        EVLOG_debug << "Client connecting...";
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
        recv_buffered_message.append(reinterpret_cast<char*>(in), reinterpret_cast<char*>(in) + len);

        // Message is complete
        if (lws_remaining_packet_payload(wsi) <= 0) {
            on_message(std::move(recv_buffered_message));
            recv_buffered_message.clear();
        }

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

    // not interested in these callbacks
    case LWS_CALLBACK_WSI_DESTROY:
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
    case LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL:
        break;

    default:
        EVLOG_debug << "Callback with unhandled reason: " << reason;
        break;
    }

    // If we are interrupted, close the socket cleanly
    if (data->is_interupted()) {
        return LWS_CLOSE_SOCKET_RESPONSE_MESSAGE;
    }

    // Return -1 on fatal error (-1 is request to close the socket)
    return 0;
}

} // namespace ocpp
