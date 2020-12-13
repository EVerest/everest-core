//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "Example.hpp"
#include "example/example_childImpl.hpp"
#include "store/kvsImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter{};
static Everest::PtrContainer<Example> mod_ptr{};

// per module configs
static example::Conf example_config;
static store::Conf store_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module Example";

    // populate config for provided implementations
    auto example_config_input = std::move(module_configs["example"]);
    example_config.current = boost::get<double>(example_config_input["current"]);
    example_config.enum_test = boost::get<std::string>(example_config_input["enum_test"]);
    example_config.enum_test2 = boost::get<int>(example_config_input["enum_test2"]);

    auto store_config_input = std::move(module_configs["store"]);

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module Example";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for Example";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for Example";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for Example";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for Example";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for Example";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module Example";

    adapter.check_complete();

    auto p_example = std::make_unique<example::example_childImpl>(&adapter, mod_ptr, example_config);
    adapter.gather_cmds(*p_example);

    auto p_store = std::make_unique<store::kvsImpl>(&adapter, mod_ptr, store_config);
    adapter.gather_cmds(*p_store);

    auto r_kvs = std::make_unique<kvsIntf>(&adapter, "kvs");
    auto r_powerin = std::make_unique<power_inIntf>(&adapter, "powerin");
    auto r_solar = std::make_unique<powerIntf>(&adapter, "optional:solar");

    static Everest::MqttProvider mqtt_provider(adapter);

    static Example module(mqtt_provider, std::move(p_example), std::move(p_store), std::move(r_kvs),
                          std::move(r_powerin), std::move(r_solar), module_conf);

    mod_ptr.set(&module);

    return adapter.registered_commands;
}

} // namespace module

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::LdEverest::init, init)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::LdEverest::ready, ready)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::everest_register, everest_register)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_call_handler, everest_register_call_cmd_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_publish_handler, everest_register_publish_var_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_subscribe_handler, everest_register_subscribe_var_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_ext_mqtt_publish_handler, everest_register_external_mqtt_publish_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_ext_mqtt_subscribe_handler, everest_register_external_mqtt_handler_callback)
