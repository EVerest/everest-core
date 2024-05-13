// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <conversions.hpp>

namespace module {
namespace conversions {

ocpp::FirmwareStatusNotification
to_ocpp_firmware_status_notification(const types::system::FirmwareUpdateStatusEnum status) {
    switch (status) {
    case types::system::FirmwareUpdateStatusEnum::Downloaded:
        return ocpp::FirmwareStatusNotification::Downloaded;
    case types::system::FirmwareUpdateStatusEnum::DownloadFailed:
        return ocpp::FirmwareStatusNotification::DownloadFailed;
    case types::system::FirmwareUpdateStatusEnum::Downloading:
        return ocpp::FirmwareStatusNotification::Downloading;
    case types::system::FirmwareUpdateStatusEnum::DownloadScheduled:
        return ocpp::FirmwareStatusNotification::DownloadScheduled;
    case types::system::FirmwareUpdateStatusEnum::DownloadPaused:
        return ocpp::FirmwareStatusNotification::DownloadPaused;
    case types::system::FirmwareUpdateStatusEnum::Idle:
        return ocpp::FirmwareStatusNotification::Idle;
    case types::system::FirmwareUpdateStatusEnum::InstallationFailed:
        return ocpp::FirmwareStatusNotification::InstallationFailed;
    case types::system::FirmwareUpdateStatusEnum::Installing:
        return ocpp::FirmwareStatusNotification::Installing;
    case types::system::FirmwareUpdateStatusEnum::Installed:
        return ocpp::FirmwareStatusNotification::Installed;
    case types::system::FirmwareUpdateStatusEnum::InstallRebooting:
        return ocpp::FirmwareStatusNotification::InstallRebooting;
    case types::system::FirmwareUpdateStatusEnum::InstallScheduled:
        return ocpp::FirmwareStatusNotification::InstallScheduled;
    case types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed:
        return ocpp::FirmwareStatusNotification::InstallVerificationFailed;
    case types::system::FirmwareUpdateStatusEnum::InvalidSignature:
        return ocpp::FirmwareStatusNotification::InvalidSignature;
    case types::system::FirmwareUpdateStatusEnum::SignatureVerified:
        return ocpp::FirmwareStatusNotification::SignatureVerified;
    default:
        throw std::out_of_range("Could not convert FirmwareUpdateStatusEnum to FirmwareStatusNotification");
    }
}

ocpp::SessionStartedReason to_ocpp_session_started_reason(const types::evse_manager::StartSessionReason reason) {
    switch (reason) {
    case types::evse_manager::StartSessionReason::EVConnected:
        return ocpp::SessionStartedReason::EVConnected;
    case types::evse_manager::StartSessionReason::Authorized:
        return ocpp::SessionStartedReason::Authorized;
    default:
        throw std::out_of_range(
            "Could not convert types::evse_manager::StartSessionReason to ocpp::SessionStartedReason");
    }
}

ocpp::v16::DataTransferStatus to_ocpp_data_transfer_status(const types::ocpp::DataTransferStatus status) {
    switch (status) {
    case types::ocpp::DataTransferStatus::Accepted:
        return ocpp::v16::DataTransferStatus::Accepted;
    case types::ocpp::DataTransferStatus::Rejected:
        return ocpp::v16::DataTransferStatus::Rejected;
    case types::ocpp::DataTransferStatus::UnknownMessageId:
        return ocpp::v16::DataTransferStatus::UnknownMessageId;
    case types::ocpp::DataTransferStatus::UnknownVendorId:
        return ocpp::v16::DataTransferStatus::UnknownVendorId;
    default:
        return ocpp::v16::DataTransferStatus::UnknownVendorId;
    }
}

ocpp::v16::Reason to_ocpp_reason(const types::evse_manager::StopTransactionReason reason) {
    switch (reason) {
    case types::evse_manager::StopTransactionReason::EmergencyStop:
        return ocpp::v16::Reason::EmergencyStop;
    case types::evse_manager::StopTransactionReason::EVDisconnected:
        return ocpp::v16::Reason::EVDisconnected;
    case types::evse_manager::StopTransactionReason::HardReset:
        return ocpp::v16::Reason::HardReset;
    case types::evse_manager::StopTransactionReason::Local:
        return ocpp::v16::Reason::Local;
    case types::evse_manager::StopTransactionReason::PowerLoss:
        return ocpp::v16::Reason::PowerLoss;
    case types::evse_manager::StopTransactionReason::Reboot:
        return ocpp::v16::Reason::Reboot;
    case types::evse_manager::StopTransactionReason::Remote:
        return ocpp::v16::Reason::Remote;
    case types::evse_manager::StopTransactionReason::SoftReset:
        return ocpp::v16::Reason::SoftReset;
    case types::evse_manager::StopTransactionReason::UnlockCommand:
        return ocpp::v16::Reason::UnlockCommand;
    case types::evse_manager::StopTransactionReason::DeAuthorized:
        return ocpp::v16::Reason::DeAuthorized;
    case types::evse_manager::StopTransactionReason::EnergyLimitReached:
    case types::evse_manager::StopTransactionReason::GroundFault:
    case types::evse_manager::StopTransactionReason::LocalOutOfCredit:
    case types::evse_manager::StopTransactionReason::MasterPass:
    case types::evse_manager::StopTransactionReason::OvercurrentFault:
    case types::evse_manager::StopTransactionReason::PowerQuality:
    case types::evse_manager::StopTransactionReason::SOCLimitReached:
    case types::evse_manager::StopTransactionReason::StoppedByEV:
    case types::evse_manager::StopTransactionReason::TimeLimitReached:
    case types::evse_manager::StopTransactionReason::Timeout:
    case types::evse_manager::StopTransactionReason::Other:
        return ocpp::v16::Reason::Other;
    default:
        throw std::out_of_range("Could not convert types::evse_manager::StopTransactionReason to ocpp::v16::Reason");
    }
}

ocpp::v201::CertificateActionEnum
to_ocpp_certificate_action_enum(const types::iso15118_charger::CertificateActionEnum action) {
    switch (action) {
    case types::iso15118_charger::CertificateActionEnum::Install:
        return ocpp::v201::CertificateActionEnum::Install;
    case types::iso15118_charger::CertificateActionEnum::Update:
        return ocpp::v201::CertificateActionEnum::Update;
    default:
        throw std::out_of_range(
            "Could not convert types::iso15118_charger::CertificateActionEnum to ocpp::v201::CertificateActionEnum");
    }
}

ocpp::v16::ReservationStatus to_ocpp_reservation_status(const types::reservation::ReservationResult result) {
    switch (result) {
    case types::reservation::ReservationResult::Accepted:
        return ocpp::v16::ReservationStatus::Accepted;
    case types::reservation::ReservationResult::Faulted:
        return ocpp::v16::ReservationStatus::Faulted;
    case types::reservation::ReservationResult::Occupied:
        return ocpp::v16::ReservationStatus::Occupied;
    case types::reservation::ReservationResult::Rejected:
        return ocpp::v16::ReservationStatus::Rejected;
    case types::reservation::ReservationResult::Unavailable:
        return ocpp::v16::ReservationStatus::Unavailable;
    default:
        throw std::out_of_range(
            "Could not convert types::reservation::ReservationResult to ocpp::v16::ReservationStatus");
    }
}

ocpp::v16::LogStatusEnumType to_ocpp_log_status_enum_type(const types::system::UploadLogsStatus status) {
    switch (status) {
    case types::system::UploadLogsStatus::Accepted:
        return ocpp::v16::LogStatusEnumType::Accepted;
    case types::system::UploadLogsStatus::Rejected:
        return ocpp::v16::LogStatusEnumType::Rejected;
    case types::system::UploadLogsStatus::AcceptedCancelled:
        return ocpp::v16::LogStatusEnumType::AcceptedCanceled;
    default:
        throw std::out_of_range("Could not convert types::system::UploadLogsStatus to ocpp::v16::LogStatusEnumType");
    }
}

ocpp::v16::UpdateFirmwareStatusEnumType
to_ocpp_update_firmware_status_enum_type(const types::system::UpdateFirmwareResponse response) {
    switch (response) {
    case types::system::UpdateFirmwareResponse::Accepted:
        return ocpp::v16::UpdateFirmwareStatusEnumType::Accepted;
    case types::system::UpdateFirmwareResponse::Rejected:
        return ocpp::v16::UpdateFirmwareStatusEnumType::Rejected;
    case types::system::UpdateFirmwareResponse::AcceptedCancelled:
        return ocpp::v16::UpdateFirmwareStatusEnumType::AcceptedCanceled;
    case types::system::UpdateFirmwareResponse::InvalidCertificate:
        return ocpp::v16::UpdateFirmwareStatusEnumType::InvalidCertificate;
    default:
        throw std::out_of_range(
            "Could not convert types::system::UpdateFirmwareResponse to ocpp::v16::UpdateFirmwareStatusEnumType");
    }
}

ocpp::v16::HashAlgorithmEnumType
to_ocpp_hash_algorithm_enum_type(const types::iso15118_charger::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::iso15118_charger::HashAlgorithm::SHA256:
        return ocpp::v16::HashAlgorithmEnumType::SHA256;
    case types::iso15118_charger::HashAlgorithm::SHA384:
        return ocpp::v16::HashAlgorithmEnumType::SHA384;
    case types::iso15118_charger::HashAlgorithm::SHA512:
        return ocpp::v16::HashAlgorithmEnumType::SHA512;
    default:
        throw std::out_of_range(
            "Could not convert types::iso15118_charger::HashAlgorithm to ocpp::v16::HashAlgorithmEnumType");
    }
}

ocpp::v16::BootReasonEnum to_ocpp_boot_reason_enum(const types::system::BootReason reason) {
    switch (reason) {
    case types::system::BootReason::ApplicationReset:
        return ocpp::v16::BootReasonEnum::ApplicationReset;
    case types::system::BootReason::FirmwareUpdate:
        return ocpp::v16::BootReasonEnum::FirmwareUpdate;
    case types::system::BootReason::LocalReset:
        return ocpp::v16::BootReasonEnum::LocalReset;
    case types::system::BootReason::PowerUp:
        return ocpp::v16::BootReasonEnum::PowerUp;
    case types::system::BootReason::RemoteReset:
        return ocpp::v16::BootReasonEnum::RemoteReset;
    case types::system::BootReason::ScheduledReset:
        return ocpp::v16::BootReasonEnum::ScheduledReset;
    case types::system::BootReason::Triggered:
        return ocpp::v16::BootReasonEnum::Triggered;
    case types::system::BootReason::Unknown:
        return ocpp::v16::BootReasonEnum::Unknown;
    case types::system::BootReason::Watchdog:
        return ocpp::v16::BootReasonEnum::Watchdog;
    default:
        throw std::runtime_error("Could not convert BootReasonEnum");
    }
}

ocpp::v201::HashAlgorithmEnum to_ocpp_hash_algorithm_enum(const types::iso15118_charger::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::iso15118_charger::HashAlgorithm::SHA256:
        return ocpp::v201::HashAlgorithmEnum::SHA256;
    case types::iso15118_charger::HashAlgorithm::SHA384:
        return ocpp::v201::HashAlgorithmEnum::SHA384;
    case types::iso15118_charger::HashAlgorithm::SHA512:
        return ocpp::v201::HashAlgorithmEnum::SHA512;
    default:
        throw std::out_of_range(
            "Could not convert types::iso15118_charger::HashAlgorithm to ocpp::v16::HashAlgorithmEnumType");
    }
}

