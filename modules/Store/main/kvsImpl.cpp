/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "kvsImpl.hpp"

namespace module {
namespace main {

void kvsImpl::init() {
}

void kvsImpl::ready() {
}

void kvsImpl::handle_store(std::string& key,
                           boost::variant<Array, Object, bool, double, int, std::nullptr_t, std::string>& value) {
    kvs[key] = value;
};

boost::variant<Array, Object, bool, double, int, std::nullptr_t, std::string> kvsImpl::handle_load(std::string& key) {
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
