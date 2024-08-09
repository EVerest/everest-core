// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef TESTS_OCPP_COMPARATORS_H
#define TESTS_OCPP_COMPARATORS_H

#include <ocpp/common/types.hpp>
#include <ocpp/v201/messages/GetCertificateStatus.hpp>
#include <ocpp/v201/types.hpp>

namespace testing::internal {

bool operator==(const ::ocpp::CertificateHashDataType& a, const ::ocpp::CertificateHashDataType& b);
bool operator==(const ::ocpp::v201::GetCertificateStatusRequest& a, const ::ocpp::v201::GetCertificateStatusRequest& b);

} // namespace testing::internal

namespace ocpp::v201 {

bool operator==(const ChargingProfile& a, const ChargingProfile& b);

} // namespace ocpp::v201

#endif // TESTS_OCPP_COMPARATORS_H
