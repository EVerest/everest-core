// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "PersistentStore.hpp"

namespace module {

PersistentStore::PersistentStore(const std::vector<std::unique_ptr<kvsIntf>>& _r_store, const std::string module_id) :
    r_store(_r_store) {

    if (r_store.size() > 0) {
        active = true;
    }

    session_key = module_id + "_session";
}

void PersistentStore::store_session(const std::string& session_uuid) {
    if (active) {
        r_store[0]->call_store(session_key, session_uuid);
    }
}

void PersistentStore::clear_session() {
    if (active) {
        r_store[0]->call_store(session_key, "");
    }
}

std::string PersistentStore::get_session() {
    if (active) {
        auto r = r_store[0]->call_load(session_key);
        try {
            if (std::holds_alternative<std::string>(r)) {
                return std::get<std::string>(r);
            }
        } catch (...) {
            return {};
        }
    }
    return {};
}

} // namespace module
