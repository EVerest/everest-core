// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace iso15118::d2::msg {

namespace data_types {
struct Parameter {
    std::string name;
    std::variant<bool, int8_t, int16_t, int32_t, PhysicalValue, std::string> value;
};

struct ParameterSet {
    ParameterSetID parameter_set_id;
    std::vector<Parameter> parameter; // [1 - 16]
};
constexpr auto ParameterSetListMaxLength = 16;

using ServiceParameterList = std::vector<ParameterSet>; // [1 - 255]
constexpr auto ServiceParameterListMaxLength = 255;
} // namespace data_types

struct ServiceDetailRequest {
    Header header;
    data_types::ServiceID service_id;
};

struct ServiceDetailResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::ServiceID service_id;
    std::optional<data_types::ServiceParameterList> service_parameter_list{std::nullopt};
};

} // namespace iso15118::d2::msg
