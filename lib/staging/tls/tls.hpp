// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef TLS_HPP_
#define TLS_HPP_

#include "extensions/status_request.hpp"
#include "extensions/tls_types.hpp"
#include "extensions/trusted_ca_keys.hpp"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <openssl/types.h>
#include <optional>
#include <pthread.h>
#include <string>
#include <unistd.h>

namespace tls {

constexpr int INVALID_SOCKET{-1};
constexpr std::uint32_t c_shutdown_timeout_ms = 5000; // 5 seconds
constexpr std::uint32_t c_serve_timeout_ms = 60000;   // 60 seconds

struct connection_ctx;
struct server_ctx;
struct client_ctx;

// ----------------------------------------------------------------------------
// ConfigItem - store configuration item allowing nullptr

/**
 * \brief class to hold configuration strings, behaves like const char *
 *        but keeps a copy
 *
 * unlike std::string this class allows nullptr as a valid setting and it
 * doesn't have scope issues since it holds a copy.
 */
class ConfigItem {
private:
    std::optional<std::string> value{};

public:
    ConfigItem() = default;
    ConfigItem(const char* ptr) {
        if (ptr != nullptr) {
            value = ptr;
        }
    }
    inline operator const char*() const {
        return (value) ? value.value().c_str() : nullptr;
    }
};

// ----------------------------------------------------------------------------
// Connection represents a TLS connection

/**
 * \brief class representing a TLS connection
 * \note small timeout values (under 200ms) can cause connections to fail
 *       even on the loopback interface.
 *
 * Non-blocking I/O is configured on the underlying socket. Its use is controlled
 * by the 'timeout_ms' parameter:
 *
 * timeout_ms is -1 means wait forever. The method will block until there is an
 * event on the socket.

 * timeout_ms is 0 means don't wait. The method will return want_read or
 * want_write when it is unable to complete the operation. The method should be
 * called with the same arguments once read/write is available on the socket -
 * via select() or poll(). The underlying socket is available via socket().
 * This supports waiting for events on many sockets.
 * A helper method wait_for() is provided where waiting on only one socket is
 * needed.
 *
 * timeout_ms > 0 means wait no more than timeout_ms.
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
        success,    //!< operation completed successfully
        closed,     //!< connection closed (possibly due to error)
        timeout,    //!< operation timed out
        want_read,  //!< non-blocking - operation waiting for read available on socket
        want_write, //!< non-blocking - operation waiting for write available on socket
    };

protected:
    std::unique_ptr<connection_ctx> m_context; //!< opaque connection data
    state_t m_state{state_t::idle};            //!< connection state
    std::string m_ip;                          //!< peer IP address
    std::string m_service;                     //!< peer port
    std::int32_t m_timeout_ms;                 //!< default operation timeout

    // prevent standalone construction
    Connection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms);

public:
    Connection() = delete;
    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;
    ~Connection();

    /**
     * \brief change the default operation timeout
     * \param[in] timeout_ms new timeout value
     */
    void timeout(std::int32_t timeout_ms) {
        m_timeout_ms = timeout_ms;
    }

    /**
     * \brief get the default operation timeout
     * \return timeout value
     */
    [[nodiscard]] std::int32_t timeout() const {
        return m_timeout_ms;
    }

    /**
     * \brief read bytes from the TLS connection
     * \param[out] buf pointer to output buffer
     * \param[in] num size of output buffer
     * \param[in] timeout_ms time to wait in milliseconds, -1 is wait forever, 0 is don't wait
     * \param[out] readBytes number of received bytes. May be less than num
     *             when there has been a timeout
     * \return see result_t. On error the connection will have
     *         been closed
     */
    [[nodiscard]] result_t read(std::byte* buf, std::size_t num, std::size_t& readbytes, int timeout_ms);
    [[nodiscard]] inline result_t read(std::byte* buf, std::size_t num, std::size_t& readbytes) {
        return read(buf, num, readbytes, m_timeout_ms);
    }

    /**
     * \brief write bytes to the TLS connection
     * \param[in] buf pointer to input buffer
     * \param[in] num size of input buffer
     * \param[in] timeout_ms time to wait in milliseconds, -1 is wait forever, 0 is don't wait
     * \param[out] writeBytes number of sent bytes. May be less than num
     *             when there has been a timeout
     * \return see result_t. On error the connection will have
     *         been closed
     */
    [[nodiscard]] result_t write(const std::byte* buf, std::size_t num, std::size_t& writebytes, int timeout_ms);
    [[nodiscard]] inline result_t write(const std::byte* buf, std::size_t num, std::size_t& writeBytes) {
        return write(buf, num, writeBytes, m_timeout_ms);
    }

