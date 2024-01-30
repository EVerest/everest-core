// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <evse_security/utils/evse_filesystem.hpp>

#include <fstream>
#include <iostream>
#include <limits>
#include <random>

#include <everest/logging.hpp>

namespace evse_security::filesystem_utils {

bool is_subdirectory(const fs::path& base, const fs::path& subdir) {
    fs::path relativePath = fs::relative(subdir, base);
    return !relativePath.empty();
}

bool delete_file(const fs::path& file_path) {
    if (fs::is_regular_file(file_path))
        return fs::remove(file_path);

    EVLOG_error << "Error deleting file: " << file_path;
    return false;
}

bool read_from_file(const fs::path& file_path, std::string& out_data) {
    if (fs::is_regular_file(file_path)) {
        fsstd::ifstream file(file_path, std::ios::binary);

        if (file.is_open()) {
            out_data = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return true;
        }
    }

    EVLOG_error << "Error reading file: " << file_path;
    return false;
}

bool create_file_if_nonexistent(const fs::path& file_path) {
    if (!fs::exists(file_path)) {
        std::ofstream file(file_path);
        return true;
    } else if (fs::is_directory(file_path)) {
        EVLOG_error << "Attempting to create file over existing directory: " << file_path;
        return false;
    }

    return true;
}

bool write_to_file(const fs::path& file_path, const std::string& data, std::ios::openmode mode) {
    try {
        fsstd::ofstream fs(file_path, mode | std::ios::binary);
        if (!fs.is_open()) {
            EVLOG_error << "Error opening file: " << file_path;
            return false;
        }
        fs.write(data.c_str(), data.size());

        if (!fs) {
            EVLOG_error << "Error writing to file: " << file_path;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown error occurred while writing to file: " << file_path;
        return false;
    }

    return true;
}

bool process_file(const fs::path& file_path, size_t buffer_size,
                  std::function<bool(const std::byte*, std::size_t, bool last_chunk)>&& func) {
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        EVLOG_error << "Error opening file: " << file_path;
        return false;
    }

    std::vector<std::byte> buffer(buffer_size);
    bool interupted = false;

    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer_size)) {
        interupted = func(buffer.data(), buffer_size, false);

        if (interupted) {
            break;
        }
    }

    // Process the remaining bytes
    if (interupted == false) {
        size_t remaining = file.gcount();
        func(buffer.data(), remaining, true);
    }

    return true;
}

std::string get_random_file_name(const std::string& extension) {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::uniform_int_distribution<int> distribution(1, std::numeric_limits<int>::max());

    static int increment = 0;

    std::ostringstream buff;

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    buff << std::put_time(std::gmtime(&time), "M%m_D%d_Y%Y_H%H_M%M_S%S_") << "i" << std::to_string(++increment) << "_r"
         << distribution(generator) << extension;

    return buff.str();
}

} // namespace evse_security::filesystem_utils
