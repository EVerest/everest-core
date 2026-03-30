// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/connectivity_manager.hpp>

#include <fstream>

#include <everest/logging.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/utils.hpp>

namespace {
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL = "internal";
/// \brief Default timeout for the return value (future) of the `configure_network_connection_profile_callback`
///        function.
constexpr std::int32_t default_network_config_timeout_seconds = 60;
} // namespace

namespace ocpp {
namespace v2 {

using namespace std::chrono_literals;

ConnectivityManager::ConnectivityManager(DeviceModelAbstract& device_model, std::shared_ptr<EvseSecurity> evse_security,
                                         std::shared_ptr<MessageLogging> logging, const fs::path& share_path,
                                         const std::function<void(const std::string& message)>& message_callback) :
    device_model{device_model},
    evse_security{evse_security},
    logging{logging},
    share_path{share_path},
    websocket{nullptr},
    message_callback{message_callback},
    wants_to_be_connected{false},
    active_network_configuration_priority{0},
    last_known_security_level{0},
    connected_ocpp_version{OcppProtocolVersion::Unknown} {
    cache_network_connection_profiles();
}

void ConnectivityManager::set_websocket_authorization_key(const std::string& authorization_key) {
    if (this->websocket != nullptr) {
        this->websocket->set_authorization_key(authorization_key);
        this->websocket->disconnect(WebsocketCloseReason::ServiceRestart);
    }
}

void ConnectivityManager::set_websocket_connection_options(const WebsocketConnectionOptions& connection_options) {
    if (this->websocket != nullptr) {
        this->websocket->set_connection_options(connection_options);
    }
}

void ConnectivityManager::set_websocket_connection_options_without_reconnect() {
    const int configuration_slot = get_active_network_configuration_slot();
    const auto connection_options = this->get_ws_connection_options(configuration_slot);
    if (connection_options.has_value()) {
        this->set_websocket_connection_options(connection_options.value());
    }
}

void ConnectivityManager::set_websocket_connected_callback(WebsocketConnectionCallback callback) {
    this->websocket_connected_callback = callback;
}

void ConnectivityManager::set_websocket_disconnected_callback(WebsocketConnectionCallback callback) {
    this->websocket_disconnected_callback = callback;
}

void ConnectivityManager::set_websocket_connection_failed_callback(WebsocketConnectionFailedCallback callback) {
    this->websocket_connection_failed_callback = callback;
}

void ConnectivityManager::set_configure_network_connection_profile_callback(
    ConfigureNetworkConnectionProfileCallback callback) {
    this->configure_network_connection_profile_callback = callback;
}

std::optional<NetworkConnectionProfile>
ConnectivityManager::get_network_connection_profile(const std::int32_t configuration_slot) const {

    for (const auto& network_profile : this->cached_network_connection_profiles) {
        if (network_profile.configurationSlot == configuration_slot) {
            if (!this->device_model
                     .get_optional_value<bool>(ControllerComponentVariables::AllowSecurityLevelZeroConnections)
                     .value_or(false) &&
                network_profile.connectionData.securityProfile ==
                    security::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION) {
                EVLOG_error << "security_profile 0 not officially allowed in OCPP 2.0.1, skipping profile";
                return std::nullopt;
            }

            return network_profile.connectionData;
        }
    }
    return std::nullopt;
}

std::optional<std::int32_t>
ConnectivityManager::get_priority_from_configuration_slot(const int configuration_slot) const {
    auto it =
        std::find(this->network_connection_slots.begin(), this->network_connection_slots.end(), configuration_slot);
    if (it != this->network_connection_slots.end()) {
        // Index is iterator - begin iterator
        return it - this->network_connection_slots.begin();
    }
    return std::nullopt;
}

int ConnectivityManager::get_active_network_configuration_slot() const {
    return this->network_connection_slots.at(this->active_network_configuration_priority);
}

int ConnectivityManager::get_configuration_slot_from_priority(const int priority) {
    return this->network_connection_slots.at(priority);
}

const std::vector<int>& ConnectivityManager::get_network_connection_slots() const {
    return this->network_connection_slots;
}

bool ConnectivityManager::is_websocket_connected() {
    return this->websocket != nullptr && this->websocket->is_connected();
}

std::chrono::time_point<std::chrono::steady_clock> ConnectivityManager::get_time_disconnected() const {
    return this->time_disconnected;
}

void ConnectivityManager::connect(std::optional<std::int32_t> network_profile_slot) {
    if (this->network_connection_slots.empty()) {
        EVLOG_warning << "No network connection profiles configured, aborting websocket connection.";
        return;
    }

    const std::int32_t configuration_slot =
        network_profile_slot.value_or(this->get_active_network_configuration_slot());
    if (!this->get_network_connection_profile(configuration_slot).has_value()) {
        EVLOG_warning << "Could not find network connection profile belonging to configuration slot "
                      << configuration_slot;
        return;
    }

    this->wants_to_be_connected = true;
    this->pending_configuration_slot = configuration_slot;
    if (this->websocket != nullptr && this->is_websocket_connected()) {
        // After the websocket gets closed a reconnect will be triggered
        this->websocket->disconnect(WebsocketCloseReason::ServiceRestart);
    } else {
        this->try_connect_websocket();
    }
}

void ConnectivityManager::disconnect() {
    this->wants_to_be_connected = false;
    this->websocket_timer.stop();
    if (this->websocket != nullptr) {
        this->websocket->disconnect(WebsocketCloseReason::Normal);
    }
}

void ConnectivityManager::confirm_successful_connection() {
    const int config_slot_int = this->get_active_network_configuration_slot();

    const auto network_connection_profile = this->get_network_connection_profile(config_slot_int);

    if (const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
        security_profile_cv.variable.has_value() and network_connection_profile.has_value()) {
        this->device_model.set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                               AttributeEnum::Actual,
                                               std::to_string(network_connection_profile.value().securityProfile),
                                               VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    this->remove_network_connection_profiles_below_actual_security_profile();
    this->check_cache_for_invalid_security_profiles();
}

void ConnectivityManager::try_connect_websocket() {
    if (this->device_model.get_value<std::string>(ControllerComponentVariables::ChargePointId).find(':') !=
        std::string::npos) {
        EVLOG_AND_THROW(std::runtime_error("ChargePointId must not contain \':\'"));
    }

    // Check the cache runtime since security profile might change async
    this->check_cache_for_invalid_security_profiles();

    const int configuration_slot_to_set =
        this->pending_configuration_slot.value_or(this->get_active_network_configuration_slot());
    const std::optional<int> priority_to_set = this->get_priority_from_configuration_slot(configuration_slot_to_set);
    const auto network_connection_profile = this->get_network_connection_profile(configuration_slot_to_set);
    // Not const as the iface member can be set by the configure network connection profile callback
    auto connection_options = this->get_ws_connection_options(configuration_slot_to_set);
    bool can_use_connection_profile = true;

    if (!network_connection_profile.has_value() || !priority_to_set.has_value()) {
        EVLOG_warning << "No network connection profile configured for " << configuration_slot_to_set;
        can_use_connection_profile = false;
    } else if (!connection_options.has_value()) {
        EVLOG_warning << "Connection profile configured for " << configuration_slot_to_set << " failed: not valid URL";
        can_use_connection_profile = false;
    } else if (this->configure_network_connection_profile_callback.has_value()) {
        std::optional<ConfigNetworkResult> config = handle_configure_network_connection_profile_callback(
            configuration_slot_to_set, network_connection_profile.value());
        if (config.has_value() && config->success) {
            connection_options->iface = config->interface_address;
        } else {
            EVLOG_debug << "Could not use config slot " << configuration_slot_to_set;
            can_use_connection_profile = false;
        }
    }

    if (!can_use_connection_profile) {
        if (this->wants_to_be_connected) {
            this->websocket_timer.timeout(
                [this, configuration_slot_to_set] {
                    this->pending_configuration_slot = get_next_configuration_slot(configuration_slot_to_set);
                    this->try_connect_websocket();
                },
                WEBSOCKET_INIT_DELAY);
        }
        return;
    }

    this->pending_configuration_slot.reset();
    this->active_network_configuration_priority = priority_to_set.value();

    if (connection_options->security_profile ==
        security::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION) {
        EVLOG_warning << "Using insecure security profile 0 without authentication";
    }

    EVLOG_info << "Open websocket with NetworkConfigurationPriority: "
               << this->active_network_configuration_priority + 1 << " which is configurationSlot "
               << configuration_slot_to_set;

    if (const auto& active_network_profile_cv = ControllerComponentVariables::ActiveNetworkProfile;
        active_network_profile_cv.variable.has_value()) {
        this->device_model.set_read_only_value(
            active_network_profile_cv.component, active_network_profile_cv.variable.value(), AttributeEnum::Actual,
            std::to_string(configuration_slot_to_set), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    if (this->websocket == nullptr) {
        this->websocket = std::make_unique<Websocket>(connection_options.value(), this->evse_security, this->logging);

        this->websocket->register_connected_callback(
            [this](OcppProtocolVersion protocol) { this->on_websocket_connected(protocol); });
        this->websocket->register_disconnected_callback([this]() { this->on_websocket_disconnected(); });
        this->websocket->register_stopped_connecting_callback(
            [this](ocpp::WebsocketCloseReason reason) { this->on_websocket_stopped_connecting(reason); });
    } else {
        this->websocket->set_connection_options(connection_options.value());
    }

    // Attach external callbacks everytime since they might have changed
    if (websocket_connection_failed_callback.has_value()) {
        this->websocket->register_connection_failed_callback(websocket_connection_failed_callback.value());
    }

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });

    this->websocket->start_connecting();
}

std::optional<ConfigNetworkResult>
ConnectivityManager::handle_configure_network_connection_profile_callback(int slot,
                                                                          const NetworkConnectionProfile& profile) {
    if (!this->configure_network_connection_profile_callback.has_value()) {
        return std::nullopt;
    }

    std::future<ConfigNetworkResult> config_status =
        this->configure_network_connection_profile_callback.value()(slot, profile);
    const std::int32_t config_timeout =
        this->device_model.get_optional_value<int>(ControllerComponentVariables::NetworkConfigTimeout)
            .value_or(default_network_config_timeout_seconds);

    if (config_status.wait_for(std::chrono::seconds(config_timeout)) == std::future_status::ready) {
        return config_status.get();
    }

    EVLOG_warning << "Timeout configuring config slot: " << slot;
    return std::nullopt;
}

int ConnectivityManager::get_next_configuration_slot(std::int32_t configuration_slot) {

    if (this->network_connection_slots.size() > 1) {
        EVLOG_info << "Switching to next network configuration priority";
    }
    const auto network_configuration_priority_opt = get_priority_from_configuration_slot(configuration_slot);

    const int network_configuration_priority =
        network_configuration_priority_opt.has_value()
            ? (network_configuration_priority_opt.value() + 1) % (clamp_to<int>(this->network_connection_slots.size()))
            : 0;

    return get_configuration_slot_from_priority(network_configuration_priority);
}

bool ConnectivityManager::send_to_websocket(const std::string& message) {
    if (this->websocket == nullptr) {
        return false;
    }

    return this->websocket->send(message);
}

void ConnectivityManager::on_network_disconnected(OCPPInterfaceEnum ocpp_interface) {

    const int actual_configuration_slot = get_active_network_configuration_slot();
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(actual_configuration_slot);

    if (!network_connection_profile.has_value()) {
        EVLOG_warning << "Network disconnected. No network connection profile configured";
    } else if (ocpp_interface == network_connection_profile.value().ocppInterface && this->websocket != nullptr) {
        // Since there is no connection anymore: disconnect the websocket, the manager will try to connect with the next
        // available network connection profile as we enable reconnects.
        this->websocket->disconnect(ocpp::WebsocketCloseReason::GoingAway);
    }
}

void ConnectivityManager::on_charging_station_certificate_changed() {
    if (this->websocket != nullptr) {
        // After the websocket gets closed a reconnect will be triggered
        this->websocket->disconnect(WebsocketCloseReason::ServiceRestart);
    }
}

std::optional<WebsocketConnectionOptions>
ConnectivityManager::get_ws_connection_options(const std::int32_t configuration_slot) {
    const auto network_connection_profile_opt = this->get_network_connection_profile(configuration_slot);

    if (!network_connection_profile_opt.has_value()) {
        EVLOG_critical << "Could not retrieve NetworkProfile of configurationSlot: " << configuration_slot;
        throw std::runtime_error("Could not retrieve NetworkProfile");
    }

    const auto& network_connection_profile = network_connection_profile_opt.value();

    try {
        // B09.FR.16-18: Check per-slot Identity override first, fall back to SecurityCtrlr.Identity
        std::string identity_to_use =
            this->device_model.get_value<std::string>(ControllerComponentVariables::SecurityCtrlrIdentity);

        const auto slot_identity_cv = NetworkConfigurationComponentVariables::get_component_variable(
            configuration_slot, NetworkConfigurationComponentVariables::Identity);
        if (const auto slot_identity = this->device_model.get_optional_value<std::string>(slot_identity_cv);
            slot_identity.has_value() && !slot_identity->empty()) {
            identity_to_use = *slot_identity;
            EVLOG_debug << "Using per-slot Identity for slot " << configuration_slot;
        }

        auto uri = Uri::parse_and_validate(network_connection_profile.ocppCsmsUrl.get(), identity_to_use,
                                           network_connection_profile.securityProfile);

        const auto ocpp_versions = utils::get_ocpp_protocol_versions(
            this->device_model.get_value<std::string>(ControllerComponentVariables::SupportedOcppVersions));

        // B09.FR.26-28: Check per-slot BasicAuthPassword override first
        std::optional<std::string> basic_auth_password;
        const auto slot_pwd_cv = NetworkConfigurationComponentVariables::get_component_variable(
            configuration_slot, NetworkConfigurationComponentVariables::BasicAuthPassword);
        if (const auto slot_pwd = this->device_model.get_optional_value<std::string>(slot_pwd_cv);
            slot_pwd.has_value() && !slot_pwd->empty()) {
            basic_auth_password = slot_pwd.value();
            EVLOG_debug << "Using per-slot BasicAuthPassword for slot " << configuration_slot;
        } else {
            basic_auth_password =
                this->device_model.get_optional_value<std::string>(ControllerComponentVariables::BasicAuthPassword);
        }

        WebsocketConnectionOptions connection_options{
            ocpp_versions, uri, network_connection_profile.securityProfile, basic_auth_password,
            // Always use a minimum of 1 second otherwise each message would timeout immediately
            std::chrono::seconds(std::max(network_connection_profile.messageTimeout, 1)),
            this->device_model.get_value<int>(ControllerComponentVariables::RetryBackOffRandomRange),
            this->device_model.get_value<int>(ControllerComponentVariables::RetryBackOffRepeatTimes),
            this->device_model.get_value<int>(ControllerComponentVariables::RetryBackOffWaitMinimum),
            this->device_model.get_value<int>(ControllerComponentVariables::NetworkProfileConnectionAttempts),
            this->device_model.get_value<std::string>(ControllerComponentVariables::SupportedCiphers12),
            this->device_model.get_value<std::string>(ControllerComponentVariables::SupportedCiphers13),
            this->device_model.get_value<int>(ControllerComponentVariables::WebSocketPingInterval),
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::WebsocketPingPayload)
                .value_or("payload"),
            this->device_model.get_optional_value<int>(ControllerComponentVariables::WebsocketPongTimeout).value_or(5),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::UseSslDefaultVerifyPaths)
                .value_or(true),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::AdditionalRootCertificateCheck)
                .value_or(false),
            std::nullopt, // hostName
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsCommonName)
                .value_or(true),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::UseTPM).value_or(false),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsAllowWildcards)
                .value_or(false),
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::IFace),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::EnableTLSKeylog).value_or(false),
            this->device_model.get_optional_value<std::string>(ControllerComponentVariables::TLSKeylogFile)};

        // Read version file and add to connection_options
        fs::path version_file_path = this->share_path.parent_path().parent_path() / "version_information.txt";
        if (fs::exists(version_file_path)) {
            std::ifstream ifs(version_file_path);
            std::string version;
            std::getline(ifs, version);                               // only get one line to avoid issues
            std::string trimmed_version = ocpp::trim_string(version); // remove leading/trailing whitespace
            trimmed_version.erase(std::remove(trimmed_version.begin(), trimmed_version.end(), '\n'),
                                  trimmed_version.end()); // remove unnecessary newline characters
            if (!trimmed_version.empty()) {
                connection_options.everest_version = trimmed_version;
            }
        }

        return connection_options;

    } catch (const std::invalid_argument& e) {
        EVLOG_error << "Could not configure the connection options: " << e.what();
    }

    return std::nullopt;
}

