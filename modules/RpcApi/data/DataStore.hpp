// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "GenericInfoStore.hpp"

namespace data {

class ChargerInfoStore : public GenericInfoStore<RPCDataTypes::ChargerInfoObj> {};
class ConnectorInfoStore : public GenericInfoStore<RPCDataTypes::ConnectorInfoObj> {
    //void set_data(const nlohmann::json& in);
    void init_data() override {
        // example, in case override is required
        memset(&dataobj, 0, sizeof(dataobj));
        dataobj.id = -1;
    }
};
class EVSEInfoStore : public GenericInfoStore<RPCDataTypes::EVSEInfoObj> {};
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
    std::vector<DataStoreConnector> connectors;
};

// This is the main data store for the charger. A charger can have multiple EVSEs, each with multiple connectors.
// For more information see 3-Tier model definition of OCPP 2.0.
struct DataStoreCharger {
    ChargerInfoStore chargerinfo;
    std::string everest_version;
    std::vector<DataStoreEvse> evses;
};

} // namespace data

#endif // DATASTORE_HPP