// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

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
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(UploadLogsStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(LogStatusEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(FirmwareUpdateStatusEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ResetType val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(BootReason val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(FirmwareUpdateRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(UploadLogsRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(UploadLogsResponse const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(LogStatus const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(FirmwareUpdateStatus const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ResetRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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

template <> UpdateFirmwareResponse deserialize(std::string const& val) {
    return json::parse(val);
}

template <> UploadLogsStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> LogStatusEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> FirmwareUpdateStatusEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ResetType deserialize(std::string const& val) {
    return json::parse(val);
}

template <> BootReason deserialize(std::string const& val) {
    return json::parse(val);
}

template <> FirmwareUpdateRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> UploadLogsRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> UploadLogsResponse deserialize(std::string const& val) {
    return json::parse(val);
}

template <> LogStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> FirmwareUpdateStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ResetRequest deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::system
