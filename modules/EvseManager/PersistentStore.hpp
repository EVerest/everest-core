// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

/*
 The Persistent Store class is an abstraction layer to store any persistent information
 (such as sessions) for the EvseManager.
*/

#ifndef EVSE_MANAGER_PERSISTENT_STORE_H_
#define EVSE_MANAGER_PERSISTENT_STORE_H_

#include <generated/interfaces/kvs/Interface.hpp>

namespace module {

class PersistentStore {
public:
    // We need the r_bsp reference to be able to talk to the bsp driver module
    explicit PersistentStore(const std::vector<std::unique_ptr<kvsIntf>>& r_store, const std::string module_id);

    void store_session(const std::string& session_uuid);
    void clear_session();
    std::string get_session();

private:
    const std::vector<std::unique_ptr<kvsIntf>>& r_store;
    std::string session_key;
    bool active{false};
};

} // namespace module

#endif // EVSE_MANAGER_PERSISTENT_STORE_H_