types::evse_manager::StopTransactionReason to_everest_stop_transaction_reason(const ocpp::v16::Reason reason) {
    switch (reason) {
    case ocpp::v16::Reason::EmergencyStop:
        return types::evse_manager::StopTransactionReason::EmergencyStop;
    case ocpp::v16::Reason::EVDisconnected:
        return types::evse_manager::StopTransactionReason::EVDisconnected;
    case ocpp::v16::Reason::HardReset:
        return types::evse_manager::StopTransactionReason::HardReset;
    case ocpp::v16::Reason::Local:
        return types::evse_manager::StopTransactionReason::Local;
    case ocpp::v16::Reason::PowerLoss:
        return types::evse_manager::StopTransactionReason::PowerLoss;
    case ocpp::v16::Reason::Reboot:
        return types::evse_manager::StopTransactionReason::Reboot;
    case ocpp::v16::Reason::Remote:
        return types::evse_manager::StopTransactionReason::Remote;
    case ocpp::v16::Reason::SoftReset:
        return types::evse_manager::StopTransactionReason::SoftReset;
    case ocpp::v16::Reason::UnlockCommand:
        return types::evse_manager::StopTransactionReason::UnlockCommand;
    case ocpp::v16::Reason::DeAuthorized:
        return types::evse_manager::StopTransactionReason::DeAuthorized;
    case ocpp::v16::Reason::Other:
        return types::evse_manager::StopTransactionReason::Other;
    default:
        throw std::out_of_range("Could not convert ocpp::v16::Reason to types::evse_manager::StopTransactionReason");
    }
}

