// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "tls.hpp"
#include "extensions/status_request.hpp"
#include "extensions/trusted_ca_keys.hpp"
#include "openssl_util.hpp"

#include <array>
#include <cassert>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>

#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/types.h>
#include <utility>

#ifdef UNIT_TEST
#include "extensions/helpers.hpp"
#endif

namespace std {
template <> class default_delete<SSL> {
public:
    void operator()(SSL* ptr) const {
        ::SSL_free(ptr);
    }
};
template <> class default_delete<SSL_CTX> {
public:
    void operator()(SSL_CTX* ptr) const {
        ::SSL_CTX_free(ptr);
    }
};
template <> class default_delete<BIO_ADDR> {
public:
    void operator()(BIO_ADDR* ptr) const {
        ::BIO_ADDR_free(ptr);
    }
};
template <> class default_delete<BIO_ADDRINFO> {
public:
    void operator()(BIO_ADDRINFO* ptr) const {
        ::BIO_ADDRINFO_free(ptr);
    }
};
} // namespace std

using ::openssl::log_error;
using ::openssl::log_warning;

namespace {

/**
 * \brief signal handler that does nothing
 * \param[in] sig is the signal that was received (not used)
 * \note exists so that poll() can be interrupted
 */
void sig_int_handler(int sig) {
}

/// OpenSSL results mapped to an enum
enum class ssl_error_t : std::uint8_t {
    error,
    error_ssl,
    error_syscall,
    none,
    want_accept,
    want_async,
    want_async_job,
    want_connect,
    want_hello_cb,
    want_read,
    want_write,
    want_x509_lookup,
    zero_return,
    timeout, // not an OpenSSL result
};

/**
 * \brief convert OpenSSL result into an enum
 * \param[in] err is the OpenSSL result
 * \return enum equivalent of err
 */
constexpr ssl_error_t convert(const int err) {
    ssl_error_t res{ssl_error_t::error};
    switch (err) {
    case SSL_ERROR_NONE:
        res = ssl_error_t::none;
        break;
    case SSL_ERROR_ZERO_RETURN:
        res = ssl_error_t::zero_return;
        break;
    case SSL_ERROR_WANT_READ:
        res = ssl_error_t::want_read;
        break;
    case SSL_ERROR_WANT_WRITE:
        res = ssl_error_t::want_write;
        break;
    case SSL_ERROR_WANT_CONNECT:
        res = ssl_error_t::want_connect;
        break;
    case SSL_ERROR_WANT_ACCEPT:
        res = ssl_error_t::want_accept;
        break;
    case SSL_ERROR_WANT_X509_LOOKUP:
        res = ssl_error_t::want_x509_lookup;
        break;
    case SSL_ERROR_WANT_ASYNC:
        res = ssl_error_t::want_async;
        break;
    case SSL_ERROR_WANT_ASYNC_JOB:
        res = ssl_error_t::want_async_job;
        break;
    case SSL_ERROR_WANT_CLIENT_HELLO_CB:
        res = ssl_error_t::want_hello_cb;
        break;
    case SSL_ERROR_SYSCALL:
        res = ssl_error_t::error_syscall;
        break;
    case SSL_ERROR_SSL:
        res = ssl_error_t::error_ssl;
        break;
    default:
        log_error(std::string("Unexpected SSL_get_error: ") + std::to_string(static_cast<int>(res)));
        break;
    };
    return res;
}

/// subset of OpenSSL results to simplify error recovery
enum class ssl_result_t : std::uint8_t {
    error,         //!< error - connection still active
    error_syscall, //!< error - connection closed, socket no longer valid
    success,
    closed,
    timeout,
    want_read,  //!< non-blocking - operation waiting for read available on socket
    want_write, //!< non-blocking - operation waiting for write available on socket
};

/**
 * \brief map OpenSSL result enum to a simplified subset
 * \param[in] err is the OpenSSL result enum value
 * \return the simplified version of err
 */
constexpr ssl_result_t convert(ssl_error_t err) {
    switch (err) {
    case ssl_error_t::none:
        return ssl_result_t::success;
    case ssl_error_t::timeout:
        return ssl_result_t::timeout;
    case ssl_error_t::error_syscall:
    case ssl_error_t::error_ssl:
        return ssl_result_t::error_syscall;
    case ssl_error_t::zero_return:
        return ssl_result_t::closed;
    case ssl_error_t::want_read:
        return ssl_result_t::want_read;
    case ssl_error_t::want_write:
        return ssl_result_t::want_write;
    case ssl_error_t::error:
    case ssl_error_t::want_accept:  // only from BIO_s_accept()
    case ssl_error_t::want_connect: // only from BIO_s_connect()
    case ssl_error_t::want_async:
    case ssl_error_t::want_async_job:
    case ssl_error_t::want_hello_cb:
    case ssl_error_t::want_x509_lookup:
    default:
        return ssl_result_t::error;
    }
}

/**
 * \brief convert the simplified result into the result used in the API
 * \param[in] err is the simplified result
 * \return the mapped API result value
 */
constexpr tls::Connection::result_t convert(ssl_result_t err) {
    switch (err) {
    case ssl_result_t::success:
        return tls::Connection::result_t::success;
    case ssl_result_t::timeout:
        return tls::Connection::result_t::timeout;
    case ssl_result_t::want_read:
        return tls::Connection::result_t::want_read;
    case ssl_result_t::want_write:
        return tls::Connection::result_t::want_write;
    case ssl_result_t::closed:
    case ssl_result_t::error:
    case ssl_result_t::error_syscall:
    default:
        return tls::Connection::result_t::closed;
    }
}

/**
 * \brief wait for an event on a socket
 * \param[in] soc is the file descriptor/socket
 * \param[in] forWrite wait for a write event when true, read event otherwise
 * \param[in] timeout_ms -1 is wait forever, 0 checks for events and returns
 *            immediately, >0 maximum time to wait in milliseconds
 * \return >0 when there are events on the socket
 *         0 for timeout
 *         -1 for error (check errno)
 *         -2 for error where errno is EINTR (interrupted call)
 */
int wait_for(int soc, bool forWrite, std::int32_t timeout_ms) {
    const std::int16_t event = (forWrite) ? POLLOUT : POLLIN;
    std::array<pollfd, 1> fds = {{{soc, event, 0}}};
    auto poll_res = poll(fds.data(), fds.size(), timeout_ms);
    if (poll_res == -1) {
        if (errno != EINTR) {
            log_error(std::string("wait_for poll: ") + std::to_string(errno));
        } else {
            poll_res = -2;
        }
    }
    return poll_res;
}

/**
 * \brief wait for an event on a socket retrying when interrupted
 * \param[in] soc is the file descriptor/socket
 * \param[in] forWrite wait for a write event when true, read event otherwise
 * \param[in] timeout_ms -1 is wait forever, 0 checks for events and returns
 *            immediately, >0 maximum time to wait in milliseconds
 * \return >0 when there are events on the socket
 *         0 for timeout
 *         -1 for error (check errno)
 */
int wait_for_loop(int soc, bool forWrite, std::int32_t timeout_ms) {
    int res{-2};
    while (res == -2) {
        res = wait_for(soc, forWrite, timeout_ms);
    }
    return res;
}

/**
 * \brief read user data from a SSL connection
 * \param[in] ctx is SSL connection data
 * \param[in] buf is where to place received data
 * \param[in] num is the size of buff (the maximum number of bytes to receive)
 * \param[out] readbytes number of bytes received
 * \returns the result of the operation
 */
[[nodiscard]] ssl_result_t ssl_read(SSL* ctx, std::byte* buf, std::size_t num, std::size_t& readbytes);
/**
 * \brief write user data to a SSL connection
 * \param[in] ctx is SSL connection data
 * \param[in] buf is the data to send
 * \param[in] num is the size of buff
 * \param[out] writebytes number of bytes sent
 * \returns the result of the operation
 */
[[nodiscard]] ssl_result_t ssl_write(SSL* ctx, const std::byte* buf, std::size_t num, std::size_t& writebytes);
/**
 * \brief accept an incoming SSL connection, runs the TLS handshake (sever)
 * \param[in] ctx is SSL connection data
 * \returns the result of the operation
 */
[[nodiscard]] ssl_result_t ssl_accept(SSL* ctx);
/**
 * \brief start a SSL connection, runs the TLS handshake (client)
 * \param[in] ctx is SSL connection data
 * \returns the result of the operation
 */
[[nodiscard]] ssl_result_t ssl_connect(SSL* ctx);
/**
 * \brief close a SSL connection
 * \param[in] ctx is SSL connection data
 * \returns the result of the operation
 */
ssl_result_t ssl_shutdown(SSL* ctx);

/// operation being performed
enum class operation_t : std::uint8_t {
    ssl_read,
    ssl_write,
    ssl_accept,
    ssl_connect,
    ssl_shutdown,
};

/**
 * \brief text representation of the operation being performed
 * \param[in] the operation
 * \return a string representing the operation
 */
const char* operation_str(operation_t operation) {
    switch (operation) {
    case operation_t::ssl_read:
        return "SSL_read: ";
    case operation_t::ssl_write:
        return "SSL_write: ";
    case operation_t::ssl_accept:
        return "SSL_accept: ";
    case operation_t::ssl_connect:
        return "SSL_connect: ";
    case operation_t::ssl_shutdown:
        return "SSL_shutdown: ";
    default:
        return "<SSL unknown>: ";
    }
}

/**
 * \brief manage the result from a SSL operation
 * \param[in] ctx is SSL connection data
 * \param[in] res is the result of the SSL operation
 * \return result is the simplified result mapped from res
 */
ssl_result_t process_result(SSL* ctx, operation_t operation, const int res) {
    ssl_error_t result{ssl_error_t::error};

    if (ctx != nullptr) {
        result = ssl_error_t::none; // success
        if (res <= 0) {
            const auto sslerr_raw = SSL_get_error(ctx, res);
            result = convert(sslerr_raw);
            switch (result) {
            case ssl_error_t::none:
            case ssl_error_t::zero_return:
            case ssl_error_t::want_read:
            case ssl_error_t::want_write:
                break;
            case ssl_error_t::error_syscall:
                // no further operation permitted on the connection
                if (errno != 0) {
                    log_error(operation_str(operation) + std::string("SSL_ERROR_SYSCALL ") + std::to_string(errno));
                }
                break;
            case ssl_error_t::error_ssl:
                if (operation != operation_t::ssl_shutdown) {
                    log_error(operation_str(operation) + std::to_string(res) + " " + std::to_string(sslerr_raw));
                }
                break;
            case ssl_error_t::error:
            case ssl_error_t::want_accept:
            case ssl_error_t::want_async:
            case ssl_error_t::want_async_job:
            case ssl_error_t::want_connect:
            case ssl_error_t::want_hello_cb:
            case ssl_error_t::want_x509_lookup:
            default:
                log_error(operation_str(operation) + std::to_string(res) + " " + std::to_string(sslerr_raw));
                break;
            }
        }
    }

    return convert(result);
}

ssl_result_t ssl_read(SSL* ctx, std::byte* buf, const std::size_t num, std::size_t& readbytes) {
    const auto res = SSL_read_ex(ctx, buf, num, &readbytes);
    return process_result(ctx, operation_t::ssl_read, res);
};

ssl_result_t ssl_write(SSL* ctx, const std::byte* buf, const std::size_t num, std::size_t& writebytes) {
    const auto res = SSL_write_ex(ctx, buf, num, &writebytes);
    return process_result(ctx, operation_t::ssl_read, res);
}

ssl_result_t ssl_accept(SSL* ctx) {
    const auto res = SSL_accept(ctx);
    // 0 is handshake not successful (ssl_error_t::zero_return -> ssl_result_t::closed)
    // < 0 is other error
    return process_result(ctx, operation_t::ssl_read, res);
}

ssl_result_t ssl_connect(SSL* ctx) {
    const auto res = SSL_connect(ctx);
    // 0 is handshake not successful (ssl_error_t::zero_return -> ssl_result_t::closed)
    // < 0 is other error
    return process_result(ctx, operation_t::ssl_read, res);
}

ssl_result_t ssl_shutdown(SSL* ctx) {
    const auto res = SSL_shutdown(ctx);
    return process_result(ctx, operation_t::ssl_read, res);
}

/**
 * \brief configure SSL context with certificates and keys
 * \param[in] ctx is SSL context data
 * \param[in] ciphersuites are the TLS 1.3 cipher suites,
 *            nullptr means use default, "" disables TSL 1.3
 * \param[in] cipher_list are the TLS 1.2 ciphers, nullptr means use default
 * \param[in] cert_config are one of more sets of key and certificates
 * \param[in] required when true, fail when cert_config is missing
 * \return true when successful
 * \note required will be true for a TLS server and can be false for a TLS client
 */
bool configure_ssl_ctx(SSL_CTX* ctx, const char* ciphersuites, const char* cipher_list,
                       const tls::Server::certificate_config_t& cert_config, bool required) {
    bool result{true};

    // TODO(james-ctc) TPM2 support

    if (ctx == nullptr) {
        log_error("server_init::SSL_CTX_new");
        result = false;
    } else {
        if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) == 0) {
            log_error("SSL_CTX_set_min_proto_version");
            result = false;
        }
        if ((ciphersuites != nullptr) && (ciphersuites[0] == '\0')) {
            // no cipher suites configured - don't use TLS 1.3
            // nullptr means use the defaults
            if (SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION) == 0) {
                log_error("SSL_CTX_set_max_proto_version");
                result = false;
            }
        }
        if (cipher_list != nullptr) {
            if (SSL_CTX_set_cipher_list(ctx, cipher_list) == 0) {
                log_error("SSL_CTX_set_cipher_list");
                result = false;
            }
        }
        if (ciphersuites != nullptr) {
            if (SSL_CTX_set_ciphersuites(ctx, ciphersuites) == 0) {
                log_error("SSL_CTX_set_ciphersuites");
                result = false;
            }
        }

        if (cert_config.certificate_chain_file != nullptr) {
            if (SSL_CTX_use_certificate_chain_file(ctx, cert_config.certificate_chain_file) != 1) {
                log_error("SSL_CTX_use_certificate_chain_file");
                result = false;
            }
        } else {
            if (required) {
                result = false;
            }
        }

        if (cert_config.private_key_file != nullptr) {
            // the password callback uses a non-const argument
            void* pass_ptr{nullptr};
            std::string pass_str;
            if (cert_config.private_key_password != nullptr) {
                pass_str = cert_config.private_key_password;
                pass_ptr = pass_str.data();
            }
            SSL_CTX_set_default_passwd_cb_userdata(ctx, pass_ptr);

            if (SSL_CTX_use_PrivateKey_file(ctx, cert_config.private_key_file, SSL_FILETYPE_PEM) != 1) {
                log_error("SSL_CTX_use_PrivateKey_file");
                result = false;
            }
            if (SSL_CTX_check_private_key(ctx) != 1) {
                log_error("SSL_CTX_check_private_key");
                result = false;
            }
        } else {
            if (required) {
                result = false;
            }
        }
    }

    return result;
}

} // namespace

