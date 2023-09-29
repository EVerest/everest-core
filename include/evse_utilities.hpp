// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_UTILITIES_HPP
#define EVSE_UTILITIES_HPP

#include <filesystem>
#include <fstream>
#include <iostream>

#include <everest/logging.hpp>
namespace evse_security {

class EvseUtils {
public:
    static bool delete_file(const std::filesystem::path& file_path) {
        if (std::filesystem::is_regular_file(file_path))
            return std::filesystem::remove(file_path);

        return false;
    }

    static bool read_from_file(const std::filesystem::path& file_path, std::string& out_data) {
        if (std::filesystem::is_regular_file(file_path)) {
            std::ifstream file(file_path, std::ios::binary);

            if (file.is_open()) {
                out_data = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                return true;
            }
        }

        return false;
    }

    static bool write_to_file(const std::filesystem::path& file_path, const std::string& data,
                              std::ios::openmode mode) {
        try {
            std::ofstream fs(file_path, mode | std::ios::binary);
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

    static std::string get_random_file_name(const std::string& extension) {
        char path[] = "XXXXXX";
        mktemp(path);

        return std::string(path) + extension;
    }
};

} // namespace evse_security

#endif