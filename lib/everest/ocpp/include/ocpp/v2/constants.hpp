// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstddef>

namespace ocpp {
namespace v2 {

/// \brief OCPP 2.0.1 defines this as 5600 but it can be set to a higher value, which we do here, if it's reported via
/// the device model, which we do as well
/// 17000 is the minimum value from OCPP 2.1
constexpr std::size_t ISO15118_GET_EV_CERTIFICATE_EXI_RESPONSE_SIZE = 17000;

} // namespace v2
} // namespace ocpp