namespace tls {

using SSL_ptr = std::unique_ptr<SSL>;
using SSL_CTX_ptr = std::unique_ptr<SSL_CTX>;
using OCSP_RESPONSE_ptr = std::shared_ptr<OCSP_RESPONSE>;

struct connection_ctx {
    SSL_ptr ctx;
    BIO* soc_bio{nullptr};
    int soc{0};
};

struct ocsp_cache_ctx {
    std::map<OcspCache::digest_t, OCSP_RESPONSE_ptr> cache;
};

struct server_ctx {
    SSL_CTX_ptr ctx;
};

struct client_ctx {
    SSL_CTX_ptr ctx;
};

// ----------------------------------------------------------------------------
// Connection represents a TLS connection (client and server)

Connection::Connection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms) :
    m_context(std::make_unique<connection_ctx>()), m_ip(ip_in), m_service(service_in), m_timeout_ms(timeout_ms) {
    m_context->ctx = SSL_ptr(SSL_new(ctx));
    m_context->soc = soc;

    if (m_context->ctx == nullptr) {
        log_error("Connection::SSL_new");
    } else {
        // BIO_free is handled when SSL_free is done (SSL_ptr)
        m_context->soc_bio = BIO_new_socket(soc, BIO_CLOSE);
        SSL_set_bio(m_context->ctx.get(), m_context->soc_bio, m_context->soc_bio);
    }

    if (m_context->soc_bio == nullptr) {
        log_error("Connection::BIO_new_socket");
    }
}

