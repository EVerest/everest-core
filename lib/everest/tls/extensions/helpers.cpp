// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "helpers.hpp"
#include "trusted_ca_keys.hpp"
#include <cstddef>

std::ostream& operator<<(std::ostream& out, const openssl::certificate_ptr& obj) {
    const auto subject = openssl::certificate_subject(obj.get());
    if (!subject.empty()) {
        out << "subject:";
        for (const auto& itt : subject) {
            out << " " << itt.first << ":" << itt.second;
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const openssl::sha_1_digest_t& obj) {
    const auto sav = out.flags();
    for (const auto& c : obj) {
        out << std::setw(2) << std::setfill('0') << std::hex << static_cast<std::uint32_t>(c);
    }
    out.flags(sav);
    return out;
}

std::ostream& operator<<(std::ostream& out, const tls::trusted_ca_keys::trusted_ca_keys_t& obj) {
    out << "trusted ca keys: pre-agreed: " << obj.pre_agreed << std::endl;
    if (!obj.cert_sha1_hash.empty()) {
        for (const auto& hash : obj.cert_sha1_hash) {
            out << "  certificate hash:          " << hash << std::endl;
        }
    }
    if (!obj.key_sha1_hash.empty()) {
        for (const auto& hash : obj.key_sha1_hash) {
            out << "  subject key hash:          " << hash << std::endl;
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const openssl::DER& obj) {
    const auto sav = out.flags();
    const auto* ptr = obj.get();
    for (std::size_t i = 0; i < obj.size(); i++) {
        out << std::setw(2) << std::setfill('0') << std::hex << static_cast<std::uint32_t>(*ptr++);
    }
    out.flags(sav);
    return out;
}
