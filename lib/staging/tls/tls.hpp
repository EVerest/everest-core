// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef TLS_HPP_
#define TLS_HPP_

#include "openssl_util.hpp"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <openssl/types.h>
#include <string>
#include <tuple>
#include <unistd.h>

#include <EnumFlags.hpp>

struct ocsp_response_st;
struct ssl_ctx_st;
struct ssl_st;
struct x509_st;

namespace tls {

constexpr int INVALID_SOCKET{-1};

using Certificate = struct ::x509_st;
using OcspResponse = struct ::ocsp_response_st;
using Ssl = struct ::ssl_st;
using SslContext = struct ::ssl_ctx_st;

struct connection_ctx;
struct ocsp_cache_ctx;
struct server_ctx;
struct client_ctx;

// ----------------------------------------------------------------------------
// ConfigItem - store configuration item allowing nullptr

/**
 * \brief class to hold configuration strings, behaves like const char *
 *        but keeps a copy
 *
 * unlike std::string this class allows nullptr as a valid setting.
 *
 * unlike const char * it doesn't have scope issues since it holds
 * a copy.
 */
class ConfigItem {
private:
    char* m_ptr{nullptr};

public:
    ConfigItem() = default;
    ConfigItem(const char* value); // must not be explicit
    ConfigItem& operator=(const char* value);
    ConfigItem(const ConfigItem& obj);
    ConfigItem& operator=(const ConfigItem& obj);
    ConfigItem(ConfigItem&& obj) noexcept;
    ConfigItem& operator=(ConfigItem&& obj) noexcept;

    ~ConfigItem();

    inline operator const char*() const {
        return m_ptr;
    }

    bool operator==(const char* ptr) const;

    inline bool operator!=(const char* ptr) const {
        return !(*this == ptr);
    }

    inline bool operator==(const ConfigItem& obj) const {
        return *this == obj.m_ptr;
    }

    inline bool operator!=(const ConfigItem& obj) const {
        return !(*this == obj);
    }
};

// ----------------------------------------------------------------------------
// Cache of OCSP responses for status_request and status_request_v2 extensions

/**
 * \brief cache of OCSP responses
 * \note responses can be updated at any time via load()
 */
class OcspCache {
public:
    using ocsp_entry_t = std::tuple<openssl::sha_256_digest_t, const char*>;

private:
    std::unique_ptr<ocsp_cache_ctx> m_context;
    std::mutex mux; //!< protects the cached OCSP responses

public:
    OcspCache();
    OcspCache(const OcspCache&) = delete;
    OcspCache(OcspCache&&) = delete;
    OcspCache& operator=(const OcspCache&) = delete;
    OcspCache& operator=(OcspCache&&) = delete;
    ~OcspCache();

    bool load(const std::vector<ocsp_entry_t>& filenames);
    std::shared_ptr<OcspResponse> lookup(const openssl::sha_256_digest_t& digest);
    static bool digest(openssl::sha_256_digest_t& digest, const x509_st* cert);
};

// ----------------------------------------------------------------------------
// TLS handshake extension status_request amd status_request_v2 support

/**
 * \brief TLS status_request and status_request_v2 support
 */
class CertificateStatusRequestV2 {
private:
    OcspCache& m_cache;

public:
    explicit CertificateStatusRequestV2(OcspCache& cache) : m_cache(cache) {
    }
    CertificateStatusRequestV2() = delete;
    CertificateStatusRequestV2(const CertificateStatusRequestV2&) = delete;
    CertificateStatusRequestV2(CertificateStatusRequestV2&&) = delete;
    CertificateStatusRequestV2& operator=(const CertificateStatusRequestV2&) = delete;
    CertificateStatusRequestV2& operator=(CertificateStatusRequestV2&&) = delete;
    ~CertificateStatusRequestV2() = default;

    /**
     * \brief set the OCSP reponse for the SSL context
     * \param[in] digest the certificate requested
     * \param[in] ctx the connection context
     * \return true on success
     * \return for status_request extension
     */
    bool set_ocsp_response(const openssl::sha_256_digest_t& digest, Ssl* ctx);

    /**
     * \brief the OpenSSL callback for the status_request extension
     * \param[in] ctx the connection context
     * \param[in] object the instance of a CertificateStatusRequest
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    static int status_request_cb(Ssl* ctx, void* object);

    /**
     * \brief set the OCSP reponse for the SSL context
     * \param[in] digest the certificate requested
     * \param[in] ctx the connection context
     * \return true on success
     * \return for status_request_v2 extension
     */
    bool set_ocsp_v2_response(const std::vector<openssl::sha_256_digest_t>& digests, Ssl* ctx);