Connection::~Connection() = default;

Connection::result_t Connection::read(std::byte* buf, std::size_t num, std::size_t& readbytes, int timeout_ms) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};

    if (m_state == state_t::connected) {
        auto ctx = m_context->ctx.get();
        bool loop{true};
        while (loop) {
            loop = false;
            result = ssl_read(ctx, buf, num, readbytes);
            switch (result) {
            case ssl_result_t::success:
            case ssl_result_t::timeout:
                break;
            case ssl_result_t::want_read:
            case ssl_result_t::want_write:
                if (timeout_ms != 0) {
                    const auto res = ::wait_for_loop(SSL_get_fd(ctx), result == ssl_result_t::want_write, timeout_ms);
                    loop = res > 0; // event received
                    result = ssl_result_t::timeout;
                }
                break;
            case ssl_result_t::error_syscall:
                m_state = state_t::fault;
                break;
            case ssl_result_t::closed:
                shutdown();
                break;
            case ssl_result_t::error:
            default:
                shutdown();
                m_state = state_t::fault;
                break;
            }
        }
    }
    return convert(result);
}

Connection::result_t Connection::write(const std::byte* buf, std::size_t num, std::size_t& writebytes, int timeout_ms) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};

    if (m_state == state_t::connected) {
        auto ctx = m_context->ctx.get();
        bool loop{true};
        while (loop) {
            loop = false;
            result = ssl_write(ctx, buf, num, writebytes);
            switch (result) {
            case ssl_result_t::success:
            case ssl_result_t::timeout:
                break;
            case ssl_result_t::want_read:
            case ssl_result_t::want_write:
                if (timeout_ms != 0) {
                    const auto res = ::wait_for_loop(SSL_get_fd(ctx), result == ssl_result_t::want_write, timeout_ms);
                    loop = res > 0; // event received
                    result = ssl_result_t::timeout;
                }
                break;
            case ssl_result_t::error_syscall:
                m_state = state_t::fault;
                break;
            case ssl_result_t::closed:
                shutdown();
                break;
            case ssl_result_t::error:
            default:
                shutdown();
                m_state = state_t::fault;
                break;
            }
        }
    }
    return convert(result);
}