void ConnectivityManager::on_websocket_connected(OcppProtocolVersion protocol) {
    this->connected_ocpp_version = protocol;
    const int actual_configuration_slot = get_active_network_configuration_slot();
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(actual_configuration_slot);

    // Write negotiated OcppVersion to the per-slot NetworkConfiguration DM variable (B09)
    if (protocol != OcppProtocolVersion::Unknown) {
        std::string version_str;
        switch (protocol) {
        case OcppProtocolVersion::v201:
            version_str = "OCPP201";
            break;
        case OcppProtocolVersion::v21:
            version_str = "OCPP21";
            break;
        case OcppProtocolVersion::v16:
        case OcppProtocolVersion::Unknown:
            break;
        }
        if (!version_str.empty()) {
            const auto nc_cv = NetworkConfigurationComponentVariables::get_component_variable(
                actual_configuration_slot, NetworkConfigurationComponentVariables::OcppVersion);
            if (nc_cv.variable.has_value()) {
                this->device_model.set_read_only_value(nc_cv.component, nc_cv.variable.value(), AttributeEnum::Actual,
                                                       version_str, VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
            }
        }
    }

    if (this->websocket_connected_callback.has_value() and network_connection_profile.has_value()) {
        this->websocket_connected_callback.value()(actual_configuration_slot, network_connection_profile.value(),
                                                   this->connected_ocpp_version);
    }
    this->time_disconnected = std::chrono::time_point<std::chrono::steady_clock>();
}

void ConnectivityManager::on_websocket_disconnected() {
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(this->get_active_network_configuration_slot());
    if (this->time_disconnected.load().time_since_epoch() == 0s) {
        this->time_disconnected = std::chrono::steady_clock::now();
    }

    if (this->websocket_disconnected_callback.has_value() and network_connection_profile.has_value()) {
        this->websocket_disconnected_callback.value()(this->get_active_network_configuration_slot(),
                                                      network_connection_profile.value(), this->connected_ocpp_version);
    }
}

void ConnectivityManager::on_websocket_stopped_connecting(ocpp::WebsocketCloseReason reason) {
    EVLOG_warning << "Closed websocket of NetworkConfigurationPriority: "
                  << this->active_network_configuration_priority + 1 << " which is configurationSlot "
                  << this->get_active_network_configuration_slot();

    if (this->wants_to_be_connected) {
        this->websocket_timer.timeout(
            [this, reason] {
                if (reason != WebsocketCloseReason::ServiceRestart) {
                    this->pending_configuration_slot =
                        get_next_configuration_slot(get_active_network_configuration_slot());
                }
                this->try_connect_websocket();
            },
            WEBSOCKET_INIT_DELAY);
    }
}

void ConnectivityManager::append_slot_to_network_configuration_priority_if_absent(const int32_t slot,
                                                                                  const std::string& source) {
    if (!ControllerComponentVariables::NetworkConfigurationPriority.variable.has_value()) {
        return;
    }

    const auto priority_str =
        this->device_model.get_optional_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority);
    const auto slot_str = std::to_string(slot);
    bool slot_found = false;
    if (priority_str.has_value()) {
        for (const auto& s : ocpp::split_string(priority_str.value(), ',')) {
            if (s == slot_str) {
                slot_found = true;
                break;
            }
        }
    }
    if (!slot_found) {
        std::string new_priority = priority_str.value_or("");
        if (!new_priority.empty()) {
            new_priority += ',';
        }
        new_priority += slot_str;
        this->device_model.set_value(ControllerComponentVariables::NetworkConfigurationPriority.component,
                                     ControllerComponentVariables::NetworkConfigurationPriority.variable.value(),
                                     AttributeEnum::Actual, new_priority, source);
    }
}