    /**
     * \brief add status_request_v2 extension to server hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] outlen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, do not include = 0, error  == -1
     */
    static int status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                     std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                     void* object);

    /**
     * \brief free status_request_v2 extension added to server hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] object the instance of a CertificateStatusRequestV2
     */
    static void status_request_v2_free(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* out,
                                       void* object);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] data pointer to the extension data
     * \param[in] datalen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, error = zero or negative
     */
    static int status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                    std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                    void* object);

    /**
     * \brief the OpenSSL callback for the client hello record
     * \param[in] ctx the connection context
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, error = zero or negative
     *
     * This callback has early access to the extensions requested by the client.
     * It is used to determine whether status_request and status_request_v2
     * have been requested so that status_request_v2 can take priority.
     */
    static int client_hello_cb(Ssl* ctx, int* alert, void* object);
};

// ----------------------------------------------------------------------------
// Connection represents a TLS connection

/**
 * \brief class representing a TLS connection
 */
class Connection {
public:
    /**
     * \brief connection state
     */
    enum class state_t : std::uint8_t {
        idle,      //!< no connection in progress
        connected, //!< active connection
        closed,    //!< connection has closed
        fault,     //!< connection has faulted
    };

    enum class result_t : std::uint8_t {
        success,
        error,
        timeout,
    };

protected:
    std::unique_ptr<connection_ctx> m_context;
    state_t m_state{state_t::idle};
    std::string m_ip;
    std::string m_service;
    std::int32_t m_timeout_ms;

public:
    Connection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms);
    Connection() = delete;
    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;
    ~Connection();

    /**
     * \brief read bytes from the TLS connection
     * \param[out] buf pointer to output buffer
     * \param[in] num size of output buffer
     * \param[out] readBytes number of received bytes. May be less than num
     *             when there has been a timeout
     * \return success, error, or timeout. On error the connection will have been closed
     */
    [[nodiscard]] result_t read(std::byte* buf, std::size_t num, std::size_t& readbytes);

    /**
     * \brief write bytes to the TLS connection
     * \param[in] buf pointer to input buffer
     * \param[in] num size of input buffer
     * \param[out] writeBytes number of sent bytes. May be less than num
     *             when there has been a timeout
     * \return success, error, or timeout. On error the connection will have been closed
     */
    [[nodiscard]] result_t write(const std::byte* buf, std::size_t num, std::size_t& writebytes);

    /**
     * \brief close the TLS connection
     */
    void shutdown();

    /**
     * IP address of the connection's peer
     */
    [[nodiscard]] const std::string& ip_address() const {
        return m_ip;
    }

    /**
     * Service (port number) of the connection's peer
     */
    [[nodiscard]] const std::string& service() const {
        return m_service;
    }

    /**
     * \brief return the current state
     * \return the current state
     */
    [[nodiscard]] state_t state() const {
        return m_state;
    }

    /**
     * \brief obtain the underlying socket for use with poll or select
     * \returns the underlying socket or INVALID_SOCKET on error
     */
    [[nodiscard]] int socket() const;

    /**
     * \brief obtain the underlying SSL context
     * \returns the underlying SSL context pointer
     */
    [[nodiscard]] SSL* ssl_context() const;

    /**
     * \brief set the read timeout in ms
     */
    void set_read_timeout(int ms) {
        m_timeout_ms = ms;
    }
};

/**
 * \brief class representing a TLS connection
 */
class ServerConnection : public Connection {
private:
    static std::uint32_t m_count;
    static std::mutex m_cv_mutex;
    static std::condition_variable m_cv;

    enum class flags_t : std::uint8_t {
        status_request,
        status_request_v2,
        last = status_request_v2,
    };

