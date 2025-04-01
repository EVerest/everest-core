// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// This contains types for all the JSON objects

namespace data {

class GenericInfoStore {
    explicit GenericInfoStore() = default;
    virtual ~GenericInfoStore() = default;
public:
    // Do we need a template for the getter?
    // Does the getter get a generic name?
    // Who converts to JSON?
    // Do the Store classes own their to_json()/from_json()? They should!
    json get_data() const;
    void set_data(const json& in);
};

class ConnectorInfoStore : GenericInfoStore {};
class HardwareCapabilitiesStore {};
class MeterDataStore {};
class ACChargeParametersStore {};
class DCChargeParametersStore {};
class ACChargeLoopStore {};
class DCChargeLoopStore {};
class DisplayParametersStore {};
class EVSEStatusStore {};
class EVSEInfoStore {};
class ChargerInfoStore {};

class DataStore {
    ConnectorInfoStore connectorinfo;
    HardwareCapabilitiesStore hardwarecapabilities;
    MeterDataStore meterdata;
    ACChargeParametersStore acchargeparameters;
    DCChargeParametersStore dcchargeparameters;
    ACChargeLoopStore acchargeloop;
    DCChargeLoopStore dcchargeloop;
    DisplayParametersStore displayparameters;
    EVSEStatusStore evsestatus;
    EVSEInfoStore evseinfo;
    ChargerInfoStore chargerinfo;
};

}
