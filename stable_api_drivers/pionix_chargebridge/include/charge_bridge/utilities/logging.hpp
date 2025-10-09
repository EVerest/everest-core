// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <iostream>


namespace charge_bridge::utilities {

    std::ostream& print_error(std::string const& device, std::string const& unit, int status);

}
