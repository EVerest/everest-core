#pragma once

#include <framework/everest.hpp>
#include <framework/runtime.hpp>
#include <memory>
#include <string>
#include <utils/types.hpp>

#include "rust/cxx.h"

struct JsonBlob;
struct Runtime;
struct RsModuleConfig;
struct RsModuleConnections;
struct ConfigField;
struct ErrorType;

enum class ConfigTypes : uint8_t;
enum class ErrorState : uint8_t;
enum class ErrorSeverity : uint8_t;

class Module {
public:
    Module(const std::string& module_id, const std::string& prefix, const std::string& conf);

    JsonBlob initialize() const;
    JsonBlob get_interface(rust::Str interface_name) const;

    void signal_ready(const Runtime& rt) const;
    void provide_command(const Runtime& rt, rust::String implementation_id, rust::String name) const;
    JsonBlob call_command(rust::Str implementation_id, size_t index, rust::Str name, JsonBlob args) const;
    void subscribe_variable(const Runtime& rt, rust::String implementation_id, size_t index, rust::String name) const;
    void subscribe_all_errors(const Runtime& rt) const;

    void publish_variable(rust::Str implementation_id, rust::Str name, JsonBlob blob) const;
    rust::Vec<RsModuleConnections> get_module_connections() const;

    void raise_error(rust::Str implementation_id, ErrorType error_type) const;

    void clear_error(rust::Str implementation_id, rust::Str error_type, bool clear_all) const;

private:
    const std::string module_id_;
    std::shared_ptr<Everest::RuntimeSettings> rs_;
    std::unique_ptr<Everest::Config> config_;
    std::unique_ptr<Everest::Everest> handle_;
};

std::unique_ptr<Module> create_module(rust::Str module_name, rust::Str prefix, rust::Str conf);

rust::Vec<RsModuleConfig> get_module_configs(rust::Str module_name, rust::Str prefix, rust::Str conf);

int init_logging(rust::Str module_name, rust::Str prefix, rust::Str conf);
void log2cxx(int level, int line, rust::Str file, rust::Str message);
