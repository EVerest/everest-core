#include "everestrs_sys.hpp"

#include <cstdlib>
#include <stdexcept>

#include "cxxbridge/lib.rs.h"

namespace {

std::unique_ptr<Everest::Everest> create_everest_instance(const std::string& module_id,
                                                          std::shared_ptr<Everest::RuntimeSettings> rs,
                                                          const Everest::Config& config) {
    return std::make_unique<Everest::Everest>(module_id, config, rs->validate_schema, rs->mqtt_broker_host,
                                              rs->mqtt_broker_port, rs->mqtt_everest_prefix, rs->mqtt_external_prefix,
                                              rs->telemetry_prefix, rs->telemetry_enabled);
}

std::unique_ptr<Everest::Config> create_config_instance(std::shared_ptr<Everest::RuntimeSettings> rs) {
    // FIXME (aw): where to initialize the logger?
    Everest::Logging::init(rs->logging_config_file);
    return std::make_unique<Everest::Config>(rs);
}

JsonBlob json2blob(const json& j) {
    // I did not find a way to not copy the data at least once here.
    const std::string dumped = j.dump();
    rust::Vec<uint8_t> vec;
    vec.reserve(dumped.size());
    std::copy(dumped.begin(), dumped.end(), std::back_inserter(vec));
    return JsonBlob{vec};
}

} // namespace

Module::Module(const std::string& module_id, const std::string& prefix, const std::string& config_file) :
    module_id_(module_id),
    rs_(std::make_shared<Everest::RuntimeSettings>(prefix, config_file)),
    config_(create_config_instance(rs_)),
    handle_(create_everest_instance(module_id, rs_, *config_)) {
}

JsonBlob Module::get_interface(rust::Str interface_name) const {
    const auto& interface_def = config_->get_interface_definition(std::string(interface_name));
    return json2blob(interface_def);
}

JsonBlob Module::initialize() const {
    handle_->connect();
    handle_->spawn_main_loop_thread();

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

void Module::subscribe_variable(const Runtime& rt, rust::String implementation_id, rust::String name) const {
    // TODO(hrapp): I am not sure how to model the multiple slots that could theoretically be here.
    const Requirement req(std::string(implementation_id), 0);
    handle_->subscribe_var(req, std::string(name), [&rt, implementation_id, name](json args) {
        rt.handle_variable(implementation_id, name, json2blob(args));
    });
}

JsonBlob Module::call_command(rust::Str implementation_id, rust::Str name, JsonBlob blob) const {
    // TODO(hrapp): I am not sure how to model the multiple slots that could theoretically be here.
    const Requirement req(std::string(implementation_id), 0);
    json return_value = handle_->call_cmd(req, std::string(name), json::parse(blob.data.begin(), blob.data.end()));

    return json2blob(return_value);
}

std::unique_ptr<Module> create_module(rust::Str module_id, rust::Str prefix, rust::Str conf) {
    return std::make_unique<Module>(std::string(module_id), std::string(prefix), std::string(conf));
}

void Module::publish_variable(rust::Str implementation_id, rust::Str name, JsonBlob blob) const {
    handle_->publish_var(std::string(implementation_id), std::string(name),
                         json::parse(blob.data.begin(), blob.data.end()));
}
