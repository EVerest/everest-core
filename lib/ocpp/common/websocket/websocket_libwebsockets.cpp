// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <evse_security/crypto/openssl/openssl_provider.hpp>
#include <ocpp/common/websocket/websocket_libwebsockets.hpp>

#include <everest/logging.hpp>

#include <libwebsockets.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
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

using namespace std::chrono_literals;

namespace {
std::optional<std::filesystem::path> keylog_file;

void keylog_callback(const SSL* ssl, const char* line) {
    if (keylog_file.has_value()) {
        std::ofstream keylog_ofs;
        keylog_ofs.open(keylog_file.value(), std::ofstream::out | std::ofstream::app);
        keylog_ofs << line << std::endl;
        keylog_ofs.close();
    }
}
} // namespace

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

using evse_security::is_custom_private_key_file;
using evse_security::OpenSSLProvider;

enum class EConnectionState {
    INITIALIZE,   ///< Initialization state
    CONNECTING,   ///< Trying to connect
    RECONNECTING, ///< After the first connect attempt, we'll change the state to reconnecting
    CONNECTED,    ///< Successfully connected
    ERROR,        ///< We couldn't connect, but we will try again soon internally
    FINALIZED,    ///< We finalized the connection and we're never going to connect again
};

/// \brief Message to return in the callback to close the socket connection
static constexpr int LWS_CLOSE_SOCKET_RESPONSE_MESSAGE = -1;

/// \brief How much we wait for a message to be sent in seconds
static constexpr int MESSAGE_SEND_TIMEOUT_S = 1;

/// \brief Current connection data, sets the internal state of the
struct ConnectionData {
    explicit ConnectionData(WebsocketLibwebsockets* owner) :
        wsi(nullptr), owner(owner), is_running(true), is_stopped_run(false), state(EConnectionState::INITIALIZE) {
    }

    ~ConnectionData() {
        state = EConnectionState::FINALIZED;
        is_running = false;
        wsi = nullptr;
        owner = nullptr;
    }

    void bind_thread_client(std::thread::id id) {
        std::lock_guard lock(this->mutex);
        websocket_client_thread_id = id;
    }

    void bind_thread_message(std::thread::id id) {
        std::lock_guard lock(this->mutex);
        websocket_recv_thread_id = id;
    }

    std::thread::id get_client_thread_id() {
        std::lock_guard lock(this->mutex);
        return websocket_client_thread_id;
    }

    std::thread::id get_message_thread_id() {
        std::lock_guard lock(this->mutex);
        return websocket_recv_thread_id;
    }

    void update_state(EConnectionState in_state) {
        std::lock_guard lock(this->mutex);
        state = in_state;
    }

    EConnectionState get_state() {
        std::lock_guard lock(this->mutex);
        return state;
    }

    /// \brief Requests an active connection to awake from 'poll' and process again
    void request_awake() {
        if (std::this_thread::get_id() == this->websocket_client_thread_id) {
            EVLOG_AND_THROW(std::runtime_error("Attempted to awake connection from websocket thread!"));
        }

        std::lock_guard lock(this->mutex);

        if (lws_ctx) {
            lws_cancel_service(lws_ctx.get());
        }
    }

    /// \brief Requests the threads that are processing to exit as soon as possible
    /// in a ordered manner
    void do_interrupt_and_exit() {
        if (std::this_thread::get_id() == this->websocket_client_thread_id) {
            EVLOG_AND_THROW(std::runtime_error("Attempted to interrupt connection from websocket thread!"));
        }

        std::lock_guard lock(this->mutex);

        if (is_running) {
            is_running = false;

            // Notify if we are on a different thread
            if (lws_ctx) {
                // Attempt to revive the running thread
                lws_cancel_service(lws_ctx.get());
            }
        }
    }

    bool is_interupted() {
        std::lock_guard lock(this->mutex);
        return (is_running == false);
    }

    void mark_stop_executed() {
        std::lock_guard lock(this->mutex);
        this->is_stopped_run = true;
    }

    bool is_stop_executed() {
        std::lock_guard lock(this->mutex);
        return this->is_stopped_run;
    }

public:
    /// \brief This should be used for a cleanup before calling the
    ///        init functions because releasing the unique ptrs has
    ///        as an effect the invocation of 'callback_minimal' during
    ///        '::lws_context_destroy(ptr);' and that causes a deadlock
    void reset_connection_data() {
        // Destroy them outside the lock scope
        std::unique_ptr<SSL_CTX> clear_sec;
        std::unique_ptr<lws_context> clear_lws;

        {
            std::lock_guard lock(this->mutex);
            this->wsi = nullptr;

            if (this->sec_context) {
                this->sec_context.swap(clear_sec);
            }

            if (this->lws_ctx) {
                this->lws_ctx.swap(clear_lws);
            }
        }
    }

    void init_connection_context(lws_context* lws_ctx, SSL_CTX* ssl_ctx) {
        std::lock_guard lock(this->mutex);

        if (this->lws_ctx || this->sec_context) {
            EVLOG_AND_THROW(std::runtime_error("Cleanup must be called before re-initing a connection!"));
        }

        // Reset the close status
        is_stopped_run = false;

        // Causes a deadlock in callback_minimal if not reset
        this->lws_ctx = std::unique_ptr<lws_context>(lws_ctx);

        if (ssl_ctx) {
            this->sec_context = std::unique_ptr<SSL_CTX>(ssl_ctx);
        }
    }

