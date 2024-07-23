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

ocpp::Powermeter to_ocpp_power_meter(const types::powermeter::Powermeter& powermeter) {
    ocpp::Powermeter ocpp_powermeter;
    ocpp_powermeter.timestamp = powermeter.timestamp;
    ocpp_powermeter.energy_Wh_import = {powermeter.energy_Wh_import.total, powermeter.energy_Wh_import.L1,
                                        powermeter.energy_Wh_import.L2, powermeter.energy_Wh_import.L3};

    ocpp_powermeter.meter_id = powermeter.meter_id;
    ocpp_powermeter.phase_seq_error = powermeter.phase_seq_error;

    if (powermeter.energy_Wh_export.has_value()) {
        const auto energy_wh_export = powermeter.energy_Wh_export.value();
        ocpp_powermeter.energy_Wh_export =
            ocpp::Energy{energy_wh_export.total, energy_wh_export.L1, energy_wh_export.L2, energy_wh_export.L3};
    }

    if (powermeter.power_W.has_value()) {
        const auto power_w = powermeter.power_W.value();
        ocpp_powermeter.power_W = ocpp::Power{power_w.total, power_w.L1, power_w.L2, power_w.L3};
    }

    if (powermeter.voltage_V.has_value()) {
        const auto voltage_v = powermeter.voltage_V.value();
        ocpp_powermeter.voltage_V = ocpp::Voltage{voltage_v.DC, voltage_v.L1, voltage_v.L2, voltage_v.L3};
    }

    if (powermeter.VAR.has_value()) {
        const auto var = powermeter.VAR.value();
        ocpp_powermeter.VAR = ocpp::ReactivePower{var.total, var.L1, var.L2, var.L3};
    }

    if (powermeter.current_A.has_value()) {
        const auto current_a = powermeter.current_A.value();
        ocpp_powermeter.current_A = ocpp::Current{current_a.DC, current_a.L1, current_a.L2, current_a.L3, current_a.N};
    }

    if (powermeter.frequency_Hz.has_value()) {
        const auto frequency_hz = powermeter.frequency_Hz.value();
        ocpp_powermeter.frequency_Hz = ocpp::Frequency{frequency_hz.L1, frequency_hz.L2, frequency_hz.L3};
    }

    return ocpp_powermeter;
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

types::display_message::MessagePriorityEnum
to_everest_display_message_priority(const ocpp::v201::MessagePriorityEnum& priority) {
    switch (priority) {
    case ocpp::v201::MessagePriorityEnum::AlwaysFront:
        return types::display_message::MessagePriorityEnum::AlwaysFront;
    case ocpp::v201::MessagePriorityEnum::InFront:
        return types::display_message::MessagePriorityEnum::InFront;
    case ocpp::v201::MessagePriorityEnum::NormalCycle:
        return types::display_message::MessagePriorityEnum::NormalCycle;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::MessagePriorityEnum to types::display_message::MessagePriorityEnum");
    }
}

ocpp::v201::MessagePriorityEnum
to_ocpp_201_message_priority(const types::display_message::MessagePriorityEnum& priority) {
    switch (priority) {
    case types::display_message::MessagePriorityEnum::AlwaysFront:
        return ocpp::v201::MessagePriorityEnum::AlwaysFront;
    case types::display_message::MessagePriorityEnum::InFront:
        return ocpp::v201::MessagePriorityEnum::InFront;
    case types::display_message::MessagePriorityEnum::NormalCycle:
        return ocpp::v201::MessagePriorityEnum::NormalCycle;
    default:
        throw std::out_of_range(
            "Could not convert types::display_message::MessagePriorityEnum to ocpp::v201::MessagePriorityEnum");
    }
}

types::display_message::MessageStateEnum to_everest_display_message_state(const ocpp::v201::MessageStateEnum& state) {
    switch (state) {
    case ocpp::v201::MessageStateEnum::Charging:
        return types::display_message::MessageStateEnum::Charging;
    case ocpp::v201::MessageStateEnum::Faulted:
        return types::display_message::MessageStateEnum::Faulted;
    case ocpp::v201::MessageStateEnum::Idle:
        return types::display_message::MessageStateEnum::Idle;
    case ocpp::v201::MessageStateEnum::Unavailable:
        return types::display_message::MessageStateEnum::Unavailable;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::MessageStateEnum to types::display_message::MessageStateEnum");
    }
}

ocpp::v201::MessageStateEnum to_ocpp_201_display_message_state(const types::display_message::MessageStateEnum& state) {
    switch (state) {
    case types::display_message::MessageStateEnum::Charging:
        return ocpp::v201::MessageStateEnum::Charging;
    case types::display_message::MessageStateEnum::Faulted:
        return ocpp::v201::MessageStateEnum::Faulted;
    case types::display_message::MessageStateEnum::Idle:
        return ocpp::v201::MessageStateEnum::Idle;
    case types::display_message::MessageStateEnum::Unavailable:
        return ocpp::v201::MessageStateEnum::Unavailable;
    default:
        throw std::out_of_range(
            "Could not convert types::display_message::MessageStateEnum to ocpp::v201::MessageStateEnum");
    }
}

types::display_message::MessageFormat
to_everest_display_message_format(const ocpp::v201::MessageFormatEnum& message_format) {
    switch (message_format) {
    case ocpp::v201::MessageFormatEnum::ASCII:
        return types::display_message::MessageFormat::ASCII;
    case ocpp::v201::MessageFormatEnum::HTML:
        return types::display_message::MessageFormat::HTML;
    case ocpp::v201::MessageFormatEnum::URI:
        return types::display_message::MessageFormat::URI;
    case ocpp::v201::MessageFormatEnum::UTF8:
        return types::display_message::MessageFormat::UTF8;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::MessageFormat to types::display_message::MessageFormatEnum");
    }
}

ocpp::v201::MessageFormatEnum to_ocpp_201_message_format_enum(const types::display_message::MessageFormat& format) {
    switch (format) {
    case types::display_message::MessageFormat::ASCII:
        return ocpp::v201::MessageFormatEnum::ASCII;
    case types::display_message::MessageFormat::HTML:
        return ocpp::v201::MessageFormatEnum::HTML;
    case types::display_message::MessageFormat::URI:
        return ocpp::v201::MessageFormatEnum::URI;
    case types::display_message::MessageFormat::UTF8:
        return ocpp::v201::MessageFormatEnum::UTF8;
    }

    throw std::out_of_range("Could not convert types::display_message::MessageFormat to ocpp::v201::MessageFormatEnum");
}

types::display_message::MessageContent
to_everest_display_message_content(const ocpp::DisplayMessageContent& message_content) {
    types::display_message::MessageContent message;
    message.content = message_content.message;
    if (message_content.message_format.has_value()) {
        message.format = to_everest_display_message_format(message_content.message_format.value());
    }
    message.language = message_content.language;

    return message;
}

ocpp::v16::DataTransferResponse
to_ocpp_data_transfer_response(const types::display_message::SetDisplayMessageResponse& set_display_message_response) {
    ocpp::v16::DataTransferResponse response;
    switch (set_display_message_response.status) {
    case types::display_message::DisplayMessageStatusEnum::Accepted:
        response.status = ocpp::v16::DataTransferStatus::Accepted;
        break;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedMessageFormat:
        response.status = ocpp::v16::DataTransferStatus::Rejected;
        break;
    case types::display_message::DisplayMessageStatusEnum::Rejected:
        response.status = ocpp::v16::DataTransferStatus::Rejected;
        break;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedPriority:
        response.status = ocpp::v16::DataTransferStatus::Rejected;
        break;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedState:
        response.status = ocpp::v16::DataTransferStatus::Rejected;
        break;
    case types::display_message::DisplayMessageStatusEnum::UnknownTransaction:
        response.status = ocpp::v16::DataTransferStatus::Rejected;
        break;
    default:
        throw std::out_of_range(
            "Could not convert types::display_message::DisplayMessageStatusEnum to ocpp::v16::DataTransferStatus");
    }

    response.data = set_display_message_response.status_info;
    return response;
}

types::display_message::DisplayMessage to_everest_display_message(const ocpp::DisplayMessage& display_message) {
    types::display_message::DisplayMessage m;
    m.id = display_message.id;
    m.message.content = display_message.message.message;
    if (display_message.message.message_format.has_value()) {
        m.message.format = to_everest_display_message_format(display_message.message.message_format.value());
    }
    m.message.language = display_message.message.language;

    if (display_message.priority.has_value()) {
        m.priority = to_everest_display_message_priority(display_message.priority.value());
    }

    m.qr_code = display_message.qr_code;
    m.session_id = display_message.transaction_id;
    if (display_message.state.has_value()) {
        m.state = to_everest_display_message_state(display_message.state.value());
    }

    m.timestamp_from = display_message.timestamp_from->to_rfc3339();
    m.timestamp_to = display_message.timestamp_to->to_rfc3339();

    return m;
}

ocpp::DisplayMessage to_ocpp_display_message(const types::display_message::DisplayMessage& display_message) {
    ocpp::DisplayMessage m;
    m.id = display_message.id;
    m.message.message = display_message.message.content;
    m.message.language = display_message.message.language;
    if (display_message.message.format.has_value()) {
        m.message.message_format = to_ocpp_201_message_format_enum(display_message.message.format.value());
    }

    if (display_message.priority.has_value()) {
        m.priority = to_ocpp_201_message_priority(display_message.priority.value());
    }

    m.qr_code = display_message.qr_code;
    m.transaction_id = display_message.session_id;

    if (display_message.state.has_value()) {
        m.state = to_ocpp_201_display_message_state(display_message.state.value());
    }

    m.timestamp_from = display_message.timestamp_from;
    m.timestamp_to = display_message.timestamp_to;

    return m;
}

types::session_cost::SessionStatus to_everest_running_cost_state(const ocpp::RunningCostState& state) {
    switch (state) {
    case ocpp::RunningCostState::Charging:
        return types::session_cost::SessionStatus::Running;
    case ocpp::RunningCostState::Idle:
        return types::session_cost::SessionStatus::Idle;
    case ocpp::RunningCostState::Finished:
        return types::session_cost::SessionStatus::Finished;
    default:
        throw std::out_of_range("Could not convert ocpp::RunningCostState to types::session_cost::SessionStatus");
    }
}

types::session_cost::SessionCostChunk create_session_cost_chunk(const double& price, const uint32_t& number_of_decimals,
                                                                const std::optional<ocpp::DateTime>& timestamp,
                                                                const std::optional<uint32_t>& meter_value) {
    types::session_cost::SessionCostChunk chunk;
    chunk.cost = types::money::MoneyAmount();
    chunk.cost->value = static_cast<int>(price * (10 ^ number_of_decimals));
    if (timestamp.has_value()) {
        chunk.timestamp_to = timestamp.value().to_rfc3339();
    }
    chunk.metervalue_to = meter_value;
    return chunk;
}

types::session_cost::ChargingPriceComponent
to_everest_charging_price_component(const double& price, const uint32_t& number_of_decimals,
                                    const types::session_cost::CostCategory category,
                                    std::optional<types::money::CurrencyCode> currency_code) {
    types::session_cost::ChargingPriceComponent c;
    types::money::Price p;
    types::money::Currency currency;
    currency.code = currency_code;
    currency.decimals = number_of_decimals;
    p.currency = currency;
    p.value.value = static_cast<int>(price * (10 ^ number_of_decimals));
    c.category = category;
    c.price = p;

    return c;
}

types::session_cost::SessionCost to_everest_session_cost(const ocpp::RunningCost& running_cost,
                                                         const uint32_t number_of_decimals,
                                                         std::optional<types::money::CurrencyCode> currency_code) {
    types::session_cost::SessionCost cost;
    cost.session_id = running_cost.transaction_id;
    cost.currency.code = currency_code;
    cost.currency.decimals = static_cast<int32_t>(number_of_decimals);
    cost.status = to_everest_running_cost_state(running_cost.state);
    cost.qrCode = running_cost.qr_code_text;
    if (running_cost.cost_messages.has_value()) {
        cost.message = std::vector<types::display_message::MessageContent>();
        for (const ocpp::DisplayMessageContent& message : running_cost.cost_messages.value()) {
            types::display_message::MessageContent m = to_everest_display_message_content(message);
            cost.message->push_back(m);
        }
    }

    types::session_cost::SessionCostChunk chunk = create_session_cost_chunk(
        running_cost.cost.value(), number_of_decimals, running_cost.timestamp, running_cost.meter_value);
    cost.cost_chunks = std::vector<types::session_cost::SessionCostChunk>();
    cost.cost_chunks->push_back(chunk);

    if (running_cost.charging_price.has_value()) {
        cost.charging_price = std::vector<types::session_cost::ChargingPriceComponent>();
        const ocpp::RunningCostChargingPrice& price = running_cost.charging_price.value();
        if (price.hour_price.has_value()) {
            types::session_cost::ChargingPriceComponent hour_price = to_everest_charging_price_component(
                price.hour_price.value(), number_of_decimals, types::session_cost::CostCategory::Time, currency_code);
            cost.charging_price->push_back(hour_price);
        }
        if (price.kWh_price.has_value()) {
            types::session_cost::ChargingPriceComponent energy_price = to_everest_charging_price_component(
                price.kWh_price.value(), number_of_decimals, types::session_cost::CostCategory::Energy, currency_code);
            cost.charging_price->push_back(energy_price);
        }
        if (price.flat_fee.has_value()) {
            types::session_cost::ChargingPriceComponent flat_fee_price = to_everest_charging_price_component(
                price.flat_fee.value(), number_of_decimals, types::session_cost::CostCategory::FlatFee, currency_code);
            cost.charging_price->push_back(flat_fee_price);
        }
    }

    if (running_cost.idle_price.has_value()) {
        types::session_cost::IdlePrice idle_price;
        const ocpp::RunningCostIdlePrice& ocpp_idle_price = running_cost.idle_price.value();
        if (ocpp_idle_price.idle_hour_price.has_value()) {
            idle_price.hour_price = ocpp_idle_price.idle_hour_price.value();
        }

        if (ocpp_idle_price.idle_grace_minutes.has_value()) {
            idle_price.grace_minutes = ocpp_idle_price.idle_grace_minutes.value();
        }

        cost.idle_price = idle_price;
    }

    if (running_cost.next_period_at_time.has_value() || running_cost.next_period_charging_price.has_value() ||
        running_cost.next_period_idle_price.has_value()) {
        types::session_cost::NextPeriodPrice next_period;
        if (running_cost.next_period_at_time.has_value()) {
            next_period.timestamp_from = running_cost.next_period_at_time.value().to_rfc3339();
        }
        if (running_cost.next_period_idle_price.has_value()) {
            types::session_cost::IdlePrice next_period_idle_price;
            const ocpp::RunningCostIdlePrice& ocpp_next_period_idle_price = running_cost.next_period_idle_price.value();
            if (ocpp_next_period_idle_price.idle_hour_price.has_value()) {
                next_period_idle_price.hour_price = ocpp_next_period_idle_price.idle_hour_price.value();
            }

            if (ocpp_next_period_idle_price.idle_grace_minutes.has_value()) {
                next_period_idle_price.grace_minutes = ocpp_next_period_idle_price.idle_grace_minutes.value();
            }

            next_period.idle_price = next_period_idle_price;
        }
        if (running_cost.next_period_charging_price.has_value()) {
            const ocpp::RunningCostChargingPrice& next_period_charging_price =
                running_cost.next_period_charging_price.value();

            next_period.charging_price = std::vector<types::session_cost::ChargingPriceComponent>();

            if (next_period_charging_price.hour_price.has_value()) {
                types::session_cost::ChargingPriceComponent hour_price = to_everest_charging_price_component(
                    next_period_charging_price.hour_price.value(), number_of_decimals,
                    types::session_cost::CostCategory::Time, currency_code);
                next_period.charging_price.push_back(hour_price);
            }

            if (next_period_charging_price.kWh_price.has_value()) {
                types::session_cost::ChargingPriceComponent energy_price = to_everest_charging_price_component(
                    next_period_charging_price.kWh_price.value(), number_of_decimals,
                    types::session_cost::CostCategory::Energy, currency_code);
                next_period.charging_price.push_back(energy_price);
            }

            if (next_period_charging_price.flat_fee.has_value()) {
                types::session_cost::ChargingPriceComponent flat_fee_price =
                    to_everest_charging_price_component(next_period_charging_price.flat_fee.value(), number_of_decimals,
                                                        types::session_cost::CostCategory::FlatFee, currency_code);
                next_period.charging_price.push_back(flat_fee_price);
            }
        }

        cost.next_period = next_period;
    }

    return cost;
}

} // namespace conversions
} // namespace module
