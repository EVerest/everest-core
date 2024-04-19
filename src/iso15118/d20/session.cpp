// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/session.hpp>

#include <random>

namespace iso15118::d20 {

Session::Session() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

    for (auto& item : id) {
        item = distribution(generator);
    }
}

Session::Session(SelectedServiceParameters service_parameters_) : selected_services(service_parameters_) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

    for (auto& item : id) {
        item = distribution(generator);
    }
}
Session::Session(OfferedServices services_) : offered_services(services_) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

    for (auto& item : id) {
        item = distribution(generator);
    }
}

Session::~Session() = default;

bool Session::find_parameter_set_id(const message_20::ServiceCategory service, int16_t id) {

    switch (service) {
    case message_20::ServiceCategory::DC:

        if (this->offered_services.dc_parameter_list.find(id) != this->offered_services.dc_parameter_list.end()) {
            return true;
        }
        break;

    case message_20::ServiceCategory::DC_BPT:
        if (this->offered_services.dc_bpt_parameter_list.find(id) !=
            this->offered_services.dc_bpt_parameter_list.end()) {
            return true;
        }
        break;

    case message_20::ServiceCategory::Internet:
        if (this->offered_services.internet_parameter_list.find(id) !=
            this->offered_services.internet_parameter_list.end()) {
            return true;
        }
        break;

    case message_20::ServiceCategory::ParkingStatus:
        if (this->offered_services.parking_parameter_list.find(id) !=
            this->offered_services.parking_parameter_list.end()) {
            return true;
        }

    default:
        // Todo(sl): logf AC, WPT, ACDP is not supported
        break;
    }

    return false;
}

void Session::selected_service_parameters(const message_20::ServiceCategory service, const uint16_t id) {

    switch (service) {
    case message_20::ServiceCategory::DC:

        if (this->offered_services.dc_parameter_list.find(id) != this->offered_services.dc_parameter_list.end()) {
            auto& parameters = this->offered_services.dc_parameter_list.at(id);
            this->selected_services =
                SelectedServiceParameters(message_20::ServiceCategory::DC, parameters.connector,
                                          parameters.control_mode, parameters.mobility_needs_mode, parameters.pricing);
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;

    case message_20::ServiceCategory::DC_BPT:
        if (this->offered_services.dc_bpt_parameter_list.find(id) !=
            this->offered_services.dc_bpt_parameter_list.end()) {
            auto& parameters = this->offered_services.dc_bpt_parameter_list.at(id);
            this->selected_services = SelectedServiceParameters(
                message_20::ServiceCategory::DC_BPT, parameters.connector, parameters.control_mode,
                parameters.mobility_needs_mode, parameters.pricing, parameters.bpt_channel, parameters.generator_mode);
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;

    case message_20::ServiceCategory::Internet:

        if (this->offered_services.internet_parameter_list.find(id) !=
            this->offered_services.internet_parameter_list.end()) {
            this->selected_vas_services.vas_services.push_back(message_20::ServiceCategory::Internet);
            auto& parameters = this->offered_services.internet_parameter_list.at(id);
            this->selected_vas_services.internet_port = parameters.port;
            this->selected_vas_services.internet_protocol = parameters.protocol;
        }
        break;

    case message_20::ServiceCategory::ParkingStatus:

        if (this->offered_services.parking_parameter_list.find(id) !=
            this->offered_services.parking_parameter_list.end()) {
            this->selected_vas_services.vas_services.push_back(message_20::ServiceCategory::ParkingStatus);
            auto& parameters = this->offered_services.parking_parameter_list.at(id);
            this->selected_vas_services.parking_intended_service = parameters.intended_service;
            this->selected_vas_services.parking_status = parameters.parking_status;
        }
        break;

    default:
        // Todo(sl): logf AC, WPT, ACDP is not supported
        break;
    }
}

} // namespace iso15118::d20
