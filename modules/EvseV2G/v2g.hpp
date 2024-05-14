// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef V2G_H
#define V2G_H

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>
#include <generated/interfaces/evse_security/Interface.hpp>
#include <mbedtls/certs.h>
#include <mbedtls/config.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ssl.h>
#include <mbedtls/version.h>
#include <mbedtls/x509.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#if MBEDTLS_VERSION_MINOR == 2
#include <mbedtls/net.h>
#else
#include <mbedtls/net_sockets.h>
#endif
#include <openv2g/EXITypes.h>
#include <openv2g/appHandEXIDatatypes.h>

#undef EXIFragment_ServiceScope_CHARACTERS_SIZE // Needed because of redefinition warnings (v2g and din type include)
#undef EXIFragment_Certificate_BYTES_SIZE
#undef EXIFragment_OEMProvisioningCert_BYTES_SIZE
#undef EXIFragment_EVCCID_BYTES_SIZE
#undef EXIFragment_SigMeterReading_BYTES_SIZE
#include <openv2g/iso1EXIDatatypes.h>
#undef EXIFragment_ServiceScope_CHARACTERS_SIZE
#undef EXIFragment_Certificate_BYTES_SIZE
#undef EXIFragment_OEMProvisioningCert_BYTES_SIZE
#undef EXIFragment_EVCCID_BYTES_SIZE
#undef EXIFragment_SigMeterReading_BYTES_SIZE
#include <event2/event.h>
#include <event2/thread.h>
#include <openv2g/dinEXIDatatypes.h>

/* timeouts in milliseconds */
#define V2G_SEQUENCE_TIMEOUT_60S              60000 /* [V2G2-443] et.al. */
#define V2G_SEQUENCE_TIMEOUT_10S              10000
#define V2G_CP_STATE_B_TO_C_D_TIMEOUT         250  /* [V2G2-847] */
#define V2G_CP_STATE_B_TO_C_D_TIMEOUT_RELAXED 500  /* [V2G2-847] */
#define V2G_CP_STATE_C_D_TO_B_TIMEOUT         250  /* [V2G2-848] */
#define V2G_CONTACTOR_CLOSE_TIMEOUT           3000 /* [V2G2-862] [V2G2-865] 4.5 s for PowerDeliveryRes */
#define V2G_COMMUNICATION_SETUP_TIMEOUT                                                                                \
    18000 /* [V2G2-723] [V2G2-029] [V2G2-032] [V2G2-714] [V2G2-716] V2G_SECC_CommunicationSetup_Performance_Time */
#define V2G_CPSTATE_DETECTION_TIMEOUT                                                                                  \
    1500 /* [V2G-DC-547] not (yet) defined for ISO and not implemented, but may be implemented */
#define V2G_CPSTATE_DETECTION_TIMEOUT_RELAXED                                                                          \
    3000 /* [V2G-DC-547] not (yet) defined for ISO and not implemented, but may be implemented */

#define SA_SCHEDULE_DURATION 86400

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

/* ISO 15118 table 105 */
enum v2g_service {
    V2G_SERVICE_ID_CHARGING = 1,
    V2G_SERVICE_ID_CERTIFICATE = 2,
    V2G_SERVICE_ID_INTERNET = 3,
    V2G_SERVICE_ID_USECASEINFORMATION = 4,
};

/*!
 * \brief The charging_phase enum to identify the actual charing phase.
 */
enum charging_phase {
    PHASE_INIT = 0,
    PHASE_AUTH,
    PHASE_PARAMETER,
    PHASE_ISOLATION,
    PHASE_PRECHARGE,
    PHASE_CHARGE,
    PHASE_WELDING,
    PHASE_STOP,
    PHASE_LENGTH
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

/* EVSE ID */
struct v2g_evse_id {
    uint8_t bytes[iso1SessionSetupResType_EVSEID_CHARACTERS_SIZE];
    uint16_t bytesLen;
};

/* Meter ID */
struct v2g_meter_id {
    uint8_t bytes[iso1MeterInfoType_MeterID_CHARACTERS_SIZE];
    uint16_t bytesLen;
};

/* Struct for tls-session-log-key tracing */
typedef struct keylogDebugCtx {
    FILE* file;
    bool inClientRandom;
    bool inMasterSecret;
    uint8_t hexdumpLinesToProcess;
} keylogDebugCtx;

struct SAE_Bidi_Data {
    bool enabled_sae_v2h;
    bool enabled_sae_v2g;
    int8_t sae_v2h_minimal_soc;
    bool discharging;
};

/**
 * Abstracts a charging port, i.e. a power outlet in this daemon.
 */
struct v2g_context {
    volatile int shutdown;