Connection::result_t Connection::shutdown(int timeout_ms) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};

    if (m_state == state_t::connected) {
        auto ctx = m_context->ctx.get();
        bool loop{true};
        while (loop) {
            loop = false;
            result = ssl_shutdown(ctx);
            switch (result) {
            case ssl_result_t::closed:
            case ssl_result_t::success:
            case ssl_result_t::timeout:
                m_state = state_t::closed;
                break;
            case ssl_result_t::want_read:
            case ssl_result_t::want_write:
                if (timeout_ms != 0) {
                    const auto res = ::wait_for_loop(SSL_get_fd(ctx), result == ssl_result_t::want_write, timeout_ms);
                    loop = res > 0; // event received
                    result = ssl_result_t::timeout;
                }
                break;
            case ssl_result_t::error:
            case ssl_result_t::error_syscall:
            default:
                m_state = state_t::fault;
                break;
            }
        }
    }
    return convert(result);
}

Connection::result_t Connection::wait_for(result_t action, int timeout_ms) {
    result_t result{action};
    auto ctx = m_context->ctx.get();

    switch (action) {
    case result_t::want_read:
    case result_t::want_write: {
        const auto res = wait_for_loop(SSL_get_fd(ctx), action == result_t::want_write, timeout_ms);
        if (res == 0) {
            result = result_t::timeout;
        } else if (res == -1) {
            result = result_t::closed;
        } else {
            result = result_t::success;
        }
        break;
    }
    case result_t::success:
    case result_t::closed:
    case result_t::timeout:
    default:
        break;
    }

    return result;
}

int Connection::socket() const {
    return m_context->soc;
}

const Certificate* Connection::peer_certificate() const {
    assert(m_context != nullptr);
    return SSL_get0_peer_certificate(m_context->ctx.get());
}

SSL* Connection::ssl_context() const {
    return m_context->ctx.get();
}

// ----------------------------------------------------------------------------
// ServerConnection represents a TLS server connection

std::uint32_t ServerConnection::m_count{0};
std::mutex ServerConnection::m_cv_mutex;
std::condition_variable ServerConnection::m_cv;