    util::AtomicEnumFlags<flags_t, std::uint8_t> flags;

public:
    ServerConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms);
    ServerConnection() = delete;
    ServerConnection(const ServerConnection&) = delete;
    ServerConnection(ServerConnection&&) = delete;
    ServerConnection& operator=(const ServerConnection&) = delete;
    ServerConnection& operator=(ServerConnection&&) = delete;
    ~ServerConnection();

    void status_request_received() {
        flags.set(flags_t::status_request);
    }
    void status_request_v2_received() {
        flags.set(flags_t::status_request_v2);
    }
    [[nodiscard]] bool has_status_request() const {
        return flags.is_set(flags_t::status_request);
    }
    [[nodiscard]] bool has_status_request_v2() const {
        return flags.is_set(flags_t::status_request_v2);
    }

    /**
     * \brief accept the incoming connection and run the TLS handshake
     * \return true when the TLS connection has been established
     */
    [[nodiscard]] bool accept();

    /**
     * \brief wait for all connections to be closed
     */
    static void wait_all_closed();

    /**
     * \brief return number of active connections (indicative only)
     * \return number of active connections
     */
    static std::uint32_t active_connections() {
        return m_count;
    }
};

/**
 * \brief class representing a TLS connection
 */
class ClientConnection : public Connection {
public:
    ClientConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms);
    ClientConnection() = delete;
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection(ClientConnection&&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;
    ClientConnection& operator=(ClientConnection&&) = delete;
    ~ClientConnection();

    /**
     * \brief run the TLS handshake
     * \return true when the TLS connection has been established
     */
    [[nodiscard]] bool connect();
};

// ----------------------------------------------------------------------------
// TLS server

/**
 * \brief represents a TLS server
 *
 * The TLS server does not make use of any threads. This is to support multiple
 * use cases giving developers the option on how best to support multiple
 * incoming connections.
 *
 * Example code in tests has some examples on how this can be done.
 *
 * One option is to have a server thread calling Server.serve() with the supplied
 * handler creating a new connection thread when a connection arrives.
 *
 * For unit tests the connection handler can perform the test directly which
 * has the advantage of preventing new connections from being accepted.
 *
 * Another option is for the connection handler to add the new connection to a list
 * which is serviced by an event handler - i.e. one thread could manage all connections.
 */
class Server {
public:
    /**
     * \brief server state
     */
    enum class state_t : std::uint8_t {
        init_needed,   //!< not initialised yet - call init()
        init_socket,   //!< TCP listen socket initialised (but not SSL) - call update()
        init_complete, //!< initialised but not running - call serve()
        running,       //!< waiting for connections - fully initialised
        stopped,       //!< stopped - reinitialisation will be needed
    };

    struct config_t {
        ConfigItem cipher_list{nullptr};  // nullptr means use default
        ConfigItem ciphersuites{nullptr}; // nullptr means use default, "" disables TSL 1.3
        ConfigItem certificate_chain_file{nullptr};
        ConfigItem private_key_file{nullptr};
        ConfigItem private_key_password{nullptr};
        ConfigItem verify_locations_file{nullptr};   // for client certificate
        ConfigItem verify_locations_path{nullptr};   // for client certificate
        ConfigItem host{nullptr};                    // see BIO_lookup_ex()
        ConfigItem service{nullptr};                 // TLS port number
        std::vector<ConfigItem> ocsp_response_files; // in certificate chain order
        int socket{INVALID_SOCKET};     // use this specific socket - bypasses socket setup in init_socket() when set
        std::int32_t io_timeout_ms{-1}; // socket timeout in milliseconds
        bool ipv6_only{true};
        bool verify_client{true};
    };

private:
    std::unique_ptr<server_ctx> m_context;
    int m_socket{INVALID_SOCKET};
    bool m_running{false};
    std::int32_t m_timeout_ms{-1};
    std::atomic_bool m_exit{false};
    std::atomic<state_t> m_state{state_t::init_needed};
    std::mutex m_mutex;
    std::mutex m_cv_mutex;
    std::condition_variable m_cv;
    OcspCache m_cache;
    CertificateStatusRequestV2 m_status_request_v2;
    std::function<bool(Server& server)> m_init_callback{nullptr};

    /**
     * \brief initialise the server socket
     * \param[in] cfg server configuration
     * \return true on success
     */
    bool init_socket(const config_t& cfg);

    /**
     * \brief initialise TLS configuration
     * \param[in] cfg server configuration
     * \return true on success
     */
    bool init_ssl(const config_t& cfg);

public:
    Server();
    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;
    ~Server();

