/*
 * Licensor: Pionix GmbH, 2025
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include <everest/io/mqtt/mosquitto_cpp.hpp>
// #include <companion/utilities/String.hpp>
// #include <utils/logging.hpp>

#include <cstdio>
#include <iostream>
#include <mosquitto.h>
#include <stdexcept>
/**
 * \file MQTT v5 client
 */

namespace {
using namespace everest::lib::io::mqtt;
constexpr int loop_timeout_ms = -1; // use default

int password_callback(char* buf, int size, [[maybe_unused]] int rwflag, [[maybe_unused]] void* userdata) {
    if ((buf != nullptr) && (size > 0)) {
        *buf = '\0';
    }
    return 0;
}

constexpr mosquitto_cpp::QoS convert_to_qos(int qos) {
    switch (qos) {
    case static_cast<int>(mosquitto_cpp::QoS::at_least_once):
    case static_cast<int>(mosquitto_cpp::QoS::at_most_once):
    case static_cast<int>(mosquitto_cpp::QoS::exactly_once):
        return static_cast<mosquitto_cpp::QoS>(qos);
    }

    throw std::out_of_range("QoS from int: " + std::to_string(qos));
}

// clang-format off
#define FOR_ALL_ERROR_CODES(apply) \
    apply(SUCCESS, Success) \
    apply(INVAL, InvalidArgument) \
    apply(NOMEM, NoMemory) \
    apply(ERRNO, Errno) \
    apply(NO_CONN, NoConnection) \
    apply(CONN_LOST, ConnectionLost) \
    apply(CONN_REFUSED, ConnectionRefused) \
    apply(PROTOCOL, Protocol) \
    apply(PAYLOAD_SIZE, PayloadSize) \
    apply(MALFORMED_UTF8, MalformedUTF8) \
    apply(DUPLICATE_PROPERTY, DuplicateProperty) \
    apply(QOS_NOT_SUPPORTED, QoSNotSupported) \
    apply(OVERSIZE_PACKET, OversizePacket) \
    apply(NOT_SUPPORTED, NotSupported) \
    apply(EAI, HostnameLookup) \
    apply(TLS, Tls) \
    apply(TLS_HANDSHAKE, TlsHandshake)

// clang-format off
#define VALUES(a, b) \
    case MOSQ_ERR_##a: return ErrorCode::b;
// clang-format on

constexpr ErrorCode convertEC(int rc) {
    switch (rc) {
        FOR_ALL_ERROR_CODES(VALUES)
    default:
        //        LogError << "Unrecognised mosquitto error: " << rc;
        return ErrorCode::Unknown;
    }
}
#undef VALUES

} // namespace

