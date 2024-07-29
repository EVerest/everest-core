// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Broker.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

Broker::Broker(Market& _market) :
    local_market(_market),
    first_trade(globals.schedule_length, true),
    slot_type(globals.schedule_length, SlotType::Undecided),
    num_phases(globals.schedule_length, 0) {
}

Market& Broker::get_local_market() {
    return local_market;
}

} // namespace module
