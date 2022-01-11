// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_ENUMS_HPP
#define OCPP1_6_ENUMS_HPP

#include <map>
#include <string>

namespace ocpp1_6 {

// from: AuthorizeResponse
enum class AuthorizationStatus
{
    Accepted,
    Blocked,
    Expired,
    Invalid,
    ConcurrentTx,
};
namespace conversions {
static const std::map<AuthorizationStatus, std::string> AuthorizationStatusStrings{
    {AuthorizationStatus::Accepted, "Accepted"},         {AuthorizationStatus::Blocked, "Blocked"},
    {AuthorizationStatus::Expired, "Expired"},           {AuthorizationStatus::Invalid, "Invalid"},
    {AuthorizationStatus::ConcurrentTx, "ConcurrentTx"},
};
static const std::map<std::string, AuthorizationStatus> AuthorizationStatusValues{
    {"Accepted", AuthorizationStatus::Accepted},         {"Blocked", AuthorizationStatus::Blocked},
    {"Expired", AuthorizationStatus::Expired},           {"Invalid", AuthorizationStatus::Invalid},
    {"ConcurrentTx", AuthorizationStatus::ConcurrentTx},
};

/// \brief Converts the given AuthorizationStatus \p e to human readable string
/// \returns a string representation of the AuthorizationStatus
static const std::string authorization_status_to_string(AuthorizationStatus e) {
    return AuthorizationStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to AuthorizationStatus
/// \returns a AuthorizationStatus from a string representation
static const AuthorizationStatus string_to_authorization_status(std::string s) {
    return AuthorizationStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given AuthorizationStatus \p authorization_status to the given output
/// stream \p os \returns an output stream with the AuthorizationStatus written to
inline std::ostream& operator<<(std::ostream& os, const AuthorizationStatus& authorization_status) {
    os << conversions::authorization_status_to_string(authorization_status);
    return os;
}

// from: BootNotificationResponse
enum class RegistrationStatus
{
    Accepted,
    Pending,
    Rejected,
};
namespace conversions {
static const std::map<RegistrationStatus, std::string> RegistrationStatusStrings{
    {RegistrationStatus::Accepted, "Accepted"},
    {RegistrationStatus::Pending, "Pending"},
    {RegistrationStatus::Rejected, "Rejected"},
};
static const std::map<std::string, RegistrationStatus> RegistrationStatusValues{
    {"Accepted", RegistrationStatus::Accepted},
    {"Pending", RegistrationStatus::Pending},
    {"Rejected", RegistrationStatus::Rejected},
};

/// \brief Converts the given RegistrationStatus \p e to human readable string
/// \returns a string representation of the RegistrationStatus
static const std::string registration_status_to_string(RegistrationStatus e) {
    return RegistrationStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to RegistrationStatus
/// \returns a RegistrationStatus from a string representation
static const RegistrationStatus string_to_registration_status(std::string s) {
    return RegistrationStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given RegistrationStatus \p registration_status to the given output
/// stream \p os \returns an output stream with the RegistrationStatus written to
inline std::ostream& operator<<(std::ostream& os, const RegistrationStatus& registration_status) {
    os << conversions::registration_status_to_string(registration_status);
    return os;
}

// from: CancelReservationResponse
enum class CancelReservationStatus
{
    Accepted,
    Rejected,
};
namespace conversions {
static const std::map<CancelReservationStatus, std::string> CancelReservationStatusStrings{
    {CancelReservationStatus::Accepted, "Accepted"},
    {CancelReservationStatus::Rejected, "Rejected"},
};
static const std::map<std::string, CancelReservationStatus> CancelReservationStatusValues{
    {"Accepted", CancelReservationStatus::Accepted},
    {"Rejected", CancelReservationStatus::Rejected},
};

/// \brief Converts the given CancelReservationStatus \p e to human readable string
/// \returns a string representation of the CancelReservationStatus
static const std::string cancel_reservation_status_to_string(CancelReservationStatus e) {
    return CancelReservationStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to CancelReservationStatus
/// \returns a CancelReservationStatus from a string representation
static const CancelReservationStatus string_to_cancel_reservation_status(std::string s) {
    return CancelReservationStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given CancelReservationStatus \p cancel_reservation_status to the
/// given output stream \p os \returns an output stream with the CancelReservationStatus written to
inline std::ostream& operator<<(std::ostream& os, const CancelReservationStatus& cancel_reservation_status) {
    os << conversions::cancel_reservation_status_to_string(cancel_reservation_status);
    return os;
}

// from: ChangeAvailabilityRequest
enum class AvailabilityType
{
    Inoperative,
    Operative,
};
namespace conversions {
static const std::map<AvailabilityType, std::string> AvailabilityTypeStrings{
    {AvailabilityType::Inoperative, "Inoperative"},
    {AvailabilityType::Operative, "Operative"},
};
static const std::map<std::string, AvailabilityType> AvailabilityTypeValues{
    {"Inoperative", AvailabilityType::Inoperative},
    {"Operative", AvailabilityType::Operative},
};

/// \brief Converts the given AvailabilityType \p e to human readable string
/// \returns a string representation of the AvailabilityType
static const std::string availability_type_to_string(AvailabilityType e) {
    return AvailabilityTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to AvailabilityType
/// \returns a AvailabilityType from a string representation
static const AvailabilityType string_to_availability_type(std::string s) {
    return AvailabilityTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given AvailabilityType \p availability_type to the given output
/// stream \p os \returns an output stream with the AvailabilityType written to
inline std::ostream& operator<<(std::ostream& os, const AvailabilityType& availability_type) {
    os << conversions::availability_type_to_string(availability_type);
    return os;
}

// from: ChangeAvailabilityResponse
enum class AvailabilityStatus
{
    Accepted,
    Rejected,
    Scheduled,
};
namespace conversions {
static const std::map<AvailabilityStatus, std::string> AvailabilityStatusStrings{
    {AvailabilityStatus::Accepted, "Accepted"},
    {AvailabilityStatus::Rejected, "Rejected"},
    {AvailabilityStatus::Scheduled, "Scheduled"},
};
static const std::map<std::string, AvailabilityStatus> AvailabilityStatusValues{
    {"Accepted", AvailabilityStatus::Accepted},
    {"Rejected", AvailabilityStatus::Rejected},
    {"Scheduled", AvailabilityStatus::Scheduled},
};

/// \brief Converts the given AvailabilityStatus \p e to human readable string
/// \returns a string representation of the AvailabilityStatus
static const std::string availability_status_to_string(AvailabilityStatus e) {
    return AvailabilityStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to AvailabilityStatus
/// \returns a AvailabilityStatus from a string representation
static const AvailabilityStatus string_to_availability_status(std::string s) {
    return AvailabilityStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given AvailabilityStatus \p availability_status to the given output
/// stream \p os \returns an output stream with the AvailabilityStatus written to
inline std::ostream& operator<<(std::ostream& os, const AvailabilityStatus& availability_status) {
    os << conversions::availability_status_to_string(availability_status);
    return os;
}

// from: ChangeConfigurationResponse
enum class ConfigurationStatus
{
    Accepted,
    Rejected,
    RebootRequired,
    NotSupported,
};
namespace conversions {
static const std::map<ConfigurationStatus, std::string> ConfigurationStatusStrings{
    {ConfigurationStatus::Accepted, "Accepted"},
    {ConfigurationStatus::Rejected, "Rejected"},
    {ConfigurationStatus::RebootRequired, "RebootRequired"},
    {ConfigurationStatus::NotSupported, "NotSupported"},
};
static const std::map<std::string, ConfigurationStatus> ConfigurationStatusValues{
    {"Accepted", ConfigurationStatus::Accepted},
    {"Rejected", ConfigurationStatus::Rejected},
    {"RebootRequired", ConfigurationStatus::RebootRequired},
    {"NotSupported", ConfigurationStatus::NotSupported},
};

/// \brief Converts the given ConfigurationStatus \p e to human readable string
/// \returns a string representation of the ConfigurationStatus
static const std::string configuration_status_to_string(ConfigurationStatus e) {
    return ConfigurationStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ConfigurationStatus
/// \returns a ConfigurationStatus from a string representation
static const ConfigurationStatus string_to_configuration_status(std::string s) {
    return ConfigurationStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ConfigurationStatus \p configuration_status to the given output
/// stream \p os \returns an output stream with the ConfigurationStatus written to
inline std::ostream& operator<<(std::ostream& os, const ConfigurationStatus& configuration_status) {
    os << conversions::configuration_status_to_string(configuration_status);
    return os;
}

// from: ClearCacheResponse
enum class ClearCacheStatus
{
    Accepted,
    Rejected,
};
namespace conversions {
static const std::map<ClearCacheStatus, std::string> ClearCacheStatusStrings{
    {ClearCacheStatus::Accepted, "Accepted"},
    {ClearCacheStatus::Rejected, "Rejected"},
};
static const std::map<std::string, ClearCacheStatus> ClearCacheStatusValues{
    {"Accepted", ClearCacheStatus::Accepted},
    {"Rejected", ClearCacheStatus::Rejected},
};

/// \brief Converts the given ClearCacheStatus \p e to human readable string
/// \returns a string representation of the ClearCacheStatus
static const std::string clear_cache_status_to_string(ClearCacheStatus e) {
    return ClearCacheStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ClearCacheStatus
/// \returns a ClearCacheStatus from a string representation
static const ClearCacheStatus string_to_clear_cache_status(std::string s) {
    return ClearCacheStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ClearCacheStatus \p clear_cache_status to the given output
/// stream \p os \returns an output stream with the ClearCacheStatus written to
inline std::ostream& operator<<(std::ostream& os, const ClearCacheStatus& clear_cache_status) {
    os << conversions::clear_cache_status_to_string(clear_cache_status);
    return os;
}

// from: ClearChargingProfileRequest
enum class ChargingProfilePurposeType
{
    ChargePointMaxProfile,
    TxDefaultProfile,
    TxProfile,
};
namespace conversions {
static const std::map<ChargingProfilePurposeType, std::string> ChargingProfilePurposeTypeStrings{
    {ChargingProfilePurposeType::ChargePointMaxProfile, "ChargePointMaxProfile"},
    {ChargingProfilePurposeType::TxDefaultProfile, "TxDefaultProfile"},
    {ChargingProfilePurposeType::TxProfile, "TxProfile"},
};
static const std::map<std::string, ChargingProfilePurposeType> ChargingProfilePurposeTypeValues{
    {"ChargePointMaxProfile", ChargingProfilePurposeType::ChargePointMaxProfile},
    {"TxDefaultProfile", ChargingProfilePurposeType::TxDefaultProfile},
    {"TxProfile", ChargingProfilePurposeType::TxProfile},
};

/// \brief Converts the given ChargingProfilePurposeType \p e to human readable string
/// \returns a string representation of the ChargingProfilePurposeType
static const std::string charging_profile_purpose_type_to_string(ChargingProfilePurposeType e) {
    return ChargingProfilePurposeTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargingProfilePurposeType
/// \returns a ChargingProfilePurposeType from a string representation
static const ChargingProfilePurposeType string_to_charging_profile_purpose_type(std::string s) {
    return ChargingProfilePurposeTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfilePurposeType \p charging_profile_purpose_type to
/// the given output stream \p os \returns an output stream with the ChargingProfilePurposeType written to
inline std::ostream& operator<<(std::ostream& os, const ChargingProfilePurposeType& charging_profile_purpose_type) {
    os << conversions::charging_profile_purpose_type_to_string(charging_profile_purpose_type);
    return os;
}

// from: ClearChargingProfileResponse
enum class ClearChargingProfileStatus
{
    Accepted,
    Unknown,
};
namespace conversions {
static const std::map<ClearChargingProfileStatus, std::string> ClearChargingProfileStatusStrings{
    {ClearChargingProfileStatus::Accepted, "Accepted"},
    {ClearChargingProfileStatus::Unknown, "Unknown"},
};
static const std::map<std::string, ClearChargingProfileStatus> ClearChargingProfileStatusValues{
    {"Accepted", ClearChargingProfileStatus::Accepted},
    {"Unknown", ClearChargingProfileStatus::Unknown},
};

/// \brief Converts the given ClearChargingProfileStatus \p e to human readable string
/// \returns a string representation of the ClearChargingProfileStatus
static const std::string clear_charging_profile_status_to_string(ClearChargingProfileStatus e) {
    return ClearChargingProfileStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ClearChargingProfileStatus
/// \returns a ClearChargingProfileStatus from a string representation
static const ClearChargingProfileStatus string_to_clear_charging_profile_status(std::string s) {
    return ClearChargingProfileStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ClearChargingProfileStatus \p clear_charging_profile_status to
/// the given output stream \p os \returns an output stream with the ClearChargingProfileStatus written to
inline std::ostream& operator<<(std::ostream& os, const ClearChargingProfileStatus& clear_charging_profile_status) {
    os << conversions::clear_charging_profile_status_to_string(clear_charging_profile_status);
    return os;
}

// from: DataTransferResponse
enum class DataTransferStatus
{
    Accepted,
    Rejected,
    UnknownMessageId,
    UnknownVendorId,
};
namespace conversions {
static const std::map<DataTransferStatus, std::string> DataTransferStatusStrings{
    {DataTransferStatus::Accepted, "Accepted"},
    {DataTransferStatus::Rejected, "Rejected"},
    {DataTransferStatus::UnknownMessageId, "UnknownMessageId"},
    {DataTransferStatus::UnknownVendorId, "UnknownVendorId"},
};
static const std::map<std::string, DataTransferStatus> DataTransferStatusValues{
    {"Accepted", DataTransferStatus::Accepted},
    {"Rejected", DataTransferStatus::Rejected},
    {"UnknownMessageId", DataTransferStatus::UnknownMessageId},
    {"UnknownVendorId", DataTransferStatus::UnknownVendorId},
};

/// \brief Converts the given DataTransferStatus \p e to human readable string
/// \returns a string representation of the DataTransferStatus
static const std::string data_transfer_status_to_string(DataTransferStatus e) {
    return DataTransferStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to DataTransferStatus
/// \returns a DataTransferStatus from a string representation
static const DataTransferStatus string_to_data_transfer_status(std::string s) {
    return DataTransferStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given DataTransferStatus \p data_transfer_status to the given output
/// stream \p os \returns an output stream with the DataTransferStatus written to
inline std::ostream& operator<<(std::ostream& os, const DataTransferStatus& data_transfer_status) {
    os << conversions::data_transfer_status_to_string(data_transfer_status);
    return os;
}

// from: DiagnosticsStatusNotificationRequest
enum class DiagnosticsStatus
{
    Idle,
    Uploaded,
    UploadFailed,
    Uploading,
};
namespace conversions {
static const std::map<DiagnosticsStatus, std::string> DiagnosticsStatusStrings{
    {DiagnosticsStatus::Idle, "Idle"},
    {DiagnosticsStatus::Uploaded, "Uploaded"},
    {DiagnosticsStatus::UploadFailed, "UploadFailed"},
    {DiagnosticsStatus::Uploading, "Uploading"},
};
static const std::map<std::string, DiagnosticsStatus> DiagnosticsStatusValues{
    {"Idle", DiagnosticsStatus::Idle},
    {"Uploaded", DiagnosticsStatus::Uploaded},
    {"UploadFailed", DiagnosticsStatus::UploadFailed},
    {"Uploading", DiagnosticsStatus::Uploading},
};

/// \brief Converts the given DiagnosticsStatus \p e to human readable string
/// \returns a string representation of the DiagnosticsStatus
static const std::string diagnostics_status_to_string(DiagnosticsStatus e) {
    return DiagnosticsStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to DiagnosticsStatus
/// \returns a DiagnosticsStatus from a string representation
static const DiagnosticsStatus string_to_diagnostics_status(std::string s) {
    return DiagnosticsStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given DiagnosticsStatus \p diagnostics_status to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatus written to
inline std::ostream& operator<<(std::ostream& os, const DiagnosticsStatus& diagnostics_status) {
    os << conversions::diagnostics_status_to_string(diagnostics_status);
    return os;
}

// from: FirmwareStatusNotificationRequest
enum class FirmwareStatus
{
    Downloaded,
    DownloadFailed,
    Downloading,
    Idle,
    InstallationFailed,
    Installing,
    Installed,
};
namespace conversions {
static const std::map<FirmwareStatus, std::string> FirmwareStatusStrings{
    {FirmwareStatus::Downloaded, "Downloaded"},
    {FirmwareStatus::DownloadFailed, "DownloadFailed"},
    {FirmwareStatus::Downloading, "Downloading"},
    {FirmwareStatus::Idle, "Idle"},
    {FirmwareStatus::InstallationFailed, "InstallationFailed"},
    {FirmwareStatus::Installing, "Installing"},
    {FirmwareStatus::Installed, "Installed"},
};
static const std::map<std::string, FirmwareStatus> FirmwareStatusValues{
    {"Downloaded", FirmwareStatus::Downloaded},
    {"DownloadFailed", FirmwareStatus::DownloadFailed},
    {"Downloading", FirmwareStatus::Downloading},
    {"Idle", FirmwareStatus::Idle},
    {"InstallationFailed", FirmwareStatus::InstallationFailed},
    {"Installing", FirmwareStatus::Installing},
    {"Installed", FirmwareStatus::Installed},
};

/// \brief Converts the given FirmwareStatus \p e to human readable string
/// \returns a string representation of the FirmwareStatus
static const std::string firmware_status_to_string(FirmwareStatus e) {
    return FirmwareStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to FirmwareStatus
/// \returns a FirmwareStatus from a string representation
static const FirmwareStatus string_to_firmware_status(std::string s) {
    return FirmwareStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given FirmwareStatus \p firmware_status to the given output stream \p
/// os \returns an output stream with the FirmwareStatus written to
inline std::ostream& operator<<(std::ostream& os, const FirmwareStatus& firmware_status) {
    os << conversions::firmware_status_to_string(firmware_status);
    return os;
}

// from: GetCompositeScheduleRequest
enum class ChargingRateUnit
{
    A,
    W,
};
namespace conversions {
static const std::map<ChargingRateUnit, std::string> ChargingRateUnitStrings{
    {ChargingRateUnit::A, "A"},
    {ChargingRateUnit::W, "W"},
};
static const std::map<std::string, ChargingRateUnit> ChargingRateUnitValues{
    {"A", ChargingRateUnit::A},
    {"W", ChargingRateUnit::W},
};

/// \brief Converts the given ChargingRateUnit \p e to human readable string
/// \returns a string representation of the ChargingRateUnit
static const std::string charging_rate_unit_to_string(ChargingRateUnit e) {
    return ChargingRateUnitStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargingRateUnit
/// \returns a ChargingRateUnit from a string representation
static const ChargingRateUnit string_to_charging_rate_unit(std::string s) {
    return ChargingRateUnitValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargingRateUnit \p charging_rate_unit to the given output
/// stream \p os \returns an output stream with the ChargingRateUnit written to
inline std::ostream& operator<<(std::ostream& os, const ChargingRateUnit& charging_rate_unit) {
    os << conversions::charging_rate_unit_to_string(charging_rate_unit);
    return os;
}

// from: GetCompositeScheduleResponse
enum class GetCompositeScheduleStatus
{
    Accepted,
    Rejected,
};
namespace conversions {
static const std::map<GetCompositeScheduleStatus, std::string> GetCompositeScheduleStatusStrings{
    {GetCompositeScheduleStatus::Accepted, "Accepted"},
    {GetCompositeScheduleStatus::Rejected, "Rejected"},
};
static const std::map<std::string, GetCompositeScheduleStatus> GetCompositeScheduleStatusValues{
    {"Accepted", GetCompositeScheduleStatus::Accepted},
    {"Rejected", GetCompositeScheduleStatus::Rejected},
};

/// \brief Converts the given GetCompositeScheduleStatus \p e to human readable string
/// \returns a string representation of the GetCompositeScheduleStatus
static const std::string get_composite_schedule_status_to_string(GetCompositeScheduleStatus e) {
    return GetCompositeScheduleStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to GetCompositeScheduleStatus
/// \returns a GetCompositeScheduleStatus from a string representation
static const GetCompositeScheduleStatus string_to_get_composite_schedule_status(std::string s) {
    return GetCompositeScheduleStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given GetCompositeScheduleStatus \p get_composite_schedule_status to
/// the given output stream \p os \returns an output stream with the GetCompositeScheduleStatus written to
inline std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleStatus& get_composite_schedule_status) {
    os << conversions::get_composite_schedule_status_to_string(get_composite_schedule_status);
    return os;
}

// from: MeterValuesRequest
enum class ReadingContext
{
    Interruption_Begin,
    Interruption_End,
    Sample_Clock,
    Sample_Periodic,
    Transaction_Begin,
    Transaction_End,
    Trigger,
    Other,
};
namespace conversions {
static const std::map<ReadingContext, std::string> ReadingContextStrings{
    {ReadingContext::Interruption_Begin, "Interruption.Begin"},
    {ReadingContext::Interruption_End, "Interruption.End"},
    {ReadingContext::Sample_Clock, "Sample.Clock"},
    {ReadingContext::Sample_Periodic, "Sample.Periodic"},
    {ReadingContext::Transaction_Begin, "Transaction.Begin"},
    {ReadingContext::Transaction_End, "Transaction.End"},
    {ReadingContext::Trigger, "Trigger"},
    {ReadingContext::Other, "Other"},
};
static const std::map<std::string, ReadingContext> ReadingContextValues{
    {"Interruption.Begin", ReadingContext::Interruption_Begin},
    {"Interruption.End", ReadingContext::Interruption_End},
    {"Sample.Clock", ReadingContext::Sample_Clock},
    {"Sample.Periodic", ReadingContext::Sample_Periodic},
    {"Transaction.Begin", ReadingContext::Transaction_Begin},
    {"Transaction.End", ReadingContext::Transaction_End},
    {"Trigger", ReadingContext::Trigger},
    {"Other", ReadingContext::Other},
};

/// \brief Converts the given ReadingContext \p e to human readable string
/// \returns a string representation of the ReadingContext
static const std::string reading_context_to_string(ReadingContext e) {
    return ReadingContextStrings.at(e);
}

/// \brief Converts the given std::string \p s to ReadingContext
/// \returns a ReadingContext from a string representation
static const ReadingContext string_to_reading_context(std::string s) {
    return ReadingContextValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ReadingContext \p reading_context to the given output stream \p
/// os \returns an output stream with the ReadingContext written to
inline std::ostream& operator<<(std::ostream& os, const ReadingContext& reading_context) {
    os << conversions::reading_context_to_string(reading_context);
    return os;
}

// from: MeterValuesRequest
enum class ValueFormat
{
    Raw,
    SignedData,
};
namespace conversions {
static const std::map<ValueFormat, std::string> ValueFormatStrings{
    {ValueFormat::Raw, "Raw"},
    {ValueFormat::SignedData, "SignedData"},
};
static const std::map<std::string, ValueFormat> ValueFormatValues{
    {"Raw", ValueFormat::Raw},
    {"SignedData", ValueFormat::SignedData},
};

/// \brief Converts the given ValueFormat \p e to human readable string
/// \returns a string representation of the ValueFormat
static const std::string value_format_to_string(ValueFormat e) {
    return ValueFormatStrings.at(e);
}

/// \brief Converts the given std::string \p s to ValueFormat
/// \returns a ValueFormat from a string representation
static const ValueFormat string_to_value_format(std::string s) {
    return ValueFormatValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ValueFormat \p value_format to the given output stream \p os
/// \returns an output stream with the ValueFormat written to
inline std::ostream& operator<<(std::ostream& os, const ValueFormat& value_format) {
    os << conversions::value_format_to_string(value_format);
    return os;
}

// from: MeterValuesRequest
enum class Measurand
{
    Energy_Active_Export_Register,
    Energy_Active_Import_Register,
    Energy_Reactive_Export_Register,
    Energy_Reactive_Import_Register,
    Energy_Active_Export_Interval,
    Energy_Active_Import_Interval,
    Energy_Reactive_Export_Interval,
    Energy_Reactive_Import_Interval,
    Power_Active_Export,
    Power_Active_Import,
    Power_Offered,
    Power_Reactive_Export,
    Power_Reactive_Import,
    Power_Factor,
    Current_Import,
    Current_Export,
    Current_Offered,
    Voltage,
    Frequency,
    Temperature,
    SoC,
    RPM,
};
namespace conversions {
static const std::map<Measurand, std::string> MeasurandStrings{
    {Measurand::Energy_Active_Export_Register, "Energy.Active.Export.Register"},
    {Measurand::Energy_Active_Import_Register, "Energy.Active.Import.Register"},
    {Measurand::Energy_Reactive_Export_Register, "Energy.Reactive.Export.Register"},
    {Measurand::Energy_Reactive_Import_Register, "Energy.Reactive.Import.Register"},
    {Measurand::Energy_Active_Export_Interval, "Energy.Active.Export.Interval"},
    {Measurand::Energy_Active_Import_Interval, "Energy.Active.Import.Interval"},
    {Measurand::Energy_Reactive_Export_Interval, "Energy.Reactive.Export.Interval"},
    {Measurand::Energy_Reactive_Import_Interval, "Energy.Reactive.Import.Interval"},
    {Measurand::Power_Active_Export, "Power.Active.Export"},
    {Measurand::Power_Active_Import, "Power.Active.Import"},
    {Measurand::Power_Offered, "Power.Offered"},
    {Measurand::Power_Reactive_Export, "Power.Reactive.Export"},
    {Measurand::Power_Reactive_Import, "Power.Reactive.Import"},
    {Measurand::Power_Factor, "Power.Factor"},
    {Measurand::Current_Import, "Current.Import"},
    {Measurand::Current_Export, "Current.Export"},
    {Measurand::Current_Offered, "Current.Offered"},
    {Measurand::Voltage, "Voltage"},
    {Measurand::Frequency, "Frequency"},
    {Measurand::Temperature, "Temperature"},
    {Measurand::SoC, "SoC"},
    {Measurand::RPM, "RPM"},
};
static const std::map<std::string, Measurand> MeasurandValues{
    {"Energy.Active.Export.Register", Measurand::Energy_Active_Export_Register},
    {"Energy.Active.Import.Register", Measurand::Energy_Active_Import_Register},
    {"Energy.Reactive.Export.Register", Measurand::Energy_Reactive_Export_Register},
    {"Energy.Reactive.Import.Register", Measurand::Energy_Reactive_Import_Register},
    {"Energy.Active.Export.Interval", Measurand::Energy_Active_Export_Interval},
    {"Energy.Active.Import.Interval", Measurand::Energy_Active_Import_Interval},
    {"Energy.Reactive.Export.Interval", Measurand::Energy_Reactive_Export_Interval},
    {"Energy.Reactive.Import.Interval", Measurand::Energy_Reactive_Import_Interval},
    {"Power.Active.Export", Measurand::Power_Active_Export},
    {"Power.Active.Import", Measurand::Power_Active_Import},
    {"Power.Offered", Measurand::Power_Offered},
    {"Power.Reactive.Export", Measurand::Power_Reactive_Export},
    {"Power.Reactive.Import", Measurand::Power_Reactive_Import},
    {"Power.Factor", Measurand::Power_Factor},
    {"Current.Import", Measurand::Current_Import},
    {"Current.Export", Measurand::Current_Export},
    {"Current.Offered", Measurand::Current_Offered},
    {"Voltage", Measurand::Voltage},
    {"Frequency", Measurand::Frequency},
    {"Temperature", Measurand::Temperature},
    {"SoC", Measurand::SoC},
    {"RPM", Measurand::RPM},
};

/// \brief Converts the given Measurand \p e to human readable string
/// \returns a string representation of the Measurand
static const std::string measurand_to_string(Measurand e) {
    return MeasurandStrings.at(e);
}

/// \brief Converts the given std::string \p s to Measurand
/// \returns a Measurand from a string representation
static const Measurand string_to_measurand(std::string s) {
    return MeasurandValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given Measurand \p measurand to the given output stream \p os
/// \returns an output stream with the Measurand written to
inline std::ostream& operator<<(std::ostream& os, const Measurand& measurand) {
    os << conversions::measurand_to_string(measurand);
    return os;
}

// from: MeterValuesRequest
enum class Phase
{
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
static const std::map<Phase, std::string> PhaseStrings{
    {Phase::L1, "L1"},       {Phase::L2, "L2"},       {Phase::L3, "L3"},     {Phase::N, "N"},
    {Phase::L1_N, "L1-N"},   {Phase::L2_N, "L2-N"},   {Phase::L3_N, "L3-N"}, {Phase::L1_L2, "L1-L2"},
    {Phase::L2_L3, "L2-L3"}, {Phase::L3_L1, "L3-L1"},
};
static const std::map<std::string, Phase> PhaseValues{
    {"L1", Phase::L1},       {"L2", Phase::L2},       {"L3", Phase::L3},     {"N", Phase::N},
    {"L1-N", Phase::L1_N},   {"L2-N", Phase::L2_N},   {"L3-N", Phase::L3_N}, {"L1-L2", Phase::L1_L2},
    {"L2-L3", Phase::L2_L3}, {"L3-L1", Phase::L3_L1},
};

/// \brief Converts the given Phase \p e to human readable string
/// \returns a string representation of the Phase
static const std::string phase_to_string(Phase e) {
    return PhaseStrings.at(e);
}

/// \brief Converts the given std::string \p s to Phase
/// \returns a Phase from a string representation
static const Phase string_to_phase(std::string s) {
    return PhaseValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given Phase \p phase to the given output stream \p os
/// \returns an output stream with the Phase written to
inline std::ostream& operator<<(std::ostream& os, const Phase& phase) {
    os << conversions::phase_to_string(phase);
    return os;
}

// from: MeterValuesRequest
enum class Location
{
    Cable,
    EV,
    Inlet,
    Outlet,
    Body,
};
namespace conversions {
static const std::map<Location, std::string> LocationStrings{
    {Location::Cable, "Cable"},   {Location::EV, "EV"},     {Location::Inlet, "Inlet"},
    {Location::Outlet, "Outlet"}, {Location::Body, "Body"},
};
static const std::map<std::string, Location> LocationValues{
    {"Cable", Location::Cable},   {"EV", Location::EV},     {"Inlet", Location::Inlet},
    {"Outlet", Location::Outlet}, {"Body", Location::Body},
};

/// \brief Converts the given Location \p e to human readable string
/// \returns a string representation of the Location
static const std::string location_to_string(Location e) {
    return LocationStrings.at(e);
}

/// \brief Converts the given std::string \p s to Location
/// \returns a Location from a string representation
static const Location string_to_location(std::string s) {
    return LocationValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given Location \p location to the given output stream \p os
/// \returns an output stream with the Location written to
inline std::ostream& operator<<(std::ostream& os, const Location& location) {
    os << conversions::location_to_string(location);
    return os;
}

// from: MeterValuesRequest
enum class UnitOfMeasure
{
    Wh,
    kWh,
    varh,
    kvarh,
    W,
    kW,
    VA,
    kVA,
    var,
    kvar,
    A,
    V,
    K,
    Celcius,
    Celsius,
    Fahrenheit,
    Percent,
};
namespace conversions {
static const std::map<UnitOfMeasure, std::string> UnitOfMeasureStrings{
    {UnitOfMeasure::Wh, "Wh"},
    {UnitOfMeasure::kWh, "kWh"},
    {UnitOfMeasure::varh, "varh"},
    {UnitOfMeasure::kvarh, "kvarh"},
    {UnitOfMeasure::W, "W"},
    {UnitOfMeasure::kW, "kW"},
    {UnitOfMeasure::VA, "VA"},
    {UnitOfMeasure::kVA, "kVA"},
    {UnitOfMeasure::var, "var"},
    {UnitOfMeasure::kvar, "kvar"},
    {UnitOfMeasure::A, "A"},
    {UnitOfMeasure::V, "V"},
    {UnitOfMeasure::K, "K"},
    {UnitOfMeasure::Celcius, "Celcius"},
    {UnitOfMeasure::Celsius, "Celsius"},
    {UnitOfMeasure::Fahrenheit, "Fahrenheit"},
    {UnitOfMeasure::Percent, "Percent"},
};
static const std::map<std::string, UnitOfMeasure> UnitOfMeasureValues{
    {"Wh", UnitOfMeasure::Wh},
    {"kWh", UnitOfMeasure::kWh},
    {"varh", UnitOfMeasure::varh},
    {"kvarh", UnitOfMeasure::kvarh},
    {"W", UnitOfMeasure::W},
    {"kW", UnitOfMeasure::kW},
    {"VA", UnitOfMeasure::VA},
    {"kVA", UnitOfMeasure::kVA},
    {"var", UnitOfMeasure::var},
    {"kvar", UnitOfMeasure::kvar},
    {"A", UnitOfMeasure::A},
    {"V", UnitOfMeasure::V},
    {"K", UnitOfMeasure::K},
    {"Celcius", UnitOfMeasure::Celcius},
    {"Celsius", UnitOfMeasure::Celsius},
    {"Fahrenheit", UnitOfMeasure::Fahrenheit},
    {"Percent", UnitOfMeasure::Percent},
};

/// \brief Converts the given UnitOfMeasure \p e to human readable string
/// \returns a string representation of the UnitOfMeasure
static const std::string unit_of_measure_to_string(UnitOfMeasure e) {
    return UnitOfMeasureStrings.at(e);
}

/// \brief Converts the given std::string \p s to UnitOfMeasure
/// \returns a UnitOfMeasure from a string representation
static const UnitOfMeasure string_to_unit_of_measure(std::string s) {
    return UnitOfMeasureValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given UnitOfMeasure \p unit_of_measure to the given output stream \p
/// os \returns an output stream with the UnitOfMeasure written to
inline std::ostream& operator<<(std::ostream& os, const UnitOfMeasure& unit_of_measure) {
    os << conversions::unit_of_measure_to_string(unit_of_measure);
    return os;
}

// from: RemoteStartTransactionRequest
enum class ChargingProfileKindType
{
    Absolute,
    Recurring,
    Relative,
};
namespace conversions {
static const std::map<ChargingProfileKindType, std::string> ChargingProfileKindTypeStrings{
    {ChargingProfileKindType::Absolute, "Absolute"},
    {ChargingProfileKindType::Recurring, "Recurring"},
    {ChargingProfileKindType::Relative, "Relative"},
};
static const std::map<std::string, ChargingProfileKindType> ChargingProfileKindTypeValues{
    {"Absolute", ChargingProfileKindType::Absolute},
    {"Recurring", ChargingProfileKindType::Recurring},
    {"Relative", ChargingProfileKindType::Relative},
};

/// \brief Converts the given ChargingProfileKindType \p e to human readable string
/// \returns a string representation of the ChargingProfileKindType
static const std::string charging_profile_kind_type_to_string(ChargingProfileKindType e) {
    return ChargingProfileKindTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargingProfileKindType
/// \returns a ChargingProfileKindType from a string representation
static const ChargingProfileKindType string_to_charging_profile_kind_type(std::string s) {
    return ChargingProfileKindTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileKindType \p charging_profile_kind_type to the
/// given output stream \p os \returns an output stream with the ChargingProfileKindType written to
inline std::ostream& operator<<(std::ostream& os, const ChargingProfileKindType& charging_profile_kind_type) {
    os << conversions::charging_profile_kind_type_to_string(charging_profile_kind_type);
    return os;
}

// from: RemoteStartTransactionRequest
enum class RecurrencyKindType
{
    Daily,
    Weekly,
};
namespace conversions {
static const std::map<RecurrencyKindType, std::string> RecurrencyKindTypeStrings{
    {RecurrencyKindType::Daily, "Daily"},
    {RecurrencyKindType::Weekly, "Weekly"},
};
static const std::map<std::string, RecurrencyKindType> RecurrencyKindTypeValues{
    {"Daily", RecurrencyKindType::Daily},
    {"Weekly", RecurrencyKindType::Weekly},
};

/// \brief Converts the given RecurrencyKindType \p e to human readable string
/// \returns a string representation of the RecurrencyKindType
static const std::string recurrency_kind_type_to_string(RecurrencyKindType e) {
    return RecurrencyKindTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to RecurrencyKindType
/// \returns a RecurrencyKindType from a string representation
static const RecurrencyKindType string_to_recurrency_kind_type(std::string s) {
    return RecurrencyKindTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given RecurrencyKindType \p recurrency_kind_type to the given output
/// stream \p os \returns an output stream with the RecurrencyKindType written to
inline std::ostream& operator<<(std::ostream& os, const RecurrencyKindType& recurrency_kind_type) {
    os << conversions::recurrency_kind_type_to_string(recurrency_kind_type);
    return os;
}

// from: RemoteStartTransactionResponse
enum class RemoteStartStopStatus
{
    Accepted,
    Rejected,
};
namespace conversions {
static const std::map<RemoteStartStopStatus, std::string> RemoteStartStopStatusStrings{
    {RemoteStartStopStatus::Accepted, "Accepted"},
    {RemoteStartStopStatus::Rejected, "Rejected"},
};
static const std::map<std::string, RemoteStartStopStatus> RemoteStartStopStatusValues{
    {"Accepted", RemoteStartStopStatus::Accepted},
    {"Rejected", RemoteStartStopStatus::Rejected},
};

/// \brief Converts the given RemoteStartStopStatus \p e to human readable string
/// \returns a string representation of the RemoteStartStopStatus
static const std::string remote_start_stop_status_to_string(RemoteStartStopStatus e) {
    return RemoteStartStopStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to RemoteStartStopStatus
/// \returns a RemoteStartStopStatus from a string representation
static const RemoteStartStopStatus string_to_remote_start_stop_status(std::string s) {
    return RemoteStartStopStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given RemoteStartStopStatus \p remote_start_stop_status to the given
/// output stream \p os \returns an output stream with the RemoteStartStopStatus written to
inline std::ostream& operator<<(std::ostream& os, const RemoteStartStopStatus& remote_start_stop_status) {
    os << conversions::remote_start_stop_status_to_string(remote_start_stop_status);
    return os;
}

// from: ReserveNowResponse
enum class ReservationStatus
{
    Accepted,
    Faulted,
    Occupied,
    Rejected,
    Unavailable,
};
namespace conversions {
static const std::map<ReservationStatus, std::string> ReservationStatusStrings{
    {ReservationStatus::Accepted, "Accepted"},       {ReservationStatus::Faulted, "Faulted"},
    {ReservationStatus::Occupied, "Occupied"},       {ReservationStatus::Rejected, "Rejected"},
    {ReservationStatus::Unavailable, "Unavailable"},
};
static const std::map<std::string, ReservationStatus> ReservationStatusValues{
    {"Accepted", ReservationStatus::Accepted},       {"Faulted", ReservationStatus::Faulted},
    {"Occupied", ReservationStatus::Occupied},       {"Rejected", ReservationStatus::Rejected},
    {"Unavailable", ReservationStatus::Unavailable},
};

/// \brief Converts the given ReservationStatus \p e to human readable string
/// \returns a string representation of the ReservationStatus
static const std::string reservation_status_to_string(ReservationStatus e) {
    return ReservationStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ReservationStatus
/// \returns a ReservationStatus from a string representation
static const ReservationStatus string_to_reservation_status(std::string s) {
    return ReservationStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ReservationStatus \p reservation_status to the given output
/// stream \p os \returns an output stream with the ReservationStatus written to
inline std::ostream& operator<<(std::ostream& os, const ReservationStatus& reservation_status) {
    os << conversions::reservation_status_to_string(reservation_status);
    return os;
}

// from: ResetRequest
enum class ResetType
{
    Hard,
    Soft,
};
namespace conversions {
static const std::map<ResetType, std::string> ResetTypeStrings{
    {ResetType::Hard, "Hard"},
    {ResetType::Soft, "Soft"},
};
static const std::map<std::string, ResetType> ResetTypeValues{
    {"Hard", ResetType::Hard},
    {"Soft", ResetType::Soft},
};

/// \brief Converts the given ResetType \p e to human readable string
/// \returns a string representation of the ResetType
static const std::string reset_type_to_string(ResetType e) {
    return ResetTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to ResetType
/// \returns a ResetType from a string representation
static const ResetType string_to_reset_type(std::string s) {
    return ResetTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ResetType \p reset_type to the given output stream \p os
/// \returns an output stream with the ResetType written to
inline std::ostream& operator<<(std::ostream& os, const ResetType& reset_type) {
    os << conversions::reset_type_to_string(reset_type);
    return os;
}

// from: ResetResponse
enum class ResetStatus
{
    Accepted,
    Rejected,
};
namespace conversions {
static const std::map<ResetStatus, std::string> ResetStatusStrings{
    {ResetStatus::Accepted, "Accepted"},
    {ResetStatus::Rejected, "Rejected"},
};
static const std::map<std::string, ResetStatus> ResetStatusValues{
    {"Accepted", ResetStatus::Accepted},
    {"Rejected", ResetStatus::Rejected},
};

/// \brief Converts the given ResetStatus \p e to human readable string
/// \returns a string representation of the ResetStatus
static const std::string reset_status_to_string(ResetStatus e) {
    return ResetStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ResetStatus
/// \returns a ResetStatus from a string representation
static const ResetStatus string_to_reset_status(std::string s) {
    return ResetStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ResetStatus \p reset_status to the given output stream \p os
/// \returns an output stream with the ResetStatus written to
inline std::ostream& operator<<(std::ostream& os, const ResetStatus& reset_status) {
    os << conversions::reset_status_to_string(reset_status);
    return os;
}

// from: SendLocalListRequest
enum class UpdateType
{
    Differential,
    Full,
};
namespace conversions {
static const std::map<UpdateType, std::string> UpdateTypeStrings{
    {UpdateType::Differential, "Differential"},
    {UpdateType::Full, "Full"},
};
static const std::map<std::string, UpdateType> UpdateTypeValues{
    {"Differential", UpdateType::Differential},
    {"Full", UpdateType::Full},
};

/// \brief Converts the given UpdateType \p e to human readable string
/// \returns a string representation of the UpdateType
static const std::string update_type_to_string(UpdateType e) {
    return UpdateTypeStrings.at(e);
}

/// \brief Converts the given std::string \p s to UpdateType
/// \returns a UpdateType from a string representation
static const UpdateType string_to_update_type(std::string s) {
    return UpdateTypeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given UpdateType \p update_type to the given output stream \p os
/// \returns an output stream with the UpdateType written to
inline std::ostream& operator<<(std::ostream& os, const UpdateType& update_type) {
    os << conversions::update_type_to_string(update_type);
    return os;
}

// from: SendLocalListResponse
enum class UpdateStatus
{
    Accepted,
    Failed,
    NotSupported,
    VersionMismatch,
};
namespace conversions {
static const std::map<UpdateStatus, std::string> UpdateStatusStrings{
    {UpdateStatus::Accepted, "Accepted"},
    {UpdateStatus::Failed, "Failed"},
    {UpdateStatus::NotSupported, "NotSupported"},
    {UpdateStatus::VersionMismatch, "VersionMismatch"},
};
static const std::map<std::string, UpdateStatus> UpdateStatusValues{
    {"Accepted", UpdateStatus::Accepted},
    {"Failed", UpdateStatus::Failed},
    {"NotSupported", UpdateStatus::NotSupported},
    {"VersionMismatch", UpdateStatus::VersionMismatch},
};

/// \brief Converts the given UpdateStatus \p e to human readable string
/// \returns a string representation of the UpdateStatus
static const std::string update_status_to_string(UpdateStatus e) {
    return UpdateStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to UpdateStatus
/// \returns a UpdateStatus from a string representation
static const UpdateStatus string_to_update_status(std::string s) {
    return UpdateStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given UpdateStatus \p update_status to the given output stream \p os
/// \returns an output stream with the UpdateStatus written to
inline std::ostream& operator<<(std::ostream& os, const UpdateStatus& update_status) {
    os << conversions::update_status_to_string(update_status);
    return os;
}

// from: SetChargingProfileResponse
enum class ChargingProfileStatus
{
    Accepted,
    Rejected,
    NotSupported,
};
namespace conversions {
static const std::map<ChargingProfileStatus, std::string> ChargingProfileStatusStrings{
    {ChargingProfileStatus::Accepted, "Accepted"},
    {ChargingProfileStatus::Rejected, "Rejected"},
    {ChargingProfileStatus::NotSupported, "NotSupported"},
};
static const std::map<std::string, ChargingProfileStatus> ChargingProfileStatusValues{
    {"Accepted", ChargingProfileStatus::Accepted},
    {"Rejected", ChargingProfileStatus::Rejected},
    {"NotSupported", ChargingProfileStatus::NotSupported},
};

/// \brief Converts the given ChargingProfileStatus \p e to human readable string
/// \returns a string representation of the ChargingProfileStatus
static const std::string charging_profile_status_to_string(ChargingProfileStatus e) {
    return ChargingProfileStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargingProfileStatus
/// \returns a ChargingProfileStatus from a string representation
static const ChargingProfileStatus string_to_charging_profile_status(std::string s) {
    return ChargingProfileStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileStatus \p charging_profile_status to the given
/// output stream \p os \returns an output stream with the ChargingProfileStatus written to
inline std::ostream& operator<<(std::ostream& os, const ChargingProfileStatus& charging_profile_status) {
    os << conversions::charging_profile_status_to_string(charging_profile_status);
    return os;
}

// from: StatusNotificationRequest
enum class ChargePointErrorCode
{
    ConnectorLockFailure,
    EVCommunicationError,
    GroundFailure,
    HighTemperature,
    InternalError,
    LocalListConflict,
    NoError,
    OtherError,
    OverCurrentFailure,
    PowerMeterFailure,
    PowerSwitchFailure,
    ReaderFailure,
    ResetFailure,
    UnderVoltage,
    OverVoltage,
    WeakSignal,
};
namespace conversions {
static const std::map<ChargePointErrorCode, std::string> ChargePointErrorCodeStrings{
    {ChargePointErrorCode::ConnectorLockFailure, "ConnectorLockFailure"},
    {ChargePointErrorCode::EVCommunicationError, "EVCommunicationError"},
    {ChargePointErrorCode::GroundFailure, "GroundFailure"},
    {ChargePointErrorCode::HighTemperature, "HighTemperature"},
    {ChargePointErrorCode::InternalError, "InternalError"},
    {ChargePointErrorCode::LocalListConflict, "LocalListConflict"},
    {ChargePointErrorCode::NoError, "NoError"},
    {ChargePointErrorCode::OtherError, "OtherError"},
    {ChargePointErrorCode::OverCurrentFailure, "OverCurrentFailure"},
    {ChargePointErrorCode::PowerMeterFailure, "PowerMeterFailure"},
    {ChargePointErrorCode::PowerSwitchFailure, "PowerSwitchFailure"},
    {ChargePointErrorCode::ReaderFailure, "ReaderFailure"},
    {ChargePointErrorCode::ResetFailure, "ResetFailure"},
    {ChargePointErrorCode::UnderVoltage, "UnderVoltage"},
    {ChargePointErrorCode::OverVoltage, "OverVoltage"},
    {ChargePointErrorCode::WeakSignal, "WeakSignal"},
};
static const std::map<std::string, ChargePointErrorCode> ChargePointErrorCodeValues{
    {"ConnectorLockFailure", ChargePointErrorCode::ConnectorLockFailure},
    {"EVCommunicationError", ChargePointErrorCode::EVCommunicationError},
    {"GroundFailure", ChargePointErrorCode::GroundFailure},
    {"HighTemperature", ChargePointErrorCode::HighTemperature},
    {"InternalError", ChargePointErrorCode::InternalError},
    {"LocalListConflict", ChargePointErrorCode::LocalListConflict},
    {"NoError", ChargePointErrorCode::NoError},
    {"OtherError", ChargePointErrorCode::OtherError},
    {"OverCurrentFailure", ChargePointErrorCode::OverCurrentFailure},
    {"PowerMeterFailure", ChargePointErrorCode::PowerMeterFailure},
    {"PowerSwitchFailure", ChargePointErrorCode::PowerSwitchFailure},
    {"ReaderFailure", ChargePointErrorCode::ReaderFailure},
    {"ResetFailure", ChargePointErrorCode::ResetFailure},
    {"UnderVoltage", ChargePointErrorCode::UnderVoltage},
    {"OverVoltage", ChargePointErrorCode::OverVoltage},
    {"WeakSignal", ChargePointErrorCode::WeakSignal},
};

/// \brief Converts the given ChargePointErrorCode \p e to human readable string
/// \returns a string representation of the ChargePointErrorCode
static const std::string charge_point_error_code_to_string(ChargePointErrorCode e) {
    return ChargePointErrorCodeStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargePointErrorCode
/// \returns a ChargePointErrorCode from a string representation
static const ChargePointErrorCode string_to_charge_point_error_code(std::string s) {
    return ChargePointErrorCodeValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargePointErrorCode \p charge_point_error_code to the given
/// output stream \p os \returns an output stream with the ChargePointErrorCode written to
inline std::ostream& operator<<(std::ostream& os, const ChargePointErrorCode& charge_point_error_code) {
    os << conversions::charge_point_error_code_to_string(charge_point_error_code);
    return os;
}

// from: StatusNotificationRequest
enum class ChargePointStatus
{
    Available,
    Preparing,
    Charging,
    SuspendedEVSE,
    SuspendedEV,
    Finishing,
    Reserved,
    Unavailable,
    Faulted,
};
namespace conversions {
static const std::map<ChargePointStatus, std::string> ChargePointStatusStrings{
    {ChargePointStatus::Available, "Available"},     {ChargePointStatus::Preparing, "Preparing"},
    {ChargePointStatus::Charging, "Charging"},       {ChargePointStatus::SuspendedEVSE, "SuspendedEVSE"},
    {ChargePointStatus::SuspendedEV, "SuspendedEV"}, {ChargePointStatus::Finishing, "Finishing"},
    {ChargePointStatus::Reserved, "Reserved"},       {ChargePointStatus::Unavailable, "Unavailable"},
    {ChargePointStatus::Faulted, "Faulted"},
};
static const std::map<std::string, ChargePointStatus> ChargePointStatusValues{
    {"Available", ChargePointStatus::Available},     {"Preparing", ChargePointStatus::Preparing},
    {"Charging", ChargePointStatus::Charging},       {"SuspendedEVSE", ChargePointStatus::SuspendedEVSE},
    {"SuspendedEV", ChargePointStatus::SuspendedEV}, {"Finishing", ChargePointStatus::Finishing},
    {"Reserved", ChargePointStatus::Reserved},       {"Unavailable", ChargePointStatus::Unavailable},
    {"Faulted", ChargePointStatus::Faulted},
};

/// \brief Converts the given ChargePointStatus \p e to human readable string
/// \returns a string representation of the ChargePointStatus
static const std::string charge_point_status_to_string(ChargePointStatus e) {
    return ChargePointStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to ChargePointStatus
/// \returns a ChargePointStatus from a string representation
static const ChargePointStatus string_to_charge_point_status(std::string s) {
    return ChargePointStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given ChargePointStatus \p charge_point_status to the given output
/// stream \p os \returns an output stream with the ChargePointStatus written to
inline std::ostream& operator<<(std::ostream& os, const ChargePointStatus& charge_point_status) {
    os << conversions::charge_point_status_to_string(charge_point_status);
    return os;
}

// from: StopTransactionRequest
enum class Reason
{
    EmergencyStop,
    EVDisconnected,
    HardReset,
    Local,
    Other,
    PowerLoss,
    Reboot,
    Remote,
    SoftReset,
    UnlockCommand,
    DeAuthorized,
};
namespace conversions {
static const std::map<Reason, std::string> ReasonStrings{
    {Reason::EmergencyStop, "EmergencyStop"},
    {Reason::EVDisconnected, "EVDisconnected"},
    {Reason::HardReset, "HardReset"},
    {Reason::Local, "Local"},
    {Reason::Other, "Other"},
    {Reason::PowerLoss, "PowerLoss"},
    {Reason::Reboot, "Reboot"},
    {Reason::Remote, "Remote"},
    {Reason::SoftReset, "SoftReset"},
    {Reason::UnlockCommand, "UnlockCommand"},
    {Reason::DeAuthorized, "DeAuthorized"},
};
static const std::map<std::string, Reason> ReasonValues{
    {"EmergencyStop", Reason::EmergencyStop},
    {"EVDisconnected", Reason::EVDisconnected},
    {"HardReset", Reason::HardReset},
    {"Local", Reason::Local},
    {"Other", Reason::Other},
    {"PowerLoss", Reason::PowerLoss},
    {"Reboot", Reason::Reboot},
    {"Remote", Reason::Remote},
    {"SoftReset", Reason::SoftReset},
    {"UnlockCommand", Reason::UnlockCommand},
    {"DeAuthorized", Reason::DeAuthorized},
};

/// \brief Converts the given Reason \p e to human readable string
/// \returns a string representation of the Reason
static const std::string reason_to_string(Reason e) {
    return ReasonStrings.at(e);
}

/// \brief Converts the given std::string \p s to Reason
/// \returns a Reason from a string representation
static const Reason string_to_reason(std::string s) {
    return ReasonValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given Reason \p reason to the given output stream \p os
/// \returns an output stream with the Reason written to
inline std::ostream& operator<<(std::ostream& os, const Reason& reason) {
    os << conversions::reason_to_string(reason);
    return os;
}

// from: TriggerMessageRequest
enum class MessageTrigger
{
    BootNotification,
    DiagnosticsStatusNotification,
    FirmwareStatusNotification,
    Heartbeat,
    MeterValues,
    StatusNotification,
};
namespace conversions {
static const std::map<MessageTrigger, std::string> MessageTriggerStrings{
    {MessageTrigger::BootNotification, "BootNotification"},
    {MessageTrigger::DiagnosticsStatusNotification, "DiagnosticsStatusNotification"},
    {MessageTrigger::FirmwareStatusNotification, "FirmwareStatusNotification"},
    {MessageTrigger::Heartbeat, "Heartbeat"},
    {MessageTrigger::MeterValues, "MeterValues"},
    {MessageTrigger::StatusNotification, "StatusNotification"},
};
static const std::map<std::string, MessageTrigger> MessageTriggerValues{
    {"BootNotification", MessageTrigger::BootNotification},
    {"DiagnosticsStatusNotification", MessageTrigger::DiagnosticsStatusNotification},
    {"FirmwareStatusNotification", MessageTrigger::FirmwareStatusNotification},
    {"Heartbeat", MessageTrigger::Heartbeat},
    {"MeterValues", MessageTrigger::MeterValues},
    {"StatusNotification", MessageTrigger::StatusNotification},
};

/// \brief Converts the given MessageTrigger \p e to human readable string
/// \returns a string representation of the MessageTrigger
static const std::string message_trigger_to_string(MessageTrigger e) {
    return MessageTriggerStrings.at(e);
}

/// \brief Converts the given std::string \p s to MessageTrigger
/// \returns a MessageTrigger from a string representation
static const MessageTrigger string_to_message_trigger(std::string s) {
    return MessageTriggerValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given MessageTrigger \p message_trigger to the given output stream \p
/// os \returns an output stream with the MessageTrigger written to
inline std::ostream& operator<<(std::ostream& os, const MessageTrigger& message_trigger) {
    os << conversions::message_trigger_to_string(message_trigger);
    return os;
}

// from: TriggerMessageResponse
enum class TriggerMessageStatus
{
    Accepted,
    Rejected,
    NotImplemented,
};
namespace conversions {
static const std::map<TriggerMessageStatus, std::string> TriggerMessageStatusStrings{
    {TriggerMessageStatus::Accepted, "Accepted"},
    {TriggerMessageStatus::Rejected, "Rejected"},
    {TriggerMessageStatus::NotImplemented, "NotImplemented"},
};
static const std::map<std::string, TriggerMessageStatus> TriggerMessageStatusValues{
    {"Accepted", TriggerMessageStatus::Accepted},
    {"Rejected", TriggerMessageStatus::Rejected},
    {"NotImplemented", TriggerMessageStatus::NotImplemented},
};

/// \brief Converts the given TriggerMessageStatus \p e to human readable string
/// \returns a string representation of the TriggerMessageStatus
static const std::string trigger_message_status_to_string(TriggerMessageStatus e) {
    return TriggerMessageStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to TriggerMessageStatus
/// \returns a TriggerMessageStatus from a string representation
static const TriggerMessageStatus string_to_trigger_message_status(std::string s) {
    return TriggerMessageStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given TriggerMessageStatus \p trigger_message_status to the given
/// output stream \p os \returns an output stream with the TriggerMessageStatus written to
inline std::ostream& operator<<(std::ostream& os, const TriggerMessageStatus& trigger_message_status) {
    os << conversions::trigger_message_status_to_string(trigger_message_status);
    return os;
}

// from: UnlockConnectorResponse
enum class UnlockStatus
{
    Unlocked,
    UnlockFailed,
    NotSupported,
};
namespace conversions {
static const std::map<UnlockStatus, std::string> UnlockStatusStrings{
    {UnlockStatus::Unlocked, "Unlocked"},
    {UnlockStatus::UnlockFailed, "UnlockFailed"},
    {UnlockStatus::NotSupported, "NotSupported"},
};
static const std::map<std::string, UnlockStatus> UnlockStatusValues{
    {"Unlocked", UnlockStatus::Unlocked},
    {"UnlockFailed", UnlockStatus::UnlockFailed},
    {"NotSupported", UnlockStatus::NotSupported},
};

/// \brief Converts the given UnlockStatus \p e to human readable string
/// \returns a string representation of the UnlockStatus
static const std::string unlock_status_to_string(UnlockStatus e) {
    return UnlockStatusStrings.at(e);
}

/// \brief Converts the given std::string \p s to UnlockStatus
/// \returns a UnlockStatus from a string representation
static const UnlockStatus string_to_unlock_status(std::string s) {
    return UnlockStatusValues.at(s);
}
} // namespace conversions

/// \brief Writes the string representation of the given UnlockStatus \p unlock_status to the given output stream \p os
/// \returns an output stream with the UnlockStatus written to
inline std::ostream& operator<<(std::ostream& os, const UnlockStatus& unlock_status) {
    os << conversions::unlock_status_to_string(unlock_status);
    return os;
}
} // namespace ocpp1_6

#endif // OCPP1_6_ENUMS_HPP
