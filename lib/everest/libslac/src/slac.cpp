// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include <slac/slac.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>

#include <arpa/inet.h>
#include <endian.h>

#include <hash_library/sha256.h>

namespace slac {
namespace utils {

// note on byte order:
//   - sha256 takes the most significant byte first from the lowest
//     memory address
//   - for the generation of the aes-128, or NMK-HS, the first octet of
//     the sha256 output is taken as the zero octet for the NMK-HS
//   - for the generation of NID, the NMK is fed into sha256, so having
//     a const char* as input should be the proper byte ordering already
void generate_nmk_hs(uint8_t nmk_hs[slac::defs::NMK_LEN], const char* plain_password, int password_len) {
    SHA256 sha256;

    // do pbkdf1 (use sha256 as hashing function, iterate 1000 times,
    // use salt)
    sha256.add(plain_password, password_len);
    sha256.add(slac::defs::NMK_HASH, sizeof(slac::defs::NMK_HASH));

    uint8_t hash[SHA256::HashBytes];
    sha256.getHash(hash);
    for (int i = 0; i < 1000 - 1; ++i) {
        sha256(hash, sizeof(hash));
        sha256.getHash(hash);
    }

    memcpy(nmk_hs, hash, slac::defs::NMK_LEN);
}

void generate_nid_from_nmk(uint8_t nid[slac::defs::NID_LEN], const uint8_t nmk[slac::defs::NMK_LEN]) {
    SHA256 sha256;

    // msb of least significant octet of NMK should be the leftmost bit
    // of the input, which corresponds to the usual const char* order

    // do pbkdf1 (use sha256 as hashing function, iterate 5 times, no
    // salt)
    uint8_t hash[SHA256::HashBytes];
    sha256(nmk, slac::defs::NMK_LEN);
    sha256.getHash(hash);
    for (int i = 0; i < 5 - 1; ++i) {
        sha256(hash, sizeof(hash));
        sha256.getHash(hash);
    }

    // use leftmost 52 bits of the hash output
    // left most bit should be bit 7 of the nid
    memcpy(nid, hash, slac::defs::NID_LEN - 1); // (bits 52 - 5)
    nid[slac::defs::NID_LEN - 1] =
        (slac::defs::NID_SECURITY_LEVEL_SIMPLE_CONNECT << slac::defs::NID_SECURITY_LEVEL_OFFSET) |
        ((static_cast<uint8_t>(hash[6])) >> slac::defs::NID_MOST_SIGNIFANT_BYTE_SHIFT);
}

} // namespace utils

namespace messages {

static constexpr auto effective_payload_length(const defs::MMV mmv) {
    if (mmv == defs::MMV::AV_1_0) {
        return sizeof(homeplug_message::payload);
    } else {
        return sizeof(homeplug_message::payload) - sizeof(homeplug_fragmentation_part);
    }
}

void HomeplugMessage::setup_payload(void const* payload, int len, uint16_t mmtype, const defs::MMV mmv) {
    // FIXME (aw): shouldn't this assert test for "<="? Furthermore it will just crash the client code ...
    assert(("Homeplug Payload length too long", len < effective_payload_length(mmv)));
    raw_msg.homeplug_header.mmv = static_cast<std::underlying_type_t<defs::MMV>>(mmv);
    raw_msg.homeplug_header.mmtype = htole16(mmtype);

    uint8_t* dst = raw_msg.payload;

    if (mmv != defs::MMV::AV_1_0) {
        homeplug_fragmentation_part fragmentation_part{};
        fragmentation_part.fmni = 0; // not implemented
        fragmentation_part.fmsn = 0; // not implemented
        memcpy(dst, &fragmentation_part, sizeof(fragmentation_part));
        dst += sizeof(fragmentation_part); // adjust effective payload start
    }

    // copy payload into place
    memcpy(dst, payload, len);

    // get pointer to the end of buffer
    uint8_t* dst_end = dst + len;

    // calculate raw message length
    raw_msg_len = dst_end - reinterpret_cast<uint8_t*>(&raw_msg);

    // do padding
    auto padding_len = defs::MME_MIN_LENGTH - raw_msg_len;
    if (padding_len > 0) {
        memset(dst_end, 0x00, padding_len);
        raw_msg_len = defs::MME_MIN_LENGTH;
    }
}

void HomeplugMessage::setup_ethernet_header(const uint8_t dst_mac_addr[ETH_ALEN],
                                            const uint8_t src_mac_addr[ETH_ALEN]) {

    // ethernet frame byte order is big endian
    raw_msg.ethernet_header.ether_type = htons(defs::ETH_P_HOMEPLUG_GREENPHY);
    if (dst_mac_addr) {
        memcpy(raw_msg.ethernet_header.ether_dhost, dst_mac_addr, ETH_ALEN);
    }

    if (src_mac_addr) {
        memcpy(raw_msg.ethernet_header.ether_shost, src_mac_addr, ETH_ALEN);
        keep_src_mac = true;
    } else {
        keep_src_mac = false;
    }
}

uint16_t HomeplugMessage::get_mmtype() const {
    return le16toh(raw_msg.homeplug_header.mmtype);
}

uint8_t* HomeplugMessage::get_src_mac() {
    return raw_msg.ethernet_header.ether_shost;
}

bool HomeplugMessage::is_valid() const {
    return raw_msg_len >= defs::MME_MIN_LENGTH;
}

} // namespace messages
} // namespace slac
