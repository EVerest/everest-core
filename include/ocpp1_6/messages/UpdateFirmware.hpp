// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_UPDATEFIRMWARE_HPP
#define OCPP1_6_UPDATEFIRMWARE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 UpdateFirmware message
struct UpdateFirmwareRequest : public Message {
    TODO : std::string with format : uri location;
    DateTime retrieveDate;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;

    /// \brief Provides the type of this UpdateFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "UpdateFirmware";
    }

    /// \brief Conversion from a given UpdateFirmwareRequest \p k to a given json object \p j
    friend void to_json(json& j, const UpdateFirmwareRequest& k) {
        // the required parts of the message
        j = json{
            {"location", k.location},
            {"retrieveDate", k.retrieveDate.to_rfc3339()},
        };
        // the optional parts of the message
        if (k.retries) {
            j["retries"] = k.retries.value();
        }
        if (k.retryInterval) {
            j["retryInterval"] = k.retryInterval.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given UpdateFirmwareRequest \p k
    friend void from_json(const json& j, UpdateFirmwareRequest& k) {
        // the required parts of the message
        k.location = j.at("location");
        k.retrieveDate = DateTime(std::string(j.at("retrieveDate")));
        ;

        // the optional parts of the message
        if (j.contains("retries")) {
            k.retries.emplace(j.at("retries"));
        }
        if (j.contains("retryInterval")) {
            k.retryInterval.emplace(j.at("retryInterval"));
        }
    }

    /// \brief Writes the string representation of the given UpdateFirmwareRequest \p k to the given output stream \p os
    /// \returns an output stream with the UpdateFirmwareRequest written to
    friend std::ostream& operator<<(std::ostream& os, const UpdateFirmwareRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 UpdateFirmwareResponse message
struct UpdateFirmwareResponse : public Message {

    /// \brief Provides the type of this UpdateFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "UpdateFirmwareResponse";
    }

    /// \brief Conversion from a given UpdateFirmwareResponse \p k to a given json object \p j
    friend void to_json(json& j, const UpdateFirmwareResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given UpdateFirmwareResponse \p k
    friend void from_json(const json& j, UpdateFirmwareResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given UpdateFirmwareResponse \p k to the given output stream \p
    /// os \returns an output stream with the UpdateFirmwareResponse written to
    friend std::ostream& operator<<(std::ostream& os, const UpdateFirmwareResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_UPDATEFIRMWARE_HPP
