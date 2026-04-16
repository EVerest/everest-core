// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "OCPPExtensionExample.hpp"
#include "module_stub.hpp"

namespace {

// ----------------------------------------------------------------------------
// the test
// WARNING - do not add additional tests - they won't work
//
// despite Setup2 being a complete copy of Setup test it always fails
// It looks like part of the EVerest module steup is keeping a pointer to
// ExtendedModuleAdapter and as soon as a the old one goes out of scope or
// a new one is created then a segmentation fault occurs
// it is possibly related to the this pointer capture in the lambdas or
// the static objects in ld-ev.cpp

TEST(OCPPExtension, Setup) {
    module::stub::ExtendedModuleAdapter adapter;
    module::stub::OCPPExtensionExampleStub module(adapter);

    ModuleInfo module_info{"ocpp_extension", {}, "Apache-2.0", "ocpp_ext", {"/etc", "/libexec", "/share"}, false, false,
                           std::nullopt};
    RequirementInitialization req;
    module::register_module_adapter(adapter);
    std::vector<Everest::cmd> commands = module::everest_register(req);
    std::cout << "commands: count " << commands.size() << '\n';
    for (const auto& i : commands) {
        std::cout << "id: " << i.impl_id << " name: " << i.cmd_name << '\n';
    }
    json p = R"({"request":{"data":"Hello","vendor_id":"EVerest"}})"_json;
    EVLOG_debug << p.dump();
    auto res = commands[0].cmd(p);
    EVLOG_debug << res.value().dump();
    adapter.register_commands(commands);
    module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"Pionix"}})"_json);

    ModuleConfigs configs = R"({
        "data_transfer": {},
        "!module":{
            "enable": true,
            "poll_interval": 0.0,
            "id": 0,
            "keys_to_monitor": ""
        }
    })"_json;

    module::LdEverest::init(configs, module_info);
    module::LdEverest::ready();
    module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"EVerest"}})"_json);
}

#if 0
TEST(OCPPExtension, Setup2) {
    stubs::ExtendedModuleAdapter adapter;
    stubs::OCPPExtensionExampleStub module(adapter);

    ModuleInfo module_info{"ocpp_extension", {}, "Apache-2.0", "ocpp_ext", {"/etc", "/libexec", "/share"}, false, false,
                           std::nullopt};
    RequirementInitialization req;
    module::register_module_adapter(adapter);
    std::vector<Everest::cmd> commands = module::everest_register(req);
    std::cout << "commands: count " << commands.size() << '\n';
    for (const auto& i : commands) {
        std::cout << "id: " << i.impl_id << " name: " << i.cmd_name << '\n';
    }
    json p = R"({"request":{"data":"Hello","vendor_id":"EVerest"}})"_json;
    EVLOG_debug << p.dump();
    auto res = commands[0].cmd(p);
    EVLOG_debug << res.value().dump();
    adapter.register_commands(commands);
    module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"Pionix"}})"_json);
    ModuleConfigs configs;
    module::LdEverest::init(configs, module_info);
    module::LdEverest::ready();
    module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"EVerest"}})"_json);
}
#endif

} // namespace
