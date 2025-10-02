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

struct FileHashTestParams {
    std::string test_name;
    std::string expected_csv_path;
    std::string actual_csv_path;
};

void PrintTo(const FileHashTestParams& params, std::ostream* os) {
    *os << "\n  Test Parameters:"
        << "\n    test_name: " << params.test_name << "\n    expected_csv_path: " << params.expected_csv_path
        << "\n    actual_csv_path: " << params.actual_csv_path
        << "\n    On test failure: See \033[1;36m lib/everest/everest_api_types/README.md\033[0m for further details.";
}

class EverestFileHashTest : public ::testing::TestWithParam<FileHashTestParams> {};

TEST_P(EverestFileHashTest, verify_file_hashes) {
    const auto& params = GetParam();

    auto expected_lines_opt = read_csv_lines(params.expected_csv_path);
    auto actual_lines_opt = read_csv_lines(params.actual_csv_path);

    ASSERT_TRUE(expected_lines_opt) << "Could not open or read '" << params.expected_csv_path << "'";
    ASSERT_TRUE(actual_lines_opt) << "Could not open or read '" << params.actual_csv_path << "'";

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
        EXPECT_EQ(expected_lines[i], actual_lines[i]);
    }

    EXPECT_TRUE(sizes_match and not mismatch_found);
}

INSTANTIATE_TEST_SUITE_P(everest_api, EverestFileHashTest,
                         ::testing::Values(FileHashTestParams{"everest_core_types", EXPECTED_TYPE_FILE_HASHES_CSV_PATH,
                                                              ACTUAL_TYPE_FILE_HASHES_CSV_PATH},
                                           FileHashTestParams{"everest_core_interfaces",
                                                              EXPECTED_IFC_FILE_HASHES_CSV_PATH,
                                                              ACTUAL_IFC_FILE_HASHES_CSV_PATH}),
                         // This lambda tells GTest how to name each individual test case
                         // for clear output (e.g., everest_api/EverestHashTest.VerifySourceHashes/EverestCoreTypes)
                         [](const testing::TestParamInfo<FileHashTestParams>& info) { return info.param.test_name; });
