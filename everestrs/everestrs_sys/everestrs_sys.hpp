#pragma once

#include <framework/everest.hpp>
#include <framework/runtime.hpp>
#include <memory>
#include <string>
#include <utils/types.hpp>

#include "cxxbridge/rust.h"

struct JsonBlob;
struct Runtime;
struct RsModuleConfig;
struct ConfigField;
enum class ConfigTypes : uint8_t;

class Module {
public:
    Module(const std::string& module_id, const std::string& prefix, const std::string& conf);

    JsonBlob initialize() const;
    JsonBlob get_interface(rust::Str interface_name) const;

    void signal_ready(const Runtime& rt) const;
    void provide_command(const Runtime& rt, rust::String implementation_id, rust::String name) const;
    JsonBlob call_command(rust::Str implementation_id, rust::Str name, JsonBlob args) const;
    void subscribe_variable(const Runtime& rt, rust::String implementation_id, rust::String name) const;
    void publish_variable(rust::Str implementation_id, rust::Str name, JsonBlob blob) const;

private:
    const std::string module_id_;
    std::shared_ptr<Everest::RuntimeSettings> rs_;
    std::unique_ptr<Everest::Config> config_;
    std::unique_ptr<Everest::Everest> handle_;
};

std::unique_ptr<Module> create_module(rust::Str module_name, rust::Str prefix, rust::Str conf);

rust::Vec<RsModuleConfig> get_module_configs(rust::Str module_name, rust::Str prefix, rust::Str conf);
