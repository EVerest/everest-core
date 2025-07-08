// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

struct SelectedService {
    ServiceCategory service_id;
    uint16_t parameter_set_id;
};

struct VasSelectedService {
    uint16_t service_id;
    uint16_t parameter_set_id;
};

using VasSelectedServiceList = std::vector<VasSelectedService>; // Max: 16

} // namespace datatypes

struct ServiceSelectionRequest {
    Header header;
    datatypes::SelectedService selected_energy_transfer_service;
    std::optional<datatypes::VasSelectedServiceList> selected_vas_list;
};

struct ServiceSelectionResponse {
    Header header;
    datatypes::ResponseCode response_code;
};

} // namespace iso15118::message_20
