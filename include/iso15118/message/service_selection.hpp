// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct ServiceSelectionRequest {
    struct SelectedService {
        ServiceCategory service_id;
        int16_t parameter_set_id;
    };

    Header header;
    SelectedService selected_energy_transfer_service;
    std::optional<std::vector<SelectedService>> selected_vas_list;
};

struct ServiceSelectionResponse {
    Header header;
    ResponseCode response_code;
};

} // namespace iso15118::message_20
