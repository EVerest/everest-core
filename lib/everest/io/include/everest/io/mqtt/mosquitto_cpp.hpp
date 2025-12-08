// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>

struct mosquitto;
struct mosquitto_message;
struct mqtt5__property;

namespace everest::lib::io::mqtt {

enum class ErrorCode : std::uint8_t {
    Success = 0,
    InvalidArgument,
    NoMemory,
    Errno, // check errno for more details
    NoConnection,
    ConnectionLost,
    ConnectionRefused,
    Protocol,
    PayloadSize,
    MalformedUTF8,
    DuplicateProperty,
    QoSNotSupported,
    OversizePacket,
    NotSupported,
    HostnameLookup,
    Tls,
    TlsHandshake,
    Unknown,
    // additional errors (non-mosquitto)
    MapInsert,
};

std::string_view to_string(ErrorCode ec);

class PropertiesBase {
protected:
    mqtt5__property* props;
    bool is_const;

public:
    PropertiesBase() : props(nullptr), is_const(true) {
    }
    PropertiesBase(const mqtt5__property* ptr) : props(const_cast<mqtt5__property*>(ptr)), is_const(true) {
    }
    PropertiesBase(mqtt5__property* ptr) : props(ptr), is_const(false) {
    }
    ~PropertiesBase();

    enum class property_t : std::uint8_t {
        AssignedClientId = 18,
        AuthenticationData = 22,
        AuthenticationMethod = 21,
        ContentType = 3,
        CorrelationData = 9,
        MaximumPacketSize = 39,
        MaximumQoS = 36,
        MessageExpiryInterval = 2,
        PayloadFormatIndicator = 1,
        ReasonString = 31,
        ReceiveMaximum = 33,
        RequestProblemInfo = 23,
        RequestResponseInfo = 25,
        ResponseInfo = 26,
        ResponseTopic = 8,
        RetainAvailable = 37,
        SubscriptionId = 11,
        SubscriptionIdAvailable = 41,
        ServerKeepAlive = 19,
        ServerRef = 28,
        SessionExpiryInterval = 17,
        SharedSubAvailable = 42,
        TopicAlias = 35,
        TopicAliasMaximum = 34,
        UserProperty = 38,
        WildcardSubAvailable = 40,
        WillDelayInterval = 24,
    };

    constexpr operator const mqtt5__property*() const {
        return props;
    }

    constexpr mqtt5__property* release() {
        mqtt5__property* ptr{nullptr};
        if (!is_const) {
            ptr = props;
            props = nullptr;
        }
        return ptr;
    }

    // useful after release() has been used
    static void free_property(mqtt5__property** ptr);

    const mqtt5__property* get_property(property_t prop) const;
    std::string get_response_topic() const;
    std::string get_correlation_data() const;
};

class Properties : public PropertiesBase {
public:
    using PropertiesBase::PropertiesBase;

    ErrorCode set_response_topic(const std::string& topic);
    ErrorCode set_correlation_data(const std::string& data);
};

class PropertiesAccess : public PropertiesBase {
public:
    using PropertiesBase::PropertiesBase;
};

using subscribe_callback = std::function<void(const std::string_view& topic, const std::string_view& payload)>;

class mosquitto_cpp {
public:
    enum class QoS : std::uint8_t {
        at_most_once = 0,
        at_least_once = 1,
        exactly_once = 2,
    };

    static constexpr QoS default_QoS = QoS::exactly_once;
    static constexpr QoS telemetry_QoS = QoS::at_most_once;
    static constexpr bool default_publish_retain = false;

    enum class LogLevel : std::uint8_t {
        info,
        notice,
        warning,
        error,
        debug,
    };

    enum class ResponseCode : std::uint8_t {
        Success = 0x00,

        v3_ProtocolError = 0x01,
        v3_IdentifierRejected = 0x02,
        v3_ServerUnavailable = 0x03,
        v3_BadUserNamePassword = 0x04,
        v3_NotAuthorized = 0x05,

        UnspecifiedError = 0x80,
        MalformedPacket = 0x81,
        ProtocolError = 0x82,
        ImplementationSpecificError = 0x83,
        UnsupportedProtocolVersion = 0x84,
        ClientIdentifierInvalid = 0x85,
        BadUserNamePassword = 0x86,
        NotAuthorized = 0x87,
        ServerUnavailable = 0x88,
        ServerBusy = 0x89,
        Banned = 0x8A,
        BadAuthenticationmethod = 0x8C,
        TopicNameInvalid = 0x90,
        PacketTooLarge = 0x95,
        QuotaExceeded = 0x97,
        PayloadFormatInvalid = 0x99,
        RetainNotSupported = 0x9A,
        QoSNotSupported = 0x9B,
        UseAnotherServer = 0x9C,
        ServerMoved = 0x9D,
        ConnectionRateExceeded = 0x9F,
    };

    struct message {
        std::string topic;
        std::string payload;
        QoS qos{default_QoS};
    };

    using connect_callback =
        std::function<void(mosquitto_cpp& client, ResponseCode rc, int flags, const PropertiesAccess& props)>;

