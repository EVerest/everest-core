// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::isolation_monitor {

struct IsolationMeasurement {
    float resistance_F_Ohm;
    std::optional<float> voltage_V;
};

enum class ErrorEnum {
    DeviceFault,
    CommunicationFault,
    VendorError,
    VendorWarning
};

struct Error {
    ErrorEnum type;
    std::optional<std::string> sub_type;
    std::optional<std::string> message;
};

} // namespace everest::lib::API::V1_0::types::isolation_monitor