    evse_securityIntf* r_security;
    ISO15118_chargerImplBase* p_charger;

    struct event_base* event_base;
    pthread_t event_thread;

    struct event* com_setup_timeout;

    const char* if_name;
    struct sockaddr_in6* local_tcp_addr;
    struct sockaddr_in6* local_tls_addr;

    std::string certs_path;
    std::string tls_key_logging_path;

    uint32_t network_read_timeout;     /* in milli seconds */
    uint32_t network_read_timeout_tls; /* in milli seconds */

    enum tls_security_level tls_security;

    int sdp_socket;
    int tcp_socket;
    pthread_t tcp_thread;

    mbedtls_ssl_config ssl_config;
    mbedtls_x509_crt* evseTlsCrt;
    uint8_t num_of_tls_crt;
    mbedtls_pk_context* evse_tls_crt_key;
    mbedtls_x509_crt v2g_root_crt;
    mbedtls_net_context tls_socket;
    keylogDebugCtx tls_log_ctx;
    bool tls_key_logging;
    pthread_t tls_thread;

    mbedtls_x509_crt mop_root_ca_list;

    pthread_mutex_t mqtt_lock;
    pthread_cond_t mqtt_cond;
    pthread_condattr_t mqtt_attr;

    struct {
        float evse_ac_current_limit; // default is 0
    } basic_config;                  // This config will not reseted after beginning of a new charging session

    /* actual charging state */
    enum V2gMsgTypeId last_v2g_msg;    /* holds the current v2g msg type */
    enum V2gMsgTypeId current_v2g_msg; /* holds the last v2g msg type */
    int state;                         /* holds the current state id */
    bool is_dc_charger;         /* Is set to true if it is a DC charger. Value is configured after configuration of the
                                   supported energy type */
    bool debugMode;             /* To activate/deactivate the debug mode */
    int8_t supported_protocols; /* Is an bit mask and holds the supported app protocols. See v2g_protocol enum */
    enum v2g_protocol selected_protocol; /* Holds the selected protocole after supported app protocol */
    std::atomic<bool>
        intl_emergency_shutdown; /* Is set to true if an internal emergency_shutdown has occurred (send failed response,
                                    configure emergency shutdown in EVSEStatus and close tcp connection) */
    std::atomic_bool stop_hlc; /* is set to true if a shutdown of the charging session should be initiated (send failed
                      response and close tcp connection) */
    std::atomic_bool is_connection_terminated; /* Is set to true if the connection is terminated (CP State A/F, shutdown
                                      immediately without response message) */
    std::atomic<bool> terminate_connection_on_failed_response;
    std::atomic<bool> contactor_is_closed; /* Actual contactor state */

    struct {
        bool meter_info_is_used;
        uint64_t meter_reading;
        struct v2g_meter_id meter_id;
    } meter_info;

    struct {
        /* EVSE V2G values */
        uint64_t session_id; // Is the evse session id, generated by the evse. This id shall not change during a V2G
                             // Communication Session.
        uint32_t notification_max_delay;
        uint8_t evse_isolation_status;
        unsigned int evse_isolation_status_is_used;
        uint8_t evse_notification;
        uint8_t evse_status_code[PHASE_LENGTH];
        uint8_t evse_processing[PHASE_LENGTH];
        struct v2g_evse_id evse_id;
        unsigned int date_time_now_is_used;
        struct iso1ChargeServiceType charge_service;
        struct iso1ServiceType evse_service_list[iso1ServiceListType_Service_ARRAY_SIZE];
        struct iso1ServiceParameterListType service_parameter_list[iso1ServiceListType_Service_ARRAY_SIZE];
        uint16_t evse_service_list_len;

        struct iso1SAScheduleListType evse_sa_schedule_list;
        bool evse_sa_schedule_list_is_used;

        iso1paymentOptionType payment_option_list[iso1PaymentOptionListType_PaymentOption_ARRAY_SIZE];
        uint8_t payment_option_list_len;

        bool cert_install_status;
        std::string cert_install_res_b64_buffer;

