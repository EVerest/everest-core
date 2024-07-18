// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

namespace module {
namespace data_transfer {

void ocpp_data_transferImpl::init() {
}

void ocpp_data_transferImpl::ready() {
}

types::ocpp::DataTransferStatus to_everest(ocpp::v16::DataTransferStatus status) {
    switch (status) {
    case ocpp::v16::DataTransferStatus::Accepted:
        return types::ocpp::DataTransferStatus::Accepted;
    case ocpp::v16::DataTransferStatus::Rejected:
        return types::ocpp::DataTransferStatus::Rejected;
    case ocpp::v16::DataTransferStatus::UnknownMessageId:
        return types::ocpp::DataTransferStatus::UnknownMessageId;
    case ocpp::v16::DataTransferStatus::UnknownVendorId:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    default:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    }
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    auto ocpp_response = mod->charge_point->data_transfer(request.vendor_id, request.message_id, request.data);
    types::ocpp::DataTransferResponse response;
    if (ocpp_response.has_value()) {
        response.status = to_everest(ocpp_response.value().status);
        response.data = ocpp_response.value().data;
    } else {
        response.status = types::ocpp::DataTransferStatus::Offline;
    }
    return response;
}

} // namespace data_transfer
} // namespace module
