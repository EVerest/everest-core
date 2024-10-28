// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "reservationImpl.hpp"

namespace module {
namespace reservation {

void reservationImpl::init() {
}

void reservationImpl::ready() {
}

// TODO mz change to evse id??
types::reservation::ReservationResult
reservationImpl::handle_reserve_now(types::reservation::ReserveNowRequest& request) {
    // your code for cmd reserve_now goes here

    const auto reservation_result = this->mod->auth_handler->handle_reservation(request.evse_id, request.reservation);
    if (reservation_result == ReservationResult::Accepted) {
        this->mod->auth_handler->call_reserved(request.evse_id, request.reservation.reservation_id);
    }
    return reservation_result;
};

bool reservationImpl::handle_cancel_reservation(int& reservation_id) {
    const auto connector = this->mod->auth_handler->handle_cancel_reservation(reservation_id);
    if (connector != -1) {
        this->mod->auth_handler->call_reservation_cancelled(connector, reservation_id, ReservationEndReason::Cancelled);
        return true;
    }
    return false;
}

bool reservationImpl::handle_is_reservation_for_token(types::reservation::ReservationCheck& request) {
    return this->mod->auth_handler->handle_is_reservation_for_token(request.connector_id, request.id_token,
                                                                    request.group_id_token);
};

} // namespace reservation
} // namespace module