ServerConnection::ServerConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in,
                                   std::int32_t timeout_ms) :
    Connection(ctx, soc, ip_in, service_in, timeout_ms), m_tck_data{m_trusted_ca_keys, m_flags} {
    {
        std::lock_guard lock(m_cv_mutex);
        m_count++;
    }
    if (m_context->soc_bio != nullptr) {
        SSL_set_accept_state(m_context->ctx.get());
        ServerStatusRequestV2::set_data(m_context->ctx.get(), &m_flags);
        ServerTrustedCaKeys::set_data(m_context->ctx.get(), &m_tck_data);
    }
}

ServerConnection::~ServerConnection() {
    {
        std::lock_guard lock(m_cv_mutex);
        m_count--;
    }
    m_cv.notify_all();
}

Connection::result_t ServerConnection::accept(int timeout_ms) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};

    if (m_state == state_t::idle) {
        auto ctx = m_context->ctx.get();
        bool loop{true};
        while (loop) {
            loop = false;
            result = ssl_accept(ctx);
            switch (result) {
            case ssl_result_t::success:
                m_state = state_t::connected;
                break;
            case ssl_result_t::want_read:
            case ssl_result_t::want_write:
                if (timeout_ms != 0) {
                    const auto res = ::wait_for_loop(SSL_get_fd(ctx), result == ssl_result_t::want_write, timeout_ms);
                    loop = res > 0; // event received
                    result = ssl_result_t::timeout;
                }
                break;
            case ssl_result_t::error_syscall:
                m_state = state_t::fault;
                break;
            case ssl_result_t::closed:
                shutdown();
                break;
            case ssl_result_t::error:
            default:
                shutdown();
                m_state = state_t::fault;
                break;
            }
        }
    }
    return convert(result);
}

void ServerConnection::wait_all_closed() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [] { return m_count == 0; });
    lock.unlock();
}

// ----------------------------------------------------------------------------
// ClientConnection represents a TLS client connection

ClientConnection::ClientConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in,
                                   std::int32_t timeout_ms) :
    Connection(ctx, soc, ip_in, service_in, timeout_ms) {
    if (m_context->soc_bio != nullptr) {
        SSL_set_connect_state(m_context->ctx.get());
    }
}

ClientConnection::~ClientConnection() = default;

Connection::result_t ClientConnection::connect(int timeout_ms) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};

    if (m_state == state_t::idle) {
        auto ctx = m_context->ctx.get();
        bool loop{true};
        while (loop) {
            loop = false;
            result = ssl_connect(ctx);
            switch (result) {
            case ssl_result_t::success:
                m_state = state_t::connected;
                break;
            case ssl_result_t::want_read:
            case ssl_result_t::want_write:
                if (timeout_ms != 0) {
                    const auto res = ::wait_for_loop(SSL_get_fd(ctx), result == ssl_result_t::want_write, timeout_ms);
                    loop = res > 0; // event received
                    result = ssl_result_t::timeout;
                }
                break;
            case ssl_result_t::error_syscall:
                m_state = state_t::fault;
                break;
            case ssl_result_t::closed:
                shutdown();
                break;
            case ssl_result_t::error:
            default:
                shutdown();
                m_state = state_t::fault;
                break;
            }
        }
    }
    return convert(result);
}

// ----------------------------------------------------------------------------
// TLS Server

int Server::s_sig_int{-1};

Server::Server() : m_context(std::make_unique<server_ctx>()), m_status_request_v2(m_cache) {
}

Server::~Server() {
    stop();
    wait_stopped();
}

bool Server::init_socket(const config_t& cfg) {
    bool result = false;
    if (cfg.socket == INVALID_SOCKET) {
        BIO_ADDRINFO* addrinfo_tmp{nullptr};
        // AF_UNSPEC is another option but seems to prefer IPv4
        const int family{(cfg.ipv6_only) ? AF_INET6 : AF_INET};

        result = BIO_lookup_ex(cfg.host, cfg.service, BIO_LOOKUP_SERVER, family, SOCK_STREAM, IPPROTO_TCP,
                               &addrinfo_tmp) != 0;

        if (!result) {
            log_error("init_socket::BIO_lookup_ex");
        } else {
            std::unique_ptr<BIO_ADDRINFO> addrinfo(addrinfo_tmp);
            const auto sock_family = BIO_ADDRINFO_family(addrinfo.get());
            const auto sock_type = BIO_ADDRINFO_socktype(addrinfo.get());
            const auto sock_protocol = BIO_ADDRINFO_protocol(addrinfo.get());
            const auto* sock_address = BIO_ADDRINFO_address(addrinfo.get());
            m_socket = BIO_socket(sock_family, sock_type, sock_protocol, 0);

            if (m_socket == INVALID_SOCKET) {
                log_error("init_socket::BIO_socket");
                if (cfg.ipv6_only) {
                    log_warning("Verify that the configured interface has a valid IPv6 link local address configured.");
                }
            } else {
                int sock_options{BIO_SOCK_REUSEADDR | BIO_SOCK_NONBLOCK};
                if (cfg.ipv6_only) {
                    sock_options = BIO_SOCK_REUSEADDR | BIO_SOCK_V6_ONLY | BIO_SOCK_NONBLOCK;
                }

                result = BIO_listen(m_socket, sock_address, sock_options) != 0;
                if (!result) {
                    log_error("init_socket::BIO_listen");
                    BIO_closesocket(m_socket);
                    m_socket = INVALID_SOCKET;
                }
            }
        }
    } else {
        // the code that sets cfg.socket is responsible for
        // all socket initialisation
        m_socket = cfg.socket;
        result = true;
    }

    return result;
}