void ConnectivityManager::cache_network_connection_profiles() {
    this->cached_network_connection_profiles.clear();
    this->network_connection_slots.clear();

    // Build profiles and priority-ordered slot list from NetworkConfiguration DM components
    for (const std::string& str : ocpp::split_string(
             this->device_model.get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority),
             ',')) {
        const int slot = std::stoi(str);
        this->network_connection_slots.push_back(slot);
        if (const auto profile =
                NetworkConfigurationComponentVariables::read_profile_from_device_model(this->device_model, slot)) {
            SetNetworkProfileRequest req;
            req.configurationSlot = slot;
            req.connectionData = *profile;
            this->cached_network_connection_profiles.push_back(req);
        }
    }

    if (!this->cached_network_connection_profiles.empty() &&
        !this->device_model.get_optional_value<bool>(ControllerComponentVariables::AllowSecurityLevelZeroConnections)
             .value_or(false) &&
        std::none_of(this->cached_network_connection_profiles.begin(), this->cached_network_connection_profiles.end(),
                     [](const SetNetworkProfileRequest& profile) {
                         return profile.connectionData.securityProfile !=
                                security::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION;
                     })) {
        EVLOG_error << "All profiles configured have security_profile 0 which is not officially allowed in OCPP 2.0.1";
    }
}

