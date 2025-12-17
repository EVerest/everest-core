// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <vector>

namespace fs = std::filesystem;
namespace charge_bridge::filesystem_utils {

bool read_from_file_partial(const fs::path& file_path, const std::size_t byte_count, std::string& out_data);

bool read_from_file(const fs::path& file_path, std::string& out_data);

/// @brief Process the file in chunks with the provided function. If the process function
/// returns false, this function will also immediately  return
/// @return True if the file was properly opened false otherwise
bool process_file(const fs::path& file_path, std::size_t buffer_size,
                  std::function<bool(const std::vector<std::uint8_t>&, bool last_chunk)>&& func);

bool process_file(std::ifstream& file, std::size_t buffer_size,
                  std::function<bool(const std::vector<std::uint8_t>&, bool last_chunk)>&& func);

struct CryptSignedHeader {
    std::string firmware_version; // max 32 bytes long string describing the fw version
    std::uint8_t sig_len = 0;
    std::vector<std::uint8_t> signature; // length = sig_len
    std::uint8_t num_sectors = 0;        // global one-byte value
    std::array<std::uint8_t, 16> iv{};   // 16-byte IV from file #2
};

bool read_crypt_signed_header(const fs::path& path, CryptSignedHeader& hdr, std::uint32_t& image_offset);

} // namespace charge_bridge::filesystem_utils
