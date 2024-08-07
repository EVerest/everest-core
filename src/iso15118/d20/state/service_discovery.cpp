// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>

#include <iso15118/d20/state/service_detail.hpp>
#include <iso15118/d20/state/service_discovery.hpp>

#include <iso15118/detail/helper.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/service_discovery.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>

namespace iso15118::d20::state {

static bool find_service_id(const std::vector<uint16_t>& req_service_ids, const uint16_t service) {
    return std::find(req_service_ids.begin(), req_service_ids.end(), service) != req_service_ids.end();
}

message_20::ServiceDiscoveryResponse handle_request(const message_20::ServiceDiscoveryRequest& req,
                                                    d20::Session& session,
                                                    const std::vector<message_20::ServiceCategory>& energy_services,
                                                    const std::vector<message_20::ServiceCategory>& vas_services) {

    message_20::ServiceDiscoveryResponse res = message_20::ServiceDiscoveryResponse();

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_20::ResponseCode::FAILED_UnknownSession);
    }

    // Service renegotiation is not yet supported
    res.service_renegotiation_supported = false;
    session.service_renegotiation_supported = false;

    // Reset default value
    res.energy_transfer_service_list.clear();

    std::vector<message_20::ServiceDiscoveryResponse::Service> energy_services_list;
    std::vector<message_20::ServiceDiscoveryResponse::Service> vas_services_list;

    // EV supported service ID's
    if (req.supported_service_ids.has_value() == true) {
        for (auto& energy_service : energy_services) {
            if (find_service_id(req.supported_service_ids.value(), static_cast<uint16_t>(energy_service))) {
                energy_services_list.push_back({energy_service, false});
            }
        }
        for (auto& vas_service : vas_services) {
            if (find_service_id(req.supported_service_ids.value(), static_cast<uint16_t>(vas_service))) {
                vas_services_list.push_back({vas_service, false});
            }
        }
    } else {
        for (auto& energy_service : energy_services) {
            energy_services_list.push_back({energy_service, false});
        }
        for (auto& vas_service : vas_services) {
            vas_services_list.push_back({vas_service, false});
        }
    }

    for (auto& conf_energy_service : energy_services_list) {
        auto& energy_service = res.energy_transfer_service_list.emplace_back();
        energy_service = conf_energy_service;
        session.offered_services.energy_services.push_back(conf_energy_service.service_id);
    }

    if (vas_services_list.empty() == false) {
        auto& vas_service_list = res.vas_list.emplace();
        vas_service_list.reserve(vas_services_list.size());
        for (auto& conf_vas_service : vas_services_list) {
            auto& vas_service = vas_service_list.emplace_back();
            vas_service = conf_vas_service;
            session.offered_services.vas_services.push_back(conf_vas_service.service_id);
        }
    }

    return response_with_code(res, message_20::ResponseCode::OK);
}

void ServiceDiscovery::enter() {
    ctx.log.enter_state("ServiceDiscovery");
}

FsmSimpleState::HandleEventReturnType ServiceDiscovery::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::ServiceDiscoveryRequest>()) {
        if (req->supported_service_ids) {
            logf("Possible ids\n");
            for (auto id : req->supported_service_ids.value()) {
                logf("  %d\n", id);
            }
        }

        const auto res = handle_request(*req, ctx.session, ctx.config.supported_energy_transfer_services,
                                        ctx.config.supported_vas_services);

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<ServiceDetail>(ctx);
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ServiceDiscoveryReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
