// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "reservationImpl.hpp"

namespace module {
namespace reservation {

void reservationImpl::init() {
}

void reservationImpl::ready() {
}

types::reservation::ReservationResult
reservationImpl::handle_reserve_now(int& connector_id, types::reservation::Reservation& reservation) {
    // your code for cmd reserve_now goes here

    const auto reservation_result = this->mod->auth_handler->handle_reservation(connector_id, reservation);
    if (reservation_result == ReservationResult::Accepted) {
        this->mod->auth_handler->call_reserved(connector_id, reservation.reservation_id);
    }
    return reservation_result;
};

bool reservationImpl::handle_cancel_reservation(int& reservation_id) {
    const auto connector = this->mod->auth_handler->handle_cancel_reservation(reservation_id);
    if (connector != -1) {
        this->mod->auth_handler->call_reservation_cancelled(connector);
        return true;
    }
    return false;
};

} // namespace reservation
} // namespace module
