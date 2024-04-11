// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

#include <conversions.hpp>

namespace module {
namespace data_transfer {

void ocpp_data_transferImpl::init() {
}

void ocpp_data_transferImpl::ready() {
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    ocpp::v201::DataTransferRequest ocpp_request = conversions::to_ocpp_data_transfer_request(request);
    auto ocpp_response = mod->charge_point->data_transfer_req(ocpp_request);
    types::ocpp::DataTransferResponse response = conversions::to_everest_data_transfer_response(ocpp_response);
    return response;
}

} // namespace data_transfer
} // namespace module
