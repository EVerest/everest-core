// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/udp/udp_payload.hpp>
#include <vector>

namespace everest::lib::io::udp {

udp_payload::udp_payload(std::string const& msg) {
    set_message(msg);
}

udp_payload::udp_payload(const char* msg) {
    set_message(msg);
}

bool udp_payload::operator==(udp_payload const& other) const {
    return buffer == other.buffer;
}

size_t udp_payload::size() const {
    return buffer.size();
}

bool udp_payload::set_message(std::string const& msg) {
    if (msg.size() > max_size) {
        return false;
    }
    buffer = std::vector<uint8_t>(msg.begin(), msg.end());
    return true;
}

bool udp_payload::set_message(void const* data, size_t size) {
    if (size > max_size) {
        return false;
    }
    auto ptr = reinterpret_cast<uint8_t const*>(data);
    buffer = std::vector<uint8_t>(ptr, ptr + size);
    return true;
}

} // namespace everest::lib::io::udp
