// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef SLAC_SLAC_HPP
#define SLAC_SLAC_HPP

#include <cstdint>
#include <utility>

#include <net/ethernet.h>

namespace slac {

// TODO (aw):
//  - is run_id 8 or 16 bytes?
//  - is nid 7 or 8 bytes?

namespace defs {

const uint16_t ETH_P_HOMEPLUG_GREENPHY = 0x88E1;

enum class MMV : uint8_t {
    AV_1_0 = 0x0,
    AV_1_1 = 0x1,
    AV_2_0 = 0x2,
};

const int MME_MIN_LENGTH = 60;

const int STATION_ID_LEN = 17;
const int NID_LEN = 7;
const int NID_MOST_SIGNIFANT_BYTE_SHIFT = 4;
const uint8_t NID_SECURITY_LEVEL_SIMPLE_CONNECT = 0b00;
const int NID_SECURITY_LEVEL_OFFSET = 4;

const uint8_t DAKS_HASH[] = {0x08, 0x85, 0x6d, 0xaf, 0x7c, 0xf5, 0x81, 0x85};
const uint8_t NMK_HASH[] = {0x08, 0x85, 0x6d, 0xaf, 0x7c, 0xf5, 0x81, 0x86};

const int NMK_LEN = 16;

const int AAG_LIST_LEN = 58;
const int RUN_ID_LEN = 8;

// FIXME (aw): where to put these iso15118/3 consts?
const int C_EV_START_ATTEN_CHAR_INDS = 3;
const int C_EV_MATCH_RETRY = 2;
const int C_EV_MATCH_MNBC = 10;
const int TP_EV_BATCH_MSG_INTERVAL_MS = 40; // 20ms - 50ms, interval between start_atten_char and mnbc_sound msgs
const int TT_EV_ATTEN_RESULTS_MS = 1200;    // max. 1200ms
const int TT_EVSE_MATCH_MNBC_MS = 600;
const int TT_MATCH_SEQUENCE_MS = 400;
const int TT_MATCH_RESPONSE_MS = 200;
const int TT_EVSE_MATCH_SESSION_MS = 10000;
const int TT_EVSE_SLAC_INIT_MS = 40000; // (20s - 50s)
const int TT_MATCH_JOIN_MS = 12000;     // max. 12s
const int T_STEP_EF_MS = 4000;          // min. 4s

const uint16_t MMTYPE_CM_SET_KEY = 0x6008;
const uint16_t MMTYPE_CM_SLAC_PARAM = 0x6064;
const uint16_t MMTYPE_CM_START_ATTEN_CHAR = 0x6068;
const uint16_t MMTYPE_CM_ATTEN_CHAR = 0x606C;
const uint16_t MMTYPE_CM_MNBC_SOUND = 0x6074;
const uint16_t MMTYPE_CM_VALIDATE = 0x6078;
const uint16_t MMTYPE_CM_SLAC_MATCH = 0x607C;
const uint16_t MMTYPE_CM_ATTEN_PROFILE = 0x6084;

// Qualcomm Vendor MMEs
namespace qualcomm {
const uint16_t MMTYPE_CM_RESET_DEVICE = 0xA01C;
const uint16_t MMTYPE_LINK_STATUS = 0xA0B8;
const uint16_t MMTYPE_OP_ATTR = 0xA068;
const uint16_t MMTYPE_NW_INFO = 0xA038;
const uint16_t MMTYPE_GET_SW = 0xA000;
} // namespace qualcomm

// Lumissil Vendor MMEs
namespace lumissil {
const uint16_t MMTYPE_NSCM_RESET_DEVICE = 0xAC70;
const uint16_t MMTYPE_NSCM_GET_VERSION = 0xAC6C;
const uint16_t MMTYPE_NSCM_GET_D_LINK_STATUS = 0xAC9C;
} // namespace lumissil

// Standard mmtypes
const uint16_t MMTYPE_MODE_REQ = 0x0000;
const uint16_t MMTYPE_MODE_CNF = 0x0001;
const uint16_t MMTYPE_MODE_IND = 0x0002;
const uint16_t MMTYPE_MODE_RSP = 0x0003;
const uint16_t MMTYPE_MODE_MASK = 0x0003;

const uint16_t MMTYPE_CATEGORY_STA_CCO = 0x0000;
const uint16_t MMTYPE_CATEGORY_PROXY = 0x2000;
const uint16_t MMTYPE_CATEGORY_CCO_CCO = 0x4000;
const uint16_t MMTYPE_CATEGORY_STA_STA = 0x6000;
const uint16_t MMTYPE_CATEGORY_MANUFACTOR_SPECIFIC = 0x8000;
const uint16_t MMTYPE_CATEGORY_VENDOR_SPECIFIC = 0xA000;
const uint16_t MMTYPE_CATEGORY_MASK = 0xE000;

const uint8_t COMMON_APPLICATION_TYPE = 0x00;
const uint8_t COMMON_SECURITY_TYPE = 0x00;

const uint8_t CM_VALIDATE_REQ_SIGNAL_TYPE = 0x00;
const uint8_t CM_VALIDATE_REQ_RESULT_READY = 0x01;
const uint8_t CM_VALIDATE_REQ_RESULT_FAILURE = 0x03;

const uint16_t CM_SLAC_MATCH_REQ_MVF_LENGTH = 0x3e;

const uint16_t CM_SLAC_MATCH_CNF_MVF_LENGTH = 0x56;

const uint8_t CM_SLAC_PARM_CNF_RESP_TYPE = 0x01; // = other GP station
const uint8_t CM_SLAC_PARM_CNF_NUM_SOUNDS = 10;  // typical value
const uint8_t CM_SLAC_PARM_CNF_TIMEOUT = 0x06;   // 600ms

const uint8_t CM_SET_KEY_REQ_KEY_TYPE_NMK = 0x01; // NMK (AES-128), Network Management Key
const uint8_t CM_SET_KEY_REQ_PID_HLE = 0x04;
const uint16_t CM_SET_KEY_REQ_PRN_UNUSED = 0x0000;
const uint8_t CM_SET_KEY_REQ_PMN_UNUSED = 0x00;
const uint8_t CM_SET_KEY_REQ_CCO_CAP_NONE = 0x00; // Level-0 CCo Capable, neither QoS nor TDMA
const uint8_t CM_SET_KEY_REQ_PEKS_NMK_KNOWN_TO_STA = 0x01;

const uint8_t CM_SET_KEY_CNF_RESULT_SUCCESS = 0x0;

const uint8_t CM_ATTEN_CHAR_RSP_RESULT = 0x00;

const uint8_t BROADCAST_MAC_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

} // namespace defs

namespace utils {
void generate_nmk_hs(uint8_t nmk_hs[slac::defs::NMK_LEN], const char* plain_password, int password_len);
void generate_nid_from_nmk(uint8_t nid[slac::defs::NID_LEN], const uint8_t nmk[slac::defs::NMK_LEN]);
} // namespace utils

namespace messages {

typedef struct {
    struct ether_header ethernet_header;
    struct {
        uint8_t mmv;     // management message version
        uint16_t mmtype; // management message type

    } __attribute__((packed)) homeplug_header;

    // the rest of this message is potentially payload data
    uint8_t payload[ETH_FRAME_LEN - ETH_HLEN - sizeof(homeplug_header)];
} __attribute__((packed)) homeplug_message;

typedef struct {
    uint8_t fmni; // fragmentation management number information
    uint8_t fmsn; // fragmentation message sequence number
} __attribute__((packed)) homeplug_fragmentation_part;

class HomeplugMessage {
public:
    homeplug_message* get_raw_message_ptr() {
        return &raw_msg;
    };

