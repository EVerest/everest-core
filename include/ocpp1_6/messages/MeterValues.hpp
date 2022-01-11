// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_METERVALUES_HPP
#define OCPP1_6_METERVALUES_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 MeterValues message
struct MeterValuesRequest : public Message {
    int32_t connectorId;
    std::vector<MeterValue> meterValue;
    boost::optional<int32_t> transactionId;

    /// \brief Provides the type of this MeterValues message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "MeterValues";
    }

    /// \brief Conversion from a given MeterValuesRequest \p k to a given json object \p j
    friend void to_json(json& j, const MeterValuesRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
            {"meterValue", k.meterValue},
        };
        // the optional parts of the message
        if (k.transactionId) {
            j["transactionId"] = k.transactionId.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given MeterValuesRequest \p k
    friend void from_json(const json& j, MeterValuesRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        for (auto val : j.at("meterValue")) {
            k.meterValue.push_back(val);
        }

        // the optional parts of the message
        if (j.contains("transactionId")) {
            k.transactionId.emplace(j.at("transactionId"));
        }
    }

    /// \brief Writes the string representation of the given MeterValuesRequest \p k to the given output stream \p os
    /// \returns an output stream with the MeterValuesRequest written to
    friend std::ostream& operator<<(std::ostream& os, const MeterValuesRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 MeterValuesResponse message
struct MeterValuesResponse : public Message {

    /// \brief Provides the type of this MeterValuesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "MeterValuesResponse";
    }

    /// \brief Conversion from a given MeterValuesResponse \p k to a given json object \p j
    friend void to_json(json& j, const MeterValuesResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given MeterValuesResponse \p k
    friend void from_json(const json& j, MeterValuesResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given MeterValuesResponse \p k to the given output stream \p os
    /// \returns an output stream with the MeterValuesResponse written to
    friend std::ostream& operator<<(std::ostream& os, const MeterValuesResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_METERVALUES_HPP
