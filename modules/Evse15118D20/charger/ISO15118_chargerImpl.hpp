// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CHARGER_ISO15118_CHARGER_IMPL_HPP
#define CHARGER_ISO15118_CHARGER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

#include "../Evse15118D20.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <bitset>

#include "utils.hpp"

#include <iso15118/d20/config.hpp>
#include <iso15118/session/feedback.hpp>
#include <iso15118/tbd_controller.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace charger {

struct Conf {};

class ISO15118_chargerImpl : public ISO15118_chargerImplBase {
public:
    ISO15118_chargerImpl() = delete;
    ISO15118_chargerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<Evse15118D20>& mod, Conf& config) :
        ISO15118_chargerImplBase(ev, "charger"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_setup(types::iso15118::EVSEID& evse_id,
                              std::vector<types::iso15118::SupportedEnergyMode>& supported_energy_transfer_modes,
                              types::iso15118::SaeJ2847BidiMode& sae_j2847_mode, bool& debug_mode) override;
    virtual void handle_set_charging_parameters(types::iso15118::SetupPhysicalValues& physical_values) override;
    virtual void handle_session_setup(std::vector<types::iso15118::PaymentOption>& payment_options,
                                      bool& supported_certificate_service) override;
    virtual void handle_authorization_response(types::authorization::AuthorizationStatus& authorization_status,
                                               types::authorization::CertificateStatus& certificate_status) override;
    virtual void handle_ac_contactor_closed(bool& status) override;
    virtual void handle_dlink_ready(bool& value) override;
    virtual void handle_cable_check_finished(bool& status) override;
    virtual void handle_receipt_is_required(bool& receipt_required) override;
    virtual void handle_stop_charging(bool& stop) override;
    virtual void handle_update_ac_max_current(double& max_current) override;
    virtual void handle_update_dc_maximum_limits(types::iso15118::DcEvseMaximumLimits& maximum_limits) override;
    virtual void handle_update_dc_minimum_limits(types::iso15118::DcEvseMinimumLimits& minimum_limits) override;
    virtual void handle_update_isolation_status(types::iso15118::IsolationStatus& isolation_status) override;
    virtual void
    handle_update_dc_present_values(types::iso15118::DcEvsePresentVoltageCurrent& present_voltage_current) override;
    virtual void handle_update_meter_info(types::powermeter::Powermeter& powermeter) override;
    virtual void handle_send_error(types::iso15118::EvseError& error) override;
    virtual void handle_reset_error() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<Evse15118D20>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    iso15118::session::feedback::Callbacks create_callbacks();

    std::unique_ptr<iso15118::TbdController> controller;

    iso15118::d20::EvseSetupConfig setup_config;
    std::bitset<NUMBER_OF_SETUP_STEPS> setup_steps_done{0};
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace charger
} // namespace module

#endif // CHARGER_ISO15118_CHARGER_IMPL_HPP