bool Server::init_ssl(const config_t& cfg) {
    assert(m_context != nullptr);

    const SSL_METHOD* method = TLS_server_method();
    auto* ctx = SSL_CTX_new(method);
    bool result = ctx != nullptr;
    result = result && (cfg.chains.size() > 0);

    if (result) {
        // use the first server chain
        result = configure_ssl_ctx(ctx, cfg.ciphersuites, cfg.cipher_list, cfg.chains[0], true);
        if (result) {
            int mode = SSL_VERIFY_NONE;

            // TODO(james-ctc): verify may need to change based on TLS version
            // 15118-2 mandates TLS 1.2 and no client certificate
            // 15118-20 mandates TLS 1.3 and requires a client certificate
            // There might be a requirement to support mutual authentication on
            // TLS 1.2
            //
            // Potential solution is to provide optional mutual authentication
            // for TLS 1.2 and mandatory mutual authentication for TLS 1.3
            // SSL_set_verify() could be used in
            // ServerStatusRequestV2::client_hello_cb
            // TLS 1.3 has a post handshake flag SSL_VERIFY_POST_HANDSHAKE
            // which might be a better approach

            if (cfg.verify_client) {
                mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
                if (SSL_CTX_load_verify_locations(ctx, cfg.verify_locations_file, cfg.verify_locations_path) != 1) {
                    log_error("SSL_CTX_load_verify_locations");
                }
            } else {
                if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
                    log_error("SSL_CTX_set_default_verify_paths");
                    result = false;
                }
            }
            SSL_CTX_set_verify(ctx, mode, nullptr);

            result = result && m_status_request_v2.init_ssl(ctx);
            result = result && m_server_trusted_ca_keys.init_ssl(ctx);
        }
    }

    if (!result) {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }

    m_context->ctx = SSL_CTX_ptr(ctx);
    return ctx != nullptr;
}

bool Server::init_certificates(const std::vector<certificate_config_t>& chain_files) {
    std::vector<OcspCache::ocsp_entry_t> entries;
    openssl::chain_list chains;

    for (const auto& i : chain_files) {
        auto certs = openssl::load_certificates(i.certificate_chain_file);
        auto tas = openssl::load_certificates(i.trust_anchor_file);
        auto pkey = openssl::load_private_key(i.private_key_file, i.private_key_password);

        if (certs.size() > 0) {
            openssl::chain_t chain;

            // update OCSP cache
            if (certs.size() == i.ocsp_response_files.size()) {
                for (std::size_t c = 0; c < certs.size(); c++) {
                    const auto& file = i.ocsp_response_files[c];
                    const auto& cert = certs[c];

                    if (file != nullptr) {
                        OcspCache::digest_t digest{};
                        if (OcspCache::digest(digest, cert.get())) {
                            entries.emplace_back(digest, file);
                        }
                    }
                }
            } else {
                log_warning("<n> certificates != <n> OCSP responses");
            }

            /*
             * If there are no trust anchors then the chain can't be verified
             * it also means that trusted_ca_keys can't be supported for the
             * chain.
             */

            if (!tas.empty()) {
                // update trusted CA keys information
                chain.chain.leaf = std::move(certs[0]);
                // remove server cert from intermediate list
                certs.erase(certs.begin());
                chain.chain.chain = std::move(certs);
                chain.chain.trust_anchors = std::move(tas);
                chain.private_key = std::move(pkey);

                if (openssl::verify_chain(chain)) {
                    chains.emplace_back(std::move(chain));
                }
            } else {
                const auto subject = openssl::certificate_subject(certs[0].get());
                std::string msg("No trust anchors for certificate:");
                for (const auto& item : subject) {
                    msg += ' ';
                    msg += item.first;
                    msg += ':';
                    msg += item.second;
                }
                log_warning(msg);
            }
        }
    }

    bool result{true};

    if (chains.empty()) {
        // continue without trusted_ca_keys support
        log_warning("trusted_ca_keys support disabled");
    } else {
        m_server_trusted_ca_keys.update(std::move(chains));
    }

    // don't error when there are no OCSP cached responses
    if (!entries.empty()) {
        if (!m_cache.load(entries)) {
            result = false;
        }
    }

    return result;
}

