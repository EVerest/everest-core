// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/network_config_sync.hpp>

#include <map>

#include <everest/logging.hpp>
#include <nlohmann/json.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_abstract.hpp>
#include <ocpp/v2/messages/SetNetworkProfile.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {
namespace network_config {

std::optional<NetworkConnectionProfile> read_profile_from_device_model(DeviceModelAbstract& dm, int32_t slot) {
    try {
        NetworkConnectionProfile profile;

        // Read required fields
        const auto url_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppCsmsUrl);
        const auto url_opt = dm.get_optional_value<std::string>(url_cv);
        if (!url_opt.has_value()) {
            return std::nullopt;
        }
        profile.ocppCsmsUrl = url_opt.value();

        const auto sec_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::SecurityProfile);
        const auto sec_opt = dm.get_optional_value<int>(sec_cv);
        if (!sec_opt.has_value()) {
            return std::nullopt;
        }
        profile.securityProfile = sec_opt.value();

        const auto iface_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppInterface);
        const auto iface_opt = dm.get_optional_value<std::string>(iface_cv);
        if (!iface_opt.has_value()) {
            return std::nullopt;
        }
        profile.ocppInterface = conversions::string_to_ocppinterface_enum(iface_opt.value());

        const auto trans_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppTransport);
        const auto trans_opt = dm.get_optional_value<std::string>(trans_cv);
        if (!trans_opt.has_value()) {
            return std::nullopt;
        }
        profile.ocppTransport = conversions::string_to_ocpptransport_enum(trans_opt.value());

        const auto timeout_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::MessageTimeout);
        const auto timeout_opt = dm.get_optional_value<int>(timeout_cv);
        if (!timeout_opt.has_value()) {
            return std::nullopt;
        }
        profile.messageTimeout = timeout_opt.value();

        // Read optional fields
        const auto identity_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::Identity);
        if (const auto identity_opt = dm.get_optional_value<std::string>(identity_cv)) {
            profile.identity = identity_opt.value();
        }

        const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::BasicAuthPassword);
        if (const auto pwd_opt = dm.get_optional_value<std::string>(pwd_cv)) {
            profile.basicAuthPassword = pwd_opt.value();
        }

        // Handle APN
        const auto apn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::ApnEnabled);
        const auto apn_enabled = dm.get_optional_value<bool>(apn_enabled_cv).value_or(false);

        if (apn_enabled) {
            APN apn;
            const auto apn_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::Apn);
            if (const auto apn_opt = dm.get_optional_value<std::string>(apn_cv)) {
                apn.apn = apn_opt.value();
            } else {
                EVLOG_warning << "APN enabled but APN value not set for slot " << slot;
                // APN is required if enabled, so fail
                return std::nullopt;
            }

            const auto auth_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::ApnAuthentication);
            if (const auto auth_opt = dm.get_optional_value<std::string>(auth_cv)) {
                apn.apnAuthentication = conversions::string_to_apnauthentication_enum(auth_opt.value());
            } else {
                apn.apnAuthentication = APNAuthenticationEnum::AUTO;
            }

            if (const auto user_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::ApnUserName);
                const auto user_opt = dm.get_optional_value<std::string>(user_cv)) {
                apn.apnUserName = user_opt.value();
            }

            if (const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::ApnPassword);
                const auto pwd_opt = dm.get_optional_value<std::string>(pwd_cv)) {
                apn.apnPassword = pwd_opt.value();
            }

            if (const auto pin_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::SimPin);
                const auto pin_opt = dm.get_optional_value<int>(pin_cv)) {
                apn.simPin = pin_opt.value();
            }

            if (const auto net_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::PreferredNetwork);
                const auto net_opt = dm.get_optional_value<std::string>(net_cv)) {
                apn.preferredNetwork = net_opt.value();
            }

            if (const auto only_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::UseOnlyPreferredNetwork);
                const auto only_opt = dm.get_optional_value<bool>(only_cv)) {
                apn.useOnlyPreferredNetwork = only_opt.value();
            }

            profile.apn = apn;
        }

        // Handle VPN
        const auto vpn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::VpnEnabled);
        const auto vpn_enabled = dm.get_optional_value<bool>(vpn_enabled_cv).value_or(false);

        if (vpn_enabled) {
            VPN vpn;
            const auto server_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnServer);
            if (const auto server_opt = dm.get_optional_value<std::string>(server_cv)) {
                vpn.server = server_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN server value not set for slot " << slot;
                return std::nullopt;
            }

            const auto user_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnUser);
            if (const auto user_opt = dm.get_optional_value<std::string>(user_cv)) {
                vpn.user = user_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN user value not set for slot " << slot;
                return std::nullopt;
            }

            const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnPassword);
            if (const auto pwd_opt = dm.get_optional_value<std::string>(pwd_cv)) {
                vpn.password = pwd_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN password value not set for slot " << slot;
                return std::nullopt;
            }

            const auto key_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnKey);
            if (const auto key_opt = dm.get_optional_value<std::string>(key_cv)) {
                vpn.key = key_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN key value not set for slot " << slot;
                return std::nullopt;
            }

            const auto type_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnType);
            if (const auto type_opt = dm.get_optional_value<std::string>(type_cv)) {
                vpn.type = conversions::string_to_vpnenum(type_opt.value());
            } else {
                EVLOG_warning << "VPN enabled but VPN type value not set for slot " << slot;
                return std::nullopt;
            }

            if (const auto group_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::VpnGroup);
                const auto group_opt = dm.get_optional_value<std::string>(group_cv)) {
                vpn.group = group_opt.value();
            }

            profile.vpn = vpn;
        }

        return profile;
    } catch (const std::exception& e) {
        EVLOG_error << "Error reading profile from device model for slot " << slot << ": " << e.what();
        return std::nullopt;
    }
}

