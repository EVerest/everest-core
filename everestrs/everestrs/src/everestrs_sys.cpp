#include "everestrs/src/everestrs_sys.hpp"
#include "everestrs/src/lib.rs.h"

#include <everest/logging.hpp>
#include <utils/error/error_manager_impl.hpp>
#include <utils/error/error_manager_req.hpp>

#include <utils/types.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include <boost/log/attributes/attribute_value_set.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/expressions/filter.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>

namespace {

JsonBlob json2blob(const json& j) {
    // I did not find a way to not copy the data at least once here.
    const std::string dumped = j.dump();
    rust::Vec<uint8_t> vec;
    vec.reserve(dumped.size());
    std::copy(dumped.begin(), dumped.end(), std::back_inserter(vec));
    return JsonBlob{vec};
}

// Below are overloads to be used with std::visit and our std::variant. We force
// a compilation error if someone changes the underlying std::variant without
// extending/adjusting the functions below.

template <typename T, typename... VARIANT_T> struct VariantMemberImpl : public std::false_type {};

template <typename T, typename... VARIANT_T>
struct VariantMemberImpl<T, std::variant<VARIANT_T...>> : public std::disjunction<std::is_same<T, VARIANT_T>...> {};

/// @brief Static checker if the type T can be converted to `ConfigEntry`.
///
/// We use this to detect `get_config_field` overloads which receive arguments
/// which aren't part of our `ConfigEntry` variant.
template <typename T> struct ConfigEntryMember : public VariantMemberImpl<T, ConfigEntry> {};

inline ConfigField get_config_field(const std::string& _name, bool _value) {
    static_assert(ConfigEntryMember<decltype(_value)>::value);
    return {_name, ConfigType::Boolean, _value, {}, 0, 0};
}

inline ConfigField get_config_field(const std::string& _name, const std::string& _value) {
    static_assert(ConfigEntryMember<std::remove_cv_t<std::remove_reference_t<decltype(_value)>>>::value);
    return {_name, ConfigType::String, false, _value, 0, 0};
}

inline ConfigField get_config_field(const std::string& _name, double _value) {
    static_assert(ConfigEntryMember<decltype(_value)>::value);
    return {_name, ConfigType::Number, false, {}, _value, 0};
}

inline ConfigField get_config_field(const std::string& _name, int _value) {
    static_assert(ConfigEntryMember<decltype(_value)>::value);
    return {_name, ConfigType::Integer, false, {}, 0, _value};
}

} // namespace

/// @brief The flag which prevents us from re-initializing the module.
std::once_flag mod_flag;

/// @brief The central handle to the EVerest infrastructure.
std::unique_ptr<Module> mod;

const Module& create_module(rust::Str module_id, rust::Str prefix, rust::Str mqtt_broker_socket_path,
                            rust::Str mqtt_broker_host, const unsigned int& mqtt_broker_port,
                            rust::Str mqtt_everest_prefix, rust::Str mqtt_external_prefix) {
    std::call_once(mod_flag, [&]() {
        auto socket_path = std::string(mqtt_broker_socket_path);
        Everest::MQTTSettings mqtt_settings;
        if (not socket_path.empty()) {
            Everest::populate_mqtt_settings(mqtt_settings, socket_path, std::string(mqtt_everest_prefix),
                                            std::string(mqtt_external_prefix));
        } else {
            Everest::populate_mqtt_settings(mqtt_settings, std::string(mqtt_broker_host), mqtt_broker_port,
                                            std::string(mqtt_everest_prefix), std::string(mqtt_external_prefix));
        }
        mod = std::make_unique<Module>(std::string(module_id), std::string(prefix), mqtt_settings);
    });
    return *mod;
}

