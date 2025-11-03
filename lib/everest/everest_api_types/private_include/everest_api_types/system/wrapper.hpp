// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/system/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/system.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::system {

using UpdateFirmwareResponse_Internal = ::types::system::UpdateFirmwareResponse;
using UpdateFirmwareResponse_External = UpdateFirmwareResponse;

UpdateFirmwareResponse_Internal to_internal_api(UpdateFirmwareResponse_External const& val);
UpdateFirmwareResponse_External to_external_api(UpdateFirmwareResponse_Internal const& val);

using UploadLogsStatus_Internal = ::types::system::UploadLogsStatus;
using UploadLogsStatus_External = UploadLogsStatus;

UploadLogsStatus_Internal to_internal_api(UploadLogsStatus_External const& val);
UploadLogsStatus_External to_external_api(UploadLogsStatus_Internal const& val);

using LogStatusEnum_Internal = ::types::system::LogStatusEnum;
using LogStatusEnum_External = LogStatusEnum;

LogStatusEnum_Internal to_internal_api(LogStatusEnum_External const& val);
LogStatusEnum_External to_external_api(LogStatusEnum_Internal const& val);

using FirmwareUpdateStatusEnum_Internal = ::types::system::FirmwareUpdateStatusEnum;
using FirmwareUpdateStatusEnum_External = FirmwareUpdateStatusEnum;

FirmwareUpdateStatusEnum_Internal to_internal_api(FirmwareUpdateStatusEnum_External const& val);
FirmwareUpdateStatusEnum_External to_external_api(FirmwareUpdateStatusEnum_Internal const& val);

using ResetType_Internal = ::types::system::ResetType;
using ResetType_External = ResetType;

ResetType_Internal to_internal_api(ResetType_External const& val);
ResetType_External to_external_api(ResetType_Internal const& val);

using BootReason_Internal = ::types::system::BootReason;
using BootReason_External = BootReason;

BootReason_Internal to_internal_api(BootReason_External const& val);
BootReason_External to_external_api(BootReason_Internal const& val);

using FirmwareUpdateRequest_Internal = ::types::system::FirmwareUpdateRequest;
using FirmwareUpdateRequest_External = FirmwareUpdateRequest;

FirmwareUpdateRequest_Internal to_internal_api(FirmwareUpdateRequest_External const& val);
FirmwareUpdateRequest_External to_external_api(FirmwareUpdateRequest_Internal const& val);

using UploadLogsRequest_Internal = ::types::system::UploadLogsRequest;
using UploadLogsRequest_External = UploadLogsRequest;

UploadLogsRequest_Internal to_internal_api(UploadLogsRequest_External const& val);
UploadLogsRequest_External to_external_api(UploadLogsRequest_Internal const& val);

using UploadLogsResponse_Internal = ::types::system::UploadLogsResponse;
using UploadLogsResponse_External = UploadLogsResponse;

UploadLogsResponse_Internal to_internal_api(UploadLogsResponse_External const& val);
UploadLogsResponse_External to_external_api(UploadLogsResponse_Internal const& val);

using LogStatus_Internal = ::types::system::LogStatus;
using LogStatus_External = LogStatus;

LogStatus_Internal to_internal_api(LogStatus_External const& val);
LogStatus_External to_external_api(LogStatus_Internal const& val);

using FirmwareUpdateStatus_Internal = ::types::system::FirmwareUpdateStatus;
using FirmwareUpdateStatus_External = FirmwareUpdateStatus;

FirmwareUpdateStatus_Internal to_internal_api(FirmwareUpdateStatus_External const& val);
FirmwareUpdateStatus_External to_external_api(FirmwareUpdateStatus_Internal const& val);

using FirmwareUpdateStatus_Internal = ::types::system::FirmwareUpdateStatus;
using FirmwareUpdateStatus_External = FirmwareUpdateStatus;

FirmwareUpdateStatus_Internal to_internal_api(FirmwareUpdateStatus_External const& val);
FirmwareUpdateStatus_External to_external_api(FirmwareUpdateStatus_Internal const& val);

} // namespace everest::lib::API::V1_0::types::system
