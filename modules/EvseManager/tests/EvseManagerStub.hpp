// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSEMANAGERSTUB_H_
#define EVSEMANAGERSTUB_H_

#include "ModuleAdapterStub.hpp"
#include <ErrorHandling.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

struct evse_managerImplStub : public evse_managerImplBase {
    evse_managerImplStub() : evse_managerImplBase(nullptr, "manager") {
    }
    virtual void init() {
    }
    virtual void ready() {
    }
    virtual types::evse_manager::Evse handle_get_evse() {
        return types::evse_manager::Evse();
    }
    virtual bool handle_enable(int& connector_id) {
        return true;
    }
    virtual bool handle_disable(int& connector_id) {
        return true;
    }
    virtual void handle_authorize_response(types::authorization::ProvidedIdToken& provided_token,
                                           types::authorization::ValidationResult& validation_result) {
    }
    virtual void handle_withdraw_authorization() {
    }
    virtual bool handle_reserve(int& reservation_id) {
        return true;
    }
    virtual void handle_cancel_reservation() {
    }
    virtual void handle_set_faulted() {
    }
    virtual bool handle_pause_charging() {
        return true;
    }
    virtual bool handle_resume_charging() {
        return true;
    }
    virtual bool handle_stop_transaction(types::evse_manager::StopTransactionRequest& request) {
        return true;
    }
    virtual bool handle_force_unlock(int& connector_id) {
        return true;
    }
    virtual void handle_set_external_limits(types::energy::ExternalLimits& value) {
    }
    virtual types::evse_manager::SwitchThreePhasesWhileChargingResult
    handle_switch_three_phases_while_charging(bool& three_phases) {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Success;
    }
    virtual void
    handle_set_get_certificate_response(types::iso15118_charger::Response_Exi_Stream_Status& certificate_response) {
    }
    virtual bool handle_external_ready_to_start_charging() {
        return true;
    }
};

struct EvseManagerModuleAdapter : public ModuleAdapterStub {
    EvseManagerModuleAdapter() : id("evse_manager", "main") {
    }

    ImplementationIdentifier id;
    std::map<std::string, Everest::error::ErrorCallback> error_raise;
    std::map<std::string, Everest::error::ErrorCallback> error_clear;

    virtual void subscribe_error_fn(const Requirement&, const std::string& str, Everest::error::ErrorCallback cb) {
        // std::printf("subscribe_error_fn %s\n", str.c_str());
        error_raise[str] = cb;
    }

    virtual void subscribe_error_cleared_fn(const Requirement&, const std::string& str,
                                            Everest::error::ErrorCallback cb) {
        // std::printf("subscribe_error_cleared_fn %s\n", str.c_str());
        error_clear[str] = cb;
    }

    void raise_error(const std::string& error_type, const std::string& error_message, const std::string& error_desc) {
        Everest::error::Error error(error_type, error_message, error_desc, id);
        error_raise[error_type](error);
    }

    void clear_error(const std::string& error_type, const std::string& error_message, const std::string& error_desc) {
        Everest::error::Error error(error_type, error_message, error_desc, id);
        error_clear[error_type](error);
    }
};

} // namespace module::stub

#endif
