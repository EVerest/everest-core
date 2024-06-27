// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "generic_arrayImpl.hpp"

namespace module {
namespace system_specific_data_1 {

void generic_arrayImpl::init() {
    mod->serial.signal_opaque_data.connect([this](int connector, const std::vector<int32_t>& data) {
        if (connector != 1)
            return;
        EVLOG_info << "Received data from " << connector;
        publish_vector_of_ints({data});
    });
}

void generic_arrayImpl::ready() {
}

} // namespace system_specific_data_1
} // namespace module
