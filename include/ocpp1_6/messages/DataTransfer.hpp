// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DATATRANSFER_HPP
#define OCPP1_6_DATATRANSFER_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 DataTransfer message
struct DataTransferRequest : public Message {
    CiString255Type vendorId;
    boost::optional<CiString50Type> messageId;
    boost::optional<std::string> data;

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

/// \brief Contains a OCPP 1.6 DataTransferResponse message
struct DataTransferResponse : public Message {
    DataTransferStatus status;
    boost::optional<std::string> data;

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

} // namespace ocpp1_6

#endif // OCPP1_6_DATATRANSFER_HPP
