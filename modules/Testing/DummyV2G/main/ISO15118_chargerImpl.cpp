// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_chargerImpl.hpp"

namespace module {
namespace main {

void ISO15118_chargerImpl::init() {
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_setup(types::iso15118::EVSEID& evse_id,
                                        types::iso15118::SaeJ2847BidiMode& sae_j2847_mode, bool& debug_mode) {
    // your code for cmd setup goes here
}

void ISO15118_chargerImpl::handle_set_charging_parameters(types::iso15118::SetupPhysicalValues& physical_values) {
    // your code for cmd set_charging_parameters goes here
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118::PaymentOption>& payment_options,
                                                bool& supported_certificate_service,
                                                bool& central_contract_validation_allowed) {
    // your code for cmd session_setup goes here
}

void ISO15118_chargerImpl::handle_bpt_setup(types::iso15118::BptSetup& bpt_config) {
    // your code for cmd bpt_setup goes here
}

void ISO15118_chargerImpl::handle_set_powersupply_capabilities(types::power_supply_DC::Capabilities& capabilities) {
    // your code for cmd set_powersupply_capabilities goes here
}

void ISO15118_chargerImpl::handle_authorization_response(
    types::authorization::AuthorizationStatus& authorization_status,
    types::authorization::CertificateStatus& certificate_status) {
    // your code for cmd authorization_response goes here
}

void ISO15118_chargerImpl::handle_ac_contactor_closed(bool& status) {
    // your code for cmd ac_contactor_closed goes here
}

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
    // your code for cmd dlink_ready goes here
}

void ISO15118_chargerImpl::handle_cable_check_finished(bool& status) {
    // your code for cmd cable_check_finished goes here
}

void ISO15118_chargerImpl::handle_receipt_is_required(bool& receipt_required) {
    // your code for cmd receipt_is_required goes here
}

void ISO15118_chargerImpl::handle_stop_charging(bool& stop) {
    // your code for cmd stop_charging goes here
}

void ISO15118_chargerImpl::handle_pause_charging(bool& pause) {
    // your code for cmd pause_charging goes here
}

void ISO15118_chargerImpl::handle_no_energy_pause_charging(types::iso15118::NoEnergyPauseMode& mode) {
    // your code for cmd no_energy_pause_charging goes here
}

bool ISO15118_chargerImpl::handle_update_supported_app_protocols(
    types::iso15118::SupportedAppProtocols& supported_app_protocols) {
    // your code for cmd no_energy_pause_charging goes here
    return false;
}

void ISO15118_chargerImpl::handle_update_energy_transfer_modes(
    std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes) {
    // your code for cmd update_energy_transfer_modes goes here
}

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    // your code for cmd update_ac_max_current goes here
}

void ISO15118_chargerImpl::handle_update_ac_parameters(types::iso15118::AcParameters& ac_parameters) {
    // your code for cmd update_ac_parameters goes here
}

void ISO15118_chargerImpl::handle_update_ac_maximum_limits(types::iso15118::AcEvseMaximumPower& maximum_limits) {
    // your code for cmd update_ac_maximum_limits goes here
}

void ISO15118_chargerImpl::handle_update_ac_minimum_limits(types::iso15118::AcEvseMinimumPower& minimum_limits) {
    // your code for cmd update_ac_minimum_limits goes here
}

void ISO15118_chargerImpl::handle_update_ac_target_values(types::iso15118::AcTargetValues& target_values) {
    // your code for cmd update_ac_target_values goes here
}

void ISO15118_chargerImpl::handle_update_ac_present_power(types::units::Power& present_power) {
    // your code for cmd update_ac_present_power goes here
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(types::iso15118::DcEvseMaximumLimits& maximum_limits) {
    // your code for cmd update_dc_maximum_limits goes here
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(types::iso15118::DcEvseMinimumLimits& minimum_limits) {
    // your code for cmd update_dc_minimum_limits goes here
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118::IsolationStatus& isolation_status) {
    // your code for cmd update_isolation_status goes here
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118::DcEvsePresentVoltageCurrent& present_voltage_current) {
    // your code for cmd update_dc_present_values goes here
}

void ISO15118_chargerImpl::handle_update_meter_info(types::powermeter::Powermeter& powermeter) {
    // your code for cmd update_meter_info goes here
}

void ISO15118_chargerImpl::handle_send_error(types::iso15118::EvseError& error) {
    // your code for cmd send_error goes here
}

void ISO15118_chargerImpl::handle_reset_error() {
    // your code for cmd reset_error goes here
}

} // namespace main
} // namespace module
