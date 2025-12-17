// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <string>

#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

namespace iso15118::d2::msg {

template <typename InType, typename OutType> void convert(const InType&, OutType&);

namespace data_types {

constexpr auto SESSION_ID_LENGTH = 8;
using SESSION_ID = std::array<uint8_t, SESSION_ID_LENGTH>; // hexBinary, max length 8

constexpr auto GEN_CHALLENGE_LENGTH = 16;
using GenChallenge = std::array<uint8_t, GEN_CHALLENGE_LENGTH>; // base64 binary
using PercentValue = uint8_t;                                   // [0 - 100]

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

enum class EVSENotification {
    None,
    StopCharging,
    ReNegotiation,
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

enum class isolationLevel {
    Invalid,
    Valid,
    Warning,
    Fault,
    No_IMD
};

struct Notification {
    FaultCode fault_code;
    std::optional<std::string> fault_msg;
};

struct EVSEStatus {
    uint16_t notification_max_delay{0};
    EVSENotification evse_notification{EVSENotification::None};
};

struct AC_EVSEStatus : public EVSEStatus {
    bool rcd;
};

struct DC_EVSEStatus : public EVSEStatus {
    std::optional<isolationLevel> evse_isolation_status;
    DC_EVSEStatusCode evse_status_code;
};

struct DC_EVStatus {
    bool ev_ready;
    DC_EVErrorCode ev_error_code;
    PercentValue ev_ress_soc;
};

} // namespace data_types

struct Header {
    data_types::SESSION_ID session_id;
    std::optional<data_types::Notification> notification;
    // TODO: Missing xml signature
};

void convert(const struct iso2_MessageHeaderType& in, Header& out);
void convert(const Header& in, struct iso2_MessageHeaderType& out);

void convert(const iso2_DC_EVStatusType& in, data_types::DC_EVStatus& out);
void convert(const data_types::AC_EVSEStatus& in, iso2_AC_EVSEStatusType& out);
void convert(const data_types::DC_EVSEStatus& in, iso2_DC_EVSEStatusType& out);

} // namespace iso15118::d2::msg