types::system::ResetType to_everest_reset_type(const ocpp::v16::ResetType type) {
    switch (type) {
    case ocpp::v16::ResetType::Hard:
        return types::system::ResetType::Hard;
    case ocpp::v16::ResetType::Soft:
        return types::system::ResetType::Soft;
    default:
        throw std::out_of_range("Could not convert ocpp::v16::ResetType to types::system::ResetType");
    }
}

types::iso15118_charger::Status
to_everest_iso15118_charger_status(const ocpp::v201::Iso15118EVCertificateStatusEnum status) {
    switch (status) {
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Accepted:
        return types::iso15118_charger::Status::Accepted;
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Failed:
        return types::iso15118_charger::Status::Failed;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::Iso15118EVCertificateStatusEnum to types::iso15118_charger::Status");
    }
}

types::iso15118_charger::CertificateActionEnum
to_everest_certificate_action_enum(const ocpp::v201::CertificateActionEnum action) {
    switch (action) {
    case ocpp::v201::CertificateActionEnum::Install:
        return types::iso15118_charger::CertificateActionEnum::Install;
    case ocpp::v201::CertificateActionEnum::Update:
        return types::iso15118_charger::CertificateActionEnum::Update;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::CertificateActionEnum to types::iso15118_charger::CertificateActionEnum");
    }
}

