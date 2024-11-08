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
        this->config.prioritize_authorization_over_stopping_transaction, this->config.ignore_connector_faults,
        this->info.id, (!this->r_kvs.empty() ? this->r_kvs.at(0).get() : nullptr));

    for (const auto& token_provider : this->r_token_provider) {
        token_provider->subscribe_provided_token([this](ProvidedIdToken provided_token) {
            std::thread t([this, provided_token]() { this->auth_handler->on_token(provided_token); });
            t.detach();
        });
    }
}

void Auth::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_reservation);

    int32_t evse_index = 0;
    for (const auto& evse_manager : this->r_evse_manager) {
        const int32_t evse_id = evse_manager->call_get_evse().id;

        for (const auto& connector : evse_manager->call_get_evse().connectors) {
            this->auth_handler->init_connector(
                connector.id, evse_index, connector.type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
        }

        evse_manager->subscribe_session_event([this, evse_id](SessionEvent session_event) {
            this->auth_handler->handle_session_event(evse_id, session_event);
        });

        evse_manager->subscribe_error(
            "evse_manager/Inoperative",
            // If no connector id is given, it defaults to connector id 1.
            [this, evse_id](const Everest::error::Error& error) {
                this->auth_handler->handle_permanent_fault_raised(evse_id, 1);
            },
            // If no connector id is given, it defaults to connector id 1.
            [this, evse_id](const Everest::error::Error& error) {
                this->auth_handler->handle_permanent_fault_cleared(evse_id, 1);
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
    this->auth_handler->register_reserved_callback(
        [this](const std::optional<int32_t> evse_id, const int32_t& reservation_id) {
            // Only call the evse manager to store the reservation if it is done for a specific evse.
            if (evse_id.has_value()) {
                EVLOG_info << "Call reserved callback for evse id " << evse_id.value();

                // TODO mz do something with the result.
                this->r_evse_manager.at(evse_id.value() - 1)->call_reserve(reservation_id);
            }

            ReservationUpdateStatus status;
            status.reservation_id = reservation_id;
            status.reservation_status = Reservation_status::Placed;
            this->p_reservation->publish_reservation_update(status);
        });
    this->auth_handler->register_reservation_cancelled_callback(
        [this](const std::optional<int32_t> evse_id, const int32_t reservation_id, const ReservationEndReason reason) {
            // Only call the evse manager to cancel the reservation if it was for a specific evse
            if (evse_id.has_value()) {
                EVLOG_debug << "Call evse manager to cancel the reservation with evse id " << evse_id.value();
                this->r_evse_manager.at(evse_id.value() - 1)->call_cancel_reservation();
            }

            ReservationUpdateStatus status;
            status.reservation_id = reservation_id;
            if (reason == ReservationEndReason::Expired) {
                status.reservation_status = Reservation_status::Expired;
            } else if (reason == ReservationEndReason::Cancelled) {
                status.reservation_status = Reservation_status::Removed;
            } else {
                // On reservation used: do not publish a reservation update!!
                // TODO mz implement this to publish on placed etc. !! And let somewhere else the software not send
                // messages.
                return;
            }
            this->p_reservation->publish_reservation_update(status);
        });

    this->auth_handler->initialized();
}

void Auth::set_connection_timeout(int& connection_timeout) {
    this->auth_handler->set_connection_timeout(connection_timeout);
}

void Auth::set_master_pass_group_id(const std::string& master_pass_group_id) {
    this->auth_handler->set_master_pass_group_id(master_pass_group_id);
}

} // namespace module
