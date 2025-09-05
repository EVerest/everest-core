// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/system/API.hpp>

namespace everest::lib::API::V1_0::types::system {

using json = nlohmann::json;

void to_json(json& j, const FirmwareUpdateRequest& k) noexcept;
void from_json(const json& j, FirmwareUpdateRequest& k);

void to_json(json& j, const UploadLogsRequest& k) noexcept;
void from_json(const json& j, UploadLogsRequest& k);

void to_json(json& j, const UploadLogsResponse& k) noexcept;
void from_json(const json& j, UploadLogsResponse& k);

void to_json(json& j, const LogStatus& k) noexcept;
void from_json(const json& j, LogStatus& k);

void to_json(json& j, const FirmwareUpdateStatus& k) noexcept;
void from_json(const json& j, FirmwareUpdateStatus& k);

void to_json(json& j, const ResetRequest& k) noexcept;
void from_json(const json& j, ResetRequest& k);

void to_json(json& j, const UpdateFirmwareResponse& k) noexcept;
void from_json(const json& j, UpdateFirmwareResponse& k);

void to_json(json& j, const UploadLogsStatus& k) noexcept;
void from_json(const json& j, UploadLogsStatus& k);

void to_json(json& j, const LogStatusEnum& k) noexcept;
void from_json(const json& j, LogStatusEnum& k);

void to_json(json& j, const FirmwareUpdateStatusEnum& k) noexcept;
void from_json(const json& j, FirmwareUpdateStatusEnum& k);

void to_json(json& j, const ResetType& k) noexcept;
void from_json(const json& j, ResetType& k);

void to_json(json& j, const BootReason& k) noexcept;
void from_json(const json& j, BootReason& k);

} // namespace everest::lib::API::V1_0::types::system
