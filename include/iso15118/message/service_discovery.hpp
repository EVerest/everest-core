// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {
using ServiceIdList = std::vector<std::uint16_t>; //

struct Service {
    ServiceCategory service_id;
    bool free_service;
};
using ServiceList = std::vector<Service>; // max: 8

struct VasService {
    uint16_t service_id;
    bool free_service;
};
using VasServiceList = std::vector<VasService>;

} // namespace datatypes

struct ServiceDiscoveryRequest {
    Header header;
    std::optional<datatypes::ServiceIdList> supported_service_ids;
};

struct ServiceDiscoveryResponse {

    Header header;
    datatypes::ResponseCode response_code;
    bool service_renegotiation_supported = false;
    datatypes::ServiceList energy_transfer_service_list = {{
        datatypes::ServiceCategory::AC, // service_id
        false                           // free_service
    }};
    std::optional<datatypes::VasServiceList> vas_list;
};

} // namespace iso15118::message_20
