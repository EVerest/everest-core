// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <fmt/core.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/filesystem.hpp>

namespace Everest {

/// \brief Check if the provided \p path is a directory
/// \returns The canonical version of the provided path
/// \throws BootException if the path doesn't exist or isn't a directory
fs::path assert_dir(const std::string& path, const std::string& path_alias) {
    auto fs_path = fs::path(path);

    if (!fs::exists(fs_path)) {
        throw BootException(fmt::format("{} path '{}' does not exist", path_alias, path));
    }

    fs_path = fs::canonical(fs_path);

    if (!fs::is_directory(fs_path)) {
        throw BootException(fmt::format("{} path '{}' is not a directory", path_alias, path));
    }

    return fs_path;
}

/// \brief Check if the provided \p path is a file
/// \returns The canonical version of the provided path
/// \throws BootException if the path doesn't exist or isn't a regular file
fs::path assert_file(const std::string& path, const std::string& file_alias) {
    auto fs_file = fs::path(path);

    if (!fs::exists(fs_file)) {
        throw BootException(fmt::format("{} file '{}' does not exist", file_alias, path));
    }

    fs_file = fs::canonical(fs_file);

    if (!fs::is_regular_file(fs_file)) {
        throw BootException(fmt::format("{} file '{}' is not a regular file", file_alias, path));
    }

    return fs_file;
}

/// \returns true if the file at the provided \p path has an extensions \p ext
bool has_extension(const std::string& path, const std::string& ext) {
    auto path_ext = fs::path(path).extension().string();

    // lowercase the string
    std::transform(path_ext.begin(), path_ext.end(), path_ext.begin(), [](unsigned char c) { return std::tolower(c); });

    return path_ext == ext;
}

/// \returns a path that has been prefixed by \p prefix from the provided json \p value
std::string get_prefixed_path_from_json(const nlohmann::json& value, const fs::path& prefix) {
    auto settings_configs_dir = value.get<std::string>();
    if (fs::path(settings_configs_dir).is_relative()) {
        settings_configs_dir = (prefix / settings_configs_dir).string();
    }
    return settings_configs_dir;
}

} // namespace Everest
