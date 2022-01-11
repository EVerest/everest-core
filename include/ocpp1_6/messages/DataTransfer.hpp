// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DATATRANSFER_HPP
#define OCPP1_6_DATATRANSFER_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 DataTransfer message
struct DataTransferRequest : public Message {
    CiString255Type vendorId;
    boost::optional<CiString50Type> messageId;
    boost::optional<std::string> data;

    /// \brief Provides the type of this DataTransfer message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "DataTransfer";
    }

    /// \brief Conversion from a given DataTransferRequest \p k to a given json object \p j
    friend void to_json(json& j, const DataTransferRequest& k) {
        // the required parts of the message
        j = json{
            {"vendorId", k.vendorId},
        };
        // the optional parts of the message
        if (k.messageId) {
            j["messageId"] = k.messageId.value();
        }
        if (k.data) {
            j["data"] = k.data.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given DataTransferRequest \p k
    friend void from_json(const json& j, DataTransferRequest& k) {
        // the required parts of the message
        k.vendorId = j.at("vendorId");

        // the optional parts of the message
        if (j.contains("messageId")) {
            k.messageId.emplace(j.at("messageId"));
        }
        if (j.contains("data")) {
            k.data.emplace(j.at("data"));
        }
    }

    /// \brief Writes the string representation of the given DataTransferRequest \p k to the given output stream \p os
    /// \returns an output stream with the DataTransferRequest written to
    friend std::ostream& operator<<(std::ostream& os, const DataTransferRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 DataTransferResponse message
struct DataTransferResponse : public Message {
    DataTransferStatus status;
    boost::optional<std::string> data;

    /// \brief Provides the type of this DataTransferResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "DataTransferResponse";
    }

    /// \brief Conversion from a given DataTransferResponse \p k to a given json object \p j
    friend void to_json(json& j, const DataTransferResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::data_transfer_status_to_string(k.status)},
        };
        // the optional parts of the message
        if (k.data) {
            j["data"] = k.data.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given DataTransferResponse \p k
    friend void from_json(const json& j, DataTransferResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_data_transfer_status(j.at("status"));

        // the optional parts of the message
        if (j.contains("data")) {
            k.data.emplace(j.at("data"));
        }
    }

    /// \brief Writes the string representation of the given DataTransferResponse \p k to the given output stream \p os
    /// \returns an output stream with the DataTransferResponse written to
    friend std::ostream& operator<<(std::ostream& os, const DataTransferResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_DATATRANSFER_HPP