void Server::wait_for_connection(const ConnectionHandler& handler) {
    std::unique_ptr<BIO_ADDR> peer(BIO_ADDR_new());
    if (peer == nullptr) {
        log_error("serve::BIO_ADDR_new");
        m_exit = true;
    } else {
        int soc{INVALID_SOCKET};
        while ((soc < 0) && !m_exit) {
            auto poll_res = wait_for(m_socket, false, c_serve_timeout_ms);
            if (poll_res == -1) {
                // poll() has failed
                m_exit = true;
            } else if ((poll_res == -2) || (poll_res == 0)) {
                // EINTR is -2 - triggered by stop()
                // timeout is 0
                // nothing to accept, check for m_exit
            } else {
                soc = BIO_accept_ex(m_socket, peer.get(), BIO_SOCK_NONBLOCK);
                if (BIO_sock_should_retry(soc) == 0) {
                    break;
                }
            }
        };

        // attempt to get SSL configuration when not set yet
        if ((soc >= 0) && (m_state == state_t::init_socket)) {
            auto new_config = m_init_callback();
            bool success{false};
            if (new_config && new_config.value()) {
                success = update(*new_config.value());
            }
            if (success) {
                m_state = state_t::running;
            } else {
                // updated configuration failed
                BIO_closesocket(soc);
                soc = INVALID_SOCKET;
            }
        }

        if (m_exit) {
            if (soc >= 0) {
                BIO_closesocket(soc);
            }
        } else {
            if (soc < 0) {
                log_error("serve::BIO_accept_ex");
            } else {
                // new connection, pass to handler
                auto* ip = BIO_ADDR_hostname_string(peer.get(), 1);
                auto* service = BIO_ADDR_service_string(peer.get(), 1);
                auto connection =
                    std::make_unique<ServerConnection>(m_context->ctx.get(), soc, ip, service, m_timeout_ms);
                handler(std::move(connection));
                OPENSSL_free(ip);
                OPENSSL_free(service);
            }
        }
    }
}

void Server::configure_signal_handler(int interrupt_signal) {
    s_sig_int = interrupt_signal;
    struct sigaction action {};
    action.sa_handler = &sig_int_handler;
    action.sa_flags = SA_RESTART;
    if (sigaction(s_sig_int, &action, nullptr) == -1) {
        log_error("Server::configure_signal_handler: " + std::to_string(errno));
    }
}

Server::state_t Server::init(const config_t& cfg, const ConfigurationCallback& init_ssl_cb) {
    // prevent multiple calls to init, or calling init whilst serve() is running
    std::lock_guard lock(m_mutex);
    m_init_callback = init_ssl_cb; // save handler for later
    m_state = state_t::init_needed;
    if (init_socket(cfg)) {
        m_state = state_t::init_socket;
        if (update(cfg)) {
            m_state = state_t::init_complete;
        }
    }
    return m_state;
}

bool Server::update(const config_t& cfg) {
    // does not change server socket settings, use init() if needed
    std::vector<OcspCache::ocsp_entry_t> entries;

    m_timeout_ms = cfg.io_timeout_ms;
    // always try init_certificates() and init_ssl()
    bool result = init_certificates(cfg.chains);
    if (!init_ssl(cfg)) {
        result = false;
    }
    return result;
}

Server::state_t Server::serve(const ConnectionHandler& handler) {
    assert(m_context != nullptr);
    // prevent init() or server() being called while serve is running
    std::lock_guard lock(m_mutex);

    // check server socket configuration has been successful
    bool result{false};
    state_t tmp = m_state;
    switch (tmp) {
    case state_t::init_socket:
        if (m_init_callback != nullptr) {
            result = m_socket != INVALID_SOCKET;
        }
        break;
    case state_t::init_complete:
        result = m_socket != INVALID_SOCKET;
        break;
    case state_t::init_needed:
    case state_t::running:
    case state_t::stopped:
    default:
        break;
    }

    // wakeup wait_running()
    {
        std::lock_guard lock(m_cv_mutex);
        m_running = true;
    }
    m_cv.notify_all();
    m_server_thread = pthread_self(); // for use by stop()

    if (result) {
        m_exit = false;
        m_state = (m_state == state_t::init_complete) ? state_t::running : state_t::init_socket;
        while (!m_exit) {
            wait_for_connection(handler);
        }

        BIO_closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        m_state = state_t::stopped;
    }

    // wakeup wait_stopped()
    {
        std::lock_guard lock(m_cv_mutex);
        m_running = false;
    }
    m_cv.notify_all();
    return m_state;
}

void Server::stop() {
    m_exit = true;
    // raise a signal if a hander was installed
    if (m_running && (s_sig_int != -1)) {
        pthread_kill(m_server_thread, s_sig_int);
    }
}

void Server::wait_running() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [this]() { return this->m_running; });
}

void Server::wait_stopped() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [this]() { return !this->m_running; });
}

// ----------------------------------------------------------------------------
// Client

Client::Client() :
    m_context(std::make_unique<client_ctx>()), m_status_request_v2(std::make_unique<ClientStatusRequestV2>()) {
}

Client::Client(std::unique_ptr<ClientStatusRequestV2>&& handler) :
    m_context(std::make_unique<client_ctx>()), m_status_request_v2(std::move(handler)) {
}

Client::~Client() = default;

bool Client::init(const config_t& cfg) {
    return init(cfg, default_overrides());
}

