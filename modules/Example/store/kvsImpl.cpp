// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "kvsImpl.hpp"

namespace module {
namespace store {

void kvsImpl::init() {
}

void kvsImpl::ready() {
}

void kvsImpl::handle_store(std::string& key,
                           std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string>& value) {
    mod->r_kvs->call_store(key, value);
};

std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> kvsImpl::handle_load(std::string& key) {
    return mod->r_kvs->call_load(key);
};

void kvsImpl::handle_delete(std::string& key) {
    mod->r_kvs->call_delete(key);
};

bool kvsImpl::handle_exists(std::string& key) {
    return mod->r_kvs->call_exists(key);
};

} // namespace store
} // namespace module
