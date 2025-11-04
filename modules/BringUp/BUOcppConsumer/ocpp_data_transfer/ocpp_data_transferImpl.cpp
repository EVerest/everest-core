// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

namespace module {
namespace ocpp_data_transfer {

void ocpp_data_transferImpl::init() {
}

void ocpp_data_transferImpl::ready() {
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    // your code for cmd data_transfer goes here
    return {};
}

} // namespace ocpp_data_transfer
} // namespace module
