// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DATATRANSFER_HPP
#define OCPP1_6_DATATRANSFER_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct DataTransferRequest : public Message {
    CiString255Type vendorId;
    boost::optional<CiString50Type> messageId;
    boost::optional<std::string> data;

    std::string get_type() const {
        return "DataTransfer";
    }

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

    friend std::ostream& operator<<(std::ostream& os, const DataTransferRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct DataTransferResponse : public Message {
    DataTransferStatus status;
    boost::optional<std::string> data;

    std::string get_type() const {
        return "DataTransferResponse";
    }

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

    friend void from_json(const json& j, DataTransferResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_data_transfer_status(j.at("status"));

        // the optional parts of the message
        if (j.contains("data")) {
            k.data.emplace(j.at("data"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const DataTransferResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_DATATRANSFER_HPP
