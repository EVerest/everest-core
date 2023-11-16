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

    py::class_<Interface>(m, "Interface")
        .def_readonly("variables", &Interface::variables)
        .def_readonly("commands", &Interface::commands)
        .def_readonly("errors", &Interface::errors);

    py::class_<Fulfillment>(m, "Fulfillment")
        .def("__repr__",
             [](const Fulfillment& f) {
                 return "<framework.Fulfillment: (" + f.module_id + ":" + f.implementation_id + ")>";
             })
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
        .def("init_done", py::overload_cast<std::function<void()>>(&Module::init_done))
        .def("call_command", &Module::call_command)
        .def("publish_variable", &Module::publish_variable)
        .def("implement_command", &Module::implement_command)
        .def("subscribe_variable", &Module::subscribe_variable)
        .def("raise_error", &Module::raise_error)
        .def("request_clear_error_uuid", &Module::request_clear_error_uuid)
        .def("request_clear_error_all_of_type", &Module::request_clear_error_all_of_type)
        .def("request_clear_error_all_of_module", &Module::request_clear_error_all_of_module)
        .def("subscribe_error", &Module::subscribe_error)
        .def("subscribe_all_errors", &Module::subscribe_all_errors)
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

    m.attr("__version__") = "0.6";
}
