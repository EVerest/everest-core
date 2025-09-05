// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::system {

std::string serialize(UpdateFirmwareResponse val) noexcept;
std::string serialize(UploadLogsStatus val) noexcept;
std::string serialize(LogStatusEnum val) noexcept;
std::string serialize(FirmwareUpdateStatusEnum val) noexcept;
std::string serialize(ResetType val) noexcept;
std::string serialize(BootReason val) noexcept;
std::string serialize(FirmwareUpdateRequest const& val) noexcept;
std::string serialize(UploadLogsRequest const& val) noexcept;
std::string serialize(UploadLogsResponse const& val) noexcept;
std::string serialize(LogStatus const& val) noexcept;
std::string serialize(FirmwareUpdateStatus const& val) noexcept;
std::string serialize(ResetRequest const& val) noexcept;

std::ostream& operator<<(std::ostream& os, UpdateFirmwareResponse const& val);
std::ostream& operator<<(std::ostream& os, UploadLogsStatus const& val);
std::ostream& operator<<(std::ostream& os, LogStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, FirmwareUpdateStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, ResetType const& val);
std::ostream& operator<<(std::ostream& os, BootReason const& val);
std::ostream& operator<<(std::ostream& os, FirmwareUpdateRequest const& val);
std::ostream& operator<<(std::ostream& os, UploadLogsRequest const& val);
std::ostream& operator<<(std::ostream& os, LogStatus const& val);
std::ostream& operator<<(std::ostream& os, FirmwareUpdateStatus const& val);
std::ostream& operator<<(std::ostream& os, UploadLogsResponse const& val);
std::ostream& operator<<(std::ostream& os, ResetRequest const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::system
