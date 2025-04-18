#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <vector>

#include <Connector.hpp>
#include <everest/timer.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/reservation.hpp>

class kvsIntf;

namespace module {

struct ReservationEvseStatus {
    std::set<int32_t> reserved;
    std::set<int32_t> available;
};

class ReservationHandler {
private: // Members
    /// \brief Map of EVSE's, with EVSE id as key and the EVSE struct as value.
    std::map<int, std::unique_ptr<module::EVSEContext>>& evses;
    /// \brief Key value store id.
    const std::string kvs_store_key_id;
    /// \brief Key value store for storing reservations.
    kvsIntf* store;
    /// \brief Map of EVSE specific reservations, with EVSE id as key and the Reservation type as value.
    std::map<uint32_t, types::reservation::Reservation> evse_reservations;
    /// \brief All reservations not bound to a specific EVSE.
    std::vector<types::reservation::Reservation> global_reservations;
    /// \brief event mutex, for all timer bound locks (for `reservation_id_to_reservation_timeout_timer_map`)
    mutable std::recursive_mutex event_mutex;
    /// \brief Map with reservations and their timer.
    ///
    /// Every reservation has a specific end time, which is stored in this map. Key is the reservation id. When the
    /// timer expires, it is removed from the map and the reservation is removed from the `evse_reservations` or
    /// `global_reservations`.
    std::map<int, std::unique_ptr<Everest::SteadyTimer>> reservation_id_to_reservation_timeout_timer_map;

    /// \brief The callback that is called when a reservation is cancelled.
    std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                       const types::reservation::ReservationEndReason reason, const bool send_reservation_update)>
        reservation_cancelled_callback;

    std::set<int32_t> last_reserved_status;

    /// \brief worker for the timers.
    boost::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work;
    /// \brief io_context for the worker for the timers.
    boost::asio::io_context io_context;
    /// \brief io context thread for the timers.
    std::thread io_context_thread;

public:
    ///
    /// \brief Constructor.
    ///
    ReservationHandler(std::map<int, std::unique_ptr<module::EVSEContext>>& evses, const std::string& id,
                       kvsIntf* store);

    ///
    /// \brief Destructor.
    ///
    ~ReservationHandler();

    ///
    /// \brief Load reservations from key value store.
    ///
    void load_reservations();

    ///
    /// \brief Try to make a reservation.
    /// \param evse_id      Optional, the evse id. If omitted, a 'global' reservation will be made.
    /// \param reservation  The reservation to make.
    /// \return The result of the reservation (`Accepted` if the reservation could be made).
    ///
    types::reservation::ReservationResult make_reservation(const std::optional<uint32_t> evse_id,
                                                           const types::reservation::Reservation& reservation);

    ///
    /// \brief Change a specific connector state.
    ///
    /// This is important for the reservation handler, to know which connector is in which state, to know if a
    /// reservation can be made or not.
    ///
    /// \param connector_state  The state of the connector.
    /// \param evse_id          The EVSE id the connector belongs to.
    /// \param connector_id     The connector id.
    ///
    void on_connector_state_changed(const ConnectorState connector_state, const uint32_t evse_id,
                                    const uint32_t connector_id);

    ///
    /// \brief Check if charging is possible on a given EVSE.
    ///
    /// If there are multiple global reservations, while a charging station might look available, it is possible that
    /// charging is not possible because then cars that made reservations can not charge anymore.
    ///
    /// Only use this function to check if a car can charge without having a reservation id.
    ///
    /// \param evse_id  The evse on which a car wants to charge.
    /// \return True if charging is possible.
    ///
    bool is_charging_possible(const uint32_t evse_id);

    ///
    /// \brief Check is an EVSE is reserved.
    ///
    /// This only looks at EVSE specific reservations.
    ///
    /// \param evse_id  The evse id to check.
    /// \return True if EVSE is reserved.
    ///
    bool is_evse_reserved(const uint32_t evse_id);

    ///
    /// \brief Cancel a reservation.
    /// \param reservation_id       The id of the reservation to cancel.
    /// \param execute_callback     True if the `reservation_cancelled_callback` must be called.
    /// \param reason               The cancel reason.
    /// \return First: true if reservation could be cancelled.
    ///         Second: The evse id if the reservation to cancel was made for a specific EVSE.
    ///
    std::pair<bool, std::optional<uint32_t>> cancel_reservation(const int reservation_id, const bool execute_callback,
                                                                const types::reservation::ReservationEndReason reason);

