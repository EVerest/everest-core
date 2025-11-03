// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSE_SECURITYINTFSTUB_H_
#define EVSE_SECURITYINTFSTUB_H_

#include <iostream>

#include "ModuleAdapterStub.hpp"
#include "generated/types/evse_security.hpp"
#include "utils/types.hpp"
#include <functional>
#include <generated/interfaces/evse_security/Interface.hpp>
#include <optional>
#include <string>

//-----------------------------------------------------------------------------
namespace module::stub {

class evse_securityIntfStub : public evse_securityIntf {
private:
    std::map<const std::string, Result (evse_securityIntfStub::*)(const Requirement& req, const Parameters& args)>
        functions;

public:
    evse_securityIntfStub(ModuleAdapterStub* adapter) :
        evse_securityIntf(adapter, Requirement{"", 0}, "EvseSecurity", std::nullopt) {
        functions["get_verify_file"] = &evse_securityIntfStub::get_verify_file;
        functions["get_leaf_certificate_info"] = &evse_securityIntfStub::get_leaf_certificate_info;
    }
    evse_securityIntfStub(ModuleAdapterStub& adapter) :
        evse_securityIntf(&adapter, Requirement{"", 0}, "EvseSecurity", std::nullopt) {
        functions["get_verify_file"] = &evse_securityIntfStub::get_verify_file;
        functions["get_leaf_certificate_info"] = &evse_securityIntfStub::get_leaf_certificate_info;
    }

    virtual Result call_fn(const Requirement& req, const std::string& str, Parameters args) {
        if (auto it = functions.find(str); it != functions.end()) {
            return std::invoke(it->second, this, req, args);
        }
        std::printf("call_fn (%s)\n", str.c_str());
        return std::nullopt;
    }

    virtual Result get_verify_file(const Requirement& req, const Parameters& args) {
        std::cout << "evse_securityIntf::get_verify_file called" << std::endl;
        return "";
    }

    virtual Result get_leaf_certificate_info(const Requirement& req, const Parameters& args) {
        std::cout << "evse_securityIntf::get_leaf_certificate_info called" << std::endl;
        return "";
    }
};

} // namespace module::stub

#endif // EVSE_SECURITYINTFSTUB_H_
