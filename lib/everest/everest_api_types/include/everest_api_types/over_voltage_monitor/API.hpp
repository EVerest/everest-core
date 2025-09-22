// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

enum class ErrorEnum {
    MREC5OverVoltage,
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

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