    int get_raw_msg_len() const {
        return raw_msg_len;
    }

    void setup_payload(void const* payload, int len, uint16_t mmtype, const defs::MMV mmv);
    void setup_ethernet_header(const uint8_t dst_mac_addr[ETH_ALEN], const uint8_t src_mac_addr[ETH_ALEN] = nullptr);

    uint16_t get_mmtype() const;
    uint8_t* get_src_mac();

    template <typename T> const T& get_payload() {
        if (raw_msg.homeplug_header.mmv == static_cast<std::underlying_type_t<defs::MMV>>(defs::MMV::AV_1_0)) {
            return *reinterpret_cast<T*>(raw_msg.payload);
        }

        // if not av 1.0 message, we need to shift by the fragmentation part
        return *reinterpret_cast<T*>(raw_msg.payload + sizeof(homeplug_fragmentation_part));
    }

    bool is_valid() const;
    bool keep_source_mac() const {
        return keep_src_mac;
    }

private:
    homeplug_message raw_msg;

    int raw_msg_len{-1};
    bool keep_src_mac{false};
};

const int M_SOUND_TARGET_LEN = 6;
const int SENDER_ID_LEN = defs::STATION_ID_LEN;
const int SOURCE_ID_LEN = defs::STATION_ID_LEN;
const int RESP_ID_LEN = defs::STATION_ID_LEN;
const int PEV_ID_LEN = defs::STATION_ID_LEN;
const int EVSE_ID_LEN = defs::STATION_ID_LEN;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    // cipher fields are missing, because we restrict to security_type = 0x00
} __attribute__((packed)) cm_slac_parm_req;

typedef struct {
    uint8_t m_sound_target[M_SOUND_TARGET_LEN]; // fixed to 0xFFFFFFFFFFFF
    uint8_t num_sounds;                         // number of expected m-sounds
    uint8_t timeout;                            // corresponds to TT_EVSE_match_MNBC, in units of 100ms
    uint8_t resp_type;                          // fixed to 0x01, indicating 'other gp station'
    uint8_t forwarding_sta[ETH_ALEN];           // ev host mac address
    uint8_t application_type;                   // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;                      // fixed to 0x00, indicating 'no security'
    uint8_t run_id[defs::RUN_ID_LEN];           // matching run identifier, corresponding to the request
    // cipher field is missing, because we restrict to security_type = 0x00
} __attribute__((packed)) cm_slac_parm_cnf;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint8_t num_sounds;               // number of expected m-sounds
    uint8_t timeout;                  // corresponds to TT_EVSE_match_MNBC
    uint8_t resp_type;                // fixed to 0x01, indicating 'other gp station'
    uint8_t forwarding_sta[ETH_ALEN]; // ev host mac address
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
} __attribute__((packed)) cm_start_atten_char_ind;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint8_t source_address[ETH_ALEN]; // mac address of EV host, which initiates matching
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    uint8_t source_id[SOURCE_ID_LEN]; // unique id of the station, that sent the m-sounds
    uint8_t resp_id[RESP_ID_LEN];     // unique id of the station, that is sending this message
    uint8_t num_sounds;               // number of sounds used for attenuation profile
    struct {
        uint8_t num_groups;              // number of OFDM carrier groups
        uint8_t aag[defs::AAG_LIST_LEN]; // AAG_1 .. AAG_N
    } __attribute__((packed)) attenuation_profile;
} __attribute__((packed)) cm_atten_char_ind;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint8_t source_address[ETH_ALEN]; // mac address of EV host, which initiates matching
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    uint8_t source_id[SOURCE_ID_LEN]; // unique id of the station, that sent the m-sounds
    uint8_t resp_id[RESP_ID_LEN];     // unique id of the station, that is sending this message
    uint8_t result;                   // fixed to 0x00, indicates successful SLAC process
} __attribute__((packed)) cm_atten_char_rsp;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint8_t sender_id[SENDER_ID_LEN]; // sender id, if application_type = 0x00, it should be the pev's vin code
    uint8_t remaining_sound_count;    // count of remaining sound messages
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    uint8_t _reserved[8]; // note: this is to pad the run_id, which is defined to be 16 bytes for this message
    uint8_t random[16];   // random value
} __attribute__((packed)) cm_mnbc_sound_ind;

