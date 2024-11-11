// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef REMOTE_CHARGING_SLAC_HELPER_HPP
#define REMOTE_CHARGING_SLAC_HELPER_HPP

#include <arpa/inet.h>
#include <functional>
#include <net/ethernet.h>
#include <string>

#include <everest/logging.hpp>
#include <slac/slac.hpp>

namespace slac_helper {

uint8_t plc_mac[ETH_ALEN] = {0x00, 0xB0, 0x52, 0x00, 0x00, 0x01};

template <typename SlacMessageType> struct MMTYPE;

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

bool is_homeplug_protocol(std::string packet) {
    uint16_t type = reinterpret_cast<struct ether_header*>(packet.data())->ether_type;
    return htons(type) == 0x88E1;
}

slac::messages::HomeplugMessage to_homeplug_msg(const std::string& packet) {
    // create home plug message
    slac::messages::HomeplugMessage msg;
    int size = packet.size();

    if (size > sizeof(slac::messages::homeplug_message)) {
        size = sizeof(slac::messages::homeplug_message);
    }

    memcpy(msg.get_raw_message_ptr(), packet.data(), size);

    return msg;
}

bool is_cm_atten_profile_ind(const slac::messages::HomeplugMessage& msg) {
    return msg.get_mmtype() == (slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND);
}

bool is_cm_slac_match_cnf(const slac::messages::HomeplugMessage& msg) {
    return msg.get_mmtype() == (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_CNF);
}

template <typename SlacMessageType>
std::string create_slac_message(const uint8_t* dest_mac, SlacMessageType const& message) {
    slac::messages::HomeplugMessage hp_message;

    // FIXME this needs to be the actual MAC adress
    uint8_t src_mac[ETH_ALEN] = {0x2A, 0x8F, 0x03, 0xC4, 0x2E, 0xA2};

    hp_message.setup_ethernet_header(dest_mac, src_mac);
    hp_message.setup_payload(&message, sizeof(message), MMTYPE<SlacMessageType>::value, MMV<SlacMessageType>::value);

    return std::string(reinterpret_cast<char*>(hp_message.get_raw_message_ptr()), hp_message.get_raw_msg_len());
    ;
}

std::string set_nmk_key_from_match_cnf(slac::messages::HomeplugMessage& msg_in) {
    auto nmk = msg_in.get_payload<slac::messages::cm_slac_match_cnf>().nmk;
    slac::messages::cm_set_key_req msg;
    msg.key_type = slac::defs::CM_SET_KEY_REQ_KEY_TYPE_NMK;
    msg.my_nonce = 0xAAAAAAAA;
    msg.your_nonce = 0x00000000;
    msg.pid = slac::defs::CM_SET_KEY_REQ_PID_HLE;
    msg.prn = htole16(slac::defs::CM_SET_KEY_REQ_PRN_UNUSED);
    msg.pmn = slac::defs::CM_SET_KEY_REQ_PMN_UNUSED;
    msg.cco_capability = slac::defs::CM_SET_KEY_REQ_CCO_CAP_NONE;
    slac::utils::generate_nid_from_nmk(msg.nid, nmk);
    msg.new_eks = slac::defs::CM_SET_KEY_REQ_PEKS_NMK_KNOWN_TO_STA;
    memcpy(msg.new_key, nmk, sizeof(msg.new_key));

    return create_slac_message(plc_mac, msg);
}

} // namespace slac_helper

#endif
