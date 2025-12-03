// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_TYPES_HPP
#define OCPP_V16_TYPES_HPP

#include <iostream>
#include <sstream>

#include <nlohmann/json_fwd.hpp>

#include <everest/logging.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

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
    Internal,
    Core,
    CostAndPrice,
    FirmwareManagement,
    LocalAuthListManagement,
    Reservation,
    SmartCharging,
    RemoteTrigger,
    Security,
    PnC,
    Custom
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

/// \brief Contains the different connection states of the charge point
enum ChargePointConnectionState {
    Disconnected, // state when disconnected
    Connected,    // state when ws is connected
    Booted,       // state when ws is connected and BootNotifcation had been Accepted
    Pending,      // state when ws is connected and state when BootNotifcation is Pending
    Rejected,     // state when ws is connected and state when BootNotifcation had been Rejected
};
namespace conversions {
/// \brief Converts the given ChargePointConnectionState \p e to std::string
/// \returns a string representation of the ChargePointConnectionState
std::string charge_point_connection_state_to_string(ChargePointConnectionState e);

/// \brief Converts the given std::string \p s to ChargePointConnectionState
/// \returns a ChargePointConnectionState from a string representation
ChargePointConnectionState string_to_charge_point_connection_state(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p charge_point_connection_state
/// to the given output stream \p os \returns an output stream with the ChargePointConnectionState written to
std::ostream& operator<<(std::ostream& os, const ChargePointConnectionState& charge_point_connection_state);

/// \brief Combines a Measurand with an optional Phase
struct MeasurandWithPhase {
    Measurand measurand;        ///< A OCPP Measurand
    std::optional<Phase> phase; ///< If applicable and available a Phase

    /// \brief Comparison operator== between this MeasurandWithPhase and the given \p measurand_with_phase
    /// \returns true when measurand and phase are equal
    bool operator==(MeasurandWithPhase measurand_with_phase);
};

/// \brief Combines a Measurand with a list of Phases
struct MeasurandWithPhases {
    Measurand measurand;       ///< A OCPP Measurand
    std::vector<Phase> phases; ///< A list of Phases
};

/// \brief Combines AvailabilityType with persist flag for scheduled Availability changes
struct AvailabilityChange {
    AvailabilityType availability;
    bool persist;
};

/// \brief BootReasonEnum contains the different boot reasons of the charge point (copied from OCPP2.0.1 definition)
enum BootReasonEnum {
    ApplicationReset,
    FirmwareUpdate,
    LocalReset,
    PowerUp,
    RemoteReset,
    ScheduledReset,
    Triggered,
    Unknown,
    Watchdog
};

/// \brief Enhances ChargingSchedulePeriod with stackLevel
struct EnhancedChargingSchedulePeriod {
    int32_t startPeriod;
    float limit;
    std::optional<int32_t> numberPhases;
    int32_t stackLevel;
    bool periodTransformed = false; // indicates that a period was transformed from chargingRateUnit
};

/// \brief Conversion from a given EnhancedChargingSchedulePeriod \p k to a given json object \p j
void to_json(json& j, const EnhancedChargingSchedulePeriod& k);

/// \brief Conversion from a given json object \p j to a given EnhancedChargingSchedulePeriod \p k
void from_json(const json& j, EnhancedChargingSchedulePeriod& k);

/// \brief Enhances ChargingSchedule by containing std::vector<EnhancedChargingSchedulePeriods> instead of
/// std::vector<ChargingSchedulePeriod>
struct EnhancedChargingSchedule {
    ChargingRateUnit chargingRateUnit;
    std::vector<EnhancedChargingSchedulePeriod> chargingSchedulePeriod;
    std::optional<int32_t> duration;
    std::optional<ocpp::DateTime> startSchedule;
    std::optional<float> minChargingRate;
};

/// \brief Conversion from a given EnhancedChargingSchedule \p k to a given json object \p j
void to_json(json& j, const EnhancedChargingSchedule& k);

/// \brief Conversion from a given json object \p j to a given EnhancedChargingSchedule \p k
void from_json(const json& j, EnhancedChargingSchedule& k);

/// \brief Extends the IdTagInfo with an optional TariffMessage for california pricing
struct EnhancedIdTagInfo {
    IdTagInfo id_tag_info;
    std::optional<TariffMessage> tariff_message;
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_TYPES_HPP
