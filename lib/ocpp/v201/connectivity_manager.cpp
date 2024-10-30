// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/connectivity_manager.hpp>

#include <everest/logging.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>

namespace {
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL = "internal";
/// \brief Default timeout for the return value (future) of the `configure_network_connection_profile_callback`
///        function.
constexpr int32_t default_network_config_timeout_seconds = 60;
} // namespace

namespace ocpp {
namespace v201 {

ConnectivityManager::ConnectivityManager(DeviceModel& device_model, std::shared_ptr<EvseSecurity> evse_security,
                                         std::shared_ptr<MessageLogging> logging,
                                         const std::function<void(const std::string& message)>& message_callback) :
    device_model{device_model},
    evse_security{evse_security},
    logging{logging},
    websocket{nullptr},
    message_callback{message_callback},
    disable_automatic_websocket_reconnects{false},
    network_configuration_priority{0} {
}

void ConnectivityManager::set_websocket_authorization_key(const std::string& authorization_key) {
    if (this->websocket != nullptr) {
        this->websocket->set_authorization_key(authorization_key);
    }
}

void ConnectivityManager::set_websocket_connection_options(const WebsocketConnectionOptions& connection_options) {
    this->current_connection_options = connection_options;
    if (this->websocket != nullptr) {
        this->websocket->set_connection_options(connection_options);
    }
}

void ConnectivityManager::set_websocket_connection_options_without_reconnect() {
    const int configuration_slot = get_active_network_configuration_slot();
    const auto connection_options = this->get_ws_connection_options(configuration_slot);
    this->set_websocket_connection_options(connection_options);
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
ConnectivityManager::get_network_connection_profile(const int32_t configuration_slot) {

    for (const auto& network_profile : this->network_connection_profiles) {
        if (network_profile.configurationSlot == configuration_slot) {
            switch (auto security_profile = network_profile.connectionData.securityProfile) {
            case security::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION:
                throw std::invalid_argument("security_profile = " + std::to_string(security_profile) +
                                            " not officially allowed in OCPP 2.0.1");
            default:
                break;
            }

            return network_profile.connectionData;
        }
    }
    return std::nullopt;
}

std::optional<int> ConnectivityManager::get_configuration_slot_priority(const int configuration_slot) {
    auto it = std::find(this->network_connection_priorities.begin(), this->network_connection_priorities.end(),
                        configuration_slot);
    if (it != network_connection_priorities.end()) {
        // Index is iterator - begin iterator
        return it - network_connection_priorities.begin();
    }
    return std::nullopt;
}

const std::vector<int>& ConnectivityManager::get_network_connection_priorities() const {
    return this->network_connection_priorities;
}

bool ConnectivityManager::is_websocket_connected() {
    return this->websocket != nullptr && this->websocket->is_connected();
}

void ConnectivityManager::start() {
    init_websocket();
    if (websocket != nullptr) {
        this->disable_automatic_websocket_reconnects = false;
        websocket->connect();
    }
}

void ConnectivityManager::stop() {
    this->websocket_timer.stop();
    disconnect_websocket(WebsocketCloseReason::Normal);
}

void ConnectivityManager::connect() {
    if (this->websocket != nullptr and !this->websocket->is_connected()) {
        this->disable_automatic_websocket_reconnects = false;
        this->init_websocket();
        this->websocket->connect();
    }
}

void ConnectivityManager::disconnect_websocket(WebsocketCloseReason code) {
    if (this->websocket != nullptr) {
        if (code != WebsocketCloseReason::ServiceRestart) {
            this->disable_automatic_websocket_reconnects = true;
        }
        this->websocket->disconnect(code);
    }
}

bool ConnectivityManager::send_to_websocket(const std::string& message) {
    if (this->websocket == nullptr) {
        return false;
    }

    return this->websocket->send(message);
}

void ConnectivityManager::on_network_disconnected(int32_t configuration_slot) {
    const int actual_configuration_slot = get_active_network_configuration_slot();
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(actual_configuration_slot);

    if (!network_connection_profile.has_value()) {
        EVLOG_warning << "Network disconnected. No network connection profile configured";
    } else if (configuration_slot == actual_configuration_slot) {
        // Since there is no connection anymore: disconnect the websocket, the manager will try to connect with the next
        // available network connection profile as we enable reconnects.
        this->disconnect_websocket(ocpp::WebsocketCloseReason::GoingAway);
        this->disable_automatic_websocket_reconnects = false;
    }
}

void ConnectivityManager::on_network_disconnected(OCPPInterfaceEnum ocpp_interface) {

    const int actual_configuration_slot = get_active_network_configuration_slot();
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(actual_configuration_slot);

    if (!network_connection_profile.has_value()) {
        EVLOG_warning << "Network disconnected. No network connection profile configured";
    } else if (ocpp_interface == network_connection_profile.value().ocppInterface) {
        // Since there is no connection anymore: disconnect the websocket, the manager will try to connect with the next
        // available network connection profile as we enable reconnects.
        this->disconnect_websocket(ocpp::WebsocketCloseReason::GoingAway);
        this->disable_automatic_websocket_reconnects = false;
    }
}

bool ConnectivityManager::on_try_switch_network_connection_profile(const int32_t configuration_slot) {
    if (!is_higher_priority_profile(configuration_slot)) {
        return false;
    }

    EVLOG_info << "Trying to connect with higher priority network connection profile (configuration slots: "
               << this->get_active_network_configuration_slot() << " --> " << configuration_slot << ").";

    const std::optional<NetworkConnectionProfile> network_connection_profile_opt =
        this->get_network_connection_profile(configuration_slot);
    if (!network_connection_profile_opt.has_value()) {
        EVLOG_warning << "Could not find network connection profile belonging to configuration slot "
                      << configuration_slot;
        return false;
    }
    this->disconnect_websocket(WebsocketCloseReason::Normal);
    reconnect(WebsocketCloseReason::Normal, get_configuration_slot_priority(configuration_slot));
    return true;
}

void ConnectivityManager::init_websocket() {
    if (this->device_model.get_value<std::string>(ControllerComponentVariables::ChargePointId).find(':') !=
        std::string::npos) {
        EVLOG_AND_THROW(std::runtime_error("ChargePointId must not contain \':\'"));
    }

    // cache the network profiles on initialization
    cache_network_connection_profiles();

    const int config_slot_int = this->network_connection_priorities.at(this->network_configuration_priority);

    const auto network_connection_profile = this->get_network_connection_profile(config_slot_int);
    // Not const as the iface member can be set by the configure network connection profile callback
    auto connection_options = this->get_ws_connection_options(config_slot_int);
    bool can_use_connection_profile = true;

    if (!network_connection_profile.has_value()) {
        EVLOG_warning << "No network connection profile configured for " << config_slot_int;
        can_use_connection_profile = false;
    } else if (this->configure_network_connection_profile_callback.has_value()) {
        EVLOG_debug << "Request to configure network connection profile " << config_slot_int;

        std::future<ConfigNetworkResult> config_status = this->configure_network_connection_profile_callback.value()(
            config_slot_int, network_connection_profile.value());
        const int32_t config_timeout =
            this->device_model.get_optional_value<int>(ControllerComponentVariables::NetworkConfigTimeout)
                .value_or(default_network_config_timeout_seconds);

        std::future_status status = config_status.wait_for(std::chrono::seconds(config_timeout));

        switch (status) {
        case std::future_status::deferred:
        case std::future_status::timeout: {
            EVLOG_warning << "Timeout configuring config slot: " << config_slot_int;
            can_use_connection_profile = false;
            break;
        }
        case std::future_status::ready: {
            ConfigNetworkResult result = config_status.get();
            if (result.success and result.network_profile_slot == config_slot_int) {
                EVLOG_debug << "Config slot " << config_slot_int << " is configured";
                // Set interface or ip to connection options.
                connection_options.iface = result.interface_address;
            } else {
                EVLOG_warning << "Could not configure config slot " << config_slot_int;
                can_use_connection_profile = false;
            }
            break;
        }
        }
    }

    if (!can_use_connection_profile) {
        this->websocket_timer.timeout(
            [this]() {
                this->next_network_configuration_priority();
                this->start();
            },
            WEBSOCKET_INIT_DELAY);
        return;
    }

    EVLOG_info << "Open websocket with NetworkConfigurationPriority: " << this->network_configuration_priority + 1
               << " which is configurationSlot " << config_slot_int;

    if (const auto& active_network_profile_cv = ControllerComponentVariables::ActiveNetworkProfile;
        active_network_profile_cv.variable.has_value()) {
        this->device_model.set_read_only_value(
            active_network_profile_cv.component, active_network_profile_cv.variable.value(), AttributeEnum::Actual,
            std::to_string(config_slot_int), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    if (const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
        security_profile_cv.variable.has_value()) {
        this->device_model.set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                               AttributeEnum::Actual,
                                               std::to_string(network_connection_profile.value().securityProfile),
                                               VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    this->websocket = std::make_unique<Websocket>(connection_options, this->evse_security, this->logging);

    this->websocket->register_connected_callback(
        std::bind(&ConnectivityManager::on_websocket_connected, this, std::placeholders::_1));
    this->websocket->register_disconnected_callback(std::bind(&ConnectivityManager::on_websocket_disconnected, this));
    this->websocket->register_closed_callback(
        std::bind(&ConnectivityManager::on_websocket_closed, this, std::placeholders::_1));

    if (websocket_connection_failed_callback.has_value()) {
        this->websocket->register_connection_failed_callback(websocket_connection_failed_callback.value());
    }

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

WebsocketConnectionOptions ConnectivityManager::get_ws_connection_options(const int32_t configuration_slot) {
    const auto network_connection_profile_opt = this->get_network_connection_profile(configuration_slot);

    if (!network_connection_profile_opt.has_value()) {
        EVLOG_critical << "Could not retrieve NetworkProfile of configurationSlot: " << configuration_slot;
        throw std::runtime_error("Could not retrieve NetworkProfile");
    }

    const auto network_connection_profile = network_connection_profile_opt.value();

    auto uri = Uri::parse_and_validate(
        network_connection_profile.ocppCsmsUrl.get(),
        this->device_model.get_value<std::string>(ControllerComponentVariables::SecurityCtrlrIdentity),
        network_connection_profile.securityProfile);

    WebsocketConnectionOptions connection_options{
        OcppProtocolVersion::v201,
        uri,
        network_connection_profile.securityProfile,
        this->device_model.get_optional_value<std::string>(ControllerComponentVariables::BasicAuthPassword),
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
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsCommonName).value_or(true),
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::UseTPM).value_or(false),
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsAllowWildcards)
            .value_or(false),
        this->device_model.get_optional_value<std::string>(ControllerComponentVariables::IFace),
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::EnableTLSKeylog).value_or(false),
        this->device_model.get_optional_value<std::string>(ControllerComponentVariables::TLSKeylogFile)};

    return connection_options;
}

void ConnectivityManager::on_websocket_connected([[maybe_unused]] int security_profile) {
    const int actual_configuration_slot = get_active_network_configuration_slot();
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(actual_configuration_slot);

    if (this->websocket_connected_callback.has_value() and network_connection_profile.has_value()) {
        this->websocket_connected_callback.value()(actual_configuration_slot, network_connection_profile.value());
    }
}

void ConnectivityManager::on_websocket_disconnected() {
    std::optional<NetworkConnectionProfile> network_connection_profile =
        this->get_network_connection_profile(this->get_active_network_configuration_slot());

    if (this->websocket_disconnected_callback.has_value() and network_connection_profile.has_value()) {
        this->websocket_disconnected_callback.value()(this->get_active_network_configuration_slot(),
                                                      network_connection_profile.value());
    }
}

void ConnectivityManager::on_websocket_closed(ocpp::WebsocketCloseReason reason) {
    EVLOG_warning << "Closed websocket of NetworkConfigurationPriority: " << this->network_configuration_priority + 1
                  << " which is configurationSlot " << this->get_active_network_configuration_slot();

    if (!this->disable_automatic_websocket_reconnects) {
        reconnect(reason);
    }
}

void ConnectivityManager::reconnect(WebsocketCloseReason reason, std::optional<int> next_priority) {
    this->websocket_timer.timeout(
        [this, reason, next_priority]() {
            if (reason != WebsocketCloseReason::ServiceRestart) {
                if (!next_priority.has_value()) {
                    this->next_network_configuration_priority();
                } else {
                    this->network_configuration_priority = next_priority.value();
                }
            }
            this->start();
        },
        WEBSOCKET_INIT_DELAY);
}

bool ConnectivityManager::is_higher_priority_profile(const int new_configuration_slot) {

    const int current_slot = get_active_network_configuration_slot();
    if (current_slot == 0) {
        // No slot in use, new is always higher priority.
        return true;
    }

    if (current_slot == new_configuration_slot) {
        // Slot is the same, probably already connected
        return false;
    }

    const std::optional<int> new_priority = get_configuration_slot_priority(new_configuration_slot);
    if (!new_priority.has_value()) {
        // Slot not found.
        return false;
    }

    const std::optional<int> current_priority = get_configuration_slot_priority(current_slot);
    if (!current_priority.has_value()) {
        // Slot not found.
        return false;
    }

    if (new_priority.value() < current_priority.value()) {
        // Priority is indeed higher (lower index means higher priority)
        return true;
    }

    return false;
}

int ConnectivityManager::get_active_network_configuration_slot() {
    return this->network_connection_priorities.at(this->network_configuration_priority);
}

void ConnectivityManager::next_network_configuration_priority() {

    // retrieve priorities from cache
    if (this->network_connection_priorities.size() > 1) {
        EVLOG_info << "Switching to next network configuration priority";
    }
    this->network_configuration_priority =
        (this->network_configuration_priority + 1) % (this->network_connection_priorities.size());
}

void ConnectivityManager::cache_network_connection_profiles() {

    if (!this->network_connection_profiles.empty()) {
        EVLOG_debug << " Network connection profiles already cached";
        return;
    }

    // get all the network connection profiles from the device model and cache them
    this->network_connection_profiles =
        json::parse(this->device_model.get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));

    for (const std::string& str : ocpp::split_string(
             this->device_model.get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority),
             ',')) {
        int num = std::stoi(str);
        this->network_connection_priorities.push_back(num);
    }

    if (this->network_connection_priorities.empty()) {
        EVLOG_AND_THROW(std::runtime_error("NetworkConfigurationPriority must not be empty"));
    }
}
} // namespace v201
} // namespace ocpp
