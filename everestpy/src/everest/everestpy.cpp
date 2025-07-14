// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <filesystem>
#include <fstream>

#include <cstdlib>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <pybind11_json/pybind11_json.hpp>

#include "misc.hpp"
#include "module.hpp"

#include <utils/error.hpp>
#include <utils/error/error_factory.hpp>
#include <utils/error/error_state_monitor.hpp>

namespace py = pybind11;

PYBIND11_MODULE(everestpy, m) {

    // FIXME (aw): add m.doc?
    py::class_<RuntimeSession>(m, "RuntimeSession")
        .def(py::init<>())
        .def(py::init<const std::string&, const std::string&>());

    py::class_<ModuleInfo::Paths>(m, "ModuleInfoPaths")
        .def_readonly("etc", &ModuleInfo::Paths::etc)
        .def_readonly("libexec", &ModuleInfo::Paths::libexec)
        .def_readonly("share", &ModuleInfo::Paths::share);

    py::class_<ModuleInfo>(m, "ModuleInfo")
        .def_readonly("name", &ModuleInfo::name)
        .def_readonly("authors", &ModuleInfo::authors)
        .def_readonly("license", &ModuleInfo::license)
        .def_readonly("id", &ModuleInfo::id)
        .def_readonly("paths", &ModuleInfo::paths)
        .def_readonly("telemetry_enabled", &ModuleInfo::telemetry_enabled);

    auto error_submodule = m.def_submodule("error");

    py::enum_<Everest::error::Severity>(error_submodule, "Severity")
        .value("Low", Everest::error::Severity::Low)
        .value("Medium", Everest::error::Severity::Medium)
        .value("High", Everest::error::Severity::High)
        .export_values();

    py::class_<ImplementationIdentifier>(error_submodule, "ImplementationIdentifier")
        .def(py::init<const std::string&, const std::string&, std::optional<Mapping>>())
        .def_readwrite("module_id", &ImplementationIdentifier::module_id)
        .def_readwrite("implementation_id", &ImplementationIdentifier::implementation_id)
        .def_readwrite("mapping", &ImplementationIdentifier::mapping);

    py::class_<Everest::error::UUID>(error_submodule, "UUID")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def_readwrite("uuid", &Everest::error::UUID::uuid);

    py::enum_<Everest::error::State>(error_submodule, "State")
        .value("Active", Everest::error::State::Active)
        .value("ClearedByModule", Everest::error::State::ClearedByModule)
        .value("ClearedByReboot", Everest::error::State::ClearedByReboot)
        .export_values();

    py::class_<Everest::error::Error>(error_submodule, "Error")
        .def(py::init<>())
        .def_readwrite("type", &Everest::error::Error::type)
        .def_readwrite("sub_type", &Everest::error::Error::sub_type)
        .def_readwrite("description", &Everest::error::Error::description)
        .def_readwrite("message", &Everest::error::Error::message)
        .def_readwrite("severity", &Everest::error::Error::severity)
        .def_readwrite("origin", &Everest::error::Error::origin)
        .def_readwrite("timestamp", &Everest::error::Error::timestamp)
        .def_readwrite("uuid", &Everest::error::Error::uuid)
        .def_readwrite("state", &Everest::error::Error::state);

    py::class_<Everest::error::ErrorStateMonitor::StateCondition>(error_submodule, "ErrorStateCondition")
        .def(py::init<std::string, std::string, bool>())
        .def_readwrite("type", &Everest::error::ErrorStateMonitor::StateCondition::type)
        .def_readwrite("sub_type", &Everest::error::ErrorStateMonitor::StateCondition::sub_type)
        .def_readwrite("active", &Everest::error::ErrorStateMonitor::StateCondition::active);

    py::class_<Everest::error::ErrorStateMonitor, std::shared_ptr<Everest::error::ErrorStateMonitor>>(
        error_submodule, "ErrorStateMonitor")
        .def("is_error_active", &Everest::error::ErrorStateMonitor::is_error_active)
        .def("is_condition_satisfied", py::overload_cast<const Everest::error::ErrorStateMonitor::StateCondition&>(
                                           &Everest::error::ErrorStateMonitor::is_condition_satisfied, py::const_))
        .def("is_condition_satisfied",
             py::overload_cast<const std::list<Everest::error::ErrorStateMonitor::StateCondition>&>(
                 &Everest::error::ErrorStateMonitor::is_condition_satisfied, py::const_));

    py::class_<Everest::error::ErrorFactory, std::shared_ptr<Everest::error::ErrorFactory>>(error_submodule,
                                                                                            "ErrorFactory")
        .def("create_error", py::overload_cast<>(&Everest::error::ErrorFactory::create_error, py::const_))
        .def("create_error",
             py::overload_cast<const Everest::error::ErrorType&, const Everest::error::ErrorSubType&,
                               const std::string&>(&Everest::error::ErrorFactory::create_error, py::const_))
        .def("create_error", py::overload_cast<const Everest::error::ErrorType&, const Everest::error::ErrorSubType&,
                                               const std::string&, Everest::error::Severity>(
                                 &Everest::error::ErrorFactory::create_error, py::const_))
        .def("create_error", py::overload_cast<const Everest::error::ErrorType&, const Everest::error::ErrorSubType&,
                                               const std::string&, Everest::error::State>(
                                 &Everest::error::ErrorFactory::create_error, py::const_))
        .def("create_error", py::overload_cast<const Everest::error::ErrorType&, const Everest::error::ErrorSubType&,
                                               const std::string&, Everest::error::Severity, Everest::error::State>(
                                 &Everest::error::ErrorFactory::create_error, py::const_));

    py::class_<Interface>(m, "Interface")
        .def_readonly("variables", &Interface::variables)
        .def_readonly("commands", &Interface::commands)
        .def_readonly("errors", &Interface::errors);

    py::class_<Fulfillment>(m, "Fulfillment")
        .def_readonly("module_id", &Fulfillment::module_id)
        .def_readonly("implementation_id", &Fulfillment::implementation_id);

    py::class_<ModuleSetup::Configurations>(m, "ModuleSetupConfigurations")
        .def_readonly("implementations", &ModuleSetup::Configurations::implementations)
        .def_readonly("module", &ModuleSetup::Configurations::module);

    py::class_<ModuleSetup>(m, "ModuleSetup")
        .def_readonly("configs", &ModuleSetup::configs)
        .def_readonly("connections", &ModuleSetup::connections);

    py::class_<Module>(m, "Module")
        .def(py::init<const RuntimeSession&>())
        .def(py::init<const std::string&, const RuntimeSession&>())
        .def("say_hello", &Module::say_hello)
        .def("init_done", py::overload_cast<>(&Module::init_done))
        .def("init_done", py::overload_cast<const std::function<void()>&>(&Module::init_done))
        .def("call_command", &Module::call_command)
        .def("publish_variable", &Module::publish_variable)
        .def("implement_command", &Module::implement_command)
        .def("subscribe_variable", &Module::subscribe_variable)
        .def("raise_error", &Module::raise_error)
        .def("clear_error",
             py::overload_cast<const std::string&, const Everest::error::ErrorType&>(&Module::clear_error),
             py::arg("impl_id"), py::arg("type"))
        .def("clear_error",
             py::overload_cast<const std::string&, const Everest::error::ErrorType&,
                               const Everest::error::ErrorSubType&>(&Module::clear_error),
             py::arg("impl_id"), py::arg("type"), py::arg("sub_type"))
        .def("clear_all_errors_of_impl", py::overload_cast<const std::string&>(&Module::clear_all_errors_of_impl),
             py::arg("impl_id"))
        .def("clear_all_errors_of_impl",
             py::overload_cast<const std::string&, const Everest::error::ErrorType&>(&Module::clear_all_errors_of_impl),
             py::arg("impl_id"), py::arg("type"))
        .def("get_error_state_monitor_impl", &Module::get_error_state_monitor_impl)
        .def("get_error_factory", &Module::get_error_factory)
        .def("subscribe_error", &Module::subscribe_error)
        .def("subscribe_all_errors", &Module::subscribe_all_errors)
        .def("get_error_state_monitor_req", &Module::get_error_state_monitor_req)
        .def_property_readonly("fulfillments", &Module::get_fulfillments)
        .def_property_readonly("implementations", &Module::get_implementations)
        .def_property_readonly("requirements", &Module::get_requirements)
        .def_property_readonly("info", &Module::get_info);

    auto log_submodule = m.def_submodule("log");
    log_submodule.def("update_process_name",
                      [](const std::string& process_name) { Everest::Logging::update_process_name(process_name); });

    log_submodule.def("verbose", [](const std::string& message) { EVLOG_verbose << message; });
    log_submodule.def("debug", [](const std::string& message) { EVLOG_debug << message; });
    log_submodule.def("info", [](const std::string& message) { EVLOG_info << message; });
    log_submodule.def("warning", [](const std::string& message) { EVLOG_warning << message; });
    log_submodule.def("error", [](const std::string& message) { EVLOG_error << message; });
    log_submodule.def("critical", [](const std::string& message) { EVLOG_critical << message; });

    m.attr("__version__") = "0.22.1";
}
