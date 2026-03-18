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
    std::vector<dt::EnergyTransferMode> supported_energy_transfer_modes;
    std::vector<dt::PaymentOption> supported_payment_options;
    std::vector<dt::ServiceID> offered_services;
};

// This should only have EVSE information
struct SessionConfig {
    // TODO(kd) move the constructor to .cpp file
    explicit SessionConfig(EvseSetupConfig config) :
        evse_id(std::move(config.evse_id)),
        supported_energy_transfer_modes(std::move(config.supported_energy_transfer_modes)),
        supported_payment_options(std::move(config.supported_payment_options)),
        offered_services(std::move(config.offered_services)){};

    std::string evse_id;
    std::vector<dt::EnergyTransferMode> supported_energy_transfer_modes;
    std::vector<dt::PaymentOption> supported_payment_options;
    std::vector<dt::ServiceID> offered_services;
};

} // namespace iso15118::d2