    using disconnect_callback = std::function<void(mosquitto_cpp& client, ErrorCode ec, const PropertiesAccess& props)>;

    using publish_callback =
        std::function<void(mosquitto_cpp& client, int mid, ResponseCode rc, const PropertiesAccess& props)>;

    using subscribe_callback =
        std::function<void(mosquitto_cpp& client, const std::string_view& topic, const std::string_view& payload,
                           QoS qos, const PropertiesAccess& props)>;

    using subscribe_message_callback = std::function<void(mosquitto_cpp&, message const&)>;

private:
    std::unique_ptr<mosquitto, void (*)(mosquitto*)> client;
    std::map<std::string, subscribe_callback> callbacks;
    connect_callback connect_cb;
    disconnect_callback disconnect_cb;
    publish_callback publish_cb;

    static void cb_connect(mosquitto* mosq, void* obj, int rc, int flags, const mqtt5__property* props);
    static void cb_disconnect(mosquitto* mosq, void* obj, int rc, const mqtt5__property* props);
    static void cb_log(mosquitto* mosq, void* obj, int level, const char* str);
    static void cb_message(mosquitto* mosq, void* obj, const mosquitto_message* msg, const mqtt5__property* props);
    static void cb_publish(mosquitto* mosq, void* obj, int mid, int rc, const mqtt5__property* props);

    void connect_ccb(ResponseCode rc, int flags, const PropertiesAccess& props);
    void disconnect_ccb(ErrorCode ec, const PropertiesAccess& props);
    void message_ccb(const std::string_view& topic, const std::string_view& payload, QoS qos,
                     const PropertiesAccess& props);
    void publish_ccb(int mid, ResponseCode rc, const PropertiesAccess& props);

protected:
    virtual ErrorCode connect_impl(const std::string_view& bind_address, const std::string_view& host,
                                   std::uint16_t port, std::uint16_t keepalive_seconds);
    virtual ErrorCode set_will_impl(const std::string_view& topic, const std::string_view& payload, QoS qos,
                                    bool retain, PropertiesBase&& props);

    virtual ErrorCode publish_impl(int* mid, const std::string_view& topic, const std::string_view& payload, QoS qos,
                                   bool retain, const PropertiesBase& props);
    virtual ErrorCode subscribe_impl(const std::string_view& topic, QoS qos, int options, const PropertiesBase& props,
                                     subscribe_callback cb);
    virtual ErrorCode unsubscribe_impl(const std::string_view& topic, const PropertiesBase& props);

    virtual void set_callback_connect_impl(connect_callback cb);
    virtual void set_callback_disconnect_impl(disconnect_callback cb);
    virtual void set_callback_publish_impl(publish_callback cb);

    bool is_connect_callback_set();
    bool is_disconnect_callback_set();

public:
    mosquitto_cpp();
    mosquitto_cpp(const char* id);

    virtual ~mosquitto_cpp();

    virtual void log(LogLevel level, const char* str);

    void set_callback_connect(connect_callback cb);
    void set_callback_disconnect(disconnect_callback cb);
    void set_callback_publish(publish_callback cb);

    ErrorCode tls(const char* ca_file, const char* ca_path, const char* cert_file, const char* key_file);
    ErrorCode tls(const ::std::string& ca_file, const ::std::string& ca_path, const ::std::string& cert_file,
                  const ::std::string& key_file);
    ErrorCode connect(const std::string_view& host, std::uint16_t port, std::uint16_t keepalive_seconds);
    ErrorCode connect(const std::string_view& bind_address, const std::string_view& host, std::uint16_t port,
                      std::uint16_t keepalive_seconds);

    ErrorCode connect(const std::string_view& unix_domain_socket, std::uint16_t keepalive_seconds);

    ErrorCode reconnect();
    ErrorCode disconnect();

    ErrorCode loop_forever();

    ErrorCode loop_read();
    ErrorCode loop_write();
    ErrorCode loop_misc();

    bool want_write();
    int socket() const;

    void set_option_threaded(bool val);
    void set_option_tcpnodelay(bool val);

    // must be called before connect()
    ErrorCode set_will(const std::string_view& topic, const std::string_view& payload, QoS qos, bool retain,
                       PropertiesBase&& props);

    ErrorCode publish(int* mid, const std::string_view& topic, const std::string_view& payload, QoS qos, bool retain,
                      const PropertiesBase& props);

    ErrorCode publish(const std::string_view& topic, const std::string_view& payload, QoS qos, bool retain,
                      const PropertiesBase& props);

    ErrorCode publish(const std::string_view& topic, const std::string_view& payload);

    ErrorCode publish(message const& data);

    ErrorCode subscribe(const std::string_view& topic, QoS qos, int options, const PropertiesBase& props,
                        subscribe_callback cb);
    ErrorCode subscribe(const std::string_view& topic, subscribe_callback cb);

    ErrorCode subscribe(std::string_view const& topic, subscribe_message_callback const& cb, QoS qos);

    ErrorCode unsubscribe(const std::string_view& topic, const PropertiesBase& props);
    ErrorCode unsubscribe(const std::string_view& topic);

    static bool library_init();
    static void library_cleanup();
};
} // namespace everest::lib::io::mqtt
