// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "system/wrapper.hpp"
#include "system/codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::system {

UpdateFirmwareResponse_Internal to_internal_api(UpdateFirmwareResponse_External const& val) {
    using SrcT = UpdateFirmwareResponse_External;
    using TarT = UpdateFirmwareResponse_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    case SrcT::InvalidCertificate:
        return TarT::InvalidCertificate;
    case SrcT::RevokedCertificate:
        return TarT::RevokedCertificate;
    }
    throw std::out_of_range("Unexpected value for UpdateFirmwareResponse_Internal" + serialize(val));
}
UpdateFirmwareResponse_External to_external_api(UpdateFirmwareResponse_Internal const& val) {
    using SrcT = UpdateFirmwareResponse_Internal;
    using TarT = UpdateFirmwareResponse_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    case SrcT::InvalidCertificate:
        return TarT::InvalidCertificate;
    case SrcT::RevokedCertificate:
        return TarT::RevokedCertificate;
    }
    throw std::out_of_range("Unexpected value for UpdateFirmwareResponse_External");
}

UploadLogsStatus_Internal to_internal_api(UploadLogsStatus_External const& val) {
    using SrcT = UploadLogsStatus_External;
    using TarT = UploadLogsStatus_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    }
    throw std::out_of_range("Unexpected value for UploadLogsStatus_Internal" + serialize(val));
}
UploadLogsStatus_External to_external_api(UploadLogsStatus_Internal const& val) {
    using SrcT = UploadLogsStatus_Internal;
    using TarT = UploadLogsStatus_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    }
    throw std::out_of_range("Unexpected value for UploadLogsStatus_External");
}

LogStatusEnum_Internal to_internal_api(LogStatusEnum_External const& val) {
    using SrcT = LogStatusEnum_External;
    using TarT = LogStatusEnum_Internal;
    switch (val) {
    case SrcT::BadMessage:
        return TarT::BadMessage;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::NotSupportedOperation:
        return TarT::NotSupportedOperation;
    case SrcT::PermissionDenied:
        return TarT::PermissionDenied;
    case SrcT::Uploaded:
        return TarT::Uploaded;
    case SrcT::UploadFailure:
        return TarT::UploadFailure;
    case SrcT::Uploading:
        return TarT::Uploading;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    }
    throw std::out_of_range("Unexpected value for LogStatusEnum_Internal" + serialize(val));
}
LogStatusEnum_External to_external_api(LogStatusEnum_Internal const& val) {
    using SrcT = LogStatusEnum_Internal;
    using TarT = LogStatusEnum_External;
    switch (val) {
    case SrcT::BadMessage:
        return TarT::BadMessage;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::NotSupportedOperation:
        return TarT::NotSupportedOperation;
    case SrcT::PermissionDenied:
        return TarT::PermissionDenied;
    case SrcT::Uploaded:
        return TarT::Uploaded;
    case SrcT::UploadFailure:
        return TarT::UploadFailure;
    case SrcT::Uploading:
        return TarT::Uploading;
    case SrcT::AcceptedCanceled:
        return TarT::AcceptedCanceled;
    }
    throw std::out_of_range("Unexpected value for LogStatusEnum_External");
}

