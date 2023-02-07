// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11_json/pybind11_json.hpp>

#include <framework/ModuleAdapter.hpp>
#include <framework/everest.hpp>
#include <framework/runtime.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;
namespace py = pybind11;

// support for boost::variant from
// https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#c-17-library-containers
namespace pybind11 {
namespace detail {
template <typename... Ts> struct type_caster<boost::variant<Ts...>> : variant_caster<boost::variant<Ts...>> {};

// Specifies the function used to visit the variant -- `apply_visitor` instead of `visit`
template <> struct visit_helper<boost::variant> {
    template <typename... Args> static auto call(Args&&... args) -> decltype(boost::apply_visitor(args...)) {
        return boost::apply_visitor(args...);
    }
};
} // namespace detail
} // namespace pybind11

struct Log {
    static void verbose(const std::string& message) {
        EVLOG_verbose << message;
    }
    static void debug(const std::string& message) {
        EVLOG_debug << message;
    }
    static void info(const std::string& message) {
        EVLOG_info << message;
    }
    static void warning(const std::string& message) {
        EVLOG_warning << message;
    }
    static void error(const std::string& message) {
        EVLOG_error << message;
    }
    static void critical(const std::string& message) {
        EVLOG_critical << message;
    }
};

struct CmdWithArguments {
    std::function<json(json json_value)> cmd;
    json arguments;
};

struct RequirementMinMax {
    size_t min_connections;
    size_t max_connections;
};

struct Reqs {
    std::map<std::string,
             std::map<std::string, std::map<std::string, std::function<void(std::function<void(json json_value)>)>>>>
        vars;
    std::map<std::string, std::map<std::string, std::function<void(json json_value)>>> pub_vars;
    std::map<std::string, std::map<std::string, std::map<std::string, CmdWithArguments>>> call_cmds;
    std::map<std::string, std::map<std::string, json>> pub_cmds;
    bool enable_external_mqtt = false;
    std::map<std::string, RequirementMinMax> requirements;
};

struct EverestPyCmd {
    std::string impl_id;
    std::string cmd_name;
    std::function<json(json parameters)> handler;
};

struct EverestPy {
    ::Everest::ModuleCallbacks module_callbacks;
    std::function<void(const Reqs& reqs)> pre_init;
    std::function<std::vector<EverestPyCmd>(const json& connections)> everest_register;
};

EverestPy everest_py;

void register_module_adapter_callback(
    const std::function<void(::Everest::ModuleAdapter module_adapter)>& register_module_adapter) {
    everest_py.module_callbacks.register_module_adapter = register_module_adapter;
}

void register_everest_register_callback(
    const std::function<std::vector<EverestPyCmd>(const json& connections)>& everest_register) {
    everest_py.everest_register = everest_register;
}

void register_init_callback(const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init) {
    everest_py.module_callbacks.init = init;
}

void register_pre_init_callback(const std::function<void(const Reqs& reqs)>& pre_init) {
    everest_py.pre_init = pre_init;
}

void register_ready_callback(const std::function<void()>& ready) {
    everest_py.module_callbacks.ready = ready;
}

