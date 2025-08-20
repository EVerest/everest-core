// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <log.hpp>

#include <string>
#include <vector>

namespace module::stub {
std::vector<std::string>& get_logs(dloglevel_t loglevel);
void clear_logs();

} // namespace module::stub
