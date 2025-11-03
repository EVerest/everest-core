//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "YetiDriver.hpp"
#include "powermeter/powermeterImpl.hpp"
#include "board_support/board_support_ACImpl.hpp"
#include "yeti_extras/yeti_extrasImpl.hpp"
#include "debug_yeti/debug_jsonImpl.hpp"
#include "debug_powermeter/debug_jsonImpl.hpp"
#include "debug_state/debug_jsonImpl.hpp"
#include "debug_keepalive/debug_jsonImpl.hpp"
#include "yeti_simulation_control/yeti_simulation_controlImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter {};
static Everest::PtrContainer<YetiDriver> mod_ptr {};

// per module configs
static powermeter::Conf powermeter_config;
static board_support::Conf board_support_config;
static yeti_extras::Conf yeti_extras_config;
static debug_yeti::Conf debug_yeti_config;
static debug_powermeter::Conf debug_powermeter_config;
static debug_state::Conf debug_state_config;
static debug_keepalive::Conf debug_keepalive_config;
static yeti_simulation_control::Conf yeti_simulation_control_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module YetiDriver";

    // populate config for provided implementations
    auto powermeter_config_input = std::move(module_configs["powermeter"]);

    auto board_support_config_input = std::move(module_configs["board_support"]);

    auto yeti_extras_config_input = std::move(module_configs["yeti_extras"]);

    auto debug_yeti_config_input = std::move(module_configs["debug_yeti"]);

    auto debug_powermeter_config_input = std::move(module_configs["debug_powermeter"]);

    auto debug_state_config_input = std::move(module_configs["debug_state"]);

    auto debug_keepalive_config_input = std::move(module_configs["debug_keepalive"]);

    auto yeti_simulation_control_config_input = std::move(module_configs["yeti_simulation_control"]);


    module_conf.serial_port = boost::get<std::string>(module_configs["!module"]["serial_port"]);
    module_conf.baud_rate = boost::get<int>(module_configs["!module"]["baud_rate"]);
    module_conf.control_mode = boost::get<std::string>(module_configs["!module"]["control_mode"]);

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module YetiDriver";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for YetiDriver";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for YetiDriver";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for YetiDriver";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for YetiDriver";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for YetiDriver";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module YetiDriver";

    adapter.check_complete();

    auto p_powermeter = std::make_unique<powermeter::powermeterImpl>(&adapter, mod_ptr, powermeter_config);
    adapter.gather_cmds(*p_powermeter);

    auto p_board_support = std::make_unique<board_support::board_support_ACImpl>(&adapter, mod_ptr, board_support_config);
    adapter.gather_cmds(*p_board_support);

    auto p_yeti_extras = std::make_unique<yeti_extras::yeti_extrasImpl>(&adapter, mod_ptr, yeti_extras_config);
    adapter.gather_cmds(*p_yeti_extras);

    auto p_debug_yeti = std::make_unique<debug_yeti::debug_jsonImpl>(&adapter, mod_ptr, debug_yeti_config);
    adapter.gather_cmds(*p_debug_yeti);

    auto p_debug_powermeter = std::make_unique<debug_powermeter::debug_jsonImpl>(&adapter, mod_ptr, debug_powermeter_config);
    adapter.gather_cmds(*p_debug_powermeter);

    auto p_debug_state = std::make_unique<debug_state::debug_jsonImpl>(&adapter, mod_ptr, debug_state_config);
    adapter.gather_cmds(*p_debug_state);

    auto p_debug_keepalive = std::make_unique<debug_keepalive::debug_jsonImpl>(&adapter, mod_ptr, debug_keepalive_config);
    adapter.gather_cmds(*p_debug_keepalive);

    auto p_yeti_simulation_control = std::make_unique<yeti_simulation_control::yeti_simulation_controlImpl>(&adapter, mod_ptr, yeti_simulation_control_config);
    adapter.gather_cmds(*p_yeti_simulation_control);


    static Everest::MqttProvider mqtt_provider(adapter);

    static YetiDriver module(mqtt_provider, std::move(p_powermeter), std::move(p_board_support), std::move(p_yeti_extras), std::move(p_debug_yeti), std::move(p_debug_powermeter), std::move(p_debug_state), std::move(p_debug_keepalive), std::move(p_yeti_simulation_control), module_conf);

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
