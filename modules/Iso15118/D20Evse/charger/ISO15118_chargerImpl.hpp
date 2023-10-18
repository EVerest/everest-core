// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CHARGER_ISO15118_CHARGER_IMPL_HPP
#define CHARGER_ISO15118_CHARGER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

#include "../D20Evse.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace charger {

struct Conf {};

class ISO15118_chargerImpl : public ISO15118_chargerImplBase {
public:
    ISO15118_chargerImpl() = delete;
    ISO15118_chargerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<D20Evse>& mod, Conf& config) :
        ISO15118_chargerImplBase(ev, "charger"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_setup(types::iso15118_charger::EVSEID& evse_id,
                              std::vector<types::iso15118_charger::EnergyTransferMode>& supported_energy_transfer_modes,
                              types::iso15118_charger::SAE_J2847_Bidi_Mode& sae_j2847_mode, bool& debug_mode,
                              types::iso15118_charger::SetupPhysicalValues& physical_values) override;
    virtual void handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                      bool& supported_certificate_service) override;
    virtual void
    handle_certificate_response(types::iso15118_charger::Response_Exi_Stream_Status& exi_stream_status) override;
    virtual void handle_authorization_response(types::authorization::AuthorizationStatus& authorization_status,
                                               types::authorization::CertificateStatus& certificate_status) override;
    virtual void handle_ac_contactor_closed(bool& status) override;
    virtual void handle_dlink_ready(bool& value) override;
    virtual void handle_cable_check_finished(bool& status) override;
    virtual void handle_receipt_is_required(bool& receipt_required) override;
    virtual void handle_stop_charging(bool& stop) override;
    virtual void handle_update_ac_max_current(double& max_current) override;
    virtual void
    handle_update_dc_maximum_limits(types::iso15118_charger::DC_EVSEMaximumLimits& maximum_limits) override;
    virtual void
    handle_update_dc_minimum_limits(types::iso15118_charger::DC_EVSEMinimumLimits& minimum_limits) override;
    virtual void handle_update_isolation_status(types::iso15118_charger::IsolationStatus& isolation_status) override;
    virtual void handle_update_dc_present_values(
        types::iso15118_charger::DC_EVSEPresentVoltage_Current& present_voltage_current) override;
    virtual void handle_update_meter_info(types::powermeter::Powermeter& powermeter) override;
    virtual void handle_send_error(types::iso15118_charger::EvseError& error) override;
    virtual void handle_reset_error() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<D20Evse>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace charger
} // namespace module

#endif // CHARGER_ISO15118_CHARGER_IMPL_HPP