    /**
     * \brief close the TLS connection
     * \param[in] timeout_ms time to wait in milliseconds
     * \return see result_t
     * \note result_t::closed is more likely than result_t::success since the
     *       connection is closed.
     */
    result_t shutdown(int timeout_ms);
    inline result_t shutdown() {
        return shutdown(c_shutdown_timeout_ms);
    }

    /**
     * \brief wait for activity on the socket
     * \param[in] action is result_t::want_read or result_t::want_write
     * \param[in] timeout_ms time to wait in milliseconds, -1 is wait forever, 0 is don't wait
     * \return see result_t
     */
    result_t wait_for(result_t action, int timeout_ms);
    inline result_t wait_for(result_t action) {
        return wait_for(action, m_timeout_ms);
    }

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
     * \brief obtain the underlying socket for use with poll() or select()
     * \returns the underlying socket or INVALID_SOCKET on error
     */
    [[nodiscard]] int socket() const;

    /**
     * \brief obtain the peer certificate
     * \returns the certificate or nullptr
     * \note the certificate must not be freed
     */
    [[nodiscard]] const Certificate* peer_certificate() const;

    /**
     * \brief obtain the underlying SSL context
     * \returns the underlying SSL context pointer
     */
    [[nodiscard]] [[deprecated(
        "Temporarily used with IsoMux module. Will be removed together with IsoMux module in the future.")]] SSL*
    ssl_context() const;

    /**
     * \brief set the read timeout in ms
     */
    [[deprecated(
        "Temporarily used with IsoMux module. Will be removed together with IsoMux module in the future.")]] void
    set_read_timeout(int ms) {
        m_timeout_ms = ms;
    }
};

/**
 * \brief class representing a TLS connection (server side)
 */
class ServerConnection : public Connection {
private:
    using ServerStatusRequestV2 = status_request::ServerStatusRequestV2;
    using ServerTrustedCaKeys = trusted_ca_keys::ServerTrustedCaKeys;
    using trusted_ca_keys_t = trusted_ca_keys::trusted_ca_keys_t;
    using server_trusted_ca_keys_t = trusted_ca_keys::server_trusted_ca_keys_t;

    static std::uint32_t m_count;        //!< number of active connections
    static std::mutex m_cv_mutex;        //!< used for wait_all_closed()
    static std::condition_variable m_cv; //!< used for wait_all_closed()
    trusted_ca_keys_t m_trusted_ca_keys; //!< trusted CA keys configuration
    StatusFlags m_flags;                 //!< extension flags
    server_trusted_ca_keys_t m_tck_data; //!< extension per connection data

public:
    ServerConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms);
    ServerConnection() = delete;
    ServerConnection(const ServerConnection&) = delete;
    ServerConnection(ServerConnection&&) = delete;
    ServerConnection& operator=(const ServerConnection&) = delete;
    ServerConnection& operator=(ServerConnection&&) = delete;
    ~ServerConnection();

    /**
     * \brief accept the incoming connection and run the TLS handshake
     * \param[in] timeout_ms time to wait in milliseconds, -1 is wait forever, 0 is don't wait
     * \return see result_t. On error the connection will have
     *         been closed
     */
    [[nodiscard]] result_t accept(int timeout_ms);
    [[nodiscard]] inline result_t accept() {
        return accept(m_timeout_ms);
    }

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
 * \brief class representing a TLS connection (client side)
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
     * \param[in] timeout_ms time to wait in milliseconds, -1 is wait forever, 0 is don't wait
     * \return see result_t. On error the connection will have
     *         been closed
     */
    [[nodiscard]] result_t connect(int timeout_ms);
    [[nodiscard]] inline result_t connect() {
        return connect(m_timeout_ms);
    }
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
 * One option is to have a server thread calling Server.serve() with the
 * supplied handler creating a new connection thread when a connection arrives.
 *
 * For unit tests the connection handler can perform the test directly which
 * has the advantage of preventing new connections from being accepted.
 *
 * Another option is for the connection handler to add the new connection to a
 * list which is serviced by an event handler - i.e. one thread could manage
 * all connections.
 *
 * Listens on either IPv4 or IPv6. OpenSSL recommends that two listen sockets
 * are used to support both IPv4 and IPv6. (not implemented)
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

    struct certificate_config_t {
        //!< server certificate is the first certificate in the file followed by any intermediate CAs
        ConfigItem certificate_chain_file{nullptr};
        ConfigItem trust_anchor_file{nullptr};       //!< one or more trust anchor PEM certificates
        ConfigItem private_key_file{nullptr};        //!< key associated with the server certificate
        ConfigItem private_key_password{nullptr};    //!< optional password to read private key
        std::vector<ConfigItem> ocsp_response_files; //!< list of OCSP files in certificate chain order
    };

    struct config_t {
        ConfigItem cipher_list{nullptr};  //!< nullptr means use default
        ConfigItem ciphersuites{nullptr}; //!< nullptr means use default, "" disables TSL 1.3

        std::vector<certificate_config_t> chains; //!< server certificate chains - must be at least one
        //!< one or more trust anchor PEM certificates for client certificate verification
        ConfigItem verify_locations_file{nullptr};
        ConfigItem verify_locations_path{nullptr}; //!< for client certificate
        std::int32_t io_timeout_ms{-1};            //!< socket timeout in milliseconds (recommend > 1 sec)
        bool verify_client{true};                  //!< client certificate required

        // config not used on update()
        ConfigItem host{nullptr};    //!< see BIO_lookup_ex()
        ConfigItem service{nullptr}; //!< TLS port number as a string
        int socket{INVALID_SOCKET};  //!< use this specific socket - bypasses socket setup in init_socket() when set
        bool ipv6_only{true};        //!< listen on IPv6 only, when false listen on IPv4 only
    };

    using ConnectionPtr = std::unique_ptr<ServerConnection>;
    using ConnectionHandler = std::function<void(ConnectionPtr&& ctx)>;
    using OptionalConfig = std::optional<std::unique_ptr<config_t>>;
    using ConfigurationCallback = std::function<OptionalConfig()>;