FirmwareUpdateStatusEnum_Internal to_internal_api(FirmwareUpdateStatusEnum_External const& val) {
    using SrcT = FirmwareUpdateStatusEnum_External;
    using TarT = FirmwareUpdateStatusEnum_Internal;
    switch (val) {
    case SrcT::Downloaded:
        return TarT::Downloaded;
    case SrcT::DownloadFailed:
        return TarT::DownloadFailed;
    case SrcT::Downloading:
        return TarT::Downloading;
    case SrcT::DownloadScheduled:
        return TarT::DownloadScheduled;
    case SrcT::DownloadPaused:
        return TarT::DownloadPaused;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::InstallationFailed:
        return TarT::InstallationFailed;
    case SrcT::Installing:
        return TarT::Installing;
    case SrcT::Installed:
        return TarT::Installed;
    case SrcT::InstallRebooting:
        return TarT::InstallRebooting;
    case SrcT::InstallScheduled:
        return TarT::InstallScheduled;
    case SrcT::InstallVerificationFailed:
        return TarT::InstallVerificationFailed;
    case SrcT::InvalidSignature:
        return TarT::InvalidSignature;
    case SrcT::SignatureVerified:
        return TarT::SignatureVerified;
    }
    throw std::out_of_range("Unexpected value for FirmwareUpdateStatusEnum_Internal" + serialize(val));
}
FirmwareUpdateStatusEnum_External to_external_api(FirmwareUpdateStatusEnum_Internal const& val) {
    using SrcT = FirmwareUpdateStatusEnum_Internal;
    using TarT = FirmwareUpdateStatusEnum_External;
    switch (val) {
    case SrcT::Downloaded:
        return TarT::Downloaded;
    case SrcT::DownloadFailed:
        return TarT::DownloadFailed;
    case SrcT::Downloading:
        return TarT::Downloading;
    case SrcT::DownloadScheduled:
        return TarT::DownloadScheduled;
    case SrcT::DownloadPaused:
        return TarT::DownloadPaused;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::InstallationFailed:
        return TarT::InstallationFailed;
    case SrcT::Installing:
        return TarT::Installing;
    case SrcT::Installed:
        return TarT::Installed;
    case SrcT::InstallRebooting:
        return TarT::InstallRebooting;
    case SrcT::InstallScheduled:
        return TarT::InstallScheduled;
    case SrcT::InstallVerificationFailed:
        return TarT::InstallVerificationFailed;
    case SrcT::InvalidSignature:
        return TarT::InvalidSignature;
    case SrcT::SignatureVerified:
        return TarT::SignatureVerified;
    }
    throw std::out_of_range("Unexpected value for FirmwareUpdateStatusEnum_External");
}

ResetType_Internal to_internal_api(ResetType_External const& val) {
    using SrcT = ResetType_External;
    using TarT = ResetType_Internal;
    switch (val) {
    case SrcT::Soft:
        return TarT::Soft;
    case SrcT::Hard:
        return TarT::Hard;
    case SrcT::NotSpecified:
        return TarT::NotSpecified;
    }
    throw std::out_of_range("Unexpected value for ResetType_Internal" + serialize(val));
}
ResetType_External to_external_api(ResetType_Internal const& val) {
    using SrcT = ResetType_Internal;
    using TarT = ResetType_External;
    switch (val) {
    case SrcT::Soft:
        return TarT::Soft;
    case SrcT::Hard:
        return TarT::Hard;
    case SrcT::NotSpecified:
        return TarT::NotSpecified;
    }
    throw std::out_of_range("Unexpected value for ResetType_External");
}

BootReason_Internal to_internal_api(BootReason_External const& val) {
    using SrcT = BootReason_External;
    using TarT = BootReason_Internal;
    switch (val) {
    case SrcT::ApplicationReset:
        return TarT::ApplicationReset;
    case SrcT::FirmwareUpdate:
        return TarT::FirmwareUpdate;
    case SrcT::LocalReset:
        return TarT::LocalReset;
    case SrcT::PowerUp:
        return TarT::PowerUp;
    case SrcT::RemoteReset:
        return TarT::RemoteReset;
    case SrcT::ScheduledReset:
        return TarT::ScheduledReset;
    case SrcT::Triggered:
        return TarT::Triggered;
    case SrcT::Unknown:
        return TarT::Unknown;
    case SrcT::Watchdog:
        return TarT::Watchdog;
    }
    throw std::out_of_range("Unexpected value for BootReason_Internal" + serialize(val));
}
BootReason_External to_external_api(BootReason_Internal const& val) {
    using SrcT = BootReason_Internal;
    using TarT = BootReason_External;
    switch (val) {
    case SrcT::ApplicationReset:
        return TarT::ApplicationReset;
    case SrcT::FirmwareUpdate:
        return TarT::FirmwareUpdate;
    case SrcT::LocalReset:
        return TarT::LocalReset;
    case SrcT::PowerUp:
        return TarT::PowerUp;
    case SrcT::RemoteReset:
        return TarT::RemoteReset;
    case SrcT::ScheduledReset:
        return TarT::ScheduledReset;
    case SrcT::Triggered:
        return TarT::Triggered;
    case SrcT::Unknown:
        return TarT::Unknown;
    case SrcT::Watchdog:
        return TarT::Watchdog;
    }
    throw std::out_of_range("Unexpected value for BootReason_External");
}

