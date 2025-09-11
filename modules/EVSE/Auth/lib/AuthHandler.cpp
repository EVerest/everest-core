// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <AuthHandler.hpp>

#include <everest/helpers/helpers.hpp>
#include <everest/logging.hpp>
#include <generated/interfaces/kvs/Interface.hpp>

using everest::helpers::is_equal_case_insensitive;

namespace module {

/// \brief helper method to intersect referenced_connectors (from ProvidedIdToken) with evses that are listed
/// within ValidationResult
std::vector<int> intersect(std::vector<int>& a, std::vector<int>& b) {
    std::vector<int> result;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    return result;
}

namespace conversions {
std::string token_handling_result_to_string(const TokenHandlingResult& result) {
    switch (result) {
    case TokenHandlingResult::ALREADY_IN_PROCESS:
        return "ALREADY_IN_PROCESS";
    case TokenHandlingResult::NO_CONNECTOR_AVAILABLE:
        return "NO_CONNECTOR_AVAILABLE";
    case TokenHandlingResult::REJECTED:
        return "REJECTED";
    case TokenHandlingResult::TIMEOUT:
        return "TIMEOUT";
    case TokenHandlingResult::USED_TO_START_TRANSACTION:
        return "USED_TO_START_TRANSACTION";
    case TokenHandlingResult::USED_TO_STOP_TRANSACTION:
        return "USED_TO_STOP_TRANSACTION";
    case TokenHandlingResult::WITHDRAWN:
        return "WITHDRAWN";
    default:
        throw std::runtime_error("No known conversion for the given token handling result");
    }
}
} // namespace conversions

AuthHandler::AuthHandler(const SelectionAlgorithm& selection_algorithm, const int connection_timeout,
                         bool prioritize_authorization_over_stopping_transaction, bool ignore_faults,
                         const std::string& id, kvsIntf* store) :
    selection_algorithm(selection_algorithm),
    connection_timeout(connection_timeout),
    prioritize_authorization_over_stopping_transaction(prioritize_authorization_over_stopping_transaction),
    ignore_faults(ignore_faults),
    reservation_handler(evses, id, store) {
}

AuthHandler::~AuthHandler() {
}

void AuthHandler::init_evse(const int evse_id, const int evse_index, const std::vector<Connector>& connectors) {
    std::lock_guard<std::mutex> lock(this->event_mutex);
    EVLOG_debug << "Add evse with evse id " << evse_id;

    if (evse_id <= 0) {
        EVLOG_error << "Can not initialize EVSE: evse id is <= 0.";
        return;
    }

    this->evses[evse_id] = std::make_unique<EVSEContext>(evse_id, evse_index, connectors);
}

void AuthHandler::initialize() {
    std::lock_guard<std::mutex> lock(this->event_mutex);
    this->reservation_handler.load_reservations();
    check_evse_reserved_and_send_updates();
}

TokenHandlingResult AuthHandler::on_token(const ProvidedIdToken& provided_token) {
    std::unique_lock<std::mutex> lk(this->event_mutex);
    if (!this->publish_token_validation_status_callback) {
        return TokenHandlingResult::REJECTED;
    }

    TokenHandlingResult result;
    ProvidedIdToken provided_token_copy = provided_token;

    // check if token is already currently processed
    EVLOG_info << "Received new token: " << everest::helpers::redact(provided_token);
    const auto referenced_evses = this->get_referenced_evses(provided_token);

    if (!this->is_token_already_in_process(provided_token, referenced_evses)) {
        // process token if not already in process
        this->tokens_in_process.insert(provided_token);
        this->publish_token_validation_status_callback(provided_token, TokenValidationStatus::Processing);
        result = this->handle_token(provided_token_copy, lk);
    } else {
        // do nothing if token is currently processed
        EVLOG_info << "Received token " << everest::helpers::redact(provided_token.id_token.value)
                   << " repeatedly while still processing it";
        result = TokenHandlingResult::ALREADY_IN_PROCESS;
    }

    switch (result) {
    case TokenHandlingResult::ALREADY_IN_PROCESS:
        break;
    case TokenHandlingResult::TIMEOUT: // Timeout means accepted but failed to pick contactor
        this->publish_token_validation_status_callback(provided_token_copy, TokenValidationStatus::TimedOut);
        break;
    case TokenHandlingResult::NO_CONNECTOR_AVAILABLE:
    case TokenHandlingResult::REJECTED:
        this->publish_token_validation_status_callback(provided_token_copy, TokenValidationStatus::Rejected);
        break;
    case TokenHandlingResult::USED_TO_START_TRANSACTION:
        this->publish_token_validation_status_callback(provided_token_copy, TokenValidationStatus::UsedToStart);
        break;
    case TokenHandlingResult::USED_TO_STOP_TRANSACTION:
        this->publish_token_validation_status_callback(provided_token_copy, TokenValidationStatus::UsedToStop);
        break;
    case TokenHandlingResult::WITHDRAWN:
        this->publish_token_validation_status_callback(provided_token_copy, TokenValidationStatus::Withdrawn);
        break;
    }

    if (result != TokenHandlingResult::ALREADY_IN_PROCESS) {
        this->tokens_in_process.erase(provided_token);
    }

    EVLOG_info << "Result for token: " << everest::helpers::redact(provided_token.id_token.value) << ": "
               << conversions::token_handling_result_to_string(result);
    this->processing_finished_cv.notify_all();
    return result;
}

void AuthHandler::handle_token_validation_result_update(const ValidationResultUpdate& validation_result_update) {
    std::unique_lock<std::mutex> lk(this->event_mutex);
    auto connector_id = validation_result_update.connector_id;
    if (this->evses.find(connector_id) != this->evses.end() and this->evses.at(connector_id)->identifier.has_value()) {
        EVLOG_info << "Updating validation result on evse#" << connector_id; // old OCPP "connector" is now "EVSE"
        // Currently we only support updating the parent id token
        this->evses.at(connector_id)->identifier->authorization_status =
            validation_result_update.validation_result.authorization_status;
        this->evses.at(connector_id)->identifier->parent_id_token =
            validation_result_update.validation_result.parent_id_token;
        types::authorization::ProvidedIdToken provided_token;
        provided_token.id_token = this->evses.at(connector_id)->identifier->id_token;
        provided_token.authorization_type = this->evses.at(connector_id)->identifier->type;
        provided_token.parent_id_token = validation_result_update.validation_result.parent_id_token;
        std::vector<int32_t> connectors_allowed{connector_id};
        provided_token.connectors = connectors_allowed;
        this->publish_token_validation_status_callback(provided_token,
                                                       types::authorization::TokenValidationStatus::Accepted);
    } else {
        EVLOG_error << "Unknown evse#" << connector_id
                    << " or unknown authorization identifier on the evse for validation result update.";
    }
}

TokenHandlingResult AuthHandler::handle_token(ProvidedIdToken& provided_token, std::unique_lock<std::mutex>& lk) {
    std::vector<int> referenced_evses = this->get_referenced_evses(provided_token);

    // Only provided token with type RFID can be used to stop a transaction
    if (provided_token.authorization_type == AuthorizationType::RFID) {
        // check if id_token is used for an active transaction
        const auto evse_used_for_transaction =
            this->used_for_transaction(referenced_evses, provided_token.id_token.value);
        if (evse_used_for_transaction != -1) {
            StopTransactionRequest req;
            req.reason = StopTransactionReason::Local;
            req.id_tag.emplace(provided_token);
            this->stop_transaction_callback(this->evses.at(evse_used_for_transaction)->evse_index, req);
            EVLOG_info << "Transaction was stopped because id_token was used for transaction for evse#"
                       << evse_used_for_transaction;
            if (this->evses.at(evse_used_for_transaction)->identifier->parent_id_token.has_value()) {
                provided_token.parent_id_token =
                    this->evses.at(evse_used_for_transaction)->identifier->parent_id_token.value();
            }
            provided_token.connectors = std::vector<int32_t>{evse_used_for_transaction};
            return TokenHandlingResult::USED_TO_STOP_TRANSACTION;
        }
    }

    /** Check if validation of token shall be requested. In some situations its not useful to validate
     * the token because either no evse is available anyways or the provided token does not match a present
     * reservation. Yet it has to be checked if the incoming token can be used to stop an active transaction or if the
     * parent id of the token (that is only known after validation) can be used to stop or start transactions
     */

    /* If no evse is available AND no parent_id is deposited at any evse and no master pass group id is
    configured, we can immediately respond with NO_CONNECTOR_AVAILABLE */
    if (!this->any_evse_available(referenced_evses) and !this->any_parent_id_present(referenced_evses) and
        !this->master_pass_group_id.has_value()) {
        return TokenHandlingResult::NO_CONNECTOR_AVAILABLE;
    }

    /* If all evses are reserved and the given identifier doesnt match any reserved identifier and no parent id is
     * deposited for a reservation, we can immediately respond with NO_CONNECTOR_AVAILABLE */
    bool all_evses_reserved_and_tag_does_not_match = true;
    for (const auto evse_id : referenced_evses) {
        if (evse_id < 0) {
            EVLOG_warning << "Handle token: Evse id is negative: that should not be possible.";
            continue;
        }

        const uint32_t evse_id_u = static_cast<uint32_t>(evse_id);

        if (!this->reservation_handler.is_evse_reserved(evse_id_u) &&
            this->reservation_handler.is_charging_possible(evse_id_u)) {
            all_evses_reserved_and_tag_does_not_match = false;
            break;
        }

        const std::optional<int32_t> reservation_id = this->reservation_handler.matches_reserved_identifier(
            provided_token.id_token.value, evse_id_u, std::nullopt);

        if (reservation_id.has_value()) {
            all_evses_reserved_and_tag_does_not_match = false;
            break;
        }
        if (this->reservation_handler.has_reservation_parent_id(evse_id_u)) {
            all_evses_reserved_and_tag_does_not_match = false;
            break;
        }
    }

    if (all_evses_reserved_and_tag_does_not_match) {
        return TokenHandlingResult::NO_CONNECTOR_AVAILABLE;
    }

    // Validate the provided token using the available validators
    std::vector<ValidationResult> validation_results;
    // only validate if token is not prevalidated
    if (provided_token.prevalidated && provided_token.prevalidated.value()) {
        ValidationResult validation_result;
        validation_result.authorization_status = AuthorizationStatus::Accepted;
        validation_result.parent_id_token = provided_token.parent_id_token;
        validation_results.push_back(validation_result);
    } else {
        // release event_mutex before potentially blocking callback(s) via MQTT
        // validate_token_callback does not touch any shared state
        lk.unlock();
        validation_results = this->validate_token_callback(provided_token);
        lk.lock();
    }

    bool attempt_stop_with_parent_id_token = false;
    if (this->prioritize_authorization_over_stopping_transaction) {
        // check if any evse is available
        if (!this->any_evse_available(referenced_evses)) {
            // check if parent_id_token can be used to finish transaction
            attempt_stop_with_parent_id_token = true;
        }
    } else {
        attempt_stop_with_parent_id_token = true;
    }

    if (attempt_stop_with_parent_id_token) {
        for (const auto& validation_result : validation_results) {
            if (validation_result.authorization_status == AuthorizationStatus::Accepted &&
                validation_result.parent_id_token.has_value()) {
                // check if parent_id_token is equal to master_pass_group_id
                if (this->equals_master_pass_group_id(validation_result.parent_id_token)) {
                    EVLOG_info << "Provided parent_id_token is equal to master_pass_group_id. Stopping all active "
                                  "transactions!";
                    std::vector<int32_t> connectors;
                    for (const auto evse_id : referenced_evses) {
                        if (this->evses[evse_id]->transaction_active) {
                            StopTransactionRequest req;
                            req.reason = StopTransactionReason::MasterPass;
                            req.id_tag.emplace(provided_token);
                            this->stop_transaction_callback(this->evses.at(evse_id)->evse_index, req);
                            connectors.emplace_back(evse_id);
                        }
                    }
                    // TOOD: Add handling in case there is a display which can be used which transaction should stop
                    // (see C16 of OCPP2.0.1 spec)
                    provided_token.parent_id_token = validation_result.parent_id_token.value();
                    provided_token.connectors = connectors;
                    return TokenHandlingResult::USED_TO_STOP_TRANSACTION;
                }

                const auto evse_used_for_transaction =
                    this->used_for_transaction(referenced_evses, validation_result.parent_id_token.value().value);
                if (evse_used_for_transaction != -1) {
                    provided_token.connectors = std::vector<int32_t>{evse_used_for_transaction};
                    if (!this->evses[evse_used_for_transaction]->transaction_active) {
                        return TokenHandlingResult::ALREADY_IN_PROCESS;
                    } else {
                        StopTransactionRequest req;
                        req.reason = StopTransactionReason::Local;
                        req.id_tag.emplace(provided_token);
                        this->stop_transaction_callback(this->evses.at(evse_used_for_transaction)->evse_index, req);
                        EVLOG_info << "Transaction was stopped because parent_id_token was used for transaction";
                        provided_token.parent_id_token = validation_result.parent_id_token.value();
                        return TokenHandlingResult::USED_TO_STOP_TRANSACTION;
                    }
                }
            }
        }
    }

    // check if any evse is available
    if (!this->any_evse_available(referenced_evses)) {
        return TokenHandlingResult::NO_CONNECTOR_AVAILABLE;
    }

    // We can remove evse_ids from referenced_evses that already have an identifier assigend, since we don't want to
    // consider those when selecting an evse
    referenced_evses.erase(
        std::remove_if(referenced_evses.begin(), referenced_evses.end(),
                       [this](int32_t evse_id) { return this->evses.at(evse_id)->identifier != std::nullopt; }),
        referenced_evses.end());

    types::authorization::ValidationResult validation_result = {types::authorization::AuthorizationStatus::Unknown};
    if (!validation_results.empty()) {
        bool authorized = false;
        std::vector<ValidationResult>::size_type i = 0;
        // iterate over validation results
        while (i < validation_results.size() && !authorized && !referenced_evses.empty()) {
            validation_result = validation_results.at(i);
            if (validation_result.authorization_status == AuthorizationStatus::Accepted) {

                if (this->equals_master_pass_group_id(validation_result.parent_id_token)) {
                    EVLOG_info << "parent_id_token of validation result is equal to master_pass_group_id. Not allowed "
                                  "to authorize this token for starting transactions!";
                    return TokenHandlingResult::REJECTED;
                }

                if (validation_result.parent_id_token.has_value()) {
                    provided_token.parent_id_token = validation_result.parent_id_token.value();
                }
                this->publish_token_validation_status_callback(provided_token,
                                                               types::authorization::TokenValidationStatus::Accepted);
                /* although validator accepts the authorization request, the Auth module still needs to
                    - select the evse for the authorization request
                    - process it against placed reservations
                    - compare referenced_evses against the evses listed in the validation_result
                    - check if request has been withdrawn while selecting an evse
                */
                const auto select_evse_result =
                    this->select_evse(referenced_evses, provided_token.id_token, lk); // might block

                if (not select_evse_result.evse_id.has_value()) {
                    if (select_evse_result.status == SelectEvseReturnStatus::TimeOut) {
                        return TokenHandlingResult::TIMEOUT;
                    } else if (select_evse_result.status == SelectEvseReturnStatus::Interrupted) {
                        return TokenHandlingResult::WITHDRAWN;
                    }
                }

                int evse_id = select_evse_result.evse_id.value();
                EVLOG_debug << "Selected evse#" << evse_id
                            << " for token: " << everest::helpers::redact(provided_token.id_token.value);
                std::optional<std::string> parent_id_token;
                if (validation_result.parent_id_token.has_value()) {
                    parent_id_token = validation_result.parent_id_token.value().value;
                }
                const std::optional<int32_t> reservation_id = this->reservation_handler.matches_reserved_identifier(
                    provided_token.id_token.value, static_cast<uint32_t>(evse_id), parent_id_token);

                if (validation_result.evse_ids.has_value() and
                    intersect(referenced_evses, validation_result.evse_ids.value()).empty()) {
                    EVLOG_debug << "Empty intersection between referenced evses and evses that are authorized";
                    validation_result.authorization_status = AuthorizationStatus::NotAtThisLocation;
                } else if (reservation_id == std::nullopt &&
                           !this->reservation_handler.is_charging_possible(static_cast<uint32_t>(evse_id))) {
                    validation_result.authorization_status = AuthorizationStatus::NotAtThisTime;
                } else if (!this->reservation_handler.is_evse_reserved(static_cast<uint32_t>(evse_id)) &&
                           (reservation_id == std::nullopt)) {
                    EVLOG_info << "Providing authorization to evse#" << evse_id;
                    authorized = true;
                    provided_token.connectors = std::vector<int32_t>{evse_id};
                } else {
                    EVLOG_debug << "Evse is reserved. Checking if token matches...";

                    if (reservation_id.has_value()) {
                        EVLOG_info << "Evse#" << evse_id << " is reserved and token is valid for this reservation";
                        this->reservation_handler.on_reservation_used(reservation_id.value());
                        authorized = true;
                        provided_token.connectors = std::vector<int32_t>{evse_id};
                        validation_result.reservation_id = reservation_id.value();
                    } else {
                        EVLOG_info << "Evse#" << evse_id << " is reserved but token is not valid for this reservation";
                        validation_result.authorization_status = AuthorizationStatus::NotAtThisTime;
                    }
                }
                this->notify_evse(evse_id, provided_token, validation_result, lk);
            }
            i++;
        }
        if (authorized) {
            return TokenHandlingResult::USED_TO_START_TRANSACTION;
        } else {
            EVLOG_debug << "id_token could not be validated by any validator";
            // in case the validation was not successful, we need to notify the evse and transmit the validation result.
            // This is especially required for Plug&Charge with ISO15118 in order to allow the ISO15118 state machine to
            // escape the Authorize loop. We do this for all evses that were referenced
            if (provided_token.connectors.has_value()) {
                const auto connectors = provided_token.connectors.value();
                std::for_each(connectors.begin(), connectors.end(),
                              [this, provided_token, validation_result, &lk](int32_t connector) {
                                  this->notify_evse(connector, provided_token, validation_result, lk);
                              });
            }
            return TokenHandlingResult::REJECTED;
        }
    } else {
        EVLOG_warning << "No validation result was received by any validator.";
        return TokenHandlingResult::REJECTED;
    }
}

std::vector<int> AuthHandler::get_referenced_evses(const ProvidedIdToken& provided_token) {
    std::vector<int> evse_ids;

    // either insert the given connector references of the provided token
    if (provided_token.connectors) {
        std::copy_if(provided_token.connectors.value().begin(), provided_token.connectors.value().end(),
                     std::back_inserter(evse_ids), [this](int evse_id) {
                         if (this->evses.find(evse_id) != this->evses.end()) {
                             return !this->evses.at(evse_id)->is_unavailable();
                         } else {
                             EVLOG_warning << "Provided token included references to evse_id that does not exist";
                             return false;
                         }
                     });
    }
    // or if there is no reference to connectors take all connectors
    else {
        for (const auto& entry : this->evses) {
            if (!entry.second->is_unavailable()) {
                evse_ids.push_back(entry.first);
            }
        }
    }
    return evse_ids;
}

int AuthHandler::used_for_transaction(const std::vector<int>& evse_ids, const std::string& token) {
    for (const auto evse_id : evse_ids) {
        if (this->evses.at(evse_id)->identifier.has_value()) {
            const auto& identifier = this->evses.at(evse_id)->identifier.value();
            // check against id_token
            if (is_equal_case_insensitive(identifier.id_token.value, token)) {
                return evse_id;
            }
            // check against parent_id_token
            else if (identifier.parent_id_token.has_value() &&
                     is_equal_case_insensitive(identifier.parent_id_token.value().value, token)) {
                return evse_id;
            }
        }
    }
    return -1;
}

bool AuthHandler::is_token_already_in_process(const ProvidedIdToken& provided_id_token,
                                              const std::vector<int>& referenced_evses) {

    // checks if the token is currently already processed by the module (because already swiped)
    if (this->tokens_in_process.find(provided_id_token) != this->tokens_in_process.end()) {
        return true;
    } else {
        // check if id_token was already used to authorize evse but no transaction has been started yet
        for (const auto evse_id : referenced_evses) {
            const auto& evse = this->evses.at(evse_id);
            if (evse->identifier.has_value() &&
                is_equal_case_insensitive(evse->identifier.value().id_token, provided_id_token.id_token) &&
                !evse->transaction_active) {
                return true;
            }
        }
    }
    return false;
}

bool AuthHandler::any_evse_available(const std::vector<int>& evse_ids) {
    EVLOG_debug << "Checking availability of evses...";
    for (const auto evse_id : evse_ids) {
        if (this->evses.at(evse_id)->is_available()) {
            EVLOG_debug << "There is at least one evse available";
            return true;
        }
    }
    EVLOG_debug << "No evse is available for this id_token";
    return false;
}

bool AuthHandler::any_parent_id_present(const std::vector<int>& evse_ids) {
    for (const auto evse_id : evse_ids) {
        if (this->evses.at(evse_id)->identifier.has_value() and
            this->evses.at(evse_id)->identifier.value().parent_id_token.has_value()) {
            EVLOG_debug << "Parent id is currently present";
            return true;
        }
    }
    EVLOG_debug << "No parent id is currently present";
    return false;
}

bool AuthHandler::equals_master_pass_group_id(const std::optional<types::authorization::IdToken> parent_id_token) {
    if (!this->master_pass_group_id.has_value()) {
        return false;
    }

    if (!parent_id_token.has_value()) {
        return false;
    }

    return is_equal_case_insensitive(parent_id_token.value().value, this->master_pass_group_id.value());
}

int AuthHandler::get_latest_plugin(const std::vector<int>& evse_ids) {
    for (const auto evse_id : this->plug_in_queue) {
        if (std::find(evse_ids.begin(), evse_ids.end(), evse_id) != evse_ids.end()) {
            return evse_id;
        }
    }
    return -1;
}

AuthHandler::SelectEvseResult AuthHandler::select_evse(const std::vector<int>& selected_evses, const IdToken& id_token,
                                                       std::unique_lock<std::mutex>& lk) {
    SelectEvseResult result;

    if (selected_evses.size() == 1) {
        result.status = SelectEvseReturnStatus::EvseSelected;
        result.evse_id = selected_evses.at(0);
        return result;
    }

    if (this->selection_algorithm == SelectionAlgorithm::PlugEvents) {
        // locks all referenced evses for this request. Subsequent requests referencing one or more of the locked
        // evses are blocked until handle_token returns
        if (this->get_latest_plugin(selected_evses) == -1) {
            // no EV has been plugged in yet at the referenced evses
            EVLOG_debug << "No evse in authorization queue. Waiting for a plug in...";
            // blocks until respective plugin for evse occurred or until timeout
            if (!this->cv.wait_for(lk, std::chrono::seconds(this->connection_timeout),
                                   [this, selected_evses, id_token] {
                                       return this->get_latest_plugin(selected_evses) != -1 ||
                                              this->is_authorization_withdrawn(selected_evses, id_token);
                                   })) {
                result.status = SelectEvseReturnStatus::TimeOut;
                return result;
            }
            EVLOG_debug << "Plug in at evse occured or authorization withdrawn";
        }

        if (this->is_authorization_withdrawn(selected_evses, id_token)) {
            result.status = SelectEvseReturnStatus::Interrupted;
        } else {
            result.status = SelectEvseReturnStatus::EvseSelected;
            result.evse_id = this->get_latest_plugin(selected_evses);
        }

        return result;
    } else if (this->selection_algorithm == SelectionAlgorithm::FindFirst) {
        EVLOG_debug << "SelectionAlgorithm FindFirst: Selecting first available evse without an active transaction";
        const auto selected_evse_id = this->get_latest_plugin(selected_evses);
        if (selected_evse_id != -1 and !this->evses.at(selected_evse_id)->transaction_active) {
            // an EV has been plugged in yet at the referenced evses
            result.status = SelectEvseReturnStatus::EvseSelected;
            result.evse_id = this->get_latest_plugin(selected_evses);
            return result;
        } else {
            // no EV has been plugged in yet at the referenced evses; choosing the first one where no
            // transaction is active
            for (const auto& evse_id : selected_evses) {
                const auto& evse = this->evses.at(evse_id);
                if (!evse->transaction_active) {
                    result.status = SelectEvseReturnStatus::EvseSelected;
                    result.evse_id = evse_id;
                    return result;
                }
            }
        }
        result.status = SelectEvseReturnStatus::TimeOut;
        return result;
    } else {
        throw std::runtime_error("SelectionAlgorithm not implemented: " +
                                 selection_algorithm_to_string(this->selection_algorithm));
    }
}

/// Checks if given \p withdraw_request matches given \p id_token and/or one evse of given \p selected_evses
bool does_withdraw_request_match(const WithdrawAuthorizationRequest& withdraw_request,
                                 const std::vector<int>& selected_evses, const IdToken& id_token) {
    // Check if the withdraw request is specific to an EVSE
    const bool has_evse_id = withdraw_request.evse_id.has_value();
    const bool is_evse_in_selected =
        has_evse_id and (std::find(selected_evses.begin(), selected_evses.end(), withdraw_request.evse_id.value()) !=
                         selected_evses.end());

    // Check if the ID token matches or is absent
    const bool id_token_matches = not withdraw_request.id_token.has_value() or
                                  (is_equal_case_insensitive(withdraw_request.id_token.value(), id_token));

    return id_token_matches and (not has_evse_id or is_evse_in_selected);
}

bool AuthHandler::is_authorization_withdrawn(const std::vector<int>& selected_evses, const IdToken& id_token) {
    if (withdraw_request == nullptr) {
        return false;
    }
    return does_withdraw_request_match(*this->withdraw_request, selected_evses, id_token);
}

void AuthHandler::notify_evse(int evse_id, const ProvidedIdToken& provided_token,
                              const ValidationResult& validation_result, std::unique_lock<std::mutex>& lk) {
    const auto evse_index = this->evses.at(evse_id)->evse_index;

    if (validation_result.authorization_status == AuthorizationStatus::Accepted) {
        Identifier identifier{provided_token.id_token, provided_token.authorization_type,
                              validation_result.authorization_status, validation_result.expiry_time,
                              validation_result.parent_id_token};
        this->evses.at(evse_id)->identifier.emplace(identifier);

        // wait for potentially running timeout task to finish execution
        this->cv.wait(lk, [&evse = this->evses.at(evse_id)]() { return !evse->timeout_in_progress.load(); });

        this->evses.at(evse_id)->timeout_timer.stop();
        this->evses.at(evse_id)->timeout_timer.timeout(
            [this, evse_index, &evse = this->evses.at(evse_id), provided_token]() {
                evse->timeout_in_progress = true;
                std::lock_guard<std::mutex> lk(this->event_mutex);
                EVLOG_debug << "Authorization timeout for evse#" << evse_index;
                evse->identifier.reset();
                this->withdraw_authorization_callback(evse_index);
                this->publish_token_validation_status_callback(provided_token, TokenValidationStatus::TimedOut);
                evse->timeout_in_progress = false;
                this->cv.notify_all();
            },
            std::chrono::seconds(this->connection_timeout));
        this->plug_in_queue.remove_if([evse_id](int value) { return value == evse_id; });
    }

    this->notify_evse_callback(evse_index, provided_token, validation_result);
}

types::reservation::ReservationResult AuthHandler::handle_reservation(const Reservation& reservation) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    std::optional<uint32_t> evse;
    if (reservation.evse_id.has_value()) {
        if (reservation.evse_id.value() >= 0) {
            evse = static_cast<uint32_t>(reservation.evse_id.value());
        }
    }

