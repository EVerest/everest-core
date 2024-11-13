// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _AUTH_HANDLER_HPP_
#define _AUTH_HANDLER_HPP_

#include <condition_variable>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>

#include <utils/types.hpp>

#include <generated/types/authorization.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/reservation.hpp>

#include <Connector.hpp>
#include <ReservationHandler.hpp>

using namespace types::evse_manager;
using namespace types::authorization;
using namespace types::reservation;

namespace module {

enum class TokenHandlingResult {
    ACCEPTED,
    ALREADY_IN_PROCESS,
    REJECTED,
    USED_TO_STOP_TRANSACTION,
    TIMEOUT,
    NO_CONNECTOR_AVAILABLE
};

namespace conversions {
std::string token_handling_result_to_string(const TokenHandlingResult& result);
} // namespace conversions

/**
 * @brief This class handles authorization and reservation requests. It keeps track of the state of each connector and
 * validates incoming token and reservation requests accordingly.
 *
 */
class AuthHandler {

public:
    AuthHandler(const SelectionAlgorithm& selection_algorithm, const int connection_timeout,
                bool prioritize_authorization_over_stopping_transaction, bool ignore_connector_faults,
                const std::string& id, kvsIntf* store);
    virtual ~AuthHandler();

    /**
     * @brief Initializes the evse with the given \p connectors and the given \p evse_id . It instantiates new
     * connector objects and fills data sturctures of the class.
     *
     * @param evse_id
     * @param evse_index
     * @param connectors    The connectors.
     */
    void init_evse(const int evse_id, const int evse_index, const std::vector<Connector>& connectors);

    /**
     * @brief Call when everything is initialized. This will call 'init' of the reservation handler.
     */
    void initialize();

    /**
     * @brief Handler for a new incoming \p provided_token
     *
     * @param provided_token
     */
    TokenHandlingResult on_token(const ProvidedIdToken& provided_token);

    /**
     * @brief Handler for new incoming \p reservation for the given \p connector . Places the reservation if possible.
     *
     * @param reservation
     * @return types::reservation::ReservationResult
     */
    types::reservation::ReservationResult handle_reservation(const Reservation& reservation);

    /**
     * @brief Handler for incoming cancel reservation request for the given \p reservation_id .
     *
     * @param reservation_id
     * @return return value first returns false if the reservation could not been cancelled. Return value second is the
     *         evse id or nullopt if the reservation was a 'global' reservation without evse id.
     */
    std::pair<bool, std::optional<int32_t>> handle_cancel_reservation(const int32_t reservation_id);

    /**
     * @brief Callback to check if there is a reservation for the given token (on the given evse id).
     * @param id_token          The token to check.
     * @param evse_id           The evse to check the reservation for.
     * @param group_id_token    The group id token to check.
     * @return True if there is a reservation for the given token.
     */
    bool handle_reservation_exists(std::string& id_token, const std::optional<int>& evse_id,
                                   std::optional<std::string>& group_id_token);

    /**
     * @brief Callback to signal EvseManager that the given \p connector_id has been reserved with the given \p
     * reservation_id .
     *
     * @param evse_id
     * @param reservation_id
     *
     * @return true of EvseManager accepted the reservation.
     */
    bool call_reserved(const int reservation_id, const std::optional<int>& evse_id);

    /**
     * @brief Callback to signal EvseManager that the reservation for the given \p evse_id has been cancelled.
     *
     * @param reservation_id    The id of the cancelled reservation.
     * @param reason            The reason the reservation was cancelled.
     * @param evse_id           Evse id if reservation was for a specific evse.
     */
    void call_reservation_cancelled(const int32_t reservation_id, const ReservationEndReason reason,
                                    const std::optional<int>& evse_id);

    /**
     * @brief Handler for the given \p events at the given \p connector . Submits events to the state machine of the
     * handler.
     *
     * @param evse_id
     * @param events
     */
    void handle_session_event(const int evse_id, const SessionEvent& events);

    /**
     * @brief Handler for permanent faults from evsemanager that prevents charging
     */
    void handle_permanent_fault_cleared(const int evse_id, const int32_t connector_id);
    void handle_permanent_fault_raised(const int evse_id, const int32_t connector_id);

    /**
     * @brief Set the connection timeout of the handler.
     *
     * @param connection_timeout
     */
    void set_connection_timeout(const int connection_timeout);

    /**
     * @brief Set the master pass group id of the handler.
     *
     * @param master_pass_group_id
     */
    void set_master_pass_group_id(const std::string& master_pass_group_id);

    /**
     * @brief Set the prioritize authorization over stopping transaction flag of the handler.
     *
     * @param b
     */
    void set_prioritize_authorization_over_stopping_transaction(bool b);

