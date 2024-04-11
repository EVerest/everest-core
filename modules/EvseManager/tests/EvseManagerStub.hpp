// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSEMANAGERSTUB_H_
#define EVSEMANAGERSTUB_H_

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

struct ModuleAdapterStub : public Everest::ModuleAdapter {

    ModuleAdapterStub() : Everest::ModuleAdapter() {
        call = [this](const Requirement& req, const std::string& str, Parameters p) {
            return this->call_fn(req, str, p);
        };
        publish = [this](const std::string& s1, const std::string& s2, Value v) { this->publish_fn(s1, s2, v); };
        subscribe = [this](const Requirement& req, const std::string& str, ValueCallback cb) {
            this->subscribe_fn(req, str, cb);
        };
        subscribe_error = [this](const Requirement& req, const std::string& str, Everest::error::ErrorCallback cb) {
            this->subscribe_error_fn(req, str, cb);
        };
        subscribe_all_errors = [this](Everest::error::ErrorCallback cb) { this->subscribe_all_errors_fn(cb); };
        subscribe_error_cleared = [this](const Requirement& req, const std::string& str,
                                         Everest::error::ErrorCallback cb) {
            this->subscribe_error_cleared_fn(req, str, cb);
        };
        subscribe_all_errors_cleared = [this](Everest::error::ErrorCallback cb) {
            subscribe_all_errors_cleared_fn(cb);
        };
        raise_error = [this](const std::string& s1, const std::string& s2, const std::string& s3,
                             const Everest::error::Severity& sev) { return this->raise_error_fn(s1, s2, s3, sev); };
        request_clear_error_uuid = [this](const std::string& str, const Everest::error::ErrorHandle& eh) {
            return this->request_clear_error_uuid_fn(str, eh);
        };
        request_clear_all_errors_of_module = [this](const std::string& str) {
            return this->request_clear_all_errors_of_module_fn(str);
        };
        request_clear_all_errors_of_type_of_module = [this](const std::string& s1, const std::string& s2) {
            return this->request_clear_all_errors_of_type_of_module_fn(s1, s2);
        };
        ext_mqtt_publish = [this](const std::string& s1, const std::string& s2) { this->ext_mqtt_publish_fn(s1, s2); };
        ext_mqtt_subscribe = [this](const std::string& str, StringHandler sh) {
            return this->ext_mqtt_subscribe_fn(str, sh);
        };
        telemetry_publish = [this](const std::string& s1, const std::string& s2, const std::string& s3,
                                   const Everest::TelemetryMap& tm) { this->telemetry_publish_fn(s1, s2, s3, tm); };
    }

    virtual Result call_fn(const Requirement&, const std::string&, Parameters) {
        std::printf("call_fn\n");
        return std::nullopt;
    }
    virtual void publish_fn(const std::string&, const std::string&, Value) {
        std::printf("publish_fn\n");
    }
    virtual void subscribe_fn(const Requirement&, const std::string&, ValueCallback) {
        std::printf("subscribe_fn\n");
    }
    virtual void subscribe_error_fn(const Requirement&, const std::string&, Everest::error::ErrorCallback) {
        std::printf("subscribe_error_fn\n");
    }
    virtual void subscribe_all_errors_fn(Everest::error::ErrorCallback) {
        std::printf("subscribe_all_errors_fn\n");
    }
    virtual void subscribe_error_cleared_fn(const Requirement&, const std::string&, Everest::error::ErrorCallback) {
        std::printf("subscribe_error_cleared_fn\n");
    }
    virtual void subscribe_all_errors_cleared_fn(Everest::error::ErrorCallback) {
        std::printf("subscribe_all_errors_cleared_fn\n");
    }
    virtual Everest::error::ErrorHandle raise_error_fn(const std::string&, const std::string&, const std::string&,
                                                       const Everest::error::Severity&) {
        std::printf("raise_error_fn\n");
        return Everest::error::ErrorHandle();
    }
    virtual Result request_clear_error_uuid_fn(const std::string&, const Everest::error::ErrorHandle&) {
        std::printf("request_clear_error_uuid_fn\n");
        return std::nullopt;
    }
    virtual Result request_clear_all_errors_of_module_fn(const std::string&) {
        std::printf("request_clear_all_errors_of_module_fn\n");
        return std::nullopt;
    }
    virtual Result request_clear_all_errors_of_type_of_module_fn(const std::string&, const std::string&) {
        std::printf("request_clear_all_errors_of_type_of_module_fn\n");
        return std::nullopt;
    }
    virtual void ext_mqtt_publish_fn(const std::string&, const std::string&) {
        std::printf("ext_mqtt_publish_fn\n");
    }
    virtual std::function<void()> ext_mqtt_subscribe_fn(const std::string&, StringHandler) {
        std::printf("ext_mqtt_subscribe_fn\n");
        return nullptr;
    }
    virtual void telemetry_publish_fn(const std::string&, const std::string&, const std::string&,
                                      const Everest::TelemetryMap&) {
        std::printf("telemetry_publish_fn\n");
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
