#include "everestrs_sys.hpp"

#include <cstdlib>
#include <stdexcept>

#include "everestrs/lib.rs.h"

namespace {

std::unique_ptr<Everest::Everest> create_everest_instance(const std::string& module_id,
                                                          std::shared_ptr<RuntimeSettings> rs,
                                                          const Everest::Config& config) {
    return std::make_unique<Everest::Everest>(module_id, config, , rs->validate_schema, rs->mqtt_broker_host,
                                              rs->mqtt_broker_port, rs->mqtt_everest_prefix, rs->mqtt_external_prefix,
                                              rs->telemetry_prefix, rs->telemetry_enabled);
}

std::unique_ptr<Everest::Config> create_config_instance(std::shared_ptr<RuntimeSettings> rs) {
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
    rs_(prefix, config_file),
    config_(create_config_instance(rs_)),
    handle_(create_everest_instance(module_id, rs_, *config_)) {
}

JsonBlob Module::get_interface(rust::Str interface_name) const {
    const auto& interface_def = config_->get_interface_definition(std::string(interface_name));
    return json2blob(interface_def);
}

JsonBlob Module::initialize() {
    handle_->connect();
    handle_->spawn_main_loop_thread();

    const std::string& module_name = config_->get_main_config().at(module_id_).at("module");
    return json2blob(config_->get_manifests().at(module_name));
}

void Module::signal_ready(const Runtime& rt) const {
    handle_->register_on_ready_handler([&rt]() { rt.on_ready(); });
    handle_->signal_ready();
}

void Module::provide_command(const Runtime& rt, const CommandMeta& meta) const {
    handle_->provide_cmd(std::string(meta.implementation_id), std::string(meta.name), [&rt, meta](json args) {
        JsonBlob blob = rt.handle_command(meta, json2blob(args));
        return json::parse(blob.data.begin(), blob.data.end());
    });
}

std::unique_ptr<Module> create_module(rust::Str module_id, rust::Str prefix, rust::Str conf) {
    return std::make_unique<Module>(std::string(module_id), std::string(prefix), std::string(conf));
}