bool write_profile_to_device_model(DeviceModelAbstract& dm, int32_t slot, const NetworkConnectionProfile& profile,
                                   const std::string& source) {
    try {
        // Write required fields
        const auto url_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppCsmsUrl);
        if (dm.set_value(url_cv.component, url_cv.variable.value(), AttributeEnum::Actual, profile.ocppCsmsUrl.get(),
                         source) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppCsmsUrl for slot " << slot;
            return false;
        }

        const auto sec_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::SecurityProfile);
        if (dm.set_value(sec_cv.component, sec_cv.variable.value(), AttributeEnum::Actual,
                         std::to_string(profile.securityProfile), source) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set SecurityProfile for slot " << slot;
            return false;
        }

        const auto iface_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppInterface);
        if (dm.set_value(iface_cv.component, iface_cv.variable.value(), AttributeEnum::Actual,
                         conversions::ocppinterface_enum_to_string(profile.ocppInterface),
                         source) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppInterface for slot " << slot;
            return false;
        }

        const auto trans_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::OcppTransport);
        if (dm.set_value(trans_cv.component, trans_cv.variable.value(), AttributeEnum::Actual,
                         conversions::ocpptransport_enum_to_string(profile.ocppTransport),
                         source) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppTransport for slot " << slot;
            return false;
        }

        const auto timeout_cv = NetworkConfigurationComponentVariables::get_component_variable(
            slot, NetworkConfigurationComponentVariables::MessageTimeout);
        if (dm.set_value(timeout_cv.component, timeout_cv.variable.value(), AttributeEnum::Actual,
                         std::to_string(profile.messageTimeout), source) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set MessageTimeout for slot " << slot;
            return false;
        }

        // Write optional fields
        if (profile.identity.has_value()) {
            const auto identity_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::Identity);
            if (dm.set_value(identity_cv.component, identity_cv.variable.value(), AttributeEnum::Actual,
                             profile.identity.value().get(), source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set Identity for slot " << slot;
            }
        }

        if (profile.basicAuthPassword.has_value()) {
            const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::BasicAuthPassword);
            if (dm.set_value(pwd_cv.component, pwd_cv.variable.value(), AttributeEnum::Actual,
                             profile.basicAuthPassword.value().get(), source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set BasicAuthPassword for slot " << slot;
            }
        }

        // Write APN
        if (profile.apn.has_value()) {
            const auto apn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::ApnEnabled);
            if (dm.set_value(apn_enabled_cv.component, apn_enabled_cv.variable.value(), AttributeEnum::Actual, "true",
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set ApnEnabled for slot " << slot;
            }

            const auto& apn = profile.apn.value();
            const auto apn_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::Apn);
            if (dm.set_value(apn_cv.component, apn_cv.variable.value(), AttributeEnum::Actual, apn.apn.get(), source) !=
                SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set Apn for slot " << slot;
            }

            const auto auth_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::ApnAuthentication);
            if (dm.set_value(auth_cv.component, auth_cv.variable.value(), AttributeEnum::Actual,
                             conversions::apnauthentication_enum_to_string(apn.apnAuthentication),
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set ApnAuthentication for slot " << slot;
            }

            if (apn.apnUserName.has_value()) {
                const auto user_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::ApnUserName);
                if (dm.set_value(user_cv.component, user_cv.variable.value(), AttributeEnum::Actual,
                                 apn.apnUserName.value().get(), source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set ApnUserName for slot " << slot;
                }
            }

            if (apn.apnPassword.has_value()) {
                const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::ApnPassword);
                if (dm.set_value(pwd_cv.component, pwd_cv.variable.value(), AttributeEnum::Actual,
                                 apn.apnPassword.value().get(), source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set ApnPassword for slot " << slot;
                }
            }

            if (apn.simPin.has_value()) {
                const auto pin_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::SimPin);
                if (dm.set_value(pin_cv.component, pin_cv.variable.value(), AttributeEnum::Actual,
                                 std::to_string(apn.simPin.value()), source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set SimPin for slot " << slot;
                }
            }

            if (apn.preferredNetwork.has_value()) {
                const auto net_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::PreferredNetwork);
                if (dm.set_value(net_cv.component, net_cv.variable.value(), AttributeEnum::Actual,
                                 apn.preferredNetwork.value().get(), source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set PreferredNetwork for slot " << slot;
                }
            }

            if (apn.useOnlyPreferredNetwork.has_value()) {
                const auto only_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::UseOnlyPreferredNetwork);
                if (dm.set_value(only_cv.component, only_cv.variable.value(), AttributeEnum::Actual,
                                 apn.useOnlyPreferredNetwork.value() ? "true" : "false",
                                 source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set UseOnlyPreferredNetwork for slot " << slot;
                }
            }
        } else {
            const auto apn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::ApnEnabled);
            if (dm.set_value(apn_enabled_cv.component, apn_enabled_cv.variable.value(), AttributeEnum::Actual, "false",
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set ApnEnabled=false for slot " << slot;
            }
        }

        // Write VPN
        if (profile.vpn.has_value()) {
            const auto vpn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnEnabled);
            if (dm.set_value(vpn_enabled_cv.component, vpn_enabled_cv.variable.value(), AttributeEnum::Actual, "true",
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnEnabled for slot " << slot;
            }

            const auto& vpn = profile.vpn.value();
            const auto server_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnServer);
            if (dm.set_value(server_cv.component, server_cv.variable.value(), AttributeEnum::Actual, vpn.server.get(),
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnServer for slot " << slot;
            }

            const auto user_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnUser);
            if (dm.set_value(user_cv.component, user_cv.variable.value(), AttributeEnum::Actual, vpn.user.get(),
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnUser for slot " << slot;
            }

            const auto pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnPassword);
            if (dm.set_value(pwd_cv.component, pwd_cv.variable.value(), AttributeEnum::Actual, vpn.password.get(),
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnPassword for slot " << slot;
            }

            const auto key_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnKey);
            if (dm.set_value(key_cv.component, key_cv.variable.value(), AttributeEnum::Actual, vpn.key.get(), source) !=
                SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnKey for slot " << slot;
            }

            const auto type_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnType);
            if (dm.set_value(type_cv.component, type_cv.variable.value(), AttributeEnum::Actual,
                             conversions::vpnenum_to_string(vpn.type), source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnType for slot " << slot;
            }

            if (vpn.group.has_value()) {
                const auto group_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::VpnGroup);
                if (dm.set_value(group_cv.component, group_cv.variable.value(), AttributeEnum::Actual,
                                 vpn.group.value().get(), source) != SetVariableStatusEnum::Accepted) {
                    EVLOG_warning << "Failed to set VpnGroup for slot " << slot;
                }
            }
        } else {
            const auto vpn_enabled_cv = NetworkConfigurationComponentVariables::get_component_variable(
                slot, NetworkConfigurationComponentVariables::VpnEnabled);
            if (dm.set_value(vpn_enabled_cv.component, vpn_enabled_cv.variable.value(), AttributeEnum::Actual, "false",
                             source) != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to set VpnEnabled=false for slot " << slot;
            }
        }

        return true;
    } catch (const std::exception& e) {
        EVLOG_error << "Error writing profile to device model for slot " << slot << ": " << e.what();
        return false;
    }
}

