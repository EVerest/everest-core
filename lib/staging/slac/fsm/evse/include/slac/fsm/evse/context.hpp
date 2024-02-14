// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_CONTEXT_HPP
#define EVSE_SLAC_CONTEXT_HPP

#include <functional>
#include <string>

#include <slac/slac.hpp>

namespace slac::fsm::evse {

namespace _context_detail {

template <typename SlacMessageType> struct MMTYPE;
template <> struct MMTYPE<slac::messages::cm_slac_parm_cnf> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_CNF;
};
template <> struct MMTYPE<slac::messages::cm_atten_char_ind> {
    static const uint16_t value = slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND;
};
template <> struct MMTYPE<slac::messages::cm_set_key_req> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ;
};
template <> struct MMTYPE<slac::messages::cm_validate_cnf> {
    static const uint16_t value = slac::defs::MMTYPE_CM_VALIDATE | slac::defs::MMTYPE_MODE_CNF;
};
template <> struct MMTYPE<slac::messages::cm_slac_match_cnf> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_CNF;
};

template <> struct MMTYPE<slac::messages::qualcomm::cm_reset_device_req> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::qualcomm::cm_reset_device_cnf> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_CNF;
};

template <> struct MMTYPE<slac::messages::qualcomm::link_status_req> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_LINK_STATUS | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::qualcomm::link_status_cnf> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_LINK_STATUS | slac::defs::MMTYPE_MODE_CNF;
};

template <> struct MMTYPE<slac::messages::qualcomm::op_attr_req> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_OP_ATTR | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::qualcomm::op_attr_cnf> {
    static const uint16_t value = slac::defs::qualcomm::MMTYPE_OP_ATTR | slac::defs::MMTYPE_MODE_CNF;
};

// This message has no CNF counterpart
template <> struct MMTYPE<slac::messages::lumissil::nscm_reset_device_req> {
    static const uint16_t value = slac::defs::lumissil::MMTYPE_NSCM_RESET_DEVICE | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::lumissil::nscm_get_version_req> {
    static const uint16_t value = slac::defs::lumissil::MMTYPE_NSCM_GET_VERSION | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::lumissil::nscm_get_version_cnf> {
    static const uint16_t value = slac::defs::lumissil::MMTYPE_NSCM_GET_VERSION | slac::defs::MMTYPE_MODE_CNF;
};

template <> struct MMTYPE<slac::messages::lumissil::nscm_get_d_link_status_req> {
    static const uint16_t value = slac::defs::lumissil::MMTYPE_NSCM_GET_D_LINK_STATUS | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::lumissil::nscm_get_d_link_status_cnf> {
    static const uint16_t value = slac::defs::lumissil::MMTYPE_NSCM_GET_D_LINK_STATUS | slac::defs::MMTYPE_MODE_CNF;
};

template <typename SlacMessageType> struct MMV {
    // this is the default value for homeplug av 2.0 messages, which are
    // backward compatible with homeplug av 1.1 messages
    // non-backward (to 1.1) compatible message are CM_CHAN_EST,
    // CM_AMP_MAP and CM_NW_STATS, these need to use AV_2_0
    // older av 1.0 message need to use AV_1_0
    static constexpr auto value = slac::defs::MMV::AV_1_1;
};

template <> struct MMV<slac::messages::qualcomm::cm_reset_device_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::qualcomm::cm_reset_device_cnf> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::qualcomm::link_status_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::qualcomm::link_status_cnf> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::qualcomm::op_attr_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::qualcomm::op_attr_cnf> {
    static constexpr auto value = slac::defs::MMV::AV_1_0;
};

template <> struct MMV<slac::messages::lumissil::nscm_reset_device_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0; // FIXME this is unclear
};

template <> struct MMV<slac::messages::lumissil::nscm_get_version_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0; // FIXME this is unclear
};

template <> struct MMV<slac::messages::lumissil::nscm_get_version_cnf> {
    static constexpr auto value = slac::defs::MMV::AV_1_0; // FIXME this is unclear
};

template <> struct MMV<slac::messages::lumissil::nscm_get_d_link_status_req> {
    static constexpr auto value = slac::defs::MMV::AV_1_0; // FIXME this is unclear
};

template <> struct MMV<slac::messages::lumissil::nscm_get_d_link_status_cnf> {
    static constexpr auto value = slac::defs::MMV::AV_1_0; // FIXME this is unclear
};

} // namespace _context_detail

// FIXME (aw): this should be moved to common headers (in libslac)
enum class ModemVendor {
    Unknown,
    Qualcomm,
    Lumissil,
    VertexCom,
};

struct ContextCallbacks {
    std::function<void(slac::messages::HomeplugMessage&)> send_raw_slac{nullptr};
    std::function<void(const std::string&)> signal_state{nullptr};
    std::function<void(bool)> signal_dlink_ready{nullptr};
    std::function<void()> signal_error_routine_request{nullptr};
    std::function<void(const std::string&)> signal_ev_mac_address_parm_req{nullptr};
    std::function<void(const std::string&)> signal_ev_mac_address_match_cnf{nullptr};
    std::function<void(const std::string&)> log{nullptr};
};

struct EvseSlacConfig {
    // MAC address of our (EVSE) PLC modem
    // FIXME (aw): is that used somehow?
    uint8_t plc_peer_mac[ETH_ALEN] = {0x00, 0xB0, 0x52, 0x00, 0x00, 0x01};

    // FIXME (aw): we probably want to use std::array here
    void generate_nmk();
    uint8_t session_nmk[slac::defs::NMK_LEN]{};

    // flag for using 5% PWM in AC mode
    bool ac_mode_five_percent{true};

    // timeout for CM_SET_KEY.REQ
    int set_key_timeout_ms = 500;

    // Settings CM_DEVICE_RESET.REQ
    struct chip_reset_struct {
        bool enabled = false;
        int timeout_ms = 500;
        int delay_ms = 100;
    } chip_reset;

    // Settings for LINK_STATUS detection
    struct link_status_struct {
        bool do_detect = false;
        int retry_ms = 100;
        int timeout_ms = 5000;
        bool debug_simulate_failed_matching = false;
    } link_status;

    int request_info_delay_ms = 100;

    // offset for adjusting the calculated sounding attenuation
    int sounding_atten_adjustment = 0;
};

struct Context {
    explicit Context(const ContextCallbacks& callbacks_) : callbacks(callbacks_){};

    EvseSlacConfig slac_config{};

    // event specific payloads
    // FIXME (aw): due to the synchroneous nature of the fsm, this could be even a ptr/ref
    slac::messages::HomeplugMessage slac_message_payload;

    // FIXME (aw): message should be const, but libslac doesn't allow for const ptr - needs changes in libslac
    template <typename SlacMessageType> void send_slac_message(const uint8_t* mac, SlacMessageType const& message) {
        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac);
        hp_message.setup_payload(&message, sizeof(message), _context_detail::MMTYPE<SlacMessageType>::value,
                                 _context_detail::MMV<SlacMessageType>::value);
        callbacks.send_raw_slac(hp_message);
    }

    // signal handlers
    void signal_cm_slac_parm_req(const uint8_t* ev_mac);
    void signal_cm_slac_match_cnf(const uint8_t* ev_mac);
    void signal_dlink_ready(bool value);
    void signal_error_routine_request();
    void signal_state(const std::string& state);

    // logging util
    void log_info(const std::string& text);

    ModemVendor modem_vendor{ModemVendor::Unknown};

private:
    const ContextCallbacks& callbacks;
};

} // namespace slac::fsm::evse

#endif // EVSE_SLAC_CONTEXT_HPP
