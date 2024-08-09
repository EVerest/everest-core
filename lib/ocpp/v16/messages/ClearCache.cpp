// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/ClearCache.hpp>

#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string ClearCacheRequest::get_type() const {
    return "ClearCache";
}

void to_json(json& j, const ClearCacheRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    (void)k; // no elements to unpack, silence unused parameter warning
}

void from_json(const json& j, ClearCacheRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    // no elements to unpack, silence unused parameter warning
    (void)j;
    (void)k;
}

/// \brief Writes the string representation of the given ClearCacheRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheRequest written to
std::ostream& operator<<(std::ostream& os, const ClearCacheRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearCacheResponse::get_type() const {
    return "ClearCacheResponse";
}

void to_json(json& j, const ClearCacheResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::clear_cache_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ClearCacheResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_cache_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ClearCacheResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheResponse written to
std::ostream& operator<<(std::ostream& os, const ClearCacheResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
