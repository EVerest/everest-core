// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "ocpp_1_6_charge_pointImpl.hpp"

namespace module {
namespace main {

void ocpp_1_6_charge_pointImpl::init() {
}

void ocpp_1_6_charge_pointImpl::ready() {
}

bool ocpp_1_6_charge_pointImpl::handle_stop() {
    mod->charging_schedules_timer->stop();
    return mod->charge_point->stop();
}
bool ocpp_1_6_charge_pointImpl::handle_restart() {
    mod->charging_schedules_timer->interval(std::chrono::seconds(this->mod->config.PublishChargingScheduleIntervalS));
    return mod->charge_point->restart();
}

types::ocpp::DataTransferStatus convert_ocpp_data_transfer_status(ocpp::v16::DataTransferStatus status) {
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
ocpp_1_6_charge_pointImpl::handle_data_transfer(std::string& vendor_id, std::string& message_id, std::string& data) {
    auto ocpp_response = mod->charge_point->data_transfer(vendor_id, message_id, data);
    types::ocpp::DataTransferResponse response;
    response.status = convert_ocpp_data_transfer_status(ocpp_response.status);
    response.data = ocpp_response.data;
    return response;
}

} // namespace main
} // namespace module
