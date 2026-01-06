// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/ocpp_enums.hpp>

#include <string>

#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v2 {

// from: AFRRSignalResponse
namespace conversions {
std::string generic_status_enum_to_string(GenericStatusEnum e) {
    switch (e) {
    case GenericStatusEnum::Accepted:
        return "Accepted";
    case GenericStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "GenericStatusEnum"};
}

GenericStatusEnum string_to_generic_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GenericStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return GenericStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "GenericStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GenericStatusEnum& generic_status_enum) {
    os << conversions::generic_status_enum_to_string(generic_status_enum);
    return os;
}

// from: AuthorizeRequest
namespace conversions {
std::string hash_algorithm_enum_to_string(HashAlgorithmEnum e) {
    switch (e) {
    case HashAlgorithmEnum::SHA256:
        return "SHA256";
    case HashAlgorithmEnum::SHA384:
        return "SHA384";
    case HashAlgorithmEnum::SHA512:
        return "SHA512";
    }

    throw EnumToStringException{e, "HashAlgorithmEnum"};
}

HashAlgorithmEnum string_to_hash_algorithm_enum(const std::string& s) {
    if (s == "SHA256") {
        return HashAlgorithmEnum::SHA256;
    }
    if (s == "SHA384") {
        return HashAlgorithmEnum::SHA384;
    }
    if (s == "SHA512") {
        return HashAlgorithmEnum::SHA512;
    }

    throw StringToEnumException{s, "HashAlgorithmEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const HashAlgorithmEnum& hash_algorithm_enum) {
    os << conversions::hash_algorithm_enum_to_string(hash_algorithm_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string authorization_status_enum_to_string(AuthorizationStatusEnum e) {
    switch (e) {
    case AuthorizationStatusEnum::Accepted:
        return "Accepted";
    case AuthorizationStatusEnum::Blocked:
        return "Blocked";
    case AuthorizationStatusEnum::ConcurrentTx:
        return "ConcurrentTx";
    case AuthorizationStatusEnum::Expired:
        return "Expired";
    case AuthorizationStatusEnum::Invalid:
        return "Invalid";
    case AuthorizationStatusEnum::NoCredit:
        return "NoCredit";
    case AuthorizationStatusEnum::NotAllowedTypeEVSE:
        return "NotAllowedTypeEVSE";
    case AuthorizationStatusEnum::NotAtThisLocation:
        return "NotAtThisLocation";
    case AuthorizationStatusEnum::NotAtThisTime:
        return "NotAtThisTime";
    case AuthorizationStatusEnum::Unknown:
        return "Unknown";
    }

    throw EnumToStringException{e, "AuthorizationStatusEnum"};
}

AuthorizationStatusEnum string_to_authorization_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return AuthorizationStatusEnum::Accepted;
    }
    if (s == "Blocked") {
        return AuthorizationStatusEnum::Blocked;
    }
    if (s == "ConcurrentTx") {
        return AuthorizationStatusEnum::ConcurrentTx;
    }
    if (s == "Expired") {
        return AuthorizationStatusEnum::Expired;
    }
    if (s == "Invalid") {
        return AuthorizationStatusEnum::Invalid;
    }
    if (s == "NoCredit") {
        return AuthorizationStatusEnum::NoCredit;
    }
    if (s == "NotAllowedTypeEVSE") {
        return AuthorizationStatusEnum::NotAllowedTypeEVSE;
    }
    if (s == "NotAtThisLocation") {
        return AuthorizationStatusEnum::NotAtThisLocation;
    }
    if (s == "NotAtThisTime") {
        return AuthorizationStatusEnum::NotAtThisTime;
    }
    if (s == "Unknown") {
        return AuthorizationStatusEnum::Unknown;
    }

    throw StringToEnumException{s, "AuthorizationStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const AuthorizationStatusEnum& authorization_status_enum) {
    os << conversions::authorization_status_enum_to_string(authorization_status_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string message_format_enum_to_string(MessageFormatEnum e) {
    switch (e) {
    case MessageFormatEnum::ASCII:
        return "ASCII";
    case MessageFormatEnum::HTML:
        return "HTML";
    case MessageFormatEnum::URI:
        return "URI";
    case MessageFormatEnum::UTF8:
        return "UTF8";
    case MessageFormatEnum::QRCODE:
        return "QRCODE";
    }

    throw EnumToStringException{e, "MessageFormatEnum"};
}

MessageFormatEnum string_to_message_format_enum(const std::string& s) {
    if (s == "ASCII") {
        return MessageFormatEnum::ASCII;
    }
    if (s == "HTML") {
        return MessageFormatEnum::HTML;
    }
    if (s == "URI") {
        return MessageFormatEnum::URI;
    }
    if (s == "UTF8") {
        return MessageFormatEnum::UTF8;
    }
    if (s == "QRCODE") {
        return MessageFormatEnum::QRCODE;
    }

    throw StringToEnumException{s, "MessageFormatEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageFormatEnum& message_format_enum) {
    os << conversions::message_format_enum_to_string(message_format_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string authorize_certificate_status_enum_to_string(AuthorizeCertificateStatusEnum e) {
    switch (e) {
    case AuthorizeCertificateStatusEnum::Accepted:
        return "Accepted";
    case AuthorizeCertificateStatusEnum::SignatureError:
        return "SignatureError";
    case AuthorizeCertificateStatusEnum::CertificateExpired:
        return "CertificateExpired";
    case AuthorizeCertificateStatusEnum::CertificateRevoked:
        return "CertificateRevoked";
    case AuthorizeCertificateStatusEnum::NoCertificateAvailable:
        return "NoCertificateAvailable";
    case AuthorizeCertificateStatusEnum::CertChainError:
        return "CertChainError";
    case AuthorizeCertificateStatusEnum::ContractCancelled:
        return "ContractCancelled";
    }

    throw EnumToStringException{e, "AuthorizeCertificateStatusEnum"};
}

AuthorizeCertificateStatusEnum string_to_authorize_certificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return AuthorizeCertificateStatusEnum::Accepted;
    }
    if (s == "SignatureError") {
        return AuthorizeCertificateStatusEnum::SignatureError;
    }
    if (s == "CertificateExpired") {
        return AuthorizeCertificateStatusEnum::CertificateExpired;
    }
    if (s == "CertificateRevoked") {
        return AuthorizeCertificateStatusEnum::CertificateRevoked;
    }
    if (s == "NoCertificateAvailable") {
        return AuthorizeCertificateStatusEnum::NoCertificateAvailable;
    }
    if (s == "CertChainError") {
        return AuthorizeCertificateStatusEnum::CertChainError;
    }
    if (s == "ContractCancelled") {
        return AuthorizeCertificateStatusEnum::ContractCancelled;
    }

    throw StringToEnumException{s, "AuthorizeCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const AuthorizeCertificateStatusEnum& authorize_certificate_status_enum) {
    os << conversions::authorize_certificate_status_enum_to_string(authorize_certificate_status_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string energy_transfer_mode_enum_to_string(EnergyTransferModeEnum e) {
    switch (e) {
    case EnergyTransferModeEnum::AC_single_phase:
        return "AC_single_phase";
    case EnergyTransferModeEnum::AC_two_phase:
        return "AC_two_phase";
    case EnergyTransferModeEnum::AC_three_phase:
        return "AC_three_phase";
    case EnergyTransferModeEnum::DC:
        return "DC";
    case EnergyTransferModeEnum::AC_BPT:
        return "AC_BPT";
    case EnergyTransferModeEnum::AC_BPT_DER:
        return "AC_BPT_DER";
    case EnergyTransferModeEnum::AC_DER:
        return "AC_DER";
    case EnergyTransferModeEnum::DC_BPT:
        return "DC_BPT";
    case EnergyTransferModeEnum::DC_ACDP:
        return "DC_ACDP";
    case EnergyTransferModeEnum::DC_ACDP_BPT:
        return "DC_ACDP_BPT";
    case EnergyTransferModeEnum::WPT:
        return "WPT";
    case EnergyTransferModeEnum::MCS:
        return "MCS";
    case EnergyTransferModeEnum::MCS_BPT:
        return "MCS_BPT";
    }

    throw EnumToStringException{e, "EnergyTransferModeEnum"};
}

EnergyTransferModeEnum string_to_energy_transfer_mode_enum(const std::string& s) {
    if (s == "AC_single_phase") {
        return EnergyTransferModeEnum::AC_single_phase;
    }
    if (s == "AC_two_phase") {
        return EnergyTransferModeEnum::AC_two_phase;
    }
    if (s == "AC_three_phase") {
        return EnergyTransferModeEnum::AC_three_phase;
    }
    if (s == "DC") {
        return EnergyTransferModeEnum::DC;
    }
    if (s == "AC_BPT") {
        return EnergyTransferModeEnum::AC_BPT;
    }
    if (s == "AC_BPT_DER") {
        return EnergyTransferModeEnum::AC_BPT_DER;
    }
    if (s == "AC_DER") {
        return EnergyTransferModeEnum::AC_DER;
    }
    if (s == "DC_BPT") {
        return EnergyTransferModeEnum::DC_BPT;
    }
    if (s == "DC_ACDP") {
        return EnergyTransferModeEnum::DC_ACDP;
    }
    if (s == "DC_ACDP_BPT") {
        return EnergyTransferModeEnum::DC_ACDP_BPT;
    }
    if (s == "WPT") {
        return EnergyTransferModeEnum::WPT;
    }
    if (s == "MCS") {
        return EnergyTransferModeEnum::MCS;
    }
    if (s == "MCS_BPT") {
        return EnergyTransferModeEnum::MCS_BPT;
    }

    throw StringToEnumException{s, "EnergyTransferModeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const EnergyTransferModeEnum& energy_transfer_mode_enum) {
    os << conversions::energy_transfer_mode_enum_to_string(energy_transfer_mode_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string day_of_week_enum_to_string(DayOfWeekEnum e) {
    switch (e) {
    case DayOfWeekEnum::Monday:
        return "Monday";
    case DayOfWeekEnum::Tuesday:
        return "Tuesday";
    case DayOfWeekEnum::Wednesday:
        return "Wednesday";
    case DayOfWeekEnum::Thursday:
        return "Thursday";
    case DayOfWeekEnum::Friday:
        return "Friday";
    case DayOfWeekEnum::Saturday:
        return "Saturday";
    case DayOfWeekEnum::Sunday:
        return "Sunday";
    }

    throw EnumToStringException{e, "DayOfWeekEnum"};
}

DayOfWeekEnum string_to_day_of_week_enum(const std::string& s) {
    if (s == "Monday") {
        return DayOfWeekEnum::Monday;
    }
    if (s == "Tuesday") {
        return DayOfWeekEnum::Tuesday;
    }
    if (s == "Wednesday") {
        return DayOfWeekEnum::Wednesday;
    }
    if (s == "Thursday") {
        return DayOfWeekEnum::Thursday;
    }
    if (s == "Friday") {
        return DayOfWeekEnum::Friday;
    }
    if (s == "Saturday") {
        return DayOfWeekEnum::Saturday;
    }
    if (s == "Sunday") {
        return DayOfWeekEnum::Sunday;
    }

    throw StringToEnumException{s, "DayOfWeekEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DayOfWeekEnum& day_of_week_enum) {
    os << conversions::day_of_week_enum_to_string(day_of_week_enum);
    return os;
}

// from: AuthorizeResponse
namespace conversions {
std::string evse_kind_enum_to_string(EvseKindEnum e) {
    switch (e) {
    case EvseKindEnum::AC:
        return "AC";
    case EvseKindEnum::DC:
        return "DC";
    }

    throw EnumToStringException{e, "EvseKindEnum"};
}

EvseKindEnum string_to_evse_kind_enum(const std::string& s) {
    if (s == "AC") {
        return EvseKindEnum::AC;
    }
    if (s == "DC") {
        return EvseKindEnum::DC;
    }

    throw StringToEnumException{s, "EvseKindEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const EvseKindEnum& evse_kind_enum) {
    os << conversions::evse_kind_enum_to_string(evse_kind_enum);
    return os;
}

// from: BatterySwapRequest
namespace conversions {
std::string battery_swap_event_enum_to_string(BatterySwapEventEnum e) {
    switch (e) {
    case BatterySwapEventEnum::BatteryIn:
        return "BatteryIn";
    case BatterySwapEventEnum::BatteryOut:
        return "BatteryOut";
    case BatterySwapEventEnum::BatteryOutTimeout:
        return "BatteryOutTimeout";
    }

    throw EnumToStringException{e, "BatterySwapEventEnum"};
}

BatterySwapEventEnum string_to_battery_swap_event_enum(const std::string& s) {
    if (s == "BatteryIn") {
        return BatterySwapEventEnum::BatteryIn;
    }
    if (s == "BatteryOut") {
        return BatterySwapEventEnum::BatteryOut;
    }
    if (s == "BatteryOutTimeout") {
        return BatterySwapEventEnum::BatteryOutTimeout;
    }

    throw StringToEnumException{s, "BatterySwapEventEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const BatterySwapEventEnum& battery_swap_event_enum) {
    os << conversions::battery_swap_event_enum_to_string(battery_swap_event_enum);
    return os;
}

// from: BootNotificationRequest
namespace conversions {
std::string boot_reason_enum_to_string(BootReasonEnum e) {
    switch (e) {
    case BootReasonEnum::ApplicationReset:
        return "ApplicationReset";
    case BootReasonEnum::FirmwareUpdate:
        return "FirmwareUpdate";
    case BootReasonEnum::LocalReset:
        return "LocalReset";
    case BootReasonEnum::PowerUp:
        return "PowerUp";
    case BootReasonEnum::RemoteReset:
        return "RemoteReset";
    case BootReasonEnum::ScheduledReset:
        return "ScheduledReset";
    case BootReasonEnum::Triggered:
        return "Triggered";
    case BootReasonEnum::Unknown:
        return "Unknown";
    case BootReasonEnum::Watchdog:
        return "Watchdog";
    }

    throw EnumToStringException{e, "BootReasonEnum"};
}

BootReasonEnum string_to_boot_reason_enum(const std::string& s) {
    if (s == "ApplicationReset") {
        return BootReasonEnum::ApplicationReset;
    }
    if (s == "FirmwareUpdate") {
        return BootReasonEnum::FirmwareUpdate;
    }
    if (s == "LocalReset") {
        return BootReasonEnum::LocalReset;
    }
    if (s == "PowerUp") {
        return BootReasonEnum::PowerUp;
    }
    if (s == "RemoteReset") {
        return BootReasonEnum::RemoteReset;
    }
    if (s == "ScheduledReset") {
        return BootReasonEnum::ScheduledReset;
    }
    if (s == "Triggered") {
        return BootReasonEnum::Triggered;
    }
    if (s == "Unknown") {
        return BootReasonEnum::Unknown;
    }
    if (s == "Watchdog") {
        return BootReasonEnum::Watchdog;
    }

    throw StringToEnumException{s, "BootReasonEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const BootReasonEnum& boot_reason_enum) {
    os << conversions::boot_reason_enum_to_string(boot_reason_enum);
    return os;
}

// from: BootNotificationResponse
namespace conversions {
std::string registration_status_enum_to_string(RegistrationStatusEnum e) {
    switch (e) {
    case RegistrationStatusEnum::Accepted:
        return "Accepted";
    case RegistrationStatusEnum::Pending:
        return "Pending";
    case RegistrationStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "RegistrationStatusEnum"};
}

RegistrationStatusEnum string_to_registration_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return RegistrationStatusEnum::Accepted;
    }
    if (s == "Pending") {
        return RegistrationStatusEnum::Pending;
    }
    if (s == "Rejected") {
        return RegistrationStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "RegistrationStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const RegistrationStatusEnum& registration_status_enum) {
    os << conversions::registration_status_enum_to_string(registration_status_enum);
    return os;
}

// from: CancelReservationResponse
namespace conversions {
std::string cancel_reservation_status_enum_to_string(CancelReservationStatusEnum e) {
    switch (e) {
    case CancelReservationStatusEnum::Accepted:
        return "Accepted";
    case CancelReservationStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "CancelReservationStatusEnum"};
}

CancelReservationStatusEnum string_to_cancel_reservation_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return CancelReservationStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return CancelReservationStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "CancelReservationStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CancelReservationStatusEnum& cancel_reservation_status_enum) {
    os << conversions::cancel_reservation_status_enum_to_string(cancel_reservation_status_enum);
    return os;
}

// from: CertificateSignedRequest
namespace conversions {
std::string certificate_signing_use_enum_to_string(CertificateSigningUseEnum e) {
    switch (e) {
    case CertificateSigningUseEnum::ChargingStationCertificate:
        return "ChargingStationCertificate";
    case CertificateSigningUseEnum::V2GCertificate:
        return "V2GCertificate";
    case CertificateSigningUseEnum::V2G20Certificate:
        return "V2G20Certificate";
    }

    throw EnumToStringException{e, "CertificateSigningUseEnum"};
}

CertificateSigningUseEnum string_to_certificate_signing_use_enum(const std::string& s) {
    if (s == "ChargingStationCertificate") {
        return CertificateSigningUseEnum::ChargingStationCertificate;
    }
    if (s == "V2GCertificate") {
        return CertificateSigningUseEnum::V2GCertificate;
    }
    if (s == "V2G20Certificate") {
        return CertificateSigningUseEnum::V2G20Certificate;
    }

    throw StringToEnumException{s, "CertificateSigningUseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateSigningUseEnum& certificate_signing_use_enum) {
    os << conversions::certificate_signing_use_enum_to_string(certificate_signing_use_enum);
    return os;
}

// from: CertificateSignedResponse
namespace conversions {
std::string certificate_signed_status_enum_to_string(CertificateSignedStatusEnum e) {
    switch (e) {
    case CertificateSignedStatusEnum::Accepted:
        return "Accepted";
    case CertificateSignedStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "CertificateSignedStatusEnum"};
}

CertificateSignedStatusEnum string_to_certificate_signed_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return CertificateSignedStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return CertificateSignedStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "CertificateSignedStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateSignedStatusEnum& certificate_signed_status_enum) {
    os << conversions::certificate_signed_status_enum_to_string(certificate_signed_status_enum);
    return os;
}

// from: ChangeAvailabilityRequest
namespace conversions {
std::string operational_status_enum_to_string(OperationalStatusEnum e) {
    switch (e) {
    case OperationalStatusEnum::Inoperative:
        return "Inoperative";
    case OperationalStatusEnum::Operative:
        return "Operative";
    }

    throw EnumToStringException{e, "OperationalStatusEnum"};
}

OperationalStatusEnum string_to_operational_status_enum(const std::string& s) {
    if (s == "Inoperative") {
        return OperationalStatusEnum::Inoperative;
    }
    if (s == "Operative") {
        return OperationalStatusEnum::Operative;
    }

    throw StringToEnumException{s, "OperationalStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OperationalStatusEnum& operational_status_enum) {
    os << conversions::operational_status_enum_to_string(operational_status_enum);
    return os;
}

// from: ChangeAvailabilityResponse
namespace conversions {
std::string change_availability_status_enum_to_string(ChangeAvailabilityStatusEnum e) {
    switch (e) {
    case ChangeAvailabilityStatusEnum::Accepted:
        return "Accepted";
    case ChangeAvailabilityStatusEnum::Rejected:
        return "Rejected";
    case ChangeAvailabilityStatusEnum::Scheduled:
        return "Scheduled";
    }

    throw EnumToStringException{e, "ChangeAvailabilityStatusEnum"};
}

ChangeAvailabilityStatusEnum string_to_change_availability_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ChangeAvailabilityStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return ChangeAvailabilityStatusEnum::Rejected;
    }
    if (s == "Scheduled") {
        return ChangeAvailabilityStatusEnum::Scheduled;
    }

    throw StringToEnumException{s, "ChangeAvailabilityStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityStatusEnum& change_availability_status_enum) {
    os << conversions::change_availability_status_enum_to_string(change_availability_status_enum);
    return os;
}

// from: ChangeTransactionTariffResponse
namespace conversions {
std::string tariff_change_status_enum_to_string(TariffChangeStatusEnum e) {
    switch (e) {
    case TariffChangeStatusEnum::Accepted:
        return "Accepted";
    case TariffChangeStatusEnum::Rejected:
        return "Rejected";
    case TariffChangeStatusEnum::TooManyElements:
        return "TooManyElements";
    case TariffChangeStatusEnum::ConditionNotSupported:
        return "ConditionNotSupported";
    case TariffChangeStatusEnum::TxNotFound:
        return "TxNotFound";
    case TariffChangeStatusEnum::NoCurrencyChange:
        return "NoCurrencyChange";
    }

    throw EnumToStringException{e, "TariffChangeStatusEnum"};
}

TariffChangeStatusEnum string_to_tariff_change_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return TariffChangeStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return TariffChangeStatusEnum::Rejected;
    }
    if (s == "TooManyElements") {
        return TariffChangeStatusEnum::TooManyElements;
    }
    if (s == "ConditionNotSupported") {
        return TariffChangeStatusEnum::ConditionNotSupported;
    }
    if (s == "TxNotFound") {
        return TariffChangeStatusEnum::TxNotFound;
    }
    if (s == "NoCurrencyChange") {
        return TariffChangeStatusEnum::NoCurrencyChange;
    }

    throw StringToEnumException{s, "TariffChangeStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffChangeStatusEnum& tariff_change_status_enum) {
    os << conversions::tariff_change_status_enum_to_string(tariff_change_status_enum);
    return os;
}

// from: ClearCacheResponse
namespace conversions {
std::string clear_cache_status_enum_to_string(ClearCacheStatusEnum e) {
    switch (e) {
    case ClearCacheStatusEnum::Accepted:
        return "Accepted";
    case ClearCacheStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "ClearCacheStatusEnum"};
}

ClearCacheStatusEnum string_to_clear_cache_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ClearCacheStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return ClearCacheStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "ClearCacheStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ClearCacheStatusEnum& clear_cache_status_enum) {
    os << conversions::clear_cache_status_enum_to_string(clear_cache_status_enum);
    return os;
}

// from: ClearChargingProfileRequest
namespace conversions {
std::string charging_profile_purpose_enum_to_string(ChargingProfilePurposeEnum e) {
    switch (e) {
    case ChargingProfilePurposeEnum::ChargingStationExternalConstraints:
        return "ChargingStationExternalConstraints";
    case ChargingProfilePurposeEnum::ChargingStationMaxProfile:
        return "ChargingStationMaxProfile";
    case ChargingProfilePurposeEnum::TxDefaultProfile:
        return "TxDefaultProfile";
    case ChargingProfilePurposeEnum::TxProfile:
        return "TxProfile";
    case ChargingProfilePurposeEnum::PriorityCharging:
        return "PriorityCharging";
    case ChargingProfilePurposeEnum::LocalGeneration:
        return "LocalGeneration";
    }

    throw EnumToStringException{e, "ChargingProfilePurposeEnum"};
}

ChargingProfilePurposeEnum string_to_charging_profile_purpose_enum(const std::string& s) {
    if (s == "ChargingStationExternalConstraints") {
        return ChargingProfilePurposeEnum::ChargingStationExternalConstraints;
    }
    if (s == "ChargingStationMaxProfile") {
        return ChargingProfilePurposeEnum::ChargingStationMaxProfile;
    }
    if (s == "TxDefaultProfile") {
        return ChargingProfilePurposeEnum::TxDefaultProfile;
    }
    if (s == "TxProfile") {
        return ChargingProfilePurposeEnum::TxProfile;
    }
    if (s == "PriorityCharging") {
        return ChargingProfilePurposeEnum::PriorityCharging;
    }
    if (s == "LocalGeneration") {
        return ChargingProfilePurposeEnum::LocalGeneration;
    }

    throw StringToEnumException{s, "ChargingProfilePurposeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargingProfilePurposeEnum& charging_profile_purpose_enum) {
    os << conversions::charging_profile_purpose_enum_to_string(charging_profile_purpose_enum);
    return os;
}

// from: ClearChargingProfileResponse
namespace conversions {
std::string clear_charging_profile_status_enum_to_string(ClearChargingProfileStatusEnum e) {
    switch (e) {
    case ClearChargingProfileStatusEnum::Accepted:
        return "Accepted";
    case ClearChargingProfileStatusEnum::Unknown:
        return "Unknown";
    }

    throw EnumToStringException{e, "ClearChargingProfileStatusEnum"};
}

ClearChargingProfileStatusEnum string_to_clear_charging_profile_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ClearChargingProfileStatusEnum::Accepted;
    }
    if (s == "Unknown") {
        return ClearChargingProfileStatusEnum::Unknown;
    }

    throw StringToEnumException{s, "ClearChargingProfileStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ClearChargingProfileStatusEnum& clear_charging_profile_status_enum) {
    os << conversions::clear_charging_profile_status_enum_to_string(clear_charging_profile_status_enum);
    return os;
}

// from: ClearDERControlRequest
namespace conversions {
std::string dercontrol_enum_to_string(DERControlEnum e) {
    switch (e) {
    case DERControlEnum::EnterService:
        return "EnterService";
    case DERControlEnum::FreqDroop:
        return "FreqDroop";
    case DERControlEnum::FreqWatt:
        return "FreqWatt";
    case DERControlEnum::FixedPFAbsorb:
        return "FixedPFAbsorb";
    case DERControlEnum::FixedPFInject:
        return "FixedPFInject";
    case DERControlEnum::FixedVar:
        return "FixedVar";
    case DERControlEnum::Gradients:
        return "Gradients";
    case DERControlEnum::HFMustTrip:
        return "HFMustTrip";
    case DERControlEnum::HFMayTrip:
        return "HFMayTrip";
    case DERControlEnum::HVMustTrip:
        return "HVMustTrip";
    case DERControlEnum::HVMomCess:
        return "HVMomCess";
    case DERControlEnum::HVMayTrip:
        return "HVMayTrip";
    case DERControlEnum::LimitMaxDischarge:
        return "LimitMaxDischarge";
    case DERControlEnum::LFMustTrip:
        return "LFMustTrip";
    case DERControlEnum::LVMustTrip:
        return "LVMustTrip";
    case DERControlEnum::LVMomCess:
        return "LVMomCess";
    case DERControlEnum::LVMayTrip:
        return "LVMayTrip";
    case DERControlEnum::PowerMonitoringMustTrip:
        return "PowerMonitoringMustTrip";
    case DERControlEnum::VoltVar:
        return "VoltVar";
    case DERControlEnum::VoltWatt:
        return "VoltWatt";
    case DERControlEnum::WattPF:
        return "WattPF";
    case DERControlEnum::WattVar:
        return "WattVar";
    }

    throw EnumToStringException{e, "DERControlEnum"};
}

DERControlEnum string_to_dercontrol_enum(const std::string& s) {
    if (s == "EnterService") {
        return DERControlEnum::EnterService;
    }
    if (s == "FreqDroop") {
        return DERControlEnum::FreqDroop;
    }
    if (s == "FreqWatt") {
        return DERControlEnum::FreqWatt;
    }
    if (s == "FixedPFAbsorb") {
        return DERControlEnum::FixedPFAbsorb;
    }
    if (s == "FixedPFInject") {
        return DERControlEnum::FixedPFInject;
    }
    if (s == "FixedVar") {
        return DERControlEnum::FixedVar;
    }
    if (s == "Gradients") {
        return DERControlEnum::Gradients;
    }
    if (s == "HFMustTrip") {
        return DERControlEnum::HFMustTrip;
    }
    if (s == "HFMayTrip") {
        return DERControlEnum::HFMayTrip;
    }
    if (s == "HVMustTrip") {
        return DERControlEnum::HVMustTrip;
    }
    if (s == "HVMomCess") {
        return DERControlEnum::HVMomCess;
    }
    if (s == "HVMayTrip") {
        return DERControlEnum::HVMayTrip;
    }
    if (s == "LimitMaxDischarge") {
        return DERControlEnum::LimitMaxDischarge;
    }
    if (s == "LFMustTrip") {
        return DERControlEnum::LFMustTrip;
    }
    if (s == "LVMustTrip") {
        return DERControlEnum::LVMustTrip;
    }
    if (s == "LVMomCess") {
        return DERControlEnum::LVMomCess;
    }
    if (s == "LVMayTrip") {
        return DERControlEnum::LVMayTrip;
    }
    if (s == "PowerMonitoringMustTrip") {
        return DERControlEnum::PowerMonitoringMustTrip;
    }
    if (s == "VoltVar") {
        return DERControlEnum::VoltVar;
    }
    if (s == "VoltWatt") {
        return DERControlEnum::VoltWatt;
    }
    if (s == "WattPF") {
        return DERControlEnum::WattPF;
    }
    if (s == "WattVar") {
        return DERControlEnum::WattVar;
    }

    throw StringToEnumException{s, "DERControlEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DERControlEnum& dercontrol_enum) {
    os << conversions::dercontrol_enum_to_string(dercontrol_enum);
    return os;
}

// from: ClearDERControlResponse
namespace conversions {
std::string dercontrol_status_enum_to_string(DERControlStatusEnum e) {
    switch (e) {
    case DERControlStatusEnum::Accepted:
        return "Accepted";
    case DERControlStatusEnum::Rejected:
        return "Rejected";
    case DERControlStatusEnum::NotSupported:
        return "NotSupported";
    case DERControlStatusEnum::NotFound:
        return "NotFound";
    }

    throw EnumToStringException{e, "DERControlStatusEnum"};
}

DERControlStatusEnum string_to_dercontrol_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return DERControlStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return DERControlStatusEnum::Rejected;
    }
    if (s == "NotSupported") {
        return DERControlStatusEnum::NotSupported;
    }
    if (s == "NotFound") {
        return DERControlStatusEnum::NotFound;
    }

    throw StringToEnumException{s, "DERControlStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DERControlStatusEnum& dercontrol_status_enum) {
    os << conversions::dercontrol_status_enum_to_string(dercontrol_status_enum);
    return os;
}

// from: ClearDisplayMessageResponse
namespace conversions {
std::string clear_message_status_enum_to_string(ClearMessageStatusEnum e) {
    switch (e) {
    case ClearMessageStatusEnum::Accepted:
        return "Accepted";
    case ClearMessageStatusEnum::Unknown:
        return "Unknown";
    case ClearMessageStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "ClearMessageStatusEnum"};
}

ClearMessageStatusEnum string_to_clear_message_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ClearMessageStatusEnum::Accepted;
    }
    if (s == "Unknown") {
        return ClearMessageStatusEnum::Unknown;
    }
    if (s == "Rejected") {
        return ClearMessageStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "ClearMessageStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ClearMessageStatusEnum& clear_message_status_enum) {
    os << conversions::clear_message_status_enum_to_string(clear_message_status_enum);
    return os;
}

// from: ClearTariffsResponse
namespace conversions {
std::string tariff_clear_status_enum_to_string(TariffClearStatusEnum e) {
    switch (e) {
    case TariffClearStatusEnum::Accepted:
        return "Accepted";
    case TariffClearStatusEnum::Rejected:
        return "Rejected";
    case TariffClearStatusEnum::NoTariff:
        return "NoTariff";
    }

    throw EnumToStringException{e, "TariffClearStatusEnum"};
}

TariffClearStatusEnum string_to_tariff_clear_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return TariffClearStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return TariffClearStatusEnum::Rejected;
    }
    if (s == "NoTariff") {
        return TariffClearStatusEnum::NoTariff;
    }

    throw StringToEnumException{s, "TariffClearStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffClearStatusEnum& tariff_clear_status_enum) {
    os << conversions::tariff_clear_status_enum_to_string(tariff_clear_status_enum);
    return os;
}

// from: ClearVariableMonitoringResponse
namespace conversions {
std::string clear_monitoring_status_enum_to_string(ClearMonitoringStatusEnum e) {
    switch (e) {
    case ClearMonitoringStatusEnum::Accepted:
        return "Accepted";
    case ClearMonitoringStatusEnum::Rejected:
        return "Rejected";
    case ClearMonitoringStatusEnum::NotFound:
        return "NotFound";
    }

    throw EnumToStringException{e, "ClearMonitoringStatusEnum"};
}

ClearMonitoringStatusEnum string_to_clear_monitoring_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ClearMonitoringStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return ClearMonitoringStatusEnum::Rejected;
    }
    if (s == "NotFound") {
        return ClearMonitoringStatusEnum::NotFound;
    }

    throw StringToEnumException{s, "ClearMonitoringStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ClearMonitoringStatusEnum& clear_monitoring_status_enum) {
    os << conversions::clear_monitoring_status_enum_to_string(clear_monitoring_status_enum);
    return os;
}

// from: CustomerInformationResponse
namespace conversions {
std::string customer_information_status_enum_to_string(CustomerInformationStatusEnum e) {
    switch (e) {
    case CustomerInformationStatusEnum::Accepted:
        return "Accepted";
    case CustomerInformationStatusEnum::Rejected:
        return "Rejected";
    case CustomerInformationStatusEnum::Invalid:
        return "Invalid";
    }

    throw EnumToStringException{e, "CustomerInformationStatusEnum"};
}

CustomerInformationStatusEnum string_to_customer_information_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return CustomerInformationStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return CustomerInformationStatusEnum::Rejected;
    }
    if (s == "Invalid") {
        return CustomerInformationStatusEnum::Invalid;
    }

    throw StringToEnumException{s, "CustomerInformationStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CustomerInformationStatusEnum& customer_information_status_enum) {
    os << conversions::customer_information_status_enum_to_string(customer_information_status_enum);
    return os;
}

// from: DataTransferResponse
namespace conversions {
std::string data_transfer_status_enum_to_string(DataTransferStatusEnum e) {
    switch (e) {
    case DataTransferStatusEnum::Accepted:
        return "Accepted";
    case DataTransferStatusEnum::Rejected:
        return "Rejected";
    case DataTransferStatusEnum::UnknownMessageId:
        return "UnknownMessageId";
    case DataTransferStatusEnum::UnknownVendorId:
        return "UnknownVendorId";
    }

    throw EnumToStringException{e, "DataTransferStatusEnum"};
}

DataTransferStatusEnum string_to_data_transfer_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return DataTransferStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return DataTransferStatusEnum::Rejected;
    }
    if (s == "UnknownMessageId") {
        return DataTransferStatusEnum::UnknownMessageId;
    }
    if (s == "UnknownVendorId") {
        return DataTransferStatusEnum::UnknownVendorId;
    }

    throw StringToEnumException{s, "DataTransferStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DataTransferStatusEnum& data_transfer_status_enum) {
    os << conversions::data_transfer_status_enum_to_string(data_transfer_status_enum);
    return os;
}

// from: DeleteCertificateResponse
namespace conversions {
std::string delete_certificate_status_enum_to_string(DeleteCertificateStatusEnum e) {
    switch (e) {
    case DeleteCertificateStatusEnum::Accepted:
        return "Accepted";
    case DeleteCertificateStatusEnum::Failed:
        return "Failed";
    case DeleteCertificateStatusEnum::NotFound:
        return "NotFound";
    }

    throw EnumToStringException{e, "DeleteCertificateStatusEnum"};
}

DeleteCertificateStatusEnum string_to_delete_certificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return DeleteCertificateStatusEnum::Accepted;
    }
    if (s == "Failed") {
        return DeleteCertificateStatusEnum::Failed;
    }
    if (s == "NotFound") {
        return DeleteCertificateStatusEnum::NotFound;
    }

    throw StringToEnumException{s, "DeleteCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DeleteCertificateStatusEnum& delete_certificate_status_enum) {
    os << conversions::delete_certificate_status_enum_to_string(delete_certificate_status_enum);
    return os;
}

// from: FirmwareStatusNotificationRequest
namespace conversions {
std::string firmware_status_enum_to_string(FirmwareStatusEnum e) {
    switch (e) {
    case FirmwareStatusEnum::Downloaded:
        return "Downloaded";
    case FirmwareStatusEnum::DownloadFailed:
        return "DownloadFailed";
    case FirmwareStatusEnum::Downloading:
        return "Downloading";
    case FirmwareStatusEnum::DownloadScheduled:
        return "DownloadScheduled";
    case FirmwareStatusEnum::DownloadPaused:
        return "DownloadPaused";
    case FirmwareStatusEnum::Idle:
        return "Idle";
    case FirmwareStatusEnum::InstallationFailed:
        return "InstallationFailed";
    case FirmwareStatusEnum::Installing:
        return "Installing";
    case FirmwareStatusEnum::Installed:
        return "Installed";
    case FirmwareStatusEnum::InstallRebooting:
        return "InstallRebooting";
    case FirmwareStatusEnum::InstallScheduled:
        return "InstallScheduled";
    case FirmwareStatusEnum::InstallVerificationFailed:
        return "InstallVerificationFailed";
    case FirmwareStatusEnum::InvalidSignature:
        return "InvalidSignature";
    case FirmwareStatusEnum::SignatureVerified:
        return "SignatureVerified";
    }

    throw EnumToStringException{e, "FirmwareStatusEnum"};
}

FirmwareStatusEnum string_to_firmware_status_enum(const std::string& s) {
    if (s == "Downloaded") {
        return FirmwareStatusEnum::Downloaded;
    }
    if (s == "DownloadFailed") {
        return FirmwareStatusEnum::DownloadFailed;
    }
    if (s == "Downloading") {
        return FirmwareStatusEnum::Downloading;
    }
    if (s == "DownloadScheduled") {
        return FirmwareStatusEnum::DownloadScheduled;
    }
    if (s == "DownloadPaused") {
        return FirmwareStatusEnum::DownloadPaused;
    }
    if (s == "Idle") {
        return FirmwareStatusEnum::Idle;
    }
    if (s == "InstallationFailed") {
        return FirmwareStatusEnum::InstallationFailed;
    }
    if (s == "Installing") {
        return FirmwareStatusEnum::Installing;
    }
    if (s == "Installed") {
        return FirmwareStatusEnum::Installed;
    }
    if (s == "InstallRebooting") {
        return FirmwareStatusEnum::InstallRebooting;
    }
    if (s == "InstallScheduled") {
        return FirmwareStatusEnum::InstallScheduled;
    }
    if (s == "InstallVerificationFailed") {
        return FirmwareStatusEnum::InstallVerificationFailed;
    }
    if (s == "InvalidSignature") {
        return FirmwareStatusEnum::InvalidSignature;
    }
    if (s == "SignatureVerified") {
        return FirmwareStatusEnum::SignatureVerified;
    }

    throw StringToEnumException{s, "FirmwareStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const FirmwareStatusEnum& firmware_status_enum) {
    os << conversions::firmware_status_enum_to_string(firmware_status_enum);
    return os;
}

// from: Get15118EVCertificateRequest
namespace conversions {
std::string certificate_action_enum_to_string(CertificateActionEnum e) {
    switch (e) {
    case CertificateActionEnum::Install:
        return "Install";
    case CertificateActionEnum::Update:
        return "Update";
    }

    throw EnumToStringException{e, "CertificateActionEnum"};
}

CertificateActionEnum string_to_certificate_action_enum(const std::string& s) {
    if (s == "Install") {
        return CertificateActionEnum::Install;
    }
    if (s == "Update") {
        return CertificateActionEnum::Update;
    }

    throw StringToEnumException{s, "CertificateActionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateActionEnum& certificate_action_enum) {
    os << conversions::certificate_action_enum_to_string(certificate_action_enum);
    return os;
}

// from: Get15118EVCertificateResponse
namespace conversions {
std::string iso15118evcertificate_status_enum_to_string(Iso15118EVCertificateStatusEnum e) {
    switch (e) {
    case Iso15118EVCertificateStatusEnum::Accepted:
        return "Accepted";
    case Iso15118EVCertificateStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "Iso15118EVCertificateStatusEnum"};
}

Iso15118EVCertificateStatusEnum string_to_iso15118evcertificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return Iso15118EVCertificateStatusEnum::Accepted;
    }
    if (s == "Failed") {
        return Iso15118EVCertificateStatusEnum::Failed;
    }

    throw StringToEnumException{s, "Iso15118EVCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const Iso15118EVCertificateStatusEnum& iso15118evcertificate_status_enum) {
    os << conversions::iso15118evcertificate_status_enum_to_string(iso15118evcertificate_status_enum);
    return os;
}

// from: GetBaseReportRequest
namespace conversions {
std::string report_base_enum_to_string(ReportBaseEnum e) {
    switch (e) {
    case ReportBaseEnum::ConfigurationInventory:
        return "ConfigurationInventory";
    case ReportBaseEnum::FullInventory:
        return "FullInventory";
    case ReportBaseEnum::SummaryInventory:
        return "SummaryInventory";
    }

    throw EnumToStringException{e, "ReportBaseEnum"};
}

ReportBaseEnum string_to_report_base_enum(const std::string& s) {
    if (s == "ConfigurationInventory") {
        return ReportBaseEnum::ConfigurationInventory;
    }
    if (s == "FullInventory") {
        return ReportBaseEnum::FullInventory;
    }
    if (s == "SummaryInventory") {
        return ReportBaseEnum::SummaryInventory;
    }

    throw StringToEnumException{s, "ReportBaseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ReportBaseEnum& report_base_enum) {
    os << conversions::report_base_enum_to_string(report_base_enum);
    return os;
}

// from: GetBaseReportResponse
namespace conversions {
std::string generic_device_model_status_enum_to_string(GenericDeviceModelStatusEnum e) {
    switch (e) {
    case GenericDeviceModelStatusEnum::Accepted:
        return "Accepted";
    case GenericDeviceModelStatusEnum::Rejected:
        return "Rejected";
    case GenericDeviceModelStatusEnum::NotSupported:
        return "NotSupported";
    case GenericDeviceModelStatusEnum::EmptyResultSet:
        return "EmptyResultSet";
    }

    throw EnumToStringException{e, "GenericDeviceModelStatusEnum"};
}

GenericDeviceModelStatusEnum string_to_generic_device_model_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GenericDeviceModelStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return GenericDeviceModelStatusEnum::Rejected;
    }
    if (s == "NotSupported") {
        return GenericDeviceModelStatusEnum::NotSupported;
    }
    if (s == "EmptyResultSet") {
        return GenericDeviceModelStatusEnum::EmptyResultSet;
    }

    throw StringToEnumException{s, "GenericDeviceModelStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GenericDeviceModelStatusEnum& generic_device_model_status_enum) {
    os << conversions::generic_device_model_status_enum_to_string(generic_device_model_status_enum);
    return os;
}

// from: GetCertificateChainStatusRequest
namespace conversions {
std::string certificate_status_source_enum_to_string(CertificateStatusSourceEnum e) {
    switch (e) {
    case CertificateStatusSourceEnum::CRL:
        return "CRL";
    case CertificateStatusSourceEnum::OCSP:
        return "OCSP";
    }

    throw EnumToStringException{e, "CertificateStatusSourceEnum"};
}

CertificateStatusSourceEnum string_to_certificate_status_source_enum(const std::string& s) {
    if (s == "CRL") {
        return CertificateStatusSourceEnum::CRL;
    }
    if (s == "OCSP") {
        return CertificateStatusSourceEnum::OCSP;
    }

    throw StringToEnumException{s, "CertificateStatusSourceEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateStatusSourceEnum& certificate_status_source_enum) {
    os << conversions::certificate_status_source_enum_to_string(certificate_status_source_enum);
    return os;
}

// from: GetCertificateChainStatusResponse
namespace conversions {
std::string certificate_status_enum_to_string(CertificateStatusEnum e) {
    switch (e) {
    case CertificateStatusEnum::Good:
        return "Good";
    case CertificateStatusEnum::Revoked:
        return "Revoked";
    case CertificateStatusEnum::Unknown:
        return "Unknown";
    case CertificateStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "CertificateStatusEnum"};
}

CertificateStatusEnum string_to_certificate_status_enum(const std::string& s) {
    if (s == "Good") {
        return CertificateStatusEnum::Good;
    }
    if (s == "Revoked") {
        return CertificateStatusEnum::Revoked;
    }
    if (s == "Unknown") {
        return CertificateStatusEnum::Unknown;
    }
    if (s == "Failed") {
        return CertificateStatusEnum::Failed;
    }

    throw StringToEnumException{s, "CertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateStatusEnum& certificate_status_enum) {
    os << conversions::certificate_status_enum_to_string(certificate_status_enum);
    return os;
}

// from: GetCertificateStatusResponse
namespace conversions {
std::string get_certificate_status_enum_to_string(GetCertificateStatusEnum e) {
    switch (e) {
    case GetCertificateStatusEnum::Accepted:
        return "Accepted";
    case GetCertificateStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "GetCertificateStatusEnum"};
}

GetCertificateStatusEnum string_to_get_certificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GetCertificateStatusEnum::Accepted;
    }
    if (s == "Failed") {
        return GetCertificateStatusEnum::Failed;
    }

    throw StringToEnumException{s, "GetCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetCertificateStatusEnum& get_certificate_status_enum) {
    os << conversions::get_certificate_status_enum_to_string(get_certificate_status_enum);
    return os;
}

// from: GetChargingProfilesResponse
namespace conversions {
std::string get_charging_profile_status_enum_to_string(GetChargingProfileStatusEnum e) {
    switch (e) {
    case GetChargingProfileStatusEnum::Accepted:
        return "Accepted";
    case GetChargingProfileStatusEnum::NoProfiles:
        return "NoProfiles";
    }

    throw EnumToStringException{e, "GetChargingProfileStatusEnum"};
}

GetChargingProfileStatusEnum string_to_get_charging_profile_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GetChargingProfileStatusEnum::Accepted;
    }
    if (s == "NoProfiles") {
        return GetChargingProfileStatusEnum::NoProfiles;
    }

    throw StringToEnumException{s, "GetChargingProfileStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetChargingProfileStatusEnum& get_charging_profile_status_enum) {
    os << conversions::get_charging_profile_status_enum_to_string(get_charging_profile_status_enum);
    return os;
}

// from: GetCompositeScheduleRequest
namespace conversions {
std::string charging_rate_unit_enum_to_string(ChargingRateUnitEnum e) {
    switch (e) {
    case ChargingRateUnitEnum::W:
        return "W";
    case ChargingRateUnitEnum::A:
        return "A";
    }

    throw EnumToStringException{e, "ChargingRateUnitEnum"};
}

ChargingRateUnitEnum string_to_charging_rate_unit_enum(const std::string& s) {
    if (s == "W") {
        return ChargingRateUnitEnum::W;
    }
    if (s == "A") {
        return ChargingRateUnitEnum::A;
    }

    throw StringToEnumException{s, "ChargingRateUnitEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargingRateUnitEnum& charging_rate_unit_enum) {
    os << conversions::charging_rate_unit_enum_to_string(charging_rate_unit_enum);
    return os;
}

// from: GetCompositeScheduleResponse
namespace conversions {
std::string operation_mode_enum_to_string(OperationModeEnum e) {
    switch (e) {
    case OperationModeEnum::Idle:
        return "Idle";
    case OperationModeEnum::ChargingOnly:
        return "ChargingOnly";
    case OperationModeEnum::CentralSetpoint:
        return "CentralSetpoint";
    case OperationModeEnum::ExternalSetpoint:
        return "ExternalSetpoint";
    case OperationModeEnum::ExternalLimits:
        return "ExternalLimits";
    case OperationModeEnum::CentralFrequency:
        return "CentralFrequency";
    case OperationModeEnum::LocalFrequency:
        return "LocalFrequency";
    case OperationModeEnum::LocalLoadBalancing:
        return "LocalLoadBalancing";
    }

    throw EnumToStringException{e, "OperationModeEnum"};
}

OperationModeEnum string_to_operation_mode_enum(const std::string& s) {
    if (s == "Idle") {
        return OperationModeEnum::Idle;
    }
    if (s == "ChargingOnly") {
        return OperationModeEnum::ChargingOnly;
    }
    if (s == "CentralSetpoint") {
        return OperationModeEnum::CentralSetpoint;
    }
    if (s == "ExternalSetpoint") {
        return OperationModeEnum::ExternalSetpoint;
    }
    if (s == "ExternalLimits") {
        return OperationModeEnum::ExternalLimits;
    }
    if (s == "CentralFrequency") {
        return OperationModeEnum::CentralFrequency;
    }
    if (s == "LocalFrequency") {
        return OperationModeEnum::LocalFrequency;
    }
    if (s == "LocalLoadBalancing") {
        return OperationModeEnum::LocalLoadBalancing;
    }

    throw StringToEnumException{s, "OperationModeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OperationModeEnum& operation_mode_enum) {
    os << conversions::operation_mode_enum_to_string(operation_mode_enum);
    return os;
}

// from: GetDisplayMessagesRequest
namespace conversions {
std::string message_priority_enum_to_string(MessagePriorityEnum e) {
    switch (e) {
    case MessagePriorityEnum::AlwaysFront:
        return "AlwaysFront";
    case MessagePriorityEnum::InFront:
        return "InFront";
    case MessagePriorityEnum::NormalCycle:
        return "NormalCycle";
    }

    throw EnumToStringException{e, "MessagePriorityEnum"};
}

MessagePriorityEnum string_to_message_priority_enum(const std::string& s) {
    if (s == "AlwaysFront") {
        return MessagePriorityEnum::AlwaysFront;
    }
    if (s == "InFront") {
        return MessagePriorityEnum::InFront;
    }
    if (s == "NormalCycle") {
        return MessagePriorityEnum::NormalCycle;
    }

    throw StringToEnumException{s, "MessagePriorityEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessagePriorityEnum& message_priority_enum) {
    os << conversions::message_priority_enum_to_string(message_priority_enum);
    return os;
}

// from: GetDisplayMessagesRequest
namespace conversions {
std::string message_state_enum_to_string(MessageStateEnum e) {
    switch (e) {
    case MessageStateEnum::Charging:
        return "Charging";
    case MessageStateEnum::Faulted:
        return "Faulted";
    case MessageStateEnum::Idle:
        return "Idle";
    case MessageStateEnum::Unavailable:
        return "Unavailable";
    case MessageStateEnum::Suspended:
        return "Suspended";
    case MessageStateEnum::Discharging:
        return "Discharging";
    }

    throw EnumToStringException{e, "MessageStateEnum"};
}

MessageStateEnum string_to_message_state_enum(const std::string& s) {
    if (s == "Charging") {
        return MessageStateEnum::Charging;
    }
    if (s == "Faulted") {
        return MessageStateEnum::Faulted;
    }
    if (s == "Idle") {
        return MessageStateEnum::Idle;
    }
    if (s == "Unavailable") {
        return MessageStateEnum::Unavailable;
    }
    if (s == "Suspended") {
        return MessageStateEnum::Suspended;
    }
    if (s == "Discharging") {
        return MessageStateEnum::Discharging;
    }

    throw StringToEnumException{s, "MessageStateEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageStateEnum& message_state_enum) {
    os << conversions::message_state_enum_to_string(message_state_enum);
    return os;
}

// from: GetDisplayMessagesResponse
namespace conversions {
std::string get_display_messages_status_enum_to_string(GetDisplayMessagesStatusEnum e) {
    switch (e) {
    case GetDisplayMessagesStatusEnum::Accepted:
        return "Accepted";
    case GetDisplayMessagesStatusEnum::Unknown:
        return "Unknown";
    }

    throw EnumToStringException{e, "GetDisplayMessagesStatusEnum"};
}

GetDisplayMessagesStatusEnum string_to_get_display_messages_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GetDisplayMessagesStatusEnum::Accepted;
    }
    if (s == "Unknown") {
        return GetDisplayMessagesStatusEnum::Unknown;
    }

    throw StringToEnumException{s, "GetDisplayMessagesStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetDisplayMessagesStatusEnum& get_display_messages_status_enum) {
    os << conversions::get_display_messages_status_enum_to_string(get_display_messages_status_enum);
    return os;
}

// from: GetInstalledCertificateIdsRequest
namespace conversions {
std::string get_certificate_id_use_enum_to_string(GetCertificateIdUseEnum e) {
    switch (e) {
    case GetCertificateIdUseEnum::V2GRootCertificate:
        return "V2GRootCertificate";
    case GetCertificateIdUseEnum::MORootCertificate:
        return "MORootCertificate";
    case GetCertificateIdUseEnum::CSMSRootCertificate:
        return "CSMSRootCertificate";
    case GetCertificateIdUseEnum::V2GCertificateChain:
        return "V2GCertificateChain";
    case GetCertificateIdUseEnum::ManufacturerRootCertificate:
        return "ManufacturerRootCertificate";
    case GetCertificateIdUseEnum::OEMRootCertificate:
        return "OEMRootCertificate";
    }

    throw EnumToStringException{e, "GetCertificateIdUseEnum"};
}

GetCertificateIdUseEnum string_to_get_certificate_id_use_enum(const std::string& s) {
    if (s == "V2GRootCertificate") {
        return GetCertificateIdUseEnum::V2GRootCertificate;
    }
    if (s == "MORootCertificate") {
        return GetCertificateIdUseEnum::MORootCertificate;
    }
    if (s == "CSMSRootCertificate") {
        return GetCertificateIdUseEnum::CSMSRootCertificate;
    }
    if (s == "V2GCertificateChain") {
        return GetCertificateIdUseEnum::V2GCertificateChain;
    }
    if (s == "ManufacturerRootCertificate") {
        return GetCertificateIdUseEnum::ManufacturerRootCertificate;
    }
    if (s == "OEMRootCertificate") {
        return GetCertificateIdUseEnum::OEMRootCertificate;
    }

    throw StringToEnumException{s, "GetCertificateIdUseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetCertificateIdUseEnum& get_certificate_id_use_enum) {
    os << conversions::get_certificate_id_use_enum_to_string(get_certificate_id_use_enum);
    return os;
}

// from: GetInstalledCertificateIdsResponse
namespace conversions {
std::string get_installed_certificate_status_enum_to_string(GetInstalledCertificateStatusEnum e) {
    switch (e) {
    case GetInstalledCertificateStatusEnum::Accepted:
        return "Accepted";
    case GetInstalledCertificateStatusEnum::NotFound:
        return "NotFound";
    }

    throw EnumToStringException{e, "GetInstalledCertificateStatusEnum"};
}

GetInstalledCertificateStatusEnum string_to_get_installed_certificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GetInstalledCertificateStatusEnum::Accepted;
    }
    if (s == "NotFound") {
        return GetInstalledCertificateStatusEnum::NotFound;
    }

    throw StringToEnumException{s, "GetInstalledCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os,
                         const GetInstalledCertificateStatusEnum& get_installed_certificate_status_enum) {
    os << conversions::get_installed_certificate_status_enum_to_string(get_installed_certificate_status_enum);
    return os;
}

// from: GetLogRequest
namespace conversions {
std::string log_enum_to_string(LogEnum e) {
    switch (e) {
    case LogEnum::DiagnosticsLog:
        return "DiagnosticsLog";
    case LogEnum::SecurityLog:
        return "SecurityLog";
    case LogEnum::DataCollectorLog:
        return "DataCollectorLog";
    }

    throw EnumToStringException{e, "LogEnum"};
}

LogEnum string_to_log_enum(const std::string& s) {
    if (s == "DiagnosticsLog") {
        return LogEnum::DiagnosticsLog;
    }
    if (s == "SecurityLog") {
        return LogEnum::SecurityLog;
    }
    if (s == "DataCollectorLog") {
        return LogEnum::DataCollectorLog;
    }

    throw StringToEnumException{s, "LogEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const LogEnum& log_enum) {
    os << conversions::log_enum_to_string(log_enum);
    return os;
}

// from: GetLogResponse
namespace conversions {
std::string log_status_enum_to_string(LogStatusEnum e) {
    switch (e) {
    case LogStatusEnum::Accepted:
        return "Accepted";
    case LogStatusEnum::Rejected:
        return "Rejected";
    case LogStatusEnum::AcceptedCanceled:
        return "AcceptedCanceled";
    }

    throw EnumToStringException{e, "LogStatusEnum"};
}

LogStatusEnum string_to_log_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return LogStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return LogStatusEnum::Rejected;
    }
    if (s == "AcceptedCanceled") {
        return LogStatusEnum::AcceptedCanceled;
    }

    throw StringToEnumException{s, "LogStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const LogStatusEnum& log_status_enum) {
    os << conversions::log_status_enum_to_string(log_status_enum);
    return os;
}

// from: GetMonitoringReportRequest
namespace conversions {
std::string monitoring_criterion_enum_to_string(MonitoringCriterionEnum e) {
    switch (e) {
    case MonitoringCriterionEnum::ThresholdMonitoring:
        return "ThresholdMonitoring";
    case MonitoringCriterionEnum::DeltaMonitoring:
        return "DeltaMonitoring";
    case MonitoringCriterionEnum::PeriodicMonitoring:
        return "PeriodicMonitoring";
    }

    throw EnumToStringException{e, "MonitoringCriterionEnum"};
}

MonitoringCriterionEnum string_to_monitoring_criterion_enum(const std::string& s) {
    if (s == "ThresholdMonitoring") {
        return MonitoringCriterionEnum::ThresholdMonitoring;
    }
    if (s == "DeltaMonitoring") {
        return MonitoringCriterionEnum::DeltaMonitoring;
    }
    if (s == "PeriodicMonitoring") {
        return MonitoringCriterionEnum::PeriodicMonitoring;
    }

    throw StringToEnumException{s, "MonitoringCriterionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MonitoringCriterionEnum& monitoring_criterion_enum) {
    os << conversions::monitoring_criterion_enum_to_string(monitoring_criterion_enum);
    return os;
}

// from: GetReportRequest
namespace conversions {
std::string component_criterion_enum_to_string(ComponentCriterionEnum e) {
    switch (e) {
    case ComponentCriterionEnum::Active:
        return "Active";
    case ComponentCriterionEnum::Available:
        return "Available";
    case ComponentCriterionEnum::Enabled:
        return "Enabled";
    case ComponentCriterionEnum::Problem:
        return "Problem";
    }

    throw EnumToStringException{e, "ComponentCriterionEnum"};
}

ComponentCriterionEnum string_to_component_criterion_enum(const std::string& s) {
    if (s == "Active") {
        return ComponentCriterionEnum::Active;
    }
    if (s == "Available") {
        return ComponentCriterionEnum::Available;
    }
    if (s == "Enabled") {
        return ComponentCriterionEnum::Enabled;
    }
    if (s == "Problem") {
        return ComponentCriterionEnum::Problem;
    }

    throw StringToEnumException{s, "ComponentCriterionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ComponentCriterionEnum& component_criterion_enum) {
    os << conversions::component_criterion_enum_to_string(component_criterion_enum);
    return os;
}

// from: GetTariffsResponse
namespace conversions {
std::string tariff_get_status_enum_to_string(TariffGetStatusEnum e) {
    switch (e) {
    case TariffGetStatusEnum::Accepted:
        return "Accepted";
    case TariffGetStatusEnum::Rejected:
        return "Rejected";
    case TariffGetStatusEnum::NoTariff:
        return "NoTariff";
    }

    throw EnumToStringException{e, "TariffGetStatusEnum"};
}

TariffGetStatusEnum string_to_tariff_get_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return TariffGetStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return TariffGetStatusEnum::Rejected;
    }
    if (s == "NoTariff") {
        return TariffGetStatusEnum::NoTariff;
    }

    throw StringToEnumException{s, "TariffGetStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffGetStatusEnum& tariff_get_status_enum) {
    os << conversions::tariff_get_status_enum_to_string(tariff_get_status_enum);
    return os;
}

// from: GetTariffsResponse
namespace conversions {
std::string tariff_kind_enum_to_string(TariffKindEnum e) {
    switch (e) {
    case TariffKindEnum::DefaultTariff:
        return "DefaultTariff";
    case TariffKindEnum::DriverTariff:
        return "DriverTariff";
    }

    throw EnumToStringException{e, "TariffKindEnum"};
}

TariffKindEnum string_to_tariff_kind_enum(const std::string& s) {
    if (s == "DefaultTariff") {
        return TariffKindEnum::DefaultTariff;
    }
    if (s == "DriverTariff") {
        return TariffKindEnum::DriverTariff;
    }

    throw StringToEnumException{s, "TariffKindEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffKindEnum& tariff_kind_enum) {
    os << conversions::tariff_kind_enum_to_string(tariff_kind_enum);
    return os;
}

// from: GetVariablesRequest
namespace conversions {
std::string attribute_enum_to_string(AttributeEnum e) {
    switch (e) {
    case AttributeEnum::Actual:
        return "Actual";
    case AttributeEnum::Target:
        return "Target";
    case AttributeEnum::MinSet:
        return "MinSet";
    case AttributeEnum::MaxSet:
        return "MaxSet";
    }

    throw EnumToStringException{e, "AttributeEnum"};
}

AttributeEnum string_to_attribute_enum(const std::string& s) {
    if (s == "Actual") {
        return AttributeEnum::Actual;
    }
    if (s == "Target") {
        return AttributeEnum::Target;
    }
    if (s == "MinSet") {
        return AttributeEnum::MinSet;
    }
    if (s == "MaxSet") {
        return AttributeEnum::MaxSet;
    }

    throw StringToEnumException{s, "AttributeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const AttributeEnum& attribute_enum) {
    os << conversions::attribute_enum_to_string(attribute_enum);
    return os;
}

// from: GetVariablesResponse
namespace conversions {
std::string get_variable_status_enum_to_string(GetVariableStatusEnum e) {
    switch (e) {
    case GetVariableStatusEnum::Accepted:
        return "Accepted";
    case GetVariableStatusEnum::Rejected:
        return "Rejected";
    case GetVariableStatusEnum::UnknownComponent:
        return "UnknownComponent";
    case GetVariableStatusEnum::UnknownVariable:
        return "UnknownVariable";
    case GetVariableStatusEnum::NotSupportedAttributeType:
        return "NotSupportedAttributeType";
    }

    throw EnumToStringException{e, "GetVariableStatusEnum"};
}

GetVariableStatusEnum string_to_get_variable_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return GetVariableStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return GetVariableStatusEnum::Rejected;
    }
    if (s == "UnknownComponent") {
        return GetVariableStatusEnum::UnknownComponent;
    }
    if (s == "UnknownVariable") {
        return GetVariableStatusEnum::UnknownVariable;
    }
    if (s == "NotSupportedAttributeType") {
        return GetVariableStatusEnum::NotSupportedAttributeType;
    }

    throw StringToEnumException{s, "GetVariableStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetVariableStatusEnum& get_variable_status_enum) {
    os << conversions::get_variable_status_enum_to_string(get_variable_status_enum);
    return os;
}

// from: InstallCertificateRequest
namespace conversions {
std::string install_certificate_use_enum_to_string(InstallCertificateUseEnum e) {
    switch (e) {
    case InstallCertificateUseEnum::V2GRootCertificate:
        return "V2GRootCertificate";
    case InstallCertificateUseEnum::MORootCertificate:
        return "MORootCertificate";
    case InstallCertificateUseEnum::ManufacturerRootCertificate:
        return "ManufacturerRootCertificate";
    case InstallCertificateUseEnum::CSMSRootCertificate:
        return "CSMSRootCertificate";
    case InstallCertificateUseEnum::OEMRootCertificate:
        return "OEMRootCertificate";
    }

    throw EnumToStringException{e, "InstallCertificateUseEnum"};
}

InstallCertificateUseEnum string_to_install_certificate_use_enum(const std::string& s) {
    if (s == "V2GRootCertificate") {
        return InstallCertificateUseEnum::V2GRootCertificate;
    }
    if (s == "MORootCertificate") {
        return InstallCertificateUseEnum::MORootCertificate;
    }
    if (s == "ManufacturerRootCertificate") {
        return InstallCertificateUseEnum::ManufacturerRootCertificate;
    }
    if (s == "CSMSRootCertificate") {
        return InstallCertificateUseEnum::CSMSRootCertificate;
    }
    if (s == "OEMRootCertificate") {
        return InstallCertificateUseEnum::OEMRootCertificate;
    }

    throw StringToEnumException{s, "InstallCertificateUseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const InstallCertificateUseEnum& install_certificate_use_enum) {
    os << conversions::install_certificate_use_enum_to_string(install_certificate_use_enum);
    return os;
}

// from: InstallCertificateResponse
namespace conversions {
std::string install_certificate_status_enum_to_string(InstallCertificateStatusEnum e) {
    switch (e) {
    case InstallCertificateStatusEnum::Accepted:
        return "Accepted";
    case InstallCertificateStatusEnum::Rejected:
        return "Rejected";
    case InstallCertificateStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "InstallCertificateStatusEnum"};
}

InstallCertificateStatusEnum string_to_install_certificate_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return InstallCertificateStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return InstallCertificateStatusEnum::Rejected;
    }
    if (s == "Failed") {
        return InstallCertificateStatusEnum::Failed;
    }

    throw StringToEnumException{s, "InstallCertificateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const InstallCertificateStatusEnum& install_certificate_status_enum) {
    os << conversions::install_certificate_status_enum_to_string(install_certificate_status_enum);
    return os;
}

// from: LogStatusNotificationRequest
namespace conversions {
std::string upload_log_status_enum_to_string(UploadLogStatusEnum e) {
    switch (e) {
    case UploadLogStatusEnum::BadMessage:
        return "BadMessage";
    case UploadLogStatusEnum::Idle:
        return "Idle";
    case UploadLogStatusEnum::NotSupportedOperation:
        return "NotSupportedOperation";
    case UploadLogStatusEnum::PermissionDenied:
        return "PermissionDenied";
    case UploadLogStatusEnum::Uploaded:
        return "Uploaded";
    case UploadLogStatusEnum::UploadFailure:
        return "UploadFailure";
    case UploadLogStatusEnum::Uploading:
        return "Uploading";
    case UploadLogStatusEnum::AcceptedCanceled:
        return "AcceptedCanceled";
    }

    throw EnumToStringException{e, "UploadLogStatusEnum"};
}

UploadLogStatusEnum string_to_upload_log_status_enum(const std::string& s) {
    if (s == "BadMessage") {
        return UploadLogStatusEnum::BadMessage;
    }
    if (s == "Idle") {
        return UploadLogStatusEnum::Idle;
    }
    if (s == "NotSupportedOperation") {
        return UploadLogStatusEnum::NotSupportedOperation;
    }
    if (s == "PermissionDenied") {
        return UploadLogStatusEnum::PermissionDenied;
    }
    if (s == "Uploaded") {
        return UploadLogStatusEnum::Uploaded;
    }
    if (s == "UploadFailure") {
        return UploadLogStatusEnum::UploadFailure;
    }
    if (s == "Uploading") {
        return UploadLogStatusEnum::Uploading;
    }
    if (s == "AcceptedCanceled") {
        return UploadLogStatusEnum::AcceptedCanceled;
    }

    throw StringToEnumException{s, "UploadLogStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const UploadLogStatusEnum& upload_log_status_enum) {
    os << conversions::upload_log_status_enum_to_string(upload_log_status_enum);
    return os;
}

// from: MeterValuesRequest
namespace conversions {
std::string measurand_enum_to_string(MeasurandEnum e) {
    switch (e) {
    case MeasurandEnum::Current_Export:
        return "Current.Export";
    case MeasurandEnum::Current_Export_Offered:
        return "Current.Export.Offered";
    case MeasurandEnum::Current_Export_Minimum:
        return "Current.Export.Minimum";
    case MeasurandEnum::Current_Import:
        return "Current.Import";
    case MeasurandEnum::Current_Import_Offered:
        return "Current.Import.Offered";
    case MeasurandEnum::Current_Import_Minimum:
        return "Current.Import.Minimum";
    case MeasurandEnum::Current_Offered:
        return "Current.Offered";
    case MeasurandEnum::Display_PresentSOC:
        return "Display.PresentSOC";
    case MeasurandEnum::Display_MinimumSOC:
        return "Display.MinimumSOC";
    case MeasurandEnum::Display_TargetSOC:
        return "Display.TargetSOC";
    case MeasurandEnum::Display_MaximumSOC:
        return "Display.MaximumSOC";
    case MeasurandEnum::Display_RemainingTimeToMinimumSOC:
        return "Display.RemainingTimeToMinimumSOC";
    case MeasurandEnum::Display_RemainingTimeToTargetSOC:
        return "Display.RemainingTimeToTargetSOC";
    case MeasurandEnum::Display_RemainingTimeToMaximumSOC:
        return "Display.RemainingTimeToMaximumSOC";
    case MeasurandEnum::Display_ChargingComplete:
        return "Display.ChargingComplete";
    case MeasurandEnum::Display_BatteryEnergyCapacity:
        return "Display.BatteryEnergyCapacity";
    case MeasurandEnum::Display_InletHot:
        return "Display.InletHot";
    case MeasurandEnum::Energy_Active_Export_Interval:
        return "Energy.Active.Export.Interval";
    case MeasurandEnum::Energy_Active_Export_Register:
        return "Energy.Active.Export.Register";
    case MeasurandEnum::Energy_Active_Import_Interval:
        return "Energy.Active.Import.Interval";
    case MeasurandEnum::Energy_Active_Import_Register:
        return "Energy.Active.Import.Register";
    case MeasurandEnum::Energy_Active_Import_CableLoss:
        return "Energy.Active.Import.CableLoss";
    case MeasurandEnum::Energy_Active_Import_LocalGeneration_Register:
        return "Energy.Active.Import.LocalGeneration.Register";
    case MeasurandEnum::Energy_Active_Net:
        return "Energy.Active.Net";
    case MeasurandEnum::Energy_Active_Setpoint_Interval:
        return "Energy.Active.Setpoint.Interval";
    case MeasurandEnum::Energy_Apparent_Export:
        return "Energy.Apparent.Export";
    case MeasurandEnum::Energy_Apparent_Import:
        return "Energy.Apparent.Import";
    case MeasurandEnum::Energy_Apparent_Net:
        return "Energy.Apparent.Net";
    case MeasurandEnum::Energy_Reactive_Export_Interval:
        return "Energy.Reactive.Export.Interval";
    case MeasurandEnum::Energy_Reactive_Export_Register:
        return "Energy.Reactive.Export.Register";
    case MeasurandEnum::Energy_Reactive_Import_Interval:
        return "Energy.Reactive.Import.Interval";
    case MeasurandEnum::Energy_Reactive_Import_Register:
        return "Energy.Reactive.Import.Register";
    case MeasurandEnum::Energy_Reactive_Net:
        return "Energy.Reactive.Net";
    case MeasurandEnum::EnergyRequest_Target:
        return "EnergyRequest.Target";
    case MeasurandEnum::EnergyRequest_Minimum:
        return "EnergyRequest.Minimum";
    case MeasurandEnum::EnergyRequest_Maximum:
        return "EnergyRequest.Maximum";
    case MeasurandEnum::EnergyRequest_Minimum_V2X:
        return "EnergyRequest.Minimum.V2X";
    case MeasurandEnum::EnergyRequest_Maximum_V2X:
        return "EnergyRequest.Maximum.V2X";
    case MeasurandEnum::EnergyRequest_Bulk:
        return "EnergyRequest.Bulk";
    case MeasurandEnum::Frequency:
        return "Frequency";
    case MeasurandEnum::Power_Active_Export:
        return "Power.Active.Export";
    case MeasurandEnum::Power_Active_Import:
        return "Power.Active.Import";
    case MeasurandEnum::Power_Active_Setpoint:
        return "Power.Active.Setpoint";
    case MeasurandEnum::Power_Active_Residual:
        return "Power.Active.Residual";
    case MeasurandEnum::Power_Export_Minimum:
        return "Power.Export.Minimum";
    case MeasurandEnum::Power_Export_Offered:
        return "Power.Export.Offered";
    case MeasurandEnum::Power_Factor:
        return "Power.Factor";
    case MeasurandEnum::Power_Import_Offered:
        return "Power.Import.Offered";
    case MeasurandEnum::Power_Import_Minimum:
        return "Power.Import.Minimum";
    case MeasurandEnum::Power_Offered:
        return "Power.Offered";
    case MeasurandEnum::Power_Reactive_Export:
        return "Power.Reactive.Export";
    case MeasurandEnum::Power_Reactive_Import:
        return "Power.Reactive.Import";
    case MeasurandEnum::SoC:
        return "SoC";
    case MeasurandEnum::Voltage:
        return "Voltage";
    case MeasurandEnum::Voltage_Minimum:
        return "Voltage.Minimum";
    case MeasurandEnum::Voltage_Maximum:
        return "Voltage.Maximum";
    }

    throw EnumToStringException{e, "MeasurandEnum"};
}

MeasurandEnum string_to_measurand_enum(const std::string& s) {
    if (s == "Current.Export") {
        return MeasurandEnum::Current_Export;
    }
    if (s == "Current.Export.Offered") {
        return MeasurandEnum::Current_Export_Offered;
    }
    if (s == "Current.Export.Minimum") {
        return MeasurandEnum::Current_Export_Minimum;
    }
    if (s == "Current.Import") {
        return MeasurandEnum::Current_Import;
    }
    if (s == "Current.Import.Offered") {
        return MeasurandEnum::Current_Import_Offered;
    }
    if (s == "Current.Import.Minimum") {
        return MeasurandEnum::Current_Import_Minimum;
    }
    if (s == "Current.Offered") {
        return MeasurandEnum::Current_Offered;
    }
    if (s == "Display.PresentSOC") {
        return MeasurandEnum::Display_PresentSOC;
    }
    if (s == "Display.MinimumSOC") {
        return MeasurandEnum::Display_MinimumSOC;
    }
    if (s == "Display.TargetSOC") {
        return MeasurandEnum::Display_TargetSOC;
    }
    if (s == "Display.MaximumSOC") {
        return MeasurandEnum::Display_MaximumSOC;
    }
    if (s == "Display.RemainingTimeToMinimumSOC") {
        return MeasurandEnum::Display_RemainingTimeToMinimumSOC;
    }
    if (s == "Display.RemainingTimeToTargetSOC") {
        return MeasurandEnum::Display_RemainingTimeToTargetSOC;
    }
    if (s == "Display.RemainingTimeToMaximumSOC") {
        return MeasurandEnum::Display_RemainingTimeToMaximumSOC;
    }
    if (s == "Display.ChargingComplete") {
        return MeasurandEnum::Display_ChargingComplete;
    }
    if (s == "Display.BatteryEnergyCapacity") {
        return MeasurandEnum::Display_BatteryEnergyCapacity;
    }
    if (s == "Display.InletHot") {
        return MeasurandEnum::Display_InletHot;
    }
    if (s == "Energy.Active.Export.Interval") {
        return MeasurandEnum::Energy_Active_Export_Interval;
    }
    if (s == "Energy.Active.Export.Register") {
        return MeasurandEnum::Energy_Active_Export_Register;
    }
    if (s == "Energy.Active.Import.Interval") {
        return MeasurandEnum::Energy_Active_Import_Interval;
    }
    if (s == "Energy.Active.Import.Register") {
        return MeasurandEnum::Energy_Active_Import_Register;
    }
    if (s == "Energy.Active.Import.CableLoss") {
        return MeasurandEnum::Energy_Active_Import_CableLoss;
    }
    if (s == "Energy.Active.Import.LocalGeneration.Register") {
        return MeasurandEnum::Energy_Active_Import_LocalGeneration_Register;
    }
    if (s == "Energy.Active.Net") {
        return MeasurandEnum::Energy_Active_Net;
    }
    if (s == "Energy.Active.Setpoint.Interval") {
        return MeasurandEnum::Energy_Active_Setpoint_Interval;
    }
    if (s == "Energy.Apparent.Export") {
        return MeasurandEnum::Energy_Apparent_Export;
    }
    if (s == "Energy.Apparent.Import") {
        return MeasurandEnum::Energy_Apparent_Import;
    }
    if (s == "Energy.Apparent.Net") {
        return MeasurandEnum::Energy_Apparent_Net;
    }
    if (s == "Energy.Reactive.Export.Interval") {
        return MeasurandEnum::Energy_Reactive_Export_Interval;
    }
    if (s == "Energy.Reactive.Export.Register") {
        return MeasurandEnum::Energy_Reactive_Export_Register;
    }
    if (s == "Energy.Reactive.Import.Interval") {
        return MeasurandEnum::Energy_Reactive_Import_Interval;
    }
    if (s == "Energy.Reactive.Import.Register") {
        return MeasurandEnum::Energy_Reactive_Import_Register;
    }
    if (s == "Energy.Reactive.Net") {
        return MeasurandEnum::Energy_Reactive_Net;
    }
    if (s == "EnergyRequest.Target") {
        return MeasurandEnum::EnergyRequest_Target;
    }
    if (s == "EnergyRequest.Minimum") {
        return MeasurandEnum::EnergyRequest_Minimum;
    }
    if (s == "EnergyRequest.Maximum") {
        return MeasurandEnum::EnergyRequest_Maximum;
    }
    if (s == "EnergyRequest.Minimum.V2X") {
        return MeasurandEnum::EnergyRequest_Minimum_V2X;
    }
    if (s == "EnergyRequest.Maximum.V2X") {
        return MeasurandEnum::EnergyRequest_Maximum_V2X;
    }
    if (s == "EnergyRequest.Bulk") {
        return MeasurandEnum::EnergyRequest_Bulk;
    }
    if (s == "Frequency") {
        return MeasurandEnum::Frequency;
    }
    if (s == "Power.Active.Export") {
        return MeasurandEnum::Power_Active_Export;
    }
    if (s == "Power.Active.Import") {
        return MeasurandEnum::Power_Active_Import;
    }
    if (s == "Power.Active.Setpoint") {
        return MeasurandEnum::Power_Active_Setpoint;
    }
    if (s == "Power.Active.Residual") {
        return MeasurandEnum::Power_Active_Residual;
    }
    if (s == "Power.Export.Minimum") {
        return MeasurandEnum::Power_Export_Minimum;
    }
    if (s == "Power.Export.Offered") {
        return MeasurandEnum::Power_Export_Offered;
    }
    if (s == "Power.Factor") {
        return MeasurandEnum::Power_Factor;
    }
    if (s == "Power.Import.Offered") {
        return MeasurandEnum::Power_Import_Offered;
    }
    if (s == "Power.Import.Minimum") {
        return MeasurandEnum::Power_Import_Minimum;
    }
    if (s == "Power.Offered") {
        return MeasurandEnum::Power_Offered;
    }
    if (s == "Power.Reactive.Export") {
        return MeasurandEnum::Power_Reactive_Export;
    }
    if (s == "Power.Reactive.Import") {
        return MeasurandEnum::Power_Reactive_Import;
    }
    if (s == "SoC") {
        return MeasurandEnum::SoC;
    }
    if (s == "Voltage") {
        return MeasurandEnum::Voltage;
    }
    if (s == "Voltage.Minimum") {
        return MeasurandEnum::Voltage_Minimum;
    }
    if (s == "Voltage.Maximum") {
        return MeasurandEnum::Voltage_Maximum;
    }

    throw StringToEnumException{s, "MeasurandEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MeasurandEnum& measurand_enum) {
    os << conversions::measurand_enum_to_string(measurand_enum);
    return os;
}

// from: MeterValuesRequest
namespace conversions {
std::string reading_context_enum_to_string(ReadingContextEnum e) {
    switch (e) {
    case ReadingContextEnum::Interruption_Begin:
        return "Interruption.Begin";
    case ReadingContextEnum::Interruption_End:
        return "Interruption.End";
    case ReadingContextEnum::Other:
        return "Other";
    case ReadingContextEnum::Sample_Clock:
        return "Sample.Clock";
    case ReadingContextEnum::Sample_Periodic:
        return "Sample.Periodic";
    case ReadingContextEnum::Transaction_Begin:
        return "Transaction.Begin";
    case ReadingContextEnum::Transaction_End:
        return "Transaction.End";
    case ReadingContextEnum::Trigger:
        return "Trigger";
    }

    throw EnumToStringException{e, "ReadingContextEnum"};
}

ReadingContextEnum string_to_reading_context_enum(const std::string& s) {
    if (s == "Interruption.Begin") {
        return ReadingContextEnum::Interruption_Begin;
    }
    if (s == "Interruption.End") {
        return ReadingContextEnum::Interruption_End;
    }
    if (s == "Other") {
        return ReadingContextEnum::Other;
    }
    if (s == "Sample.Clock") {
        return ReadingContextEnum::Sample_Clock;
    }
    if (s == "Sample.Periodic") {
        return ReadingContextEnum::Sample_Periodic;
    }
    if (s == "Transaction.Begin") {
        return ReadingContextEnum::Transaction_Begin;
    }
    if (s == "Transaction.End") {
        return ReadingContextEnum::Transaction_End;
    }
    if (s == "Trigger") {
        return ReadingContextEnum::Trigger;
    }

    throw StringToEnumException{s, "ReadingContextEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ReadingContextEnum& reading_context_enum) {
    os << conversions::reading_context_enum_to_string(reading_context_enum);
    return os;
}

// from: MeterValuesRequest
namespace conversions {
std::string phase_enum_to_string(PhaseEnum e) {
    switch (e) {
    case PhaseEnum::L1:
        return "L1";
    case PhaseEnum::L2:
        return "L2";
    case PhaseEnum::L3:
        return "L3";
    case PhaseEnum::N:
        return "N";
    case PhaseEnum::L1_N:
        return "L1-N";
    case PhaseEnum::L2_N:
        return "L2-N";
    case PhaseEnum::L3_N:
        return "L3-N";
    case PhaseEnum::L1_L2:
        return "L1-L2";
    case PhaseEnum::L2_L3:
        return "L2-L3";
    case PhaseEnum::L3_L1:
        return "L3-L1";
    }

    throw EnumToStringException{e, "PhaseEnum"};
}

PhaseEnum string_to_phase_enum(const std::string& s) {
    if (s == "L1") {
        return PhaseEnum::L1;
    }
    if (s == "L2") {
        return PhaseEnum::L2;
    }
    if (s == "L3") {
        return PhaseEnum::L3;
    }
    if (s == "N") {
        return PhaseEnum::N;
    }
    if (s == "L1-N") {
        return PhaseEnum::L1_N;
    }
    if (s == "L2-N") {
        return PhaseEnum::L2_N;
    }
    if (s == "L3-N") {
        return PhaseEnum::L3_N;
    }
    if (s == "L1-L2") {
        return PhaseEnum::L1_L2;
    }
    if (s == "L2-L3") {
        return PhaseEnum::L2_L3;
    }
    if (s == "L3-L1") {
        return PhaseEnum::L3_L1;
    }

    throw StringToEnumException{s, "PhaseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PhaseEnum& phase_enum) {
    os << conversions::phase_enum_to_string(phase_enum);
    return os;
}

// from: MeterValuesRequest
namespace conversions {
std::string location_enum_to_string(LocationEnum e) {
    switch (e) {
    case LocationEnum::Body:
        return "Body";
    case LocationEnum::Cable:
        return "Cable";
    case LocationEnum::EV:
        return "EV";
    case LocationEnum::Inlet:
        return "Inlet";
    case LocationEnum::Outlet:
        return "Outlet";
    case LocationEnum::Upstream:
        return "Upstream";
    }

    throw EnumToStringException{e, "LocationEnum"};
}

LocationEnum string_to_location_enum(const std::string& s) {
    if (s == "Body") {
        return LocationEnum::Body;
    }
    if (s == "Cable") {
        return LocationEnum::Cable;
    }
    if (s == "EV") {
        return LocationEnum::EV;
    }
    if (s == "Inlet") {
        return LocationEnum::Inlet;
    }
    if (s == "Outlet") {
        return LocationEnum::Outlet;
    }
    if (s == "Upstream") {
        return LocationEnum::Upstream;
    }

    throw StringToEnumException{s, "LocationEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const LocationEnum& location_enum) {
    os << conversions::location_enum_to_string(location_enum);
    return os;
}

// from: NotifyAllowedEnergyTransferResponse
namespace conversions {
std::string notify_allowed_energy_transfer_status_enum_to_string(NotifyAllowedEnergyTransferStatusEnum e) {
    switch (e) {
    case NotifyAllowedEnergyTransferStatusEnum::Accepted:
        return "Accepted";
    case NotifyAllowedEnergyTransferStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "NotifyAllowedEnergyTransferStatusEnum"};
}

NotifyAllowedEnergyTransferStatusEnum string_to_notify_allowed_energy_transfer_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return NotifyAllowedEnergyTransferStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return NotifyAllowedEnergyTransferStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "NotifyAllowedEnergyTransferStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os,
                         const NotifyAllowedEnergyTransferStatusEnum& notify_allowed_energy_transfer_status_enum) {
    os << conversions::notify_allowed_energy_transfer_status_enum_to_string(notify_allowed_energy_transfer_status_enum);
    return os;
}

// from: NotifyChargingLimitRequest
namespace conversions {
std::string cost_kind_enum_to_string(CostKindEnum e) {
    switch (e) {
    case CostKindEnum::CarbonDioxideEmission:
        return "CarbonDioxideEmission";
    case CostKindEnum::RelativePricePercentage:
        return "RelativePricePercentage";
    case CostKindEnum::RenewableGenerationPercentage:
        return "RenewableGenerationPercentage";
    }

    throw EnumToStringException{e, "CostKindEnum"};
}

CostKindEnum string_to_cost_kind_enum(const std::string& s) {
    if (s == "CarbonDioxideEmission") {
        return CostKindEnum::CarbonDioxideEmission;
    }
    if (s == "RelativePricePercentage") {
        return CostKindEnum::RelativePricePercentage;
    }
    if (s == "RenewableGenerationPercentage") {
        return CostKindEnum::RenewableGenerationPercentage;
    }

    throw StringToEnumException{s, "CostKindEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CostKindEnum& cost_kind_enum) {
    os << conversions::cost_kind_enum_to_string(cost_kind_enum);
    return os;
}

// from: NotifyDERAlarmRequest
namespace conversions {
std::string grid_event_fault_enum_to_string(GridEventFaultEnum e) {
    switch (e) {
    case GridEventFaultEnum::CurrentImbalance:
        return "CurrentImbalance";
    case GridEventFaultEnum::LocalEmergency:
        return "LocalEmergency";
    case GridEventFaultEnum::LowInputPower:
        return "LowInputPower";
    case GridEventFaultEnum::OverCurrent:
        return "OverCurrent";
    case GridEventFaultEnum::OverFrequency:
        return "OverFrequency";
    case GridEventFaultEnum::OverVoltage:
        return "OverVoltage";
    case GridEventFaultEnum::PhaseRotation:
        return "PhaseRotation";
    case GridEventFaultEnum::RemoteEmergency:
        return "RemoteEmergency";
    case GridEventFaultEnum::UnderFrequency:
        return "UnderFrequency";
    case GridEventFaultEnum::UnderVoltage:
        return "UnderVoltage";
    case GridEventFaultEnum::VoltageImbalance:
        return "VoltageImbalance";
    }

    throw EnumToStringException{e, "GridEventFaultEnum"};
}

GridEventFaultEnum string_to_grid_event_fault_enum(const std::string& s) {
    if (s == "CurrentImbalance") {
        return GridEventFaultEnum::CurrentImbalance;
    }
    if (s == "LocalEmergency") {
        return GridEventFaultEnum::LocalEmergency;
    }
    if (s == "LowInputPower") {
        return GridEventFaultEnum::LowInputPower;
    }
    if (s == "OverCurrent") {
        return GridEventFaultEnum::OverCurrent;
    }
    if (s == "OverFrequency") {
        return GridEventFaultEnum::OverFrequency;
    }
    if (s == "OverVoltage") {
        return GridEventFaultEnum::OverVoltage;
    }
    if (s == "PhaseRotation") {
        return GridEventFaultEnum::PhaseRotation;
    }
    if (s == "RemoteEmergency") {
        return GridEventFaultEnum::RemoteEmergency;
    }
    if (s == "UnderFrequency") {
        return GridEventFaultEnum::UnderFrequency;
    }
    if (s == "UnderVoltage") {
        return GridEventFaultEnum::UnderVoltage;
    }
    if (s == "VoltageImbalance") {
        return GridEventFaultEnum::VoltageImbalance;
    }

    throw StringToEnumException{s, "GridEventFaultEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GridEventFaultEnum& grid_event_fault_enum) {
    os << conversions::grid_event_fault_enum_to_string(grid_event_fault_enum);
    return os;
}

// from: NotifyEVChargingNeedsRequest
namespace conversions {
std::string islanding_detection_enum_to_string(IslandingDetectionEnum e) {
    switch (e) {
    case IslandingDetectionEnum::NoAntiIslandingSupport:
        return "NoAntiIslandingSupport";
    case IslandingDetectionEnum::RoCoF:
        return "RoCoF";
    case IslandingDetectionEnum::UVP_OVP:
        return "UVP_OVP";
    case IslandingDetectionEnum::UFP_OFP:
        return "UFP_OFP";
    case IslandingDetectionEnum::VoltageVectorShift:
        return "VoltageVectorShift";
    case IslandingDetectionEnum::ZeroCrossingDetection:
        return "ZeroCrossingDetection";
    case IslandingDetectionEnum::OtherPassive:
        return "OtherPassive";
    case IslandingDetectionEnum::ImpedanceMeasurement:
        return "ImpedanceMeasurement";
    case IslandingDetectionEnum::ImpedanceAtFrequency:
        return "ImpedanceAtFrequency";
    case IslandingDetectionEnum::SlipModeFrequencyShift:
        return "SlipModeFrequencyShift";
    case IslandingDetectionEnum::SandiaFrequencyShift:
        return "SandiaFrequencyShift";
    case IslandingDetectionEnum::SandiaVoltageShift:
        return "SandiaVoltageShift";
    case IslandingDetectionEnum::FrequencyJump:
        return "FrequencyJump";
    case IslandingDetectionEnum::RCLQFactor:
        return "RCLQFactor";
    case IslandingDetectionEnum::OtherActive:
        return "OtherActive";
    }

    throw EnumToStringException{e, "IslandingDetectionEnum"};
}

IslandingDetectionEnum string_to_islanding_detection_enum(const std::string& s) {
    if (s == "NoAntiIslandingSupport") {
        return IslandingDetectionEnum::NoAntiIslandingSupport;
    }
    if (s == "RoCoF") {
        return IslandingDetectionEnum::RoCoF;
    }
    if (s == "UVP_OVP") {
        return IslandingDetectionEnum::UVP_OVP;
    }
    if (s == "UFP_OFP") {
        return IslandingDetectionEnum::UFP_OFP;
    }
    if (s == "VoltageVectorShift") {
        return IslandingDetectionEnum::VoltageVectorShift;
    }
    if (s == "ZeroCrossingDetection") {
        return IslandingDetectionEnum::ZeroCrossingDetection;
    }
    if (s == "OtherPassive") {
        return IslandingDetectionEnum::OtherPassive;
    }
    if (s == "ImpedanceMeasurement") {
        return IslandingDetectionEnum::ImpedanceMeasurement;
    }
    if (s == "ImpedanceAtFrequency") {
        return IslandingDetectionEnum::ImpedanceAtFrequency;
    }
    if (s == "SlipModeFrequencyShift") {
        return IslandingDetectionEnum::SlipModeFrequencyShift;
    }
    if (s == "SandiaFrequencyShift") {
        return IslandingDetectionEnum::SandiaFrequencyShift;
    }
    if (s == "SandiaVoltageShift") {
        return IslandingDetectionEnum::SandiaVoltageShift;
    }
    if (s == "FrequencyJump") {
        return IslandingDetectionEnum::FrequencyJump;
    }
    if (s == "RCLQFactor") {
        return IslandingDetectionEnum::RCLQFactor;
    }
    if (s == "OtherActive") {
        return IslandingDetectionEnum::OtherActive;
    }

    throw StringToEnumException{s, "IslandingDetectionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const IslandingDetectionEnum& islanding_detection_enum) {
    os << conversions::islanding_detection_enum_to_string(islanding_detection_enum);
    return os;
}

// from: NotifyEVChargingNeedsRequest
namespace conversions {
std::string control_mode_enum_to_string(ControlModeEnum e) {
    switch (e) {
    case ControlModeEnum::ScheduledControl:
        return "ScheduledControl";
    case ControlModeEnum::DynamicControl:
        return "DynamicControl";
    }

    throw EnumToStringException{e, "ControlModeEnum"};
}

ControlModeEnum string_to_control_mode_enum(const std::string& s) {
    if (s == "ScheduledControl") {
        return ControlModeEnum::ScheduledControl;
    }
    if (s == "DynamicControl") {
        return ControlModeEnum::DynamicControl;
    }

    throw StringToEnumException{s, "ControlModeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ControlModeEnum& control_mode_enum) {
    os << conversions::control_mode_enum_to_string(control_mode_enum);
    return os;
}

// from: NotifyEVChargingNeedsRequest
namespace conversions {
std::string mobility_needs_mode_enum_to_string(MobilityNeedsModeEnum e) {
    switch (e) {
    case MobilityNeedsModeEnum::EVCC:
        return "EVCC";
    case MobilityNeedsModeEnum::EVCC_SECC:
        return "EVCC_SECC";
    }

    throw EnumToStringException{e, "MobilityNeedsModeEnum"};
}

MobilityNeedsModeEnum string_to_mobility_needs_mode_enum(const std::string& s) {
    if (s == "EVCC") {
        return MobilityNeedsModeEnum::EVCC;
    }
    if (s == "EVCC_SECC") {
        return MobilityNeedsModeEnum::EVCC_SECC;
    }

    throw StringToEnumException{s, "MobilityNeedsModeEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MobilityNeedsModeEnum& mobility_needs_mode_enum) {
    os << conversions::mobility_needs_mode_enum_to_string(mobility_needs_mode_enum);
    return os;
}

// from: NotifyEVChargingNeedsResponse
namespace conversions {
std::string notify_evcharging_needs_status_enum_to_string(NotifyEVChargingNeedsStatusEnum e) {
    switch (e) {
    case NotifyEVChargingNeedsStatusEnum::Accepted:
        return "Accepted";
    case NotifyEVChargingNeedsStatusEnum::Rejected:
        return "Rejected";
    case NotifyEVChargingNeedsStatusEnum::Processing:
        return "Processing";
    case NotifyEVChargingNeedsStatusEnum::NoChargingProfile:
        return "NoChargingProfile";
    }

    throw EnumToStringException{e, "NotifyEVChargingNeedsStatusEnum"};
}

NotifyEVChargingNeedsStatusEnum string_to_notify_evcharging_needs_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return NotifyEVChargingNeedsStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return NotifyEVChargingNeedsStatusEnum::Rejected;
    }
    if (s == "Processing") {
        return NotifyEVChargingNeedsStatusEnum::Processing;
    }
    if (s == "NoChargingProfile") {
        return NotifyEVChargingNeedsStatusEnum::NoChargingProfile;
    }

    throw StringToEnumException{s, "NotifyEVChargingNeedsStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsStatusEnum& notify_evcharging_needs_status_enum) {
    os << conversions::notify_evcharging_needs_status_enum_to_string(notify_evcharging_needs_status_enum);
    return os;
}

// from: NotifyEventRequest
namespace conversions {
std::string event_trigger_enum_to_string(EventTriggerEnum e) {
    switch (e) {
    case EventTriggerEnum::Alerting:
        return "Alerting";
    case EventTriggerEnum::Delta:
        return "Delta";
    case EventTriggerEnum::Periodic:
        return "Periodic";
    }

    throw EnumToStringException{e, "EventTriggerEnum"};
}

EventTriggerEnum string_to_event_trigger_enum(const std::string& s) {
    if (s == "Alerting") {
        return EventTriggerEnum::Alerting;
    }
    if (s == "Delta") {
        return EventTriggerEnum::Delta;
    }
    if (s == "Periodic") {
        return EventTriggerEnum::Periodic;
    }

    throw StringToEnumException{s, "EventTriggerEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const EventTriggerEnum& event_trigger_enum) {
    os << conversions::event_trigger_enum_to_string(event_trigger_enum);
    return os;
}

// from: NotifyEventRequest
namespace conversions {
std::string event_notification_enum_to_string(EventNotificationEnum e) {
    switch (e) {
    case EventNotificationEnum::HardWiredNotification:
        return "HardWiredNotification";
    case EventNotificationEnum::HardWiredMonitor:
        return "HardWiredMonitor";
    case EventNotificationEnum::PreconfiguredMonitor:
        return "PreconfiguredMonitor";
    case EventNotificationEnum::CustomMonitor:
        return "CustomMonitor";
    }

    throw EnumToStringException{e, "EventNotificationEnum"};
}

EventNotificationEnum string_to_event_notification_enum(const std::string& s) {
    if (s == "HardWiredNotification") {
        return EventNotificationEnum::HardWiredNotification;
    }
    if (s == "HardWiredMonitor") {
        return EventNotificationEnum::HardWiredMonitor;
    }
    if (s == "PreconfiguredMonitor") {
        return EventNotificationEnum::PreconfiguredMonitor;
    }
    if (s == "CustomMonitor") {
        return EventNotificationEnum::CustomMonitor;
    }

    throw StringToEnumException{s, "EventNotificationEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const EventNotificationEnum& event_notification_enum) {
    os << conversions::event_notification_enum_to_string(event_notification_enum);
    return os;
}

// from: NotifyMonitoringReportRequest
namespace conversions {
std::string monitor_enum_to_string(MonitorEnum e) {
    switch (e) {
    case MonitorEnum::UpperThreshold:
        return "UpperThreshold";
    case MonitorEnum::LowerThreshold:
        return "LowerThreshold";
    case MonitorEnum::Delta:
        return "Delta";
    case MonitorEnum::Periodic:
        return "Periodic";
    case MonitorEnum::PeriodicClockAligned:
        return "PeriodicClockAligned";
    case MonitorEnum::TargetDelta:
        return "TargetDelta";
    case MonitorEnum::TargetDeltaRelative:
        return "TargetDeltaRelative";
    }

    throw EnumToStringException{e, "MonitorEnum"};
}

MonitorEnum string_to_monitor_enum(const std::string& s) {
    if (s == "UpperThreshold") {
        return MonitorEnum::UpperThreshold;
    }
    if (s == "LowerThreshold") {
        return MonitorEnum::LowerThreshold;
    }
    if (s == "Delta") {
        return MonitorEnum::Delta;
    }
    if (s == "Periodic") {
        return MonitorEnum::Periodic;
    }
    if (s == "PeriodicClockAligned") {
        return MonitorEnum::PeriodicClockAligned;
    }
    if (s == "TargetDelta") {
        return MonitorEnum::TargetDelta;
    }
    if (s == "TargetDeltaRelative") {
        return MonitorEnum::TargetDeltaRelative;
    }

    throw StringToEnumException{s, "MonitorEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MonitorEnum& monitor_enum) {
    os << conversions::monitor_enum_to_string(monitor_enum);
    return os;
}

// from: NotifyReportRequest
namespace conversions {
std::string mutability_enum_to_string(MutabilityEnum e) {
    switch (e) {
    case MutabilityEnum::ReadOnly:
        return "ReadOnly";
    case MutabilityEnum::WriteOnly:
        return "WriteOnly";
    case MutabilityEnum::ReadWrite:
        return "ReadWrite";
    }

    throw EnumToStringException{e, "MutabilityEnum"};
}

MutabilityEnum string_to_mutability_enum(const std::string& s) {
    if (s == "ReadOnly") {
        return MutabilityEnum::ReadOnly;
    }
    if (s == "WriteOnly") {
        return MutabilityEnum::WriteOnly;
    }
    if (s == "ReadWrite") {
        return MutabilityEnum::ReadWrite;
    }

    throw StringToEnumException{s, "MutabilityEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MutabilityEnum& mutability_enum) {
    os << conversions::mutability_enum_to_string(mutability_enum);
    return os;
}

// from: NotifyReportRequest
namespace conversions {
std::string data_enum_to_string(DataEnum e) {
    switch (e) {
    case DataEnum::string:
        return "string";
    case DataEnum::decimal:
        return "decimal";
    case DataEnum::integer:
        return "integer";
    case DataEnum::dateTime:
        return "dateTime";
    case DataEnum::boolean:
        return "boolean";
    case DataEnum::OptionList:
        return "OptionList";
    case DataEnum::SequenceList:
        return "SequenceList";
    case DataEnum::MemberList:
        return "MemberList";
    }

    throw EnumToStringException{e, "DataEnum"};
}

DataEnum string_to_data_enum(const std::string& s) {
    if (s == "string") {
        return DataEnum::string;
    }
    if (s == "decimal") {
        return DataEnum::decimal;
    }
    if (s == "integer") {
        return DataEnum::integer;
    }
    if (s == "dateTime") {
        return DataEnum::dateTime;
    }
    if (s == "boolean") {
        return DataEnum::boolean;
    }
    if (s == "OptionList") {
        return DataEnum::OptionList;
    }
    if (s == "SequenceList") {
        return DataEnum::SequenceList;
    }
    if (s == "MemberList") {
        return DataEnum::MemberList;
    }

    throw StringToEnumException{s, "DataEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DataEnum& data_enum) {
    os << conversions::data_enum_to_string(data_enum);
    return os;
}

// from: NotifySettlementRequest
namespace conversions {
std::string payment_status_enum_to_string(PaymentStatusEnum e) {
    switch (e) {
    case PaymentStatusEnum::Settled:
        return "Settled";
    case PaymentStatusEnum::Canceled:
        return "Canceled";
    case PaymentStatusEnum::Rejected:
        return "Rejected";
    case PaymentStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "PaymentStatusEnum"};
}

PaymentStatusEnum string_to_payment_status_enum(const std::string& s) {
    if (s == "Settled") {
        return PaymentStatusEnum::Settled;
    }
    if (s == "Canceled") {
        return PaymentStatusEnum::Canceled;
    }
    if (s == "Rejected") {
        return PaymentStatusEnum::Rejected;
    }
    if (s == "Failed") {
        return PaymentStatusEnum::Failed;
    }

    throw StringToEnumException{s, "PaymentStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PaymentStatusEnum& payment_status_enum) {
    os << conversions::payment_status_enum_to_string(payment_status_enum);
    return os;
}

// from: PublishFirmwareStatusNotificationRequest
namespace conversions {
std::string publish_firmware_status_enum_to_string(PublishFirmwareStatusEnum e) {
    switch (e) {
    case PublishFirmwareStatusEnum::Idle:
        return "Idle";
    case PublishFirmwareStatusEnum::DownloadScheduled:
        return "DownloadScheduled";
    case PublishFirmwareStatusEnum::Downloading:
        return "Downloading";
    case PublishFirmwareStatusEnum::Downloaded:
        return "Downloaded";
    case PublishFirmwareStatusEnum::Published:
        return "Published";
    case PublishFirmwareStatusEnum::DownloadFailed:
        return "DownloadFailed";
    case PublishFirmwareStatusEnum::DownloadPaused:
        return "DownloadPaused";
    case PublishFirmwareStatusEnum::InvalidChecksum:
        return "InvalidChecksum";
    case PublishFirmwareStatusEnum::ChecksumVerified:
        return "ChecksumVerified";
    case PublishFirmwareStatusEnum::PublishFailed:
        return "PublishFailed";
    }

    throw EnumToStringException{e, "PublishFirmwareStatusEnum"};
}

PublishFirmwareStatusEnum string_to_publish_firmware_status_enum(const std::string& s) {
    if (s == "Idle") {
        return PublishFirmwareStatusEnum::Idle;
    }
    if (s == "DownloadScheduled") {
        return PublishFirmwareStatusEnum::DownloadScheduled;
    }
    if (s == "Downloading") {
        return PublishFirmwareStatusEnum::Downloading;
    }
    if (s == "Downloaded") {
        return PublishFirmwareStatusEnum::Downloaded;
    }
    if (s == "Published") {
        return PublishFirmwareStatusEnum::Published;
    }
    if (s == "DownloadFailed") {
        return PublishFirmwareStatusEnum::DownloadFailed;
    }
    if (s == "DownloadPaused") {
        return PublishFirmwareStatusEnum::DownloadPaused;
    }
    if (s == "InvalidChecksum") {
        return PublishFirmwareStatusEnum::InvalidChecksum;
    }
    if (s == "ChecksumVerified") {
        return PublishFirmwareStatusEnum::ChecksumVerified;
    }
    if (s == "PublishFailed") {
        return PublishFirmwareStatusEnum::PublishFailed;
    }

    throw StringToEnumException{s, "PublishFirmwareStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusEnum& publish_firmware_status_enum) {
    os << conversions::publish_firmware_status_enum_to_string(publish_firmware_status_enum);
    return os;
}

// from: PullDynamicScheduleUpdateResponse
namespace conversions {
std::string charging_profile_status_enum_to_string(ChargingProfileStatusEnum e) {
    switch (e) {
    case ChargingProfileStatusEnum::Accepted:
        return "Accepted";
    case ChargingProfileStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "ChargingProfileStatusEnum"};
}

ChargingProfileStatusEnum string_to_charging_profile_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ChargingProfileStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return ChargingProfileStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "ChargingProfileStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargingProfileStatusEnum& charging_profile_status_enum) {
    os << conversions::charging_profile_status_enum_to_string(charging_profile_status_enum);
    return os;
}

// from: ReportChargingProfilesRequest
namespace conversions {
std::string charging_profile_kind_enum_to_string(ChargingProfileKindEnum e) {
    switch (e) {
    case ChargingProfileKindEnum::Absolute:
        return "Absolute";
    case ChargingProfileKindEnum::Recurring:
        return "Recurring";
    case ChargingProfileKindEnum::Relative:
        return "Relative";
    case ChargingProfileKindEnum::Dynamic:
        return "Dynamic";
    }

    throw EnumToStringException{e, "ChargingProfileKindEnum"};
}

ChargingProfileKindEnum string_to_charging_profile_kind_enum(const std::string& s) {
    if (s == "Absolute") {
        return ChargingProfileKindEnum::Absolute;
    }
    if (s == "Recurring") {
        return ChargingProfileKindEnum::Recurring;
    }
    if (s == "Relative") {
        return ChargingProfileKindEnum::Relative;
    }
    if (s == "Dynamic") {
        return ChargingProfileKindEnum::Dynamic;
    }

    throw StringToEnumException{s, "ChargingProfileKindEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargingProfileKindEnum& charging_profile_kind_enum) {
    os << conversions::charging_profile_kind_enum_to_string(charging_profile_kind_enum);
    return os;
}

// from: ReportChargingProfilesRequest
namespace conversions {
std::string recurrency_kind_enum_to_string(RecurrencyKindEnum e) {
    switch (e) {
    case RecurrencyKindEnum::Daily:
        return "Daily";
    case RecurrencyKindEnum::Weekly:
        return "Weekly";
    }

    throw EnumToStringException{e, "RecurrencyKindEnum"};
}

RecurrencyKindEnum string_to_recurrency_kind_enum(const std::string& s) {
    if (s == "Daily") {
        return RecurrencyKindEnum::Daily;
    }
    if (s == "Weekly") {
        return RecurrencyKindEnum::Weekly;
    }

    throw StringToEnumException{s, "RecurrencyKindEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const RecurrencyKindEnum& recurrency_kind_enum) {
    os << conversions::recurrency_kind_enum_to_string(recurrency_kind_enum);
    return os;
}

// from: ReportDERControlRequest
namespace conversions {
std::string power_during_cessation_enum_to_string(PowerDuringCessationEnum e) {
    switch (e) {
    case PowerDuringCessationEnum::Active:
        return "Active";
    case PowerDuringCessationEnum::Reactive:
        return "Reactive";
    }

    throw EnumToStringException{e, "PowerDuringCessationEnum"};
}

PowerDuringCessationEnum string_to_power_during_cessation_enum(const std::string& s) {
    if (s == "Active") {
        return PowerDuringCessationEnum::Active;
    }
    if (s == "Reactive") {
        return PowerDuringCessationEnum::Reactive;
    }

    throw StringToEnumException{s, "PowerDuringCessationEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PowerDuringCessationEnum& power_during_cessation_enum) {
    os << conversions::power_during_cessation_enum_to_string(power_during_cessation_enum);
    return os;
}

// from: ReportDERControlRequest
namespace conversions {
std::string derunit_enum_to_string(DERUnitEnum e) {
    switch (e) {
    case DERUnitEnum::Not_Applicable:
        return "Not_Applicable";
    case DERUnitEnum::PctMaxW:
        return "PctMaxW";
    case DERUnitEnum::PctMaxVar:
        return "PctMaxVar";
    case DERUnitEnum::PctWAvail:
        return "PctWAvail";
    case DERUnitEnum::PctVarAvail:
        return "PctVarAvail";
    case DERUnitEnum::PctEffectiveV:
        return "PctEffectiveV";
    }

    throw EnumToStringException{e, "DERUnitEnum"};
}

DERUnitEnum string_to_derunit_enum(const std::string& s) {
    if (s == "Not_Applicable") {
        return DERUnitEnum::Not_Applicable;
    }
    if (s == "PctMaxW") {
        return DERUnitEnum::PctMaxW;
    }
    if (s == "PctMaxVar") {
        return DERUnitEnum::PctMaxVar;
    }
    if (s == "PctWAvail") {
        return DERUnitEnum::PctWAvail;
    }
    if (s == "PctVarAvail") {
        return DERUnitEnum::PctVarAvail;
    }
    if (s == "PctEffectiveV") {
        return DERUnitEnum::PctEffectiveV;
    }

    throw StringToEnumException{s, "DERUnitEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DERUnitEnum& derunit_enum) {
    os << conversions::derunit_enum_to_string(derunit_enum);
    return os;
}

// from: RequestStartTransactionResponse
namespace conversions {
std::string request_start_stop_status_enum_to_string(RequestStartStopStatusEnum e) {
    switch (e) {
    case RequestStartStopStatusEnum::Accepted:
        return "Accepted";
    case RequestStartStopStatusEnum::Rejected:
        return "Rejected";
    }

    throw EnumToStringException{e, "RequestStartStopStatusEnum"};
}

RequestStartStopStatusEnum string_to_request_start_stop_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return RequestStartStopStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return RequestStartStopStatusEnum::Rejected;
    }

    throw StringToEnumException{s, "RequestStartStopStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const RequestStartStopStatusEnum& request_start_stop_status_enum) {
    os << conversions::request_start_stop_status_enum_to_string(request_start_stop_status_enum);
    return os;
}

// from: ReservationStatusUpdateRequest
namespace conversions {
std::string reservation_update_status_enum_to_string(ReservationUpdateStatusEnum e) {
    switch (e) {
    case ReservationUpdateStatusEnum::Expired:
        return "Expired";
    case ReservationUpdateStatusEnum::Removed:
        return "Removed";
    case ReservationUpdateStatusEnum::NoTransaction:
        return "NoTransaction";
    }

    throw EnumToStringException{e, "ReservationUpdateStatusEnum"};
}

ReservationUpdateStatusEnum string_to_reservation_update_status_enum(const std::string& s) {
    if (s == "Expired") {
        return ReservationUpdateStatusEnum::Expired;
    }
    if (s == "Removed") {
        return ReservationUpdateStatusEnum::Removed;
    }
    if (s == "NoTransaction") {
        return ReservationUpdateStatusEnum::NoTransaction;
    }

    throw StringToEnumException{s, "ReservationUpdateStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ReservationUpdateStatusEnum& reservation_update_status_enum) {
    os << conversions::reservation_update_status_enum_to_string(reservation_update_status_enum);
    return os;
}

// from: ReserveNowResponse
namespace conversions {
std::string reserve_now_status_enum_to_string(ReserveNowStatusEnum e) {
    switch (e) {
    case ReserveNowStatusEnum::Accepted:
        return "Accepted";
    case ReserveNowStatusEnum::Faulted:
        return "Faulted";
    case ReserveNowStatusEnum::Occupied:
        return "Occupied";
    case ReserveNowStatusEnum::Rejected:
        return "Rejected";
    case ReserveNowStatusEnum::Unavailable:
        return "Unavailable";
    }

    throw EnumToStringException{e, "ReserveNowStatusEnum"};
}

ReserveNowStatusEnum string_to_reserve_now_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ReserveNowStatusEnum::Accepted;
    }
    if (s == "Faulted") {
        return ReserveNowStatusEnum::Faulted;
    }
    if (s == "Occupied") {
        return ReserveNowStatusEnum::Occupied;
    }
    if (s == "Rejected") {
        return ReserveNowStatusEnum::Rejected;
    }
    if (s == "Unavailable") {
        return ReserveNowStatusEnum::Unavailable;
    }

    throw StringToEnumException{s, "ReserveNowStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ReserveNowStatusEnum& reserve_now_status_enum) {
    os << conversions::reserve_now_status_enum_to_string(reserve_now_status_enum);
    return os;
}

// from: ResetRequest
namespace conversions {
std::string reset_enum_to_string(ResetEnum e) {
    switch (e) {
    case ResetEnum::Immediate:
        return "Immediate";
    case ResetEnum::OnIdle:
        return "OnIdle";
    case ResetEnum::ImmediateAndResume:
        return "ImmediateAndResume";
    }

    throw EnumToStringException{e, "ResetEnum"};
}

ResetEnum string_to_reset_enum(const std::string& s) {
    if (s == "Immediate") {
        return ResetEnum::Immediate;
    }
    if (s == "OnIdle") {
        return ResetEnum::OnIdle;
    }
    if (s == "ImmediateAndResume") {
        return ResetEnum::ImmediateAndResume;
    }

    throw StringToEnumException{s, "ResetEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ResetEnum& reset_enum) {
    os << conversions::reset_enum_to_string(reset_enum);
    return os;
}

// from: ResetResponse
namespace conversions {
std::string reset_status_enum_to_string(ResetStatusEnum e) {
    switch (e) {
    case ResetStatusEnum::Accepted:
        return "Accepted";
    case ResetStatusEnum::Rejected:
        return "Rejected";
    case ResetStatusEnum::Scheduled:
        return "Scheduled";
    }

    throw EnumToStringException{e, "ResetStatusEnum"};
}

ResetStatusEnum string_to_reset_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return ResetStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return ResetStatusEnum::Rejected;
    }
    if (s == "Scheduled") {
        return ResetStatusEnum::Scheduled;
    }

    throw StringToEnumException{s, "ResetStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ResetStatusEnum& reset_status_enum) {
    os << conversions::reset_status_enum_to_string(reset_status_enum);
    return os;
}

// from: SendLocalListRequest
namespace conversions {
std::string update_enum_to_string(UpdateEnum e) {
    switch (e) {
    case UpdateEnum::Differential:
        return "Differential";
    case UpdateEnum::Full:
        return "Full";
    }

    throw EnumToStringException{e, "UpdateEnum"};
}

UpdateEnum string_to_update_enum(const std::string& s) {
    if (s == "Differential") {
        return UpdateEnum::Differential;
    }
    if (s == "Full") {
        return UpdateEnum::Full;
    }

    throw StringToEnumException{s, "UpdateEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const UpdateEnum& update_enum) {
    os << conversions::update_enum_to_string(update_enum);
    return os;
}

// from: SendLocalListResponse
namespace conversions {
std::string send_local_list_status_enum_to_string(SendLocalListStatusEnum e) {
    switch (e) {
    case SendLocalListStatusEnum::Accepted:
        return "Accepted";
    case SendLocalListStatusEnum::Failed:
        return "Failed";
    case SendLocalListStatusEnum::VersionMismatch:
        return "VersionMismatch";
    }

    throw EnumToStringException{e, "SendLocalListStatusEnum"};
}

SendLocalListStatusEnum string_to_send_local_list_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return SendLocalListStatusEnum::Accepted;
    }
    if (s == "Failed") {
        return SendLocalListStatusEnum::Failed;
    }
    if (s == "VersionMismatch") {
        return SendLocalListStatusEnum::VersionMismatch;
    }

    throw StringToEnumException{s, "SendLocalListStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const SendLocalListStatusEnum& send_local_list_status_enum) {
    os << conversions::send_local_list_status_enum_to_string(send_local_list_status_enum);
    return os;
}

// from: SetDefaultTariffResponse
namespace conversions {
std::string tariff_set_status_enum_to_string(TariffSetStatusEnum e) {
    switch (e) {
    case TariffSetStatusEnum::Accepted:
        return "Accepted";
    case TariffSetStatusEnum::Rejected:
        return "Rejected";
    case TariffSetStatusEnum::TooManyElements:
        return "TooManyElements";
    case TariffSetStatusEnum::ConditionNotSupported:
        return "ConditionNotSupported";
    case TariffSetStatusEnum::DuplicateTariffId:
        return "DuplicateTariffId";
    }

    throw EnumToStringException{e, "TariffSetStatusEnum"};
}

TariffSetStatusEnum string_to_tariff_set_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return TariffSetStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return TariffSetStatusEnum::Rejected;
    }
    if (s == "TooManyElements") {
        return TariffSetStatusEnum::TooManyElements;
    }
    if (s == "ConditionNotSupported") {
        return TariffSetStatusEnum::ConditionNotSupported;
    }
    if (s == "DuplicateTariffId") {
        return TariffSetStatusEnum::DuplicateTariffId;
    }

    throw StringToEnumException{s, "TariffSetStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffSetStatusEnum& tariff_set_status_enum) {
    os << conversions::tariff_set_status_enum_to_string(tariff_set_status_enum);
    return os;
}

// from: SetDisplayMessageResponse
namespace conversions {
std::string display_message_status_enum_to_string(DisplayMessageStatusEnum e) {
    switch (e) {
    case DisplayMessageStatusEnum::Accepted:
        return "Accepted";
    case DisplayMessageStatusEnum::NotSupportedMessageFormat:
        return "NotSupportedMessageFormat";
    case DisplayMessageStatusEnum::Rejected:
        return "Rejected";
    case DisplayMessageStatusEnum::NotSupportedPriority:
        return "NotSupportedPriority";
    case DisplayMessageStatusEnum::NotSupportedState:
        return "NotSupportedState";
    case DisplayMessageStatusEnum::UnknownTransaction:
        return "UnknownTransaction";
    case DisplayMessageStatusEnum::LanguageNotSupported:
        return "LanguageNotSupported";
    }

    throw EnumToStringException{e, "DisplayMessageStatusEnum"};
}

DisplayMessageStatusEnum string_to_display_message_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return DisplayMessageStatusEnum::Accepted;
    }
    if (s == "NotSupportedMessageFormat") {
        return DisplayMessageStatusEnum::NotSupportedMessageFormat;
    }
    if (s == "Rejected") {
        return DisplayMessageStatusEnum::Rejected;
    }
    if (s == "NotSupportedPriority") {
        return DisplayMessageStatusEnum::NotSupportedPriority;
    }
    if (s == "NotSupportedState") {
        return DisplayMessageStatusEnum::NotSupportedState;
    }
    if (s == "UnknownTransaction") {
        return DisplayMessageStatusEnum::UnknownTransaction;
    }
    if (s == "LanguageNotSupported") {
        return DisplayMessageStatusEnum::LanguageNotSupported;
    }

    throw StringToEnumException{s, "DisplayMessageStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DisplayMessageStatusEnum& display_message_status_enum) {
    os << conversions::display_message_status_enum_to_string(display_message_status_enum);
    return os;
}

// from: SetMonitoringBaseRequest
namespace conversions {
std::string monitoring_base_enum_to_string(MonitoringBaseEnum e) {
    switch (e) {
    case MonitoringBaseEnum::All:
        return "All";
    case MonitoringBaseEnum::FactoryDefault:
        return "FactoryDefault";
    case MonitoringBaseEnum::HardWiredOnly:
        return "HardWiredOnly";
    }

    throw EnumToStringException{e, "MonitoringBaseEnum"};
}

MonitoringBaseEnum string_to_monitoring_base_enum(const std::string& s) {
    if (s == "All") {
        return MonitoringBaseEnum::All;
    }
    if (s == "FactoryDefault") {
        return MonitoringBaseEnum::FactoryDefault;
    }
    if (s == "HardWiredOnly") {
        return MonitoringBaseEnum::HardWiredOnly;
    }

    throw StringToEnumException{s, "MonitoringBaseEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MonitoringBaseEnum& monitoring_base_enum) {
    os << conversions::monitoring_base_enum_to_string(monitoring_base_enum);
    return os;
}

// from: SetNetworkProfileRequest
namespace conversions {
std::string apnauthentication_enum_to_string(APNAuthenticationEnum e) {
    switch (e) {
    case APNAuthenticationEnum::PAP:
        return "PAP";
    case APNAuthenticationEnum::CHAP:
        return "CHAP";
    case APNAuthenticationEnum::NONE:
        return "NONE";
    case APNAuthenticationEnum::AUTO:
        return "AUTO";
    }

    throw EnumToStringException{e, "APNAuthenticationEnum"};
}

APNAuthenticationEnum string_to_apnauthentication_enum(const std::string& s) {
    if (s == "PAP") {
        return APNAuthenticationEnum::PAP;
    }
    if (s == "CHAP") {
        return APNAuthenticationEnum::CHAP;
    }
    if (s == "NONE") {
        return APNAuthenticationEnum::NONE;
    }
    if (s == "AUTO") {
        return APNAuthenticationEnum::AUTO;
    }

    throw StringToEnumException{s, "APNAuthenticationEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const APNAuthenticationEnum& apnauthentication_enum) {
    os << conversions::apnauthentication_enum_to_string(apnauthentication_enum);
    return os;
}

// from: SetNetworkProfileRequest
namespace conversions {
std::string ocppversion_enum_to_string(OCPPVersionEnum e) {
    switch (e) {
    case OCPPVersionEnum::OCPP12:
        return "OCPP12";
    case OCPPVersionEnum::OCPP15:
        return "OCPP15";
    case OCPPVersionEnum::OCPP16:
        return "OCPP16";
    case OCPPVersionEnum::OCPP20:
        return "OCPP20";
    case OCPPVersionEnum::OCPP201:
        return "OCPP201";
    case OCPPVersionEnum::OCPP21:
        return "OCPP21";
    }

    throw EnumToStringException{e, "OCPPVersionEnum"};
}

OCPPVersionEnum string_to_ocppversion_enum(const std::string& s) {
    if (s == "OCPP12") {
        return OCPPVersionEnum::OCPP12;
    }
    if (s == "OCPP15") {
        return OCPPVersionEnum::OCPP15;
    }
    if (s == "OCPP16") {
        return OCPPVersionEnum::OCPP16;
    }
    if (s == "OCPP20") {
        return OCPPVersionEnum::OCPP20;
    }
    if (s == "OCPP201") {
        return OCPPVersionEnum::OCPP201;
    }
    if (s == "OCPP21") {
        return OCPPVersionEnum::OCPP21;
    }

    throw StringToEnumException{s, "OCPPVersionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OCPPVersionEnum& ocppversion_enum) {
    os << conversions::ocppversion_enum_to_string(ocppversion_enum);
    return os;
}

// from: SetNetworkProfileRequest
namespace conversions {
std::string ocppinterface_enum_to_string(OCPPInterfaceEnum e) {
    switch (e) {
    case OCPPInterfaceEnum::Wired0:
        return "Wired0";
    case OCPPInterfaceEnum::Wired1:
        return "Wired1";
    case OCPPInterfaceEnum::Wired2:
        return "Wired2";
    case OCPPInterfaceEnum::Wired3:
        return "Wired3";
    case OCPPInterfaceEnum::Wireless0:
        return "Wireless0";
    case OCPPInterfaceEnum::Wireless1:
        return "Wireless1";
    case OCPPInterfaceEnum::Wireless2:
        return "Wireless2";
    case OCPPInterfaceEnum::Wireless3:
        return "Wireless3";
    case OCPPInterfaceEnum::Any:
        return "Any";
    }

    throw EnumToStringException{e, "OCPPInterfaceEnum"};
}

OCPPInterfaceEnum string_to_ocppinterface_enum(const std::string& s) {
    if (s == "Wired0") {
        return OCPPInterfaceEnum::Wired0;
    }
    if (s == "Wired1") {
        return OCPPInterfaceEnum::Wired1;
    }
    if (s == "Wired2") {
        return OCPPInterfaceEnum::Wired2;
    }
    if (s == "Wired3") {
        return OCPPInterfaceEnum::Wired3;
    }
    if (s == "Wireless0") {
        return OCPPInterfaceEnum::Wireless0;
    }
    if (s == "Wireless1") {
        return OCPPInterfaceEnum::Wireless1;
    }
    if (s == "Wireless2") {
        return OCPPInterfaceEnum::Wireless2;
    }
    if (s == "Wireless3") {
        return OCPPInterfaceEnum::Wireless3;
    }
    if (s == "Any") {
        return OCPPInterfaceEnum::Any;
    }

    throw StringToEnumException{s, "OCPPInterfaceEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OCPPInterfaceEnum& ocppinterface_enum) {
    os << conversions::ocppinterface_enum_to_string(ocppinterface_enum);
    return os;
}

// from: SetNetworkProfileRequest
namespace conversions {
std::string ocpptransport_enum_to_string(OCPPTransportEnum e) {
    switch (e) {
    case OCPPTransportEnum::SOAP:
        return "SOAP";
    case OCPPTransportEnum::JSON:
        return "JSON";
    }

    throw EnumToStringException{e, "OCPPTransportEnum"};
}

OCPPTransportEnum string_to_ocpptransport_enum(const std::string& s) {
    if (s == "SOAP") {
        return OCPPTransportEnum::SOAP;
    }
    if (s == "JSON") {
        return OCPPTransportEnum::JSON;
    }

    throw StringToEnumException{s, "OCPPTransportEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OCPPTransportEnum& ocpptransport_enum) {
    os << conversions::ocpptransport_enum_to_string(ocpptransport_enum);
    return os;
}

// from: SetNetworkProfileRequest
namespace conversions {
std::string vpnenum_to_string(VPNEnum e) {
    switch (e) {
    case VPNEnum::IKEv2:
        return "IKEv2";
    case VPNEnum::IPSec:
        return "IPSec";
    case VPNEnum::L2TP:
        return "L2TP";
    case VPNEnum::PPTP:
        return "PPTP";
    }

    throw EnumToStringException{e, "VPNEnum"};
}

VPNEnum string_to_vpnenum(const std::string& s) {
    if (s == "IKEv2") {
        return VPNEnum::IKEv2;
    }
    if (s == "IPSec") {
        return VPNEnum::IPSec;
    }
    if (s == "L2TP") {
        return VPNEnum::L2TP;
    }
    if (s == "PPTP") {
        return VPNEnum::PPTP;
    }

    throw StringToEnumException{s, "VPNEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const VPNEnum& vpnenum) {
    os << conversions::vpnenum_to_string(vpnenum);
    return os;
}

// from: SetNetworkProfileResponse
namespace conversions {
std::string set_network_profile_status_enum_to_string(SetNetworkProfileStatusEnum e) {
    switch (e) {
    case SetNetworkProfileStatusEnum::Accepted:
        return "Accepted";
    case SetNetworkProfileStatusEnum::Rejected:
        return "Rejected";
    case SetNetworkProfileStatusEnum::Failed:
        return "Failed";
    }

    throw EnumToStringException{e, "SetNetworkProfileStatusEnum"};
}

SetNetworkProfileStatusEnum string_to_set_network_profile_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return SetNetworkProfileStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return SetNetworkProfileStatusEnum::Rejected;
    }
    if (s == "Failed") {
        return SetNetworkProfileStatusEnum::Failed;
    }

    throw StringToEnumException{s, "SetNetworkProfileStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const SetNetworkProfileStatusEnum& set_network_profile_status_enum) {
    os << conversions::set_network_profile_status_enum_to_string(set_network_profile_status_enum);
    return os;
}

// from: SetVariableMonitoringResponse
namespace conversions {
std::string set_monitoring_status_enum_to_string(SetMonitoringStatusEnum e) {
    switch (e) {
    case SetMonitoringStatusEnum::Accepted:
        return "Accepted";
    case SetMonitoringStatusEnum::UnknownComponent:
        return "UnknownComponent";
    case SetMonitoringStatusEnum::UnknownVariable:
        return "UnknownVariable";
    case SetMonitoringStatusEnum::UnsupportedMonitorType:
        return "UnsupportedMonitorType";
    case SetMonitoringStatusEnum::Rejected:
        return "Rejected";
    case SetMonitoringStatusEnum::Duplicate:
        return "Duplicate";
    }

    throw EnumToStringException{e, "SetMonitoringStatusEnum"};
}

SetMonitoringStatusEnum string_to_set_monitoring_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return SetMonitoringStatusEnum::Accepted;
    }
    if (s == "UnknownComponent") {
        return SetMonitoringStatusEnum::UnknownComponent;
    }
    if (s == "UnknownVariable") {
        return SetMonitoringStatusEnum::UnknownVariable;
    }
    if (s == "UnsupportedMonitorType") {
        return SetMonitoringStatusEnum::UnsupportedMonitorType;
    }
    if (s == "Rejected") {
        return SetMonitoringStatusEnum::Rejected;
    }
    if (s == "Duplicate") {
        return SetMonitoringStatusEnum::Duplicate;
    }

    throw StringToEnumException{s, "SetMonitoringStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const SetMonitoringStatusEnum& set_monitoring_status_enum) {
    os << conversions::set_monitoring_status_enum_to_string(set_monitoring_status_enum);
    return os;
}

// from: SetVariablesResponse
namespace conversions {
std::string set_variable_status_enum_to_string(SetVariableStatusEnum e) {
    switch (e) {
    case SetVariableStatusEnum::Accepted:
        return "Accepted";
    case SetVariableStatusEnum::Rejected:
        return "Rejected";
    case SetVariableStatusEnum::UnknownComponent:
        return "UnknownComponent";
    case SetVariableStatusEnum::UnknownVariable:
        return "UnknownVariable";
    case SetVariableStatusEnum::NotSupportedAttributeType:
        return "NotSupportedAttributeType";
    case SetVariableStatusEnum::RebootRequired:
        return "RebootRequired";
    }

    throw EnumToStringException{e, "SetVariableStatusEnum"};
}

SetVariableStatusEnum string_to_set_variable_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return SetVariableStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return SetVariableStatusEnum::Rejected;
    }
    if (s == "UnknownComponent") {
        return SetVariableStatusEnum::UnknownComponent;
    }
    if (s == "UnknownVariable") {
        return SetVariableStatusEnum::UnknownVariable;
    }
    if (s == "NotSupportedAttributeType") {
        return SetVariableStatusEnum::NotSupportedAttributeType;
    }
    if (s == "RebootRequired") {
        return SetVariableStatusEnum::RebootRequired;
    }

    throw StringToEnumException{s, "SetVariableStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const SetVariableStatusEnum& set_variable_status_enum) {
    os << conversions::set_variable_status_enum_to_string(set_variable_status_enum);
    return os;
}

// from: StatusNotificationRequest
namespace conversions {
std::string connector_status_enum_to_string(ConnectorStatusEnum e) {
    switch (e) {
    case ConnectorStatusEnum::Available:
        return "Available";
    case ConnectorStatusEnum::Occupied:
        return "Occupied";
    case ConnectorStatusEnum::Reserved:
        return "Reserved";
    case ConnectorStatusEnum::Unavailable:
        return "Unavailable";
    case ConnectorStatusEnum::Faulted:
        return "Faulted";
    }

    throw EnumToStringException{e, "ConnectorStatusEnum"};
}

ConnectorStatusEnum string_to_connector_status_enum(const std::string& s) {
    if (s == "Available") {
        return ConnectorStatusEnum::Available;
    }
    if (s == "Occupied") {
        return ConnectorStatusEnum::Occupied;
    }
    if (s == "Reserved") {
        return ConnectorStatusEnum::Reserved;
    }
    if (s == "Unavailable") {
        return ConnectorStatusEnum::Unavailable;
    }
    if (s == "Faulted") {
        return ConnectorStatusEnum::Faulted;
    }

    throw StringToEnumException{s, "ConnectorStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ConnectorStatusEnum& connector_status_enum) {
    os << conversions::connector_status_enum_to_string(connector_status_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string cost_dimension_enum_to_string(CostDimensionEnum e) {
    switch (e) {
    case CostDimensionEnum::Energy:
        return "Energy";
    case CostDimensionEnum::MaxCurrent:
        return "MaxCurrent";
    case CostDimensionEnum::MinCurrent:
        return "MinCurrent";
    case CostDimensionEnum::MaxPower:
        return "MaxPower";
    case CostDimensionEnum::MinPower:
        return "MinPower";
    case CostDimensionEnum::IdleTIme:
        return "IdleTIme";
    case CostDimensionEnum::ChargingTime:
        return "ChargingTime";
    }

    throw EnumToStringException{e, "CostDimensionEnum"};
}

CostDimensionEnum string_to_cost_dimension_enum(const std::string& s) {
    if (s == "Energy") {
        return CostDimensionEnum::Energy;
    }
    if (s == "MaxCurrent") {
        return CostDimensionEnum::MaxCurrent;
    }
    if (s == "MinCurrent") {
        return CostDimensionEnum::MinCurrent;
    }
    if (s == "MaxPower") {
        return CostDimensionEnum::MaxPower;
    }
    if (s == "MinPower") {
        return CostDimensionEnum::MinPower;
    }
    if (s == "IdleTIme") {
        return CostDimensionEnum::IdleTIme;
    }
    if (s == "ChargingTime") {
        return CostDimensionEnum::ChargingTime;
    }

    throw StringToEnumException{s, "CostDimensionEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CostDimensionEnum& cost_dimension_enum) {
    os << conversions::cost_dimension_enum_to_string(cost_dimension_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string tariff_cost_enum_to_string(TariffCostEnum e) {
    switch (e) {
    case TariffCostEnum::NormalCost:
        return "NormalCost";
    case TariffCostEnum::MinCost:
        return "MinCost";
    case TariffCostEnum::MaxCost:
        return "MaxCost";
    }

    throw EnumToStringException{e, "TariffCostEnum"};
}

TariffCostEnum string_to_tariff_cost_enum(const std::string& s) {
    if (s == "NormalCost") {
        return TariffCostEnum::NormalCost;
    }
    if (s == "MinCost") {
        return TariffCostEnum::MinCost;
    }
    if (s == "MaxCost") {
        return TariffCostEnum::MaxCost;
    }

    throw StringToEnumException{s, "TariffCostEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TariffCostEnum& tariff_cost_enum) {
    os << conversions::tariff_cost_enum_to_string(tariff_cost_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string transaction_event_enum_to_string(TransactionEventEnum e) {
    switch (e) {
    case TransactionEventEnum::Ended:
        return "Ended";
    case TransactionEventEnum::Started:
        return "Started";
    case TransactionEventEnum::Updated:
        return "Updated";
    }

    throw EnumToStringException{e, "TransactionEventEnum"};
}

TransactionEventEnum string_to_transaction_event_enum(const std::string& s) {
    if (s == "Ended") {
        return TransactionEventEnum::Ended;
    }
    if (s == "Started") {
        return TransactionEventEnum::Started;
    }
    if (s == "Updated") {
        return TransactionEventEnum::Updated;
    }

    throw StringToEnumException{s, "TransactionEventEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TransactionEventEnum& transaction_event_enum) {
    os << conversions::transaction_event_enum_to_string(transaction_event_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string trigger_reason_enum_to_string(TriggerReasonEnum e) {
    switch (e) {
    case TriggerReasonEnum::AbnormalCondition:
        return "AbnormalCondition";
    case TriggerReasonEnum::Authorized:
        return "Authorized";
    case TriggerReasonEnum::CablePluggedIn:
        return "CablePluggedIn";
    case TriggerReasonEnum::ChargingRateChanged:
        return "ChargingRateChanged";
    case TriggerReasonEnum::ChargingStateChanged:
        return "ChargingStateChanged";
    case TriggerReasonEnum::CostLimitReached:
        return "CostLimitReached";
    case TriggerReasonEnum::Deauthorized:
        return "Deauthorized";
    case TriggerReasonEnum::EnergyLimitReached:
        return "EnergyLimitReached";
    case TriggerReasonEnum::EVCommunicationLost:
        return "EVCommunicationLost";
    case TriggerReasonEnum::EVConnectTimeout:
        return "EVConnectTimeout";
    case TriggerReasonEnum::EVDeparted:
        return "EVDeparted";
    case TriggerReasonEnum::EVDetected:
        return "EVDetected";
    case TriggerReasonEnum::LimitSet:
        return "LimitSet";
    case TriggerReasonEnum::MeterValueClock:
        return "MeterValueClock";
    case TriggerReasonEnum::MeterValuePeriodic:
        return "MeterValuePeriodic";
    case TriggerReasonEnum::OperationModeChanged:
        return "OperationModeChanged";
    case TriggerReasonEnum::RemoteStart:
        return "RemoteStart";
    case TriggerReasonEnum::RemoteStop:
        return "RemoteStop";
    case TriggerReasonEnum::ResetCommand:
        return "ResetCommand";
    case TriggerReasonEnum::RunningCost:
        return "RunningCost";
    case TriggerReasonEnum::SignedDataReceived:
        return "SignedDataReceived";
    case TriggerReasonEnum::SoCLimitReached:
        return "SoCLimitReached";
    case TriggerReasonEnum::StopAuthorized:
        return "StopAuthorized";
    case TriggerReasonEnum::TariffChanged:
        return "TariffChanged";
    case TriggerReasonEnum::TariffNotAccepted:
        return "TariffNotAccepted";
    case TriggerReasonEnum::TimeLimitReached:
        return "TimeLimitReached";
    case TriggerReasonEnum::Trigger:
        return "Trigger";
    case TriggerReasonEnum::TxResumed:
        return "TxResumed";
    case TriggerReasonEnum::UnlockCommand:
        return "UnlockCommand";
    }

    throw EnumToStringException{e, "TriggerReasonEnum"};
}

TriggerReasonEnum string_to_trigger_reason_enum(const std::string& s) {
    if (s == "AbnormalCondition") {
        return TriggerReasonEnum::AbnormalCondition;
    }
    if (s == "Authorized") {
        return TriggerReasonEnum::Authorized;
    }
    if (s == "CablePluggedIn") {
        return TriggerReasonEnum::CablePluggedIn;
    }
    if (s == "ChargingRateChanged") {
        return TriggerReasonEnum::ChargingRateChanged;
    }
    if (s == "ChargingStateChanged") {
        return TriggerReasonEnum::ChargingStateChanged;
    }
    if (s == "CostLimitReached") {
        return TriggerReasonEnum::CostLimitReached;
    }
    if (s == "Deauthorized") {
        return TriggerReasonEnum::Deauthorized;
    }
    if (s == "EnergyLimitReached") {
        return TriggerReasonEnum::EnergyLimitReached;
    }
    if (s == "EVCommunicationLost") {
        return TriggerReasonEnum::EVCommunicationLost;
    }
    if (s == "EVConnectTimeout") {
        return TriggerReasonEnum::EVConnectTimeout;
    }
    if (s == "EVDeparted") {
        return TriggerReasonEnum::EVDeparted;
    }
    if (s == "EVDetected") {
        return TriggerReasonEnum::EVDetected;
    }
    if (s == "LimitSet") {
        return TriggerReasonEnum::LimitSet;
    }
    if (s == "MeterValueClock") {
        return TriggerReasonEnum::MeterValueClock;
    }
    if (s == "MeterValuePeriodic") {
        return TriggerReasonEnum::MeterValuePeriodic;
    }
    if (s == "OperationModeChanged") {
        return TriggerReasonEnum::OperationModeChanged;
    }
    if (s == "RemoteStart") {
        return TriggerReasonEnum::RemoteStart;
    }
    if (s == "RemoteStop") {
        return TriggerReasonEnum::RemoteStop;
    }
    if (s == "ResetCommand") {
        return TriggerReasonEnum::ResetCommand;
    }
    if (s == "RunningCost") {
        return TriggerReasonEnum::RunningCost;
    }
    if (s == "SignedDataReceived") {
        return TriggerReasonEnum::SignedDataReceived;
    }
    if (s == "SoCLimitReached") {
        return TriggerReasonEnum::SoCLimitReached;
    }
    if (s == "StopAuthorized") {
        return TriggerReasonEnum::StopAuthorized;
    }
    if (s == "TariffChanged") {
        return TriggerReasonEnum::TariffChanged;
    }
    if (s == "TariffNotAccepted") {
        return TriggerReasonEnum::TariffNotAccepted;
    }
    if (s == "TimeLimitReached") {
        return TriggerReasonEnum::TimeLimitReached;
    }
    if (s == "Trigger") {
        return TriggerReasonEnum::Trigger;
    }
    if (s == "TxResumed") {
        return TriggerReasonEnum::TxResumed;
    }
    if (s == "UnlockCommand") {
        return TriggerReasonEnum::UnlockCommand;
    }

    throw StringToEnumException{s, "TriggerReasonEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TriggerReasonEnum& trigger_reason_enum) {
    os << conversions::trigger_reason_enum_to_string(trigger_reason_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string preconditioning_status_enum_to_string(PreconditioningStatusEnum e) {
    switch (e) {
    case PreconditioningStatusEnum::Unknown:
        return "Unknown";
    case PreconditioningStatusEnum::Ready:
        return "Ready";
    case PreconditioningStatusEnum::NotReady:
        return "NotReady";
    case PreconditioningStatusEnum::Preconditioning:
        return "Preconditioning";
    }

    throw EnumToStringException{e, "PreconditioningStatusEnum"};
}

PreconditioningStatusEnum string_to_preconditioning_status_enum(const std::string& s) {
    if (s == "Unknown") {
        return PreconditioningStatusEnum::Unknown;
    }
    if (s == "Ready") {
        return PreconditioningStatusEnum::Ready;
    }
    if (s == "NotReady") {
        return PreconditioningStatusEnum::NotReady;
    }
    if (s == "Preconditioning") {
        return PreconditioningStatusEnum::Preconditioning;
    }

    throw StringToEnumException{s, "PreconditioningStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PreconditioningStatusEnum& preconditioning_status_enum) {
    os << conversions::preconditioning_status_enum_to_string(preconditioning_status_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string charging_state_enum_to_string(ChargingStateEnum e) {
    switch (e) {
    case ChargingStateEnum::EVConnected:
        return "EVConnected";
    case ChargingStateEnum::Charging:
        return "Charging";
    case ChargingStateEnum::SuspendedEV:
        return "SuspendedEV";
    case ChargingStateEnum::SuspendedEVSE:
        return "SuspendedEVSE";
    case ChargingStateEnum::Idle:
        return "Idle";
    }

    throw EnumToStringException{e, "ChargingStateEnum"};
}

ChargingStateEnum string_to_charging_state_enum(const std::string& s) {
    if (s == "EVConnected") {
        return ChargingStateEnum::EVConnected;
    }
    if (s == "Charging") {
        return ChargingStateEnum::Charging;
    }
    if (s == "SuspendedEV") {
        return ChargingStateEnum::SuspendedEV;
    }
    if (s == "SuspendedEVSE") {
        return ChargingStateEnum::SuspendedEVSE;
    }
    if (s == "Idle") {
        return ChargingStateEnum::Idle;
    }

    throw StringToEnumException{s, "ChargingStateEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargingStateEnum& charging_state_enum) {
    os << conversions::charging_state_enum_to_string(charging_state_enum);
    return os;
}

// from: TransactionEventRequest
namespace conversions {
std::string reason_enum_to_string(ReasonEnum e) {
    switch (e) {
    case ReasonEnum::DeAuthorized:
        return "DeAuthorized";
    case ReasonEnum::EmergencyStop:
        return "EmergencyStop";
    case ReasonEnum::EnergyLimitReached:
        return "EnergyLimitReached";
    case ReasonEnum::EVDisconnected:
        return "EVDisconnected";
    case ReasonEnum::GroundFault:
        return "GroundFault";
    case ReasonEnum::ImmediateReset:
        return "ImmediateReset";
    case ReasonEnum::MasterPass:
        return "MasterPass";
    case ReasonEnum::Local:
        return "Local";
    case ReasonEnum::LocalOutOfCredit:
        return "LocalOutOfCredit";
    case ReasonEnum::Other:
        return "Other";
    case ReasonEnum::OvercurrentFault:
        return "OvercurrentFault";
    case ReasonEnum::PowerLoss:
        return "PowerLoss";
    case ReasonEnum::PowerQuality:
        return "PowerQuality";
    case ReasonEnum::Reboot:
        return "Reboot";
    case ReasonEnum::Remote:
        return "Remote";
    case ReasonEnum::SOCLimitReached:
        return "SOCLimitReached";
    case ReasonEnum::StoppedByEV:
        return "StoppedByEV";
    case ReasonEnum::TimeLimitReached:
        return "TimeLimitReached";
    case ReasonEnum::Timeout:
        return "Timeout";
    case ReasonEnum::ReqEnergyTransferRejected:
        return "ReqEnergyTransferRejected";
    }

    throw EnumToStringException{e, "ReasonEnum"};
}

ReasonEnum string_to_reason_enum(const std::string& s) {
    if (s == "DeAuthorized") {
        return ReasonEnum::DeAuthorized;
    }
    if (s == "EmergencyStop") {
        return ReasonEnum::EmergencyStop;
    }
    if (s == "EnergyLimitReached") {
        return ReasonEnum::EnergyLimitReached;
    }
    if (s == "EVDisconnected") {
        return ReasonEnum::EVDisconnected;
    }
    if (s == "GroundFault") {
        return ReasonEnum::GroundFault;
    }
    if (s == "ImmediateReset") {
        return ReasonEnum::ImmediateReset;
    }
    if (s == "MasterPass") {
        return ReasonEnum::MasterPass;
    }
    if (s == "Local") {
        return ReasonEnum::Local;
    }
    if (s == "LocalOutOfCredit") {
        return ReasonEnum::LocalOutOfCredit;
    }
    if (s == "Other") {
        return ReasonEnum::Other;
    }
    if (s == "OvercurrentFault") {
        return ReasonEnum::OvercurrentFault;
    }
    if (s == "PowerLoss") {
        return ReasonEnum::PowerLoss;
    }
    if (s == "PowerQuality") {
        return ReasonEnum::PowerQuality;
    }
    if (s == "Reboot") {
        return ReasonEnum::Reboot;
    }
    if (s == "Remote") {
        return ReasonEnum::Remote;
    }
    if (s == "SOCLimitReached") {
        return ReasonEnum::SOCLimitReached;
    }
    if (s == "StoppedByEV") {
        return ReasonEnum::StoppedByEV;
    }
    if (s == "TimeLimitReached") {
        return ReasonEnum::TimeLimitReached;
    }
    if (s == "Timeout") {
        return ReasonEnum::Timeout;
    }
    if (s == "ReqEnergyTransferRejected") {
        return ReasonEnum::ReqEnergyTransferRejected;
    }

    throw StringToEnumException{s, "ReasonEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ReasonEnum& reason_enum) {
    os << conversions::reason_enum_to_string(reason_enum);
    return os;
}

// from: TriggerMessageRequest
namespace conversions {
std::string message_trigger_enum_to_string(MessageTriggerEnum e) {
    switch (e) {
    case MessageTriggerEnum::BootNotification:
        return "BootNotification";
    case MessageTriggerEnum::LogStatusNotification:
        return "LogStatusNotification";
    case MessageTriggerEnum::FirmwareStatusNotification:
        return "FirmwareStatusNotification";
    case MessageTriggerEnum::Heartbeat:
        return "Heartbeat";
    case MessageTriggerEnum::MeterValues:
        return "MeterValues";
    case MessageTriggerEnum::SignChargingStationCertificate:
        return "SignChargingStationCertificate";
    case MessageTriggerEnum::SignV2GCertificate:
        return "SignV2GCertificate";
    case MessageTriggerEnum::SignV2G20Certificate:
        return "SignV2G20Certificate";
    case MessageTriggerEnum::StatusNotification:
        return "StatusNotification";
    case MessageTriggerEnum::TransactionEvent:
        return "TransactionEvent";
    case MessageTriggerEnum::SignCombinedCertificate:
        return "SignCombinedCertificate";
    case MessageTriggerEnum::PublishFirmwareStatusNotification:
        return "PublishFirmwareStatusNotification";
    case MessageTriggerEnum::CustomTrigger:
        return "CustomTrigger";
    }

    throw EnumToStringException{e, "MessageTriggerEnum"};
}

MessageTriggerEnum string_to_message_trigger_enum(const std::string& s) {
    if (s == "BootNotification") {
        return MessageTriggerEnum::BootNotification;
    }
    if (s == "LogStatusNotification") {
        return MessageTriggerEnum::LogStatusNotification;
    }
    if (s == "FirmwareStatusNotification") {
        return MessageTriggerEnum::FirmwareStatusNotification;
    }
    if (s == "Heartbeat") {
        return MessageTriggerEnum::Heartbeat;
    }
    if (s == "MeterValues") {
        return MessageTriggerEnum::MeterValues;
    }
    if (s == "SignChargingStationCertificate") {
        return MessageTriggerEnum::SignChargingStationCertificate;
    }
    if (s == "SignV2GCertificate") {
        return MessageTriggerEnum::SignV2GCertificate;
    }
    if (s == "SignV2G20Certificate") {
        return MessageTriggerEnum::SignV2G20Certificate;
    }
    if (s == "StatusNotification") {
        return MessageTriggerEnum::StatusNotification;
    }
    if (s == "TransactionEvent") {
        return MessageTriggerEnum::TransactionEvent;
    }
    if (s == "SignCombinedCertificate") {
        return MessageTriggerEnum::SignCombinedCertificate;
    }
    if (s == "PublishFirmwareStatusNotification") {
        return MessageTriggerEnum::PublishFirmwareStatusNotification;
    }
    if (s == "CustomTrigger") {
        return MessageTriggerEnum::CustomTrigger;
    }

    throw StringToEnumException{s, "MessageTriggerEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageTriggerEnum& message_trigger_enum) {
    os << conversions::message_trigger_enum_to_string(message_trigger_enum);
    return os;
}

// from: TriggerMessageResponse
namespace conversions {
std::string trigger_message_status_enum_to_string(TriggerMessageStatusEnum e) {
    switch (e) {
    case TriggerMessageStatusEnum::Accepted:
        return "Accepted";
    case TriggerMessageStatusEnum::Rejected:
        return "Rejected";
    case TriggerMessageStatusEnum::NotImplemented:
        return "NotImplemented";
    }

    throw EnumToStringException{e, "TriggerMessageStatusEnum"};
}

TriggerMessageStatusEnum string_to_trigger_message_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return TriggerMessageStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return TriggerMessageStatusEnum::Rejected;
    }
    if (s == "NotImplemented") {
        return TriggerMessageStatusEnum::NotImplemented;
    }

    throw StringToEnumException{s, "TriggerMessageStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const TriggerMessageStatusEnum& trigger_message_status_enum) {
    os << conversions::trigger_message_status_enum_to_string(trigger_message_status_enum);
    return os;
}

// from: UnlockConnectorResponse
namespace conversions {
std::string unlock_status_enum_to_string(UnlockStatusEnum e) {
    switch (e) {
    case UnlockStatusEnum::Unlocked:
        return "Unlocked";
    case UnlockStatusEnum::UnlockFailed:
        return "UnlockFailed";
    case UnlockStatusEnum::OngoingAuthorizedTransaction:
        return "OngoingAuthorizedTransaction";
    case UnlockStatusEnum::UnknownConnector:
        return "UnknownConnector";
    }

    throw EnumToStringException{e, "UnlockStatusEnum"};
}

UnlockStatusEnum string_to_unlock_status_enum(const std::string& s) {
    if (s == "Unlocked") {
        return UnlockStatusEnum::Unlocked;
    }
    if (s == "UnlockFailed") {
        return UnlockStatusEnum::UnlockFailed;
    }
    if (s == "OngoingAuthorizedTransaction") {
        return UnlockStatusEnum::OngoingAuthorizedTransaction;
    }
    if (s == "UnknownConnector") {
        return UnlockStatusEnum::UnknownConnector;
    }

    throw StringToEnumException{s, "UnlockStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const UnlockStatusEnum& unlock_status_enum) {
    os << conversions::unlock_status_enum_to_string(unlock_status_enum);
    return os;
}

// from: UnpublishFirmwareResponse
namespace conversions {
std::string unpublish_firmware_status_enum_to_string(UnpublishFirmwareStatusEnum e) {
    switch (e) {
    case UnpublishFirmwareStatusEnum::DownloadOngoing:
        return "DownloadOngoing";
    case UnpublishFirmwareStatusEnum::NoFirmware:
        return "NoFirmware";
    case UnpublishFirmwareStatusEnum::Unpublished:
        return "Unpublished";
    }

    throw EnumToStringException{e, "UnpublishFirmwareStatusEnum"};
}

UnpublishFirmwareStatusEnum string_to_unpublish_firmware_status_enum(const std::string& s) {
    if (s == "DownloadOngoing") {
        return UnpublishFirmwareStatusEnum::DownloadOngoing;
    }
    if (s == "NoFirmware") {
        return UnpublishFirmwareStatusEnum::NoFirmware;
    }
    if (s == "Unpublished") {
        return UnpublishFirmwareStatusEnum::Unpublished;
    }

    throw StringToEnumException{s, "UnpublishFirmwareStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareStatusEnum& unpublish_firmware_status_enum) {
    os << conversions::unpublish_firmware_status_enum_to_string(unpublish_firmware_status_enum);
    return os;
}

// from: UpdateFirmwareResponse
namespace conversions {
std::string update_firmware_status_enum_to_string(UpdateFirmwareStatusEnum e) {
    switch (e) {
    case UpdateFirmwareStatusEnum::Accepted:
        return "Accepted";
    case UpdateFirmwareStatusEnum::Rejected:
        return "Rejected";
    case UpdateFirmwareStatusEnum::AcceptedCanceled:
        return "AcceptedCanceled";
    case UpdateFirmwareStatusEnum::InvalidCertificate:
        return "InvalidCertificate";
    case UpdateFirmwareStatusEnum::RevokedCertificate:
        return "RevokedCertificate";
    }

    throw EnumToStringException{e, "UpdateFirmwareStatusEnum"};
}

UpdateFirmwareStatusEnum string_to_update_firmware_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return UpdateFirmwareStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return UpdateFirmwareStatusEnum::Rejected;
    }
    if (s == "AcceptedCanceled") {
        return UpdateFirmwareStatusEnum::AcceptedCanceled;
    }
    if (s == "InvalidCertificate") {
        return UpdateFirmwareStatusEnum::InvalidCertificate;
    }
    if (s == "RevokedCertificate") {
        return UpdateFirmwareStatusEnum::RevokedCertificate;
    }

    throw StringToEnumException{s, "UpdateFirmwareStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const UpdateFirmwareStatusEnum& update_firmware_status_enum) {
    os << conversions::update_firmware_status_enum_to_string(update_firmware_status_enum);
    return os;
}

// from: UsePriorityChargingResponse
namespace conversions {
std::string priority_charging_status_enum_to_string(PriorityChargingStatusEnum e) {
    switch (e) {
    case PriorityChargingStatusEnum::Accepted:
        return "Accepted";
    case PriorityChargingStatusEnum::Rejected:
        return "Rejected";
    case PriorityChargingStatusEnum::NoProfile:
        return "NoProfile";
    }

    throw EnumToStringException{e, "PriorityChargingStatusEnum"};
}

PriorityChargingStatusEnum string_to_priority_charging_status_enum(const std::string& s) {
    if (s == "Accepted") {
        return PriorityChargingStatusEnum::Accepted;
    }
    if (s == "Rejected") {
        return PriorityChargingStatusEnum::Rejected;
    }
    if (s == "NoProfile") {
        return PriorityChargingStatusEnum::NoProfile;
    }

    throw StringToEnumException{s, "PriorityChargingStatusEnum"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const PriorityChargingStatusEnum& priority_charging_status_enum) {
    os << conversions::priority_charging_status_enum_to_string(priority_charging_status_enum);
    return os;
}

} // namespace v2
} // namespace ocpp
