// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/utilities/filesystem.hpp>

#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

namespace charge_bridge::filesystem_utils {

bool read_from_file_partial(const fs::path& file_path, const std::size_t byte_count, std::string& out_data) {
    try {
        if (fs::is_regular_file(file_path)) {
            std::ifstream file(file_path, std::ios::binary);

            if (file.is_open()) {
                std::vector<char> buffer(byte_count);
                file.read(buffer.data(), byte_count);

                std::size_t read_bytes = file.gcount();

                if (read_bytes == byte_count) {
                    out_data.assign(buffer.data(), read_bytes);
                    return true;
                }
            }
        }
    } catch (const std::exception& e) {
        return false;
    }

    return false;
}

bool read_from_file(const fs::path& file_path, std::string& out_data) {
    try {
        if (fs::is_regular_file(file_path)) {
            std::ifstream file(file_path, std::ios::binary);

            if (file.is_open()) {
                out_data = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                return true;
            }
        }
    } catch (const std::exception& e) {
        return false;
    }

    return false;
}

bool process_file(const fs::path& file_path, size_t buffer_size,
                  std::function<bool(const std::vector<std::uint8_t>&, bool last_chunk)>&& func) {
    std::ifstream file(file_path, std::ios::binary);

    return process_file(file, buffer_size, std::move(func));
}

bool process_file(std::ifstream& file, size_t buffer_size,
                  std::function<bool(const std::vector<std::uint8_t>&, bool last_chunk)>&& func) {
    if (!file) {
        return false;
    }

    std::vector<std::uint8_t> buffer(buffer_size);
    bool interupted = false;

    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer_size)) {
        interupted = func(buffer, false);

        if (interupted) {
            break;
        }
    }

    // Process the remaining bytes
    if (interupted == false) {
        size_t remaining = file.gcount();

        // Keep only remaining elements
        buffer.resize(remaining);
        func(buffer, true);
    }

    return true;
}

// Returns true on success, fills `hdr`, and sets `image_offset` to the byte
// position (from start of file) where the firmware image begins.
bool read_crypt_signed_header(const fs::path& path, CryptSignedHeader& hdr, std::uint32_t& image_offset) {
    std::ifstream f(path, std::ios::binary);
    if (!f)
        return false;

    auto read_exact = [&](void* dst, std::size_t n) -> bool {
        f.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(n));
        return f.good() || (f.eof() && static_cast<std::size_t>(f.gcount()) == n);
    };

    char firmware_version_str[32];
    std::memset(firmware_version_str, 0, sizeof(firmware_version_str)); // all zeros
    // 32-byte reserved header
    if (!read_exact(firmware_version_str, sizeof(firmware_version_str)))
        return false;

    hdr.firmware_version = std::string(firmware_version_str);

    // 1-byte signature length
    if (!read_exact(&hdr.sig_len, 1))
        return false;

    // L-byte signature
    hdr.signature.resize(hdr.sig_len);
    if (hdr.sig_len > 0) {
        if (!read_exact(hdr.signature.data(), hdr.signature.size()))
            return false;
    }

    // 1-byte NUM_SECTORS
    if (!read_exact(&hdr.num_sectors, 1))
        return false;

    // 16-byte IV
    if (!read_exact(hdr.iv.data(), hdr.iv.size()))
        return false;

    // Where the firmware image starts:
    // offset = 32 + 1 + L + 1 + 16
    image_offset = static_cast<std::uint32_t>(f.tellg());
    // As a sanity fallback, compute if tellg() failed:
    // Disabled, since it is always false
    // if (static_cast<std::streamoff>(image_offset)< 0) {
    //     image_offset = 32u + 1u + static_cast<std::uint64_t>(hdr.sig_len) + 1u + 16u;
    // }

    return true;
}

} // namespace charge_bridge::filesystem_utils
