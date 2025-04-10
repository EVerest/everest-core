// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATA_HPP
#define DATA_HPP

#include <nlohmann/json.hpp>
#include <optional>

// This contains types for all the data objects

namespace data {

class GenericInfoStore {
public:
    explicit GenericInfoStore() {
        this->init_data();
    };
    // override this to represent the collected data as json according to the JSON RPC API
    // if the returned value has no value, the data is incomplete or not available
    virtual std::optional<nlohmann::json> get_data() const = 0;
    // TBD: Do we need to be able to return an error if we cannot set the data?
    virtual void set_data(const nlohmann::json& in) = 0;
    // register a callback which is triggered when any data in this data store changes
    void register_notification_callback(const std::function<void()>& callback);

protected:
    // override this if structures need special (non-default) initialization
    virtual void init_data(){};
    // whether the non-optional values are valid, so that the RPC interface can generate an error
    bool data_is_valid{false};
};

class ConnectorInfoStore : public GenericInfoStore {
    std::optional<nlohmann::json> get_data() const;
    void set_data(const nlohmann::json& in);
    void init_data() override;
    // void register_notification_callback() override;
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

} // namespace data

#endif // DATA_HPP
