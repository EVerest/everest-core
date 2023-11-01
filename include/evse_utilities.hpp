// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_UTILITIES_HPP
#define EVSE_UTILITIES_HPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

#include <everest/logging.hpp>
namespace evse_security {

class EvseUtils {
public:
    static bool delete_file(const std::filesystem::path& file_path) {
        if (std::filesystem::is_regular_file(file_path))
            return std::filesystem::remove(file_path);

        EVLOG_error << "Error deleting file: " << file_path;
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

        EVLOG_error << "Error reading file: " << file_path;
        return false;
    }

    /// @brief Should be used to ensure file exists, not for directories
    static bool create_file_if_nonexistent(const std::filesystem::path& file_path) {
        if (!std::filesystem::exists(file_path)) {
            std::ofstream file(file_path);
            return true;
        } else if (std::filesystem::is_directory(file_path)) {
            EVLOG_error << "Attempting to create file over existing directory: " << file_path;
            return false;
        }

        return true;
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
        static std::random_device rd;
        static std::mt19937 generator(rd());
        static std::uniform_int_distribution<int> distribution(1, std::numeric_limits<int>::max());

        static int increment = 0;

        std::ostringstream buff;

        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        buff << std::put_time(std::gmtime(&time), "%m_%d_%Y_%H_%M_%S_") << std::to_string(++increment) << "_"
             << distribution(generator) << extension;

        return buff.str();
    }
};

} // namespace evse_security

#endif