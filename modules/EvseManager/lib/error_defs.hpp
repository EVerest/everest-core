// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <initializer_list>

// FIXME (aw): everest-framework imports are not properly namespaced
#include <utils/error.hpp>

namespace module {

using ErrorList = std::initializer_list<Everest::error::ErrorType>;

namespace errors_to_ignore {
// FIXME (aw): these strings should be strongly typed (propably via autogeneration) instead of repeatedly hardcoded
// p_evse. We need to ignore Inoperative here as this is the result of this check.
inline const ErrorList evse{"evse_manager/Inoperative"};
inline const ErrorList bsp{"evse_board_support/MREC3HighTemperature", "evse_board_support/MREC18CableOverTempDerate",
                           "evse_board_support/VendorWarning"};
inline const ErrorList connector_lock{"connector_lock/VendorWarning"};
inline const ErrorList ac_rcd{"ac_rcd/VendorWarning"};
inline const ErrorList imd{"isolation_monitor/VendorWarning"};
inline const ErrorList powersupply{"power_supply_DC/VendorWarning"};

} // namespace errors_to_ignore

} // namespace module