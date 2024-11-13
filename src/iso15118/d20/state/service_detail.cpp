// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/service_detail.hpp>
#include <iso15118/d20/state/service_selection.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/service_detail.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::ServiceDetailResponse handle_request(const message_20::ServiceDetailRequest& req, d20::Session& session,
                                                 const d20::SessionConfig& config) {

    message_20::ServiceDetailResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    bool service_found = false;

    for (auto& energy_service : session.offered_services.energy_services) {
        if (energy_service == req.service) {
            service_found = true;
            break;
        }
    }

    for (auto& vas_service : session.offered_services.vas_services) {
        if (vas_service == req.service) {
            service_found = true;
            break;
        }
    }

    if (!service_found) {
        return response_with_code(res, dt::ResponseCode::FAILED_ServiceIDInvalid);
    }

    res.service_parameter_list.clear(); // reset default values

    uint8_t id = 0;

    switch (req.service) {
    case dt::ServiceCategory::DC:
        res.service = dt::ServiceCategory::DC;
        for (auto& parameter_set : config.dc_parameter_list) {
            session.offered_services.dc_parameter_list[id] = parameter_set;
            res.service_parameter_list.push_back(dt::ParameterSet(id++, parameter_set));
        }
        break;
    case dt::ServiceCategory::DC_BPT:
        res.service = dt::ServiceCategory::DC_BPT;
        for (auto& parameter_set : config.dc_bpt_parameter_list) {
            session.offered_services.dc_bpt_parameter_list[id] = parameter_set;
            res.service_parameter_list.push_back(dt::ParameterSet(id++, parameter_set));
        }
        break;

    case dt::ServiceCategory::Internet:
        res.service = dt::ServiceCategory::Internet;

        for (auto& parameter_set : config.internet_parameter_list) {
            // TODO(sl): Possibly refactor, define const
            if (parameter_set.port == dt::Port::Port20) {
                id = 1;
            } else if (parameter_set.port == dt::Port::Port21) {
                id = 2;
            } else if (parameter_set.port == dt::Port::Port80) {
                id = 3;
            } else if (parameter_set.port == dt::Port::Port443) {
                id = 4;
            }
            session.offered_services.internet_parameter_list[id] = parameter_set;
            res.service_parameter_list.push_back(dt::ParameterSet(id, parameter_set));
        }

        break;

    case dt::ServiceCategory::ParkingStatus:
        res.service = dt::ServiceCategory::ParkingStatus;

        for (auto& parameter_set : config.parking_parameter_list) {
            session.offered_services.parking_parameter_list[id] = parameter_set;
            res.service_parameter_list.push_back(dt::ParameterSet(id++, parameter_set));
        }
        break;

    default:
        // Todo(sl): fill not supported
        break;
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void ServiceDetail::enter() {
    ctx.log.enter_state("ServiceDetail");
}

FsmSimpleState::HandleEventReturnType ServiceDetail::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::ServiceDetailRequest>()) {
        logf_info("Requested info about ServiceID: %d", req->service);

        const auto res = handle_request(*req, ctx.session, ctx.session_config);

        ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<ServiceSelection>(ctx);
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
