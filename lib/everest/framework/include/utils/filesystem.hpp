// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_FILESYSTEM_HPP
#define UTILS_FILESYSTEM_HPP

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
namespace fs = std::filesystem;

/// \brief Check if the provided \p path is a directory
/// \returns The canonical version of the provided path
/// \throws BootException if the path doesn't exist or isn't a directory
fs::path assert_dir(const std::string& path, const std::string& path_alias = "The");

/// \brief Check if the provided \p path is a file
/// \returns The canonical version of the provided path
/// \throws BootException if the path doesn't exist or isn't a regular file
fs::path assert_file(const std::string& path, const std::string& file_alias = "The");

/// \returns true if the file at the provided \p path has an extensions \p ext
bool has_extension(const std::string& path, const std::string& ext);

/// \returns a path that has been prefixed by \p prefix from the provided json \p value
std::string get_prefixed_path_from_json(const nlohmann::json& value, const fs::path& prefix);
} // namespace Everest

#endif // UTILS_FILESYSTEM_HPP