FirmwareUpdateRequest_Internal to_internal_api(FirmwareUpdateRequest_External const& val) {
    FirmwareUpdateRequest_Internal result;
    result.request_id = val.request_id;
    result.location = val.location;
    result.retries = val.retries;
    result.retry_interval_s = val.retry_interval_s;
    result.retrieve_timestamp = val.retrieve_timestamp;
    result.install_timestamp = val.install_timestamp;
    result.signing_certificate = val.signing_certificate;
    result.signature = val.signature;
    return result;
}
FirmwareUpdateRequest_External to_external_api(FirmwareUpdateRequest_Internal const& val) {
    FirmwareUpdateRequest_External result;
    result.request_id = val.request_id;
    result.location = val.location;
    result.retries = val.retries;
    result.retry_interval_s = val.retry_interval_s;
    result.retrieve_timestamp = val.retrieve_timestamp;
    result.install_timestamp = val.install_timestamp;
    result.signing_certificate = val.signing_certificate;
    result.signature = val.signature;
    return result;
}

UploadLogsRequest_Internal to_internal_api(UploadLogsRequest_External const& val) {
    UploadLogsRequest_Internal result;
    result.location = val.location;
    result.retries = val.retries;
    result.retry_interval_s = val.retry_interval_s;
    result.oldest_timestamp = val.oldest_timestamp;
    result.latest_timestamp = val.latest_timestamp;
    result.type = val.type;
    result.request_id = val.request_id;
    return result;
}
UploadLogsRequest_External to_external_api(UploadLogsRequest_Internal const& val) {
    UploadLogsRequest_External result;
    result.location = val.location;
    result.retries = val.retries;
    result.retry_interval_s = val.retry_interval_s;
    result.oldest_timestamp = val.oldest_timestamp;
    result.latest_timestamp = val.latest_timestamp;
    result.type = val.type;
    result.request_id = val.request_id;
    return result;
}

UploadLogsResponse_Internal to_internal_api(UploadLogsResponse_External const& val) {
    UploadLogsResponse_Internal result;
    result.upload_logs_status = to_internal_api(val.upload_logs_status);
    result.file_name = val.file_name;
    return result;
}
UploadLogsResponse_External to_external_api(UploadLogsResponse_Internal const& val) {
    UploadLogsResponse_External result;
    result.upload_logs_status = to_external_api(val.upload_logs_status);
    result.file_name = val.file_name;
    return result;
}

LogStatus_Internal to_internal_api(LogStatus_External const& val) {
    LogStatus_Internal result;
    result.log_status = to_internal_api(val.log_status);
    result.request_id = val.request_id;
    return result;
}
LogStatus_External to_external_api(LogStatus_Internal const& val) {
    LogStatus_External result;
    result.log_status = to_external_api(val.log_status);
    result.request_id = val.request_id;
    return result;
}

FirmwareUpdateStatus_Internal to_internal_api(FirmwareUpdateStatus_External const& val) {
    FirmwareUpdateStatus_Internal result;
    result.firmware_update_status = to_internal_api(val.firmware_update_status);
    result.request_id = val.request_id;
    return result;
}
FirmwareUpdateStatus_External to_external_api(FirmwareUpdateStatus_Internal const& val) {
    FirmwareUpdateStatus_External result;
    result.firmware_update_status = to_external_api(val.firmware_update_status);
    result.request_id = val.request_id;
    return result;
}

} // namespace everest::lib::API::V1_0::types::system
