// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>

#include <evse_security/utils/evse_filesystem_types.hpp>

namespace evse_security::filesystem_utils {

bool is_subdirectory(const fs::path& base, const fs::path& subdir);

/// @brief Should be used to ensure file exists, not for directories
bool create_file_if_nonexistent(const fs::path& file_path);
bool delete_file(const fs::path& file_path);

bool read_from_file(const fs::path& file_path, std::string& out_data);
bool write_to_file(const fs::path& file_path, const std::string& data, std::ios::openmode mode);

/// @brief Process the file in chunks with the provided function. If the process function
/// returns false, this function will also immediately  return
/// @return True if the file was properly opened false otherwise
bool process_file(const fs::path& file_path, size_t buffer_size,
                  std::function<bool(const std::byte*, std::size_t, bool last_chunk)>&& func);

std::string get_random_file_name(const std::string& extension);

} // namespace evse_security::filesystem_utils
