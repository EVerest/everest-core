// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_RESET_HPP
#define OCPP1_6_RESET_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 Reset message
struct ResetRequest : public Message {
    ResetType type;

    /// \brief Provides the type of this Reset message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "Reset";
    }

    /// \brief Conversion from a given ResetRequest \p k to a given json object \p j
    friend void to_json(json& j, const ResetRequest& k) {
        // the required parts of the message
        j = json{
            {"type", conversions::reset_type_to_string(k.type)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ResetRequest \p k
    friend void from_json(const json& j, ResetRequest& k) {
        // the required parts of the message
        k.type = conversions::string_to_reset_type(j.at("type"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ResetRequest \p k to the given output stream \p os
    /// \returns an output stream with the ResetRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ResetRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ResetResponse message
struct ResetResponse : public Message {
    ResetStatus status;

    /// \brief Provides the type of this ResetResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ResetResponse";
    }

    /// \brief Conversion from a given ResetResponse \p k to a given json object \p j
    friend void to_json(json& j, const ResetResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::reset_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ResetResponse \p k
    friend void from_json(const json& j, ResetResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_reset_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ResetResponse \p k to the given output stream \p os
    /// \returns an output stream with the ResetResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ResetResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_RESET_HPP
