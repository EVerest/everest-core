// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "kvsImpl.hpp"

namespace module {
namespace main {

void kvsImpl::init() {
}

void kvsImpl::ready() {
}

void kvsImpl::shutdown() {
}

void kvsImpl::handle_store(std::string& key,
                           boost::variant<boost::blank, Array, Object, bool, double, int, std::string>& value) {
    kvs[key] = value;
};

boost::variant<boost::blank, Array, Object, bool, double, int, std::string> kvsImpl::handle_load(std::string& key) {
    return kvs[key];
};

void kvsImpl::handle_delete(std::string& key) {
    kvs.erase(key);
};

bool kvsImpl::handle_exists(std::string& key) {
    return kvs.count(key) != 0;
};

} // namespace main
} // namespace module
