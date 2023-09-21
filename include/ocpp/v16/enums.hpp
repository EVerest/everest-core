// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_ENUMS_HPP
#define OCPP_V16_ENUMS_HPP

#include <iosfwd>
#include <string>

namespace ocpp {
namespace v16 {

// from: AuthorizeResponse
enum class AuthorizationStatus {
    Accepted,
    Blocked,
    Expired,
    Invalid,
    ConcurrentTx,
};

namespace conversions {
/// \brief Converts the given AuthorizationStatus \p e to human readable string
/// \returns a string representation of the AuthorizationStatus
std::string authorization_status_to_string(AuthorizationStatus e);

/// \brief Converts the given std::string \p s to AuthorizationStatus
/// \returns a AuthorizationStatus from a string representation
AuthorizationStatus string_to_authorization_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AuthorizationStatus \p authorization_status to the given output
/// stream \p os \returns an output stream with the AuthorizationStatus written to
std::ostream& operator<<(std::ostream& os, const AuthorizationStatus& authorization_status);

// from: BootNotificationResponse
enum class RegistrationStatus {
    Accepted,
    Pending,
    Rejected,
};

namespace conversions {
/// \brief Converts the given RegistrationStatus \p e to human readable string
/// \returns a string representation of the RegistrationStatus
std::string registration_status_to_string(RegistrationStatus e);

/// \brief Converts the given std::string \p s to RegistrationStatus
/// \returns a RegistrationStatus from a string representation
RegistrationStatus string_to_registration_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RegistrationStatus \p registration_status to the given output
/// stream \p os \returns an output stream with the RegistrationStatus written to
std::ostream& operator<<(std::ostream& os, const RegistrationStatus& registration_status);

// from: CancelReservationResponse
enum class CancelReservationStatus {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given CancelReservationStatus \p e to human readable string
/// \returns a string representation of the CancelReservationStatus
std::string cancel_reservation_status_to_string(CancelReservationStatus e);

/// \brief Converts the given std::string \p s to CancelReservationStatus
/// \returns a CancelReservationStatus from a string representation
CancelReservationStatus string_to_cancel_reservation_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CancelReservationStatus \p cancel_reservation_status to the
/// given output stream \p os \returns an output stream with the CancelReservationStatus written to
std::ostream& operator<<(std::ostream& os, const CancelReservationStatus& cancel_reservation_status);

// from: CertificateSignedResponse
enum class CertificateSignedStatusEnumType {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given CertificateSignedStatusEnumType \p e to human readable string
/// \returns a string representation of the CertificateSignedStatusEnumType
std::string certificate_signed_status_enum_type_to_string(CertificateSignedStatusEnumType e);

/// \brief Converts the given std::string \p s to CertificateSignedStatusEnumType
/// \returns a CertificateSignedStatusEnumType from a string representation
CertificateSignedStatusEnumType string_to_certificate_signed_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateSignedStatusEnumType \p
/// certificate_signed_status_enum_type to the given output stream \p os \returns an output stream with the
/// CertificateSignedStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedStatusEnumType& certificate_signed_status_enum_type);

// from: ChangeAvailabilityRequest
enum class AvailabilityType {
    Inoperative,
    Operative,
};

namespace conversions {
/// \brief Converts the given AvailabilityType \p e to human readable string
/// \returns a string representation of the AvailabilityType
std::string availability_type_to_string(AvailabilityType e);

/// \brief Converts the given std::string \p s to AvailabilityType
/// \returns a AvailabilityType from a string representation
AvailabilityType string_to_availability_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AvailabilityType \p availability_type to the given output
/// stream \p os \returns an output stream with the AvailabilityType written to
std::ostream& operator<<(std::ostream& os, const AvailabilityType& availability_type);

// from: ChangeAvailabilityResponse
enum class AvailabilityStatus {
    Accepted,
    Rejected,
    Scheduled,
};

namespace conversions {
/// \brief Converts the given AvailabilityStatus \p e to human readable string
/// \returns a string representation of the AvailabilityStatus
std::string availability_status_to_string(AvailabilityStatus e);

/// \brief Converts the given std::string \p s to AvailabilityStatus
/// \returns a AvailabilityStatus from a string representation
AvailabilityStatus string_to_availability_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given AvailabilityStatus \p availability_status to the given output
/// stream \p os \returns an output stream with the AvailabilityStatus written to
std::ostream& operator<<(std::ostream& os, const AvailabilityStatus& availability_status);

// from: ChangeConfigurationResponse
enum class ConfigurationStatus {
    Accepted,
    Rejected,
    RebootRequired,
    NotSupported,
};

namespace conversions {
/// \brief Converts the given ConfigurationStatus \p e to human readable string
/// \returns a string representation of the ConfigurationStatus
std::string configuration_status_to_string(ConfigurationStatus e);

/// \brief Converts the given std::string \p s to ConfigurationStatus
/// \returns a ConfigurationStatus from a string representation
ConfigurationStatus string_to_configuration_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ConfigurationStatus \p configuration_status to the given output
/// stream \p os \returns an output stream with the ConfigurationStatus written to
std::ostream& operator<<(std::ostream& os, const ConfigurationStatus& configuration_status);

// from: ClearCacheResponse
enum class ClearCacheStatus {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given ClearCacheStatus \p e to human readable string
/// \returns a string representation of the ClearCacheStatus
std::string clear_cache_status_to_string(ClearCacheStatus e);

/// \brief Converts the given std::string \p s to ClearCacheStatus
/// \returns a ClearCacheStatus from a string representation
ClearCacheStatus string_to_clear_cache_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearCacheStatus \p clear_cache_status to the given output
/// stream \p os \returns an output stream with the ClearCacheStatus written to
std::ostream& operator<<(std::ostream& os, const ClearCacheStatus& clear_cache_status);

// from: ClearChargingProfileRequest
enum class ChargingProfilePurposeType {
    ChargePointMaxProfile,
    TxDefaultProfile,
    TxProfile,
};

namespace conversions {
/// \brief Converts the given ChargingProfilePurposeType \p e to human readable string
/// \returns a string representation of the ChargingProfilePurposeType
std::string charging_profile_purpose_type_to_string(ChargingProfilePurposeType e);

/// \brief Converts the given std::string \p s to ChargingProfilePurposeType
/// \returns a ChargingProfilePurposeType from a string representation
ChargingProfilePurposeType string_to_charging_profile_purpose_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfilePurposeType \p charging_profile_purpose_type to
/// the given output stream \p os \returns an output stream with the ChargingProfilePurposeType written to
std::ostream& operator<<(std::ostream& os, const ChargingProfilePurposeType& charging_profile_purpose_type);

// from: ClearChargingProfileResponse
enum class ClearChargingProfileStatus {
    Accepted,
    Unknown,
};

namespace conversions {
/// \brief Converts the given ClearChargingProfileStatus \p e to human readable string
/// \returns a string representation of the ClearChargingProfileStatus
std::string clear_charging_profile_status_to_string(ClearChargingProfileStatus e);

/// \brief Converts the given std::string \p s to ClearChargingProfileStatus
/// \returns a ClearChargingProfileStatus from a string representation
ClearChargingProfileStatus string_to_clear_charging_profile_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ClearChargingProfileStatus \p clear_charging_profile_status to
/// the given output stream \p os \returns an output stream with the ClearChargingProfileStatus written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileStatus& clear_charging_profile_status);

// from: DataTransferResponse
enum class DataTransferStatus {
    Accepted,
    Rejected,
    UnknownMessageId,
    UnknownVendorId,
};

namespace conversions {
/// \brief Converts the given DataTransferStatus \p e to human readable string
/// \returns a string representation of the DataTransferStatus
std::string data_transfer_status_to_string(DataTransferStatus e);

/// \brief Converts the given std::string \p s to DataTransferStatus
/// \returns a DataTransferStatus from a string representation
DataTransferStatus string_to_data_transfer_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DataTransferStatus \p data_transfer_status to the given output
/// stream \p os \returns an output stream with the DataTransferStatus written to
std::ostream& operator<<(std::ostream& os, const DataTransferStatus& data_transfer_status);

// from: DeleteCertificateRequest
enum class HashAlgorithmEnumType {
    SHA256,
    SHA384,
    SHA512,
};

namespace conversions {
/// \brief Converts the given HashAlgorithmEnumType \p e to human readable string
/// \returns a string representation of the HashAlgorithmEnumType
std::string hash_algorithm_enum_type_to_string(HashAlgorithmEnumType e);

/// \brief Converts the given std::string \p s to HashAlgorithmEnumType
/// \returns a HashAlgorithmEnumType from a string representation
HashAlgorithmEnumType string_to_hash_algorithm_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given HashAlgorithmEnumType \p hash_algorithm_enum_type to the given
/// output stream \p os \returns an output stream with the HashAlgorithmEnumType written to
std::ostream& operator<<(std::ostream& os, const HashAlgorithmEnumType& hash_algorithm_enum_type);

// from: DeleteCertificateResponse
enum class DeleteCertificateStatusEnumType {
    Accepted,
    Failed,
    NotFound,
};

namespace conversions {
/// \brief Converts the given DeleteCertificateStatusEnumType \p e to human readable string
/// \returns a string representation of the DeleteCertificateStatusEnumType
std::string delete_certificate_status_enum_type_to_string(DeleteCertificateStatusEnumType e);

/// \brief Converts the given std::string \p s to DeleteCertificateStatusEnumType
/// \returns a DeleteCertificateStatusEnumType from a string representation
DeleteCertificateStatusEnumType string_to_delete_certificate_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DeleteCertificateStatusEnumType \p
/// delete_certificate_status_enum_type to the given output stream \p os \returns an output stream with the
/// DeleteCertificateStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateStatusEnumType& delete_certificate_status_enum_type);

// from: DiagnosticsStatusNotificationRequest
enum class DiagnosticsStatus {
    Idle,
    Uploaded,
    UploadFailed,
    Uploading,
};

namespace conversions {
/// \brief Converts the given DiagnosticsStatus \p e to human readable string
/// \returns a string representation of the DiagnosticsStatus
std::string diagnostics_status_to_string(DiagnosticsStatus e);

/// \brief Converts the given std::string \p s to DiagnosticsStatus
/// \returns a DiagnosticsStatus from a string representation
DiagnosticsStatus string_to_diagnostics_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DiagnosticsStatus \p diagnostics_status to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatus written to
std::ostream& operator<<(std::ostream& os, const DiagnosticsStatus& diagnostics_status);

// from: ExtendedTriggerMessageRequest
enum class MessageTriggerEnumType {
    BootNotification,
    LogStatusNotification,
    FirmwareStatusNotification,
    Heartbeat,
    MeterValues,
    SignChargePointCertificate,
    StatusNotification,
};

namespace conversions {
/// \brief Converts the given MessageTriggerEnumType \p e to human readable string
/// \returns a string representation of the MessageTriggerEnumType
std::string message_trigger_enum_type_to_string(MessageTriggerEnumType e);

/// \brief Converts the given std::string \p s to MessageTriggerEnumType
/// \returns a MessageTriggerEnumType from a string representation
MessageTriggerEnumType string_to_message_trigger_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessageTriggerEnumType \p message_trigger_enum_type to the
/// given output stream \p os \returns an output stream with the MessageTriggerEnumType written to
std::ostream& operator<<(std::ostream& os, const MessageTriggerEnumType& message_trigger_enum_type);

// from: ExtendedTriggerMessageResponse
enum class TriggerMessageStatusEnumType {
    Accepted,
    Rejected,
    NotImplemented,
};

namespace conversions {
/// \brief Converts the given TriggerMessageStatusEnumType \p e to human readable string
/// \returns a string representation of the TriggerMessageStatusEnumType
std::string trigger_message_status_enum_type_to_string(TriggerMessageStatusEnumType e);

/// \brief Converts the given std::string \p s to TriggerMessageStatusEnumType
/// \returns a TriggerMessageStatusEnumType from a string representation
TriggerMessageStatusEnumType string_to_trigger_message_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TriggerMessageStatusEnumType \p
/// trigger_message_status_enum_type to the given output stream \p os \returns an output stream with the
/// TriggerMessageStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageStatusEnumType& trigger_message_status_enum_type);

// from: FirmwareStatusNotificationRequest
enum class FirmwareStatus {
    Downloaded,
    DownloadFailed,
    Downloading,
    Idle,
    InstallationFailed,
    Installing,
    Installed,
};

namespace conversions {
/// \brief Converts the given FirmwareStatus \p e to human readable string
/// \returns a string representation of the FirmwareStatus
std::string firmware_status_to_string(FirmwareStatus e);

/// \brief Converts the given std::string \p s to FirmwareStatus
/// \returns a FirmwareStatus from a string representation
FirmwareStatus string_to_firmware_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given FirmwareStatus \p firmware_status to the given output stream \p
/// os \returns an output stream with the FirmwareStatus written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatus& firmware_status);

// from: GetCompositeScheduleRequest
enum class ChargingRateUnit {
    A,
    W,
};

namespace conversions {
/// \brief Converts the given ChargingRateUnit \p e to human readable string
/// \returns a string representation of the ChargingRateUnit
std::string charging_rate_unit_to_string(ChargingRateUnit e);

/// \brief Converts the given std::string \p s to ChargingRateUnit
/// \returns a ChargingRateUnit from a string representation
ChargingRateUnit string_to_charging_rate_unit(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingRateUnit \p charging_rate_unit to the given output
/// stream \p os \returns an output stream with the ChargingRateUnit written to
std::ostream& operator<<(std::ostream& os, const ChargingRateUnit& charging_rate_unit);

// from: GetCompositeScheduleResponse
enum class GetCompositeScheduleStatus {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given GetCompositeScheduleStatus \p e to human readable string
/// \returns a string representation of the GetCompositeScheduleStatus
std::string get_composite_schedule_status_to_string(GetCompositeScheduleStatus e);

/// \brief Converts the given std::string \p s to GetCompositeScheduleStatus
/// \returns a GetCompositeScheduleStatus from a string representation
GetCompositeScheduleStatus string_to_get_composite_schedule_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetCompositeScheduleStatus \p get_composite_schedule_status to
/// the given output stream \p os \returns an output stream with the GetCompositeScheduleStatus written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleStatus& get_composite_schedule_status);

// from: GetInstalledCertificateIdsRequest
enum class CertificateUseEnumType {
    CentralSystemRootCertificate,
    ManufacturerRootCertificate,
};

namespace conversions {
/// \brief Converts the given CertificateUseEnumType \p e to human readable string
/// \returns a string representation of the CertificateUseEnumType
std::string certificate_use_enum_type_to_string(CertificateUseEnumType e);

/// \brief Converts the given std::string \p s to CertificateUseEnumType
/// \returns a CertificateUseEnumType from a string representation
CertificateUseEnumType string_to_certificate_use_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateUseEnumType \p certificate_use_enum_type to the
/// given output stream \p os \returns an output stream with the CertificateUseEnumType written to
std::ostream& operator<<(std::ostream& os, const CertificateUseEnumType& certificate_use_enum_type);

// from: GetInstalledCertificateIdsResponse
enum class GetInstalledCertificateStatusEnumType {
    Accepted,
    NotFound,
};

namespace conversions {
/// \brief Converts the given GetInstalledCertificateStatusEnumType \p e to human readable string
/// \returns a string representation of the GetInstalledCertificateStatusEnumType
std::string get_installed_certificate_status_enum_type_to_string(GetInstalledCertificateStatusEnumType e);

/// \brief Converts the given std::string \p s to GetInstalledCertificateStatusEnumType
/// \returns a GetInstalledCertificateStatusEnumType from a string representation
GetInstalledCertificateStatusEnumType string_to_get_installed_certificate_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GetInstalledCertificateStatusEnumType \p
/// get_installed_certificate_status_enum_type to the given output stream \p os \returns an output stream with the
/// GetInstalledCertificateStatusEnumType written to
std::ostream& operator<<(std::ostream& os,
                         const GetInstalledCertificateStatusEnumType& get_installed_certificate_status_enum_type);

// from: GetLogRequest
enum class LogEnumType {
    DiagnosticsLog,
    SecurityLog,
};

namespace conversions {
/// \brief Converts the given LogEnumType \p e to human readable string
/// \returns a string representation of the LogEnumType
std::string log_enum_type_to_string(LogEnumType e);

/// \brief Converts the given std::string \p s to LogEnumType
/// \returns a LogEnumType from a string representation
LogEnumType string_to_log_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given LogEnumType \p log_enum_type to the given output stream \p os
/// \returns an output stream with the LogEnumType written to
std::ostream& operator<<(std::ostream& os, const LogEnumType& log_enum_type);

// from: GetLogResponse
enum class LogStatusEnumType {
    Accepted,
    Rejected,
    AcceptedCanceled,
};

namespace conversions {
/// \brief Converts the given LogStatusEnumType \p e to human readable string
/// \returns a string representation of the LogStatusEnumType
std::string log_status_enum_type_to_string(LogStatusEnumType e);

/// \brief Converts the given std::string \p s to LogStatusEnumType
/// \returns a LogStatusEnumType from a string representation
LogStatusEnumType string_to_log_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given LogStatusEnumType \p log_status_enum_type to the given output
/// stream \p os \returns an output stream with the LogStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const LogStatusEnumType& log_status_enum_type);

// from: InstallCertificateResponse
enum class InstallCertificateStatusEnumType {
    Accepted,
    Failed,
    Rejected,
};

namespace conversions {
/// \brief Converts the given InstallCertificateStatusEnumType \p e to human readable string
/// \returns a string representation of the InstallCertificateStatusEnumType
std::string install_certificate_status_enum_type_to_string(InstallCertificateStatusEnumType e);

/// \brief Converts the given std::string \p s to InstallCertificateStatusEnumType
/// \returns a InstallCertificateStatusEnumType from a string representation
InstallCertificateStatusEnumType string_to_install_certificate_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given InstallCertificateStatusEnumType \p
/// install_certificate_status_enum_type to the given output stream \p os \returns an output stream with the
/// InstallCertificateStatusEnumType written to
std::ostream& operator<<(std::ostream& os,
                         const InstallCertificateStatusEnumType& install_certificate_status_enum_type);

// from: LogStatusNotificationRequest
enum class UploadLogStatusEnumType {
    BadMessage,
    Idle,
    NotSupportedOperation,
    PermissionDenied,
    Uploaded,
    UploadFailure,
    Uploading,
};

namespace conversions {
/// \brief Converts the given UploadLogStatusEnumType \p e to human readable string
/// \returns a string representation of the UploadLogStatusEnumType
std::string upload_log_status_enum_type_to_string(UploadLogStatusEnumType e);

/// \brief Converts the given std::string \p s to UploadLogStatusEnumType
/// \returns a UploadLogStatusEnumType from a string representation
UploadLogStatusEnumType string_to_upload_log_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UploadLogStatusEnumType \p upload_log_status_enum_type to the
/// given output stream \p os \returns an output stream with the UploadLogStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const UploadLogStatusEnumType& upload_log_status_enum_type);

// from: MeterValuesRequest
enum class ReadingContext {
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
/// \brief Converts the given ReadingContext \p e to human readable string
/// \returns a string representation of the ReadingContext
std::string reading_context_to_string(ReadingContext e);

/// \brief Converts the given std::string \p s to ReadingContext
/// \returns a ReadingContext from a string representation
ReadingContext string_to_reading_context(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReadingContext \p reading_context to the given output stream \p
/// os \returns an output stream with the ReadingContext written to
std::ostream& operator<<(std::ostream& os, const ReadingContext& reading_context);

// from: MeterValuesRequest
enum class ValueFormat {
    Raw,
    SignedData,
};

namespace conversions {
/// \brief Converts the given ValueFormat \p e to human readable string
/// \returns a string representation of the ValueFormat
std::string value_format_to_string(ValueFormat e);

/// \brief Converts the given std::string \p s to ValueFormat
/// \returns a ValueFormat from a string representation
ValueFormat string_to_value_format(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ValueFormat \p value_format to the given output stream \p os
/// \returns an output stream with the ValueFormat written to
std::ostream& operator<<(std::ostream& os, const ValueFormat& value_format);

// from: MeterValuesRequest
enum class Measurand {
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
/// \brief Converts the given Measurand \p e to human readable string
/// \returns a string representation of the Measurand
std::string measurand_to_string(Measurand e);

/// \brief Converts the given std::string \p s to Measurand
/// \returns a Measurand from a string representation
Measurand string_to_measurand(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given Measurand \p measurand to the given output stream \p os
/// \returns an output stream with the Measurand written to
std::ostream& operator<<(std::ostream& os, const Measurand& measurand);

// from: MeterValuesRequest
enum class Phase {
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
/// \brief Converts the given Phase \p e to human readable string
/// \returns a string representation of the Phase
std::string phase_to_string(Phase e);

/// \brief Converts the given std::string \p s to Phase
/// \returns a Phase from a string representation
Phase string_to_phase(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given Phase \p phase to the given output stream \p os
/// \returns an output stream with the Phase written to
std::ostream& operator<<(std::ostream& os, const Phase& phase);

// from: MeterValuesRequest
enum class Location {
    Cable,
    EV,
    Inlet,
    Outlet,
    Body,
};

namespace conversions {
/// \brief Converts the given Location \p e to human readable string
/// \returns a string representation of the Location
std::string location_to_string(Location e);

/// \brief Converts the given std::string \p s to Location
/// \returns a Location from a string representation
Location string_to_location(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given Location \p location to the given output stream \p os
/// \returns an output stream with the Location written to
std::ostream& operator<<(std::ostream& os, const Location& location);

// from: MeterValuesRequest
enum class UnitOfMeasure {
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
    RevolutionsPerMinute,
};

namespace conversions {
/// \brief Converts the given UnitOfMeasure \p e to human readable string
/// \returns a string representation of the UnitOfMeasure
std::string unit_of_measure_to_string(UnitOfMeasure e);

/// \brief Converts the given std::string \p s to UnitOfMeasure
/// \returns a UnitOfMeasure from a string representation
UnitOfMeasure string_to_unit_of_measure(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UnitOfMeasure \p unit_of_measure to the given output stream \p
/// os \returns an output stream with the UnitOfMeasure written to
std::ostream& operator<<(std::ostream& os, const UnitOfMeasure& unit_of_measure);

// from: RemoteStartTransactionRequest
enum class ChargingProfileKindType {
    Absolute,
    Recurring,
    Relative,
};

namespace conversions {
/// \brief Converts the given ChargingProfileKindType \p e to human readable string
/// \returns a string representation of the ChargingProfileKindType
std::string charging_profile_kind_type_to_string(ChargingProfileKindType e);

/// \brief Converts the given std::string \p s to ChargingProfileKindType
/// \returns a ChargingProfileKindType from a string representation
ChargingProfileKindType string_to_charging_profile_kind_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileKindType \p charging_profile_kind_type to the
/// given output stream \p os \returns an output stream with the ChargingProfileKindType written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileKindType& charging_profile_kind_type);

// from: RemoteStartTransactionRequest
enum class RecurrencyKindType {
    Daily,
    Weekly,
};

namespace conversions {
/// \brief Converts the given RecurrencyKindType \p e to human readable string
/// \returns a string representation of the RecurrencyKindType
std::string recurrency_kind_type_to_string(RecurrencyKindType e);

/// \brief Converts the given std::string \p s to RecurrencyKindType
/// \returns a RecurrencyKindType from a string representation
RecurrencyKindType string_to_recurrency_kind_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RecurrencyKindType \p recurrency_kind_type to the given output
/// stream \p os \returns an output stream with the RecurrencyKindType written to
std::ostream& operator<<(std::ostream& os, const RecurrencyKindType& recurrency_kind_type);

// from: RemoteStartTransactionResponse
enum class RemoteStartStopStatus {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given RemoteStartStopStatus \p e to human readable string
/// \returns a string representation of the RemoteStartStopStatus
std::string remote_start_stop_status_to_string(RemoteStartStopStatus e);

/// \brief Converts the given std::string \p s to RemoteStartStopStatus
/// \returns a RemoteStartStopStatus from a string representation
RemoteStartStopStatus string_to_remote_start_stop_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given RemoteStartStopStatus \p remote_start_stop_status to the given
/// output stream \p os \returns an output stream with the RemoteStartStopStatus written to
std::ostream& operator<<(std::ostream& os, const RemoteStartStopStatus& remote_start_stop_status);

// from: ReserveNowResponse
enum class ReservationStatus {
    Accepted,
    Faulted,
    Occupied,
    Rejected,
    Unavailable,
};

namespace conversions {
/// \brief Converts the given ReservationStatus \p e to human readable string
/// \returns a string representation of the ReservationStatus
std::string reservation_status_to_string(ReservationStatus e);

/// \brief Converts the given std::string \p s to ReservationStatus
/// \returns a ReservationStatus from a string representation
ReservationStatus string_to_reservation_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ReservationStatus \p reservation_status to the given output
/// stream \p os \returns an output stream with the ReservationStatus written to
std::ostream& operator<<(std::ostream& os, const ReservationStatus& reservation_status);

// from: ResetRequest
enum class ResetType {
    Hard,
    Soft,
};

namespace conversions {
/// \brief Converts the given ResetType \p e to human readable string
/// \returns a string representation of the ResetType
std::string reset_type_to_string(ResetType e);

/// \brief Converts the given std::string \p s to ResetType
/// \returns a ResetType from a string representation
ResetType string_to_reset_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ResetType \p reset_type to the given output stream \p os
/// \returns an output stream with the ResetType written to
std::ostream& operator<<(std::ostream& os, const ResetType& reset_type);

// from: ResetResponse
enum class ResetStatus {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given ResetStatus \p e to human readable string
/// \returns a string representation of the ResetStatus
std::string reset_status_to_string(ResetStatus e);

/// \brief Converts the given std::string \p s to ResetStatus
/// \returns a ResetStatus from a string representation
ResetStatus string_to_reset_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ResetStatus \p reset_status to the given output stream \p os
/// \returns an output stream with the ResetStatus written to
std::ostream& operator<<(std::ostream& os, const ResetStatus& reset_status);

// from: SendLocalListRequest
enum class UpdateType {
    Differential,
    Full,
};

namespace conversions {
/// \brief Converts the given UpdateType \p e to human readable string
/// \returns a string representation of the UpdateType
std::string update_type_to_string(UpdateType e);

/// \brief Converts the given std::string \p s to UpdateType
/// \returns a UpdateType from a string representation
UpdateType string_to_update_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UpdateType \p update_type to the given output stream \p os
/// \returns an output stream with the UpdateType written to
std::ostream& operator<<(std::ostream& os, const UpdateType& update_type);

// from: SendLocalListResponse
enum class UpdateStatus {
    Accepted,
    Failed,
    NotSupported,
    VersionMismatch,
};

namespace conversions {
/// \brief Converts the given UpdateStatus \p e to human readable string
/// \returns a string representation of the UpdateStatus
std::string update_status_to_string(UpdateStatus e);

/// \brief Converts the given std::string \p s to UpdateStatus
/// \returns a UpdateStatus from a string representation
UpdateStatus string_to_update_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UpdateStatus \p update_status to the given output stream \p os
/// \returns an output stream with the UpdateStatus written to
std::ostream& operator<<(std::ostream& os, const UpdateStatus& update_status);

// from: SetChargingProfileResponse
enum class ChargingProfileStatus {
    Accepted,
    Rejected,
    NotSupported,
};

namespace conversions {
/// \brief Converts the given ChargingProfileStatus \p e to human readable string
/// \returns a string representation of the ChargingProfileStatus
std::string charging_profile_status_to_string(ChargingProfileStatus e);

/// \brief Converts the given std::string \p s to ChargingProfileStatus
/// \returns a ChargingProfileStatus from a string representation
ChargingProfileStatus string_to_charging_profile_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargingProfileStatus \p charging_profile_status to the given
/// output stream \p os \returns an output stream with the ChargingProfileStatus written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileStatus& charging_profile_status);

// from: SignCertificateResponse
enum class GenericStatusEnumType {
    Accepted,
    Rejected,
};

namespace conversions {
/// \brief Converts the given GenericStatusEnumType \p e to human readable string
/// \returns a string representation of the GenericStatusEnumType
std::string generic_status_enum_type_to_string(GenericStatusEnumType e);

/// \brief Converts the given std::string \p s to GenericStatusEnumType
/// \returns a GenericStatusEnumType from a string representation
GenericStatusEnumType string_to_generic_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given GenericStatusEnumType \p generic_status_enum_type to the given
/// output stream \p os \returns an output stream with the GenericStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const GenericStatusEnumType& generic_status_enum_type);

// from: SignedFirmwareStatusNotificationRequest
enum class FirmwareStatusEnumType {
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
/// \brief Converts the given FirmwareStatusEnumType \p e to human readable string
/// \returns a string representation of the FirmwareStatusEnumType
std::string firmware_status_enum_type_to_string(FirmwareStatusEnumType e);

/// \brief Converts the given std::string \p s to FirmwareStatusEnumType
/// \returns a FirmwareStatusEnumType from a string representation
FirmwareStatusEnumType string_to_firmware_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given FirmwareStatusEnumType \p firmware_status_enum_type to the
/// given output stream \p os \returns an output stream with the FirmwareStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusEnumType& firmware_status_enum_type);

// from: SignedUpdateFirmwareResponse
enum class UpdateFirmwareStatusEnumType {
    Accepted,
    Rejected,
    AcceptedCanceled,
    InvalidCertificate,
    RevokedCertificate,
};

namespace conversions {
/// \brief Converts the given UpdateFirmwareStatusEnumType \p e to human readable string
/// \returns a string representation of the UpdateFirmwareStatusEnumType
std::string update_firmware_status_enum_type_to_string(UpdateFirmwareStatusEnumType e);

/// \brief Converts the given std::string \p s to UpdateFirmwareStatusEnumType
/// \returns a UpdateFirmwareStatusEnumType from a string representation
UpdateFirmwareStatusEnumType string_to_update_firmware_status_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UpdateFirmwareStatusEnumType \p
/// update_firmware_status_enum_type to the given output stream \p os \returns an output stream with the
/// UpdateFirmwareStatusEnumType written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareStatusEnumType& update_firmware_status_enum_type);

// from: StatusNotificationRequest
enum class ChargePointErrorCode {
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
/// \brief Converts the given ChargePointErrorCode \p e to human readable string
/// \returns a string representation of the ChargePointErrorCode
std::string charge_point_error_code_to_string(ChargePointErrorCode e);

/// \brief Converts the given std::string \p s to ChargePointErrorCode
/// \returns a ChargePointErrorCode from a string representation
ChargePointErrorCode string_to_charge_point_error_code(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargePointErrorCode \p charge_point_error_code to the given
/// output stream \p os \returns an output stream with the ChargePointErrorCode written to
std::ostream& operator<<(std::ostream& os, const ChargePointErrorCode& charge_point_error_code);

// from: StatusNotificationRequest
enum class ChargePointStatus {
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
/// \brief Converts the given ChargePointStatus \p e to human readable string
/// \returns a string representation of the ChargePointStatus
std::string charge_point_status_to_string(ChargePointStatus e);

/// \brief Converts the given std::string \p s to ChargePointStatus
/// \returns a ChargePointStatus from a string representation
ChargePointStatus string_to_charge_point_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given ChargePointStatus \p charge_point_status to the given output
/// stream \p os \returns an output stream with the ChargePointStatus written to
std::ostream& operator<<(std::ostream& os, const ChargePointStatus& charge_point_status);

// from: StopTransactionRequest
enum class Reason {
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
/// \brief Converts the given Reason \p e to human readable string
/// \returns a string representation of the Reason
std::string reason_to_string(Reason e);

/// \brief Converts the given std::string \p s to Reason
/// \returns a Reason from a string representation
Reason string_to_reason(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given Reason \p reason to the given output stream \p os
/// \returns an output stream with the Reason written to
std::ostream& operator<<(std::ostream& os, const Reason& reason);

// from: TriggerMessageRequest
enum class MessageTrigger {
    BootNotification,
    DiagnosticsStatusNotification,
    FirmwareStatusNotification,
    Heartbeat,
    MeterValues,
    StatusNotification,
};

namespace conversions {
/// \brief Converts the given MessageTrigger \p e to human readable string
/// \returns a string representation of the MessageTrigger
std::string message_trigger_to_string(MessageTrigger e);

/// \brief Converts the given std::string \p s to MessageTrigger
/// \returns a MessageTrigger from a string representation
MessageTrigger string_to_message_trigger(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given MessageTrigger \p message_trigger to the given output stream \p
/// os \returns an output stream with the MessageTrigger written to
std::ostream& operator<<(std::ostream& os, const MessageTrigger& message_trigger);

// from: TriggerMessageResponse
enum class TriggerMessageStatus {
    Accepted,
    Rejected,
    NotImplemented,
};

namespace conversions {
/// \brief Converts the given TriggerMessageStatus \p e to human readable string
/// \returns a string representation of the TriggerMessageStatus
std::string trigger_message_status_to_string(TriggerMessageStatus e);

/// \brief Converts the given std::string \p s to TriggerMessageStatus
/// \returns a TriggerMessageStatus from a string representation
TriggerMessageStatus string_to_trigger_message_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given TriggerMessageStatus \p trigger_message_status to the given
/// output stream \p os \returns an output stream with the TriggerMessageStatus written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageStatus& trigger_message_status);

// from: UnlockConnectorResponse
enum class UnlockStatus {
    Unlocked,
    UnlockFailed,
    NotSupported,
};

namespace conversions {
/// \brief Converts the given UnlockStatus \p e to human readable string
/// \returns a string representation of the UnlockStatus
std::string unlock_status_to_string(UnlockStatus e);

/// \brief Converts the given std::string \p s to UnlockStatus
/// \returns a UnlockStatus from a string representation
UnlockStatus string_to_unlock_status(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given UnlockStatus \p unlock_status to the given output stream \p os
/// \returns an output stream with the UnlockStatus written to
std::ostream& operator<<(std::ostream& os, const UnlockStatus& unlock_status);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_ENUMS_HPP
