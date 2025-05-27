// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ISO15118_CHARGERIMPLSTUB_H_
#define ISO15118_CHARGERIMPLSTUB_H_

#include <iostream>
#include <memory>

#include "ModuleAdapterStub.hpp"

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

struct ISO15118_chargerImplStub : public ISO15118_chargerImplBase {
    ISO15118_chargerImplStub(ModuleAdapterStub& adapter) : ISO15118_chargerImplBase(&adapter, "EvseV2G"){};
    ISO15118_chargerImplStub(ModuleAdapterStub* adapter) : ISO15118_chargerImplBase(adapter, "EvseV2G"){};

    virtual void init() {
    }
    virtual void ready() {
    }

    virtual void handle_setup(types::iso15118::EVSEID& evse_id,
                              std::vector<types::iso15118::SupportedEnergyMode>& supported_energy_transfer_modes,
                              types::iso15118::SaeJ2847BidiMode& sae_j2847_mode, bool& debug_mode) {
        std::cout << "ISO15118_chargerImplBase::handle_setup called" << std::endl;
    }
    virtual void handle_set_charging_parameters(types::iso15118::SetupPhysicalValues& physical_values) {
        std::cout << "ISO15118_chargerImplBase::handle_set_charging_parameters called" << std::endl;
    }
    virtual void handle_session_setup(std::vector<types::iso15118::PaymentOption>& payment_options,
                                      bool& supported_certificate_service, bool& central_contract_validation_allowed) {
        std::cout << "ISO15118_chargerImplBase::handle_session_setup called" << std::endl;
    }
    virtual void handle_authorization_response(types::authorization::AuthorizationStatus& authorization_status,
                                               types::authorization::CertificateStatus& certificate_status) {
        std::cout << "ISO15118_chargerImplBase::handle_authorization_response called" << std::endl;
    }
    virtual void handle_ac_contactor_closed(bool& status) {
        std::cout << "ISO15118_chargerImplBase::handle_ac_contactor_closed called" << std::endl;
    }
    virtual void handle_dlink_ready(bool& value) {
        std::cout << "ISO15118_chargerImplBase::handle_dlink_ready called" << std::endl;
    }
    virtual void handle_cable_check_finished(bool& status) {
        std::cout << "ISO15118_chargerImplBase::handle_cable_check_finished called" << std::endl;
    }
    virtual void handle_receipt_is_required(bool& receipt_required) {
        std::cout << "ISO15118_chargerImplBase::handle_receipt_is_required called" << std::endl;
    }
    virtual void handle_stop_charging(bool& stop) {
        std::cout << "ISO15118_chargerImplBase::handle_stop_charging called" << std::endl;
    }
    virtual void handle_update_ac_max_current(double& max_current) {
        std::cout << "ISO15118_chargerImplBase::handle_update_ac_max_current called" << std::endl;
    }
    virtual void handle_update_dc_maximum_limits(types::iso15118::DcEvseMaximumLimits& maximum_limits) {
        std::cout << "ISO15118_chargerImplBase::handle_update_dc_maximum_limits called" << std::endl;
    }
    virtual void handle_update_dc_minimum_limits(types::iso15118::DcEvseMinimumLimits& minimum_limits) {
        std::cout << "ISO15118_chargerImplBase::handle_update_dc_minimum_limits called" << std::endl;
    }
    virtual void handle_update_isolation_status(types::iso15118::IsolationStatus& isolation_status) {
        std::cout << "ISO15118_chargerImplBase::handle_update_isolation_status called" << std::endl;
    }
    virtual void
    handle_update_dc_present_values(types::iso15118::DcEvsePresentVoltageCurrent& present_voltage_current) {
        std::cout << "ISO15118_chargerImplBase::handle_update_dc_present_values called" << std::endl;
    }
    virtual void handle_update_meter_info(types::powermeter::Powermeter& powermeter) {
        std::cout << "ISO15118_chargerImplBase::handle_update_meter_info called" << std::endl;
    }
    virtual void handle_send_error(types::iso15118::EvseError& error) {
        std::cout << "ISO15118_chargerImplBase::handle_send_error called" << std::endl;
    }
    virtual void handle_reset_error() {
        std::cout << "ISO15118_chargerImplBase::handle_reset_error called" << std::endl;
    }
};

} // namespace module::stub

#endif // ISO15118_CHARGERIMPLSTUB_H_