    return reservation_handler.make_reservation(evse, reservation);
}

std::pair<bool, std::optional<int32_t>> AuthHandler::handle_cancel_reservation(const int32_t reservation_id) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    std::pair<bool, std::optional<uint32_t>> reservation_cancelled = this->reservation_handler.cancel_reservation(
        reservation_id, false, types::reservation::ReservationEndReason::Cancelled);

    if (reservation_cancelled.first) {
        if (reservation_cancelled.second.has_value()) {
            return {true, static_cast<int32_t>(reservation_cancelled.second.value())};
        }
        return {true, std::nullopt};
    }

    return {false, std::nullopt};
}

ReservationCheckStatus AuthHandler::handle_reservation_exists(std::string& id_token, const std::optional<int>& evse_id,
                                                              std::optional<std::string>& group_id_token) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    // Evse id has no value.
    std::optional<int32_t> reservation_id =
        this->reservation_handler.matches_reserved_identifier(id_token, evse_id, group_id_token);

    if (!evse_id.has_value()) {
        if (reservation_id.has_value()) {
            return ReservationCheckStatus::ReservedForToken;
        }

        return ReservationCheckStatus::NotReserved;
    }

    // Evse id has a value.
    if (!this->reservation_handler.is_evse_reserved(evse_id.has_value())) {
        // There is an evse id, but the evse is not reserved.
        return ReservationCheckStatus::NotReserved;
    }

    if (reservation_id.has_value()) {
        // There is an evse id and the reservation is for the given token.
        return ReservationCheckStatus::ReservedForToken;
    }

    // Evse is reserved. No reservation for the given id_token. But there is also the group id token, let's do some
    // checks here.
    if (!group_id_token.has_value()) {
        if (reservation_handler.has_reservation_parent_id(evse_id)) {
            // Group id token has no value, but the reservation for this evse has a parent token. It might be that
            // this token will be checked later.
            return ReservationCheckStatus::ReservedForOtherTokenAndHasParentToken;
        }

        // Group id token has no value and the reservation for this evse has no parent token.
        return ReservationCheckStatus::ReservedForOtherToken;
    }

    // Group id token has a value but it is not valid for this reservation
    return ReservationCheckStatus::ReservedForOtherToken;
}