    /**
     * \brief initialise the server socket and TLS configuration
     * \param[in] cfg server configuration
     * \param[in] init_ssl function to collect certificates and keys, can be nullptr
     * \return need_init - initialisation failed
     *         socket_init - server socket created and ready for serve()
     *         init_complete - SSL certificates and keys loaded
     *
     * It is possible to initialise the server and start listening for
     * connections before certificates and keys are available.
     * when init() returns socket_init the server will call init_ssl() with a
     * reference to the object so that update() can be called with updated
     * OCSP and SSL configuration.
     *
     * init_ssl() should return true when SSL has been configured so that the
     * incoming connection is accepted.
     */
    state_t init(const config_t& cfg, const std::function<bool(Server& server)>& init_ssl);

    /**
     * \brief update the OCSP cache and SSL certificates and keys
     * \param[in] cfg server configuration
     * \return true on success
     * \note used to update OCSP caches and SSL config
     */
    bool update(const config_t& cfg);

    /**
     * \brief wait for incomming connections
     * \param[in] handler called when there is a new connection
     * \return stopped after it has been running, or init_ values when listening
     *         can not start
     * \note this is a blocking call that will not return until stop() has been
     *       called (unless it couldn't start listening)
     * \note changing socket configuration requires stopping the server and
     *       calling init()
     * \note after server() returns stopped init() will need to be called
     *       before further connections can be managed
     */
    state_t serve(const std::function<void(std::shared_ptr<ServerConnection>& ctx)>& handler);

    /**
     * \brief stop listening for new connections
     * \note returns immediately
     */
    void stop();

    /**
     * \brief wait for server to start listening for connections
     * \note blocks until server is running
     */
    void wait_running();

    /**
     * \brief wait for server to stop
     * \note blocks until server has stopped
     */
    void wait_stopped();

    /**
     * \brief return the current server state (indicative only)
     * \return current server state
     */
    [[nodiscard]] state_t state() const {
        return m_state;
    }
};

// ----------------------------------------------------------------------------
// TLS client

/**
 * \brief represents a TLS client
 */
class Client {
public:
    /**
     * \brief client state
     */
    enum class state_t : std::uint8_t {
        need_init, //!< not initialised yet
        init,      //!< initialised but not connected
        connected, //!< connected to server
        stopped,   //!< stopped
    };

    struct config_t {
        const char* cipher_list{nullptr};  // nullptr means use default
        const char* ciphersuites{nullptr}; // nullptr means use default, "" disables TSL 1.3
        const char* certificate_chain_file{nullptr};
        const char* private_key_file{nullptr};
        const char* private_key_password{nullptr};
        const char* verify_locations_file{nullptr}; // for client certificate
        const char* verify_locations_path{nullptr}; // for client certificate
        std::int32_t io_timeout_ms{-1};             // socket timeout in milliseconds
        bool verify_server{true};
        bool status_request{false};
        bool status_request_v2{false};
    };

private:
    std::unique_ptr<client_ctx> m_context;
    std::int32_t m_timeout_ms{-1};
    std::atomic<state_t> m_state{state_t::need_init};

public:
    Client();
    Client(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;
    virtual ~Client();

    static bool print_ocsp_response(FILE* stream, const unsigned char*& response, std::size_t length);

    /**
     * \brief initialise the client socket and TLS configuration
     * \param[in] cfg server configuration
     * \return true on success
     * \note when the server certificate and key change then the client needs
     *       to be stopped, initialised and start serving.
     */
    bool init(const config_t& cfg);

    /**
     * \brief connect to server
     * \param[in] host host to connect to
     * \param[in] service port to connect to
     * \param[in] ipv6_only false - also support IPv4
     * \return a connection pointer (nullptr on error)
     */
    std::unique_ptr<ClientConnection> connect(const char* host, const char* service, bool ipv6_only);

    /**
     * \brief return the current server state (indicative only)
     * \return current server state
     */
    [[nodiscard]] state_t state() const {
        return m_state;
    }

    /**
     * \brief the OpenSSL callback for the status_request and status_request_v2 extensions
     * \param[in] ctx the connection context
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    virtual int status_request_cb(Ssl* ctx);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] object the instance of a CertificateStatusRequest
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    static int status_request_v2_multi_cb(Ssl* ctx, void* object);

    /**
     * \brief add status_request_v2 extension to client hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] outlen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, do not include = 0, error  == -1
     */
    static int status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                     std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                     void* object);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] data pointer to the extension data
     * \param[in] datalen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, error = zero or negative
     */
    static int status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                    std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                    void* object);
};

} // namespace tls

#endif // TLS_HPP_