void sync_json_blob_from_device_model(DeviceModelAbstract& dm, const std::vector<int32_t>& slots) {
    try {
        // Read the existing blob to preserve slots not being updated
        std::map<int32_t, json> existing_by_slot;
        if (ControllerComponentVariables::NetworkConnectionProfiles.variable.has_value()) {
            try {
                const auto existing_str =
                    dm.get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles);
                for (const auto& entry : json::parse(existing_str)) {
                    const int32_t slot = entry.at("configurationSlot").get<int32_t>();
                    existing_by_slot[slot] = entry;
                }
            } catch (const std::exception& e) {
                EVLOG_warning << "Could not parse existing NetworkConnectionProfiles blob, rebuilding: " << e.what();
            }
        }

        // Update only the modified slots
        for (int32_t slot : slots) {
            if (const auto profile = read_profile_from_device_model(dm, slot)) {
                SetNetworkProfileRequest req;
                req.configurationSlot = slot;
                req.connectionData = profile.value();
                existing_by_slot[slot] = json::parse(nlohmann::json(req).dump());
            }
        }

        // Rebuild the full array from the merged map
        json profiles = json::array();
        for (const auto& [slot, entry] : existing_by_slot) {
            profiles.push_back(entry);
        }

        if (ControllerComponentVariables::NetworkConnectionProfiles.variable.has_value()) {
            if (dm.set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                             ControllerComponentVariables::NetworkConnectionProfiles.variable.value(),
                             AttributeEnum::Actual, profiles.dump(), "internal") != SetVariableStatusEnum::Accepted) {
                EVLOG_warning << "Failed to sync NetworkConnectionProfiles JSON blob";
            }
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error syncing JSON blob from device model: " << e.what();
    }
}

void seed_device_model_from_json_blob(DeviceModelAbstract& dm) {
    try {
        const auto json_blob = dm.get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles);
        const auto profiles = json::parse(json_blob);

        for (const auto& profile_json : profiles) {
            SetNetworkProfileRequest req = profile_json;
            if (write_profile_to_device_model(dm, req.configurationSlot, req.connectionData, "internal")) {
                EVLOG_debug << "Seeded NetworkConfiguration[" << req.configurationSlot << "] from JSON blob";
            } else {
                EVLOG_warning << "Failed to seed NetworkConfiguration[" << req.configurationSlot << "]";
            }
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error seeding device model from JSON blob: " << e.what();
    }
}

} // namespace network_config
} // namespace v2
} // namespace ocpp
