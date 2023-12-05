// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

namespace module {
namespace data_transfer {

void ocpp_data_transferImpl::init() {
}

void ocpp_data_transferImpl::ready() {
}

types::ocpp::DataTransferStatus to_everest(ocpp::v201::DataTransferStatusEnum status) {
    switch (status) {
    case ocpp::v201::DataTransferStatusEnum::Accepted:
        return types::ocpp::DataTransferStatus::Accepted;
    case ocpp::v201::DataTransferStatusEnum::Rejected:
        return types::ocpp::DataTransferStatus::Rejected;
    case ocpp::v201::DataTransferStatusEnum::UnknownMessageId:
        return types::ocpp::DataTransferStatus::UnknownMessageId;
    case ocpp::v201::DataTransferStatusEnum::UnknownVendorId:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    default:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    }
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    auto ocpp_response = mod->charge_point->data_transfer_req(request.vendor_id, request.message_id, request.data);
    types::ocpp::DataTransferResponse response;
    response.status = to_everest(ocpp_response.status);
    response.data = ocpp_response.data;
    return response;
}

} // namespace data_transfer
} // namespace module
