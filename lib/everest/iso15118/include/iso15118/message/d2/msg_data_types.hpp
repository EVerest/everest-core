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

struct Notification {
    FaultCode fault_code;
    std::optional<std::string> fault_msg;
};

} // namespace data_types

struct Header {
    data_types::SESSION_ID session_id;
    std::optional<data_types::Notification> notification;
    // TODO: Missing xml signature
};

void convert(const struct iso2_MessageHeaderType& in, Header& out);
void convert(const Header& in, struct iso2_MessageHeaderType& out);

} // namespace iso15118::d2::msg
