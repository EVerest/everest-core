// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "system/codec.hpp"
#include "nlohmann/json.hpp"
#include "system/API.hpp"
#include "system/json_codec.hpp"
#include "utilities/constants.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::system {

std::string serialize(UpdateFirmwareResponse val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(UploadLogsStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(LogStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(FirmwareUpdateStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ResetType val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(BootReason val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(FirmwareUpdateRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(UploadLogsRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(UploadLogsResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(LogStatus const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(FirmwareUpdateStatus const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ResetRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, UpdateFirmwareResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, UploadLogsStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LogStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, FirmwareUpdateStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ResetType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, BootReason const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, FirmwareUpdateRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, UploadLogsRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LogStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, FirmwareUpdateStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, UploadLogsResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ResetRequest const& val) {
    os << serialize(val);
    return os;
}

template <> UpdateFirmwareResponse deserialize(std::string const& s) {
    auto data = json::parse(s);
    UpdateFirmwareResponse result = data;
    return result;
}

template <> UploadLogsStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    UploadLogsStatus result = data;
    return result;
}

template <> LogStatusEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    LogStatusEnum result = data;
    return result;
}

template <> FirmwareUpdateStatusEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    FirmwareUpdateStatusEnum result = data;
    return result;
}

template <> ResetType deserialize(std::string const& s) {
    auto data = json::parse(s);
    ResetType result = data;
    return result;
}

template <> BootReason deserialize(std::string const& s) {
    auto data = json::parse(s);
    BootReason result = data;
    return result;
}

template <> FirmwareUpdateRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    FirmwareUpdateRequest result = data;
    return result;
}

template <> UploadLogsRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    UploadLogsRequest result = data;
    return result;
}

template <> UploadLogsResponse deserialize(std::string const& s) {
    auto data = json::parse(s);
    UploadLogsResponse result = data;
    return result;
}

template <> LogStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    LogStatus result = data;
    return result;
}

template <> FirmwareUpdateStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    FirmwareUpdateStatus result = data;
    return result;
}

template <> ResetRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    ResetRequest result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::system
