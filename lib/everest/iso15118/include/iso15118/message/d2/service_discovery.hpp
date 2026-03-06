// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <array>
#include <ctype.h>
#include <optional>
#include <string>
#include <vector>

namespace iso15118::d2::msg {

struct ServiceDiscoveryRequest {
    Header header;
    std::optional<data_types::ServiceScope> service_scope{std::nullopt};
    std::optional<data_types::ServiceCategory> service_category{std::nullopt};
};

struct ServiceDiscoveryResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::PaymentOptionList payment_option_list;
    data_types::ChargeService charge_service;
    std::optional<data_types::ServiceList> service_list{std::nullopt};
};

} // namespace iso15118::d2::msg
