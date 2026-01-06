// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_OCPP_ENUMS_HPP
#define OCPP_V2_OCPP_ENUMS_HPP

#include <iosfwd>
#include <string>

namespace ocpp {
namespace v2 {

// from: AFRRSignalResponse
enum class GenericStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given GenericStatusEnum \p e to human readable string
/// \returns a string representation of the GenericStatusEnum
std::string generic_status_enum_to_string(GenericStatusEnum e);

/// \brief Converts the given std::string \p s to GenericStatusEnum
/// \returns a GenericStatusEnum from a string representation
GenericStatusEnum string_to_generic_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GenericStatusEnum \p generic_status_enum to the given output
/// stream \p os
/// \returns an output stream with the GenericStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GenericStatusEnum& generic_status_enum);

// from: AuthorizeRequest
enum class HashAlgorithmEnum {
    SHA256,
    SHA384,
    SHA512,
};

namespace conversions {
/// \brief Converts the given HashAlgorithmEnum \p e to human readable string
/// \returns a string representation of the HashAlgorithmEnum
std::string hash_algorithm_enum_to_string(HashAlgorithmEnum e);

/// \brief Converts the given std::string \p s to HashAlgorithmEnum
/// \returns a HashAlgorithmEnum from a string representation
HashAlgorithmEnum string_to_hash_algorithm_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given HashAlgorithmEnum \p hash_algorithm_enum to the given output
/// stream \p os
/// \returns an output stream with the HashAlgorithmEnum written to
std::ostream& operator<<(std::ostream& os, const HashAlgorithmEnum& hash_algorithm_enum);

// from: AuthorizeResponse
enum class AuthorizationStatusEnum {
    Accepted,
    Blocked,
    ConcurrentTx,
    Expired,
    Invalid,
    NoCredit,
    NotAllowedTypeEVSE,
    NotAtThisLocation,
    NotAtThisTime,
    Unknown,
};

namespace conversions {
/// \brief Converts the given AuthorizationStatusEnum \p e to human readable string
/// \returns a string representation of the AuthorizationStatusEnum
std::string authorization_status_enum_to_string(AuthorizationStatusEnum e);

/// \brief Converts the given std::string \p s to AuthorizationStatusEnum
/// \returns a AuthorizationStatusEnum from a string representation
AuthorizationStatusEnum string_to_authorization_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AuthorizationStatusEnum \p authorization_status_enum to the
/// given output stream \p os
/// \returns an output stream with the AuthorizationStatusEnum written to
std::ostream& operator<<(std::ostream& os, const AuthorizationStatusEnum& authorization_status_enum);

// from: AuthorizeResponse
enum class MessageFormatEnum {
    ASCII,
    HTML,
    URI,
    UTF8,
    QRCODE,
};

namespace conversions {
/// \brief Converts the given MessageFormatEnum \p e to human readable string
/// \returns a string representation of the MessageFormatEnum
std::string message_format_enum_to_string(MessageFormatEnum e);

/// \brief Converts the given std::string \p s to MessageFormatEnum
/// \returns a MessageFormatEnum from a string representation
MessageFormatEnum string_to_message_format_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessageFormatEnum \p message_format_enum to the given output
/// stream \p os
/// \returns an output stream with the MessageFormatEnum written to
std::ostream& operator<<(std::ostream& os, const MessageFormatEnum& message_format_enum);

// from: AuthorizeResponse
enum class AuthorizeCertificateStatusEnum {
    Accepted,
    SignatureError,
    CertificateExpired,
    CertificateRevoked,
    NoCertificateAvailable,
    CertChainError,
    ContractCancelled,
};

namespace conversions {
/// \brief Converts the given AuthorizeCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the AuthorizeCertificateStatusEnum
std::string authorize_certificate_status_enum_to_string(AuthorizeCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to AuthorizeCertificateStatusEnum
/// \returns a AuthorizeCertificateStatusEnum from a string representation
AuthorizeCertificateStatusEnum string_to_authorize_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AuthorizeCertificateStatusEnum \p
/// authorize_certificate_status_enum to the given output stream \p os
/// \returns an output stream with the AuthorizeCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const AuthorizeCertificateStatusEnum& authorize_certificate_status_enum);

// from: AuthorizeResponse
enum class EnergyTransferModeEnum {
    AC_single_phase,
    AC_two_phase,
    AC_three_phase,
    DC,
    AC_BPT,
    AC_BPT_DER,
    AC_DER,
    DC_BPT,
    DC_ACDP,
    DC_ACDP_BPT,
    WPT,
    MCS,
    MCS_BPT,
};

namespace conversions {
/// \brief Converts the given EnergyTransferModeEnum \p e to human readable string
/// \returns a string representation of the EnergyTransferModeEnum
std::string energy_transfer_mode_enum_to_string(EnergyTransferModeEnum e);

/// \brief Converts the given std::string \p s to EnergyTransferModeEnum
/// \returns a EnergyTransferModeEnum from a string representation
EnergyTransferModeEnum string_to_energy_transfer_mode_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given EnergyTransferModeEnum \p energy_transfer_mode_enum to the
/// given output stream \p os
/// \returns an output stream with the EnergyTransferModeEnum written to
std::ostream& operator<<(std::ostream& os, const EnergyTransferModeEnum& energy_transfer_mode_enum);

// from: AuthorizeResponse
enum class DayOfWeekEnum {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday,
};

namespace conversions {
/// \brief Converts the given DayOfWeekEnum \p e to human readable string
/// \returns a string representation of the DayOfWeekEnum
std::string day_of_week_enum_to_string(DayOfWeekEnum e);

/// \brief Converts the given std::string \p s to DayOfWeekEnum
/// \returns a DayOfWeekEnum from a string representation
DayOfWeekEnum string_to_day_of_week_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DayOfWeekEnum \p day_of_week_enum to the given output stream \p
/// os
/// \returns an output stream with the DayOfWeekEnum written to
std::ostream& operator<<(std::ostream& os, const DayOfWeekEnum& day_of_week_enum);

// from: AuthorizeResponse
enum class EvseKindEnum {
    AC,
    DC,
};

namespace conversions {
/// \brief Converts the given EvseKindEnum \p e to human readable string
/// \returns a string representation of the EvseKindEnum
std::string evse_kind_enum_to_string(EvseKindEnum e);

/// \brief Converts the given std::string \p s to EvseKindEnum
/// \returns a EvseKindEnum from a string representation
EvseKindEnum string_to_evse_kind_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given EvseKindEnum \p evse_kind_enum to the given output stream \p os
/// \returns an output stream with the EvseKindEnum written to
std::ostream& operator<<(std::ostream& os, const EvseKindEnum& evse_kind_enum);

// from: BatterySwapRequest
enum class BatterySwapEventEnum {
    BatteryIn,
    BatteryOut,
    BatteryOutTimeout,
};

namespace conversions {
/// \brief Converts the given BatterySwapEventEnum \p e to human readable string
/// \returns a string representation of the BatterySwapEventEnum
std::string battery_swap_event_enum_to_string(BatterySwapEventEnum e);

/// \brief Converts the given std::string \p s to BatterySwapEventEnum
/// \returns a BatterySwapEventEnum from a string representation
BatterySwapEventEnum string_to_battery_swap_event_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given BatterySwapEventEnum \p battery_swap_event_enum to the given
/// output stream \p os
/// \returns an output stream with the BatterySwapEventEnum written to
std::ostream& operator<<(std::ostream& os, const BatterySwapEventEnum& battery_swap_event_enum);

// from: BootNotificationRequest
enum class BootReasonEnum {
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

namespace conversions {
/// \brief Converts the given BootReasonEnum \p e to human readable string
/// \returns a string representation of the BootReasonEnum
std::string boot_reason_enum_to_string(BootReasonEnum e);

/// \brief Converts the given std::string \p s to BootReasonEnum
/// \returns a BootReasonEnum from a string representation
BootReasonEnum string_to_boot_reason_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given BootReasonEnum \p boot_reason_enum to the given output stream
/// \p os
/// \returns an output stream with the BootReasonEnum written to
std::ostream& operator<<(std::ostream& os, const BootReasonEnum& boot_reason_enum);

// from: BootNotificationResponse
enum class RegistrationStatusEnum {
    Accepted,
    Pending,
    Rejected,
};

namespace conversions {
/// \brief Converts the given RegistrationStatusEnum \p e to human readable string
/// \returns a string representation of the RegistrationStatusEnum
std::string registration_status_enum_to_string(RegistrationStatusEnum e);

/// \brief Converts the given std::string \p s to RegistrationStatusEnum
/// \returns a RegistrationStatusEnum from a string representation
RegistrationStatusEnum string_to_registration_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RegistrationStatusEnum \p registration_status_enum to the given
/// output stream \p os
/// \returns an output stream with the RegistrationStatusEnum written to
std::ostream& operator<<(std::ostream& os, const RegistrationStatusEnum& registration_status_enum);

// from: CancelReservationResponse
enum class CancelReservationStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given CancelReservationStatusEnum \p e to human readable string
/// \returns a string representation of the CancelReservationStatusEnum
std::string cancel_reservation_status_enum_to_string(CancelReservationStatusEnum e);

/// \brief Converts the given std::string \p s to CancelReservationStatusEnum
/// \returns a CancelReservationStatusEnum from a string representation
CancelReservationStatusEnum string_to_cancel_reservation_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CancelReservationStatusEnum \p cancel_reservation_status_enum
/// to the given output stream \p os
/// \returns an output stream with the CancelReservationStatusEnum written to
std::ostream& operator<<(std::ostream& os, const CancelReservationStatusEnum& cancel_reservation_status_enum);

// from: CertificateSignedRequest
enum class CertificateSigningUseEnum {
    ChargingStationCertificate,
    V2GCertificate,
    V2G20Certificate,
};

namespace conversions {
/// \brief Converts the given CertificateSigningUseEnum \p e to human readable string
/// \returns a string representation of the CertificateSigningUseEnum
std::string certificate_signing_use_enum_to_string(CertificateSigningUseEnum e);

/// \brief Converts the given std::string \p s to CertificateSigningUseEnum
/// \returns a CertificateSigningUseEnum from a string representation
CertificateSigningUseEnum string_to_certificate_signing_use_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateSigningUseEnum \p certificate_signing_use_enum to
/// the given output stream \p os
/// \returns an output stream with the CertificateSigningUseEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateSigningUseEnum& certificate_signing_use_enum);

// from: CertificateSignedResponse
enum class CertificateSignedStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given CertificateSignedStatusEnum \p e to human readable string
/// \returns a string representation of the CertificateSignedStatusEnum
std::string certificate_signed_status_enum_to_string(CertificateSignedStatusEnum e);

/// \brief Converts the given std::string \p s to CertificateSignedStatusEnum
/// \returns a CertificateSignedStatusEnum from a string representation
CertificateSignedStatusEnum string_to_certificate_signed_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateSignedStatusEnum \p certificate_signed_status_enum
/// to the given output stream \p os
/// \returns an output stream with the CertificateSignedStatusEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedStatusEnum& certificate_signed_status_enum);

// from: ChangeAvailabilityRequest
enum class OperationalStatusEnum {
    Inoperative,
    Operative,
};

namespace conversions {
/// \brief Converts the given OperationalStatusEnum \p e to human readable string
/// \returns a string representation of the OperationalStatusEnum
std::string operational_status_enum_to_string(OperationalStatusEnum e);

/// \brief Converts the given std::string \p s to OperationalStatusEnum
/// \returns a OperationalStatusEnum from a string representation
OperationalStatusEnum string_to_operational_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OperationalStatusEnum \p operational_status_enum to the given
/// output stream \p os
/// \returns an output stream with the OperationalStatusEnum written to
std::ostream& operator<<(std::ostream& os, const OperationalStatusEnum& operational_status_enum);

// from: ChangeAvailabilityResponse
enum class ChangeAvailabilityStatusEnum {
    Accepted,
    Rejected,
    Scheduled,
};

namespace conversions {
/// \brief Converts the given ChangeAvailabilityStatusEnum \p e to human readable string
/// \returns a string representation of the ChangeAvailabilityStatusEnum
std::string change_availability_status_enum_to_string(ChangeAvailabilityStatusEnum e);

/// \brief Converts the given std::string \p s to ChangeAvailabilityStatusEnum
/// \returns a ChangeAvailabilityStatusEnum from a string representation
ChangeAvailabilityStatusEnum string_to_change_availability_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChangeAvailabilityStatusEnum \p change_availability_status_enum
/// to the given output stream \p os
/// \returns an output stream with the ChangeAvailabilityStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityStatusEnum& change_availability_status_enum);

// from: ChangeTransactionTariffResponse
enum class TariffChangeStatusEnum {
    Accepted,
    Rejected,
    TooManyElements,
    ConditionNotSupported,
    TxNotFound,
    NoCurrencyChange,
};

namespace conversions {
/// \brief Converts the given TariffChangeStatusEnum \p e to human readable string
/// \returns a string representation of the TariffChangeStatusEnum
std::string tariff_change_status_enum_to_string(TariffChangeStatusEnum e);

/// \brief Converts the given std::string \p s to TariffChangeStatusEnum
/// \returns a TariffChangeStatusEnum from a string representation
TariffChangeStatusEnum string_to_tariff_change_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffChangeStatusEnum \p tariff_change_status_enum to the
/// given output stream \p os
/// \returns an output stream with the TariffChangeStatusEnum written to
std::ostream& operator<<(std::ostream& os, const TariffChangeStatusEnum& tariff_change_status_enum);

// from: ClearCacheResponse
enum class ClearCacheStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given ClearCacheStatusEnum \p e to human readable string
/// \returns a string representation of the ClearCacheStatusEnum
std::string clear_cache_status_enum_to_string(ClearCacheStatusEnum e);

/// \brief Converts the given std::string \p s to ClearCacheStatusEnum
/// \returns a ClearCacheStatusEnum from a string representation
ClearCacheStatusEnum string_to_clear_cache_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearCacheStatusEnum \p clear_cache_status_enum to the given
/// output stream \p os
/// \returns an output stream with the ClearCacheStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ClearCacheStatusEnum& clear_cache_status_enum);

// from: ClearChargingProfileRequest
enum class ChargingProfilePurposeEnum {
    ChargingStationExternalConstraints,
    ChargingStationMaxProfile,
    TxDefaultProfile,
    TxProfile,
    PriorityCharging,
    LocalGeneration,
};

namespace conversions {
/// \brief Converts the given ChargingProfilePurposeEnum \p e to human readable string
/// \returns a string representation of the ChargingProfilePurposeEnum
std::string charging_profile_purpose_enum_to_string(ChargingProfilePurposeEnum e);

/// \brief Converts the given std::string \p s to ChargingProfilePurposeEnum
/// \returns a ChargingProfilePurposeEnum from a string representation
ChargingProfilePurposeEnum string_to_charging_profile_purpose_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfilePurposeEnum \p charging_profile_purpose_enum to
/// the given output stream \p os
/// \returns an output stream with the ChargingProfilePurposeEnum written to
std::ostream& operator<<(std::ostream& os, const ChargingProfilePurposeEnum& charging_profile_purpose_enum);

// from: ClearChargingProfileResponse
enum class ClearChargingProfileStatusEnum {
    Accepted,
    Unknown,
};

namespace conversions {
/// \brief Converts the given ClearChargingProfileStatusEnum \p e to human readable string
/// \returns a string representation of the ClearChargingProfileStatusEnum
std::string clear_charging_profile_status_enum_to_string(ClearChargingProfileStatusEnum e);

/// \brief Converts the given std::string \p s to ClearChargingProfileStatusEnum
/// \returns a ClearChargingProfileStatusEnum from a string representation
ClearChargingProfileStatusEnum string_to_clear_charging_profile_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearChargingProfileStatusEnum \p
/// clear_charging_profile_status_enum to the given output stream \p os
/// \returns an output stream with the ClearChargingProfileStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileStatusEnum& clear_charging_profile_status_enum);

// from: ClearDERControlRequest
enum class DERControlEnum {
    EnterService,
    FreqDroop,
    FreqWatt,
    FixedPFAbsorb,
    FixedPFInject,
    FixedVar,
    Gradients,
    HFMustTrip,
    HFMayTrip,
    HVMustTrip,
    HVMomCess,
    HVMayTrip,
    LimitMaxDischarge,
    LFMustTrip,
    LVMustTrip,
    LVMomCess,
    LVMayTrip,
    PowerMonitoringMustTrip,
    VoltVar,
    VoltWatt,
    WattPF,
    WattVar,
};

namespace conversions {
/// \brief Converts the given DERControlEnum \p e to human readable string
/// \returns a string representation of the DERControlEnum
std::string dercontrol_enum_to_string(DERControlEnum e);

/// \brief Converts the given std::string \p s to DERControlEnum
/// \returns a DERControlEnum from a string representation
DERControlEnum string_to_dercontrol_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DERControlEnum \p dercontrol_enum to the given output stream \p
/// os
/// \returns an output stream with the DERControlEnum written to
std::ostream& operator<<(std::ostream& os, const DERControlEnum& dercontrol_enum);

// from: ClearDERControlResponse
enum class DERControlStatusEnum {
    Accepted,
    Rejected,
    NotSupported,
    NotFound,
};

namespace conversions {
/// \brief Converts the given DERControlStatusEnum \p e to human readable string
/// \returns a string representation of the DERControlStatusEnum
std::string dercontrol_status_enum_to_string(DERControlStatusEnum e);

/// \brief Converts the given std::string \p s to DERControlStatusEnum
/// \returns a DERControlStatusEnum from a string representation
DERControlStatusEnum string_to_dercontrol_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DERControlStatusEnum \p dercontrol_status_enum to the given
/// output stream \p os
/// \returns an output stream with the DERControlStatusEnum written to
std::ostream& operator<<(std::ostream& os, const DERControlStatusEnum& dercontrol_status_enum);

// from: ClearDisplayMessageResponse
enum class ClearMessageStatusEnum {
    Accepted,
    Unknown,
    Rejected,
};

namespace conversions {
/// \brief Converts the given ClearMessageStatusEnum \p e to human readable string
/// \returns a string representation of the ClearMessageStatusEnum
std::string clear_message_status_enum_to_string(ClearMessageStatusEnum e);

/// \brief Converts the given std::string \p s to ClearMessageStatusEnum
/// \returns a ClearMessageStatusEnum from a string representation
ClearMessageStatusEnum string_to_clear_message_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearMessageStatusEnum \p clear_message_status_enum to the
/// given output stream \p os
/// \returns an output stream with the ClearMessageStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ClearMessageStatusEnum& clear_message_status_enum);

// from: ClearTariffsResponse
enum class TariffClearStatusEnum {
    Accepted,
    Rejected,
    NoTariff,
};

namespace conversions {
/// \brief Converts the given TariffClearStatusEnum \p e to human readable string
/// \returns a string representation of the TariffClearStatusEnum
std::string tariff_clear_status_enum_to_string(TariffClearStatusEnum e);

/// \brief Converts the given std::string \p s to TariffClearStatusEnum
/// \returns a TariffClearStatusEnum from a string representation
TariffClearStatusEnum string_to_tariff_clear_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffClearStatusEnum \p tariff_clear_status_enum to the given
/// output stream \p os
/// \returns an output stream with the TariffClearStatusEnum written to
std::ostream& operator<<(std::ostream& os, const TariffClearStatusEnum& tariff_clear_status_enum);

// from: ClearVariableMonitoringResponse
enum class ClearMonitoringStatusEnum {
    Accepted,
    Rejected,
    NotFound,
};

namespace conversions {
/// \brief Converts the given ClearMonitoringStatusEnum \p e to human readable string
/// \returns a string representation of the ClearMonitoringStatusEnum
std::string clear_monitoring_status_enum_to_string(ClearMonitoringStatusEnum e);

/// \brief Converts the given std::string \p s to ClearMonitoringStatusEnum
/// \returns a ClearMonitoringStatusEnum from a string representation
ClearMonitoringStatusEnum string_to_clear_monitoring_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearMonitoringStatusEnum \p clear_monitoring_status_enum to
/// the given output stream \p os
/// \returns an output stream with the ClearMonitoringStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ClearMonitoringStatusEnum& clear_monitoring_status_enum);

// from: CustomerInformationResponse
enum class CustomerInformationStatusEnum {
    Accepted,
    Rejected,
    Invalid,
};

namespace conversions {
/// \brief Converts the given CustomerInformationStatusEnum \p e to human readable string
/// \returns a string representation of the CustomerInformationStatusEnum
std::string customer_information_status_enum_to_string(CustomerInformationStatusEnum e);

/// \brief Converts the given std::string \p s to CustomerInformationStatusEnum
/// \returns a CustomerInformationStatusEnum from a string representation
CustomerInformationStatusEnum string_to_customer_information_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CustomerInformationStatusEnum \p
/// customer_information_status_enum to the given output stream \p os
/// \returns an output stream with the CustomerInformationStatusEnum written to
std::ostream& operator<<(std::ostream& os, const CustomerInformationStatusEnum& customer_information_status_enum);

// from: DataTransferResponse
enum class DataTransferStatusEnum {
    Accepted,
    Rejected,
    UnknownMessageId,
    UnknownVendorId,
};

namespace conversions {
/// \brief Converts the given DataTransferStatusEnum \p e to human readable string
/// \returns a string representation of the DataTransferStatusEnum
std::string data_transfer_status_enum_to_string(DataTransferStatusEnum e);

/// \brief Converts the given std::string \p s to DataTransferStatusEnum
/// \returns a DataTransferStatusEnum from a string representation
DataTransferStatusEnum string_to_data_transfer_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DataTransferStatusEnum \p data_transfer_status_enum to the
/// given output stream \p os
/// \returns an output stream with the DataTransferStatusEnum written to
std::ostream& operator<<(std::ostream& os, const DataTransferStatusEnum& data_transfer_status_enum);

// from: DeleteCertificateResponse
enum class DeleteCertificateStatusEnum {
    Accepted,
    Failed,
    NotFound,
};

namespace conversions {
/// \brief Converts the given DeleteCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the DeleteCertificateStatusEnum
std::string delete_certificate_status_enum_to_string(DeleteCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to DeleteCertificateStatusEnum
/// \returns a DeleteCertificateStatusEnum from a string representation
DeleteCertificateStatusEnum string_to_delete_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DeleteCertificateStatusEnum \p delete_certificate_status_enum
/// to the given output stream \p os
/// \returns an output stream with the DeleteCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateStatusEnum& delete_certificate_status_enum);

// from: FirmwareStatusNotificationRequest
enum class FirmwareStatusEnum {
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

namespace conversions {
/// \brief Converts the given FirmwareStatusEnum \p e to human readable string
/// \returns a string representation of the FirmwareStatusEnum
std::string firmware_status_enum_to_string(FirmwareStatusEnum e);

/// \brief Converts the given std::string \p s to FirmwareStatusEnum
/// \returns a FirmwareStatusEnum from a string representation
FirmwareStatusEnum string_to_firmware_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given FirmwareStatusEnum \p firmware_status_enum to the given output
/// stream \p os
/// \returns an output stream with the FirmwareStatusEnum written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusEnum& firmware_status_enum);

// from: Get15118EVCertificateRequest
enum class CertificateActionEnum {
    Install,
    Update,
};

namespace conversions {
/// \brief Converts the given CertificateActionEnum \p e to human readable string
/// \returns a string representation of the CertificateActionEnum
std::string certificate_action_enum_to_string(CertificateActionEnum e);

/// \brief Converts the given std::string \p s to CertificateActionEnum
/// \returns a CertificateActionEnum from a string representation
CertificateActionEnum string_to_certificate_action_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateActionEnum \p certificate_action_enum to the given
/// output stream \p os
/// \returns an output stream with the CertificateActionEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateActionEnum& certificate_action_enum);

// from: Get15118EVCertificateResponse
enum class Iso15118EVCertificateStatusEnum {
    Accepted,
    Failed,
};

namespace conversions {
/// \brief Converts the given Iso15118EVCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the Iso15118EVCertificateStatusEnum
std::string iso15118evcertificate_status_enum_to_string(Iso15118EVCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to Iso15118EVCertificateStatusEnum
/// \returns a Iso15118EVCertificateStatusEnum from a string representation
Iso15118EVCertificateStatusEnum string_to_iso15118evcertificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given Iso15118EVCertificateStatusEnum \p
/// iso15118evcertificate_status_enum to the given output stream \p os
/// \returns an output stream with the Iso15118EVCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const Iso15118EVCertificateStatusEnum& iso15118evcertificate_status_enum);

// from: GetBaseReportRequest
enum class ReportBaseEnum {
    ConfigurationInventory,
    FullInventory,
    SummaryInventory,
};

namespace conversions {
/// \brief Converts the given ReportBaseEnum \p e to human readable string
/// \returns a string representation of the ReportBaseEnum
std::string report_base_enum_to_string(ReportBaseEnum e);

/// \brief Converts the given std::string \p s to ReportBaseEnum
/// \returns a ReportBaseEnum from a string representation
ReportBaseEnum string_to_report_base_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReportBaseEnum \p report_base_enum to the given output stream
/// \p os
/// \returns an output stream with the ReportBaseEnum written to
std::ostream& operator<<(std::ostream& os, const ReportBaseEnum& report_base_enum);

// from: GetBaseReportResponse
enum class GenericDeviceModelStatusEnum {
    Accepted,
    Rejected,
    NotSupported,
    EmptyResultSet,
};

namespace conversions {
/// \brief Converts the given GenericDeviceModelStatusEnum \p e to human readable string
/// \returns a string representation of the GenericDeviceModelStatusEnum
std::string generic_device_model_status_enum_to_string(GenericDeviceModelStatusEnum e);

/// \brief Converts the given std::string \p s to GenericDeviceModelStatusEnum
/// \returns a GenericDeviceModelStatusEnum from a string representation
GenericDeviceModelStatusEnum string_to_generic_device_model_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GenericDeviceModelStatusEnum \p
/// generic_device_model_status_enum to the given output stream \p os
/// \returns an output stream with the GenericDeviceModelStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GenericDeviceModelStatusEnum& generic_device_model_status_enum);

// from: GetCertificateChainStatusRequest
enum class CertificateStatusSourceEnum {
    CRL,
    OCSP,
};

namespace conversions {
/// \brief Converts the given CertificateStatusSourceEnum \p e to human readable string
/// \returns a string representation of the CertificateStatusSourceEnum
std::string certificate_status_source_enum_to_string(CertificateStatusSourceEnum e);

/// \brief Converts the given std::string \p s to CertificateStatusSourceEnum
/// \returns a CertificateStatusSourceEnum from a string representation
CertificateStatusSourceEnum string_to_certificate_status_source_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateStatusSourceEnum \p certificate_status_source_enum
/// to the given output stream \p os
/// \returns an output stream with the CertificateStatusSourceEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateStatusSourceEnum& certificate_status_source_enum);

// from: GetCertificateChainStatusResponse
enum class CertificateStatusEnum {
    Good,
    Revoked,
    Unknown,
    Failed,
};

namespace conversions {
/// \brief Converts the given CertificateStatusEnum \p e to human readable string
/// \returns a string representation of the CertificateStatusEnum
std::string certificate_status_enum_to_string(CertificateStatusEnum e);

/// \brief Converts the given std::string \p s to CertificateStatusEnum
/// \returns a CertificateStatusEnum from a string representation
CertificateStatusEnum string_to_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateStatusEnum \p certificate_status_enum to the given
/// output stream \p os
/// \returns an output stream with the CertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateStatusEnum& certificate_status_enum);

// from: GetCertificateStatusResponse
enum class GetCertificateStatusEnum {
    Accepted,
    Failed,
};

namespace conversions {
/// \brief Converts the given GetCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the GetCertificateStatusEnum
std::string get_certificate_status_enum_to_string(GetCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to GetCertificateStatusEnum
/// \returns a GetCertificateStatusEnum from a string representation
GetCertificateStatusEnum string_to_get_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetCertificateStatusEnum \p get_certificate_status_enum to the
/// given output stream \p os
/// \returns an output stream with the GetCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GetCertificateStatusEnum& get_certificate_status_enum);

// from: GetChargingProfilesResponse
enum class GetChargingProfileStatusEnum {
    Accepted,
    NoProfiles,
};

namespace conversions {
/// \brief Converts the given GetChargingProfileStatusEnum \p e to human readable string
/// \returns a string representation of the GetChargingProfileStatusEnum
std::string get_charging_profile_status_enum_to_string(GetChargingProfileStatusEnum e);

/// \brief Converts the given std::string \p s to GetChargingProfileStatusEnum
/// \returns a GetChargingProfileStatusEnum from a string representation
GetChargingProfileStatusEnum string_to_get_charging_profile_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetChargingProfileStatusEnum \p
/// get_charging_profile_status_enum to the given output stream \p os
/// \returns an output stream with the GetChargingProfileStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GetChargingProfileStatusEnum& get_charging_profile_status_enum);

// from: GetCompositeScheduleRequest
enum class ChargingRateUnitEnum {
    W,
    A,
};

namespace conversions {
/// \brief Converts the given ChargingRateUnitEnum \p e to human readable string
/// \returns a string representation of the ChargingRateUnitEnum
std::string charging_rate_unit_enum_to_string(ChargingRateUnitEnum e);

/// \brief Converts the given std::string \p s to ChargingRateUnitEnum
/// \returns a ChargingRateUnitEnum from a string representation
ChargingRateUnitEnum string_to_charging_rate_unit_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingRateUnitEnum \p charging_rate_unit_enum to the given
/// output stream \p os
/// \returns an output stream with the ChargingRateUnitEnum written to
std::ostream& operator<<(std::ostream& os, const ChargingRateUnitEnum& charging_rate_unit_enum);

// from: GetCompositeScheduleResponse
enum class OperationModeEnum {
    Idle,
    ChargingOnly,
    CentralSetpoint,
    ExternalSetpoint,
    ExternalLimits,
    CentralFrequency,
    LocalFrequency,
    LocalLoadBalancing,
};

namespace conversions {
/// \brief Converts the given OperationModeEnum \p e to human readable string
/// \returns a string representation of the OperationModeEnum
std::string operation_mode_enum_to_string(OperationModeEnum e);

/// \brief Converts the given std::string \p s to OperationModeEnum
/// \returns a OperationModeEnum from a string representation
OperationModeEnum string_to_operation_mode_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OperationModeEnum \p operation_mode_enum to the given output
/// stream \p os
/// \returns an output stream with the OperationModeEnum written to
std::ostream& operator<<(std::ostream& os, const OperationModeEnum& operation_mode_enum);

// from: GetDisplayMessagesRequest
enum class MessagePriorityEnum {
    AlwaysFront,
    InFront,
    NormalCycle,
};

namespace conversions {
/// \brief Converts the given MessagePriorityEnum \p e to human readable string
/// \returns a string representation of the MessagePriorityEnum
std::string message_priority_enum_to_string(MessagePriorityEnum e);

/// \brief Converts the given std::string \p s to MessagePriorityEnum
/// \returns a MessagePriorityEnum from a string representation
MessagePriorityEnum string_to_message_priority_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessagePriorityEnum \p message_priority_enum to the given
/// output stream \p os
/// \returns an output stream with the MessagePriorityEnum written to
std::ostream& operator<<(std::ostream& os, const MessagePriorityEnum& message_priority_enum);

// from: GetDisplayMessagesRequest
enum class MessageStateEnum {
    Charging,
    Faulted,
    Idle,
    Unavailable,
    Suspended,
    Discharging,
};

namespace conversions {
/// \brief Converts the given MessageStateEnum \p e to human readable string
/// \returns a string representation of the MessageStateEnum
std::string message_state_enum_to_string(MessageStateEnum e);

/// \brief Converts the given std::string \p s to MessageStateEnum
/// \returns a MessageStateEnum from a string representation
MessageStateEnum string_to_message_state_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessageStateEnum \p message_state_enum to the given output
/// stream \p os
/// \returns an output stream with the MessageStateEnum written to
std::ostream& operator<<(std::ostream& os, const MessageStateEnum& message_state_enum);

// from: GetDisplayMessagesResponse
enum class GetDisplayMessagesStatusEnum {
    Accepted,
    Unknown,
};

namespace conversions {
/// \brief Converts the given GetDisplayMessagesStatusEnum \p e to human readable string
/// \returns a string representation of the GetDisplayMessagesStatusEnum
std::string get_display_messages_status_enum_to_string(GetDisplayMessagesStatusEnum e);

/// \brief Converts the given std::string \p s to GetDisplayMessagesStatusEnum
/// \returns a GetDisplayMessagesStatusEnum from a string representation
GetDisplayMessagesStatusEnum string_to_get_display_messages_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetDisplayMessagesStatusEnum \p
/// get_display_messages_status_enum to the given output stream \p os
/// \returns an output stream with the GetDisplayMessagesStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GetDisplayMessagesStatusEnum& get_display_messages_status_enum);

// from: GetInstalledCertificateIdsRequest
enum class GetCertificateIdUseEnum {
    V2GRootCertificate,
    MORootCertificate,
    CSMSRootCertificate,
    V2GCertificateChain,
    ManufacturerRootCertificate,
    OEMRootCertificate,
};

namespace conversions {
/// \brief Converts the given GetCertificateIdUseEnum \p e to human readable string
/// \returns a string representation of the GetCertificateIdUseEnum
std::string get_certificate_id_use_enum_to_string(GetCertificateIdUseEnum e);

/// \brief Converts the given std::string \p s to GetCertificateIdUseEnum
/// \returns a GetCertificateIdUseEnum from a string representation
GetCertificateIdUseEnum string_to_get_certificate_id_use_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetCertificateIdUseEnum \p get_certificate_id_use_enum to the
/// given output stream \p os
/// \returns an output stream with the GetCertificateIdUseEnum written to
std::ostream& operator<<(std::ostream& os, const GetCertificateIdUseEnum& get_certificate_id_use_enum);

// from: GetInstalledCertificateIdsResponse
enum class GetInstalledCertificateStatusEnum {
    Accepted,
    NotFound,
};

namespace conversions {
/// \brief Converts the given GetInstalledCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the GetInstalledCertificateStatusEnum
std::string get_installed_certificate_status_enum_to_string(GetInstalledCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to GetInstalledCertificateStatusEnum
/// \returns a GetInstalledCertificateStatusEnum from a string representation
GetInstalledCertificateStatusEnum string_to_get_installed_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetInstalledCertificateStatusEnum \p
/// get_installed_certificate_status_enum to the given output stream \p os
/// \returns an output stream with the GetInstalledCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os,
                         const GetInstalledCertificateStatusEnum& get_installed_certificate_status_enum);

// from: GetLogRequest
enum class LogEnum {
    DiagnosticsLog,
    SecurityLog,
    DataCollectorLog,
};

namespace conversions {
/// \brief Converts the given LogEnum \p e to human readable string
/// \returns a string representation of the LogEnum
std::string log_enum_to_string(LogEnum e);

/// \brief Converts the given std::string \p s to LogEnum
/// \returns a LogEnum from a string representation
LogEnum string_to_log_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given LogEnum \p log_enum to the given output stream \p os
/// \returns an output stream with the LogEnum written to
std::ostream& operator<<(std::ostream& os, const LogEnum& log_enum);

// from: GetLogResponse
enum class LogStatusEnum {
    Accepted,
    Rejected,
    AcceptedCanceled,
};

namespace conversions {
/// \brief Converts the given LogStatusEnum \p e to human readable string
/// \returns a string representation of the LogStatusEnum
std::string log_status_enum_to_string(LogStatusEnum e);

/// \brief Converts the given std::string \p s to LogStatusEnum
/// \returns a LogStatusEnum from a string representation
LogStatusEnum string_to_log_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given LogStatusEnum \p log_status_enum to the given output stream \p
/// os
/// \returns an output stream with the LogStatusEnum written to
std::ostream& operator<<(std::ostream& os, const LogStatusEnum& log_status_enum);

// from: GetMonitoringReportRequest
enum class MonitoringCriterionEnum {
    ThresholdMonitoring,
    DeltaMonitoring,
    PeriodicMonitoring,
};

namespace conversions {
/// \brief Converts the given MonitoringCriterionEnum \p e to human readable string
/// \returns a string representation of the MonitoringCriterionEnum
std::string monitoring_criterion_enum_to_string(MonitoringCriterionEnum e);

/// \brief Converts the given std::string \p s to MonitoringCriterionEnum
/// \returns a MonitoringCriterionEnum from a string representation
MonitoringCriterionEnum string_to_monitoring_criterion_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MonitoringCriterionEnum \p monitoring_criterion_enum to the
/// given output stream \p os
/// \returns an output stream with the MonitoringCriterionEnum written to
std::ostream& operator<<(std::ostream& os, const MonitoringCriterionEnum& monitoring_criterion_enum);

// from: GetReportRequest
enum class ComponentCriterionEnum {
    Active,
    Available,
    Enabled,
    Problem,
};

namespace conversions {
/// \brief Converts the given ComponentCriterionEnum \p e to human readable string
/// \returns a string representation of the ComponentCriterionEnum
std::string component_criterion_enum_to_string(ComponentCriterionEnum e);

/// \brief Converts the given std::string \p s to ComponentCriterionEnum
/// \returns a ComponentCriterionEnum from a string representation
ComponentCriterionEnum string_to_component_criterion_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ComponentCriterionEnum \p component_criterion_enum to the given
/// output stream \p os
/// \returns an output stream with the ComponentCriterionEnum written to
std::ostream& operator<<(std::ostream& os, const ComponentCriterionEnum& component_criterion_enum);

// from: GetTariffsResponse
enum class TariffGetStatusEnum {
    Accepted,
    Rejected,
    NoTariff,
};

namespace conversions {
/// \brief Converts the given TariffGetStatusEnum \p e to human readable string
/// \returns a string representation of the TariffGetStatusEnum
std::string tariff_get_status_enum_to_string(TariffGetStatusEnum e);

/// \brief Converts the given std::string \p s to TariffGetStatusEnum
/// \returns a TariffGetStatusEnum from a string representation
TariffGetStatusEnum string_to_tariff_get_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffGetStatusEnum \p tariff_get_status_enum to the given
/// output stream \p os
/// \returns an output stream with the TariffGetStatusEnum written to
std::ostream& operator<<(std::ostream& os, const TariffGetStatusEnum& tariff_get_status_enum);

// from: GetTariffsResponse
enum class TariffKindEnum {
    DefaultTariff,
    DriverTariff,
};

namespace conversions {
/// \brief Converts the given TariffKindEnum \p e to human readable string
/// \returns a string representation of the TariffKindEnum
std::string tariff_kind_enum_to_string(TariffKindEnum e);

/// \brief Converts the given std::string \p s to TariffKindEnum
/// \returns a TariffKindEnum from a string representation
TariffKindEnum string_to_tariff_kind_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffKindEnum \p tariff_kind_enum to the given output stream
/// \p os
/// \returns an output stream with the TariffKindEnum written to
std::ostream& operator<<(std::ostream& os, const TariffKindEnum& tariff_kind_enum);

// from: GetVariablesRequest
enum class AttributeEnum {
    Actual,
    Target,
    MinSet,
    MaxSet,
};

namespace conversions {
/// \brief Converts the given AttributeEnum \p e to human readable string
/// \returns a string representation of the AttributeEnum
std::string attribute_enum_to_string(AttributeEnum e);

/// \brief Converts the given std::string \p s to AttributeEnum
/// \returns a AttributeEnum from a string representation
AttributeEnum string_to_attribute_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AttributeEnum \p attribute_enum to the given output stream \p
/// os
/// \returns an output stream with the AttributeEnum written to
std::ostream& operator<<(std::ostream& os, const AttributeEnum& attribute_enum);

// from: GetVariablesResponse
enum class GetVariableStatusEnum {
    Accepted,
    Rejected,
    UnknownComponent,
    UnknownVariable,
    NotSupportedAttributeType,
};

namespace conversions {
/// \brief Converts the given GetVariableStatusEnum \p e to human readable string
/// \returns a string representation of the GetVariableStatusEnum
std::string get_variable_status_enum_to_string(GetVariableStatusEnum e);

/// \brief Converts the given std::string \p s to GetVariableStatusEnum
/// \returns a GetVariableStatusEnum from a string representation
GetVariableStatusEnum string_to_get_variable_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetVariableStatusEnum \p get_variable_status_enum to the given
/// output stream \p os
/// \returns an output stream with the GetVariableStatusEnum written to
std::ostream& operator<<(std::ostream& os, const GetVariableStatusEnum& get_variable_status_enum);

// from: InstallCertificateRequest
enum class InstallCertificateUseEnum {
    V2GRootCertificate,
    MORootCertificate,
    ManufacturerRootCertificate,
    CSMSRootCertificate,
    OEMRootCertificate,
};

namespace conversions {
/// \brief Converts the given InstallCertificateUseEnum \p e to human readable string
/// \returns a string representation of the InstallCertificateUseEnum
std::string install_certificate_use_enum_to_string(InstallCertificateUseEnum e);

/// \brief Converts the given std::string \p s to InstallCertificateUseEnum
/// \returns a InstallCertificateUseEnum from a string representation
InstallCertificateUseEnum string_to_install_certificate_use_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given InstallCertificateUseEnum \p install_certificate_use_enum to
/// the given output stream \p os
/// \returns an output stream with the InstallCertificateUseEnum written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateUseEnum& install_certificate_use_enum);

// from: InstallCertificateResponse
enum class InstallCertificateStatusEnum {
    Accepted,
    Rejected,
    Failed,
};

namespace conversions {
/// \brief Converts the given InstallCertificateStatusEnum \p e to human readable string
/// \returns a string representation of the InstallCertificateStatusEnum
std::string install_certificate_status_enum_to_string(InstallCertificateStatusEnum e);

/// \brief Converts the given std::string \p s to InstallCertificateStatusEnum
/// \returns a InstallCertificateStatusEnum from a string representation
InstallCertificateStatusEnum string_to_install_certificate_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given InstallCertificateStatusEnum \p install_certificate_status_enum
/// to the given output stream \p os
/// \returns an output stream with the InstallCertificateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateStatusEnum& install_certificate_status_enum);

// from: LogStatusNotificationRequest
enum class UploadLogStatusEnum {
    BadMessage,
    Idle,
    NotSupportedOperation,
    PermissionDenied,
    Uploaded,
    UploadFailure,
    Uploading,
    AcceptedCanceled,
};

namespace conversions {
/// \brief Converts the given UploadLogStatusEnum \p e to human readable string
/// \returns a string representation of the UploadLogStatusEnum
std::string upload_log_status_enum_to_string(UploadLogStatusEnum e);

/// \brief Converts the given std::string \p s to UploadLogStatusEnum
/// \returns a UploadLogStatusEnum from a string representation
UploadLogStatusEnum string_to_upload_log_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UploadLogStatusEnum \p upload_log_status_enum to the given
/// output stream \p os
/// \returns an output stream with the UploadLogStatusEnum written to
std::ostream& operator<<(std::ostream& os, const UploadLogStatusEnum& upload_log_status_enum);

// from: MeterValuesRequest
enum class MeasurandEnum {
    Current_Export,
    Current_Export_Offered,
    Current_Export_Minimum,
    Current_Import,
    Current_Import_Offered,
    Current_Import_Minimum,
    Current_Offered,
    Display_PresentSOC,
    Display_MinimumSOC,
    Display_TargetSOC,
    Display_MaximumSOC,
    Display_RemainingTimeToMinimumSOC,
    Display_RemainingTimeToTargetSOC,
    Display_RemainingTimeToMaximumSOC,
    Display_ChargingComplete,
    Display_BatteryEnergyCapacity,
    Display_InletHot,
    Energy_Active_Export_Interval,
    Energy_Active_Export_Register,
    Energy_Active_Import_Interval,
    Energy_Active_Import_Register,
    Energy_Active_Import_CableLoss,
    Energy_Active_Import_LocalGeneration_Register,
    Energy_Active_Net,
    Energy_Active_Setpoint_Interval,
    Energy_Apparent_Export,
    Energy_Apparent_Import,
    Energy_Apparent_Net,
    Energy_Reactive_Export_Interval,
    Energy_Reactive_Export_Register,
    Energy_Reactive_Import_Interval,
    Energy_Reactive_Import_Register,
    Energy_Reactive_Net,
    EnergyRequest_Target,
    EnergyRequest_Minimum,
    EnergyRequest_Maximum,
    EnergyRequest_Minimum_V2X,
    EnergyRequest_Maximum_V2X,
    EnergyRequest_Bulk,
    Frequency,
    Power_Active_Export,
    Power_Active_Import,
    Power_Active_Setpoint,
    Power_Active_Residual,
    Power_Export_Minimum,
    Power_Export_Offered,
    Power_Factor,
    Power_Import_Offered,
    Power_Import_Minimum,
    Power_Offered,
    Power_Reactive_Export,
    Power_Reactive_Import,
    SoC,
    Voltage,
    Voltage_Minimum,
    Voltage_Maximum,
};

namespace conversions {
/// \brief Converts the given MeasurandEnum \p e to human readable string
/// \returns a string representation of the MeasurandEnum
std::string measurand_enum_to_string(MeasurandEnum e);

/// \brief Converts the given std::string \p s to MeasurandEnum
/// \returns a MeasurandEnum from a string representation
MeasurandEnum string_to_measurand_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MeasurandEnum \p measurand_enum to the given output stream \p
/// os
/// \returns an output stream with the MeasurandEnum written to
std::ostream& operator<<(std::ostream& os, const MeasurandEnum& measurand_enum);

// from: MeterValuesRequest
enum class ReadingContextEnum {
    Interruption_Begin,
    Interruption_End,
    Other,
    Sample_Clock,
    Sample_Periodic,
    Transaction_Begin,
    Transaction_End,
    Trigger,
};

namespace conversions {
/// \brief Converts the given ReadingContextEnum \p e to human readable string
/// \returns a string representation of the ReadingContextEnum
std::string reading_context_enum_to_string(ReadingContextEnum e);

/// \brief Converts the given std::string \p s to ReadingContextEnum
/// \returns a ReadingContextEnum from a string representation
ReadingContextEnum string_to_reading_context_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReadingContextEnum \p reading_context_enum to the given output
/// stream \p os
/// \returns an output stream with the ReadingContextEnum written to
std::ostream& operator<<(std::ostream& os, const ReadingContextEnum& reading_context_enum);

// from: MeterValuesRequest
enum class PhaseEnum {
    L1,
    L2,
    L3,
    N,
    L1_N,
    L2_N,
    L3_N,
    L1_L2,
    L2_L3,
    L3_L1,
};

namespace conversions {
/// \brief Converts the given PhaseEnum \p e to human readable string
/// \returns a string representation of the PhaseEnum
std::string phase_enum_to_string(PhaseEnum e);

/// \brief Converts the given std::string \p s to PhaseEnum
/// \returns a PhaseEnum from a string representation
PhaseEnum string_to_phase_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PhaseEnum \p phase_enum to the given output stream \p os
/// \returns an output stream with the PhaseEnum written to
std::ostream& operator<<(std::ostream& os, const PhaseEnum& phase_enum);

// from: MeterValuesRequest
enum class LocationEnum {
    Body,
    Cable,
    EV,
    Inlet,
    Outlet,
    Upstream,
};

namespace conversions {
/// \brief Converts the given LocationEnum \p e to human readable string
/// \returns a string representation of the LocationEnum
std::string location_enum_to_string(LocationEnum e);

/// \brief Converts the given std::string \p s to LocationEnum
/// \returns a LocationEnum from a string representation
LocationEnum string_to_location_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given LocationEnum \p location_enum to the given output stream \p os
/// \returns an output stream with the LocationEnum written to
std::ostream& operator<<(std::ostream& os, const LocationEnum& location_enum);

// from: NotifyAllowedEnergyTransferResponse
enum class NotifyAllowedEnergyTransferStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given NotifyAllowedEnergyTransferStatusEnum \p e to human readable string
/// \returns a string representation of the NotifyAllowedEnergyTransferStatusEnum
std::string notify_allowed_energy_transfer_status_enum_to_string(NotifyAllowedEnergyTransferStatusEnum e);

/// \brief Converts the given std::string \p s to NotifyAllowedEnergyTransferStatusEnum
/// \returns a NotifyAllowedEnergyTransferStatusEnum from a string representation
NotifyAllowedEnergyTransferStatusEnum string_to_notify_allowed_energy_transfer_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given NotifyAllowedEnergyTransferStatusEnum \p
/// notify_allowed_energy_transfer_status_enum to the given output stream \p os
/// \returns an output stream with the NotifyAllowedEnergyTransferStatusEnum written to
std::ostream& operator<<(std::ostream& os,
                         const NotifyAllowedEnergyTransferStatusEnum& notify_allowed_energy_transfer_status_enum);

// from: NotifyChargingLimitRequest
enum class CostKindEnum {
    CarbonDioxideEmission,
    RelativePricePercentage,
    RenewableGenerationPercentage,
};

namespace conversions {
/// \brief Converts the given CostKindEnum \p e to human readable string
/// \returns a string representation of the CostKindEnum
std::string cost_kind_enum_to_string(CostKindEnum e);

/// \brief Converts the given std::string \p s to CostKindEnum
/// \returns a CostKindEnum from a string representation
CostKindEnum string_to_cost_kind_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CostKindEnum \p cost_kind_enum to the given output stream \p os
/// \returns an output stream with the CostKindEnum written to
std::ostream& operator<<(std::ostream& os, const CostKindEnum& cost_kind_enum);

// from: NotifyDERAlarmRequest
enum class GridEventFaultEnum {
    CurrentImbalance,
    LocalEmergency,
    LowInputPower,
    OverCurrent,
    OverFrequency,
    OverVoltage,
    PhaseRotation,
    RemoteEmergency,
    UnderFrequency,
    UnderVoltage,
    VoltageImbalance,
};

namespace conversions {
/// \brief Converts the given GridEventFaultEnum \p e to human readable string
/// \returns a string representation of the GridEventFaultEnum
std::string grid_event_fault_enum_to_string(GridEventFaultEnum e);

/// \brief Converts the given std::string \p s to GridEventFaultEnum
/// \returns a GridEventFaultEnum from a string representation
GridEventFaultEnum string_to_grid_event_fault_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GridEventFaultEnum \p grid_event_fault_enum to the given output
/// stream \p os
/// \returns an output stream with the GridEventFaultEnum written to
std::ostream& operator<<(std::ostream& os, const GridEventFaultEnum& grid_event_fault_enum);

// from: NotifyEVChargingNeedsRequest
enum class IslandingDetectionEnum {
    NoAntiIslandingSupport,
    RoCoF,
    UVP_OVP,
    UFP_OFP,
    VoltageVectorShift,
    ZeroCrossingDetection,
    OtherPassive,
    ImpedanceMeasurement,
    ImpedanceAtFrequency,
    SlipModeFrequencyShift,
    SandiaFrequencyShift,
    SandiaVoltageShift,
    FrequencyJump,
    RCLQFactor,
    OtherActive,
};

namespace conversions {
/// \brief Converts the given IslandingDetectionEnum \p e to human readable string
/// \returns a string representation of the IslandingDetectionEnum
std::string islanding_detection_enum_to_string(IslandingDetectionEnum e);

/// \brief Converts the given std::string \p s to IslandingDetectionEnum
/// \returns a IslandingDetectionEnum from a string representation
IslandingDetectionEnum string_to_islanding_detection_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given IslandingDetectionEnum \p islanding_detection_enum to the given
/// output stream \p os
/// \returns an output stream with the IslandingDetectionEnum written to
std::ostream& operator<<(std::ostream& os, const IslandingDetectionEnum& islanding_detection_enum);

// from: NotifyEVChargingNeedsRequest
enum class ControlModeEnum {
    ScheduledControl,
    DynamicControl,
};

namespace conversions {
/// \brief Converts the given ControlModeEnum \p e to human readable string
/// \returns a string representation of the ControlModeEnum
std::string control_mode_enum_to_string(ControlModeEnum e);

/// \brief Converts the given std::string \p s to ControlModeEnum
/// \returns a ControlModeEnum from a string representation
ControlModeEnum string_to_control_mode_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ControlModeEnum \p control_mode_enum to the given output stream
/// \p os
/// \returns an output stream with the ControlModeEnum written to
std::ostream& operator<<(std::ostream& os, const ControlModeEnum& control_mode_enum);

// from: NotifyEVChargingNeedsRequest
enum class MobilityNeedsModeEnum {
    EVCC,
    EVCC_SECC,
};

namespace conversions {
/// \brief Converts the given MobilityNeedsModeEnum \p e to human readable string
/// \returns a string representation of the MobilityNeedsModeEnum
std::string mobility_needs_mode_enum_to_string(MobilityNeedsModeEnum e);

/// \brief Converts the given std::string \p s to MobilityNeedsModeEnum
/// \returns a MobilityNeedsModeEnum from a string representation
MobilityNeedsModeEnum string_to_mobility_needs_mode_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MobilityNeedsModeEnum \p mobility_needs_mode_enum to the given
/// output stream \p os
/// \returns an output stream with the MobilityNeedsModeEnum written to
std::ostream& operator<<(std::ostream& os, const MobilityNeedsModeEnum& mobility_needs_mode_enum);

// from: NotifyEVChargingNeedsResponse
enum class NotifyEVChargingNeedsStatusEnum {
    Accepted,
    Rejected,
    Processing,
    NoChargingProfile,
};

namespace conversions {
/// \brief Converts the given NotifyEVChargingNeedsStatusEnum \p e to human readable string
/// \returns a string representation of the NotifyEVChargingNeedsStatusEnum
std::string notify_evcharging_needs_status_enum_to_string(NotifyEVChargingNeedsStatusEnum e);

/// \brief Converts the given std::string \p s to NotifyEVChargingNeedsStatusEnum
/// \returns a NotifyEVChargingNeedsStatusEnum from a string representation
NotifyEVChargingNeedsStatusEnum string_to_notify_evcharging_needs_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given NotifyEVChargingNeedsStatusEnum \p
/// notify_evcharging_needs_status_enum to the given output stream \p os
/// \returns an output stream with the NotifyEVChargingNeedsStatusEnum written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsStatusEnum& notify_evcharging_needs_status_enum);

// from: NotifyEventRequest
enum class EventTriggerEnum {
    Alerting,
    Delta,
    Periodic,
};

namespace conversions {
/// \brief Converts the given EventTriggerEnum \p e to human readable string
/// \returns a string representation of the EventTriggerEnum
std::string event_trigger_enum_to_string(EventTriggerEnum e);

/// \brief Converts the given std::string \p s to EventTriggerEnum
/// \returns a EventTriggerEnum from a string representation
EventTriggerEnum string_to_event_trigger_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given EventTriggerEnum \p event_trigger_enum to the given output
/// stream \p os
/// \returns an output stream with the EventTriggerEnum written to
std::ostream& operator<<(std::ostream& os, const EventTriggerEnum& event_trigger_enum);

// from: NotifyEventRequest
enum class EventNotificationEnum {
    HardWiredNotification,
    HardWiredMonitor,
    PreconfiguredMonitor,
    CustomMonitor,
};

namespace conversions {
/// \brief Converts the given EventNotificationEnum \p e to human readable string
/// \returns a string representation of the EventNotificationEnum
std::string event_notification_enum_to_string(EventNotificationEnum e);

/// \brief Converts the given std::string \p s to EventNotificationEnum
/// \returns a EventNotificationEnum from a string representation
EventNotificationEnum string_to_event_notification_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given EventNotificationEnum \p event_notification_enum to the given
/// output stream \p os
/// \returns an output stream with the EventNotificationEnum written to
std::ostream& operator<<(std::ostream& os, const EventNotificationEnum& event_notification_enum);

// from: NotifyMonitoringReportRequest
enum class MonitorEnum {
    UpperThreshold,
    LowerThreshold,
    Delta,
    Periodic,
    PeriodicClockAligned,
    TargetDelta,
    TargetDeltaRelative,
};

namespace conversions {
/// \brief Converts the given MonitorEnum \p e to human readable string
/// \returns a string representation of the MonitorEnum
std::string monitor_enum_to_string(MonitorEnum e);

/// \brief Converts the given std::string \p s to MonitorEnum
/// \returns a MonitorEnum from a string representation
MonitorEnum string_to_monitor_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MonitorEnum \p monitor_enum to the given output stream \p os
/// \returns an output stream with the MonitorEnum written to
std::ostream& operator<<(std::ostream& os, const MonitorEnum& monitor_enum);

// from: NotifyReportRequest
enum class MutabilityEnum {
    ReadOnly,
    WriteOnly,
    ReadWrite,
};

namespace conversions {
/// \brief Converts the given MutabilityEnum \p e to human readable string
/// \returns a string representation of the MutabilityEnum
std::string mutability_enum_to_string(MutabilityEnum e);

/// \brief Converts the given std::string \p s to MutabilityEnum
/// \returns a MutabilityEnum from a string representation
MutabilityEnum string_to_mutability_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MutabilityEnum \p mutability_enum to the given output stream \p
/// os
/// \returns an output stream with the MutabilityEnum written to
std::ostream& operator<<(std::ostream& os, const MutabilityEnum& mutability_enum);

// from: NotifyReportRequest
enum class DataEnum {
    string,
    decimal,
    integer,
    dateTime,
    boolean,
    OptionList,
    SequenceList,
    MemberList,
};

namespace conversions {
/// \brief Converts the given DataEnum \p e to human readable string
/// \returns a string representation of the DataEnum
std::string data_enum_to_string(DataEnum e);

/// \brief Converts the given std::string \p s to DataEnum
/// \returns a DataEnum from a string representation
DataEnum string_to_data_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DataEnum \p data_enum to the given output stream \p os
/// \returns an output stream with the DataEnum written to
std::ostream& operator<<(std::ostream& os, const DataEnum& data_enum);

// from: NotifySettlementRequest
enum class PaymentStatusEnum {
    Settled,
    Canceled,
    Rejected,
    Failed,
};

namespace conversions {
/// \brief Converts the given PaymentStatusEnum \p e to human readable string
/// \returns a string representation of the PaymentStatusEnum
std::string payment_status_enum_to_string(PaymentStatusEnum e);

/// \brief Converts the given std::string \p s to PaymentStatusEnum
/// \returns a PaymentStatusEnum from a string representation
PaymentStatusEnum string_to_payment_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PaymentStatusEnum \p payment_status_enum to the given output
/// stream \p os
/// \returns an output stream with the PaymentStatusEnum written to
std::ostream& operator<<(std::ostream& os, const PaymentStatusEnum& payment_status_enum);

// from: PublishFirmwareStatusNotificationRequest
enum class PublishFirmwareStatusEnum {
    Idle,
    DownloadScheduled,
    Downloading,
    Downloaded,
    Published,
    DownloadFailed,
    DownloadPaused,
    InvalidChecksum,
    ChecksumVerified,
    PublishFailed,
};

namespace conversions {
/// \brief Converts the given PublishFirmwareStatusEnum \p e to human readable string
/// \returns a string representation of the PublishFirmwareStatusEnum
std::string publish_firmware_status_enum_to_string(PublishFirmwareStatusEnum e);

/// \brief Converts the given std::string \p s to PublishFirmwareStatusEnum
/// \returns a PublishFirmwareStatusEnum from a string representation
PublishFirmwareStatusEnum string_to_publish_firmware_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PublishFirmwareStatusEnum \p publish_firmware_status_enum to
/// the given output stream \p os
/// \returns an output stream with the PublishFirmwareStatusEnum written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusEnum& publish_firmware_status_enum);

// from: PullDynamicScheduleUpdateResponse
enum class ChargingProfileStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given ChargingProfileStatusEnum \p e to human readable string
/// \returns a string representation of the ChargingProfileStatusEnum
std::string charging_profile_status_enum_to_string(ChargingProfileStatusEnum e);

/// \brief Converts the given std::string \p s to ChargingProfileStatusEnum
/// \returns a ChargingProfileStatusEnum from a string representation
ChargingProfileStatusEnum string_to_charging_profile_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileStatusEnum \p charging_profile_status_enum to
/// the given output stream \p os
/// \returns an output stream with the ChargingProfileStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileStatusEnum& charging_profile_status_enum);

// from: ReportChargingProfilesRequest
enum class ChargingProfileKindEnum {
    Absolute,
    Recurring,
    Relative,
    Dynamic,
};

namespace conversions {
/// \brief Converts the given ChargingProfileKindEnum \p e to human readable string
/// \returns a string representation of the ChargingProfileKindEnum
std::string charging_profile_kind_enum_to_string(ChargingProfileKindEnum e);

/// \brief Converts the given std::string \p s to ChargingProfileKindEnum
/// \returns a ChargingProfileKindEnum from a string representation
ChargingProfileKindEnum string_to_charging_profile_kind_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileKindEnum \p charging_profile_kind_enum to the
/// given output stream \p os
/// \returns an output stream with the ChargingProfileKindEnum written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileKindEnum& charging_profile_kind_enum);

// from: ReportChargingProfilesRequest
enum class RecurrencyKindEnum {
    Daily,
    Weekly,
};

namespace conversions {
/// \brief Converts the given RecurrencyKindEnum \p e to human readable string
/// \returns a string representation of the RecurrencyKindEnum
std::string recurrency_kind_enum_to_string(RecurrencyKindEnum e);

/// \brief Converts the given std::string \p s to RecurrencyKindEnum
/// \returns a RecurrencyKindEnum from a string representation
RecurrencyKindEnum string_to_recurrency_kind_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RecurrencyKindEnum \p recurrency_kind_enum to the given output
/// stream \p os
/// \returns an output stream with the RecurrencyKindEnum written to
std::ostream& operator<<(std::ostream& os, const RecurrencyKindEnum& recurrency_kind_enum);

// from: ReportDERControlRequest
enum class PowerDuringCessationEnum {
    Active,
    Reactive,
};

namespace conversions {
/// \brief Converts the given PowerDuringCessationEnum \p e to human readable string
/// \returns a string representation of the PowerDuringCessationEnum
std::string power_during_cessation_enum_to_string(PowerDuringCessationEnum e);

/// \brief Converts the given std::string \p s to PowerDuringCessationEnum
/// \returns a PowerDuringCessationEnum from a string representation
PowerDuringCessationEnum string_to_power_during_cessation_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PowerDuringCessationEnum \p power_during_cessation_enum to the
/// given output stream \p os
/// \returns an output stream with the PowerDuringCessationEnum written to
std::ostream& operator<<(std::ostream& os, const PowerDuringCessationEnum& power_during_cessation_enum);

// from: ReportDERControlRequest
enum class DERUnitEnum {
    Not_Applicable,
    PctMaxW,
    PctMaxVar,
    PctWAvail,
    PctVarAvail,
    PctEffectiveV,
};

namespace conversions {
/// \brief Converts the given DERUnitEnum \p e to human readable string
/// \returns a string representation of the DERUnitEnum
std::string derunit_enum_to_string(DERUnitEnum e);

/// \brief Converts the given std::string \p s to DERUnitEnum
/// \returns a DERUnitEnum from a string representation
DERUnitEnum string_to_derunit_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DERUnitEnum \p derunit_enum to the given output stream \p os
/// \returns an output stream with the DERUnitEnum written to
std::ostream& operator<<(std::ostream& os, const DERUnitEnum& derunit_enum);

// from: RequestStartTransactionResponse
enum class RequestStartStopStatusEnum {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given RequestStartStopStatusEnum \p e to human readable string
/// \returns a string representation of the RequestStartStopStatusEnum
std::string request_start_stop_status_enum_to_string(RequestStartStopStatusEnum e);

/// \brief Converts the given std::string \p s to RequestStartStopStatusEnum
/// \returns a RequestStartStopStatusEnum from a string representation
RequestStartStopStatusEnum string_to_request_start_stop_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RequestStartStopStatusEnum \p request_start_stop_status_enum to
/// the given output stream \p os
/// \returns an output stream with the RequestStartStopStatusEnum written to
std::ostream& operator<<(std::ostream& os, const RequestStartStopStatusEnum& request_start_stop_status_enum);

// from: ReservationStatusUpdateRequest
enum class ReservationUpdateStatusEnum {
    Expired,
    Removed,
    NoTransaction,
};

namespace conversions {
/// \brief Converts the given ReservationUpdateStatusEnum \p e to human readable string
/// \returns a string representation of the ReservationUpdateStatusEnum
std::string reservation_update_status_enum_to_string(ReservationUpdateStatusEnum e);

/// \brief Converts the given std::string \p s to ReservationUpdateStatusEnum
/// \returns a ReservationUpdateStatusEnum from a string representation
ReservationUpdateStatusEnum string_to_reservation_update_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReservationUpdateStatusEnum \p reservation_update_status_enum
/// to the given output stream \p os
/// \returns an output stream with the ReservationUpdateStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ReservationUpdateStatusEnum& reservation_update_status_enum);

// from: ReserveNowResponse
enum class ReserveNowStatusEnum {
    Accepted,
    Faulted,
    Occupied,
    Rejected,
    Unavailable,
};

namespace conversions {
/// \brief Converts the given ReserveNowStatusEnum \p e to human readable string
/// \returns a string representation of the ReserveNowStatusEnum
std::string reserve_now_status_enum_to_string(ReserveNowStatusEnum e);

/// \brief Converts the given std::string \p s to ReserveNowStatusEnum
/// \returns a ReserveNowStatusEnum from a string representation
ReserveNowStatusEnum string_to_reserve_now_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReserveNowStatusEnum \p reserve_now_status_enum to the given
/// output stream \p os
/// \returns an output stream with the ReserveNowStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ReserveNowStatusEnum& reserve_now_status_enum);

// from: ResetRequest
enum class ResetEnum {
    Immediate,
    OnIdle,
    ImmediateAndResume,
};

namespace conversions {
/// \brief Converts the given ResetEnum \p e to human readable string
/// \returns a string representation of the ResetEnum
std::string reset_enum_to_string(ResetEnum e);

/// \brief Converts the given std::string \p s to ResetEnum
/// \returns a ResetEnum from a string representation
ResetEnum string_to_reset_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ResetEnum \p reset_enum to the given output stream \p os
/// \returns an output stream with the ResetEnum written to
std::ostream& operator<<(std::ostream& os, const ResetEnum& reset_enum);

// from: ResetResponse
enum class ResetStatusEnum {
    Accepted,
    Rejected,
    Scheduled,
};

namespace conversions {
/// \brief Converts the given ResetStatusEnum \p e to human readable string
/// \returns a string representation of the ResetStatusEnum
std::string reset_status_enum_to_string(ResetStatusEnum e);

/// \brief Converts the given std::string \p s to ResetStatusEnum
/// \returns a ResetStatusEnum from a string representation
ResetStatusEnum string_to_reset_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ResetStatusEnum \p reset_status_enum to the given output stream
/// \p os
/// \returns an output stream with the ResetStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ResetStatusEnum& reset_status_enum);

// from: SendLocalListRequest
enum class UpdateEnum {
    Differential,
    Full,
};

namespace conversions {
/// \brief Converts the given UpdateEnum \p e to human readable string
/// \returns a string representation of the UpdateEnum
std::string update_enum_to_string(UpdateEnum e);

/// \brief Converts the given std::string \p s to UpdateEnum
/// \returns a UpdateEnum from a string representation
UpdateEnum string_to_update_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UpdateEnum \p update_enum to the given output stream \p os
/// \returns an output stream with the UpdateEnum written to
std::ostream& operator<<(std::ostream& os, const UpdateEnum& update_enum);

// from: SendLocalListResponse
enum class SendLocalListStatusEnum {
    Accepted,
    Failed,
    VersionMismatch,
};

namespace conversions {
/// \brief Converts the given SendLocalListStatusEnum \p e to human readable string
/// \returns a string representation of the SendLocalListStatusEnum
std::string send_local_list_status_enum_to_string(SendLocalListStatusEnum e);

/// \brief Converts the given std::string \p s to SendLocalListStatusEnum
/// \returns a SendLocalListStatusEnum from a string representation
SendLocalListStatusEnum string_to_send_local_list_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given SendLocalListStatusEnum \p send_local_list_status_enum to the
/// given output stream \p os
/// \returns an output stream with the SendLocalListStatusEnum written to
std::ostream& operator<<(std::ostream& os, const SendLocalListStatusEnum& send_local_list_status_enum);

// from: SetDefaultTariffResponse
enum class TariffSetStatusEnum {
    Accepted,
    Rejected,
    TooManyElements,
    ConditionNotSupported,
    DuplicateTariffId,
};

namespace conversions {
/// \brief Converts the given TariffSetStatusEnum \p e to human readable string
/// \returns a string representation of the TariffSetStatusEnum
std::string tariff_set_status_enum_to_string(TariffSetStatusEnum e);

/// \brief Converts the given std::string \p s to TariffSetStatusEnum
/// \returns a TariffSetStatusEnum from a string representation
TariffSetStatusEnum string_to_tariff_set_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffSetStatusEnum \p tariff_set_status_enum to the given
/// output stream \p os
/// \returns an output stream with the TariffSetStatusEnum written to
std::ostream& operator<<(std::ostream& os, const TariffSetStatusEnum& tariff_set_status_enum);

// from: SetDisplayMessageResponse
enum class DisplayMessageStatusEnum {
    Accepted,
    NotSupportedMessageFormat,
    Rejected,
    NotSupportedPriority,
    NotSupportedState,
    UnknownTransaction,
    LanguageNotSupported,
};

namespace conversions {
/// \brief Converts the given DisplayMessageStatusEnum \p e to human readable string
/// \returns a string representation of the DisplayMessageStatusEnum
std::string display_message_status_enum_to_string(DisplayMessageStatusEnum e);

/// \brief Converts the given std::string \p s to DisplayMessageStatusEnum
/// \returns a DisplayMessageStatusEnum from a string representation
DisplayMessageStatusEnum string_to_display_message_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DisplayMessageStatusEnum \p display_message_status_enum to the
/// given output stream \p os
/// \returns an output stream with the DisplayMessageStatusEnum written to
std::ostream& operator<<(std::ostream& os, const DisplayMessageStatusEnum& display_message_status_enum);

// from: SetMonitoringBaseRequest
enum class MonitoringBaseEnum {
    All,
    FactoryDefault,
    HardWiredOnly,
};

namespace conversions {
/// \brief Converts the given MonitoringBaseEnum \p e to human readable string
/// \returns a string representation of the MonitoringBaseEnum
std::string monitoring_base_enum_to_string(MonitoringBaseEnum e);

/// \brief Converts the given std::string \p s to MonitoringBaseEnum
/// \returns a MonitoringBaseEnum from a string representation
MonitoringBaseEnum string_to_monitoring_base_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MonitoringBaseEnum \p monitoring_base_enum to the given output
/// stream \p os
/// \returns an output stream with the MonitoringBaseEnum written to
std::ostream& operator<<(std::ostream& os, const MonitoringBaseEnum& monitoring_base_enum);

// from: SetNetworkProfileRequest
enum class APNAuthenticationEnum {
    PAP,
    CHAP,
    NONE,
    AUTO,
};

namespace conversions {
/// \brief Converts the given APNAuthenticationEnum \p e to human readable string
/// \returns a string representation of the APNAuthenticationEnum
std::string apnauthentication_enum_to_string(APNAuthenticationEnum e);

/// \brief Converts the given std::string \p s to APNAuthenticationEnum
/// \returns a APNAuthenticationEnum from a string representation
APNAuthenticationEnum string_to_apnauthentication_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given APNAuthenticationEnum \p apnauthentication_enum to the given
/// output stream \p os
/// \returns an output stream with the APNAuthenticationEnum written to
std::ostream& operator<<(std::ostream& os, const APNAuthenticationEnum& apnauthentication_enum);

// from: SetNetworkProfileRequest
enum class OCPPVersionEnum {
    OCPP12,
    OCPP15,
    OCPP16,
    OCPP20,
    OCPP201,
    OCPP21,
};

namespace conversions {
/// \brief Converts the given OCPPVersionEnum \p e to human readable string
/// \returns a string representation of the OCPPVersionEnum
std::string ocppversion_enum_to_string(OCPPVersionEnum e);

/// \brief Converts the given std::string \p s to OCPPVersionEnum
/// \returns a OCPPVersionEnum from a string representation
OCPPVersionEnum string_to_ocppversion_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OCPPVersionEnum \p ocppversion_enum to the given output stream
/// \p os
/// \returns an output stream with the OCPPVersionEnum written to
std::ostream& operator<<(std::ostream& os, const OCPPVersionEnum& ocppversion_enum);

// from: SetNetworkProfileRequest
enum class OCPPInterfaceEnum {
    Wired0,
    Wired1,
    Wired2,
    Wired3,
    Wireless0,
    Wireless1,
    Wireless2,
    Wireless3,
    Any,
};

namespace conversions {
/// \brief Converts the given OCPPInterfaceEnum \p e to human readable string
/// \returns a string representation of the OCPPInterfaceEnum
std::string ocppinterface_enum_to_string(OCPPInterfaceEnum e);

/// \brief Converts the given std::string \p s to OCPPInterfaceEnum
/// \returns a OCPPInterfaceEnum from a string representation
OCPPInterfaceEnum string_to_ocppinterface_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OCPPInterfaceEnum \p ocppinterface_enum to the given output
/// stream \p os
/// \returns an output stream with the OCPPInterfaceEnum written to
std::ostream& operator<<(std::ostream& os, const OCPPInterfaceEnum& ocppinterface_enum);

// from: SetNetworkProfileRequest
enum class OCPPTransportEnum {
    SOAP,
    JSON,
};

namespace conversions {
/// \brief Converts the given OCPPTransportEnum \p e to human readable string
/// \returns a string representation of the OCPPTransportEnum
std::string ocpptransport_enum_to_string(OCPPTransportEnum e);

/// \brief Converts the given std::string \p s to OCPPTransportEnum
/// \returns a OCPPTransportEnum from a string representation
OCPPTransportEnum string_to_ocpptransport_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OCPPTransportEnum \p ocpptransport_enum to the given output
/// stream \p os
/// \returns an output stream with the OCPPTransportEnum written to
std::ostream& operator<<(std::ostream& os, const OCPPTransportEnum& ocpptransport_enum);

// from: SetNetworkProfileRequest
enum class VPNEnum {
    IKEv2,
    IPSec,
    L2TP,
    PPTP,
};

namespace conversions {
/// \brief Converts the given VPNEnum \p e to human readable string
/// \returns a string representation of the VPNEnum
std::string vpnenum_to_string(VPNEnum e);

/// \brief Converts the given std::string \p s to VPNEnum
/// \returns a VPNEnum from a string representation
VPNEnum string_to_vpnenum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given VPNEnum \p vpnenum to the given output stream \p os
/// \returns an output stream with the VPNEnum written to
std::ostream& operator<<(std::ostream& os, const VPNEnum& vpnenum);

// from: SetNetworkProfileResponse
enum class SetNetworkProfileStatusEnum {
    Accepted,
    Rejected,
    Failed,
};

namespace conversions {
/// \brief Converts the given SetNetworkProfileStatusEnum \p e to human readable string
/// \returns a string representation of the SetNetworkProfileStatusEnum
std::string set_network_profile_status_enum_to_string(SetNetworkProfileStatusEnum e);

/// \brief Converts the given std::string \p s to SetNetworkProfileStatusEnum
/// \returns a SetNetworkProfileStatusEnum from a string representation
SetNetworkProfileStatusEnum string_to_set_network_profile_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given SetNetworkProfileStatusEnum \p set_network_profile_status_enum
/// to the given output stream \p os
/// \returns an output stream with the SetNetworkProfileStatusEnum written to
std::ostream& operator<<(std::ostream& os, const SetNetworkProfileStatusEnum& set_network_profile_status_enum);

// from: SetVariableMonitoringResponse
enum class SetMonitoringStatusEnum {
    Accepted,
    UnknownComponent,
    UnknownVariable,
    UnsupportedMonitorType,
    Rejected,
    Duplicate,
};

namespace conversions {
/// \brief Converts the given SetMonitoringStatusEnum \p e to human readable string
/// \returns a string representation of the SetMonitoringStatusEnum
std::string set_monitoring_status_enum_to_string(SetMonitoringStatusEnum e);

/// \brief Converts the given std::string \p s to SetMonitoringStatusEnum
/// \returns a SetMonitoringStatusEnum from a string representation
SetMonitoringStatusEnum string_to_set_monitoring_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given SetMonitoringStatusEnum \p set_monitoring_status_enum to the
/// given output stream \p os
/// \returns an output stream with the SetMonitoringStatusEnum written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringStatusEnum& set_monitoring_status_enum);

// from: SetVariablesResponse
enum class SetVariableStatusEnum {
    Accepted,
    Rejected,
    UnknownComponent,
    UnknownVariable,
    NotSupportedAttributeType,
    RebootRequired,
};

namespace conversions {
/// \brief Converts the given SetVariableStatusEnum \p e to human readable string
/// \returns a string representation of the SetVariableStatusEnum
std::string set_variable_status_enum_to_string(SetVariableStatusEnum e);

/// \brief Converts the given std::string \p s to SetVariableStatusEnum
/// \returns a SetVariableStatusEnum from a string representation
SetVariableStatusEnum string_to_set_variable_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given SetVariableStatusEnum \p set_variable_status_enum to the given
/// output stream \p os
/// \returns an output stream with the SetVariableStatusEnum written to
std::ostream& operator<<(std::ostream& os, const SetVariableStatusEnum& set_variable_status_enum);

// from: StatusNotificationRequest
enum class ConnectorStatusEnum {
    Available,
    Occupied,
    Reserved,
    Unavailable,
    Faulted,
};

namespace conversions {
/// \brief Converts the given ConnectorStatusEnum \p e to human readable string
/// \returns a string representation of the ConnectorStatusEnum
std::string connector_status_enum_to_string(ConnectorStatusEnum e);

/// \brief Converts the given std::string \p s to ConnectorStatusEnum
/// \returns a ConnectorStatusEnum from a string representation
ConnectorStatusEnum string_to_connector_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ConnectorStatusEnum \p connector_status_enum to the given
/// output stream \p os
/// \returns an output stream with the ConnectorStatusEnum written to
std::ostream& operator<<(std::ostream& os, const ConnectorStatusEnum& connector_status_enum);

// from: TransactionEventRequest
enum class CostDimensionEnum {
    Energy,
    MaxCurrent,
    MinCurrent,
    MaxPower,
    MinPower,
    IdleTIme,
    ChargingTime,
};

namespace conversions {
/// \brief Converts the given CostDimensionEnum \p e to human readable string
/// \returns a string representation of the CostDimensionEnum
std::string cost_dimension_enum_to_string(CostDimensionEnum e);

/// \brief Converts the given std::string \p s to CostDimensionEnum
/// \returns a CostDimensionEnum from a string representation
CostDimensionEnum string_to_cost_dimension_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CostDimensionEnum \p cost_dimension_enum to the given output
/// stream \p os
/// \returns an output stream with the CostDimensionEnum written to
std::ostream& operator<<(std::ostream& os, const CostDimensionEnum& cost_dimension_enum);

// from: TransactionEventRequest
enum class TariffCostEnum {
    NormalCost,
    MinCost,
    MaxCost,
};

namespace conversions {
/// \brief Converts the given TariffCostEnum \p e to human readable string
/// \returns a string representation of the TariffCostEnum
std::string tariff_cost_enum_to_string(TariffCostEnum e);

/// \brief Converts the given std::string \p s to TariffCostEnum
/// \returns a TariffCostEnum from a string representation
TariffCostEnum string_to_tariff_cost_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TariffCostEnum \p tariff_cost_enum to the given output stream
/// \p os
/// \returns an output stream with the TariffCostEnum written to
std::ostream& operator<<(std::ostream& os, const TariffCostEnum& tariff_cost_enum);

// from: TransactionEventRequest
enum class TransactionEventEnum {
    Ended,
    Started,
    Updated,
};

namespace conversions {
/// \brief Converts the given TransactionEventEnum \p e to human readable string
/// \returns a string representation of the TransactionEventEnum
std::string transaction_event_enum_to_string(TransactionEventEnum e);

/// \brief Converts the given std::string \p s to TransactionEventEnum
/// \returns a TransactionEventEnum from a string representation
TransactionEventEnum string_to_transaction_event_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TransactionEventEnum \p transaction_event_enum to the given
/// output stream \p os
/// \returns an output stream with the TransactionEventEnum written to
std::ostream& operator<<(std::ostream& os, const TransactionEventEnum& transaction_event_enum);

// from: TransactionEventRequest
enum class TriggerReasonEnum {
    AbnormalCondition,
    Authorized,
    CablePluggedIn,
    ChargingRateChanged,
    ChargingStateChanged,
    CostLimitReached,
    Deauthorized,
    EnergyLimitReached,
    EVCommunicationLost,
    EVConnectTimeout,
    EVDeparted,
    EVDetected,
    LimitSet,
    MeterValueClock,
    MeterValuePeriodic,
    OperationModeChanged,
    RemoteStart,
    RemoteStop,
    ResetCommand,
    RunningCost,
    SignedDataReceived,
    SoCLimitReached,
    StopAuthorized,
    TariffChanged,
    TariffNotAccepted,
    TimeLimitReached,
    Trigger,
    TxResumed,
    UnlockCommand,
};

namespace conversions {
/// \brief Converts the given TriggerReasonEnum \p e to human readable string
/// \returns a string representation of the TriggerReasonEnum
std::string trigger_reason_enum_to_string(TriggerReasonEnum e);

/// \brief Converts the given std::string \p s to TriggerReasonEnum
/// \returns a TriggerReasonEnum from a string representation
TriggerReasonEnum string_to_trigger_reason_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TriggerReasonEnum \p trigger_reason_enum to the given output
/// stream \p os
/// \returns an output stream with the TriggerReasonEnum written to
std::ostream& operator<<(std::ostream& os, const TriggerReasonEnum& trigger_reason_enum);

// from: TransactionEventRequest
enum class PreconditioningStatusEnum {
    Unknown,
    Ready,
    NotReady,
    Preconditioning,
};

namespace conversions {
/// \brief Converts the given PreconditioningStatusEnum \p e to human readable string
/// \returns a string representation of the PreconditioningStatusEnum
std::string preconditioning_status_enum_to_string(PreconditioningStatusEnum e);

/// \brief Converts the given std::string \p s to PreconditioningStatusEnum
/// \returns a PreconditioningStatusEnum from a string representation
PreconditioningStatusEnum string_to_preconditioning_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PreconditioningStatusEnum \p preconditioning_status_enum to the
/// given output stream \p os
/// \returns an output stream with the PreconditioningStatusEnum written to
std::ostream& operator<<(std::ostream& os, const PreconditioningStatusEnum& preconditioning_status_enum);

// from: TransactionEventRequest
enum class ChargingStateEnum {
    EVConnected,
    Charging,
    SuspendedEV,
    SuspendedEVSE,
    Idle,
};

namespace conversions {
/// \brief Converts the given ChargingStateEnum \p e to human readable string
/// \returns a string representation of the ChargingStateEnum
std::string charging_state_enum_to_string(ChargingStateEnum e);

/// \brief Converts the given std::string \p s to ChargingStateEnum
/// \returns a ChargingStateEnum from a string representation
ChargingStateEnum string_to_charging_state_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingStateEnum \p charging_state_enum to the given output
/// stream \p os
/// \returns an output stream with the ChargingStateEnum written to
std::ostream& operator<<(std::ostream& os, const ChargingStateEnum& charging_state_enum);

// from: TransactionEventRequest
enum class ReasonEnum {
    DeAuthorized,
    EmergencyStop,
    EnergyLimitReached,
    EVDisconnected,
    GroundFault,
    ImmediateReset,
    MasterPass,
    Local,
    LocalOutOfCredit,
    Other,
    OvercurrentFault,
    PowerLoss,
    PowerQuality,
    Reboot,
    Remote,
    SOCLimitReached,
    StoppedByEV,
    TimeLimitReached,
    Timeout,
    ReqEnergyTransferRejected,
};

namespace conversions {
/// \brief Converts the given ReasonEnum \p e to human readable string
/// \returns a string representation of the ReasonEnum
std::string reason_enum_to_string(ReasonEnum e);

/// \brief Converts the given std::string \p s to ReasonEnum
/// \returns a ReasonEnum from a string representation
ReasonEnum string_to_reason_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReasonEnum \p reason_enum to the given output stream \p os
/// \returns an output stream with the ReasonEnum written to
std::ostream& operator<<(std::ostream& os, const ReasonEnum& reason_enum);

// from: TriggerMessageRequest
enum class MessageTriggerEnum {
    BootNotification,
    LogStatusNotification,
    FirmwareStatusNotification,
    Heartbeat,
    MeterValues,
    SignChargingStationCertificate,
    SignV2GCertificate,
    SignV2G20Certificate,
    StatusNotification,
    TransactionEvent,
    SignCombinedCertificate,
    PublishFirmwareStatusNotification,
    CustomTrigger,
};

namespace conversions {
/// \brief Converts the given MessageTriggerEnum \p e to human readable string
/// \returns a string representation of the MessageTriggerEnum
std::string message_trigger_enum_to_string(MessageTriggerEnum e);

/// \brief Converts the given std::string \p s to MessageTriggerEnum
/// \returns a MessageTriggerEnum from a string representation
MessageTriggerEnum string_to_message_trigger_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessageTriggerEnum \p message_trigger_enum to the given output
/// stream \p os
/// \returns an output stream with the MessageTriggerEnum written to
std::ostream& operator<<(std::ostream& os, const MessageTriggerEnum& message_trigger_enum);

// from: TriggerMessageResponse
enum class TriggerMessageStatusEnum {
    Accepted,
    Rejected,
    NotImplemented,
};

namespace conversions {
/// \brief Converts the given TriggerMessageStatusEnum \p e to human readable string
/// \returns a string representation of the TriggerMessageStatusEnum
std::string trigger_message_status_enum_to_string(TriggerMessageStatusEnum e);

/// \brief Converts the given std::string \p s to TriggerMessageStatusEnum
/// \returns a TriggerMessageStatusEnum from a string representation
TriggerMessageStatusEnum string_to_trigger_message_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TriggerMessageStatusEnum \p trigger_message_status_enum to the
/// given output stream \p os
/// \returns an output stream with the TriggerMessageStatusEnum written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageStatusEnum& trigger_message_status_enum);

// from: UnlockConnectorResponse
enum class UnlockStatusEnum {
    Unlocked,
    UnlockFailed,
    OngoingAuthorizedTransaction,
    UnknownConnector,
};

namespace conversions {
/// \brief Converts the given UnlockStatusEnum \p e to human readable string
/// \returns a string representation of the UnlockStatusEnum
std::string unlock_status_enum_to_string(UnlockStatusEnum e);

/// \brief Converts the given std::string \p s to UnlockStatusEnum
/// \returns a UnlockStatusEnum from a string representation
UnlockStatusEnum string_to_unlock_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UnlockStatusEnum \p unlock_status_enum to the given output
/// stream \p os
/// \returns an output stream with the UnlockStatusEnum written to
std::ostream& operator<<(std::ostream& os, const UnlockStatusEnum& unlock_status_enum);

// from: UnpublishFirmwareResponse
enum class UnpublishFirmwareStatusEnum {
    DownloadOngoing,
    NoFirmware,
    Unpublished,
};

namespace conversions {
/// \brief Converts the given UnpublishFirmwareStatusEnum \p e to human readable string
/// \returns a string representation of the UnpublishFirmwareStatusEnum
std::string unpublish_firmware_status_enum_to_string(UnpublishFirmwareStatusEnum e);

/// \brief Converts the given std::string \p s to UnpublishFirmwareStatusEnum
/// \returns a UnpublishFirmwareStatusEnum from a string representation
UnpublishFirmwareStatusEnum string_to_unpublish_firmware_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UnpublishFirmwareStatusEnum \p unpublish_firmware_status_enum
/// to the given output stream \p os
/// \returns an output stream with the UnpublishFirmwareStatusEnum written to
std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareStatusEnum& unpublish_firmware_status_enum);

// from: UpdateFirmwareResponse
enum class UpdateFirmwareStatusEnum {
    Accepted,
    Rejected,
    AcceptedCanceled,
    InvalidCertificate,
    RevokedCertificate,
};

namespace conversions {
/// \brief Converts the given UpdateFirmwareStatusEnum \p e to human readable string
/// \returns a string representation of the UpdateFirmwareStatusEnum
std::string update_firmware_status_enum_to_string(UpdateFirmwareStatusEnum e);

/// \brief Converts the given std::string \p s to UpdateFirmwareStatusEnum
/// \returns a UpdateFirmwareStatusEnum from a string representation
UpdateFirmwareStatusEnum string_to_update_firmware_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UpdateFirmwareStatusEnum \p update_firmware_status_enum to the
/// given output stream \p os
/// \returns an output stream with the UpdateFirmwareStatusEnum written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareStatusEnum& update_firmware_status_enum);

// from: UsePriorityChargingResponse
enum class PriorityChargingStatusEnum {
    Accepted,
    Rejected,
    NoProfile,
};

namespace conversions {
/// \brief Converts the given PriorityChargingStatusEnum \p e to human readable string
/// \returns a string representation of the PriorityChargingStatusEnum
std::string priority_charging_status_enum_to_string(PriorityChargingStatusEnum e);

/// \brief Converts the given std::string \p s to PriorityChargingStatusEnum
/// \returns a PriorityChargingStatusEnum from a string representation
PriorityChargingStatusEnum string_to_priority_charging_status_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given PriorityChargingStatusEnum \p priority_charging_status_enum to
/// the given output stream \p os
/// \returns an output stream with the PriorityChargingStatusEnum written to
std::ostream& operator<<(std::ostream& os, const PriorityChargingStatusEnum& priority_charging_status_enum);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_OCPP_ENUMS_HPP
