// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utility>

#include "Auth.hpp"
#include <everest/logging.hpp>

namespace module {

void Auth::init() {
    invoke_init(*p_main);
    invoke_init(*p_reservation);

    this->auth_handler = std::make_unique<AuthHandler>(
        string_to_selection_algorithm(this->config.selection_algorithm), this->config.connection_timeout,
        this->config.prioritize_authorization_over_stopping_transaction, this->config.ignore_connector_faults);

    for (const auto& token_provider : this->r_token_provider) {
        token_provider->subscribe_provided_token([this](ProvidedIdToken provided_token) {
            std::thread t([this, provided_token]() { const auto res = this->auth_handler->on_token(provided_token); });
            t.detach();
        });
    }
}

void Auth::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_reservation);

    int32_t evse_index = 0;
    for (const auto& evse_manager : this->r_evse_manager) {
        int32_t connector_id = evse_manager->call_get_evse().id;
        this->auth_handler->init_connector(connector_id, evse_index);

        evse_manager->subscribe_session_event([this, connector_id](SessionEvent session_event) {
            this->auth_handler->handle_session_event(connector_id, session_event);
        });

        evse_index++;
    }

    this->auth_handler->register_publish_token_validation_status_callback(
        [this](const ProvidedIdToken& token, TokenValidationStatus status) {
            this->p_main->publish_token_validation_status({token, status});
        });
    this->auth_handler->register_notify_evse_callback(
        [this](const int evse_index, const ProvidedIdToken& provided_token, const ValidationResult& validation_result) {
            this->r_evse_manager.at(evse_index)->call_authorize_response(provided_token, validation_result);
        });
    this->auth_handler->register_withdraw_authorization_callback(
        [this](const int32_t evse_index) { this->r_evse_manager.at(evse_index)->call_withdraw_authorization(); });
    this->auth_handler->register_validate_token_callback([this](const ProvidedIdToken& provided_token) {
        std::vector<ValidationResult> validation_results;
        for (const auto& token_validator : this->r_token_validator) {
            validation_results.push_back(token_validator->call_validate_token(provided_token));
        }
        return validation_results;
    });
    this->auth_handler->register_stop_transaction_callback(
        [this](const int32_t evse_index, const StopTransactionRequest& request) {
            this->r_evse_manager.at(evse_index)->call_stop_transaction(request);
        });
    this->auth_handler->register_reserved_callback([this](const int32_t evse_index, const int32_t& reservation_id) {
        this->r_evse_manager.at(evse_index)->call_reserve(reservation_id);
    });
    this->auth_handler->register_reservation_cancelled_callback(
        [this](const int32_t evse_index) { this->r_evse_manager.at(evse_index)->call_cancel_reservation(); });
}

void Auth::set_connection_timeout(int& connection_timeout) {
    this->auth_handler->set_connection_timeout(connection_timeout);
}

} // namespace module