bool AuthHandler::call_reserved(const int reservation_id, const std::optional<int>& evse_id) {
    const bool reserved = this->reserved_callback(evse_id, reservation_id);
    if (reserved) {
        this->check_evse_reserved_and_send_updates();
    }

    return reserved;
}

void AuthHandler::call_reservation_cancelled(const int32_t reservation_id,
                                             const types::reservation::ReservationEndReason reason,
                                             const std::optional<int>& evse_id, const bool send_reservation_update) {
    std::optional<int32_t> evse_index;
    if (evse_id.has_value() && evse_id.value() > 0) {
        EVLOG_info << "Cancel reservation for evse id " << evse_id.value();
    }

    this->reservation_cancelled_callback(evse_id, reservation_id, reason, send_reservation_update);

    // If this was a global reservation or there are global reservations which made evse's go to reserved because of
    // this reservation, they should be cancelled.
    this->check_evse_reserved_and_send_updates();
}

void AuthHandler::handle_permanent_fault_raised(const int evse_id, const int32_t connector_id) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    if (not ignore_faults) {
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::FAULTED);
    }
}

void AuthHandler::handle_permanent_fault_cleared(const int evse_id, const int32_t connector_id) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    if (not ignore_faults) {
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::ERROR_CLEARED);
    }
}

