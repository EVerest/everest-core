// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/session.hpp>

#include <random>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d20 {

namespace dt = message_20::datatypes;

SelectedServiceParameters::SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::DcConnector dc_connector_,
                                                     dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_,
                                                     dt::Pricing pricing_) :
    selected_energy_service(energy_service_),
    selected_control_mode(control_mode_),
    selected_mobility_needs_mode(mobility_),
    selected_pricing(pricing_) {
    selected_connector.emplace<dt::DcConnector>(dc_connector_);
};

SelectedServiceParameters::SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::DcConnector dc_connector_,
                                                     dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_,
                                                     dt::Pricing pricing_, dt::BptChannel channel_,
                                                     dt::GeneratorMode generator_) :
    selected_energy_service(energy_service_),
    selected_control_mode(control_mode_),
    selected_mobility_needs_mode(mobility_),
    selected_pricing(pricing_),
    selected_bpt_channel(channel_),
    selected_generator_mode(generator_) {
    selected_connector.emplace<dt::DcConnector>(dc_connector_);
};

SelectedServiceParameters::SelectedServiceParameters(dt::ServiceCategory energy_service_,
                                                     dt::McsConnector mcs_connector_, dt::ControlMode control_mode_,
                                                     dt::MobilityNeedsMode mobility_, dt::Pricing pricing_) :
    selected_energy_service(energy_service_),
    selected_control_mode(control_mode_),
    selected_mobility_needs_mode(mobility_),
    selected_pricing(pricing_) {
    selected_connector.emplace<dt::McsConnector>(mcs_connector_);
};

SelectedServiceParameters::SelectedServiceParameters(dt::ServiceCategory energy_service_,
                                                     dt::McsConnector mcs_connector_, dt::ControlMode control_mode_,
                                                     dt::MobilityNeedsMode mobility_, dt::Pricing pricing_,
                                                     dt::BptChannel channel_, dt::GeneratorMode generator_) :
    selected_energy_service(energy_service_),
    selected_control_mode(control_mode_),
    selected_mobility_needs_mode(mobility_),
    selected_pricing(pricing_),
    selected_bpt_channel(channel_),
    selected_generator_mode(generator_) {
    selected_connector.emplace<dt::McsConnector>(mcs_connector_);
};

Session::Session() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

    for (auto& item : id) {
        item = distribution(generator);
    }
}

Session::Session(const PauseContext& pause_ctx) :
    id(pause_ctx.old_session_id), selected_services(pause_ctx.selected_service_parameters){};

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

bool Session::find_energy_parameter_set_id(const dt::ServiceCategory service, int16_t id) {

    switch (service) {
    case dt::ServiceCategory::DC:

        if (this->offered_services.dc_parameter_list.find(id) != this->offered_services.dc_parameter_list.end()) {
            return true;
        }
        break;

    case dt::ServiceCategory::DC_BPT:
        if (this->offered_services.dc_bpt_parameter_list.find(id) !=
            this->offered_services.dc_bpt_parameter_list.end()) {
            return true;
        }
        break;
    case dt::ServiceCategory::MCS:
        if (this->offered_services.mcs_parameter_list.find(id) != this->offered_services.mcs_parameter_list.end()) {
            return true;
        }
        break;
    case dt::ServiceCategory::MCS_BPT:
        if (this->offered_services.mcs_bpt_parameter_list.find(id) !=
            this->offered_services.mcs_bpt_parameter_list.end()) {
            return true;
        }
        break;
    case dt::ServiceCategory::AC:
        [[fallthrough]];
    case dt::ServiceCategory::WPT:
        [[fallthrough]];
    case dt::ServiceCategory::DC_ACDP:
        [[fallthrough]];
    case dt::ServiceCategory::AC_BPT:
        [[fallthrough]];
    case dt::ServiceCategory::DC_ACDP_BPT:
        [[fallthrough]];
    case dt::ServiceCategory::AC_DER:
        [[fallthrough]];
    default:
        logf_warning("Service %u is not supported yet", message_20::to_underlying_value(service));
        break;
    }

    return false;
}

