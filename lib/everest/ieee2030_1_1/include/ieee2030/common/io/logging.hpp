// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <string>

namespace ieee2030::io {
void set_logging_callback(const std::function<void(std::string)>&);

} // namespace ieee2030::io