    void init_connection(lws* lws) {
        std::lock_guard lock(this->mutex);
        this->wsi = lws;
    }

    lws* get_conn() {
        std::lock_guard lock(this->mutex);
        return wsi;
    }

    lws_context* get_context() {
        std::lock_guard lock(this->mutex);
        return lws_ctx.get();
    }

    // No need for sync here since its set on construction
    WebsocketLibwebsockets* get_owner() {
        return owner;
    }

private:
    // Openssl context, must be destroyed in this order
    std::unique_ptr<SSL_CTX> sec_context;
    // libwebsockets state
    std::unique_ptr<lws_context> lws_ctx;
    // Internal used WSI
    lws* wsi;
    // Owner, set on creation
    WebsocketLibwebsockets* owner;

    // State variables
    bool is_running;
    bool is_stopped_run;
    EConnectionState state;

    std::mutex mutex;

    /// \brief Websocket client thread ID
    std::thread::id websocket_client_thread_id;
    /// \brief Websocket received message thread ID
    std::thread::id websocket_recv_thread_id;

    // Required for access of state
    friend class WebsocketLibwebsockets;
};

struct WebsocketMessage {
    WebsocketMessage() : sent_bytes(0), message_sent(false) {
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

            if (not allow_wildcards) {
                int wildcard_result = X509_check_host(server_cert, hostname.c_str(), hostname.length(),
                                                      X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS, nullptr);
                if (result != wildcard_result) {
                    EVLOG_error << "Failed to verify server certificate hostname: \"" << hostname
                                << "\". Server certificate common name \"" << common_name
                                << "\" likely contains wildcards. Please check your OCPP configuration and set "
                                   "VerifyCsmsAllowWildcards to true if you want to allow wildcard certificates.";
                }
            }

            return false;
        }
    }

    return preverified;
}

WebsocketLibwebsockets::WebsocketLibwebsockets(const WebsocketConnectionOptions& connection_options,
                                               std::shared_ptr<EvseSecurity> evse_security) :
    WebsocketBase(),
    evse_security(evse_security),
    stop_deferred_handler(false),
    connected_ocpp_version{OcppProtocolVersion::Unknown} {

    set_connection_options(connection_options);

    EVLOG_debug << "Initialised WebsocketLibwebsockets with URI: " << this->connection_options.csms_uri.string();
}

WebsocketLibwebsockets::~WebsocketLibwebsockets() {
    std::lock_guard lock(this->connection_mutex);

    std::shared_ptr<ConnectionData> local_conn_data = conn_data;
    if (local_conn_data != nullptr) {
        auto tid = std::this_thread::get_id();

        if (tid == local_conn_data->get_client_thread_id() || tid == local_conn_data->get_message_thread_id()) {
            EVLOG_error << "Trying to destruct websocket from utility thread!";
            std::terminate();
        }
    }

    if (this->m_is_connected || is_trying_to_connect_internal()) {
        this->close_internal(WebsocketCloseReason::Normal, "websocket destructor");
    }

    // In the dtor we must make sure the deferred callback thread
    // finishes since the callbacks capture a reference to 'this'
    if (this->deferred_callback_thread != nullptr && this->deferred_callback_thread->joinable()) {
        this->stop_deferred_handler.store(true);
        this->deferred_callback_queue.notify_waiting_thread();

        this->deferred_callback_thread->join();
    }
}

void WebsocketLibwebsockets::set_connection_options(const WebsocketConnectionOptions& connection_options) {
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

    if (connection_options.ocpp_versions.empty()) {
        throw std::invalid_argument("Connection options must contain at least 1 option");
    }

    if (std::any_of(connection_options.ocpp_versions.begin(), connection_options.ocpp_versions.end(),
                    [](OcppProtocolVersion version) { return version == OcppProtocolVersion::Unknown; })) {
        throw std::invalid_argument("Ocpp_versions may not contain 'Unknown'");
    }

    set_connection_options_base(connection_options);

    // Set secure URI only if it is in TLS mode
    if (connection_options.security_profile >
        security::SecurityProfile::UNSECURED_TRANSPORT_WITH_BASIC_AUTHENTICATION) {
        this->connection_options.csms_uri.set_secure(true);
    }

    this->connection_attempts = 1; // reset connection attempts
}

static int callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    // Get user safely, since on some callbacks (void *user) can be different than what we set
    if (wsi != nullptr) {
        if (ConnectionData* data = reinterpret_cast<ConnectionData*>(lws_wsi_user(wsi))) {
            auto* owner = data->get_owner();
            if (owner not_eq nullptr) {
                return owner->process_callback(wsi, static_cast<int>(reason), user, in, len);
            } else {
                EVLOG_debug << "callback_minimal called, but data->owner is nullptr. Reason: " << reason;
            }
        }
    }

    return 0;
}

static int private_key_callback(char* buf, int size, int rwflag, void* userdata) {
    const auto* password = static_cast<const std::string*>(userdata);
    const std::size_t max_pass_len = (size - 1); // we exclude the endline
    const std::size_t max_copy_chars =
        std::min(max_pass_len, password->length()); // truncate if pass is too large and buffer too small

    std::memset(buf, 0, size);
    std::memcpy(buf, password->c_str(), max_copy_chars);

    return max_copy_chars;
}

