// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EV_SLAC_CONTEXT_HPP
#define EV_SLAC_CONTEXT_HPP

#include <functional>
#include <string>

#include <slac/slac.hpp>

namespace slac::fsm::ev {

namespace _context_detail {
template <typename SlacMessageType> struct MMTYPE;

template <> struct MMTYPE<slac::messages::cm_slac_parm_req> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::cm_start_atten_char_ind> {
    static const uint16_t value = slac::defs::MMTYPE_CM_START_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND;
};

template <> struct MMTYPE<slac::messages::cm_mnbc_sound_ind> {
    static const uint16_t value = slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND;
};

template <> struct MMTYPE<slac::messages::cm_atten_char_rsp> {
    static const uint16_t value = slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_RSP;
};

template <> struct MMTYPE<slac::messages::cm_slac_match_req> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_REQ;
};

template <> struct MMTYPE<slac::messages::cm_set_key_req> {
    static const uint16_t value = slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ;
};

template <typename SlacMessageType> struct MMV {
    // this is the default value for homeplug av 2.0 messages, which are
    // backward compatible with homeplug av 1.1 messages
    // non-backward (to 1.1) compatible message are CM_CHAN_EST,
    // CM_AMP_MAP and CM_NW_STATS, these need to use AV_2_0
    // older av 1.0 message need to use AV_1_0
    static constexpr auto value = slac::defs::MMV::AV_1_1;
};
} // namespace _context_detail

struct ContextCallbacks {
    std::function<void(slac::messages::HomeplugMessage&)> send_raw_slac{nullptr};
    std::function<void(const std::string&)> signal_state{nullptr};
    std::function<void(const std::string&)> log{nullptr};
};

struct Context {
    explicit Context(const ContextCallbacks& callbacks_) : callbacks(callbacks_){};

    // MAC address of our PLC modem (EV side)
    uint8_t plc_mac[ETH_ALEN] = {0x00, 0xB0, 0x52, 0x00, 0x00, 0x01};

    // event specific payloads
    // FIXME (aw): due to the synchroneous nature of the fsm, this could be even a ptr/ref
    slac::messages::HomeplugMessage slac_message;

    // FIXME (aw): message should be const, but libslac doesn't allow for const ptr - needs changes in libslac
    template <typename SlacMessageType>
    void send_slac_message(const uint8_t* dest_mac, SlacMessageType const& message) {
        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(dest_mac);
        hp_message.setup_payload(&message, sizeof(message), _context_detail::MMTYPE<SlacMessageType>::value,
                                 _context_detail::MMV<SlacMessageType>::value);
        callbacks.send_raw_slac(hp_message);
    }

    // signal handlers
    void signal_state(const std::string& state);

    // logging util
    void log_info(const std::string& text);

private:
    const ContextCallbacks& callbacks;
};

struct SessionParamaters {
    SessionParamaters(const uint8_t* run_id, const uint8_t* evse_mac);
    uint8_t run_id[slac::defs::RUN_ID_LEN];
    uint8_t evse_mac[ETH_ALEN];
};

} // namespace slac::fsm::ev

#endif // EV_SLAC_CONTEXT_HPP
