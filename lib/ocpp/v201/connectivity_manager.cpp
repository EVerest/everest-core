// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/connectivity_manager.hpp>

#include <everest/logging.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>

namespace {
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL = "internal";
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
    const auto configuration_slot = this->network_connection_priorities.at(this->network_configuration_priority);
    const auto connection_options = this->get_ws_connection_options(std::stoi(configuration_slot));
    this->set_websocket_connection_options(connection_options);
}

void ConnectivityManager::set_websocket_connected_callback(WebsocketConnectedCallback callback) {
    this->websocket_connected_callback = callback;
}

void ConnectivityManager::set_websocket_disconnected_callback(WebsocketDisconnectedCallback callback) {
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

bool ConnectivityManager::is_websocket_connected() {
    return this->websocket != nullptr && this->websocket->is_connected();
}

void ConnectivityManager::start() {
    init_websocket();
    if (websocket != nullptr) {
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
        this->disable_automatic_websocket_reconnects = true;
        this->websocket->disconnect(code);
    }
}

bool ConnectivityManager::send_to_websocket(const std::string& message) {
    if (this->websocket == nullptr) {
        return false;
    }

    return this->websocket->send(message);
}

void ConnectivityManager::init_websocket() {
    if (this->device_model.get_value<std::string>(ControllerComponentVariables::ChargePointId).find(':') !=
        std::string::npos) {
        EVLOG_AND_THROW(std::runtime_error("ChargePointId must not contain \':\'"));
    }

    // cache the network profiles on initialization
    cache_network_connection_profiles();

    const auto configuration_slot = this->network_connection_priorities.at(this->network_configuration_priority);
    const auto connection_options = this->get_ws_connection_options(std::stoi(configuration_slot));

    const auto network_connection_profile = this->get_network_connection_profile(std::stoi(configuration_slot));

    if (!network_connection_profile.has_value() or
        (this->configure_network_connection_profile_callback.has_value() and
         !this->configure_network_connection_profile_callback.value()(network_connection_profile.value()))) {
        EVLOG_warning << "NetworkConnectionProfile could not be retrieved or configuration of network with the given "
                         "profile failed";
        this->websocket_timer.timeout(
            [this]() {
                this->next_network_configuration_priority();
                this->start();
            },
            WEBSOCKET_INIT_DELAY);
        return;
    }

    EVLOG_info << "Open websocket with NetworkConfigurationPriority: " << this->network_configuration_priority + 1
               << " which is configurationSlot " << configuration_slot;

    if (const auto& active_network_profile_cv = ControllerComponentVariables::ActiveNetworkProfile;
        active_network_profile_cv.variable.has_value()) {
        this->device_model.set_read_only_value(active_network_profile_cv.component,
                                               active_network_profile_cv.variable.value(), AttributeEnum::Actual,
                                               configuration_slot, VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    if (const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
        security_profile_cv.variable.has_value()) {
        this->device_model.set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                               AttributeEnum::Actual,
                                               std::to_string(network_connection_profile.value().securityProfile),
                                               VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    this->websocket = std::make_unique<Websocket>(connection_options, this->evse_security, this->logging);

    if (this->websocket_connected_callback.has_value()) {
        this->websocket->register_connected_callback(websocket_connected_callback.value());
    }

    if (this->websocket_disconnected_callback.has_value()) {
        this->websocket->register_disconnected_callback(websocket_disconnected_callback.value());
    }

    this->websocket->register_closed_callback(
        [this, connection_options, configuration_slot](const WebsocketCloseReason reason) {
            EVLOG_warning << "Closed websocket of NetworkConfigurationPriority: "
                          << this->network_configuration_priority + 1 << " which is configurationSlot "
                          << configuration_slot;

            if (!this->disable_automatic_websocket_reconnects) {
                this->websocket_timer.timeout(
                    [this, reason]() {
                        if (reason != WebsocketCloseReason::ServiceRestart) {
                            this->next_network_configuration_priority();
                        }
                        this->start();
                    },
                    WEBSOCKET_INIT_DELAY);
            }
        });

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
        this->device_model.get_optional_value<std::string>(ControllerComponentVariables::IFace)};

    return connection_options;
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

    this->network_connection_priorities = ocpp::get_vector_from_csv(
        this->device_model.get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority));

    if (this->network_connection_priorities.empty()) {
        EVLOG_AND_THROW(std::runtime_error("NetworkConfigurationPriority must not be empty"));
    }
}
} // namespace v201
} // namespace ocpp
