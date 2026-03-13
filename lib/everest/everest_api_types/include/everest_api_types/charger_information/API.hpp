// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::charger_information {

struct ChargerInformation {
    std::string vendor;
    std::string model;
    std::optional<std::string> chargepoint_serial;
    std::optional<std::string> chargebox_serial;
    std::optional<std::string> friendly_name;
    std::optional<std::string> manufacturer;
    std::optional<std::string> manufacturer_url;
    std::optional<std::string> model_url;
    std::optional<std::string> model_number;
    std::optional<std::string> model_revision;
    std::optional<std::string> board_revision;
    std::optional<std::string> firmware_version;
};

} // namespace everest::lib::API::V1_0::types::charger_information
