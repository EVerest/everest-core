// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <ieee2030/common/messages/messages.hpp>

namespace ieee2030::charger::v20::state {

bool state_c_1(const messages::EV100&, const messages::EV101&, const messages::EV102&);
bool state_c_2(const messages::EV102&);

} // namespace ieee2030::charger::v20::state