    /**
     * @brief Registers the given \p callback to notify the evse about the processed authorization request.
     *
     * @param callback
     */
    void
    register_notify_evse_callback(const std::function<void(const int evse_index, const ProvidedIdToken& provided_token,
                                                           const ValidationResult& validation_result)>& callback);
    /**
     * @brief Registers the given \p callback to withdraw authorization.
     *
     * @param callback
     */
    void register_withdraw_authorization_callback(const std::function<void(const int evse_index)>& callback);

    /**
     * @brief Registers the given \p callback to validate a token.
     *
     * @param callback
     */
    void register_validate_token_callback(
        const std::function<std::vector<ValidationResult>(const ProvidedIdToken& provided_token)>& callback);

    /**
     * @brief Registers the given \p callback to stop a transaction at an EvseManager.
     *
     * @param callback
     */
    void register_stop_transaction_callback(
        const std::function<void(const int evse_index, const StopTransactionRequest& request)>& callback);

    /**
     * @brief Registers the given \p callback to signal a reservation to an EvseManager.
     *
     * @param callback
     */
    void register_reserved_callback(
        const std::function<bool(const std::optional<int>& evse_id, const int& reservation_id)>& callback);

    /**
     * @brief Registers the given \p callback to signal a reservation has been cancelled to the EvseManager.
     *
     * @param callback
     */
    void register_reservation_cancelled_callback(
        const std::function<void(const std::optional<int32_t>& evse_id, const int32_t reservation_id,
                                 const ReservationEndReason reason)>& callback);

    /**
     * @brief Registers the given \p callback to publish the intermediate token validation status.
     *
     * @param callback
     */
    void register_publish_token_validation_status_callback(
        const std::function<void(const ProvidedIdToken&, TokenValidationStatus)>& callback);

private:
    SelectionAlgorithm selection_algorithm;
    int connection_timeout;
    std::optional<std::string> master_pass_group_id;
    bool prioritize_authorization_over_stopping_transaction;
    bool ignore_faults;
    ReservationHandler reservation_handler;

    std::map<int, std::unique_ptr<EVSEContext>> evses;

    std::mutex timer_mutex;
    std::list<int> plug_in_queue;
    std::mutex plug_in_queue_mutex;
    std::mutex plug_in_mutex;
    std::set<std::string> tokens_in_process;
    std::mutex token_in_process_mutex;
    std::condition_variable cv;
    std::recursive_mutex evse_mutex;

    // callbacks
    std::function<void(const int evse_index, const ProvidedIdToken& provided_token,
                       const ValidationResult& validation_result)>
        notify_evse_callback;
    std::function<void(const int evse_index)> withdraw_authorization_callback;
    std::function<std::vector<ValidationResult>(const ProvidedIdToken& provided_token)> validate_token_callback;
    std::function<void(const int evse_index, const StopTransactionRequest& request)> stop_transaction_callback;
    std::function<void(const Array& reservations)> reservation_update_callback;
    std::function<bool(const std::optional<int>& evse_index, const int& reservation_id)> reserved_callback;
    std::function<void(const std::optional<int>& evse_index, const int32_t reservation_id,
                       const types::reservation::ReservationEndReason reason)>
        reservation_cancelled_callback;
    std::function<void(const ProvidedIdToken& token, TokenValidationStatus status)>
        publish_token_validation_status_callback;

    std::vector<int> get_referenced_evses(const ProvidedIdToken& provided_token);
    int used_for_transaction(const std::vector<int>& evse_ids, const std::string& id_token);
    bool is_token_already_in_process(const std::string& id_token, const std::vector<int>& referenced_evses);
    bool any_evse_available(const std::vector<int>& evse_ids);
    bool any_parent_id_present(const std::vector<int>& evse_ids);
    bool equals_master_pass_group_id(const std::optional<types::authorization::IdToken> parent_id_token);

    TokenHandlingResult handle_token(const ProvidedIdToken& provided_token);

    /**
     * @brief Method selects an evse based on the configured selection algorithm. It might block until an event
     * occurs that can be used to determine an evse.
     *
     * @param selected_evses
     * @return int
     */
    int select_evse(const std::vector<int>& selected_evses);

    void lock_plug_in_mutex(const std::vector<int>& evse_ids);
    void unlock_plug_in_mutex(const std::vector<int>& evse_ids);
    int get_latest_plugin(const std::vector<int>& evse_ids);
    void notify_evse(int evse_id, const ProvidedIdToken& provided_token, const ValidationResult& validation_result);
    Identifier get_identifier(const ValidationResult& validation_result, const std::string& id_token,
                              const AuthorizationType& type);
    void submit_event_for_connector(const int32_t evse_id, const int32_t connector_id,
                                    const ConnectorEvent connector_event);
};

} // namespace module

#endif //_AUTH_HANDLER_HPP_