void AuthHandler::handle_session_event(const int evse_id, const SessionEvent& event) {
    std::unique_lock<std::mutex> lk(this->event_mutex);
    // When connector id is not specified, it is assumed to be '1'.
    const int32_t connector_id = event.connector_id.value_or(1);
    if (evse_id <= 0) {
        EVLOG_error << "Handle session event: Evse id is <= 0: That should not be possible.";
        return;
    }

    if (connector_id <= 0) {
        EVLOG_error << "Handle session event: connector id is <= 0: That should not be possible.";
        return;
    }

    if (this->evses.count(evse_id) == 0) {
        EVLOG_warning << "Handle session event: no evse found with evse id " << evse_id;
        return;
    }

    const auto event_type = event.event;
    bool check_reservations = false;

    switch (event_type) {
    case SessionEventEnum::SessionStarted: {

        // only set plug in timeout when SessionStart is caused by plug in
        if (event.session_started.value().reason == StartSessionReason::EVConnected) {
            this->plug_in_queue.push_back(evse_id);
            this->cv.notify_all();
            this->evses.at(evse_id)->plugged_in = true;
            EVLOG_info << "Plug In event for evse#" << evse_id << ", starting auth";

            this->evses.at(evse_id)->timeout_timer.timeout(
                [this, evse_id]() {
                    this->evses.at(evse_id)->timeout_in_progress = true;
                    std::lock_guard<std::mutex> lk(this->event_mutex);

                    EVLOG_info << "Plug In timeout for evse#" << evse_id << ". Replug required for this EVSE";
                    this->withdraw_authorization_callback(this->evses.at(evse_id)->evse_index);

                    this->plug_in_queue.remove_if([evse_id](int value) { return value == evse_id; });
                    this->evses.at(evse_id)->plug_in_timeout = true;
                    this->evses.at(evse_id)->timeout_in_progress = false;
                    this->cv.notify_all();
                },
                std::chrono::seconds(this->connection_timeout));
        }
    } break;
    case SessionEventEnum::TransactionStarted: {
        this->evses.at(evse_id)->plugged_in = true;
        this->evses.at(evse_id)->transaction_active = true;
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::TRANSACTION_STARTED);
        // wait for potentially running timeout task to finish execution
        this->cv.wait(lk, [&evse = this->evses.at(evse_id)]() { return !evse->timeout_in_progress.load(); });
        this->evses.at(evse_id)->timeout_timer.stop();
        check_reservations = true;
        break;
    }
    case SessionEventEnum::TransactionFinished:
        this->evses.at(evse_id)->transaction_active = false;
        this->evses.at(evse_id)->identifier.reset();
        break;
    case SessionEventEnum::SessionFinished: {
        this->evses.at(evse_id)->plugged_in = false;
        this->evses.at(evse_id)->plug_in_timeout = false;
        this->evses.at(evse_id)->identifier.reset();
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::SESSION_FINISHED);
        // wait for potentially running timeout task to finish execution
        this->cv.wait(lk, [&evse = this->evses.at(evse_id)]() { return !evse->timeout_in_progress.load(); });
        this->evses.at(evse_id)->timeout_timer.stop();
        this->plug_in_queue.remove_if([evse_id](int value) { return value == evse_id; });
        check_reservations = true;
        break;
    }
    case SessionEventEnum::Disabled:
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::DISABLE);
        check_reservations = true;
        break;
    case SessionEventEnum::Enabled:
        this->submit_event_for_connector(evse_id, connector_id, ConnectorEvent::ENABLE);
        check_reservations = true;
        break;
    case SessionEventEnum::Deauthorized:
        this->evses.at(evse_id)->identifier.reset();
        // wait for potentially running timeout task to finish execution
        this->cv.wait(lk, [&evse = this->evses.at(evse_id)]() { return !evse->timeout_in_progress.load(); });
        this->evses.at(evse_id)->timeout_timer.stop();
        break;
    case SessionEventEnum::ReservationStart:
        break;
    case SessionEventEnum::ReservationEnd: {
        if (reservation_handler.is_evse_reserved(evse_id)) {
            reservation_handler.cancel_reservation(evse_id, true);
        }
        break;
    }
    /// explicitly fall through all the SessionEventEnum values we are not handling
    case SessionEventEnum::Authorized:
        [[fallthrough]];
    case SessionEventEnum::AuthRequired:
        [[fallthrough]];
    case SessionEventEnum::PrepareCharging:
        [[fallthrough]];
    case SessionEventEnum::ChargingStarted:
        [[fallthrough]];
    case SessionEventEnum::ChargingPausedEV:
        [[fallthrough]];
    case SessionEventEnum::ChargingPausedEVSE:
        [[fallthrough]];
    case SessionEventEnum::WaitingForEnergy:
        [[fallthrough]];
    case SessionEventEnum::ChargingResumed:
        [[fallthrough]];
    case SessionEventEnum::StoppingCharging:
        [[fallthrough]];
    case SessionEventEnum::ChargingFinished:
        [[fallthrough]];
    case SessionEventEnum::ReplugStarted:
        [[fallthrough]];
    case SessionEventEnum::ReplugFinished:
        [[fallthrough]];
    case SessionEventEnum::PluginTimeout:
        [[fallthrough]];
    case SessionEventEnum::SwitchingPhases:
        [[fallthrough]];
    case SessionEventEnum::SessionResumed:
        break;
    }

    // When reservation is started or ended, check if the number of reservations match the number of evses and
    // send 'reserved' notifications to the evse manager accordingly if needed.
    if (check_reservations) {
        this->check_evse_reserved_and_send_updates();
    }
}

