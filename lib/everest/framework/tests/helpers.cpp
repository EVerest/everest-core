// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <tests/helpers.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Everest {
namespace tests {

fs::path get_bin_dir() {
    return fs::canonical("/proc/self/exe").parent_path();
}

} // namespace tests
} // namespace Everest
