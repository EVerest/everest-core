// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_TYPES_HPP
#define OCPP_V16_TYPES_HPP

#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

#include <everest/logging.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/enums.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

/// \brief Contains all supported OCPP 1.6 message types
enum class MessageType {
    Authorize,
    AuthorizeResponse,
    BootNotification,
    BootNotificationResponse,
    CancelReservation,
    CancelReservationResponse,
    CertificateSigned,
    CertificateSignedResponse,
    ChangeAvailability,
    ChangeAvailabilityResponse,
    ChangeConfiguration,
    ChangeConfigurationResponse,
    ClearCache,
    ClearCacheResponse,
    ClearChargingProfile,
    ClearChargingProfileResponse,
    DataTransfer,
    DataTransferResponse,
    DeleteCertificate,
    DeleteCertificateResponse,
    DiagnosticsStatusNotification,
    DiagnosticsStatusNotificationResponse,
    ExtendedTriggerMessage,
    ExtendedTriggerMessageResponse,
    FirmwareStatusNotification,
    FirmwareStatusNotificationResponse,
    GetCompositeSchedule,
    GetCompositeScheduleResponse,
    GetConfiguration,
    GetConfigurationResponse,
    GetDiagnostics,
    GetDiagnosticsResponse,
    GetInstalledCertificateIds,
    GetInstalledCertificateIdsResponse,
    GetLocalListVersion,
    GetLocalListVersionResponse,
    GetLog,
    GetLogResponse,
    Heartbeat,
    HeartbeatResponse,
    InstallCertificate,
    InstallCertificateResponse,
    LogStatusNotification,
    LogStatusNotificationResponse,
    MeterValues,
    MeterValuesResponse,
    RemoteStartTransaction,
    RemoteStartTransactionResponse,
    RemoteStopTransaction,
    RemoteStopTransactionResponse,
    ReserveNow,
    ReserveNowResponse,
    Reset,
    ResetResponse,
    SecurityEventNotification,
    SecurityEventNotificationResponse,
    SendLocalList,
    SendLocalListResponse,
    SetChargingProfile,
    SetChargingProfileResponse,
    SignCertificate,
    SignCertificateResponse,
    SignedFirmwareStatusNotification,
    SignedFirmwareStatusNotificationResponse,
    SignedUpdateFirmware,
    SignedUpdateFirmwareResponse,
    StartTransaction,
    StartTransactionResponse,
    StatusNotification,
    StatusNotificationResponse,
    StopTransaction,
    StopTransactionResponse,
    TriggerMessage,
    TriggerMessageResponse,
    UnlockConnector,
    UnlockConnectorResponse,
    UpdateFirmware,
    UpdateFirmwareResponse,
    InternalError, // not in spec, for internal use
};

namespace conversions {
/// \brief Converts the given MessageType \p m to std::string
/// \returns a string representation of the MessageType
std::string messagetype_to_string(MessageType m);

/// \brief Converts the given std::string \p s to MessageType
/// \returns a MessageType from a string representation
MessageType string_to_messagetype(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p message_type to the given output stream \p os
/// \returns an output stream with the MessageType written to
std::ostream& operator<<(std::ostream& os, const MessageType& message_type);

/// \brief Contains the supported OCPP 1.6 feature profiles
enum SupportedFeatureProfiles {
    Core,
    FirmwareManagement,
    LocalAuthListManagement,
    Reservation,
    SmartCharging,
    RemoteTrigger,
    Security
};
namespace conversions {
/// \brief Converts the given SupportedFeatureProfiles \p e to std::string
/// \returns a string representation of the SupportedFeatureProfiles
std::string supported_feature_profiles_to_string(SupportedFeatureProfiles e);

/// \brief Converts the given std::string \p s to SupportedFeatureProfiles
/// \returns a SupportedFeatureProfiles from a string representation
SupportedFeatureProfiles string_to_supported_feature_profiles(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p supported_feature_profiles to the given output stream \p os
/// \returns an output stream with the SupportedFeatureProfiles written to
std::ostream& operator<<(std::ostream& os, const SupportedFeatureProfiles& supported_feature_profiles);

/// \brief Combines a Measurand with an optional Phase
struct MeasurandWithPhase {
    Measurand measurand;          ///< A OCPP Measurand
    boost::optional<Phase> phase; ///< If applicable and available a Phase

    /// \brief Comparison operator== between this MeasurandWithPhase and the given \p measurand_with_phase
    /// \returns true when measurand and phase are equal
    bool operator==(MeasurandWithPhase measurand_with_phase);
};

/// \brief Combines a Measurand with a list of Phases
struct MeasurandWithPhases {
    Measurand measurand;       ///< A OCPP Measurand
    std::vector<Phase> phases; ///< A list of Phases
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_TYPES_HPP