namespace everest::lib::io::mqtt {

PropertiesBase::~PropertiesBase() {
    if (!is_const) {
        mosquitto_property_free_all(&props);
    }
}

void PropertiesBase::free_property(mqtt5__property** ptr) {
    mosquitto_property_free_all(ptr);
}

const mqtt5__property* PropertiesBase::get_property(property_t prop) const {
    const mosquitto_property* result{nullptr};
    for (const auto* p = props; p != nullptr; p = mosquitto_property_next(p)) {
        if (mosquitto_property_identifier(p) == static_cast<int>(prop)) {
            result = p;
            break;
        }
    }
    return result;
}

std::string PropertiesBase::get_response_topic() const {
    std::string result;
    char* topic{nullptr};
    if (mosquitto_property_read_string(props, static_cast<int>(property_t::ResponseTopic), &topic, false) != nullptr) {
        result = topic;
    }
    ::free(topic);
    //    LogDebug << "response_topic (" << props << "): " << result;
    return result;
}

std::string PropertiesBase::get_correlation_data() const {
    std::string result;
    void* data{nullptr};
    std::uint16_t len{0};
    if (mosquitto_property_read_binary(props, static_cast<int>(property_t::CorrelationData), &data, &len, false) !=
        nullptr) {
        result = std::string{static_cast<char*>(data), len};
    }
    ::free(data);
    //    LogDebug << "correlation_data (" << props << "): " << result;
    return result;
}

ErrorCode Properties::set_response_topic(const std::string& topic) {
    const auto result =
        convertEC(mosquitto_property_add_string(&props, static_cast<int>(property_t::ResponseTopic), topic.data()));
    if (result != ErrorCode::Success) {
        //        LogError << "set_response_topic: " << to_string(result);
    }
    return result;
}

ErrorCode Properties::set_correlation_data(const std::string& data) {
    const auto result = convertEC(
        mosquitto_property_add_binary(&props, static_cast<int>(property_t::CorrelationData), data.data(), data.size()));
    if (result != ErrorCode::Success) {
        //        LogError << "set_correlation_data: " << to_string(result);
    }
    return result;
}

// clang-format off
#define VALUES(a, b) \
    case ErrorCode::b: return {#b};
// clang-format on

std::string_view to_string(ErrorCode ec) {
    switch (ec) {
        FOR_ALL_ERROR_CODES(VALUES)
    default:
        return {"<unknown>"};
    }
}
#undef VALUES

void mosquitto_cpp::cb_connect([[maybe_unused]] mosquitto* mosq, void* obj, int rc, int flags,
                               const mqtt5__property* props) {
    if (obj != nullptr) {
        auto& client = *reinterpret_cast<mosquitto_cpp*>(obj);
        const PropertiesAccess p(props);
        const auto r = static_cast<ResponseCode>(rc);
        client.connect_ccb(r, flags, p);
    }
}

void mosquitto_cpp::cb_disconnect([[maybe_unused]] mosquitto* mosq, void* obj, int rc, const mqtt5__property* props) {
    if (obj != nullptr) {
        auto& client = *reinterpret_cast<mosquitto_cpp*>(obj);
        const PropertiesAccess p(props);
        const auto ec = convertEC(rc);
        client.disconnect_ccb(ec, p);
        client.callbacks.clear();
    }
}

void mosquitto_cpp::cb_log([[maybe_unused]] mosquitto* mosq, void* obj, int level, const char* str) {
    if (obj != nullptr) {
        auto& client = *reinterpret_cast<mosquitto_cpp*>(obj);
        LogLevel l;
        switch (level) {
        case MOSQ_LOG_INFO:
            l = LogLevel::info;
            break;
        case MOSQ_LOG_NOTICE:
            l = LogLevel::notice;
            break;
        case MOSQ_LOG_WARNING:
            l = LogLevel::warning;
            break;
        case MOSQ_LOG_ERR:
            l = LogLevel::error;
            break;
        case MOSQ_LOG_DEBUG:
        default:
            l = LogLevel::debug;
            break;
        }
        client.log(l, str);
    }
}

void mosquitto_cpp::cb_message([[maybe_unused]] mosquitto* mosq, void* obj, const mosquitto_message* msg,
                               const mqtt5__property* props) {
    if ((obj != nullptr) && (msg != nullptr)) {
        auto& client = *reinterpret_cast<mosquitto_cpp*>(obj);
        const PropertiesAccess p(props);
        const std::string_view topic(msg->topic);
        const std::string_view payload(static_cast<const char*>(msg->payload), msg->payloadlen);
        const QoS qos = convert_to_qos(msg->qos);
        client.message_ccb(topic, payload, qos, p);
    }
}

void mosquitto_cpp::cb_publish([[maybe_unused]] mosquitto* mosq, void* obj, int mid, int rc,
                               const mqtt5__property* props) {
    if (obj != nullptr) {
        auto& client = *reinterpret_cast<mosquitto_cpp*>(obj);
        const PropertiesAccess p(props);
        const auto r = static_cast<ResponseCode>(rc);
        client.publish_ccb(mid, r, p);
    }
}

void mosquitto_cpp::connect_ccb(ResponseCode rc, int flags, const PropertiesAccess& props) {
    if (connect_cb) {
        connect_cb(*this, rc, flags, props);
    }
}

void mosquitto_cpp::disconnect_ccb(ErrorCode ec, const PropertiesAccess& props) {
    if (disconnect_cb) {
        disconnect_cb(*this, ec, props);
    }
}

void mosquitto_cpp::message_ccb(const std::string_view& topic, const std::string_view& payload, QoS qos,
                                const PropertiesAccess& props) {
    //    LogDebug << "MQTT msg " << topic << ' ' << bc_utils::hex_string(payload);
    for (const auto& [sub_topic, callback] : callbacks) {
        bool matches{false};
        if ((mosquitto_topic_matches_sub2(sub_topic.data(), sub_topic.size(), topic.data(), topic.size(), &matches) ==
             MOSQ_ERR_SUCCESS) &&
            matches) {
            callback(*this, topic, payload, qos, props);
        }
    }
}

void mosquitto_cpp::publish_ccb(int mid, ResponseCode rc, const PropertiesAccess& props) {
    if (publish_cb) {
        publish_cb(*this, mid, rc, props);
    }
}

mosquitto_cpp::mosquitto_cpp() : mosquitto_cpp(nullptr) {
}

mosquitto_cpp::mosquitto_cpp(const char* id) : client(nullptr, nullptr) {
    mosquitto* ptr = mosquitto_new(id, true, this);
    std::unique_ptr<mosquitto, decltype(&mosquitto_destroy)> tmp(ptr, &mosquitto_destroy);
    client.swap(tmp);
    // use v5
    mosquitto_int_option(client.get(), MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

    // TODO(james-ctc): select sensible defaults
    //  mosquitto_int_option(client.get(), MOSQ_OPT_RECEIVE_MAXIMUM, 20);
    //  mosquitto_int_option(client.get(), MOSQ_OPT_SEND_MAXIMUM, 20);

    // Enable TCP no delay
    // mosquitto_int_option(client.get(), MOSQ_OPT_TCP_NODELAY, 1);

    //  multiple threads are likely
    mosquitto_threaded_set(client.get(), true);
    mosquitto_connect_v5_callback_set(client.get(), cb_connect);
    mosquitto_disconnect_v5_callback_set(client.get(), cb_disconnect);
    mosquitto_log_callback_set(client.get(), cb_log);
    mosquitto_message_v5_callback_set(client.get(), cb_message);
    mosquitto_publish_v5_callback_set(client.get(), cb_publish);
    // mosquitto_subscribe_v5_callback_set(client.get(), cb_subscribe);
}

mosquitto_cpp::~mosquitto_cpp() {
    disconnect();
}

void mosquitto_cpp::log(LogLevel level, [[maybe_unused]] const char* str) {
    switch (level) {
    case LogLevel::info:
    case LogLevel::notice:
        //        LogInfo << str;
        break;
    case LogLevel::warning:
        //        LogWarning << str;
        break;
    case LogLevel::error:
        //        LogError << str;
        break;
    case LogLevel::debug:
        // LogDebug << str;
        break;
    default:
        // should not occur
        //        LogCritical << str;
        break;
    }
}

void mosquitto_cpp::set_callback_connect_impl(connect_callback cb) {
    connect_cb = std::move(cb);
}

void mosquitto_cpp::set_callback_disconnect_impl(disconnect_callback cb) {
    disconnect_cb = std::move(cb);
}

void mosquitto_cpp::set_callback_publish_impl(publish_callback cb) {
    publish_cb = std::move(cb);
}

void mosquitto_cpp::set_callback_connect(connect_callback cb) {
    set_callback_connect_impl(std::move(cb));
}

void mosquitto_cpp::set_callback_disconnect(disconnect_callback cb) {
    set_callback_disconnect_impl(std::move(cb));
}

void mosquitto_cpp::set_callback_publish(publish_callback cb) {
    set_callback_publish_impl(std::move(cb));
}

bool mosquitto_cpp::is_connect_callback_set() {
    return static_cast<bool>(connect_cb);
}
bool mosquitto_cpp::is_disconnect_callback_set() {
    return static_cast<bool>(disconnect_cb);
}

ErrorCode mosquitto_cpp::tls(const char* ca_file, const char* ca_path, const char* cert_file, const char* key_file) {
    return convertEC(mosquitto_tls_set(client.get(), ca_file, ca_path, cert_file, key_file, password_callback));
}

ErrorCode mosquitto_cpp::tls(const ::std::string& ca_file, const ::std::string& ca_path, const ::std::string& cert_file,
                             const ::std::string& key_file) {
    const char* ca_file_ptr{nullptr};
    const char* ca_path_ptr{nullptr};
    if (!ca_file.empty()) {
        ca_file_ptr = ca_file.c_str();
    }
    if (!ca_path.empty()) {
        ca_path_ptr = ca_path.c_str();
    }
    return tls(ca_file_ptr, ca_path_ptr, cert_file.c_str(), key_file.c_str());
}

ErrorCode mosquitto_cpp::connect(const std::string_view& host, std::uint16_t port, std::uint16_t keepalive_seconds) {
    return connect_impl("", host, port, keepalive_seconds);
}

ErrorCode mosquitto_cpp::connect(const std::string_view& bind_address, const std::string_view& host, std::uint16_t port,
                                 std::uint16_t keepalive_seconds) {
    return connect_impl(bind_address, host, port, keepalive_seconds);
}

ErrorCode mosquitto_cpp::connect(const std::string_view& unix_domain_socket, std::uint16_t keepalive_seconds) {
    return connect_impl(unix_domain_socket, "", 0, keepalive_seconds);
}

ErrorCode mosquitto_cpp::connect_impl(const std::string_view& bind_address, const std::string_view& host,
                                      std::uint16_t port, std::uint16_t keepalive_seconds) {
    if (bind_address.empty()) {
        return convertEC(mosquitto_connect_async(client.get(), host.data(), port, keepalive_seconds));
    }
    return convertEC(mosquitto_connect_bind_async(client.get(), host.data(), port, keepalive_seconds, bind_address.data()));
}

ErrorCode mosquitto_cpp::reconnect() {
    return convertEC(mosquitto_reconnect_async(client.get()));
}

ErrorCode mosquitto_cpp::disconnect() {
    return convertEC(mosquitto_disconnect(client.get()));
}

ErrorCode mosquitto_cpp::loop_forever() {
    return convertEC(mosquitto_loop_forever(client.get(), loop_timeout_ms, 1));
}

ErrorCode mosquitto_cpp::loop_read() {
    return convertEC(mosquitto_loop_read(client.get(), 1));
}

ErrorCode mosquitto_cpp::loop_write() {
    return convertEC(mosquitto_loop_write(client.get(), 1));
}

ErrorCode mosquitto_cpp::loop_misc() {
    return convertEC(mosquitto_loop_misc(client.get()));
}

bool mosquitto_cpp::want_write() {
    return mosquitto_want_write(client.get());
}

int mosquitto_cpp::socket() const {
    return mosquitto_socket(client.get());
}

void mosquitto_cpp::set_option_threaded(bool val) {
    (void)mosquitto_threaded_set(client.get(), val);
}

void mosquitto_cpp::set_option_tcpnodelay(bool val) {
    (void)mosquitto_int_option(client.get(), MOSQ_OPT_TCP_NODELAY, val);
}

ErrorCode mosquitto_cpp::set_will(const std::string_view& topic, const std::string_view& payload, QoS qos, bool retain,
                                  PropertiesBase&& props) {
    return set_will_impl(topic, payload, qos, retain, std::move(props));
}

ErrorCode mosquitto_cpp::set_will_impl(const std::string_view& topic, const std::string_view& payload, QoS qos,
                                       bool retain, PropertiesBase&& props) {
    // must be called before connect()

    // On success only, the property list becomes the property of libmosquitto
    // once this function is called and will be freed by the library.
    // The property list must be freed by the application on error.

    ErrorCode result;
    auto* prop_p = props.release();
    result = convertEC(mosquitto_will_set_v5(client.get(), topic.data(), payload.size(), payload.data(),
                                             static_cast<int>(qos), retain, prop_p));
    if (result != ErrorCode::Success) {
        PropertiesBase::free_property(&prop_p);
    }
    return result;
}

ErrorCode mosquitto_cpp::publish(int* mid, const std::string_view& topic, const std::string_view& payload, QoS qos,
                                 bool retain, const PropertiesBase& props) {
    return publish_impl(mid, topic, payload, qos, retain, props);
}

ErrorCode mosquitto_cpp::publish(const std::string_view& topic, const std::string_view& payload, QoS qos, bool retain,
                                 const PropertiesBase& props) {
    return publish(nullptr, topic, payload, qos, retain, props);
}

ErrorCode mosquitto_cpp::publish(const std::string_view& topic, const std::string_view& payload) {
    return publish(nullptr, topic, payload, default_QoS, false, {});
}

ErrorCode mosquitto_cpp::publish(message const& data) {
    return publish(data.topic, data.payload, data.qos, false, {});
}

ErrorCode mosquitto_cpp::publish_impl(int* mid, const std::string_view& topic, const std::string_view& payload, QoS qos,
                                      bool retain, const PropertiesBase& props) {
    //    LogDebug << "MQTT pub " << topic << ' ' << bc_utils::hex_string(payload);
    return convertEC(mosquitto_publish_v5(client.get(), mid, topic.data(), payload.size(), payload.data(),
                                          static_cast<int>(qos), retain, props));
}

ErrorCode mosquitto_cpp::subscribe(const std::string_view& topic, QoS qos, int options, const PropertiesBase& props,
                                   subscribe_callback cb) {
    return subscribe_impl(topic, qos, options, props, std::move(cb));
}

ErrorCode mosquitto_cpp::subscribe(const std::string_view& topic, subscribe_callback cb) {
    return subscribe(topic, default_QoS, 0, {}, std::move(cb));
}

ErrorCode mosquitto_cpp::subscribe(std::string_view const& topic, subscribe_message_callback const& cb, QoS qos) {
    return subscribe(topic, qos, 0, {},
                     [cb = std::move(cb)](auto& cb_client, auto const& cb_topic, auto const& cb_payload, QoS cb_qos,
                                          [[maybe_unused]] auto const& props) {
                         message data;
                         data.topic = cb_topic;
                         data.payload = cb_payload;
                         data.qos = cb_qos;
                         cb(cb_client, data);
                     });
}

ErrorCode mosquitto_cpp::subscribe_impl(const std::string_view& topic, QoS qos, int options,
                                        const PropertiesBase& props, subscribe_callback cb) {
    //    LogDebug << "MQTT sub " << topic;
    auto result =
        convertEC(mosquitto_subscribe_v5(client.get(), nullptr, topic.data(), static_cast<int>(qos), options, props));
    if (result == ErrorCode::Success) {
        const auto [it, success] = callbacks.insert({std::string{topic}, std::move(cb)});
        if (!success) {
            result = ErrorCode::MapInsert;
        }
    }
    return result;
}

ErrorCode mosquitto_cpp::unsubscribe(const std::string_view& topic, const PropertiesBase& props) {
    return unsubscribe_impl(topic, props);
}

ErrorCode mosquitto_cpp::unsubscribe_impl(const std::string_view& topic, const PropertiesBase& props) {
    auto result = convertEC(mosquitto_unsubscribe_v5(client.get(), nullptr, topic.data(), props));
    if (result == ErrorCode::Success) {
        callbacks.erase(std::string{topic});
    }
    return result;
}

bool mosquitto_cpp::library_init() {
    return mosquitto_lib_init() == MOSQ_ERR_SUCCESS;
}

void mosquitto_cpp::library_cleanup() {
    (void)mosquitto_lib_cleanup();
}
} // namespace everest::lib::io::mqtt
