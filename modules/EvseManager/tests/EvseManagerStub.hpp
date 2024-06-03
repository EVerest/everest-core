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
    virtual bool handle_enable_disable(int& connector_id, types::evse_manager::EnableDisableSource& cmd_source) {
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

    virtual std::shared_ptr<Everest::error::ErrorManagerReq> get_error_manager_req_fn(const Requirement& req) {
        return std::make_shared<Everest::error::ErrorManagerReq>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>({Everest::error::ErrorType("evse_board_support/VendorWarning")}),
            [this](const Everest::error::ErrorType& error_type, const Everest::error::ErrorCallback& callback,
                   const Everest::error::ErrorCallback& clear_callback) {
                error_raise[error_type] = callback;
                error_clear[error_type] = clear_callback;
            });
    }

    virtual std::shared_ptr<Everest::error::ErrorManagerImpl> get_error_manager_impl_fn(const std::string& str) {
        return std::make_shared<Everest::error::ErrorManagerImpl>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>(),
            [this](const Everest::error::Error& error) {
                std::printf("publish_raised_error\n");
                if (error_raise.find(error.type) == error_raise.end()) {
                    throw std::runtime_error("Error type " + error.type + " not found");
                }
                error_raise[error.type](error);
            },
            [this](const Everest::error::Error& error) {
                std::printf("publish_cleared_error\n");
                if (error_raise.find(error.type) == error_raise.end()) {
                    throw std::runtime_error("Error type " + error.type + " not found");
                }
                error_clear[error.type](error);
            },
            false);
    }
};

} // namespace module::stub

#endif
