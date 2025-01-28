// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "kvsImpl.hpp"
#include <filesystem>
#include <string>
#include <utils/yaml_loader.hpp>
#include <variant>

namespace module {
namespace main {

void kvsImpl::init() {
    auto kv_file_path = std::filesystem::path(mod->config.file);

    try {
        data = Everest::load_yaml(kv_file_path);
    } catch (const std::exception& err) {
        EVLOG_error << "Error parsing YAML file at " << mod->config.file << ": " << err.what();
    }
}

void kvsImpl::ready() {
}

void kvsImpl::handle_store(std::string& key,
                           std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string>& value) {
    // this is a read-only kvs - do nothing but prevent compiler warnings about unused parameters
    (void)key;
    (void)value;
}

std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> kvsImpl::handle_load(std::string& key) {
    if (data.contains(key)) {
        std::string value{data[key]};
        return value;
    }
    return nullptr;
}

void kvsImpl::handle_delete(std::string& key) {
    // this is a read-only kvs - do nothing but prevent compiler warnings about unused parameters
    (void)key;
}

bool kvsImpl::handle_exists(std::string& key) {
    return data.contains(key);
}

} // namespace main
} // namespace module