    ///
    /// \brief Cancel a reservation.
    /// \param evse_id              The evse id to cancel the reservation for.
    /// \param execute_callback     True if the `reservation_cancelled_callback` must be called.
    /// \return True if the reservation could be cancelled.
    ///
    bool cancel_reservation(const uint32_t evse_id, const bool execute_callback);

    ///
    /// \brief Register reservation cancelled callback.
    /// \param callback The callback that should be called when a reservation is cancelled.
    ///
    void register_reservation_cancelled_callback(
        const std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                                 const types::reservation::ReservationEndReason reason,
                                 const bool send_reservation_update)>& callback);

    ///
    /// \brief Called when a reservation is used, will remove it from the reservation list.
    /// \param reservation_id   The if of the reservation that is used.
    ///
    /// \note This will not set the EVSE or Connector to 'available'. That must be done separately (because we don't
    ///       know here when the connector is not connected anymore).
    ///
    void on_reservation_used(const int32_t reservation_id);

    ///
    /// @brief Function checks if the given \p id_token or \p parent_id_token matches the reserved token of the given \p
    /// evse_id
    ///
    /// @param id_token          Id token
    /// @param evse_id           Evse id
    /// @param parent_id_token   Parent id token
    /// @return The reservation id when there is a matching identifier, otherwise std::nullopt.
    ///
    std::optional<int32_t> matches_reserved_identifier(const std::string& id_token,
                                                       const std::optional<uint32_t> evse_id,
                                                       std::optional<std::string> parent_id_token);

    ///
    /// @brief Functions check if reservation at the given \p evse_id contains a parent_id
    /// @param evse_id  Evse id
    /// @return true if reservation for \p evse_id exists and reservation contains a parent_id
    ///
    bool has_reservation_parent_id(const std::optional<uint32_t> evse_id);

    ///
    /// \brief Check if the number of global reservations match the number of available evse's.
    /// \return The new reservation status of the evse's.
    ///
    /// \note The return value has the new reserved and new available statusses (so the ones that were already reserved
    ///       are not added to those lists).
    ///
    ReservationEvseStatus check_number_global_reservations_match_number_available_evses();

