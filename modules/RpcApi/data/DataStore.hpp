// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "GenericInfoStore.hpp"

namespace data {

class ChargerInfoStore : public GenericInfoStore<RPCDataTypes::ChargerInfoObj> {
public:
    // we currently don't get this info from the system yet, so allow setting to unknown
    void set_unknown() {
        this->dataobj.vendor = "unknown";
        this->dataobj.model = "unknown";
        this->dataobj.serial = "unknown";
        this->dataobj.firmware_version = "unknown";
        // pretend we got something
        this->data_is_valid = true;
    }
};

class ConnectorInfoStore : public GenericInfoStore<RPCDataTypes::ConnectorInfoObj> {
    // EXAMPLE, in case override is required
    // void init_data() override {
    //     memset(&dataobj, 0, sizeof(dataobj));
    //     this->dataobj.id = -1;
    // }
public:
    void set_id(int id) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.id = id;
    }
};
class EVSEInfoStore : public GenericInfoStore<RPCDataTypes::EVSEInfoObj> {
public:
    void set_index(int32_t index) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.index = index;
    }
    void set_id(const std::string& id) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.id = id;
    }
public:
    int32_t get_index() {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        return this->dataobj.index;
    }
};
class EVSEStatusStore : public GenericInfoStore<RPCDataTypes::EVSEStatusObj> {};
class HardwareCapabilitiesStore : public GenericInfoStore<RPCDataTypes::HardwareCapabilitiesObj> {};
class MeterDataStore : public GenericInfoStore<RPCDataTypes::MeterDataObj> {};

// This is the data store for a single connector.
struct DataStoreConnector {
    ConnectorInfoStore connectorinfo;
    HardwareCapabilitiesStore hardwarecapabilities;
};

// This is the data store for a single EVSE. An EVSE can have multiple connectors.
struct DataStoreEvse {
    EVSEInfoStore evseinfo;
    EVSEStatusStore evsestatus;
    MeterDataStore meterdata;
    std::vector<std::unique_ptr<DataStoreConnector>> connectors;
};

// This is the main data store for the charger. A charger can have multiple EVSEs, each with multiple connectors.
// For more information see 3-Tier model definition of OCPP 2.0.
struct DataStoreCharger {
    ChargerInfoStore chargerinfo;
    std::string everest_version;
    std::vector<std::unique_ptr<DataStoreEvse>> evses;

    // get the EVSE data with a specific id
    static data::DataStoreEvse* get_evse_store(DataStoreCharger& dataobj, const int32_t evse_index) {
        if (dataobj.evses.empty()) {
            return nullptr;
        }

        for (const auto& evse : dataobj.evses) {
            if (not evse->evseinfo.get_data().has_value()) {
                continue;
            }

            const auto data = evse->evseinfo.get_data().value();
            if (data.index == evse_index) {
                return evse.get();
            }
        }
        return nullptr;
    };
};

} // namespace data

#endif // DATASTORE_HPP