void AuthHandler::set_connection_timeout(const int connection_timeout) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    this->connection_timeout = connection_timeout;
};

void AuthHandler::set_master_pass_group_id(const std::string& master_pass_group_id) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    if (master_pass_group_id.empty()) {
        this->master_pass_group_id = std::nullopt;
    } else {
        this->master_pass_group_id = master_pass_group_id;
    }
}

void AuthHandler::set_prioritize_authorization_over_stopping_transaction(bool b) {
    std::lock_guard<std::mutex> lk(this->event_mutex);
    this->prioritize_authorization_over_stopping_transaction = b;
}

void AuthHandler::register_notify_evse_callback(
    const std::function<void(const int evse_index, const ProvidedIdToken& provided_token,
                             const ValidationResult& validation_result)>& callback) {
    this->notify_evse_callback = callback;
}

void AuthHandler::register_withdraw_authorization_callback(const std::function<void(const int evse_index)>& callback) {
    this->withdraw_authorization_callback = callback;
}
void AuthHandler::register_validate_token_callback(
    const std::function<std::vector<ValidationResult>(const ProvidedIdToken& provided_token)>& callback) {
    this->validate_token_callback = callback;
}
void AuthHandler::register_stop_transaction_callback(
    const std::function<void(const int evse_index, const StopTransactionRequest& request)>& callback) {
    this->stop_transaction_callback = callback;
}

