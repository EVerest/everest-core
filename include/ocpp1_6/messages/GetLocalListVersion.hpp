// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETLOCALLISTVERSION_HPP
#define OCPP1_6_GETLOCALLISTVERSION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetLocalListVersion message
struct GetLocalListVersionRequest : public Message {

    /// \brief Provides the type of this GetLocalListVersion message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetLocalListVersion";
    }

    /// \brief Conversion from a given GetLocalListVersionRequest \p k to a given json object \p j
    friend void to_json(json& j, const GetLocalListVersionRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given GetLocalListVersionRequest \p k
    friend void from_json(const json& j, GetLocalListVersionRequest& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given GetLocalListVersionRequest \p k to the given output stream
    /// \p os \returns an output stream with the GetLocalListVersionRequest written to
    friend std::ostream& operator<<(std::ostream& os, const GetLocalListVersionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 GetLocalListVersionResponse message
struct GetLocalListVersionResponse : public Message {
    int32_t listVersion;

    /// \brief Provides the type of this GetLocalListVersionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetLocalListVersionResponse";
    }

    /// \brief Conversion from a given GetLocalListVersionResponse \p k to a given json object \p j
    friend void to_json(json& j, const GetLocalListVersionResponse& k) {
        // the required parts of the message
        j = json{
            {"listVersion", k.listVersion},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given GetLocalListVersionResponse \p k
    friend void from_json(const json& j, GetLocalListVersionResponse& k) {
        // the required parts of the message
        k.listVersion = j.at("listVersion");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given GetLocalListVersionResponse \p k to the given output stream
    /// \p os \returns an output stream with the GetLocalListVersionResponse written to
    friend std::ostream& operator<<(std::ostream& os, const GetLocalListVersionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETLOCALLISTVERSION_HPP
