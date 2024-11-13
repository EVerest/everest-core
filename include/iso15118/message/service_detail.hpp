// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>
#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

struct Parameter {
    Name name;
    std::variant<bool, int8_t, int16_t, int32_t, Name, RationalNumber> value;
};

struct ParameterSet {
    uint16_t id;
    std::vector<Parameter> parameter;

    ParameterSet();
    ParameterSet(uint16_t _id, const DcParameterList& list);
    ParameterSet(uint16_t _id, const DcBptParameterList& list);
    ParameterSet(uint16_t _id, const InternetParameterList& list);
    ParameterSet(uint16_t _id, const ParkingParameterList& list);
};

using ServiceParameterList = std::vector<ParameterSet>; // Max: 32

} // namespace datatypes

struct ServiceDetailRequest {
    Header header;
    datatypes::ServiceCategory service;
};

struct ServiceDetailResponse {
    Header header;
    datatypes::ResponseCode response_code;
    datatypes::ServiceCategory service{datatypes::ServiceCategory::DC};
    datatypes::ServiceParameterList service_parameter_list = {datatypes::ParameterSet()};
};

} // namespace iso15118::message_20