constexpr auto local_protocol_name = "lws-everest-client";
static const struct lws_protocols protocols[] = {{local_protocol_name, callback_minimal, 0, 0, 0, NULL, 0},
                                                 LWS_PROTOCOL_LIST_TERM};

bool WebsocketLibwebsockets::tls_init(SSL_CTX* ctx, const std::string& path_chain, const std::string& path_key,
                                      bool custom_key, std::optional<std::string>& password) {
    auto rc = SSL_CTX_set_cipher_list(ctx, this->connection_options.supported_ciphers_12.c_str());
    if (rc != 1) {
        EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
        EVLOG_error << "Could not set TLSv1.2 cipher list";

        return false;
    }

    rc = SSL_CTX_set_ciphersuites(ctx, this->connection_options.supported_ciphers_13.c_str());
    if (rc != 1) {
        EVLOG_debug << "SSL_CTX_set_cipher_list return value: " << rc;
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    if (this->connection_options.security_profile == 3) {
        if ((path_chain.empty()) || (path_key.empty())) {
            EVLOG_error << "Cert chain: [" << path_chain << "] key: " << path_key << "]";
            EVLOG_error << "No certificate or key found for SSL";

            return false;
        }

        if (1 != SSL_CTX_use_certificate_chain_file(ctx, path_chain.c_str())) {
            ERR_print_errors_fp(stderr);
            EVLOG_error << "Could not use client certificate file within SSL context";

            return false;
        }

        if (password.has_value()) {
            SSL_CTX_set_default_passwd_cb_userdata(ctx, &password.value());
            SSL_CTX_set_default_passwd_cb(ctx, private_key_callback);
        }

        if (1 != SSL_CTX_use_PrivateKey_file(ctx, path_key.c_str(), SSL_FILETYPE_PEM)) {
            ERR_print_errors_fp(stderr);
            EVLOG_error << "Could not set private key file within SSL context";

            return false;
        }

        if (false == SSL_CTX_check_private_key(ctx)) {
            ERR_print_errors_fp(stderr);
            EVLOG_error << "Could not check private key within SSL context";

            return false;
        }
    }

    if (this->evse_security->is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
        std::string ca_csms = this->evse_security->get_verify_location(ocpp::CaCertificateType::CSMS);

        EVLOG_info << "Loading CA csms bundle to verify server certificate: " << ca_csms;

        if (std::filesystem::is_directory(ca_csms)) {
            rc = SSL_CTX_load_verify_locations(ctx, NULL, ca_csms.c_str());
        } else {
            rc = SSL_CTX_load_verify_locations(ctx, ca_csms.c_str(), NULL);
        }

        if (rc != 1) {
            EVLOG_error << "Could not load CA verify locations, error: " << ERR_error_string(ERR_get_error(), NULL);
            return false;
        }
    }

    if (this->connection_options.use_ssl_default_verify_paths) {
        rc = SSL_CTX_set_default_verify_paths(ctx);
        if (rc != 1) {
            EVLOG_error << "Could not load default CA verify path, error: " << ERR_error_string(ERR_get_error(), NULL);
            return false;
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

    return true;
}

void WebsocketLibwebsockets::thread_websocket_message_recv_loop(std::shared_ptr<ConnectionData> local_data) {
    if (local_data == nullptr) {
        EVLOG_AND_THROW(std::runtime_error("Null 'ConnectionData' in message thread, fatal error!"));
    }

    EVLOG_debug << "Init recv loop with ID: " << std::hex << std::this_thread::get_id();

    while (!local_data->is_interupted()) {
        // Process all messages
        while (true) {
            std::string message{};

            {
                if (recv_message_queue.empty())
                    break;

                message = recv_message_queue.pop();
            }

            // Invoke our processing callback, that might trigger a send back that
            // can cause a deadlock if is not managed on a different thread
            this->message_callback(message);
        }

        // While we are empty, sleep, only if we have not been interrupted in the
        // message_callback. An interrupt can be caused in the message callback
        // if we receive a certain message type that will cause the implementation
        // in the charge point to attempt a reconnect (BasicAuthPass for example)
        if (!local_data->is_interupted()) {
            recv_message_queue.wait_on_queue_element(1s);
        }
    }

    EVLOG_debug << "Exit recv loop with ID: " << std::hex << std::this_thread::get_id();
}

bool WebsocketLibwebsockets::initialize_connection_options(std::shared_ptr<ConnectionData>& local_data) {
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

    // Lifetime of this is important since we use the data from this in private_key_callback()
    std::optional<std::string> private_key_password;
    SSL_CTX* ssl_ctx = nullptr;

    if (this->connection_options.security_profile == 2 || this->connection_options.security_profile == 3) {
        // Setup context - need to know the key type first
        std::string path_key;
        std::string path_chain;

        if (this->connection_options.security_profile == 3) {
            const auto certificate_response =
                this->evse_security->get_leaf_certificate_info(CertificateSigningUseEnum::ChargingStationCertificate);

            if (certificate_response.status != ocpp::GetCertificateInfoStatus::Accepted or
                !certificate_response.info.has_value()) {
                EVLOG_error << "Connecting with security profile 3 but no client side certificate is present or valid";
                return false;
            }

            const auto& certificate_info = certificate_response.info.value();

            if (certificate_info.certificate_path.has_value()) {
                path_chain = certificate_info.certificate_path.value();
            } else if (certificate_info.certificate_single_path.has_value()) {
                path_chain = certificate_info.certificate_single_path.value();
            } else {
                EVLOG_error << "Connecting with security profile 3 but no client side certificate is present or valid";
                return false;
            }

            path_key = certificate_info.key_path;
            private_key_password = certificate_info.password;
        }

        bool custom_key = false;

        if (!path_key.empty()) {
            custom_key = is_custom_private_key_file(path_key);
        }

        OpenSSLProvider provider;

        if (custom_key) {
            provider.set_tls_mode(OpenSSLProvider::mode_t::custom_provider);
        } else {
            provider.set_tls_mode(OpenSSLProvider::mode_t::default_provider);
        }

        const SSL_METHOD* method = SSLv23_client_method();
        ssl_ctx = SSL_CTX_new_ex(provider, provider.propquery_tls_str(), method);

        if (ssl_ctx == nullptr) {
            ERR_print_errors_fp(stderr);
            EVLOG_error << "Unable to create ssl context";
            return false;
        }

        if (this->connection_options.enable_tls_keylog and this->connection_options.keylog_file.has_value()) {
            EVLOG_info << "Logging TLS secrets to: " << this->connection_options.keylog_file.value().string();
            keylog_file = this->connection_options.keylog_file;
            SSL_CTX_set_keylog_callback(ssl_ctx, keylog_callback);
        }

        // Init TLS data
        if (!tls_init(ssl_ctx, path_chain, path_key, custom_key, private_key_password)) {
            EVLOG_error << "Unable to init tls security options for websocket";
            return false;
        }

        // Setup our context
        info.provided_client_ssl_ctx = ssl_ctx;
    }

    lws_context* lws_ctx = lws_create_context(&info);
    if (nullptr == lws_ctx) {
        EVLOG_error << "lws create context failed";
        return false;
    }

    // Conn acquire the lws context and security context
    local_data->init_connection_context(lws_ctx, ssl_ctx);
    return true;
}

void WebsocketLibwebsockets::thread_websocket_client_loop(std::shared_ptr<ConnectionData> local_data) {
    if (local_data == nullptr) {
        EVLOG_AND_THROW(std::runtime_error("Null 'ConnectionData' in client thread, fatal error!"));
    }

    EVLOG_info << "Init client loop with ID: " << std::hex << std::this_thread::get_id();
    bool try_reconnect = true;

    do {
        if (!initialize_connection_options(local_data)) {
            EVLOG_error << "Could not initialize connection options.";

            local_data->update_state(EConnectionState::ERROR);
            on_conn_fail(local_data.get());
        } else {
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

                // Debugging if required
                // ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
                // ssl_connection |= LCCSCF_ALLOW_INSECURE;
                // ssl_connection |= LCCSCF_ALLOW_EXPIRED;
            }

            auto& uri = this->connection_options.csms_uri;
            lws* local_lws = nullptr;

            std::string ocpp_versions;
            bool first = true;
            for (auto version : this->connection_options.ocpp_versions) {
                if (!first) {
                    ocpp_versions += ", ";
                }
                first = false;
                ocpp_versions += conversions::ocpp_protocol_version_to_string(version);
            }

            // TODO: No idea who releases the strdup?
            i.context = local_data->lws_ctx.get();
            i.port = uri.get_port();
            i.address = strdup(uri.get_hostname().c_str()); // Base address, as resolved by getnameinfo
            i.path = strdup((uri.get_path() + uri.get_chargepoint_id()).c_str()); // Path of resource
            i.host = i.address;
            i.origin = i.address;
            i.ssl_connection = ssl_connection;
            i.protocol = strdup(ocpp_versions.c_str());
            i.local_protocol_name = local_protocol_name;
            i.pwsi = &local_lws; // Will set the local_data->wsi to a valid value in case of a successful connect
            i.userdata = local_data.get(); // See lws_context 'user'

            if (this->connection_options.iface.has_value()) {
                i.iface = this->connection_options.iface.value().c_str();
            }

            // Print data for debug
            EVLOG_info << "LWS connect with info "
                       << "port: [" << i.port << "] address: [" << i.address << "] path: [" << i.path << "] protocol: ["
                       << i.protocol << "]"
                       << " security profile: [" << this->connection_options.security_profile << "]";

            if (lws_client_connect_via_info(&i) == nullptr) {
                EVLOG_error << "LWS connect failed!";
                // This condition can occur when connecting fails to an IP address
                // retries need to be attempted
                local_data->update_state(EConnectionState::ERROR);
                on_conn_fail(local_data.get());
            } else {
                local_data->init_connection(local_lws);

                // Process while we're running
                int n = 0;
                bool processing = true;

                do {
                    // We can grab the 'state' and 'lws_ctx' members here since we're only
                    // setting them from this thread and not from the exterior

                    // Set to -1 for continuous servicing, if required, not recommended
                    n = lws_service(local_data->lws_ctx.get(), 0);

                    auto state = local_data->state;
                    processing = (!local_data->is_interupted()) &&
                                 (state != EConnectionState::FINALIZED && state != EConnectionState::ERROR);

                    if (processing && !message_queue.empty()) {
                        lws_callback_on_writable(local_data->get_conn());
                    }
                } while (n >= 0 && processing);

                // After this point no minimal_callback can be called, we have finished
                // using the connection information and we will recreate it if required
                local_data->reset_connection_data();
            }
        } // End init connection

        long reconnect_delay = 0;

        if (local_data->is_interupted() || local_data->get_state() == EConnectionState::FINALIZED) {
            EVLOG_info << "Connection interrupted or cleanly finalized, exiting websocket loop";
            try_reconnect = false;
        } else if (local_data->get_state() != EConnectionState::CONNECTED) {
            // Any other failure than a successful connect

            // -1 indicates to always attempt to reconnect
            if (this->connection_options.max_connection_attempts == -1 or
                this->connection_attempts <= this->connection_options.max_connection_attempts) {
                local_data->update_state(EConnectionState::RECONNECTING);
                reconnect_delay = this->get_reconnect_interval();
                try_reconnect = true;

                // Increment reconn attempts
                this->connection_attempts += 1;

                EVLOG_info << "Connection not successful, attempting internal reconnect in: " << reconnect_delay
                           << "ms";
            } else {
                local_data->update_state(EConnectionState::FINALIZED);
                try_reconnect = false;

                EVLOG_info << "Connection reconnect attempts exhausted, exiting websocket loop, passing back control "
                              "to the application logic";
            }
        }

        // Wait until new connection attempt
        if (local_data->get_state() == EConnectionState::RECONNECTING) {
            auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(reconnect_delay);

            while ((std::chrono::steady_clock::now() < end_time) && (false == local_data->is_interupted())) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (true == local_data->is_interupted()) {
                try_reconnect = false;
                EVLOG_info << "Interrupred reconnect attempt, not reconnecting!";
            } else {
                EVLOG_info << "Attempting reconnect after a wait of: " << reconnect_delay << "ms";
            }
        }
    } while (try_reconnect); // End trying to connect

    // Give back control to the application
    if (false == local_data->is_stop_executed()) {
        this->push_deferred_callback([this]() {
            if (this->stopped_connecting_callback) {
                this->stopped_connecting_callback(WebsocketCloseReason::Normal);
            } else {
                EVLOG_error << "Stopped connecting callback not registered!";
            }

            if (this->disconnected_callback) {
                this->disconnected_callback();
            } else {
                EVLOG_error << "Disconnected callback not registered!";
            }
        });
    }

    // Client loop finished for our tid
    EVLOG_info << "Exit websocket client loop with ID: " << std::hex << std::this_thread::get_id();
}

void WebsocketLibwebsockets::clear_all_queues() {
    this->message_queue.clear();
    this->recv_buffered_message.clear();
    this->recv_message_queue.clear();
}

void WebsocketLibwebsockets::safe_close_threads() {
    // If we already have a connection attempt started for now shut it down first
    std::shared_ptr<ConnectionData> local_conn_data = conn_data;
    bool in_message_thread = false;

    if (local_conn_data != nullptr) {
        auto tid = std::this_thread::get_id();

        if (tid == local_conn_data->get_client_thread_id()) {
            EVLOG_AND_THROW(std::runtime_error("Trying to start/stop/reconnect from client thread!"));
        }

        // TODO(ioan): reintroduce this check after we have solved the problem of a main processing
        // loop in the libocpp. The problem is that a start/stop operation is executed from the
        // message thread, because in message_callback we can initiate a reconnect when we receive
        // a message of the type 'BasicAuthPass'. In turn, because there's no main processing
        // loop with it's own thread, the response is polled immediately, and there's special behavior
        // that requires a reconnect that is called from the message thread.
        // The resulting problem is that we could have this thread dangling for a bit, since we are forced to detach

        // else if (std::this_thread::get_id() == local_conn_data->get_message_thread_id()) {
        //      EVLOG_AND_THROW(std::runtime_error("Trying to start/stop/reconnect connection from message thread!")); }

        in_message_thread = (tid == local_conn_data->get_message_thread_id());
        local_conn_data->do_interrupt_and_exit();
    }

    // Clear any pending outgoing/incoming messages on a new connection
    clear_all_queues();

    // Wait old thread for a clean state
    if (this->websocket_thread && this->websocket_thread->joinable()) {
        // Awake libwebsockets thread to quickly exit
        request_write();
        this->websocket_thread->join();
        this->websocket_thread.reset();
    }

    if (in_message_thread) {
        if (this->recv_message_thread) {
            // See the note above 'in_message_thread' on why we detach
            this->recv_message_thread->detach();
            this->recv_message_thread.reset();
        }
    } else {
        if (this->recv_message_thread && this->recv_message_thread->joinable()) {
            // Awake the receiving message thread to finish
            this->recv_message_thread->join();
            this->recv_message_thread.reset();
        }
    }
}

bool WebsocketLibwebsockets::is_trying_to_connect() {
    std::lock_guard lock(this->connection_mutex);
    return is_trying_to_connect_internal();
}

bool WebsocketLibwebsockets::is_trying_to_connect_internal() {
    std::shared_ptr<ConnectionData> local_conn_data = conn_data;
    return (local_conn_data != nullptr) && (local_conn_data->get_state() != EConnectionState::FINALIZED);
}

// Will be called from external threads as well
bool WebsocketLibwebsockets::start_connecting() {
    std::lock_guard lock(this->connection_mutex);

    if (!this->initialized()) {
        EVLOG_error << "Websocket not properly initialized. A reconnect attempt will not be made.";
        return false;
    }

    // Clear shutting down so we allow to reconnect again as well
    this->shutting_down = false;

    EVLOG_info << "Starting connection attempts to uri: " << this->connection_options.csms_uri.string()
               << " with security-profile " << this->connection_options.security_profile
               << (this->connection_options.use_tpm_tls ? " with TPM keys" : "");

    this->connected_ocpp_version = OcppProtocolVersion::Unknown;

    // If we already have a connection attempt started for now shut it down first
    safe_close_threads();

    // Create a new connection data (only created here, owner never changes)
    conn_data = std::make_shared<ConnectionData>(this);

    // Stop any pending reconnect timer
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    this->connection_attempts = 1; // reset connection attempts

    // This should always be running, start it only once
    if (this->deferred_callback_thread == nullptr) {
        this->deferred_callback_thread =
            std::make_unique<std::thread>(&WebsocketLibwebsockets::thread_deferred_callback_queue, this);
    }

    // Release other threads
    this->websocket_thread =
        std::make_unique<std::thread>(&WebsocketLibwebsockets::thread_websocket_client_loop, this, this->conn_data);

    // TODO(ioan): remove this thread when the fix will be moved into 'MessageQueue'
    // The reason for having a received message processing thread is that because
    // if we dispatch a message receive from the client_loop thread, then the callback
    // will send back another message, and since we're waiting for that message to be
    // sent over the wire on the client_loop, not giving the opportunity to the loop to
    // advance we will have a dead-lock
    this->recv_message_thread = std::make_unique<std::thread>(
        &WebsocketLibwebsockets::thread_websocket_message_recv_loop, this, this->conn_data);

    // Bind threads for various checks
    this->conn_data->bind_thread_client(this->websocket_thread->get_id());
    this->conn_data->bind_thread_message(this->recv_message_thread->get_id());

    return true;
}

void WebsocketLibwebsockets::close(const WebsocketCloseReason code, const std::string& reason) {
    std::lock_guard lock(this->connection_mutex);
    close_internal(code, reason);
}

void WebsocketLibwebsockets::close_internal(const WebsocketCloseReason code, const std::string& reason) {
    bool trying_connecting = is_trying_to_connect_internal() || this->m_is_connected;

    if (!trying_connecting) {
        EVLOG_warning << "Trying to close inactive websocket with code: " << (int)code << " and reason: " << reason
                      << ", returning";
        return;
    }

    EVLOG_info << "Closing websocket with code: " << (int)code << " and reason: " << reason;

    // Close any ongoing thread
    safe_close_threads();

    // Release the connection data and state
    conn_data.reset();
    this->m_is_connected = false;

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }
}

void WebsocketLibwebsockets::reconnect(long delay) {
    std::lock_guard lock(this->connection_mutex);

    if (this->shutting_down) {
        EVLOG_info << "Not reconnecting because the websocket is being shutdown.";
        return;
    }

    if (this->m_is_connected || is_trying_to_connect_internal()) {
        this->close_internal(WebsocketCloseReason::AbnormalClose, "before manually reconnecting");
    }

    EVLOG_info << "Externally called reconnect in: " << delay << "ms"
               << ", attempt: " << this->connection_attempts;

    std::lock_guard<std::mutex> lk(this->reconnect_mutex);
    this->reconnect_timer_tpm.timeout(
        [this]() {
            // close connection before reconnecting
            if (this->m_is_connected || is_trying_to_connect()) {
                this->close(WebsocketCloseReason::AbnormalClose, "before manually reconnecting");
            }

            this->start_connecting();
        },
        std::chrono::milliseconds(delay));
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

void WebsocketLibwebsockets::request_write() {
    if (this->m_is_connected) {
        std::shared_ptr<ConnectionData> local_data = conn_data;

        if (local_data != nullptr) {
            // Notify waiting processing thread to wake up. According to docs
            // it is ok  to call from another thread.
            local_data->request_awake();
        }
    } else {
        EVLOG_debug << "Requested write with offline TLS websocket!";
    }
}

void WebsocketLibwebsockets::poll_message(const std::shared_ptr<WebsocketMessage>& msg) {
    if (this->m_is_connected == false) {
        EVLOG_debug << "Trying to poll message without being connected!";
        return;
    }

    std::shared_ptr<ConnectionData> local_data = conn_data;

    if (local_data != nullptr) {
        auto cd_tid = local_data->get_client_thread_id();

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
    message_queue.push(msg);

    // Request a write callback
    request_write();

    message_queue.wait_on_custom_event([&] { return (true == msg->message_sent); },
                                       std::chrono::seconds(MESSAGE_SEND_TIMEOUT_S));

    if (msg->message_sent) {
        EVLOG_debug << "Successfully sent last message over TLS websocket!";
    } else {
        EVLOG_warning << "Could not send last message over TLS websocket!";
    }
}

// Will be called from external threads
bool WebsocketLibwebsockets::send(const std::string& message) {
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

void WebsocketLibwebsockets::ping() {
    if (!this->initialized()) {
        EVLOG_error << "Could not send ping because websocket is not properly initialized.";
    }

    auto msg = std::make_shared<WebsocketMessage>();
    msg->payload = this->connection_options.ping_payload;
    msg->protocol = LWS_WRITE_PING;

    poll_message(msg);
}

int WebsocketLibwebsockets::process_callback(void* wsi_ptr, int callback_reason, void* user, void* in, size_t len) {
    enum lws_callback_reasons reason = static_cast<lws_callback_reasons>(callback_reason);

    lws* wsi = reinterpret_cast<lws*>(wsi_ptr);

    // The ConnectionData is thread bound, so that if we clear it in the 'WebsocketLibwebsockets'
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

            if (!verify_csms_cn(this->connection_options.csms_uri.get_hostname(), (len == 1),
                                reinterpret_cast<X509_STORE_CTX*>(user),
                                this->connection_options.verify_csms_allow_wildcards)) {
                this->push_deferred_callback([this]() {
                    if (this->connection_failed_callback) {
                        this->connection_failed_callback(ConnectionFailedReason::InvalidCSMSCertificate);
                    } else {
                        EVLOG_error << "Connection failed callback not registered!";
                    }
                });
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

            if (0 != lws_add_http_header_by_name(wsi, reinterpret_cast<const unsigned char*>("User-Host"),
                                                 reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), ptr,
                                                 end_header)) {
                EVLOG_AND_THROW(std::runtime_error("Could not append User-Host header."));
            }
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

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
        std::string error_message = (in ? reinterpret_cast<char*>(in) : "(null)");
        EVLOG_error << "CLIENT_CONNECTION_ERROR: [" << error_message << "]. Attempting reconnect";
        ERR_print_errors_fp(stderr);

        if (error_message.find("HS: ws upgrade unauthorized") != std::string::npos) {
            this->push_deferred_callback([this]() {
                if (this->connection_failed_callback) {
                    this->connection_failed_callback(ConnectionFailedReason::FailedToAuthenticateAtCsms);
                } else {
                    EVLOG_error << "Connection failed callback not registered!";
                }
            });
        }

        data->update_state(EConnectionState::ERROR);
        on_conn_fail(data);

        return 0;
    }
    case LWS_CALLBACK_CONNECTING:
        EVLOG_debug << "Client connecting...";
        data->update_state(EConnectionState::CONNECTING);
        break;

    case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
        try {
            char buffer[16] = {0};
            lws_hdr_copy(wsi, buffer, 16, WSI_TOKEN_PROTOCOL);
            this->connected_ocpp_version = ocpp::conversions::string_to_ocpp_protocol_version(buffer);
        } catch (StringToEnumException& e) {
            EVLOG_warning << "CSMS did not select protocol: " << e.what();
            this->connected_ocpp_version = OcppProtocolVersion::Unknown;
        }
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        data->update_state(EConnectionState::CONNECTED);
        on_conn_connected(data);

        // Attempt first write after connection
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: {
        std::string close_reason(reinterpret_cast<char*>(in), len);
        unsigned char* pp = reinterpret_cast<unsigned char*>(in);
        unsigned short close_code = (unsigned short)((pp[0] << 8) | pp[1]);

        // In the case that the websocket (server) has closed the
        // connection we  must ALWAYS try to reconnect
        EVLOG_info << "Websocket peer initiated close with reason: [" << close_reason << "] close code: [" << close_code
                   << "]. Reconnecting";
        data->update_state(EConnectionState::ERROR);
        on_conn_fail(data);

        // Return 0 to print peer close reason
        return 0;
    }

    case LWS_CALLBACK_CLIENT_CLOSED:
        // Determine if the close connection was requested or if the server went away
        // case in which we receive a 'LWS_CALLBACK_CLIENT_CLOSED' that was not requested
        if (data->is_interupted()) {
            EVLOG_info << "Client closed, was requested internally, finalizing connection, not reconnecting";
            data->update_state(EConnectionState::FINALIZED);
            on_conn_close(data);
        } else {
            EVLOG_info << "Client closed, was not requested internally, attempting reconnection";
            // It means the server went away, attempt to reconnect
            data->update_state(EConnectionState::ERROR);
            on_conn_fail(data);
        }

        break;

    case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        on_conn_writable();
        if (false == message_queue.empty()) {
            lws_callback_on_writable(wsi);
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE_PONG: {
        if (false == message_queue.empty()) {
            lws_callback_on_writable(data->get_conn());
        }
    } break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        recv_buffered_message.append(reinterpret_cast<char*>(in), reinterpret_cast<char*>(in) + len);

        // Message is complete
        if (lws_remaining_packet_payload(wsi) <= 0) {
            on_conn_message(std::move(recv_buffered_message));
            recv_buffered_message.clear();
        }

        if (false == message_queue.empty()) {
            lws_callback_on_writable(data->get_conn());
        }
        break;

    case LWS_CALLBACK_EVENT_WAIT_CANCELLED: {
        if (false == message_queue.empty()) {
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

void WebsocketLibwebsockets::on_conn_connected(ConnectionData* conn_data) {
    // Called on the websocket client thread
    EVLOG_info << "OCPP client successfully connected to server with version: " << this->connected_ocpp_version;

    this->connection_attempts = 1; // reset connection attempts
    this->m_is_connected = true;
    this->reconnecting = false;

    // Stop any dangling reconnect
    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    // Clear any irrelevant data after a DC
    clear_all_queues();

    this->push_deferred_callback([this]() {
        if (connected_callback) {
            this->connected_callback(this->connected_ocpp_version);
        } else {
            EVLOG_error << "Connected callback not registered!";
        }
    });
}

void WebsocketLibwebsockets::on_conn_close(ConnectionData* conn_data) {
    // Called on the websocket client thread
    EVLOG_info << "OCPP client closed connection to server";

    this->m_is_connected = false;

    {
        std::lock_guard<std::mutex> lk(this->reconnect_mutex);
        this->reconnect_timer_tpm.stop();
    }

    // Clear any irrelevant data after a DC
    clear_all_queues();

    this->push_deferred_callback([this]() {
        if (this->stopped_connecting_callback) {
            this->stopped_connecting_callback(WebsocketCloseReason::Normal);
        } else {
            EVLOG_error << "Stopped connecting callback not registered!";
        }

        if (this->disconnected_callback) {
            this->disconnected_callback();
        } else {
            EVLOG_error << "Disconnected callback not registered!";
        }
    });

    // We have polled the stopped connected
    conn_data->mark_stop_executed();
}

void WebsocketLibwebsockets::on_conn_fail(ConnectionData* conn_data) {
    // Called on the websocket client thread
    EVLOG_error << "OCPP client connection to server failed";

    if (this->m_is_connected) {
        this->push_deferred_callback([this]() {
            if (this->disconnected_callback) {
                this->disconnected_callback();
            } else {
                EVLOG_error << "Disconnected callback not registered!";
            }
        });
    }

    this->m_is_connected = false;

    // Clear any irrelevant data after a DC
    clear_all_queues();

    // TODO: See if this is required for a faster fail
    // lws_set_timeout(conn_data->get_conn(), (enum pending_timeout)1, LWS_TO_KILL_ASYNC);
}

void WebsocketLibwebsockets::on_conn_message(std::string&& message) {
    // Called on the websocket client thread
    if (!this->initialized()) {
        EVLOG_error << "Message received but TLS websocket has not been correctly initialized. Discarding message.";
        return;
    }

    EVLOG_debug << "Received message over TLS websocket polling for process: " << message;
    recv_message_queue.push(std::move(message));
}

void WebsocketLibwebsockets::on_conn_writable() {
    // Called on the websocket client thread
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
        // Break if we have en empty queue
        if (message_queue.empty())
            break;

        auto message = message_queue.front();

        if (message == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Null message in queue, fatal error!"));
        }

        // This message was polled in a previous iteration
        if (message->sent_bytes >= message->payload.length()) {
            EVLOG_debug << "Websocket message fully written, popping processing thread from queue!";

            // If we have written all bytes to libwebsockets it means that if we received
            // this writable callback everything is sent over the wire, mark it as sent and remove
            message->message_sent = true;
            message_queue.pop();
        } else {
            // If the message was not polled, we reached the first unpolled and break
            break;
        }
    }

    // If we still have message ONLY poll a single one that can be processed in the invoke of the function
    // libwebsockets is designed so that when a message is sent to the wire from the internal buffer it
    // will invoke 'on_conn_writable' again and we can execute the code above
    if (!message_queue.empty()) {
        // Poll a single message
        EVLOG_debug << "Client writable, sending message part!";

        auto message = message_queue.front();

        if (message == nullptr) {
            EVLOG_AND_THROW(std::runtime_error("Null message in queue, fatal error!"));
        }

        if (message->sent_bytes >= message->payload.length()) {
            EVLOG_AND_THROW(std::runtime_error("Already polled message should be handled above, fatal error!"));
        }

        // Continue sending message part, for a single message only
        bool sent = send_internal(local_data->get_conn(), message.get());

        // If we failed, attempt again later
        if (!sent) {
            message->sent_bytes = 0;
        }
    }
}

void WebsocketLibwebsockets::push_deferred_callback(const std::function<void()>& callback) {
    if (!callback) {
        EVLOG_error << "Attempting to push stale callback in deferred queue!";
        return;
    }

    this->deferred_callback_queue.push(callback);
}

void WebsocketLibwebsockets::thread_deferred_callback_queue() {
    while (true) {
        std::function<void()> callback;
        {
            this->deferred_callback_queue.wait_on_queue_element_or_predicate(
                [this]() { return this->stop_deferred_handler.load(); });

            if (stop_deferred_handler and this->deferred_callback_queue.empty()) {
                break;
            }

            callback = this->deferred_callback_queue.pop();
        }

        // This needs to be out of lock scope otherwise we still keep the mutex locked while executing the callback.
        // This would block the callers of push_deferred_callback()
        if (callback) {
            callback();
        } else {
            EVLOG_error << "Stale callback in deferred queue!";
        }
    }
}

} // namespace ocpp
