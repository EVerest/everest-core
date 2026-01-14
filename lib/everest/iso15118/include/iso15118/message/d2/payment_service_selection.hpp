// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <optional>
#include <vector>

namespace iso15118::d2::msg {

namespace data_types {
struct SelectedService {
    ServiceID service_id;
    std::optional<ParameterSetID> parameter_set_id{std::nullopt};
};
using SelectedServiceList = std::vector<SelectedService>; // [1 - 16]
} // namespace data_types

struct PaymentServiceSelectionRequest {
    Header header;
    data_types::PaymentOption selected_payment_option;
    data_types::SelectedServiceList selected_service_list;
};

struct PaymentServiceSelectionResponse {
    Header header;
    data_types::ResponseCode response_code;
};

} // namespace iso15118::d2::msg