bool Client::init(const config_t& cfg, const override_t& override) {
    assert(m_context != nullptr);
    assert(override.tlsext_status_cb != nullptr);
    assert(override.status_request_v2_cb != nullptr);
    assert(override.status_request_v2_add != nullptr);
    assert(override.trusted_ca_keys_add != nullptr);
    assert(override.trusted_ca_keys_free != nullptr);

    m_timeout_ms = cfg.io_timeout_ms;
    m_trusted_ca_keys = cfg.trusted_ca_keys_data;
    const SSL_METHOD* method = TLS_client_method();
    auto* ctx = SSL_CTX_new(method);
    const Server::certificate_config_t cert_config = {
        cfg.certificate_chain_file,
        nullptr,
        cfg.private_key_file,
        cfg.private_key_password,
    };
    auto result = configure_ssl_ctx(ctx, cfg.ciphersuites, cfg.cipher_list, cert_config, false);
    if (result) {
        int mode = SSL_VERIFY_NONE;

        if (cfg.verify_server) {
            mode = SSL_VERIFY_PEER;
            if (SSL_CTX_load_verify_locations(ctx, cfg.verify_locations_file, cfg.verify_locations_path) != 1) {
                log_error("SSL_CTX_load_verify_locations");
            }
        } else {
            if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
                log_error("SSL_CTX_set_default_verify_paths");
                result = false;
            }
        }

        SSL_CTX_set_verify(ctx, mode, nullptr);
        if (cfg.status_request) {
            if (SSL_CTX_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp) != 1) {
                log_error("SSL_CTX_set_tlsext_status_type");
                result = false;
            }
        }

        if (cfg.status_request_v2) {
            constexpr int context = SSL_EXT_TLS_ONLY | SSL_EXT_TLS1_2_AND_BELOW_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION |
                                    SSL_EXT_CLIENT_HELLO | SSL_EXT_TLS1_2_SERVER_HELLO;
            if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_status_request_v2, context, override.status_request_v2_add,
                                       nullptr, nullptr, override.status_request_v2_cb,
                                       m_status_request_v2.get()) != 1) {
                log_error("SSL_CTX_add_custom_ext status_request_v2");
                result = false;
            }
        }

        if (cfg.status_request || cfg.status_request_v2) {
            if (SSL_CTX_set_tlsext_status_cb(ctx, override.tlsext_status_cb) != 1) {
                log_error("SSL_CTX_set_tlsext_status_cb");
                result = false;
            }
            if (SSL_CTX_set_tlsext_status_arg(ctx, m_status_request_v2.get()) != 1) {
                log_error("SSL_CTX_set_tlsext_status_arg");
                result = false;
            }
        }

        if (cfg.trusted_ca_keys) {
            constexpr int context = SSL_EXT_TLS_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION | SSL_EXT_CLIENT_HELLO;
            if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_trusted_ca_keys, context, override.trusted_ca_keys_add,
                                       override.trusted_ca_keys_free, &m_trusted_ca_keys, nullptr, nullptr) != 1) {
                log_error("SSL_CTX_add_custom_ext trusted_ca_keys");
                result = false;
            }
        }
    }

    if (result) {
        m_context->ctx = SSL_CTX_ptr(ctx);
    } else {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }

    return ctx != nullptr;
}

std::unique_ptr<ClientConnection> Client::connect(const char* host, const char* service, bool ipv6_only,
                                                  int timeout_ms) {
    BIO_ADDRINFO* addrinfo_tmp{nullptr};
    std::unique_ptr<ClientConnection> result;

    const int family = (ipv6_only) ? AF_INET6 : AF_UNSPEC;
    const bool lookup_result =
        BIO_lookup_ex(host, service, BIO_LOOKUP_CLIENT, family, SOCK_STREAM, IPPROTO_TCP, &addrinfo_tmp) != 0;

    if (!lookup_result) {
        log_error("connect::BIO_lookup_ex");
    } else {
        std::unique_ptr<BIO_ADDRINFO> addrinfo(addrinfo_tmp);
        const auto sock_family = BIO_ADDRINFO_family(addrinfo.get());
        const auto sock_type = BIO_ADDRINFO_socktype(addrinfo.get());
        const auto sock_protocol = BIO_ADDRINFO_protocol(addrinfo.get());
        const auto* sock_address = BIO_ADDRINFO_address(addrinfo.get());

        auto socket = BIO_socket(sock_family, sock_type, sock_protocol, 0);

        if (socket == INVALID_SOCKET) {
            log_error("connect::BIO_socket");
        } else {
            bool connected{true};
            constexpr int soc_opt = BIO_SOCK_NONBLOCK;

            // calls connect() - hence checking errno
            if (BIO_connect(socket, sock_address, soc_opt) != 1) {
                connected = false;
                if (errno == EINPROGRESS) {
                    // wait for write on the socket
                    auto res = wait_for_loop(socket, true, timeout_ms);
                    if (res == 0) {
                        log_error("connect::wait_for: timeout");
                    } else if (res > 0) {
                        res = BIO_sock_error(socket);
                        if (res == 0) {
                            connected = true;
                        } else {
                            log_error("connect::BIO_sock_error: " + std::to_string(res));
                        }
                    }
                } else {
                    log_error("connect::BIO_connect: " + std::to_string(errno));
                }
            }

            if (connected) {
                result = std::make_unique<ClientConnection>(m_context->ctx.get(), socket, host, service, m_timeout_ms);
            }
        }
    }

    return result;
}

Client::override_t Client::default_overrides() {
    return {
        &ClientStatusRequestV2::status_request_v2_multi_cb, &ClientStatusRequestV2::status_request_v2_add,
        &ClientStatusRequestV2::status_request_v2_cb,       &ClientTrustedCaKeys::trusted_ca_keys_add,
        &ClientTrustedCaKeys::trusted_ca_keys_free,
    };
}

} // namespace tls
