// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef V2G_H
#define V2G_H

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>
#include <generated/interfaces/evse_security/Interface.hpp>

#include <atomic>
#include <cstdint>
#include <netinet/in.h>
#include <pthread.h>

#include <openssl_util.hpp>
#include <tls.hpp>

#include <cbv2g/app_handshake/appHand_Datatypes.h>
#include <cbv2g/common/exi_basetypes.h>
#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/din/din_msgDefDatatypes.h>
#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

/* timeouts in milliseconds */
#define V2G_SEQUENCE_TIMEOUT_60S 60000 /* [V2G2-443] et.al. */
#define V2G_SEQUENCE_TIMEOUT_10S 10000

#define ISO_15118_2013_MSG_DEF "urn:iso:15118:2:2013:MsgDef"
#define ISO_15118_2013_MAJOR   2

#define ISO_15118_2010_MSG_DEF "urn:iso:15118:2:2010:MsgDef"
#define ISO_15118_2010_MAJOR   1

#define DIN_70121_MSG_DEF "urn:din:70121:2012:MsgDef"
#define DIN_70121_MAJOR   2

#define EVSE_LEAF_KEY_FILE_NAME "CPO_EVSE_LEAF.key"
#define EVSE_PROV_KEY_FILE_NAME "PROV_LEAF.key"
#define MO_ROOT_CRT_NAME        "MO_ROOT_CRT"
#define V2G_ROOT_CRT_NAME       "V2G_ROOT_CRT"
#define MAX_V2G_ROOT_CERTS      10
#define MAX_KEY_PW_LEN          32
#define FORCE_PUB_MSG           25 // max msg cycles when topics values must be udpated
#define MAX_PCID_LEN            17

#define DEFAULT_BUFFER_SIZE 8192

#define DEBUG 1

enum tls_security_level {
    TLS_SECURITY_ALLOW = 0,
    TLS_SECURITY_PROHIBIT,
    TLS_SECURITY_FORCE
};

enum v2g_event {
    V2G_EVENT_NO_EVENT = 0,
    V2G_EVENT_TERMINATE_CONNECTION, // Terminate the connection immediately
    V2G_EVENT_SEND_AND_TERMINATE,   // Send next msg and terminate the connection
    V2G_EVENT_SEND_RECV_EXI_MSG,    // If msg must not be exi-encoded and can be sent directly
    V2G_EVENT_IGNORE_MSG            // Received message can't be handled
};

enum v2g_protocol {
    V2G_PROTO_DIN70121 = 0,
    V2G_PROTO_ISO15118_2010,
    V2G_PROTO_ISO15118_2013,
    V2G_PROTO_ISO15118_2015,
    V2G_UNKNOWN_PROTOCOL
};

/*!
 * \brief The res_msg_ids enum is a list of response msg ids
 */
enum V2gMsgTypeId {
    V2G_SUPPORTED_APP_PROTOCOL_MSG = 0,
    V2G_SESSION_SETUP_MSG,
    V2G_SERVICE_DISCOVERY_MSG,
    V2G_SERVICE_DETAIL_MSG,
    V2G_PAYMENT_SERVICE_SELECTION_MSG,
    V2G_PAYMENT_DETAILS_MSG,
    V2G_AUTHORIZATION_MSG,
    V2G_CHARGE_PARAMETER_DISCOVERY_MSG,
    V2G_METERING_RECEIPT_MSG,
    V2G_CERTIFICATE_UPDATE_MSG,
    V2G_CERTIFICATE_INSTALLATION_MSG,
    V2G_CHARGING_STATUS_MSG,
    V2G_CABLE_CHECK_MSG,
    V2G_PRE_CHARGE_MSG,
    V2G_POWER_DELIVERY_MSG,
    V2G_CURRENT_DEMAND_MSG,
    V2G_WELDING_DETECTION_MSG,
    V2G_SESSION_STOP_MSG,
    V2G_UNKNOWN_MSG
};

/* Struct for tls-session-log-key tracing */
typedef struct keylogDebugCtx {
    FILE* file;
    bool inClientRandom;
    bool inMasterSecret;
    uint8_t hexdumpLinesToProcess;
    int udp_socket;
    std::string udp_buffer;
} keylogDebugCtx;

/**
 * Abstracts a charging port, i.e. a power outlet in this daemon.
 *
 * **** NOTE ****
 * Be very careful about adding C++ objects since constructors and
 * destructors are not called. (see v2g_ctx_create() and calloc)
 */
struct v2g_context {
    std::atomic_bool shutdown;

    evse_securityIntf* r_security;

    struct event* com_setup_timeout;

    uint16_t proxy_port_iso2;
    uint16_t proxy_port_iso20;

    const char* if_name;
    struct sockaddr_in6* local_tcp_addr;
    struct sockaddr_in6* local_tls_addr;

    std::string certs_path;

    uint32_t network_read_timeout;     /* in milli seconds */
    uint32_t network_read_timeout_tls; /* in milli seconds */
    bool selected_iso20{false};

    enum tls_security_level tls_security;

    int sdp_socket;
    int tcp_socket;

    int udp_port;
    int udp_socket;

    pthread_t tcp_thread;

    struct {
        int fd;
    } tls_socket;
    tls::Server* tls_server;

    enum V2gMsgTypeId current_v2g_msg;         /* holds the last v2g msg type */
    int state;                                 /* holds the current state id */
    std::atomic_bool is_connection_terminated; /* Is set to true if the connection is terminated (CP State A/F, shutdown
                                      immediately without response message) */
};

/**
 * High-level abstraction of an incoming TCP/TLS connection on a certain charging port.
 */
struct v2g_connection {
    pthread_t thread_id;
    struct v2g_context* ctx;

    bool is_tls_connection;

    // used for non-TLS connections
    struct {
        int socket_fd;
    } conn;

    tls::Connection* tls_connection;
    openssl::pkey_ptr* pubkey;

    ssize_t (*read)(struct v2g_connection* conn, unsigned char* buf, std::size_t count, bool read_complete);
    ssize_t (*write)(struct v2g_connection* conn, unsigned char* buf, std::size_t count);
    int (*proxy)(struct v2g_connection* conn, int proxy_fd);

    /* V2GTP EXI encoding/decoding stuff */
    uint8_t* buffer;
    uint32_t payload_len;
    exi_bitstream_t stream;

    struct appHand_exiDocument handshake_req;
    struct appHand_exiDocument handshake_resp;
};

#endif /* V2G_H */