// note: this message doesn't seem to part of hpgp, it is defined in ISO15118-3
typedef struct {
    uint8_t pev_mac[ETH_ALEN]; // mac address of the EV host
    uint8_t num_groups;        // number of OFDM carrier groups
    uint8_t _reserved;
    uint8_t aag[defs::AAG_LIST_LEN]; // list of average attenuation for each group
} __attribute__((packed)) cm_atten_profile_ind;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint16_t mvf_length;              // fixed to 0x3e = 62 bytes following
    uint8_t pev_id[PEV_ID_LEN];       // vin code of PEV
    uint8_t pev_mac[ETH_ALEN];        // mac address of the EV host
    uint8_t evse_id[EVSE_ID_LEN];     // EVSE id
    uint8_t evse_mac[ETH_ALEN];       // mac address of the EVSE
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    uint8_t _reserved[8]; // note: this is to pad the run_id, which is defined to be 16 bytes for this message
} __attribute__((packed)) cm_slac_match_req;

typedef struct {
    uint8_t application_type;         // fixed to 0x00, indicating 'pev-evse matching'
    uint8_t security_type;            // fixed to 0x00, indicating 'no security'
    uint16_t mvf_length;              // fixed to 0x56 = 86 bytes following
    uint8_t pev_id[PEV_ID_LEN];       // vin code of PEV
    uint8_t pev_mac[ETH_ALEN];        // mac address of the EV host
    uint8_t evse_id[EVSE_ID_LEN];     // EVSE id
    uint8_t evse_mac[ETH_ALEN];       // mac address of the EVSE
    uint8_t run_id[defs::RUN_ID_LEN]; // indentifier for a matching run
    uint8_t _rerserved[8];      // note: this is to pad the run_id, which is defined to be 16 bytes for this message
    uint8_t nid[defs::NID_LEN]; // network id derived from the nmk
    uint8_t _reserved2;         // note: this is to pad the nid, which is defined to be 8 bytes for this message
    uint8_t nmk[defs::NMK_LEN]; // private nmk of the EVSE
} __attribute__((packed)) cm_slac_match_cnf;

typedef struct {
    uint8_t signal_type; // fixed to 0x00: PEV S2 toggles on control pilot line
    uint8_t timer;       // in the first request response exchange: should be set to 0x00
                         // in the second request response exchange: 0x00 = 100ms, 0x01 = 200ms TT_EVSE_vald_toggle
    uint8_t result;      // in the first request response exchange: should be set to 0x01 = ready
                         // in the second request response exchange: should be set to 0x01 = ready
} __attribute__((packed)) cm_validate_req;