void AuthHandler::register_reserved_callback(
    const std::function<bool(const std::optional<int>& evse_id, const int& reservation_id)>& callback) {
    this->reserved_callback = callback;
}

void AuthHandler::register_reservation_cancelled_callback(
    const std::function<void(const std::optional<int32_t>& evse_id, const int32_t reservation_id,
                             const ReservationEndReason reason, const bool send_reservation_update)>& callback) {
    this->reservation_cancelled_callback = callback;
    this->reservation_handler.register_reservation_cancelled_callback(
        [this](const std::optional<int32_t>& evse_id, const int32_t reservation_id,
               const types::reservation::ReservationEndReason reason, const bool send_reservation_update) {
            if (evse_id.has_value() && evse_id.value() < 0) {
                EVLOG_warning << "Reservation cancelled: evse id is negative (" << evse_id.value()
                              << "), that should not be possible.";
                return;
            }

            this->call_reservation_cancelled(reservation_id, reason, evse_id, send_reservation_update);
        });
}

void AuthHandler::register_publish_token_validation_status_callback(
    const std::function<void(const ProvidedIdToken&, TokenValidationStatus)>& callback) {
    this->publish_token_validation_status_callback = callback;
}

WithdrawAuthorizationResult AuthHandler::handle_withdraw_authorization(const WithdrawAuthorizationRequest& request) {
    std::lock_guard<std::mutex> lg(this->withdraw_mutex);
    EVLOG_info << "Witdrawing authorization"
               << (request.evse_id.has_value() ? " evse: " + std::to_string(request.evse_id.value()) : "")
               << (request.id_token.has_value() ? " id token: " + request.id_token.value().value : "");

    if (request.evse_id.has_value() and this->evses.find(request.evse_id.value()) == this->evses.end()) {
        return WithdrawAuthorizationResult::EvseNotFound;
    }

    auto result = WithdrawAuthorizationResult::AuthorizationNotFound; // default

    // Wait for processing threads to finish executing
    std::unique_lock lock(this->event_mutex);

    const auto is_withdraw_request_targeting_token_in_process = [this](const WithdrawAuthorizationRequest& request) {
        for (const auto& token_in_process : this->tokens_in_process) {
            auto referenced_evses = this->get_referenced_evses(token_in_process);
            if (does_withdraw_request_match(request, referenced_evses, token_in_process.id_token)) {
                return true;
            }
        }
        return false;
    };

    this->withdraw_request = std::make_unique<WithdrawAuthorizationRequest>(request);

    if (is_withdraw_request_targeting_token_in_process(request)) {
        // Notify processing threads that wait within select_evse
        // This will interrupt the wait for a plug in in case the authorization is withdrawn by this request
        this->cv.notify_all();

        // Release the event_mutex lock and wait for threads to finish...
        this->processing_finished_cv.wait(
            lock, [&]() { return not is_withdraw_request_targeting_token_in_process(request); });

        result = WithdrawAuthorizationResult::Accepted;
    }

    // It might still be the case that the withdraw request is also targeting already granted authorization so we
    // continue checking for this

    const auto withdraw_authorization_or_stop_transaction = [this](const EVSEContext& evse) {
        if (evse.transaction_active) {
            StopTransactionRequest req;
            req.reason = StopTransactionReason::DeAuthorized;
            this->stop_transaction_callback(evse.evse_index, req);
        } else {
            this->withdraw_authorization_callback(evse.evse_index);
        }
    };

    if (request.evse_id.has_value() and request.id_token.has_value()) {
        // evse_id and id_token is specified
        // find if there is a granted authorization for id_token and evse_id
        const auto evse_id = request.evse_id.value();
        const auto id_token = request.id_token.value();
        const auto& evse = this->evses.at(evse_id);
        if (evse->identifier.has_value() &&
            is_equal_case_insensitive(request.id_token.value(), evse->identifier.value().id_token)) {
            withdraw_authorization_or_stop_transaction(*evse);
            result = WithdrawAuthorizationResult::Accepted;
        }

    } else if (request.evse_id.has_value()) {
        // only evse_id is specified
        // find if there is a granted authorization for evse_id
        const auto evse_id = request.evse_id.value();
        const auto& evse = this->evses.at(evse_id);
        if (evse->identifier.has_value()) {
            withdraw_authorization_or_stop_transaction(*evse);
            result = WithdrawAuthorizationResult::Accepted;
        }
    } else if (request.id_token.has_value()) {
        // only id_token is specified
        // find if there is a granted authorization for id_token
        for (const auto& evse : this->evses) {
            if (evse.second->identifier.has_value() &&
                is_equal_case_insensitive(request.id_token.value(), evse.second->identifier.value().id_token)) {
                withdraw_authorization_or_stop_transaction(*evse.second);
                result = WithdrawAuthorizationResult::Accepted;
            }
        }
    } else {
        // neither evse_id nor id_token is specified, withdraw all authorizations that have been granted
        for (const auto& evse : this->evses) {
            if (evse.second->identifier.has_value()) {
                withdraw_authorization_or_stop_transaction(*evse.second);
                result = WithdrawAuthorizationResult::Accepted;
            }
        }
    }

    // result was either set in one of the if statements above or is still AuthorizationNotFound
    return result;
}