void ConnectivityManager::reload_network_profiles() {
    try {
        cache_network_connection_profiles();
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to reload network profiles: " << e.what();
    }
}

bool ConnectivityManager::set_network_profile(const int32_t slot, const NetworkConnectionProfile& profile,
                                              const std::string& source) {
    NetworkConnectionProfile merged_profile = profile;
    const auto active_slot = get_active_network_configuration_slot();
    const auto active_profile =
        NetworkConfigurationComponentVariables::read_profile_from_device_model(this->device_model, active_slot);
    if (active_profile.has_value()) {
        if (!merged_profile.basicAuthPassword.has_value() && active_profile->basicAuthPassword.has_value()) {
            merged_profile.basicAuthPassword = active_profile->basicAuthPassword;
            EVLOG_info << "Populating BasicAuthPassword from active slot " << active_slot;
        }
        if (!merged_profile.identity.has_value() && active_profile->identity.has_value()) {
            merged_profile.identity = active_profile->identity;
            EVLOG_info << "Populating Identity from active slot " << active_slot;
        }
    }

    if (!NetworkConfigurationComponentVariables::write_profile_to_device_model(this->device_model, slot, merged_profile,
                                                                               source)) {
        return false;
    }

    this->append_slot_to_network_configuration_priority_if_absent(slot, source);

    try {
        cache_network_connection_profiles();
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to refresh network profiles after set_network_profile: " << e.what();
    }
    return true;
}

