// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::system {

enum class UpdateFirmwareResponse {
    Accepted,
    Rejected,
    AcceptedCanceled,
    InvalidCertificate,
    RevokedCertificate,
};

enum class UploadLogsStatus {
    Accepted,
    Rejected,
    AcceptedCanceled,
};

enum class LogStatusEnum {
    BadMessage,
    Idle,
    NotSupportedOperation,
    PermissionDenied,
    Uploaded,
    UploadFailure,
    Uploading,
    AcceptedCanceled,
};

enum class FirmwareUpdateStatusEnum {
    Downloaded,
    DownloadFailed,
    Downloading,
    DownloadScheduled,
    DownloadPaused,
    Idle,
    InstallationFailed,
    Installing,
    Installed,
    InstallRebooting,
    InstallScheduled,
    InstallVerificationFailed,
    InvalidSignature,
    SignatureVerified,
};

enum class ResetType {
    Soft,
    Hard,
    NotSpecified,
};

enum class BootReason {
    ApplicationReset,
    FirmwareUpdate,
    LocalReset,
    PowerUp,
    RemoteReset,
    ScheduledReset,
    Triggered,
    Unknown,
    Watchdog,
};

struct FirmwareUpdateRequest {
    int32_t request_id;
    std::string location;
    std::optional<int32_t> retries;
    std::optional<int32_t> retry_interval_s;
    std::optional<std::string> retrieve_timestamp;
    std::optional<std::string> install_timestamp;
    std::optional<std::string> signing_certificate;
    std::optional<std::string> signature;
};

struct UploadLogsRequest {
    std::string location;
    std::optional<int32_t> retries;
    std::optional<int32_t> retry_interval_s;
    std::optional<std::string> oldest_timestamp;
    std::optional<std::string> latest_timestamp;
    std::optional<std::string> type;
    std::optional<int32_t> request_id;
};

struct UploadLogsResponse {
    UploadLogsStatus upload_logs_status;
    std::optional<std::string> file_name;
};

struct LogStatus {
    LogStatusEnum log_status;
    int32_t request_id;
};

struct FirmwareUpdateStatus {
    FirmwareUpdateStatusEnum firmware_update_status;
    int32_t request_id;
};

struct ResetRequest {
    ResetType type;
    bool scheduled;
};

} // namespace everest::lib::API::V1_0::types::system
