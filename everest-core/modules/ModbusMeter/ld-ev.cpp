//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "ModbusMeter.hpp"
#include "main/powermeterImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter {};
static Everest::PtrContainer<ModbusMeter> mod_ptr {};

// per module configs
static main::Conf main_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module ModbusMeter";

    // populate config for provided implementations
    auto main_config_input = std::move(module_configs["main"]);
    main_config.modbus_ip_address = boost::get<std::string>(main_config_input["modbus_ip_address"]);
    main_config.modbus_port = boost::get<int>(main_config_input["modbus_port"]);
    main_config.power_unit_id = boost::get<int>(main_config_input["power_unit_id"]);
    main_config.power_in_register = boost::get<int>(main_config_input["power_in_register"]);
    main_config.power_in_length = boost::get<int>(main_config_input["power_in_length"]);
    main_config.power_out_register = boost::get<int>(main_config_input["power_out_register"]);
    main_config.power_out_length = boost::get<int>(main_config_input["power_out_length"]);
    main_config.energy_unit_id = boost::get<int>(main_config_input["energy_unit_id"]);
    main_config.energy_in_register = boost::get<int>(main_config_input["energy_in_register"]);
    main_config.energy_in_length = boost::get<int>(main_config_input["energy_in_length"]);
    main_config.energy_out_register = boost::get<int>(main_config_input["energy_out_register"]);
    main_config.energy_out_length = boost::get<int>(main_config_input["energy_out_length"]);
    main_config.update_interval = boost::get<int>(main_config_input["update_interval"]);


    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module ModbusMeter";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for ModbusMeter";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for ModbusMeter";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for ModbusMeter";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for ModbusMeter";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for ModbusMeter";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module ModbusMeter";

    adapter.check_complete();

    auto p_main = std::make_unique<main::powermeterImpl>(&adapter, mod_ptr, main_config);
    adapter.gather_cmds(*p_main);


    static Everest::MqttProvider mqtt_provider(adapter);

    static ModbusMeter module(mqtt_provider, std::move(p_main), module_conf);

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
