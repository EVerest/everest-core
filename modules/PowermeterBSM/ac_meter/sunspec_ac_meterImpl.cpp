// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "sunspec_ac_meterImpl.hpp"

namespace module {
namespace ac_meter {

void sunspec_ac_meterImpl::init() {
}

void sunspec_ac_meterImpl::ready() {
}

types::sunspec_ac_meter::Result sunspec_ac_meterImpl::handle_get_sunspec_ac_meter_value(std::string& auth_token) {
    // your code for cmd get_sunspec_ac_meter_value goes here
    EVLOG_verbose << __PRETTY_FUNCTION__ << " start " << std::endl;

    types::sunspec_ac_meter::Result result{};

    try {

        std::scoped_lock lock(mod->get_device_mutex());

        transport::AbstractDataTransport::Spt transport = mod->get_data_transport();

        transport::DataVector data = transport->fetch(known_model::Sunspec_ACMeter);

        sunspec_model::ACMeter acmeter(data);

        result.A = acmeter.A();
        result.AphA = acmeter.AphA();
        result.AphB = acmeter.AphB();
        result.AphC = acmeter.AphC();
        result.A_SF = acmeter.A_SF();
        result.PhVphA = acmeter.PhVphA();
        result.PhVphB = acmeter.PhVphB();
        result.PhVphC = acmeter.PhVphC();
        result.V_SF = acmeter.V_SF();
        result.Hz = acmeter.Hz();
        result.Hz_SF = acmeter.Hz_SF();
        result.W = acmeter.W();
        result.WphA = acmeter.WphA();
        result.WphB = acmeter.WphB();
        result.WphC = acmeter.WphC();
        result.W_SF = acmeter.W_SF();
        result.VA = acmeter.VA();
        result.VAphA = acmeter.VAphA();
        result.VAphB = acmeter.VAphB();
        result.VAphC = acmeter.VAphC();
        result.VA_SF = acmeter.VAR_SF();
        result.VAR = acmeter.VAR();
        result.VARphA = acmeter.VARphA();
        result.VARphB = acmeter.VARphB();
        result.VARphC = acmeter.VARphC();
        result.VAR_SF = acmeter.VAR_SF();
        result.PFphA = acmeter.PFphA();
        result.PFphB = acmeter.PFphB();
        result.PFphC = acmeter.PFphC();
        result.PF_SF = acmeter.PF_SF();
        result.TotWhIm = acmeter.TotWhIm();
        result.TotWh_SF = acmeter.TotWh_SF();
        result.Evt = acmeter.Evt();

    } catch (const std::runtime_error& e) {
        EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
    }

    return result;
};

} // namespace ac_meter
} // namespace module