        // AC parameter
        int rcd;
        int receipt_required;

        // evse power electronic values
        struct iso1PhysicalValueType evse_current_regulation_tolerance;
        unsigned int evse_current_regulation_tolerance_is_used;
        struct iso1PhysicalValueType evse_energy_to_be_delivered;
        unsigned int evse_energy_to_be_delivered_is_used;
        struct iso1PhysicalValueType evse_maximum_current_limit; // DC charging
        unsigned int evse_maximum_current_limit_is_used;
        int evse_current_limit_achieved;
        struct iso1PhysicalValueType evse_maximum_power_limit;
        unsigned int evse_maximum_power_limit_is_used;
        int evse_power_limit_achieved;
        struct iso1PhysicalValueType evse_maximum_voltage_limit;
        unsigned int evse_maximum_voltage_limit_is_used;
        int evse_voltage_limit_achieved;
        struct iso1PhysicalValueType evse_minimum_current_limit;
        struct iso1PhysicalValueType evse_minimum_voltage_limit;
        struct iso1PhysicalValueType evse_peak_current_ripple;
        struct iso1PhysicalValueType evse_present_voltage;
        struct iso1PhysicalValueType evse_present_current;

        /* AC only power electronic values */
        struct iso1PhysicalValueType evse_nominal_voltage;

        // Specific SAE J2847 bidi values
        struct SAE_Bidi_Data sae_bidi_data;

    } evse_v2g_data;

    struct {
        /* V2G session values */
        iso1paymentOptionType iso_selected_payment_option;
        long long int auth_start_timeout;
        int auth_timeout_eim;
        int auth_timeout_pnc;                                       // for PnC
        uint8_t gen_challenge[16];                                  // for PnC
        bool verify_contract_cert_chain;                            // for PnC
        types::authorization::CertificateStatus certificate_status; // for PnC

        struct {
            bool valid_crt;
            mbedtls_x509_crt crt;
            mbedtls_ecdsa_context pubkey;
        } contract;                   // for PnC
        bool renegotiation_required;  /* Is set to true if renegotiation is required. Only relevant for ISO */
        bool is_charging;             /* set to true if ChargeProgress is set to Start */
        uint8_t sa_schedule_tuple_id; /* selected SA schedule tuple ID*/
    } session;

    struct {
        /* EV V2G values */
        int bulk_charging_complete;
        int charging_complete;
        uint64_t received_session_id; // Is the received ev session id transmitted over the v2g header. This id shall
                                      // not change during a V2G Communication Session.

        union {
            struct dinDC_EVStatusType din_dc_ev_status;
            struct iso1DC_EVStatusType iso1_dc_ev_status;
        };
        float ev_maximum_current_limit;
        float ev_maximum_power_limit;
        float ev_maximum_voltage_limit;
        float v2g_target_current;
        float v2g_target_voltage;
        float remaining_time_to_bulk_soc;
        float remaining_time_to_full_soc;
    } ev_v2g_data;

    bool hlc_pause_active;
};

enum mqtt_dlink_action {
    MQTT_DLINK_ACTION_ERROR,
    MQTT_DLINK_ACTION_TERMINATE,
    MQTT_DLINK_ACTION_PAUSE,
};

/**
 * High-level abstraction of an incoming TCP/TLS connection on a certain charging port.
 */
struct v2g_connection {
    pthread_t thread_id;
    struct v2g_context* ctx;

    bool is_tls_connection;
    union {
        struct {
            mbedtls_ssl_config* ssl_config;
            mbedtls_ssl_context ssl_context;
            mbedtls_net_context tls_client_fd;
        } ssl;
        int socket_fd;
    } conn;

    /* V2GTP EXI encoding/decoding stuff */
    uint8_t* buffer;
    size_t buffer_pos;
    uint32_t payload_len;
    bitstream_t stream;

    struct appHandEXIDocument handshake_req;
    struct appHandEXIDocument handshake_resp;

    union {
        struct dinEXIDocument* dinEXIDocument;
        struct iso1EXIDocument* iso1EXIDocument;
    } exi_in;

    union {
        struct dinEXIDocument* dinEXIDocument;
        struct iso1EXIDocument* iso1EXIDocument;
    } exi_out;

    enum mqtt_dlink_action dlink_action; /* signaled action after connection is closed */
};

#endif /* V2G_H */
