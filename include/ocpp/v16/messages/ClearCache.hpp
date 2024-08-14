// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_CLEARCACHE_HPP
#define OCPP_V16_CLEARCACHE_HPP

#include <nlohmann/json_fwd.hpp>

#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP ClearCache message
struct ClearCacheRequest : public ocpp::Message {

    /// \brief Provides the type of this ClearCache message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearCacheRequest \p k to a given json object \p j
void to_json(json& j, const ClearCacheRequest& k);

/// \brief Conversion from a given json object \p j to a given ClearCacheRequest \p k
void from_json(const json& j, ClearCacheRequest& k);

/// \brief Writes the string representation of the given ClearCacheRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheRequest written to
std::ostream& operator<<(std::ostream& os, const ClearCacheRequest& k);

/// \brief Contains a OCPP ClearCacheResponse message
struct ClearCacheResponse : public ocpp::Message {
    ClearCacheStatus status;

    /// \brief Provides the type of this ClearCacheResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearCacheResponse \p k to a given json object \p j
void to_json(json& j, const ClearCacheResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearCacheResponse \p k
void from_json(const json& j, ClearCacheResponse& k);

/// \brief Writes the string representation of the given ClearCacheResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheResponse written to
std::ostream& operator<<(std::ostream& os, const ClearCacheResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CLEARCACHE_HPP
