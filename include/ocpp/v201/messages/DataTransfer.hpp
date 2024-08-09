// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_DATATRANSFER_HPP
#define OCPP_V201_DATATRANSFER_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP DataTransfer message
struct DataTransferRequest : public ocpp::Message {
    CiString<255> vendorId;
    std::optional<CustomData> customData;
    std::optional<CiString<50>> messageId;
    std::optional<json> data;

    /// \brief Provides the type of this DataTransfer message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given DataTransferRequest \p k to a given json object \p j
void to_json(json& j, const DataTransferRequest& k);

/// \brief Conversion from a given json object \p j to a given DataTransferRequest \p k
void from_json(const json& j, DataTransferRequest& k);

/// \brief Writes the string representation of the given DataTransferRequest \p k to the given output stream \p os
/// \returns an output stream with the DataTransferRequest written to
std::ostream& operator<<(std::ostream& os, const DataTransferRequest& k);

/// \brief Contains a OCPP DataTransferResponse message
struct DataTransferResponse : public ocpp::Message {
    DataTransferStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;
    std::optional<json> data;

    /// \brief Provides the type of this DataTransferResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given DataTransferResponse \p k to a given json object \p j
void to_json(json& j, const DataTransferResponse& k);

/// \brief Conversion from a given json object \p j to a given DataTransferResponse \p k
void from_json(const json& j, DataTransferResponse& k);

/// \brief Writes the string representation of the given DataTransferResponse \p k to the given output stream \p os
/// \returns an output stream with the DataTransferResponse written to
std::ostream& operator<<(std::ostream& os, const DataTransferResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_DATATRANSFER_HPP
