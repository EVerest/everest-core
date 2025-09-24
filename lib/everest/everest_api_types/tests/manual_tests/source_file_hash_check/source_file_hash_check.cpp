// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace {

inline std::string& trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base(), s.end());
    return s;
}

std::optional<std::vector<std::string>> read_csv_lines(const std::string& path) {
    std::ifstream file(path);
    std::vector<std::string> lines{};

    if (!file.is_open()) {
        return std::nullopt;
    }

    std::string line{};
    while (std::getline(file, line)) {
        trim(line);
        if (!line.empty() and line.front() != '#') {
            lines.push_back(line);
        }
    }
    return {lines};
}

} // namespace

TEST(everest_api, everest_core_source_hashes) {
    // file paths are injected as defines by CMake
    const std::string expected_types_file_hashes_csv_path = EXPECTED_TYPE_FILE_HASHES_CSV_PATH;
    const std::string actual_types_file_hashes_csv_path = ACTUAL_TYPE_FILE_HASHES_CSV_PATH;

    auto expected_lines_opt = read_csv_lines(expected_types_file_hashes_csv_path);
    auto actual_lines_opt = read_csv_lines(actual_types_file_hashes_csv_path);

    ASSERT_TRUE(expected_lines_opt) << "Could not open or read '" << expected_types_file_hashes_csv_path << "'";
    ASSERT_TRUE(actual_lines_opt) << "Could not open or read '" << actual_types_file_hashes_csv_path << "'";

    const auto& expected_lines = expected_lines_opt.value();
    const auto& actual_lines = actual_lines_opt.value();

    bool sizes_match = (expected_lines.size() == actual_lines.size());

    ASSERT_TRUE(sizes_match) << "The number of hash entries does not match.\n"
                             << "  - Expected file has " << expected_lines.size() << " entries.\n"
                             << "  - Actual file has " << actual_lines.size() << " entries.";

    bool mismatch_found = false;
    for (size_t i = 0; i < expected_lines.size(); ++i) {
        bool equal = (expected_lines[i] == actual_lines[i]);
        if (not equal) {
            mismatch_found = true;
        }
        EXPECT_TRUE(equal) << "Mismatch found at sorted line index " << i << ":\n"
                           << "  - Expected: " << expected_lines[i] << "\n"
                           << "  - Actual:   " << actual_lines[i];
    }
    if (not sizes_match or mismatch_found) {
        EXPECT_TRUE(false) << "Type yaml file hashes mismatched: See \033[1;36m "
                              "lib/everest/everest_api_types/README.md\033[0m for further details.\n";
    }
}
