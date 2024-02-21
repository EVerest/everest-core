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
    auto ocpp_response = mod->charge_point->data_transfer_req(request.vendor_id, request.message_id, request.data);
    types::ocpp::DataTransferResponse response;
    response.status = conversions::to_everest_data_transfer_status(ocpp_response.status);
    response.data = ocpp_response.data;
    return response;
}

} // namespace data_transfer
} // namespace module
