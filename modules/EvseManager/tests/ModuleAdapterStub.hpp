// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef MODULEADAPRERSTUB_H_
#define MODULEADAPRERSTUB_H_

#include <framework/ModuleAdapter.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

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
    virtual void subscribe_fn(const Requirement&, const std::string& fn, ValueCallback) {
        std::printf("subscribe_fn(%s)\n", fn.c_str());
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

} // namespace module::stub

#endif
