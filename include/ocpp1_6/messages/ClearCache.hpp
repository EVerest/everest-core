// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CLEARCACHE_HPP
#define OCPP1_6_CLEARCACHE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct ClearCacheRequest : public Message {

    std::string get_type() const {
        return "ClearCache";
    }

    friend void to_json(json& j, const ClearCacheRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    friend void from_json(const json& j, ClearCacheRequest& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const ClearCacheRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ClearCacheResponse : public Message {
    ClearCacheStatus status;

    std::string get_type() const {
        return "ClearCacheResponse";
    }

    friend void to_json(json& j, const ClearCacheResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::clear_cache_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, ClearCacheResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_clear_cache_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const ClearCacheResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CLEARCACHE_HPP
