// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "CarManufacturer.hpp"

namespace module {

types::evse_manager::CarManufacturer get_manufacturer_from_mac(const std::string& mac) {
    if (mac.size() < 8)
        return types::evse_manager::CarManufacturer::Unknown;
    if (mac.substr(0, 8) == "00:7D:FA")
        return types::evse_manager::CarManufacturer::VolkswagenGroup;
    if (mac.substr(0, 8) == "98:ED:5C")
        return types::evse_manager::CarManufacturer::Tesla;
    return types::evse_manager::CarManufacturer::Unknown;
}

} // namespace module
