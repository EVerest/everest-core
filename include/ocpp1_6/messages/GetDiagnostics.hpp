// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETDIAGNOSTICS_HPP
#define OCPP1_6_GETDIAGNOSTICS_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetDiagnostics message
struct GetDiagnosticsRequest : public Message {
    TODO : std::string with format : uri location;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;
    boost::optional<DateTime> startTime;
    boost::optional<DateTime> stopTime;

    /// \brief Provides the type of this GetDiagnostics message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetDiagnostics";
    }

    /// \brief Conversion from a given GetDiagnosticsRequest \p k to a given json object \p j
    friend void to_json(json& j, const GetDiagnosticsRequest& k) {
        // the required parts of the message
        j = json{
            {"location", k.location},
        };
        // the optional parts of the message
        if (k.retries) {
            j["retries"] = k.retries.value();
        }
        if (k.retryInterval) {
            j["retryInterval"] = k.retryInterval.value();
        }
        if (k.startTime) {
            j["startTime"] = k.startTime.value().to_rfc3339();
        }
        if (k.stopTime) {
            j["stopTime"] = k.stopTime.value().to_rfc3339();
        }
    }

    /// \brief Conversion from a given json object \p j to a given GetDiagnosticsRequest \p k
    friend void from_json(const json& j, GetDiagnosticsRequest& k) {
        // the required parts of the message
        k.location = j.at("location");

        // the optional parts of the message
        if (j.contains("retries")) {
            k.retries.emplace(j.at("retries"));
        }
        if (j.contains("retryInterval")) {
            k.retryInterval.emplace(j.at("retryInterval"));
        }
        if (j.contains("startTime")) {
            k.startTime.emplace(DateTime(std::string(j.at("startTime"))));
        }
        if (j.contains("stopTime")) {
            k.stopTime.emplace(DateTime(std::string(j.at("stopTime"))));
        }
    }

    /// \brief Writes the string representation of the given GetDiagnosticsRequest \p k to the given output stream \p os
    /// \returns an output stream with the GetDiagnosticsRequest written to
    friend std::ostream& operator<<(std::ostream& os, const GetDiagnosticsRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 GetDiagnosticsResponse message
struct GetDiagnosticsResponse : public Message {
    boost::optional<CiString255Type> fileName;

    /// \brief Provides the type of this GetDiagnosticsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetDiagnosticsResponse";
    }

    /// \brief Conversion from a given GetDiagnosticsResponse \p k to a given json object \p j
    friend void to_json(json& j, const GetDiagnosticsResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
        if (k.fileName) {
            j["fileName"] = k.fileName.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given GetDiagnosticsResponse \p k
    friend void from_json(const json& j, GetDiagnosticsResponse& k) {
        // the required parts of the message

        // the optional parts of the message
        if (j.contains("fileName")) {
            k.fileName.emplace(j.at("fileName"));
        }
    }

    /// \brief Writes the string representation of the given GetDiagnosticsResponse \p k to the given output stream \p
    /// os \returns an output stream with the GetDiagnosticsResponse written to
    friend std::ostream& operator<<(std::ostream& os, const GetDiagnosticsResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETDIAGNOSTICS_HPP