private: // Functions
    ///
    /// \brief Check if there is a specific connector type in the vector.
    /// \param evse_connectors  The vector to check for the type.
    /// \param connector_type   The connector type to find.
    /// \return True if the connector type is in the vector.
    ///
    bool has_evse_connector_type(const std::vector<Connector>& evse_connectors,
                                 const types::evse_manager::ConnectorTypeEnum connector_type) const;

    ///
    /// \brief Check if there is at least one EVSE with the given connector type.
    /// \param connector_type   The connector type to check.
    /// \return True if at least one EVSE has this connector type.
    ///
    bool does_evse_connector_type_exist(const types::evse_manager::ConnectorTypeEnum connector_type) const;

    ///
    /// \brief Helper function to get a reservation result from the current EVSE state and connector state, and if
    ///        there is a specific reservation for this EVSE.
    /// \param evse_id                      The evse id to get the state from.
    /// \param evse_specific_reservations   The evse specific reservations list to look in.
    /// \return The `ReservationResult` to return for this specific EVSE.
    ///
    types::reservation::ReservationResult get_evse_connector_state_reservation_result(
        const uint32_t evse_id, const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);

    ///
    /// \brief Helper function to check if the connector of a specific EVSE is available.
    /// \param evse_id          The evse id the connector belongs to.
    /// \param connector_type   The connector type to check.
    /// \return The `ReservationResult` to return for his specific connector.
    ///
    types::reservation::ReservationResult
    get_connector_availability_reservation_result(const uint32_t evse_id,
                                                  const types::evse_manager::ConnectorTypeEnum connector_type);

    ///
    /// \brief Get all possible orders of connector types given a vector of connector types.
    ///
    /// For the reservations, there must be checked if all combinations of arriving cars with specific connector types
    /// are possible. For that, we want to  have a list of all different combinations of arriving.
    ///
    /// So for example for connector type A, B and C, the different combinations are:
    /// - A, B, C
    /// - A, C, B
    /// - B, A, C
    /// - B, C, A
    /// - C, A, B
    /// - C, B, A
    ///
    /// And for connector types A, A and B, the combinations are:
    /// - A, A, B
    /// - A, B, A
    /// - B, A, A
    ///
    /// \param connectors   The connector types to get all orders from.
    /// \return A vector of all orders of the connector types.
    ///
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
    get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors) const;

    ///
    /// \brief Helper function: For a specific order of arrival of cars, check if there is still an EVSE available for
    /// each car.
    ///
    /// This function is called recursively, until no 'virtual cars' are left.
    ///
    /// \param used_evse_ids                The evse id's we have used in previous checks. This will be empty when the
    ///                                     function is first called and will be filled every time an evse id is
    ///                                     'used'.
    /// \param next_car_arrival_order       The order in which the cars arrive. This is for example 'A, B, C' and as
    ///                                     soon as the first is handled, it is removed from the list before
    ///                                     recursively calling the function again.
    /// \param evse_specific_reservations   EVSE specific reservations, to see if an EVSE is already reserved.
    /// \return True if this combination of car arrival orders is possible.
    ///
    bool can_virtual_car_arrive(const std::vector<uint32_t>& used_evse_ids,
                                const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order,
                                const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);

    ///
    /// \brief Check if it is possible to make a new reservation.
    /// \param global_reservation_type      If it is a global reservation: the reservation type.
    /// \param reservations_no_evse         The list of global reservations.
    /// \param evse_specific_reservations   The list of evse specific reservations.
    /// \return True if a reservation is possible, otherwise false.
    ///
    bool is_reservation_possible(const std::optional<types::evse_manager::ConnectorTypeEnum> global_reservation_type,
                                 const std::vector<types::reservation::Reservation>& reservations_no_evse,
                                 const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);

    ///
    /// \brief If a reservation is made, add the reservation to the `reservation_id_to_reservation_timeout_timer_map`.
    /// \param reservation  The reservation.
    /// \param evse_id      The evse id.
    ///
    void set_reservation_timer(const types::reservation::Reservation& reservation,
                               const std::optional<uint32_t> evse_id);

    ///
    /// \brief Get all evses that have a specific connector type.
    /// \param connector_type   The connector type.
    /// \return Vector with evse's.
    ///
    std::vector<EVSEContext*>
    get_all_evses_with_connector_type(const types::evse_manager::ConnectorTypeEnum connector_type) const;

    ///
    /// \brief For H01.FR.11, H01.FR.12 and H01.FR.13, the correct state must be returned.
    ///
    /// Also see @see module::ReservationHandler::get_reservation_evse_connector_state. This is a helper function to
    /// return the 'more important' state (Occupied is 'more important' than Unavailable).
    ///
    /// \param currrent_state   The current connector state.
    /// \param new_state        The new state.
    /// \return The connector state.
    ///
    ConnectorState get_new_connector_state(ConnectorState currrent_state, const ConnectorState new_state) const;

    ///
    /// \brief For H01.FR.11, H01.FR.12 and H01.FR.13, the correct state must be returned: if (all) evses are Occupied
    ///        or reserved, occupied must be returned, if (all) evses are Faulted, faulted must be returned, if (all)
    ///        evses are unavailable, unavailable must be returned. This function helps returning the correct state.
    ///
    /// If at least one of the EVSE's is Occupied, it will return occupied, then it will look to faulted and then to
    /// unavailable. So if one is occupied and one faulted, it will still return occupied.
    ///
    /// \param connector_type   The connector type to check.
    /// \return The reservation result that can be returned on the reserve now request.
    ///
    types::reservation::ReservationResult
    get_reservation_evse_connector_state(const types::evse_manager::ConnectorTypeEnum connector_type) const;

    ///
    /// \brief After a connector or evse is set to unavailable, faulted or occupied, this function can be called to
    ///        check the reservations and cancel reservations that are not possible now anymore.
    ///
    void check_reservations_and_cancel_if_not_possible();

    ///
    /// \brief Store reservations to key value store.
    ///
    void store_reservations();

    ///
    /// \brief Get new reserved / available status for evse's and store it.
    /// \param currently_available_evses    Current available evse's.
    /// \param reserved_evses               Current reserved evse's.
    /// \return A struct with changed reservation statuses compared with the last time this function was called.
    ///
    /// When an evse is reserved and it was available before, it will be added to the set in the struct (return value).
    /// But when an evse is reserved and last time it was already reserved, it is not added.
    ///
    ReservationEvseStatus
    get_evse_global_reserved_status_and_set_new_status(const std::set<int32_t>& currently_available_evses,
                                                       const std::set<int32_t>& reserved_evses);

    ///
    /// \brief Helper function to print information about reservations and evses, to find out why a reservation has
    ///        failed.
    /// \param reservation  The reservation.
    /// \param evse_id      The evse id.
    ///
    void print_reservations_debug_info(const types::reservation::Reservation& reservation,
                                       const std::optional<uint32_t> evse_id, const bool reservation_failed);
};

} // namespace module
