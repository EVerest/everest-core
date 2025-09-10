// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <openssl/hmac.h>

#include <cstring>
#include <goose/frame.hpp>

using namespace goose::frame;

GooseTimestamp::GooseTimestamp(const std::vector<uint8_t>& raw) {
    if (raw.size() != 8) {
        throw std::runtime_error("GooseTimestamp: raw data is not 8 bytes");
    }

    seconds = 0;
    for (size_t i = 0; i < 4; i++) {
        seconds = (seconds << 8) | raw[i];
    }

    fraction = 0;
    for (size_t i = 4; i < 7; i++) {
        fraction = (fraction << 8) | raw[i];
    }

    quality_of_time = raw[7];
}

std::vector<uint8_t> GooseTimestamp::encode() const {
    std::vector<uint8_t> result;
    auto seconds_be = ber::encode_be(seconds);
    auto fraction_be = ber::encode_be(fraction);
    result.insert(result.end(), seconds_be.begin(), seconds_be.end());
    result.insert(result.end(), fraction_be.begin() + 1, fraction_be.end());
    result.push_back(quality_of_time);
    return result;
}
float GooseTimestamp::to_ms() {
    return (uint64_t)seconds * 1000 + ((uint64_t)fraction * 1000) / 0x1000000;
}

GooseTimestamp GooseTimestamp::from_ms(uint64_t ms) {
    uint64_t ms_part = ms % 1000;
    uint64_t sec_part = ms / 1000;
    auto fraction = (ms_part * 0x1000000) / 1000;

    // quality is 10 as milliseconds are used for the fraction, which
    // corresponds to about 10 bits
    return GooseTimestamp(sec_part, fraction, 10);
}

GooseTimestamp GooseTimestamp::now() {
    auto now = std::chrono::system_clock().now().time_since_epoch();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    return from_ms(now_ms);
}

bool GooseTimestamp::operator==(const GooseTimestamp& other) const {
    return seconds == other.seconds && fraction == other.fraction && quality_of_time == other.quality_of_time;
}

GoosePDU::GoosePDU(const std::vector<uint8_t>& pdu) {
    auto pdu_c = pdu;
    goose::frame::ber::BEREntry root(&pdu_c);
    if (root.tag != 0x61) {
        throw std::runtime_error("GoosePDU: root tag is not 0x61");
    }

    if (pdu_c.size() > 0) {
        throw std::runtime_error("GoosePDU: received extra data, that is not part of BER encoded "
                                 "region");
    }

    auto root_value = root.value;

    // go_cb_ref
    goose::frame::ber::BEREntry go_cb_ref_entry(&root_value);
    if (go_cb_ref_entry.tag != 0x80) {
        throw std::runtime_error("GoosePDU: go_cb_ref tag is not 0x80");
    }
    if (go_cb_ref_entry.value.size() > 65) { // todo: check length
        throw std::runtime_error("GoosePDU: go_cb_ref is too long");
    }
    memcpy(go_cb_ref, go_cb_ref_entry.value.data(), go_cb_ref_entry.value.size());

    // time_allowed_to_live
    ber::PrimitiveBEREntry<uint32_t> time_allowed_to_live_entry(root_value, 0x81);
    time_allowed_to_live = time_allowed_to_live_entry.data;

    // dat_set
    goose::frame::ber::BEREntry dat_set_entry(&root_value);
    if (dat_set_entry.tag != 0x82) {
        throw std::runtime_error("GoosePDU: dat_set tag is not 0x82");
    }
    if (dat_set_entry.value.size() > 65) { // todo: check length
        throw std::runtime_error("GoosePDU: dat_set is too long");
    }
    memcpy(dat_set, dat_set_entry.value.data(), dat_set_entry.value.size());

    // go_id
    goose::frame::ber::BEREntry go_id_entry(&root_value);
    if (go_id_entry.tag != 0x83) {
        throw std::runtime_error("GoosePDU: go_id tag is not 0x83");
    }
    if (go_id_entry.value.size() > 65) { // todo: check length
        throw std::runtime_error("GoosePDU: go_id is too long");
    }
    memcpy(go_id, go_id_entry.value.data(), go_id_entry.value.size());

    // timestamp
    goose::frame::ber::BEREntry timestamp_entry(&root_value);
    if (timestamp_entry.tag != 0x84) {
        throw std::runtime_error("GoosePDU: timestamp tag is not 0x84");
    }
    if (timestamp_entry.value.size() != 8) {
        throw std::runtime_error("GoosePDU: timestamp is not 8 bytes");
    }
    timestamp = GooseTimestamp(timestamp_entry.value);

    // st_num
    ber::PrimitiveBEREntry<uint32_t> st_num_entry(root_value, 0x85);
    st_num = st_num_entry.data;

    // sq_num
    ber::PrimitiveBEREntry<uint32_t> sq_num_entry(root_value, 0x86);
    sq_num = sq_num_entry.data;

    // simulation
    ber::PrimitiveBEREntry<uint8_t> simulation_entry(root_value, 0x87);
    simulation = simulation_entry.data;

    // conf_rev
    ber::PrimitiveBEREntry<uint32_t> conf_rev_entry(root_value, 0x88);
    conf_rev = conf_rev_entry.data;

    // ndsCom
    ber::PrimitiveBEREntry<uint8_t> ndsCom_entry(root_value, 0x89);
    ndsCom = ndsCom_entry.data;

    // apdu count
    ber::PrimitiveBEREntry<uint32_t> apdu_count_entry(root_value, 0x8A);
    auto apdu_entry_count = apdu_count_entry.data;

    // apdu sequence
    goose::frame::ber::BEREntry apdu_entry(&root_value);
    if (apdu_entry.tag != 0xAB) {
        throw std::runtime_error("GoosePDU: apdu tag is not 0xAB");
    }
    auto apdu = apdu_entry.value;

    // check that no more data is left in root node
    if (root_value.size() > 0) {
        throw std::runtime_error("GoosePDU: frame has extra data");
    }

    // apdu entries
    for (size_t i = 0; i < apdu_entry_count; i++) {
        apdu_entries.emplace_back(&apdu);
    }

    // check that no more data is left in apdu sequence node
    if (apdu.size() > 0) {
        throw std::runtime_error("GoosePDU: apdu has extra data");
    }
}

