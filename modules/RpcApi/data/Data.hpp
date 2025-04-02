// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <optional>

// This contains types for all the data objects

namespace data {

class GenericInfoStore {
public:
    explicit GenericInfoStore() {
        this->init_data();
    };
    // Do the Store classes own their to_json()/from_json()? They should!

    // override this to represent the collected data as json according to the JSON RPC API
    // if the returned value has no value, the data is incomplete or not available
    virtual std::optional<json> get_data() const = 0;
    virtual void set_data(const json& in) = 0;
protected:
    // override this if structures need special (non-default) initialization
    virtual void init_data() {};
private:
    // whether the non-optional values are valid, so that the RPC interface can generate an error
    bool data_is_valid;
};

class ConnectorInfoStore : public GenericInfoStore {
    std::optional<json> get_data() const {
        return std::nullopt;
    }
    void set_data(const json& in) {
        return;
    }
    void init_data() override {
        // do something to explicitly initialize our data structures
    }
};
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