int initialize(const std::string& prefix, const std::string& config_file, const std::string& module_id) {
    auto rs = Everest::RuntimeSettings(prefix, config_file);
    Everest::Logging::init(rs.logging_config_file.string(), module_id);

    try {
        Everest::Config config = Everest::Config(
            rs.schemas_dir.string(), rs.config_file.string(), rs.modules_dir.string(), rs.interfaces_dir.string(),
            rs.types_dir.string(), rs.mqtt_everest_prefix, rs.mqtt_external_prefix);

        if (!config.contains(module_id)) {
            EVLOG_error << fmt::format("Module id '{}' not found in config!", module_id);
            return 2;
        }

        const std::string module_identifier = config.printable_identifier(module_id);
        EVLOG_info << fmt::format("Initializing framework for module {}...", module_identifier);
        EVLOG_debug << fmt::format("Setting process name to: '{}'...", module_identifier);
        int prctl_return = prctl(PR_SET_NAME, module_identifier.c_str());
        if (prctl_return == 1) {
            EVLOG_warning << fmt::format("Could not set process name to '{}'.", module_identifier);
        }
        Everest::Logging::update_process_name(module_identifier);

        Everest::Everest& everest =
            Everest::Everest::get_instance(module_id, config, rs.validate_schema, rs.mqtt_broker_host,
                                           rs.mqtt_broker_port, rs.mqtt_everest_prefix, rs.mqtt_external_prefix);

        EVLOG_info << fmt::format("Initializing module {}...", module_identifier);

        const std::string& module_name = config.get_main_config()[module_id]["module"].get<std::string>();
        auto module_manifest = config.get_manifests()[module_name];
        auto module_impls = config.get_interfaces()[module_name];

        Reqs reqs;
        reqs.enable_external_mqtt = module_manifest.at("enable_external_mqtt");

        // module provides
        for (const auto& impl_definition : module_impls.items()) {
            const auto& impl_id = impl_definition.key();
            const auto& impl_intf = module_impls[impl_id];

            std::map<std::string, std::function<void(std::function<void(json json_value)>)>> impl_vars_prop;
            if (impl_intf.contains("vars")) {
                for (const auto& var_entry : impl_intf["vars"].items()) {
                    const auto& var_name = var_entry.key();
                    reqs.pub_vars[impl_id][var_name] = [&everest, impl_id, var_name](json json_value) {
                        everest.publish_var(impl_id, var_name, json_value);
                    };
                }
            }

            if (impl_intf.contains("cmds")) {
                for (const auto& cmd_entry : impl_intf["cmds"].items()) {
                    const auto& cmd_name = cmd_entry.key();
                    reqs.pub_cmds[impl_id][cmd_name] = cmd_entry.value();
                }
            }
        }

        // module requires (uses)
        reqs.vars = {};
        reqs.call_cmds = {};
        for (auto& requirement : module_manifest["requires"].items()) {
            auto const& requirement_id = requirement.key();
            json req_route_list = config.resolve_requirement(module_id, requirement_id);
            // if this was a requirement with min_connections == 1 and max_connections == 1,
            // this will be simply a single connection, but an array of connections otherwise
            // (this array can have only one entry, if only one connection was provided, though)
            const bool is_list = req_route_list.is_array();
            if (!is_list) {
                req_route_list = json::array({req_route_list});
            }

            auto value = requirement.value();
            RequirementMinMax r;
            r.min_connections = value.at("min_connections");
            r.max_connections = value.at("max_connections");
            reqs.requirements[requirement_id] = r;

            reqs.vars[requirement_id] = {};

            reqs.call_cmds[requirement_id] = {};

            for (size_t i = 0; i < req_route_list.size(); i++) {
                auto req_route = req_route_list[i];
                const std::string& requirement_module_id = req_route["module_id"];
                reqs.vars[requirement_id][requirement_module_id] = {};
                reqs.call_cmds[requirement_id][requirement_module_id] = {};
                const std::string& requirement_impl_id = req_route["implementation_id"];
                std::string interface_name = req_route["required_interface"].get<std::string>();
                auto requirement_impl_intf = config.get_interface_definition(interface_name);
                auto requirement_vars = Everest::Config::keys(requirement_impl_intf["vars"]);
                auto requirement_cmds = Everest::Config::keys(requirement_impl_intf["cmds"]);

                for (auto var_name : requirement_vars) {
                    reqs.vars[requirement_id][requirement_module_id][var_name] =
                        [&everest, requirement_id, i, var_name](std::function<void(json json_value)> callback) {
                            everest.subscribe_var({requirement_id, i}, var_name, callback);
                        };
                }

                for (auto const& cmd_name : requirement_cmds) {
                    reqs.call_cmds[requirement_id][requirement_module_id][cmd_name] = {
                        [&everest, requirement_id, i, cmd_name](json parameters) {
                            return everest.call_cmd({requirement_id, i}, cmd_name, parameters);
                        },
                        requirement_impl_intf.at("cmds").at(cmd_name).at("arguments")};
                }
            }
        }

        if (!everest.connect()) {
            EVLOG_critical << fmt::format("Cannot connect to MQTT broker at {}:{}", rs.mqtt_broker_host,
                                          rs.mqtt_broker_port);
            return 1;
        }

        Everest::ModuleAdapter module_adapter;

        module_adapter.call = [&everest](const Requirement& req, const std::string& cmd_name, Parameters args) {
            return everest.call_cmd(req, cmd_name, args);
        };

        module_adapter.publish = [&everest](const std::string& param1, const std::string& param2, Value param3) {
            return everest.publish_var(param1, param2, param3);
        };

        module_adapter.subscribe = [&everest](const Requirement& req, const std::string& var_name,
                                              const ValueCallback& callback) {
            return everest.subscribe_var(req, var_name, callback);
        };

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_publish = std::bind(&::Everest::Everest::external_mqtt_publish, &everest,
                                                    std::placeholders::_1, std::placeholders::_2);

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_subscribe = std::bind(&::Everest::Everest::provide_external_mqtt_handler, &everest,
                                                      std::placeholders::_1, std::placeholders::_2);

        everest_py.module_callbacks.register_module_adapter(module_adapter);

        everest_py.pre_init(reqs);

        // FIXME (aw): would be nice to move this config related thing toward the module_init function
        std::vector<EverestPyCmd> cmds =
            everest_py.everest_register(config.get_main_config()[module_id]["connections"]);

        for (auto const& command : cmds) {
            everest.provide_cmd(command.impl_id, command.cmd_name, command.handler);
        }

        auto module_configs = config.get_module_configs(module_id);
        const auto module_info = config.get_module_info(module_id);

        everest_py.module_callbacks.init(module_configs, module_info);

        everest.spawn_main_loop_thread();

        // register the modules ready handler with the framework
        // this handler gets called when the global ready signal is received
        everest.register_on_ready_handler(everest_py.module_callbacks.ready);

        // the module should now be ready
        everest.signal_ready();
    } catch (boost::exception& e) {
        EVLOG_critical << fmt::format("Caught top level boost::exception:\n{}", boost::diagnostic_information(e, true));
    } catch (std::exception& e) {
        EVLOG_critical << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
    }

    return 0;
}