void ConnectivityManager::check_cache_for_invalid_security_profiles() {
    const auto security_level = this->device_model.get_value<int>(ControllerComponentVariables::SecurityProfile);

    if (this->last_known_security_level == security_level) {
        return;
    }
    this->last_known_security_level = security_level;

    // Use active slot
    auto before_slot = this->pending_configuration_slot.value_or(this->get_active_network_configuration_slot());

    auto is_lower_security_level = [this, security_level](const int slot) {
        const auto opt_profile = this->get_network_connection_profile(slot);
        return !opt_profile.has_value() || opt_profile->securityProfile < security_level;
    };

    this->network_connection_slots.erase(std::remove_if(this->network_connection_slots.begin(),
                                                        this->network_connection_slots.end(), is_lower_security_level),
                                         this->network_connection_slots.end());

    if (this->network_connection_slots.empty()) {
        EVLOG_warning << "All network connection slots were removed due to insufficient security profile";
        this->pending_configuration_slot.reset();
        return;
    }

    // Use the active slot and if not valid any longer use the next available one
    auto opt_priority = this->get_priority_from_configuration_slot(before_slot);
    if (opt_priority) {
        this->pending_configuration_slot = before_slot;
    } else {
        this->pending_configuration_slot = this->get_next_configuration_slot(before_slot);
    }
}