typedef struct {
    uint8_t signal_type; // fixed to 0x00: PEV S2 toggles on control pilot line
    uint8_t toggle_num;  // in the first request response exchange: should be set to 0x00
                         // in the second request response exchange: number of detected BC
                         // edges during TT_EVSE_vald_toggle
    uint8_t result;      // 0x00 = not ready, 0x01 = ready, 0x02 = success, 0x03 = failure, 0x04 = not required
} __attribute__((packed)) cm_validate_cnf;

typedef struct {
    uint8_t key_type;               // fixed to 0x01, indicating NMK
    uint32_t my_nonce;              // fixed to 0x00000000: encrypted payload not used
    uint32_t your_nonce;            // fixed to 0x00000000: encrypted payload not used
    uint8_t pid;                    // fixed to 0x04: HLE protocol
    uint16_t prn;                   // fixed to 0x0000: encrypted payload not used
    uint8_t pmn;                    // fixed to 0x00: encrypted payload not used
    uint8_t cco_capability;         // CCo capability according to the station role
    uint8_t nid[defs::NID_LEN];     // 54 LSBs = NID, 2 MSBs = 0b00
    uint8_t new_eks;                // fixed to 0x01: NMK
    uint8_t new_key[defs::NMK_LEN]; // new NMK
} __attribute__((packed)) cm_set_key_req;

typedef struct {
    uint8_t result; // 0x00 = success, 0x01 = failure, 0x02 - 0xFF = reserved
    uint32_t my_nonce;
    uint32_t your_nonce;
    uint8_t pid;
    uint16_t prn;
    uint8_t pmn;
    uint8_t cco_capability;
} __attribute__((packed)) cm_set_key_cnf;

namespace qualcomm {

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0xb0, 0x52}; // Qualcomm Vendor MME code
} __attribute__((packed)) cm_reset_device_req;

typedef struct {
    uint8_t vendor_mme[3]; // Vendor MME code
    uint8_t success;
} __attribute__((packed)) cm_reset_device_cnf;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0xb0, 0x52}; // Qualcomm Vendor MME code
} __attribute__((packed)) link_status_req;

typedef struct {
    uint8_t vendor_mme[3]; // Vendor MME code
    uint8_t reserved;
    uint8_t link_status;
} __attribute__((packed)) link_status_cnf;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0xb0, 0x52}; // Qualcomm Vendor MME code
    uint32_t cookie{0x12345};                   // some cookie we will also get in the reply
    uint8_t report_type{0};                     // binary report
} __attribute__((packed)) op_attr_req;

typedef struct {
    uint8_t vendor_mme[3]; // Vendor MME code
    uint16_t success;      // 0x00 means success
    uint32_t cookie;
    uint8_t report_type; // should be 0x00 (binary)
    uint16_t size;       // should be 118, otherwise we do not know the structure
    uint8_t hw_platform[16];
    uint8_t sw_platform[16];
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_pib;
    uint32_t version_build;
    uint32_t reserved;
    uint8_t build_date[8];
    uint8_t release_type[12];
    uint8_t sdram_type;
    uint8_t reserved2;
    uint8_t line_freq_zc;
    uint32_t sdram_size;
    uint8_t authorization_mode;
} __attribute__((packed)) op_attr_cnf;

} // namespace qualcomm

namespace lumissil {

typedef struct {
    uint16_t version;
    uint32_t reserved;
    uint8_t request_id;
    uint8_t reserved2[12];
} __attribute__((packed)) lms_header;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0x16, 0xE8}; // Lumissil Vendor MME code
    lms_header lms;
    uint8_t mode{0}; // Normal reset
} __attribute__((packed)) nscm_reset_device_req;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0x16, 0xE8}; // Lumissil Vendor MME code
    lms_header lms;
} __attribute__((packed)) nscm_get_version_req;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0x16, 0xE8}; // Lumissil Vendor MME code
    lms_header lms;
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t version_patch;
    uint16_t version_build;
    uint16_t reserved;
} __attribute__((packed)) nscm_get_version_cnf;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0x16, 0xE8}; // Lumissil Vendor MME code
    lms_header lms;
} __attribute__((packed)) nscm_get_d_link_status_req;

typedef struct {
    uint8_t vendor_mme[3] = {0x00, 0x16, 0xE8}; // Lumissil Vendor MME code
    lms_header lms;
    uint8_t link_status;
} __attribute__((packed)) nscm_get_d_link_status_cnf;

// There is no CNF for this reset packet

} // namespace lumissil

} // namespace messages
} // namespace slac

#endif // SLAC_SLAC_HPP
