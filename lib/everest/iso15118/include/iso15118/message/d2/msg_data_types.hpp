// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

namespace iso15118::d2::msg {

template <typename InType, typename OutType> void convert(const InType&, OutType&);

namespace data_types {

constexpr auto SESSION_ID_LENGTH = 8;
using SESSION_ID = std::array<uint8_t, SESSION_ID_LENGTH>; // hexBinary, max length 8

constexpr auto GEN_CHALLENGE_LENGTH = 16;
using GenChallenge = std::array<uint8_t, GEN_CHALLENGE_LENGTH>; // base64 binary
using PercentValue = uint8_t;                                   // [0 - 100]
using SAScheduleTupleID = int16_t;                              // [1-255]
using MeterID = std::string;                                    // MaxLength: 32
using EVSEID = std::string;                                     // Length: 7-37
using MeterReading = uint64_t;                                  // Wh
using SigMeterReading = std::vector<uint8_t>;                   // MaxLength: 64
using MeterStatus = int16_t;
using TMeter = int64_t; // Unix timestamp format

enum class ResponseCode {
    OK,
    OK_NewSessionEstablished,
    OK_OldSessionJoined,
    OK_CertificateExpiresSoon,
    FAILED,
    FAILED_SequenceError,
    FAILED_ServiceIDInvalid,
    FAILED_UnknownSession,
    FAILED_ServiceSelectionInvalid,
    FAILED_PaymentSelectionInvalid,
    FAILED_CertificateExpired,
    FAILED_SignatureError,
    FAILED_NoCertificateAvailable,
    FAILED_CertChainError,
    FAILED_ChallengeInvalid,
    FAILED_ContractCanceled,
    FAILED_WrongChargeParameter,
    FAILED_PowerDeliveryNotApplied,
    FAILED_TariffSelectionInvalid,
    FAILED_ChargingProfileInvalid,
    FAILED_MeteringSignatureNotValid,
    FAILED_NoChargeServiceSelected,
    FAILED_WrongEnergyTransferMode,
    FAILED_ContactorError,
    FAILED_CertificateNotAllowedAtThisEVSE,
    FAILED_CertificateRevoked,
};

enum class FaultCode {
    ParsingError,
    NoTLSRootCertificatAvailable,
    UnknownError,
};

enum class EvseProcessing {
    Finished,
    Ongoing,
    Ongoing_WaitingForCustomerInteraction
};

enum class DC_EVErrorCode {
    NO_ERROR,
    FAILED_RESSTemperatureInhibit,
    FAILED_EVShiftPosition,
    FAILED_ChargerConnectorLockFault,
    FAILED_EVRESSMalfunction,
    FAILED_ChargingCurrentdifferential,
    FAILED_ChargingVoltageOutOfRange,
    Reserved_A,
    Reserved_B,
    Reserved_C,
    FAILED_ChargingSystemIncompatibility,
    NoData,
};

enum class EVSEProcessing {
    Finished,
    Ongoing,
    Ongoing_WaitingForCustomerInteraction
};

enum class EVSENotification {
    None,
    StopCharging,
    ReNegotiation,
};

enum class UnitSymbol {
    h,
    m,
    s,
    A,
    V,
    W,
    Wh
};

enum class DC_EVSEStatusCode {
    EVSE_NotReady,
    EVSE_Ready,
    EVSE_Shutdown,
    EVSE_UtilityInterruptEvent,
    EVSE_IsolationMonitoringActive,
    EVSE_EmergencyShutdown,
    EVSE_Malfunction,
    Reserved_8,
    Reserved_9,
    Reserved_A,
    Reserved_B,
    Reserved_C
};

enum class IsolationLevel {
    Invalid,
    Valid,
    Warning,
    Fault,
    No_IMD
};

struct MeterInfo {
    MeterID meter_id;
    std::optional<MeterReading> meter_reading{std::nullopt};
    std::optional<SigMeterReading> sig_meter_reading{std::nullopt};
    std::optional<MeterStatus> meter_status{std::nullopt};
    std::optional<TMeter> t_meter{std::nullopt};
};

struct PhysicalValue {
    int16_t value{0};
    int8_t multiplier{0}; // [-3 - 3]
    UnitSymbol unit;
};

struct Notification {
    FaultCode fault_code;
    std::optional<std::string> fault_msg;
};

struct EVSEStatus {
    uint16_t notification_max_delay{0};
    EVSENotification evse_notification{EVSENotification::None};
};

struct AC_EVSEStatus : EVSEStatus {
    bool rcd;
};

struct DC_EVSEStatus : EVSEStatus {
    std::optional<IsolationLevel> evse_isolation_status;
    DC_EVSEStatusCode evse_status_code;
};

struct DC_EVStatus {
    bool ev_ready;
    DC_EVErrorCode ev_error_code;
    PercentValue ev_ress_soc;
};

float from_PhysicalValue(const PhysicalValue& in);
PhysicalValue from_float(const float in, const data_types::UnitSymbol unit);

} // namespace data_types

struct Header {
    data_types::SESSION_ID session_id;
    std::optional<data_types::Notification> notification;
    // TODO: Missing xml signature
};

void convert(const struct iso2_MessageHeaderType& in, Header& out);
void convert(const Header& in, struct iso2_MessageHeaderType& out);

} // namespace iso15118::d2::msg