types::authorization::CertificateStatus
to_everest_certificate_status(const ocpp::v201::AuthorizeCertificateStatusEnum status) {
    switch (status) {
    case ocpp::v201::AuthorizeCertificateStatusEnum::Accepted:
        return types::authorization::CertificateStatus::Accepted;
    case ocpp::v201::AuthorizeCertificateStatusEnum::SignatureError:
        return types::authorization::CertificateStatus::SignatureError;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertificateExpired:
        return types::authorization::CertificateStatus::CertificateExpired;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertificateRevoked:
        return types::authorization::CertificateStatus::CertificateRevoked;
    case ocpp::v201::AuthorizeCertificateStatusEnum::NoCertificateAvailable:
        return types::authorization::CertificateStatus::NoCertificateAvailable;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertChainError:
        return types::authorization::CertificateStatus::CertChainError;
    case ocpp::v201::AuthorizeCertificateStatusEnum::ContractCancelled:
        return types::authorization::CertificateStatus::ContractCancelled;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::AuthorizeCertificateStatusEnum to types::authorization::CertificateStatus");
    }
}

types::authorization::AuthorizationStatus to_everest_authorization_status(const ocpp::v16::AuthorizationStatus status) {
    switch (status) {
    case ocpp::v16::AuthorizationStatus::Accepted:
        return types::authorization::AuthorizationStatus::Accepted;
    case ocpp::v16::AuthorizationStatus::Blocked:
        return types::authorization::AuthorizationStatus::Blocked;
    case ocpp::v16::AuthorizationStatus::Expired:
        return types::authorization::AuthorizationStatus::Expired;
    case ocpp::v16::AuthorizationStatus::Invalid:
        return types::authorization::AuthorizationStatus::Invalid;
    case ocpp::v16::AuthorizationStatus::ConcurrentTx:
        return types::authorization::AuthorizationStatus::ConcurrentTx;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v16::AuthorizationStatus to types::authorization::AuthorizationStatus");
    }
}