PYBIND11_MODULE(everestpy, m) {
    m.doc() = R"pbdoc(
        Python bindings for EVerest
        -----------------------
        .. currentmodule:: everestpy
        .. autosummary::
           :toctree: _generate
           log
    )pbdoc";

    py::class_<EverestPyCmd>(m, "EverestPyCmd")
        .def(py::init<>())
        .def_readwrite("impl_id", &EverestPyCmd::impl_id)
        .def_readwrite("cmd_name", &EverestPyCmd::cmd_name)
        .def_readwrite("handler", &EverestPyCmd::handler);

    py::bind_vector<std::vector<EverestPyCmd>>(m, "EverestPyCmdVector");

    py::class_<Log>(m, "log")
        .def(py::init<>())
        .def_static("verbose", &Log::verbose)
        .def_static("debug", &Log::debug)
        .def_static("info", &Log::info)
        .def_static("warning", &Log::warning)
        .def_static("error", &Log::error)
        .def_static("critical", &Log::critical);

    py::class_<::Everest::ModuleAdapter>(m, "ModuleAdapter")
        .def(py::init<>())
        .def_readwrite("ext_mqtt_publish", &::Everest::ModuleAdapter::ext_mqtt_publish)
        .def_readwrite("ext_mqtt_subscribe", &::Everest::ModuleAdapter::ext_mqtt_subscribe);

    py::class_<ConfigEntry>(m, "ConfigEntry").def(py::init<>());
    py::class_<ConfigMap>(m, "ConfigMap").def(py::init<>());
    py::class_<ModuleConfigs>(m, "ModuleConfigs").def(py::init<>());
    py::class_<ModuleInfo>(m, "ModuleInfo").def(py::init<>());
    py::class_<CmdWithArguments>(m, "CmdWithArguments")
        .def(py::init<>())
        .def_readwrite("cmd", &CmdWithArguments::cmd)
        .def_readwrite("arguments", &CmdWithArguments::arguments);
    py::class_<RequirementMinMax>(m, "RequirementMinMax")
        .def(py::init<>())
        .def_readwrite("min_connections", &RequirementMinMax::min_connections)
        .def_readwrite("max_connections", &RequirementMinMax::max_connections);
    py::class_<Reqs>(m, "Reqs")
        .def(py::init<>())
        .def_readwrite("vars", &Reqs::vars)
        .def_readwrite("pub_vars", &Reqs::pub_vars)
        .def_readwrite("call_cmds", &Reqs::call_cmds)
        .def_readwrite("pub_cmds", &Reqs::pub_cmds)
        .def_readwrite("enable_external_mqtt", &Reqs::enable_external_mqtt)
        .def_readwrite("requirements", &Reqs::requirements);

    m.def("init", [](const std::string& prefix, const std::string& config_file, const std::string& module_id) {
        initialize(prefix, config_file, module_id);
    });

    m.def("register_module_adapter_callback", &register_module_adapter_callback);
    m.def("register_everest_register_callback", &register_everest_register_callback);
    m.def("register_init_callback", &register_init_callback);
    m.def("register_pre_init_callback", &register_pre_init_callback);
    m.def("register_ready_callback", &register_ready_callback);

    m.attr("__version__") = "0.3";
}
