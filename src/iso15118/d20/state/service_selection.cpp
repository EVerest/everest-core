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

message_20::ServiceSelectionResponse handle_request(const message_20::ServiceSelectionRequest& req,
                                                    d20::Session& session) {

    message_20::ServiceSelectionResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_20::ResponseCode::FAILED_UnknownSession);
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
        return response_with_code(res, message_20::ResponseCode::FAILED_NoEnergyTransferServiceSelected);
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
            return response_with_code(res, message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    if (not session.find_parameter_set_id(req.selected_energy_transfer_service.service_id,
                                          req.selected_energy_transfer_service.parameter_set_id)) {
        return response_with_code(res, message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
    }

    session.selected_service_parameters(req.selected_energy_transfer_service.service_id,
                                        req.selected_energy_transfer_service.parameter_set_id);

    if (req.selected_vas_list.has_value()) {
        auto& selected_vas_list = req.selected_vas_list.value();

        for (auto& vas_service : selected_vas_list) {
            if (not session.find_parameter_set_id(vas_service.service_id, vas_service.parameter_set_id)) {
                return response_with_code(res, message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
            }
            session.selected_service_parameters(vas_service.service_id, vas_service.parameter_set_id);
        }
    }

    return response_with_code(res, message_20::ResponseCode::OK);
}

void ServiceSelection::enter() {
    ctx.log.enter_state("ServiceSelection");
}

FsmSimpleState::HandleEventReturnType ServiceSelection::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::ServiceDetailRequest>()) {
        logf_info("Requested info about ServiceID: %d", req->service);

        const auto res = handle_request(*req, ctx.session, ctx.session_config);

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.HANDLED_INTERNALLY;
    } else if (const auto req = variant->get_if<message_20::ServiceSelectionRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<DC_ChargeParameterDiscovery>(ctx);
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ServiceDetailReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
