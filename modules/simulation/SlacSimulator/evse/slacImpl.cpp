// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

namespace module {
namespace evse {

void slacImpl::init() {
}

void slacImpl::ready() {
}

void slacImpl::handle_reset(bool& enable) {
    this->mod->set_unmatched_evse();
}

bool slacImpl::handle_enter_bcd() {
    this->mod->set_matching_evse();
    return true;
}

bool slacImpl::handle_leave_bcd() {
    this->mod->set_unmatched_evse();
    return true;
}

bool slacImpl::handle_dlink_terminate() {
    this->mod->set_unmatched_evse();
    return true;
}

bool slacImpl::handle_dlink_error() {
    this->mod->set_unmatched_evse();
    return true;
}

bool slacImpl::handle_dlink_pause() {
    return true;
}

} // namespace evse
} // namespace module
