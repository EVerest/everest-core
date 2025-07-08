// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_charge_parameter_discovery.hpp>
#include <iso15118/d20/state/service_selection.hpp>

#include <algorithm>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/service_detail.hpp>
#include <iso15118/detail/d20/state/service_selection.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

namespace {

bool find_energy_services(const std::vector<uint16_t>& services, const uint16_t service) {
    return std::find(services.begin(), services.end(), service) != services.end();
}

void fill_internet_parameter_list(std::vector<dt::InternetParameterList>& internet_parameter_list,
                                  const dt::ServiceParameterList& custom_vas_parameters) {
    for (const auto& parameter_set : custom_vas_parameters) {
        auto& internet_parameter = internet_parameter_list.emplace_back();
        if (parameter_set.id == 1) {
            internet_parameter.port = dt::Port::Port20;
            internet_parameter.protocol = dt::Protocol::Ftp;
        } else if (parameter_set.id == 2) {
            internet_parameter.port = dt::Port::Port21;
            internet_parameter.protocol = dt::Protocol::Ftp;
        } else if (parameter_set.id == 3) {
            internet_parameter.port = dt::Port::Port80;
            internet_parameter.protocol = dt::Protocol::Http;
        } else if (parameter_set.id == 4) {
            internet_parameter.port = dt::Port::Port443;
            internet_parameter.protocol = dt::Protocol::Https;
        }
    }
}

void fill_parking_parameter_list(std::vector<message_20::datatypes::ParkingParameterList>& parking_parameter_list,
                                 const dt::ServiceParameterList& custom_vas_parameters) {
    for (const auto& parameter_set : custom_vas_parameters) {
        auto& parking_parameter = parking_parameter_list.emplace_back();
        for (const auto& parameter : parameter_set.parameter) {
            const auto value = std::get<int32_t>(parameter.value);
            if (parameter.name == "IntendedService") {
                parking_parameter.intended_service = static_cast<dt::IntendedService>(value);
            } else if (parameter.name == "ParkingStatusType") {
                parking_parameter.parking_status = static_cast<dt::ParkingStatus>(value);
            }
        }
    }
}

} // namespace

message_20::ServiceSelectionResponse handle_request(const message_20::ServiceSelectionRequest& req,
                                                    d20::Session& session) {

    message_20::ServiceSelectionResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    bool energy_service_found = false;
    bool vas_services_found = false;

    for (auto& energy_service : session.offered_services.energy_services) {
        if (energy_service == req.selected_energy_transfer_service.service_id) {
            energy_service_found = true;
            break;
        }
    }

    if (!energy_service_found) {
        return response_with_code(res, dt::ResponseCode::FAILED_NoEnergyTransferServiceSelected);
    }

    if (req.selected_vas_list.has_value()) {
        auto& selected_vas_list = req.selected_vas_list.value();

        for (auto& vas_service : selected_vas_list) {
            if (std::find(session.offered_services.vas_services.begin(), session.offered_services.vas_services.end(),
                          vas_service.service_id) == session.offered_services.vas_services.end()) {
                vas_services_found = false;
                break;
            }
            vas_services_found = true;
        }

        if (not vas_services_found) {
            return response_with_code(res, dt::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    if (not session.find_energy_parameter_set_id(req.selected_energy_transfer_service.service_id,
                                                 req.selected_energy_transfer_service.parameter_set_id)) {
        return response_with_code(res, dt::ResponseCode::FAILED_ServiceSelectionInvalid);
    }

    session.selected_service_parameters(req.selected_energy_transfer_service.service_id,
                                        req.selected_energy_transfer_service.parameter_set_id);

    if (req.selected_vas_list.has_value()) {
        auto& selected_vas_list = req.selected_vas_list.value();

        for (auto& vas_service : selected_vas_list) {
            if (not session.find_vas_parameter_set_id(vas_service.service_id, vas_service.parameter_set_id)) {
                return response_with_code(res, dt::ResponseCode::FAILED_ServiceSelectionInvalid);
            }
            session.selected_service_parameters(vas_service.service_id, vas_service.parameter_set_id);
        }
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void ServiceSelection::enter() {
    m_ctx.log.enter_state("ServiceSelection");
}

Result ServiceSelection::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::ServiceDetailRequest>()) {
        logf_info("Requested info about ServiceID: %d", req->service);

        using Service = dt::ServiceCategory;
        const std::vector<uint16_t> energy_services{
            message_20::to_underlying_value(Service::AC),         message_20::to_underlying_value(Service::DC),
            message_20::to_underlying_value(Service::WPT),        message_20::to_underlying_value(Service::DC_ACDP),
            message_20::to_underlying_value(Service::AC_BPT),     message_20::to_underlying_value(Service::DC_BPT),
            message_20::to_underlying_value(Service::DC_ACDP_BPT)};

        std::optional<dt::ServiceParameterList> custom_vas_parameters{std::nullopt};

        if (not find_energy_services(energy_services, req->service)) {
            logf_info("Getting vas (id: %u) parameters", req->service);
            custom_vas_parameters = m_ctx.feedback.get_vas_parameters(req->service);

            if (custom_vas_parameters.has_value() and
                req->service == message_20::to_underlying_value(dt::ServiceCategory::Internet)) {
                m_ctx.session_config.internet_parameter_list.clear();

                fill_internet_parameter_list(m_ctx.session_config.internet_parameter_list,
                                             custom_vas_parameters.value());
                custom_vas_parameters.reset();

            } else if (custom_vas_parameters.has_value() and
                       req->service == message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)) {
                m_ctx.session_config.parking_parameter_list.clear();

                fill_parking_parameter_list(m_ctx.session_config.parking_parameter_list, custom_vas_parameters.value());
                custom_vas_parameters.reset();
            }
        }

        const auto res = handle_request(*req, m_ctx.session, m_ctx.session_config, custom_vas_parameters);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        return {};
    } else if (const auto req = variant->get_if<message_20::ServiceSelectionRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        if (res.response_code == message_20::datatypes::ResponseCode::OK) {
            const auto selected_services = m_ctx.session.get_selected_services();
            m_ctx.feedback.selected_service_parameters(selected_services);
        }

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (req->selected_vas_list.has_value()) {
            m_ctx.feedback.selected_vas_services(req->selected_vas_list.value());
        }

        return m_ctx.create_state<DC_ChargeParameterDiscovery>();
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;

        return {};
    } else {
        m_ctx.log("expected ServiceDetailReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
