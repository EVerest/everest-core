// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct ServiceDiscoveryRequest {
    Header header;
    std::optional<std::vector<uint16_t>> supported_service_ids;
};

struct ServiceDiscoveryResponse {
    struct Service {
        ServiceCategory service_id;
        bool free_service;
    };

    Header header;
    ResponseCode response_code;
    bool service_renegotiation_supported = false;
    std::vector<Service> energy_transfer_service_list = {{
        ServiceCategory::AC, // service_id
        false                // free_service
    }};
    std::optional<std::vector<Service>> vas_list;
};

} // namespace iso15118::message_20
