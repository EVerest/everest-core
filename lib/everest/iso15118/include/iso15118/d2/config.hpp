// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>
#include <utility>
#include <vector>

#include <iso15118/message/d2/msg_data_types.hpp>

namespace iso15118::d2 {

namespace dt = msg::data_types;

struct EvseSetupConfig {
    std::string evse_id;
    dt::ChargeService charge_service;
    std::vector<dt::PaymentOption> supported_payment_options;
    dt::ServiceList offered_services;
};

// This should only have EVSE information
struct SessionConfig {
    // TODO(kd) move the constructor to .cpp file
    explicit SessionConfig(EvseSetupConfig config) :
        evse_id(std::move(config.evse_id)),
        charge_service(std::move(config.charge_service)),
        supported_payment_options(std::move(config.supported_payment_options)),
        offered_services(config.offered_services){};

    std::string evse_id;
    dt::ChargeService charge_service;
    std::vector<dt::PaymentOption> supported_payment_options;
    dt::ServiceList offered_services;
};

} // namespace iso15118::d2
