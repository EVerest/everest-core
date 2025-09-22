// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

namespace module {
namespace data_transfer {

void ocpp_data_transferImpl::init() {
}

void ocpp_data_transferImpl::ready() {
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    types::ocpp::DataTransferResponse response;
    response.status = types::ocpp::DataTransferStatus::Rejected;

    if (request.vendor_id == "EVerest") {
        response.data = "hello there";
        response.status = types::ocpp::DataTransferStatus::Accepted;
    } else {
        response.status = types::ocpp::DataTransferStatus::UnknownVendorId;
    }

    return response;
}

} // namespace data_transfer
} // namespace module
