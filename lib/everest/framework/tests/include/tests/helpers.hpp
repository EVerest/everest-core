// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TESTS_HELPERS_HPP

#include <filesystem>

namespace fs = std::filesystem;

namespace Everest {
namespace tests {

fs::path get_bin_dir();

} // namespace tests
} // namespace Everest

#endif // TESTS_HELPERS_HPP
