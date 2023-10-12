#pragma once

#include <framework/everest.hpp>
#include <framework/runtime.hpp>
#include <memory>
#include <string>

#include "rust/cxx.h"

struct CommandMeta;
struct JsonBlob;
struct Runtime;

class Module {
public:
    Module(const std::string& module_id, const std::string& prefix, const std::string& conf);

    JsonBlob initialize();
    JsonBlob get_interface(rust::Str interface_name) const;

    void signal_ready(const Runtime& rt) const;
    void provide_command(const Runtime& rt, const CommandMeta& meta) const;

    // TODO(hrapp): Add call_command, publish_variable and subscribe_variable.

private:
    const std::string module_id_;
    std::shared_ptr<Everest::RuntimeSettings> rs_;
    std::unique_ptr<Everest::Config> config_;
    std::unique_ptr<Everest::Everest> handle_;
};

std::unique_ptr<Module> create_module(rust::Str module_name, rust::Str prefix, rust::Str conf);
