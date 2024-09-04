// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/config.hpp>

#include <algorithm>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d20 {

static std::vector<message_20::DcParameterList> get_default_dc_parameter_list() {
    return {{
        message_20::DcConnector::Extended,
        message_20::ControlMode::Scheduled,
        message_20::MobilityNeedsMode::ProvidedByEvcc,
        message_20::Pricing::NoPricing,
    }};
}

static std::vector<message_20::DcBptParameterList> get_default_dc_bpt_parameter_list() {

    return {{{
                 message_20::DcConnector::Extended,
                 message_20::ControlMode::Scheduled,
                 message_20::MobilityNeedsMode::ProvidedByEvcc,
                 message_20::Pricing::NoPricing,
             },
             message_20::BptChannel::Unified,
             message_20::GeneratorMode::GridFollowing}};
}

SessionConfig::SessionConfig(EvseSetupConfig config) :
    evse_id(std::move(config.evse_id)),
    supported_energy_transfer_services(std::move(config.supported_energy_services)),
    cert_install_service(config.enable_certificate_install_service),
    authorization_services(std::move(config.authorization_services)),
    dc_limits(std::move(config.dc_limits)) {

    const auto is_bpt_service = [](message_20::ServiceCategory service) {
        return service == message_20::ServiceCategory::DC_BPT;
    };
    const auto dc_bpt_found = std::any_of(supported_energy_transfer_services.begin(),
                                          supported_energy_transfer_services.end(), is_bpt_service);

    if (dc_bpt_found and not dc_limits.discharge_limits.has_value()) {
        logf_warning("The supported energy services contain DC_BPT, but dc limits does not contain BPT "
                     "limits. This can lead to session shutdowns.");
    }

    dc_parameter_list = get_default_dc_parameter_list();
    dc_bpt_parameter_list = get_default_dc_bpt_parameter_list();
}

} // namespace iso15118::d20
