// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "reservationImpl.hpp"

namespace module {
namespace reservation {

void reservationImpl::init() {
}

void reservationImpl::ready() {
}

types::reservation::ReservationResult reservationImpl::handle_reserve_now(types::reservation::Reservation& request) {
    // your code for cmd reserve_now goes here

    EVLOG_debug << "Handle reservation for evse id " << (request.evse_id.has_value() ? request.evse_id.value() : -1);

    const auto reservation_result = this->mod->auth_handler->handle_reservation(request);
    if (reservation_result == ReservationResult::Accepted) {
        if (!this->mod->auth_handler->call_reserved(request.reservation_id, request.evse_id)) {
            return ReservationResult::Rejected;
        }
    }
    return reservation_result;
};

bool reservationImpl::handle_cancel_reservation(int& reservation_id) {
    const auto reservation_cancelled = this->mod->auth_handler->handle_cancel_reservation(reservation_id);
    if (reservation_cancelled.first) {
        // Call reservation cancelled. This comes from outside, so we don't send the status update (otherwise this is
        // sent to OCPP and that is not according to specification).
        this->mod->auth_handler->call_reservation_cancelled(reservation_id, ReservationEndReason::Cancelled,
                                                            reservation_cancelled.second, false);
        return true;
    }

    return false;
}

types::reservation::ReservationCheckStatus
reservationImpl::handle_exists_reservation(types::reservation::ReservationCheck& request) {
    return this->mod->auth_handler->handle_reservation_exists(request.id_token, request.evse_id,
                                                              request.group_id_token);
};

} // namespace reservation
} // namespace module
