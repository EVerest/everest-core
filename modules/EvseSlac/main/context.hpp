// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_CONTEXT_HPP
#define EVSE_SLAC_CONTEXT_HPP

#include <functional>
#include <string>

#include <slac/slac.hpp>

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
} // namespace _context_detail

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
    template <typename SlacMessageType> void send_slac_message(const uint8_t* mac, SlacMessageType& message) {
        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac);
        hp_message.setup_payload(&message, sizeof(message), _context_detail::MMTYPE<SlacMessageType>::value);
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

private:
    const ContextCallbacks& callbacks;
};

#endif // EVSE_SLAC_CONTEXT_HPP