private:
    using ServerStatusRequestV2 = status_request::ServerStatusRequestV2;
    using trusted_ca_keys_t = trusted_ca_keys::trusted_ca_keys_t;
    using ServerTrustedCaKeys = trusted_ca_keys::ServerTrustedCaKeys;

    std::unique_ptr<server_ctx> m_context;              //!< opaque object data
    int m_socket{INVALID_SOCKET};                       //!< server socket value
    volatile bool m_running{false};                     //!< server is listening for connections
    std::int32_t m_timeout_ms{-1};                      //!< default operation timeout passed to new connections
    std::atomic_bool m_exit{false};                     //!< stop listening for connections
    std::atomic<state_t> m_state{state_t::init_needed}; //!< server state
    std::mutex m_mutex;                                 //!< prevent multiple initialisation or serve requests
    std::mutex m_cv_mutex;                              //!< used by wait_running() and wait_stopped()
    std::condition_variable m_cv;                       //!< used by wait_running() and wait_stopped()
    OcspCache m_cache;                                  //!< cached OCSP responses
    ServerStatusRequestV2 m_status_request_v2;          //!< status request extension handler
    ServerTrustedCaKeys m_server_trusted_ca_keys;       //!< trusted ca keys extension handler
    pthread_t m_server_thread{};                        //!< serve() POSIX threads ID
    static int s_sig_int;                               //!< signal to use to wakeup serve()
    ConfigurationCallback m_init_callback{nullptr};     //!< callback to retrieve SSL configuration

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

    /**
     * \brief initialise server certificate chains
     * \param[in] chain_files server certificate chains
     * \return true on success
     */
    bool init_certificates(const std::vector<certificate_config_t>& chain_files);

    /**
     * \brief waits for incoming connections
     * \param[in] handler - called with the new connection socket
     */
    void wait_for_connection(const ConnectionHandler& handler);

