// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ev_slacImpl.hpp"

namespace module {
namespace ev {

void ev_slacImpl::init() {
}

void ev_slacImpl::ready() {
}

void ev_slacImpl::handle_reset() {
    this->mod->set_unmatched_ev();
}

bool ev_slacImpl::handle_trigger_matching() {
    this->mod->set_matching_ev();
    return true;
}

} // namespace ev
} // namespace module
