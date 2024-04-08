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
    ocpp::v201::DataTransferRequest ocpp_request;
    ocpp_request.vendorId = request.vendor_id;
    if (request.message_id.has_value()) {
        ocpp_request.messageId = request.message_id.value();
    }
    if (request.data.has_value()) {
        ocpp_request.data = json::parse(request.data.value());
    }
    if (request.custom_data.has_value()) {
        auto custom_data = request.custom_data.value();
        json custom_data_json = json::parse(custom_data.data);
        if (not custom_data_json.contains("vendorId")) {
            EVLOG_warning << "DataTransferRequest custom_data.data does not contain vendorId, automatically adding it";
            custom_data_json["vendorId"] = custom_data.vendor_id;
        }
        ocpp_request.customData = custom_data_json;
    }
    auto ocpp_response = mod->charge_point->data_transfer_req(ocpp_request);
    types::ocpp::DataTransferResponse response;
    response.status = conversions::to_everest_data_transfer_status(ocpp_response.status);
    if (ocpp_response.data.has_value()) {
        response.data = ocpp_response.data.value().dump();
    }
    if (ocpp_response.customData.has_value()) {
        auto ocpp_custom_data = ocpp_response.customData.value();
        types::ocpp::CustomData custom_data{ocpp_custom_data.at("vendorId"), ocpp_custom_data.dump()};
        response.custom_data = ocpp_custom_data;
    }
    return response;
}

} // namespace data_transfer
} // namespace module