std::vector<uint8_t> GoosePDU::serialize() const {
    goose::frame::ber::BEREntry root;
    root.tag = 0x61;
    root.value = std::vector<uint8_t>();

    goose::frame::ber::BEREntry go_cb_ref_entry;
    go_cb_ref_entry.tag = 0x80;
    go_cb_ref_entry.value =
        std::vector<uint8_t>(go_cb_ref, go_cb_ref + strlen(go_cb_ref) + 1); // null terminated string
    root.add(go_cb_ref_entry);

    goose::frame::ber::BEREntry time_allowed_to_live_entry;
    time_allowed_to_live_entry.tag = 0x81;
    time_allowed_to_live_entry.value = ber::encode_be((uint32_t)time_allowed_to_live);
    root.add(time_allowed_to_live_entry);

    goose::frame::ber::BEREntry dat_set_entry;
    dat_set_entry.tag = 0x82;
    dat_set_entry.value = std::vector<uint8_t>(dat_set, dat_set + strlen(dat_set) + 1); // null terminated string
    root.add(dat_set_entry);

    goose::frame::ber::BEREntry go_id_entry;
    go_id_entry.tag = 0x83;
    go_id_entry.value = std::vector<uint8_t>(go_id, go_id + strlen(go_id) + 1); // null
                                                                                // terminated
                                                                                // string
    root.add(go_id_entry);

    goose::frame::ber::BEREntry timestamp_entry;
    timestamp_entry.tag = 0x84;
    timestamp_entry.value = timestamp.encode();
    root.add(timestamp_entry);

    goose::frame::ber::BEREntry st_num_entry;
    st_num_entry.tag = 0x85;
    st_num_entry.value = ber::encode_be((uint32_t)st_num);
    root.add(st_num_entry);

    goose::frame::ber::BEREntry sq_num_entry;
    sq_num_entry.tag = 0x86;
    sq_num_entry.value = ber::encode_be((uint32_t)sq_num);
    root.add(sq_num_entry);

    goose::frame::ber::BEREntry simulation_entry;
    simulation_entry.tag = 0x87;
    simulation_entry.value = std::vector<uint8_t>{simulation};
    root.add(simulation_entry);

    goose::frame::ber::BEREntry conf_rev_entry;
    conf_rev_entry.tag = 0x88;
    conf_rev_entry.value = ber::encode_be((uint32_t)conf_rev);
    root.add(conf_rev_entry);

    ber::BEREntry nds_com_entry;
    nds_com_entry.tag = 0x89;
    nds_com_entry.value = std::vector<uint8_t>{0}; // todo
    root.add(nds_com_entry);

    ber::BEREntry apdu_count_entry;
    apdu_count_entry.tag = 0x8A;
    apdu_count_entry.value = ber::encode_be((uint32_t)apdu_entries.size());
    root.add(apdu_count_entry);

    ber::BEREntry apdu_entry;
    apdu_entry.tag = 0xAB;
    apdu_entry.value = std::vector<uint8_t>();
    for (const auto& entry : apdu_entries) {
        apdu_entry.add(entry);
    }
    root.add(apdu_entry);

    return root.encode();
}

