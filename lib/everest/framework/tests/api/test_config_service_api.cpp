// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest
#include <map>
#include <set>
#include <sstream>
#include <string>

#include <catch2/catch_all.hpp>
#include <nlohmann/json.hpp>

#include <config_service_api.hpp>
#include <config_service_type_wrapper.hpp>
#include <everest_api_types/config_service/codec.hpp>
#include <utils/config_service_interface.hpp>

using json = nlohmann::json;
using namespace everest::config;


TEST_CASE("ConfigServiceAPI::get_config_value", "[config_service]") {
    ModuleConfig config;

    config.module_id = "module_id";
    config.module_name = "Module Name";
    config.standalone = true;
    config.capabilities = std::nullopt;
    config.telemetry_enabled = true;
    config.telemetry_config = std::make_optional<TelemetryConfig>(5);

    ConfigurationParameterCharacteristics characteristics1;
    characteristics1.datatype = Datatype::Integer;
    characteristics1.mutability = Mutability::ReadWrite;
    characteristics1.unit = "ms";

    ConfigurationParameterCharacteristics characteristics2;
    characteristics2.datatype = Datatype::String;
    characteristics2.mutability = Mutability::ReadOnly;

    ConfigurationParameterCharacteristics characteristics4;
    characteristics4.datatype = Datatype::Decimal;
    characteristics4.mutability = Mutability::ReadWrite;

    ConfigurationParameterCharacteristics characteristics5;
    characteristics5.datatype = Datatype::Boolean;
    characteristics5.mutability = Mutability::WriteOnly;

    ConfigurationParameter param1;
    param1.name = "integer_param";
    param1.value = 10;
    param1.characteristics = characteristics1;

    ConfigurationParameter param2;
    param2.name = "string_param";
    param2.value = std::string("example_value");
    param2.characteristics = characteristics2;

    ConfigurationParameter param4;
    param4.name = "decimal_param";
    param4.value = 42.23;
    param4.characteristics = characteristics4;

    ConfigurationParameter param5;
    param5.name = "boolean_param";
    param5.value = true;
    param5.characteristics = characteristics5;

    config.configuration_parameters["!module"].push_back({param1});
    config.configuration_parameters["impl_1"].push_back({param2});
    config.configuration_parameters["!module"].push_back({param4});
    config.configuration_parameters["!module"].push_back({param5});

    Fulfillment f1{"module_a", "impl_a1", {"conn1", 0}};
    Fulfillment f2{"module_a", "impl_a2", {"conn2", 0}};
    Fulfillment f3{"module_b", "impl_b1", {"conn3", 0}};
    Fulfillment f4{"module_c", "impl_b2", {"conn4", 0}};
    Fulfillment f5{"module_d", "impl_b2", {"conn4", 1}};
    Fulfillment f6{"module_e", "impl_b2", {"conn4", 2}};

    config.connections = {
        {"conn1", {f1}},
        {"conn2", {f2}},
        {"conn3", {f3}},
        {"conn4", {f4, f5, f6}},
    };

    ModuleTierMappings mtm;
    mtm.module = std::make_optional<Mapping>(1, 2);
    mtm.implementations = {
        {"impl_1", std::make_optional<Mapping>(3)},
        {"impl_2", std::make_optional<Mapping>(6)},
        {"impl_3", std::make_optional<Mapping>(4, 5)},
    };
    config.mapping = mtm;

    ModuleConfigAccess mca;
    mca.allow_read = true;
    mca.allow_write = false;
    mca.allow_set_read_only = true;
    ConfigAccess ca;
    ca.allow_global_read = false;
    ca.allow_global_write = true;
    ca.allow_set_read_only = false;
    ca.modules["other_module_id"] = mca;
    config.access = {ca};

    SECTION("ModuleConfig round-trip conversion") {
        auto external_config = Everest::api::types::config_service::to_external_api(config);
        auto serialized_config = serialize(external_config);
        auto deserialized_external_config = everest::lib::API::V1_0::types::config_service::deserialize<
            everest::lib::API::V1_0::types::config_service::ModuleConfiguration>(serialized_config);
        auto internal_config = Everest::api::types::config_service::to_internal_api(deserialized_external_config);

        CHECK(internal_config == config);
    }
}
