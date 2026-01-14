// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef GENERICINFOSTORE_HPP
#define GENERICINFOSTORE_HPP

#include <atomic>
#include <functional> // for std::function
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <types/json_rpc_api/json_rpc_api.hpp>
#include <vector>

// This contains types for all the data objects

namespace data {

namespace RPCDataTypes = types::json_rpc_api;

template <typename T> class GenericInfoStore {
protected:
    // the associated data store
    T dataobj;
    // protect the data object
    // NB: mutable in order to be able to lock the mutex in const functions
    mutable std::mutex data_mutex;
    // function to call when changes occurred
    std::function<void(const decltype(dataobj)&)> notification_callback;

    // override this if structures need special (non-default) initialization
    virtual void init_data(){};
    // whether the non-optional values are valid, so that the RPC interface can generate an error
    std::atomic<bool> data_is_valid{false};

public:
    explicit GenericInfoStore() {
        this->init_data();
    };
    // if the returned value has no value, the data is incomplete or not available
    std::optional<T> get_data() const {
        if (this->data_is_valid) {
            std::unique_lock<std::mutex> data_lock{this->data_mutex};
            return this->dataobj;
        } else {
            return std::nullopt;
        }
    }
    // set the data object. This method must be overridden if the data object is not a simple type
    //   e.g. pointers
    // Note: all setters, also in derived classes, must use the data mutex
    //   e.g. std::unique_lock<std::mutex> data_lock(this->data_mutex)
    void set_data(const T& in) {
        // check for changes
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        if (in != this->dataobj) {
            this->dataobj = in;
            this->data_is_valid = true;
            data_lock.unlock();
            // call the notification callback if it is set
            notify_data_changed();
        }
    }

    // notify that data has changed
    void notify_data_changed() {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        if (this->notification_callback && this->data_is_valid) {
            // create a copy of the data object
            T data_copy = this->dataobj;
            // unlock explicitly before entering callback
            data_lock.unlock();
            this->notification_callback(data_copy);
        }
    }

    // register a callback which is triggered when any data in the associated data store changes
    void register_notification_callback(const std::function<void(const T&)>& callback) {
        this->notification_callback = callback;
    }
};

} // namespace data

#endif // GENERICINFOSTORE_HPP
