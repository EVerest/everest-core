// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/helpers/helpers.hpp>

#include <utility>

#include "Auth.hpp"
#include <everest/logging.hpp>

namespace module {

void Auth::init() {
    invoke_init(*p_main);
    invoke_init(*p_reservation);

    this->auth_handler = std::make_unique<AuthHandler>(
        string_to_selection_algorithm(this->config.selection_algorithm), this->config.connection_timeout,
        this->config.plug_in_timeout_enabled, this->config.prioritize_authorization_over_stopping_transaction,
        this->config.ignore_connector_faults, this->info.id,
        (!this->r_kvs.empty() ? this->r_kvs.at(0).get() : nullptr));

    for (const auto& token_provider : this->r_token_provider) {
        token_provider->subscribe_provided_token([this](ProvidedIdToken provided_token) {
            {
                auto state = this->event_state.handle();
                if (!state->started) {
                    EVLOG_warning << "Auth not fully initialized. Discarding provided token";
                    return;
                }
            }
            std::thread t([this, provided_token]() { this->auth_handler->on_token(provided_token); });
            t.detach();
        });
    }
    for (const auto& token_validator : this->r_token_validator) {
        token_validator->subscribe_validate_result_update([this](ValidationResultUpdate validation_result_update) {
            {
                auto state = this->event_state.handle();
                if (!state->started) {
                    EVLOG_warning << "Auth not fully initialized. Discarding validation result update";
                    return;
                }
            }
            this->auth_handler->handle_token_validation_result_update(validation_result_update);
        });
    }

    // Subscribe to session events and errors in init() so we don't miss any events
    // Events received before ready() are queued.
    int32_t evse_index = 0;
    for (const auto& evse_manager : this->r_evse_manager) {
        const int32_t evse_idx = evse_index;

        evse_manager->subscribe_session_event([this, evse_idx](SessionEvent session_event) {
            {
                auto state = this->event_state.handle();
                if (!state->started) {
                    EVLOG_debug << "Auth not fully initialized, but received a session event on evse_index: "
                                << evse_idx << " that will be queued up: " << session_event.event;
                    state->event_queue.emplace(evse_idx, session_event);
                    return;
                }
            }
            this->auth_handler->handle_session_event(this->auth_handler->get_evse_id_by_index(evse_idx), session_event);
        });

        evse_manager->subscribe_error(
            "evse_manager/Inoperative",
            [this, evse_idx](const Everest::error::Error& error) {
                {
                    auto state = this->event_state.handle();
                    if (!state->started) {
                        EVLOG_debug << "Auth not fully initialized, queuing permanent fault raised for evse_index: "
                                    << evse_idx;
                        state->event_queue.emplace(evse_idx, PermanentFaultRaised{1});
                        return;
                    }
                }
                this->auth_handler->handle_permanent_fault_raised(this->auth_handler->get_evse_id_by_index(evse_idx),
                                                                  1);
            },
            [this, evse_idx](const Everest::error::Error& error) {
                {
                    auto state = this->event_state.handle();
                    if (!state->started) {
                        EVLOG_debug << "Auth not fully initialized, queuing permanent fault cleared for evse_index: "
                                    << evse_idx;
                        state->event_queue.emplace(evse_idx, PermanentFaultCleared{1});
                        return;
                    }
                }
                this->auth_handler->handle_permanent_fault_cleared(this->auth_handler->get_evse_id_by_index(evse_idx),
                                                                   1);
            });

        evse_index++;
    }
}

void Auth::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_reservation);

    int32_t evse_index = 0;
    for (const auto& evse_manager : this->r_evse_manager) {
        const int32_t evse_id = evse_manager->call_get_evse().id;
        std::vector<Connector> connectors;
        for (const auto& connector : evse_manager->call_get_evse().connectors) {
            connectors.push_back(
                Connector(connector.id, connector.type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown)));
        }

        this->auth_handler->init_evse(evse_id, evse_index, connectors);

        evse_index++;
    }

    this->auth_handler->register_publish_token_action_callback(
        [this](const ProvidedIdToken& token, TokenActionStatus status,
               const std::vector<MessageContent>& tariff_messages) {
            this->p_main->publish_token_action({token, status, tariff_messages});
        });
    this->auth_handler->register_publish_token_validation_results_callback(
        [this](const ProvidedIdToken& token, const std::vector<ValidationResult>& results) {
            this->p_main->publish_token_validation_status({token, results});
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
            try {
                const auto result = token_validator->call_validate_token(provided_token);
                validation_results.push_back(result);
                // TODO: This is very broad catch, make it more narrow when the everest-framework error handling will be
                // established
            } catch (const std::exception& e) {
                EVLOG_warning << "Exception during validating token: " << e.what();
                ValidationResult validation_result;
                validation_result.authorization_status = AuthorizationStatus::Unknown;
                validation_results.push_back(validation_result);
            }
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

                if (!this->r_evse_manager.at(evse_id.value() - 1)->call_reserve(reservation_id)) {
                    EVLOG_warning << "EVSE manager does not allow placing a reservation for evse id " << evse_id.value()
                                  << ": cancelling reservation.";
                    this->auth_handler->handle_cancel_reservation(reservation_id);
                    return false;
                }
            }

            ReservationUpdateStatus status;
            status.reservation_id = reservation_id;
            status.reservation_status = Reservation_status::Placed;
            this->p_reservation->publish_reservation_update(status);
            return true;
        });
    this->auth_handler->register_reservation_cancelled_callback(
        [this](const std::optional<int32_t> evse_id, const int32_t reservation_id, const ReservationEndReason reason,
               const bool send_reservation_update) {
            // Only call the evse manager to cancel the reservation if it was for a specific evse
            if (evse_id.has_value() && evse_id.value() > 0) {
                EVLOG_debug << "Call evse manager to cancel the reservation with evse id " << evse_id.value();
                this->r_evse_manager.at(evse_id.value() - 1)->call_cancel_reservation();
            }

            if (send_reservation_update) {
                ReservationUpdateStatus status;
                status.reservation_id = reservation_id;
                if (reason == ReservationEndReason::Expired) {
                    status.reservation_status = Reservation_status::Expired;
                } else if (reason == ReservationEndReason::Cancelled) {
                    status.reservation_status = Reservation_status::Removed;
                } else {
                    // On reservation used: do not publish a reservation update!!
                    return;
                }
                this->p_reservation->publish_reservation_update(status);
            }
        });

    // Process any events that were queued during init before we were ready
    {
        auto state = this->event_state.handle();
        while (!state->event_queue.empty()) {
            auto queued_event = state->event_queue.front();
            state->event_queue.pop();
            const int32_t evse_id = this->auth_handler->get_evse_id_by_index(queued_event.evse_index);
            if (std::holds_alternative<SessionEvent>(queued_event.data)) {
                const auto& session_event = std::get<SessionEvent>(queued_event.data);
                EVLOG_debug << "Processing queued session event for evse_id: " << evse_id
                            << ", event: " << session_event.event;
                this->auth_handler->handle_session_event(evse_id, session_event);
            } else if (std::holds_alternative<PermanentFaultRaised>(queued_event.data)) {
                const auto& fault = std::get<PermanentFaultRaised>(queued_event.data);
                EVLOG_debug << "Processing queued permanent fault raised for evse_id: " << evse_id;
                this->auth_handler->handle_permanent_fault_raised(evse_id, fault.connector_id);
            } else if (std::holds_alternative<PermanentFaultCleared>(queued_event.data)) {
                const auto& fault = std::get<PermanentFaultCleared>(queued_event.data);
                EVLOG_debug << "Processing queued permanent fault cleared for evse_id: " << evse_id;
                this->auth_handler->handle_permanent_fault_cleared(evse_id, fault.connector_id);
            }
        }
        state->started = true;
    }

    this->auth_handler->initialize();
}

void Auth::set_connection_timeout(int& connection_timeout) {
    this->auth_handler->set_connection_timeout(connection_timeout);
}

void Auth::set_master_pass_group_id(const std::string& master_pass_group_id) {
    this->auth_handler->set_master_pass_group_id(master_pass_group_id);
}

WithdrawAuthorizationResult Auth::handle_withdraw_authorization(const WithdrawAuthorizationRequest& request) {
    return this->auth_handler->handle_withdraw_authorization(request);
}

} // namespace module
