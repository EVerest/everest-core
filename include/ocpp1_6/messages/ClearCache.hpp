// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CLEARCACHE_HPP
#define OCPP1_6_CLEARCACHE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ClearCache message
struct ClearCacheRequest : public Message {

    /// \brief Provides the type of this ClearCache message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ClearCache";
    }

    /// \brief Conversion from a given ClearCacheRequest \p k to a given json object \p j
    friend void to_json(json& j, const ClearCacheRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ClearCacheRequest \p k
    friend void from_json(const json& j, ClearCacheRequest& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ClearCacheRequest \p k to the given output stream \p os
    /// \returns an output stream with the ClearCacheRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ClearCacheRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ClearCacheResponse message
struct ClearCacheResponse : public Message {
    ClearCacheStatus status;

    /// \brief Provides the type of this ClearCacheResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ClearCacheResponse";
    }

    /// \brief Conversion from a given ClearCacheResponse \p k to a given json object \p j
    friend void to_json(json& j, const ClearCacheResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::clear_cache_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ClearCacheResponse \p k
    friend void from_json(const json& j, ClearCacheResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_clear_cache_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ClearCacheResponse \p k to the given output stream \p os
    /// \returns an output stream with the ClearCacheResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ClearCacheResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CLEARCACHE_HPP