bool Session::find_vas_parameter_set_id(const uint16_t vas_service, int16_t id) {
    if (vas_service == message_20::to_underlying_value(dt::ServiceCategory::Internet)) {
        if (this->offered_services.internet_parameter_list.find(id) !=
            this->offered_services.internet_parameter_list.end()) {
            return true;
        }
    } else if (vas_service == message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)) {
        if (this->offered_services.parking_parameter_list.find(id) !=
            this->offered_services.parking_parameter_list.end()) {
            return true;
        }
    } else {
        logf_info("Find parameter_set_id from service: %u", vas_service);
        if (this->offered_services.custom_vas_list.find(vas_service) != this->offered_services.custom_vas_list.end()) {
            const auto custom_vas_parameter_set_ids = this->offered_services.custom_vas_list.at(vas_service);
            for (auto offered_id : custom_vas_parameter_set_ids) {
                if (offered_id == id) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Session::selected_service_parameters(const dt::ServiceCategory service, const uint16_t id) {

    switch (service) {
    case dt::ServiceCategory::DC:
        if (this->offered_services.dc_parameter_list.find(id) != this->offered_services.dc_parameter_list.end()) {
            auto& parameters = this->offered_services.dc_parameter_list.at(id);
            this->selected_services =
                SelectedServiceParameters(dt::ServiceCategory::DC, parameters.connector, parameters.control_mode,
                                          parameters.mobility_needs_mode, parameters.pricing);

            logf_info("Selected DC service parameters: control mode: %s, mobility needs mode: %s",
                      dt::from_control_mode(parameters.control_mode).c_str(),
                      dt::from_mobility_needs_mode(parameters.mobility_needs_mode).c_str());
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;
    case dt::ServiceCategory::DC_BPT:
        if (this->offered_services.dc_bpt_parameter_list.find(id) !=
            this->offered_services.dc_bpt_parameter_list.end()) {
            auto& parameters = this->offered_services.dc_bpt_parameter_list.at(id);
            this->selected_services = SelectedServiceParameters(
                dt::ServiceCategory::DC_BPT, parameters.connector, parameters.control_mode,
                parameters.mobility_needs_mode, parameters.pricing, parameters.bpt_channel, parameters.generator_mode);

            logf_info("Selected DC_BPT service parameters: control mode: %s, mobility needs mode: %s",
                      dt::from_control_mode(parameters.control_mode).c_str(),
                      dt::from_mobility_needs_mode(parameters.mobility_needs_mode).c_str());
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;
    case dt::ServiceCategory::MCS:
        if (this->offered_services.mcs_parameter_list.find(id) != this->offered_services.mcs_parameter_list.end()) {
            auto& parameters = this->offered_services.mcs_parameter_list.at(id);
            this->selected_services =
                SelectedServiceParameters(dt::ServiceCategory::MCS, parameters.connector, parameters.control_mode,
                                          parameters.mobility_needs_mode, parameters.pricing);

            logf_info("Selected MCS service parameters: control mode: %s, mobility needs mode: %s",
                      dt::from_control_mode(parameters.control_mode).c_str(),
                      dt::from_mobility_needs_mode(parameters.mobility_needs_mode).c_str());
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;

    case dt::ServiceCategory::MCS_BPT:
        if (this->offered_services.mcs_bpt_parameter_list.find(id) !=
            this->offered_services.mcs_bpt_parameter_list.end()) {
            auto& parameters = this->offered_services.mcs_bpt_parameter_list.at(id);
            this->selected_services = SelectedServiceParameters(
                dt::ServiceCategory::MCS_BPT, parameters.connector, parameters.control_mode,
                parameters.mobility_needs_mode, parameters.pricing, parameters.bpt_channel, parameters.generator_mode);

            logf_info("Selected MCS_BPT service parameters: control mode: %s, mobility needs mode: %s",
                      dt::from_control_mode(parameters.control_mode).c_str(),
                      dt::from_mobility_needs_mode(parameters.mobility_needs_mode).c_str());
        } else {
            // Todo(sl): Should be not the case -> Raise Error?
        }
        break;
    case dt::ServiceCategory::AC:
        [[fallthrough]];
    case dt::ServiceCategory::WPT:
        [[fallthrough]];
    case dt::ServiceCategory::DC_ACDP:
        [[fallthrough]];
    case dt::ServiceCategory::AC_BPT:
        [[fallthrough]];
    case dt::ServiceCategory::DC_ACDP_BPT:
        [[fallthrough]];
    case dt::ServiceCategory::AC_DER:
        [[fallthrough]];
    default:
        logf_warning("Service %u is not supported yet", message_20::to_underlying_value(service));
        break;
    }
}

void Session::selected_service_parameters(const uint16_t vas_service, const uint16_t id) {

    if (vas_service == message_20::to_underlying_value(dt::ServiceCategory::Internet)) {
        if (this->offered_services.internet_parameter_list.find(id) !=
            this->offered_services.internet_parameter_list.end()) {
            this->selected_vas_services.vas_services.push_back(dt::ServiceCategory::Internet);
            auto& parameters = this->offered_services.internet_parameter_list.at(id);
            this->selected_vas_services.internet_port = parameters.port;
            this->selected_vas_services.internet_protocol = parameters.protocol;
        }
    } else if (vas_service == message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)) {
        if (this->offered_services.parking_parameter_list.find(id) !=
            this->offered_services.parking_parameter_list.end()) {
            this->selected_vas_services.vas_services.push_back(dt::ServiceCategory::ParkingStatus);
            auto& parameters = this->offered_services.parking_parameter_list.at(id);
            this->selected_vas_services.parking_intended_service = parameters.intended_service;
            this->selected_vas_services.parking_status = parameters.parking_status;
        }
    } else {
        logf_info("Right now not selecting anything for custom vas service");
    }
}

} // namespace iso15118::d20
