// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"

namespace module {
namespace energy_source_and_sink {

void energyImpl::init() {
}

void energyImpl::ready() {
}

void energyImpl::handle_enforce_limits(std::string& uuid, types::energy::Limits& limits_import,
                                       types::energy::Limits& limits_export,
                                       std::vector<types::energy::TimeSeriesEntry>& schedule_import,
                                       std::vector<types::energy::TimeSeriesEntry>& schedule_export){
    // your code for cmd enforce_limits goes here
};

} // namespace energy_source_and_sink
} // namespace module
