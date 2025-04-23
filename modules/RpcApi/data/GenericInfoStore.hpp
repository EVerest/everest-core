// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef GENERICINFOSTORE_HPP
#define GENERICINFOSTORE_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <generated/types/json_rpc_api.hpp>
#include <functional> // for std::function
#include <vector>

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
    // set the data object. This method must be overridden if the data object is not a simple type
    // e.g. pointers
    void set_data(const T& in) {
        // check for changes
        if (in != this->dataobj) {
            this->dataobj = in;
            this->data_is_valid = true;
            // call the notification callback if it is set
            if (this->notification_callback) {
                this->notification_callback();
            }
        }
    };

    // TBD: Do we need to be able to return an error if we cannot set the data?
    // virtual void set_data(const nlohmann::json& in) = 0;

    // register a callback which is triggered when any data in the associated data store changes
    void register_notification_callback(const std::function<void()>& callback) {
        this->notification_callback = callback;
    }
};

} // namespace data

#endif // GENERICINFOSTORE_HPP
