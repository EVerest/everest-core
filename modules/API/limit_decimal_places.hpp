// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef LIMIT_DECIMAL_PLACES_HPP
#define LIMIT_DECIMAL_PLACES_HPP

#include <generated/interfaces/evse_manager/Interface.hpp>

#include "API.hpp"

namespace module {

struct Conf;

class LimitDecimalPlaces {
public:
    LimitDecimalPlaces(const Conf& config) : config(config){};
    std::string limit(const types::powermeter::Powermeter& powermeter);
    std::string limit(const types::evse_board_support::HardwareCapabilities& hw_capabilities);
    std::string limit(const types::evse_manager::Limits& limits);
    std::string limit(const types::evse_board_support::Telemetry& telemetry);
    double round_to_nearest_step(double value, double step);

private:
    const Conf& config;
};

} // namespace module

#endif // LIMIT_DECIMAL_PLACES_HPP