void AuthHandler::submit_event_for_connector(const int32_t evse_id, const int32_t connector_id,
                                             const ConnectorEvent connector_event) {
    for (auto& connector : this->evses.at(evse_id)->connectors) {
        if (connector.id == connector_id) {
            connector.submit_event(connector_event);
            this->reservation_handler.on_connector_state_changed(connector.get_state(), evse_id, connector_id);
            break;
        }
    }
}

void AuthHandler::check_evse_reserved_and_send_updates() {
    ReservationEvseStatus reservation_status =
        this->reservation_handler.check_number_global_reservations_match_number_available_evses();
    for (const auto& available_evse : reservation_status.available) {
        EVLOG_debug << "Evse " << available_evse << " is now available";
        this->reservation_cancelled_callback(
            available_evse, -1, types::reservation::ReservationEndReason::GlobalReservationRequirementDropped, false);
    }

    for (const auto& reserved_evse : reservation_status.reserved) {
        EVLOG_debug << "Evse " << reserved_evse << " is now reserved";
        if (this->reserved_callback != nullptr) {
            const bool reserved = this->reserved_callback(reserved_evse, -1);
            if (!reserved) {
                EVLOG_warning << "Could not reserve " << reserved_evse << " for non evse specific reservations";
            }
        }
    }
}

} // namespace module