Module::Module(const std::string& module_id, const std::string& prefix, const Everest::MQTTSettings& mqtt_settings) :
    module_id_(module_id) {

    const auto mqtt_abstraction = std::make_shared<Everest::MQTTAbstraction>(mqtt_settings);
    // TODO(ddo) what happens when this returns false?
    mqtt_abstraction->connect();
    mqtt_abstraction->spawn_main_loop_thread();

    const auto result = Everest::get_module_config(mqtt_abstraction, this->module_id_);

    this->rs_ = std::make_unique<Everest::RuntimeSettings>(result.at("settings"));

    config_ = std::make_shared<Everest::Config>(mqtt_settings, result);

    handle_ =
        std::make_unique<Everest::Everest>(this->module_id_, *this->config_, this->rs_->validate_schema,
                                           mqtt_abstraction, this->rs_->telemetry_prefix, this->rs_->telemetry_enabled);

    // Not needed but done to be congruent with the other bindings.
    handle_->spawn_main_loop_thread();
}

JsonBlob Module::get_interface(rust::Str interface_name) const {
    const auto& interface_def = config_->get_interface_definition(std::string(interface_name));
    return json2blob(interface_def);
}

JsonBlob Module::get_manifest() const {
    const std::string& module_name = config_->get_main_config().at(module_id_).at("module");
    return json2blob(config_->get_manifests().at(module_name));
}

void Module::signal_ready(const Runtime& rt) const {
    handle_->register_on_ready_handler([&rt]() { rt.on_ready(); });
    handle_->signal_ready();
}

void Module::provide_command(const Runtime& rt, rust::String implementation_id, rust::String name) const {
    handle_->provide_cmd(std::string(implementation_id), std::string(name), [&rt, implementation_id, name](json args) {
        JsonBlob blob = rt.handle_command(implementation_id, name, json2blob(args));
        return json::parse(blob.data.begin(), blob.data.end());
    });
}

void Module::subscribe_variable(const Runtime& rt, rust::String implementation_id, std::size_t index,
                                rust::String name) const {
    const auto req = Requirement{std::string(implementation_id), index};
    // The handle_ptr is guaranteed to be alive in the callback.
    const auto handle_ptr = handle_.get();
    handle_->subscribe_var(req, std::string(name), [&rt, implementation_id, index, name, handle_ptr](json args) {
        handle_ptr->ensure_ready();
        rt.handle_variable(implementation_id, index, name, json2blob(args));
    });
}

void Module::subscribe_all_errors(const Runtime& rt) const {
    for (const Requirement& req : config_->get_requirements(module_id_)) {
        const auto handle_ptr = handle_.get();
        handle_->get_error_manager_req(req)->subscribe_all_errors(
            [&rt, req, handle_ptr](Everest::error::Error error) {
                handle_ptr->ensure_ready();
                const ErrorType rust_error{rust::String(error.type), rust::String(error.description),
                                           rust::String(error.message), static_cast<ErrorSeverity>(error.severity)};
                rt.handle_on_error(rust::Str(req.id), req.index, rust_error, true);
            },
            [&rt, req, handle_ptr](Everest::error::Error error) {
                handle_ptr->ensure_ready();
                const ErrorType rust_error{rust::String(error.type), rust::String(error.description),
                                           rust::String(error.message), static_cast<ErrorSeverity>(error.severity)};
                rt.handle_on_error(rust::Str(req.id), req.index, rust_error, false);
            });
    }
}

JsonBlob Module::call_command(rust::Str implementation_id, std::size_t index, rust::Str name, JsonBlob blob) const {
    const auto req = Requirement{std::string(implementation_id), index};
    json return_value = handle_->call_cmd(req, std::string(name), json::parse(blob.data.begin(), blob.data.end()));

    return json2blob(return_value);
}

void Module::publish_variable(rust::Str implementation_id, rust::Str name, JsonBlob blob) const {
    handle_->publish_var(std::string(implementation_id), std::string(name),
                         json::parse(blob.data.begin(), blob.data.end()));
}

