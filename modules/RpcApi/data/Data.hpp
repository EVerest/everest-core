// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATA_HPP
#define DATA_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <generated/types/json_rpc_api.hpp>

// This contains types for all the data objects

namespace data {

namespace RPCDataTypes = types::json_rpc_api;

template <typename T>
class GenericInfoStore {
protected:
    // the associated data store
    T dataobj;
    // function to call when changes occurred
    std::function<void()> notification_callback;

    // override this if structures need special (non-default) initialization
    virtual void init_data(){};
    // whether the non-optional values are valid, so that the RPC interface can generate an error
    bool data_is_valid{false};

public:
    explicit GenericInfoStore() {
        this->init_data();
    };
    // if the returned value has no value, the data is incomplete or not available
    std::optional<T> get_data() const {
        if (this->data_is_valid) {
            return this->dataobj;
        } else {
            return std::nullopt;
        }
    };
    // TBD: Do we need to be able to return an error if we cannot set the data?
    // virtual void set_data(const nlohmann::json& in) = 0;

    // register a callback which is triggered when any data in the associated data store changes
    void register_notification_callback(const std::function<void()>& callback) {
        this->notification_callback = callback;
    }
};

class ChargerInfoStore : public GenericInfoStore<RPCDataTypes::ChargerInfoObj> {};
class ConnectorInfoStore : public GenericInfoStore<RPCDataTypes::ConnectorInfoObj> {
    void set_data(const nlohmann::json& in);
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

class DataStore {
    ChargerInfoStore chargerinfo;
    ConnectorInfoStore connectorinfo;
    EVSEInfoStore evseinfo;
    EVSEStatusStore evsestatus;
    HardwareCapabilitiesStore hardwarecapabilities;
    MeterDataStore meterdata;
};

} // namespace data

#endif // DATA_HPP