public:
    Server();
    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;
    ~Server();

    /**
     * \brief configure a signal handler to wakeup serve()
     * \note stop() sends interrupt_signal to m_server_thread to interrupt poll()
     */
    static void configure_signal_handler(int interrupt_signal);

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
    state_t init(const config_t& cfg, const ConfigurationCallback& init_ssl);

    /**
     * \brief update configuration
     * \param[in] cfg server configuration
     * \return true on success
     * \note used to update OCSP caches and SSL certificates and keys.
     *       Does not change the listen socket settings
     */
    bool update(const config_t& cfg);

    /**
     * \brief wait for incomming connections
     * \param[in] handler called when there is a new connection
     * \return stopped after it has been running, or init_* values when listening
     *         can not start
     * \note this is a blocking call that will not return until stop() has been
     *       called (unless it couldn't start listening)
     * \note changing socket configuration requires stopping the server and
     *       calling init()
     * \note after server() returns stopped init() will need to be called
     *       before further connections can be managed
     */
    state_t serve(const ConnectionHandler& handler);

    /**
     * \brief stop listening for new connections
     * \note returns immediately
     *
     * may raise a signal on the m_server_thread to terminate poll() so that
     * serve() stops quickly. (see configure_signal_handler())
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
 * \note mainly intended for unit and system tests
 */
class Client {
public:
    using trusted_ca_keys_t = trusted_ca_keys::trusted_ca_keys_t;
    using ClientStatusRequestV2 = status_request::ClientStatusRequestV2;
    using ClientTrustedCaKeys = trusted_ca_keys::ClientTrustedCaKeys;

    /**
     * \brief configure different extension handler callbacks.
     * \note all callbacks must be specified, nullptr is not allowed.
     * \note mainly used for unit tests to send invalid extensions to the server.
     */
    struct override_t {
        int (*tlsext_status_cb)(Ssl* ctx, void* object){nullptr};
        int (*status_request_v2_add)(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                     std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                     void* object){nullptr};
        int (*status_request_v2_cb)(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                    std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                    void* object){nullptr};
        int (*trusted_ca_keys_add)(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                   std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                   void* object){nullptr};
        void (*trusted_ca_keys_free)(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* out,
                                     void* object){nullptr};
    };

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
        const char* cipher_list{nullptr};            //!< nullptr means use default
        const char* ciphersuites{nullptr};           //!< nullptr means use default, "" disables TSL 1.3
        const char* certificate_chain_file{nullptr}; //!< client certificate and intermediate CAs
        const char* private_key_file{nullptr};       //!< private key for client certificate
        const char* private_key_password{nullptr};   //!< optional password for private key
        const char* verify_locations_file{nullptr};  //!< PEM trust anchors for server certificate
        const char* verify_locations_path{nullptr};  //!< for server certificate
        trusted_ca_keys_t trusted_ca_keys_data;      //!< trusted CA keys configuration data
        std::int32_t io_timeout_ms{-1};              //!< default socket timeout in milliseconds (recommend > 1 sec)
        bool verify_server{true};                    //!< verify the server certificate
        bool status_request{false};                  //!< include a status request extension in the client hello
        bool status_request_v2{false};               //!< include a status request v2 extension in the client hello
        bool trusted_ca_keys{false};                 //!< include a trusted ca keys extension in the client hello
    };

    using ConnectionPtr = std::unique_ptr<ClientConnection>;

private:
    std::unique_ptr<client_ctx> m_context;                      //!< opaque object data
    std::int32_t m_timeout_ms{-1};                              //!< default operation timeout
    trusted_ca_keys_t m_trusted_ca_keys;                        //!< trusted CA keys configuration data
    std::unique_ptr<ClientStatusRequestV2> m_status_request_v2; //!< status request extension handler

public:
    Client();
    explicit Client(std::unique_ptr<ClientStatusRequestV2>&& handler);
    Client(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;
    virtual ~Client();

    /**
     * \brief initialise the client socket and TLS configuration
     * \param[in] cfg server configuration
     * \return true on success
     */
    bool init(const config_t& cfg);

    /**
     * \brief initialise the client socket and TLS configuration
     * \param[in] cfg server configuration
     * \param[in] override override SSL callbacks
     * \return true on success
     */
    bool init(const config_t& cfg, const override_t& override);

    /**
     * \brief connect to server
     * \param[in] host host to connect to
     * \param[in] service port to connect to
     * \param[in] ipv6_only false - also support IPv4
     * \param[in] timeout_ms how long to wait for a connection in milliseconds
     * \return a connection pointer (nullptr on error)
     */
    [[nodiscard]] ConnectionPtr connect(const char* host, const char* service, bool ipv6_only, int timeout_ms);
    [[nodiscard]] inline ConnectionPtr connect(const char* host, const char* service, bool ipv6_only) {
        return connect(host, service, ipv6_only, m_timeout_ms);
    }

    /**
     * \brief the default SSL callbacks
     */
    static override_t default_overrides();
};

} // namespace tls

#endif // TLS_HPP_
