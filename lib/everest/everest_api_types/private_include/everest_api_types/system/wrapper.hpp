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

UpdateFirmwareResponse_Internal toInternalApi(UpdateFirmwareResponse_External const& val);
UpdateFirmwareResponse_External toExternalApi(UpdateFirmwareResponse_Internal const& val);

using UploadLogsStatus_Internal = ::types::system::UploadLogsStatus;
using UploadLogsStatus_External = UploadLogsStatus;

UploadLogsStatus_Internal toInternalApi(UploadLogsStatus_External const& val);
UploadLogsStatus_External toExternalApi(UploadLogsStatus_Internal const& val);

using LogStatusEnum_Internal = ::types::system::LogStatusEnum;
using LogStatusEnum_External = LogStatusEnum;

LogStatusEnum_Internal toInternalApi(LogStatusEnum_External const& val);
LogStatusEnum_External toExternalApi(LogStatusEnum_Internal const& val);

using FirmwareUpdateStatusEnum_Internal = ::types::system::FirmwareUpdateStatusEnum;
using FirmwareUpdateStatusEnum_External = FirmwareUpdateStatusEnum;

FirmwareUpdateStatusEnum_Internal toInternalApi(FirmwareUpdateStatusEnum_External const& val);
FirmwareUpdateStatusEnum_External toExternalApi(FirmwareUpdateStatusEnum_Internal const& val);

using ResetType_Internal = ::types::system::ResetType;
using ResetType_External = ResetType;

ResetType_Internal toInternalApi(ResetType_External const& val);
ResetType_External toExternalApi(ResetType_Internal const& val);

using BootReason_Internal = ::types::system::BootReason;
using BootReason_External = BootReason;

BootReason_Internal toInternalApi(BootReason_External const& val);
BootReason_External toExternalApi(BootReason_Internal const& val);

using FirmwareUpdateRequest_Internal = ::types::system::FirmwareUpdateRequest;
using FirmwareUpdateRequest_External = FirmwareUpdateRequest;

FirmwareUpdateRequest_Internal toInternalApi(FirmwareUpdateRequest_External const& val);
FirmwareUpdateRequest_External toExternalApi(FirmwareUpdateRequest_Internal const& val);

using UploadLogsRequest_Internal = ::types::system::UploadLogsRequest;
using UploadLogsRequest_External = UploadLogsRequest;

UploadLogsRequest_Internal toInternalApi(UploadLogsRequest_External const& val);
UploadLogsRequest_External toExternalApi(UploadLogsRequest_Internal const& val);

using UploadLogsResponse_Internal = ::types::system::UploadLogsResponse;
using UploadLogsResponse_External = UploadLogsResponse;

UploadLogsResponse_Internal toInternalApi(UploadLogsResponse_External const& val);
UploadLogsResponse_External toExternalApi(UploadLogsResponse_Internal const& val);

using LogStatus_Internal = ::types::system::LogStatus;
using LogStatus_External = LogStatus;

LogStatus_Internal toInternalApi(LogStatus_External const& val);
LogStatus_External toExternalApi(LogStatus_Internal const& val);

using FirmwareUpdateStatus_Internal = ::types::system::FirmwareUpdateStatus;
using FirmwareUpdateStatus_External = FirmwareUpdateStatus;

FirmwareUpdateStatus_Internal toInternalApi(FirmwareUpdateStatus_External const& val);
FirmwareUpdateStatus_External toExternalApi(FirmwareUpdateStatus_Internal const& val);

using FirmwareUpdateStatus_Internal = ::types::system::FirmwareUpdateStatus;
using FirmwareUpdateStatus_External = FirmwareUpdateStatus;

FirmwareUpdateStatus_Internal toInternalApi(FirmwareUpdateStatus_External const& val);
FirmwareUpdateStatus_External toExternalApi(FirmwareUpdateStatus_Internal const& val);

} // namespace everest::lib::API::V1_0::types::system
