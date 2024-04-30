// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_chargerImpl.hpp"

namespace module {
namespace main {

void ISO15118_chargerImpl::init() {
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_setup(
    types::iso15118_charger::EVSEID& evse_id,
    std::vector<types::iso15118_charger::EnergyTransferMode>& supported_energy_transfer_modes,
    types::iso15118_charger::SAE_J2847_Bidi_Mode& sae_j2847_mode, bool& debug_mode) {
    // your code for cmd setup goes here
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                                bool& supported_certificate_service) {
    // your code for cmd session_setup goes here
}

void ISO15118_chargerImpl::handle_certificate_response(
    types::iso15118_charger::Response_Exi_Stream_Status& exi_stream_status) {
    // your code for cmd certificate_response goes here
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

void ISO15118_chargerImpl::handle_set_physical_values(types::iso15118_charger::SetupPhysicalValues& physical_values) {
}

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    // your code for cmd update_ac_max_current goes here
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(
    types::iso15118_charger::DC_EVSEMaximumLimits& maximum_limits) {
    // your code for cmd update_dc_maximum_limits goes here
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(
    types::iso15118_charger::DC_EVSEMinimumLimits& minimum_limits) {
    // your code for cmd update_dc_minimum_limits goes here
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118_charger::IsolationStatus& isolation_status) {
    // your code for cmd update_isolation_status goes here
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& present_voltage_current) {
    // your code for cmd update_dc_present_values goes here
}

void ISO15118_chargerImpl::handle_update_meter_info(types::powermeter::Powermeter& powermeter) {
    // your code for cmd update_meter_info goes here
}

void ISO15118_chargerImpl::handle_send_error(types::iso15118_charger::EvseError& error) {
    // your code for cmd send_error goes here
}

void ISO15118_chargerImpl::handle_reset_error() {
    // your code for cmd reset_error goes here
}

} // namespace main
} // namespace module