GooseFrameIntf::GooseFrameIntf(const goose_ethernet::EthernetFrame& ethernet_frame) {
    memcpy(source_mac_address, ethernet_frame.source, 6);
    memcpy(destination_mac_address, ethernet_frame.destination, 6);

    if (ethernet_frame.ethertype != GOOSE_ETHERTYPE) {
        throw std::runtime_error("GooseFrame: not a GOOSE frame");
    }

    if (!ethernet_frame.eth_802_1q_tag.has_value()) {
        throw std::runtime_error("GooseFrame: no 802.1Q tag");
    }

    auto tag_802_1q = ethernet_frame.eth_802_1q_tag.value();
    this->priority = (tag_802_1q & 0xE000) >> 13;
    this->vlan_id = tag_802_1q & 0x0FFF;

    // appid
    if (ethernet_frame.payload.size() < 2) {
        throw std::runtime_error("GooseFrame: no appid");
    }
    appid[0] = ethernet_frame.payload[0];
    appid[1] = ethernet_frame.payload[1];

    uint16_t length = (ethernet_frame.payload[2] << 8) | ethernet_frame.payload[3];

    uint16_t reserve1 = (ethernet_frame.payload[4] << 8) | ethernet_frame.payload[5];
    uint16_t reserve2 = (ethernet_frame.payload[6] << 8) | ethernet_frame.payload[7];

    // goose pdu
    goose::frame::GoosePDU pdu(
        std::vector<uint8_t>(ethernet_frame.payload.data() + 8, ethernet_frame.payload.data() + length));
    this->pdu = pdu;
};

GooseFrame::GooseFrame(const goose_ethernet::EthernetFrame& ethernet_frame) : GooseFrameIntf(ethernet_frame) {
    uint16_t reserve1 = (ethernet_frame.payload[4] << 8) | ethernet_frame.payload[5];
    uint16_t reserve2 = (ethernet_frame.payload[6] << 8) | ethernet_frame.payload[7];

    if (reserve1 != 0) {
        throw std::runtime_error("GooseFrame: reserve1 byte 2 is not 0");
    }

    if (reserve2 != 0) {
        throw std::runtime_error("GooseFrame: reserve2 byte 2 is not 0");
    }

    if (ethernet_frame.payload.size() != 8 + pdu.serialize().size()) {
        throw std::runtime_error("GooseFrame: payload size does not match");
    }
}

goose_ethernet::EthernetFrame GooseFrame::serialize() const {
    goose_ethernet::EthernetFrame ethernet_frame;
    memcpy(ethernet_frame.source, source_mac_address, 6);
    memcpy(ethernet_frame.destination, destination_mac_address, 6);
    ethernet_frame.ethertype = GOOSE_ETHERTYPE;
    ethernet_frame.eth_802_1q_tag = (priority << 13) | vlan_id;

    auto pdu_data = pdu.serialize();
    ethernet_frame.payload.clear();
    ethernet_frame.payload.push_back(appid[0]);
    ethernet_frame.payload.push_back(appid[1]);
    // length
    auto length = pdu_data.size() + 8;
    ethernet_frame.payload.push_back(length >> 8);
    ethernet_frame.payload.push_back(length & 0xFF);
    // reserve1
    ethernet_frame.payload.push_back(0);
    ethernet_frame.payload.push_back(0);
    // reserve2
    ethernet_frame.payload.push_back(0);
    ethernet_frame.payload.push_back(0);
    // pdu
    ethernet_frame.payload.insert(ethernet_frame.payload.end(), pdu_data.begin(), pdu_data.end());

    return ethernet_frame;
}

