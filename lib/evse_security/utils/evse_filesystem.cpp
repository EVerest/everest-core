// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <evse_security/evse_types.hpp>
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
    try {
        if (fs::is_regular_file(file_path)) {
            return fs::remove(file_path);
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Filesystem error: " << e.what();
    }

    EVLOG_error << "Error deleting file: " << file_path;
    return false;
}

bool read_from_file(const fs::path& file_path, std::string& out_data) {
    try {
        if (fs::is_regular_file(file_path)) {
            fsstd::ifstream file(file_path, std::ios::binary);

            if (file.is_open()) {
                out_data = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                return true;
            }
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while reading from file: " << e.what();
    }

    EVLOG_error << "Error reading file: " << file_path;
    return false;
}

bool create_file_if_nonexistent(const fs::path& file_path) {
    if (file_path.empty()) {
        EVLOG_warning << "Provided empty path!";
        return false;
    }

    try {
        if (!fs::exists(file_path)) {
            std::ofstream file(file_path);
            return true;
        } else if (fs::is_directory(file_path)) {
            EVLOG_error << "Attempting to create file over existing directory: " << file_path;
            return false;
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while creating file: " << e.what();
    }

    return true;
}

bool create_file_or_dir_if_nonexistent(const fs::path& path) {
    if (path.empty()) {
        EVLOG_warning << "Provided empty path!";
        return false;
    }

    try {
        // In case the path is missing, create it
        if (fs::exists(path) == false) {
            if (path.has_extension()) {
                std::ofstream new_file(path.c_str());
                new_file.close();
                return true;
            } else {
                // Else create a directory
                fs::create_directories(path);
                return true;
            }
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while creating dir/file: " << e.what();
    }

    return false;
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
                  std::function<bool(const std::uint8_t*, std::size_t, bool last_chunk)>&& func) {
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        EVLOG_error << "Error opening file: " << file_path;
        return false;
    }

    std::vector<std::uint8_t> buffer(buffer_size);
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

bool read_hash_from_file(const fs::path& file_path, CertificateHashData& out_hash) {
    if (file_path.extension() == CERT_HASH_EXTENSION) {
        try {
            std::ifstream hs(file_path);
            std::string algo;

            hs >> algo;
            hs >> out_hash.issuer_name_hash;
            hs >> out_hash.issuer_key_hash;
            hs >> out_hash.serial_number;
            hs.close();

            out_hash.hash_algorithm = conversions::string_to_hash_algorithm(algo);

            return true;
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown error occurred while reading cert hash file: " << file_path << " err: " << e.what();
            return false;
        }
    }

    return false;
}

bool write_hash_to_file(const fs::path& file_path, const CertificateHashData& hash) {
    auto real_path = file_path;

    if (file_path.has_extension() == false || file_path.extension() != CERT_HASH_EXTENSION) {
        real_path.replace_extension(CERT_HASH_EXTENSION);
    }

    try {
        // Write out the related hash
        std::ofstream hs(real_path.c_str());

        hs << conversions::hash_algorithm_to_string(hash.hash_algorithm) << "\n";
        hs << hash.issuer_name_hash << "\n";
        hs << hash.issuer_key_hash << "\n";
        hs << hash.serial_number << "\n";
        hs.close();

        return true;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown error occurred writing cert hash file: " << file_path << " err: " << e.what();
        return false;
    }

    return false;
}

} // namespace evse_security::filesystem_utils
