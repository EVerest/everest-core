// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/d2/state/payment_service_selection.hpp>
#include <iso15118/d2/state/service_discovery.hpp>
#include <iso15118/message/d2/service_discovery.hpp>

#include <iso15118/detail/d2/context_helper.hpp>
#include <iso15118/detail/helper.hpp>
#include <optional>
#include <vector>

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

dt::ServiceCategory convert_service_id_to_service_category(const std::uint16_t service_id) {
    switch (service_id) {
    case 1:
        return dt::ServiceCategory::EvCharging;
        break;
    case 2:
        return dt::ServiceCategory::ContractCertificate;
        break;
    case 3:
        return dt::ServiceCategory::Internet;
        break;
    case 4:
    // According to ISO15118-2 the service_category should be EVSEInformation but it is not defined in
    // ServiceCategoryType so we fall back to OtherCustom
    default:
        return dt::ServiceCategory::OtherCustom;
    }
}

dt::Service construct_service_from_id(const dt::ServiceID& id) {
    dt::Service service;
    service.service_id = id;

    // See ISO15118-2 Table 105
    if (id == 1) {
        service.service_name = "AC_DC_Charging";
        service.service_category = dt::ServiceCategory::EvCharging;
    } else if (id == 2) {
        service.service_name = "Certificate";
        service.service_category = dt::ServiceCategory::ContractCertificate;
    } else if (id == 3) {
        service.service_name = "InternetAccess";
        service.service_category = dt::ServiceCategory::Internet;
    } else if (id == 4) {
        service.service_name = "UseCaseInformation";
        // According to ISO15118-2 the service_category should be EVSEInformation but it is not defined in
        // ServiceCategoryType
        service.service_category = dt::ServiceCategory::OtherCustom;
    }

    return service;
}

d2::msg::ServiceDiscoveryResponse handle_request(const d2::msg::ServiceDiscoveryRequest& req, d2::Session& session,
                                                 const std::vector<dt::EnergyTransferMode>& energy_transfer_modes,
                                                 const std::vector<dt::PaymentOption>& payment_options,
                                                 const std::vector<dt::Service>& offered_services) {
    d2::msg::ServiceDiscoveryResponse res;
    setup_header(res.header, session);

    dt::ChargeService charge_service;
    charge_service.service_id = 1; // ISO15118-2 Table 105
    charge_service.service_name = "AC_DC_Charging";
    charge_service.supported_energy_transfer_mode = energy_transfer_modes;
    charge_service.service_category = dt::ServiceCategory::EvCharging;
    charge_service.FreeService = true;

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
        std::vector<dt::Service> offered_services;
        for (const auto& id : m_ctx.session_config.offered_services) {
            dt::ServiceCategory service_category = convert_service_id_to_service_category(id);
            if (service_category == dt::ServiceCategory::OtherCustom) {
                auto service = m_ctx.feedback.get_service_from_id(id);
                if (service.has_value()) {
                    offered_services.push_back(service.value());
                } else {
                    // m_ctx.log("Error constructing service from id %d. Feedback function returned no value.", id);
                }
            } else {
                offered_services.push_back(construct_service_from_id(id));
            }
        }

        const auto res = handle_request(*req, m_ctx.session, m_ctx.session_config.supported_energy_transfer_modes,
                                        m_ctx.session_config.supported_payment_options, offered_services);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            // m_ctx.log("Error processing ServiceDiscoveryReq");
            return {};
        }

        return m_ctx.create_state<PaymentServiceSelection>();
    }
    // m_ctx.log("expected ServiceDiscoveryReq! But code type id: %d", variant->get_type());

    // Sequence Error [V2G2-538]
    const msg::Type req_type = variant->get_type();
    send_sequence_error(req_type, m_ctx);

    m_ctx.session_stopped = true;

    return {};
}

} // namespace iso15118::d2::state
