// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_vasImpl.hpp"

namespace module {
namespace iso15118_vas {

void ISO15118_vasImpl::init() {
}

void ISO15118_vasImpl::ready() {
}

std::vector<types::iso15118_vas::ParameterSet> ISO15118_vasImpl::handle_get_service_parameters(int& service_id) {
    // your code for cmd get_service_parameters goes here
    return {};
}

void ISO15118_vasImpl::handle_selected_services(std::vector<types::iso15118_vas::SelectedService>& selected_services) {
    // your code for cmd selected_services goes here
}

} // namespace iso15118_vas
} // namespace module
