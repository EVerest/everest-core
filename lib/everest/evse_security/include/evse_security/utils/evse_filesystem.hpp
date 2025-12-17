// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>

#include <evse_security/utils/evse_filesystem_types.hpp>

namespace evse_security {
struct CertificateHashData;
}

namespace evse_security::filesystem_utils {

bool is_subdirectory(const fs::path& base, const fs::path& subdir);

/// @brief Should be used to ensure file exists, not for directories
bool create_file_if_nonexistent(const fs::path& file_path);
/// @brief Ensure a file exists (if there's an extension), or a directory if no extension is found
bool create_file_or_dir_if_nonexistent(const fs::path& file_path);
bool delete_file(const fs::path& file_path);

bool read_from_file(const fs::path& file_path, std::string& out_data);
bool write_to_file(const fs::path& file_path, const std::string& data, std::ios::openmode mode);

/// @brief Process the file in chunks with the provided function. If the process function
/// returns false, this function will also immediately  return
/// @return True if the file was properly opened false otherwise
bool process_file(const fs::path& file_path, size_t buffer_size,
                  std::function<bool(const std::uint8_t*, std::size_t, bool last_chunk)>&& func);

std::string get_random_file_name(const std::string& extension);

/// @brief Attempts to read a certificate hash from a file. The extension is taken into account
/// @return True if we could read, false otherwise
bool read_hash_from_file(const fs::path& file_path, CertificateHashData& out_hash);

/// @brief Attempts to write a certificate hash to a file
/// @return True if we could write, false otherwise
bool write_hash_to_file(const fs::path& file_path, const CertificateHashData& hash);

} // namespace evse_security::filesystem_utils
