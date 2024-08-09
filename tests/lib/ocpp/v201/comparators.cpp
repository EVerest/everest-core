// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <comparators.hpp>

namespace testing::internal {

bool operator==(const ::ocpp::CertificateHashDataType& a, const ::ocpp::CertificateHashDataType& b) {
    return a.serialNumber == b.serialNumber && a.issuerKeyHash == b.issuerKeyHash &&
           a.issuerNameHash == b.issuerNameHash && a.hashAlgorithm == b.hashAlgorithm;
}
bool operator==(const ::ocpp::v201::GetCertificateStatusRequest& a,
                const ::ocpp::v201::GetCertificateStatusRequest& b) {
    return a.ocspRequestData.serialNumber == b.ocspRequestData.serialNumber &&
           a.ocspRequestData.issuerKeyHash == b.ocspRequestData.issuerKeyHash &&
           a.ocspRequestData.issuerNameHash == b.ocspRequestData.issuerNameHash &&
           a.ocspRequestData.hashAlgorithm == b.ocspRequestData.hashAlgorithm &&
           a.ocspRequestData.responderURL == b.ocspRequestData.responderURL;
}

} // namespace testing::internal

namespace ocpp::v201 {

bool operator==(const ChargingProfile& a, const ChargingProfile& b) {
    return a.chargingProfileKind == b.chargingProfileKind && a.chargingProfilePurpose == b.chargingProfilePurpose &&
           a.id == b.id && a.stackLevel == b.stackLevel;
}
} // namespace ocpp::v201