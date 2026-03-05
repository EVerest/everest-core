// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <vector>

#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelAbstract;

namespace network_config {

/// \brief Read a profile from device model variables for a given slot
/// \param dm Device model instance
/// \param slot Configuration slot number
/// \return Optional NetworkConnectionProfile if all required fields are present
std::optional<NetworkConnectionProfile> read_profile_from_device_model(DeviceModelAbstract& dm, int32_t slot);

/// \brief Write a profile to device model variables for a given slot
/// \param dm Device model instance
/// \param slot Configuration slot number
/// \param profile Profile to write
/// \param source Source of the change (e.g., "internal", "csms")
/// \return true if successful, false otherwise
bool write_profile_to_device_model(DeviceModelAbstract& dm, int32_t slot, const NetworkConnectionProfile& profile,
                                   const std::string& source);

/// \brief Rebuild InternalCtrlr.NetworkConnectionProfiles JSON from all NetworkConfiguration instances
/// \param dm Device model instance
/// \param slots List of configuration slots to sync
void sync_json_blob_from_device_model(DeviceModelAbstract& dm, const std::vector<int32_t>& slots);

/// \brief Populate NetworkConfiguration instances from existing JSON blob (migration on first boot)
/// \param dm Device model instance
void seed_device_model_from_json_blob(DeviceModelAbstract& dm);

} // namespace network_config
} // namespace v2
} // namespace ocpp