void Module::raise_error(rust::Str implementation_id, ErrorType error_type) const {
    const Everest::error::Error error{std::string(error_type.error_type),
                                      std::string{},
                                      std::string(error_type.message),
                                      std::string(error_type.description),
                                      module_id_,
                                      std::string(implementation_id),
                                      static_cast<Everest::error::Severity>(error_type.severity)};
    handle_->get_error_manager_impl(std::string(implementation_id))->raise_error(error);
}

void Module::clear_error(rust::Str implementation_id, rust::Str error_type, bool clear_all) const {
    const auto manager = handle_->get_error_manager_impl(std::string(implementation_id));

    if (error_type.empty()) {
        manager->clear_all_errors();
    } else {
        if (clear_all) {
            manager->clear_all_errors(std::string(error_type));
        } else {
            manager->clear_error(std::string(error_type));
        }
    }
}

rust::Vec<RsModuleConnections> Module::get_module_connections() const {
    const auto connections = config_->get_main_config().at(std::string(module_id_))["connections"];

    // Iterate over the connections block.
    rust::Vec<RsModuleConnections> out;
    out.reserve(connections.size());
    for (const auto& connection : connections.items()) {
        out.emplace_back(RsModuleConnections{rust::String{connection.key()}, connection.value().size()});
    };
    return out;
}

rust::Vec<RsModuleConfig> Module::get_module_configs(rust::Str module_id) const {
    // TODO(ddo) We call this before initializing the logger.
    const auto module_configs = config_->get_module_configs(std::string(module_id));

    rust::Vec<RsModuleConfig> out;
    out.reserve(module_configs.size());

    // Iterate over all modules stored in the module_config.
    for (const auto& mm : module_configs) {
        RsModuleConfig mm_out{mm.first, {}};
        mm_out.data.reserve(mm.second.size());

        // Iterate over all configs stored in the mm (our current module).
        for (const auto& cc : mm.second) {
            mm_out.data.emplace_back(
                std::visit([&](auto&& _value) { return ::get_config_field(cc.first, _value); }, cc.second));
        }
        out.emplace_back(std::move(mm_out));
    }

    return out;
}

int init_logging(rust::Str module_id, rust::Str prefix, rust::Str logging_config_file) {
    using namespace boost::log;
    using namespace Everest::Logging;

    const std::string module_id_cpp{module_id};
    const std::string prefix_cpp{prefix};
    const std::string logging_config_file_cpp{logging_config_file};

    // Init the CPP logger.
    init(logging_config_file_cpp, module_id_cpp);

    // Below is something really ugly. Boost's log filter rules may actually be
    // quite "complex" but the library does not expose any way to check the
    // already installed filters. We therefore reopen the config and construct
    // or own filter - and feed it with dummy values to determine its filtering
    // behaviour (the lowest severity which is accepted by the filter)
    std::filesystem::path logging_path{logging_config_file_cpp};
    std::ifstream logging_config(logging_path.c_str());
    if (!logging_config.is_open()) {
        return info;
    }
    const auto settings = parse_settings(logging_config);

    if (auto core_settings = settings["Core"]) {
        if (boost::optional<std::string> param = core_settings["Filter"]) {

            const auto filter = parse_filter(param.get());
            // Check for the severity values - which is the first one to be
            // accepted by the filter.
            static_assert(static_cast<int>(verbose) == 0);
            static_assert(static_cast<int>(error) == 4);
            for (int ii = static_cast<int>(verbose); ii <= static_cast<int>(error); ++ii) {
                attribute_set set1, set2, set3;
                set1["Severity"] = attributes::constant<severity_level>{static_cast<severity_level>(ii)};

                attribute_value_set value_set(set1, set2, set3);
                value_set.freeze();

                if (filter(value_set)) {
                    return ii;
                }
            }
        }
    }

    return info;
}

void log2cxx(int level, int line, rust::Str file, rust::Str message) {
    const auto logging_level = static_cast<::Everest::Logging::severity_level>(level);
    BOOST_LOG_SEV(::global_logger::get(), logging_level)
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("file", std::string{file})
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("line", line) << std::string{message};
}
