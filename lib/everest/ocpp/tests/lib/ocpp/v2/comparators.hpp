// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef TESTS_OCPP_COMPARATORS_H
#define TESTS_OCPP_COMPARATORS_H

#include <ocpp/common/types.hpp>
#include <ocpp/v2/messages/GetCertificateStatus.hpp>
#include <ocpp/v2/types.hpp>

namespace testing::internal {

bool operator==(const ::ocpp::CertificateHashDataType& a, const ::ocpp::CertificateHashDataType& b);
bool operator==(const ::ocpp::v2::GetCertificateStatusRequest& a, const ::ocpp::v2::GetCertificateStatusRequest& b);

} // namespace testing::internal

namespace ocpp::v2 {

bool operator==(const ChargingProfile& a, const ChargingProfile& b);

} // namespace ocpp::v2

#endif // TESTS_OCPP_COMPARATORS_H