void ConnectivityManager::remove_network_connection_profiles_below_actual_security_profile() {
    const auto security_level = this->device_model.get_value<int>(ControllerComponentVariables::SecurityProfile);

    // Collect slots to prune from the in-memory cache
    std::vector<int32_t> pruned_slots;
    for (const auto& profile : this->cached_network_connection_profiles) {
        if (profile.connectionData.securityProfile < security_level) {
            pruned_slots.push_back(profile.configurationSlot);
        }
    }

    if (pruned_slots.empty()) {
        return;
    }

    // Clear per-slot DM variables to prevent re-injection via SetVariables
    for (const int32_t slot : pruned_slots) {
        NetworkConfigurationComponentVariables::clear_slot_in_device_model(this->device_model, slot);
    }

    // Remove pruned slots from in-memory caches
    auto is_pruned = [&pruned_slots](int32_t slot) {
        return std::find(pruned_slots.begin(), pruned_slots.end(), slot) != pruned_slots.end();
    };

    this->cached_network_connection_profiles.erase(
        std::remove_if(this->cached_network_connection_profiles.begin(), this->cached_network_connection_profiles.end(),
                       [&is_pruned](const SetNetworkProfileRequest& p) { return is_pruned(p.configurationSlot); }),
        this->cached_network_connection_profiles.end());

    this->network_connection_slots.erase(
        std::remove_if(this->network_connection_slots.begin(), this->network_connection_slots.end(), is_pruned),
        this->network_connection_slots.end());

    // Rebuild and persist NetworkConfigurationPriority from remaining slots
    if (not ControllerComponentVariables::NetworkConfigurationPriority.variable.has_value()) {
        EVLOG_warning << "NetworkConfigurationPriority variable is not set, cannot update network priority";
        return;
    }

    std::string new_priority;
    for (const int32_t slot : this->network_connection_slots) {
        if (!new_priority.empty()) {
            new_priority += ',';
        }
        new_priority += std::to_string(slot);
    }

    this->device_model.set_value(ControllerComponentVariables::NetworkConfigurationPriority.component,
                                 ControllerComponentVariables::NetworkConfigurationPriority.variable.value(),
                                 AttributeEnum::Actual, new_priority, VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
}

} // namespace v2
} // namespace ocpp
