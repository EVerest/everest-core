// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "CarManufacturer.hpp"
#include <set>

namespace module {

types::evse_manager::CarManufacturer get_manufacturer_from_mac(const std::string& mac) {
    // Tesla OUIs according to http://standards-oui.ieee.org/oui.txt
    const std::set<std::string> tesla = {
        "0C:29:8F",
        "4C:FC:AA",
        "54:F8:F0",
        "98:ED:5C",
    };

    if (mac.size() < 8) {
        return types::evse_manager::CarManufacturer::Unknown;
    }

    if (mac.substr(0, 8) == "00:7D:FA") {
        return types::evse_manager::CarManufacturer::VolkswagenGroup;
    }

    if (tesla.count(mac.substr(0, 8)) > 0) {
        return types::evse_manager::CarManufacturer::Tesla;
    }

    // Tesla also acquired a /28 sub-range, let's have a dedicated check for this
    // https://mac.lc/address/DC:44:27
    if (mac.substr(0, 10) == "DC:44:27:1") {
        return types::evse_manager::CarManufacturer::Tesla;
    }

    return types::evse_manager::CarManufacturer::Unknown;
}

} // namespace module
