// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/d2/state/post_service_discovery.hpp>
#include <iso15118/d2/state/service_discovery.hpp>
#include <iso15118/message/d2/service_discovery.hpp>

#include <iso15118/detail/d2/context_helper.hpp>
#include <iso15118/detail/helper.hpp>
#include <optional>

namespace iso15118::d2::state {

namespace dt = d2::msg::data_types;

dt::ServiceList filter_services(const dt::ServiceList& offered_services, const std::optional<dt::ServiceScope>& scope,
                                const std::optional<dt::ServiceCategory>& category) {
    dt::ServiceList filtered_services;

    for (const auto& service : offered_services) {
        if (scope.has_value() && service.service_scope != scope.value()) {
            continue;
        }
        if (category.has_value() && service.service_category != category.value()) {
            continue;
        }
        filtered_services.push_back(service);
    }
    return filtered_services;
}

d2::msg::ServiceDiscoveryResponse handle_request(const d2::msg::ServiceDiscoveryRequest& req, d2::Session& session,
                                                 const dt::ChargeService& charge_service,
                                                 const std::vector<dt::PaymentOption>& payment_options,
                                                 const dt::ServiceList& offered_services) {
    d2::msg::ServiceDiscoveryResponse res;
    setup_header(res.header, session);

    res.charge_service = charge_service;
    res.payment_option_list = payment_options;

    if (!offered_services.empty()) {
        if (req.service_category.has_value() || req.service_scope.has_value()) {
            dt::ServiceList services_to_offer =
                filter_services(offered_services, req.service_scope, req.service_category);
            res.service_list = services_to_offer;
        } else {
            res.service_list = offered_services;
        }
    }

    // [V2G2-545]
    return response_with_code(res, dt::ResponseCode::OK);
}

void ServiceDiscovery::enter() {
    // m_ctx.log.enter_state("ServiceDiscovery");
}

Result ServiceDiscovery::feed(Event ev) {
    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<msg::ServiceDiscoveryRequest>()) {

        const auto res =
            handle_request(*req, m_ctx.session, m_ctx.session_config.charge_service,
                           m_ctx.session_config.supported_payment_options, m_ctx.session_config.offered_services);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            // m_ctx.log("Error processing ServiceDiscoveryReq");
            return {};
        }

        // TODO(kd): I wonder how to call the next state
        // It waits either for ServiceDetailReq or ServicePaymentSelectionReq
        // Perhaps PostServiceDiscovery?
        return m_ctx.create_state<PostServiceDiscovery>();
    }
    // m_ctx.log("expected ServiceDiscoveryReq! But code type id: %d", variant->get_type());

    // Sequence Error [V2G2-538]
    const msg::Type req_type = variant->get_type();
    send_sequence_error(req_type, m_ctx);

    m_ctx.session_stopped = true;

    return {};
}

} // namespace iso15118::d2::state
