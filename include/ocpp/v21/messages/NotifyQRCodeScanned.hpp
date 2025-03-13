// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYQRCODESCANNED_HPP
#define OCPP_V21_NOTIFYQRCODESCANNED_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyQRCodeScanned message
struct NotifyQRCodeScannedRequest : public ocpp::Message {
    int32_t evseId;
    int32_t timeout;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyQRCodeScanned message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyQRCodeScannedRequest \p k to a given json object \p j
void to_json(json& j, const NotifyQRCodeScannedRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyQRCodeScannedRequest \p k
void from_json(const json& j, NotifyQRCodeScannedRequest& k);

/// \brief Writes the string representation of the given NotifyQRCodeScannedRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyQRCodeScannedRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyQRCodeScannedRequest& k);

/// \brief Contains a OCPP NotifyQRCodeScannedResponse message
struct NotifyQRCodeScannedResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyQRCodeScannedResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyQRCodeScannedResponse \p k to a given json object \p j
void to_json(json& j, const NotifyQRCodeScannedResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyQRCodeScannedResponse \p k
void from_json(const json& j, NotifyQRCodeScannedResponse& k);

/// \brief Writes the string representation of the given NotifyQRCodeScannedResponse \p k to the given output stream \p
/// os \returns an output stream with the NotifyQRCodeScannedResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyQRCodeScannedResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYQRCODESCANNED_HPP
