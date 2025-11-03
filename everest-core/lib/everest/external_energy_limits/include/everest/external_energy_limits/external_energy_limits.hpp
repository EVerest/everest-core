// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <generated/interfaces/external_energy_limits/Interface.hpp>

namespace external_energy_limits {

/// \brief Checks if \p r_evse_energy_sink vector contains an element that has a mapping to the given \p evse_id
/// \param r_evse_energy_sink
/// \param evse_id
/// \return
bool is_evse_sink_configured(const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink,
                             const int32_t evse_id);

/// \brief Returns the reference of external_energy_limitsIntf in \p r_evse_energy_sink that maps to the given \p
/// evse_id \param r_evse_energy_sink \param evse_id \return
external_energy_limitsIntf&
get_evse_sink_by_evse_id(const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink,
                         const int32_t evse_id);

} // namespace external_energy_limits
