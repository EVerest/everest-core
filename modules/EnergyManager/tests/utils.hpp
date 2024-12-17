// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <fstream>

#include <generated/types/energy.hpp>

using namespace types::energy;

static EnergyFlowRequest get_energy_flow_request_from_file(const std::string& path) {
    std::ifstream f(path.c_str());
    json data = json::parse(f);

    EnergyFlowRequest e;
    from_json(data, e);
    return e;
}