types::authorization::AuthorizationStatus
to_everest_authorization_status(const ocpp::v201::AuthorizationStatusEnum status) {
    switch (status) {
    case ocpp::v201::AuthorizationStatusEnum::Accepted:
        return types::authorization::AuthorizationStatus::Accepted;
    case ocpp::v201::AuthorizationStatusEnum::Blocked:
        return types::authorization::AuthorizationStatus::Blocked;
    case ocpp::v201::AuthorizationStatusEnum::ConcurrentTx:
        return types::authorization::AuthorizationStatus::ConcurrentTx;
    case ocpp::v201::AuthorizationStatusEnum::Expired:
        return types::authorization::AuthorizationStatus::Expired;
    case ocpp::v201::AuthorizationStatusEnum::Invalid:
        return types::authorization::AuthorizationStatus::Invalid;
    case ocpp::v201::AuthorizationStatusEnum::NoCredit:
        return types::authorization::AuthorizationStatus::NoCredit;
    case ocpp::v201::AuthorizationStatusEnum::NotAllowedTypeEVSE:
        return types::authorization::AuthorizationStatus::NotAllowedTypeEVSE;
    case ocpp::v201::AuthorizationStatusEnum::NotAtThisLocation:
        return types::authorization::AuthorizationStatus::NotAtThisLocation;
    case ocpp::v201::AuthorizationStatusEnum::NotAtThisTime:
        return types::authorization::AuthorizationStatus::NotAtThisTime;
    case ocpp::v201::AuthorizationStatusEnum::Unknown:
        return types::authorization::AuthorizationStatus::Unknown;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::AuthorizationStatusEnum to types::authorization::AuthorizationStatus");
    }
}

types::ocpp::ChargingSchedulePeriod
to_charging_schedule_period(const ocpp::v16::EnhancedChargingSchedulePeriod& period) {
    types::ocpp::ChargingSchedulePeriod csp = {
        period.startPeriod,
        period.limit,
        period.stackLevel,
        period.numberPhases,
    };
    return csp;
}

types::ocpp::ChargingSchedule to_charging_schedule(const ocpp::v16::EnhancedChargingSchedule& schedule) {
    types::ocpp::ChargingSchedule csch = {
        0,
        ocpp::v16::conversions::charging_rate_unit_to_string(schedule.chargingRateUnit),
        {},
        std::nullopt,
        schedule.duration,
        std::nullopt,
        schedule.minChargingRate};
    for (const auto& i : schedule.chargingSchedulePeriod) {
        csch.charging_schedule_period.emplace_back(to_charging_schedule_period(i));
    }
    if (schedule.startSchedule.has_value()) {
        csch.start_schedule = schedule.startSchedule.value().to_rfc3339();
    }
    return csch;
}

types::ocpp::BootNotificationResponse
to_everest_boot_notification_response(const ocpp::v16::BootNotificationResponse& boot_notification_response) {
    types::ocpp::BootNotificationResponse everest_boot_notification_response;
    everest_boot_notification_response.status = to_everest_registration_status(boot_notification_response.status);
    everest_boot_notification_response.current_time = boot_notification_response.currentTime.to_rfc3339();
    everest_boot_notification_response.interval = boot_notification_response.interval;
    return everest_boot_notification_response;
}

types::ocpp::RegistrationStatus
to_everest_registration_status(const ocpp::v16::RegistrationStatus& registration_status) {
    switch (registration_status) {
    case ocpp::v16::RegistrationStatus::Accepted:
        return types::ocpp::RegistrationStatus::Accepted;
    case ocpp::v16::RegistrationStatus::Pending:
        return types::ocpp::RegistrationStatus::Pending;
    case ocpp::v16::RegistrationStatus::Rejected:
        return types::ocpp::RegistrationStatus::Rejected;
    default:
        throw std::out_of_range("Could not convert ocpp::v201::RegistrationStatus to types::ocpp::RegistrationStatus");
    }
}

} // namespace conversions
} // namespace module
