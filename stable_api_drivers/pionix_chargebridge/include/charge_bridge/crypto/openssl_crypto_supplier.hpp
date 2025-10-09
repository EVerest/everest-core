// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <vector>
#include <charge_bridge/utilities/filesystem.hpp>

namespace charge_bridge::crypto {

class OpenSSLSupplier {
public:
    static bool digest_file_sha256(const fs::path& path, std::vector<std::uint8_t>& out_digest);

    static bool base64_decode_to_bytes(const std::string& base64_string, std::vector<std::uint8_t>& out_decoded);
    static bool base64_decode_to_string(const std::string& base64_string, std::string& out_decoded);

    static bool base64_encode_from_bytes(const std::vector<std::uint8_t>& bytes, std::string& out_encoded);
    static bool base64_encode_from_string(const std::string& string, std::string& out_encoded);
};

} // namespace charge_bridge::crypto