uint16_t crc(std::vector<uint8_t> data) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < data.size(); i++) {
        crc ^= data[i];
        for (size_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

SecureGooseFrame::SecureGooseFrame(const goose_ethernet::EthernetFrame& ethernet_frame,
                                   std::optional<std::vector<uint8_t>> hmac_key) :
    GooseFrameIntf(ethernet_frame) {
    uint16_t length = (ethernet_frame.payload[2] << 8) | ethernet_frame.payload[3];

    uint16_t reserve1 = (ethernet_frame.payload[4] << 8) | ethernet_frame.payload[5];
    uint16_t reserve2 = (ethernet_frame.payload[6] << 8) | ethernet_frame.payload[7];

    uint16_t extended_length = reserve1 & 0x00FF;
    if (extended_length == 0) {
        throw std::runtime_error("GooseFrame: reserve1 byte 2 is 0, thus not a secure frame");
    }
    if (extended_length < 32) {
        throw std::runtime_error("GooseFrame: reserve1 byte 2 is less than 32, thus no hmac 256 fits");
    }

    std::vector<uint8_t> reserve2_crc_data;
    reserve2_crc_data.push_back(ethernet_frame.ethertype >> 8);
    reserve2_crc_data.push_back(ethernet_frame.ethertype & 0xFF);
    reserve2_crc_data.push_back(ethernet_frame.payload[0]);
    reserve2_crc_data.push_back(ethernet_frame.payload[1]);
    reserve2_crc_data.push_back(ethernet_frame.payload[2]);
    reserve2_crc_data.push_back(ethernet_frame.payload[3]);
    reserve2_crc_data.push_back(ethernet_frame.payload[4]);
    reserve2_crc_data.push_back(ethernet_frame.payload[5]);

    uint16_t crc_value = crc(reserve2_crc_data);
    if (crc_value != reserve2) {
        throw std::runtime_error("GooseFrame: crc value does not match");
    }

    if (hmac_key.has_value()) {
        // verify hmac
        std::vector<uint8_t> hmac = std::vector<uint8_t>(ethernet_frame.payload.data() + length + extended_length - 32,
                                                         ethernet_frame.payload.data() + length + extended_length);

        // todo: unknown if this is right
        std::vector<uint8_t> hmac_data;
        hmac_data.push_back(ethernet_frame.ethertype >> 8);
        hmac_data.push_back(ethernet_frame.ethertype & 0xFF);
        hmac_data.insert(hmac_data.end(), ethernet_frame.payload.begin(),
                         ethernet_frame.payload.begin() + length + extended_length - 35);

        std::vector<uint8_t> calculated_hmac(32);
        uint32_t calculated_hmac_len = calculated_hmac.size();
        auto ret = HMAC(EVP_sha256(), hmac_key.value().data(), hmac_key.value().size(), hmac_data.data(),
                        hmac_data.size(), calculated_hmac.data(), &calculated_hmac_len);

        if (ret == NULL) {
            throw std::runtime_error("SecureGooseFrame: HMAC failed");
        }

        if (calculated_hmac != hmac) {
            throw std::runtime_error("SecureGooseFrame: HMAC does not match");
        }
    }
}

goose_ethernet::EthernetFrame SecureGooseFrame::serialize(std::vector<uint8_t> hmac_key) const {
    auto pdu_data = pdu.serialize();

    uint16_t length_in_header = pdu_data.size() + 8; // appid + length + reserve1 + reserve2

    // Ethernet frame without mac's and
    // 802.1Q but with ethertype (which is removed later,
    // before putting it into the EthernetFrame)
    std::vector<uint8_t> ethernet_payload;

    // ethertype
    ethernet_payload.push_back(0x88);
    ethernet_payload.push_back(0xB8);
    // appid
    ethernet_payload.push_back(appid[0]);
    ethernet_payload.push_back(appid[1]);
    // length
    ethernet_payload.push_back(length_in_header >> 8);
    ethernet_payload.push_back(length_in_header & 0xFF);
    // reserve1
    ethernet_payload.push_back(0);
    ethernet_payload.push_back(0x23); // 32 bytes hmac + 3 bytes TLV
    // reserve2
    uint16_t crc_val = crc(ethernet_payload);
    ethernet_payload.push_back(crc_val >> 8);
    ethernet_payload.push_back(crc_val & 0xFF);
    // pdu
    ethernet_payload.insert(ethernet_payload.end(), pdu_data.begin(), pdu_data.end());

    // Calulate HMAC over the whole frame
    std::vector<uint8_t> hmac(32);

    HMAC(EVP_sha256(), hmac_key.data(), hmac_key.size(), ethernet_payload.data(), ethernet_payload.size(), hmac.data(),
         NULL);

    // append hmac data to the frame
    ethernet_payload.push_back(0xad);
    ethernet_payload.push_back(0x00);
    ethernet_payload.push_back(0x20);
    ethernet_payload.insert(ethernet_payload.end(), hmac.begin(), hmac.end());

    // remove ethertype
    ethernet_payload.erase(ethernet_payload.begin(), ethernet_payload.begin() + 2);

    // populate EthernetFrame struct
    goose_ethernet::EthernetFrame ethernet_frame;
    memcpy(ethernet_frame.source, source_mac_address, 6);
    memcpy(ethernet_frame.destination, destination_mac_address, 6);
    ethernet_frame.ethertype = GOOSE_ETHERTYPE;
    ethernet_frame.eth_802_1q_tag = (priority << 13) | vlan_id;
    ethernet_frame.payload = ethernet_payload;

    return ethernet_frame;
}
