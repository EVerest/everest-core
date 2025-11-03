// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYWEBPAYMENTSTARTED_HPP
#define OCPP_V21_NOTIFYWEBPAYMENTSTARTED_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyWebPaymentStarted message
struct NotifyWebPaymentStartedRequest : public ocpp::Message {
    int32_t evseId;
    int32_t timeout;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyWebPaymentStarted message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyWebPaymentStartedRequest \p k to a given json object \p j
void to_json(json& j, const NotifyWebPaymentStartedRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyWebPaymentStartedRequest \p k
void from_json(const json& j, NotifyWebPaymentStartedRequest& k);

/// \brief Writes the string representation of the given NotifyWebPaymentStartedRequest \p k to the given output stream
/// \p os \returns an output stream with the NotifyWebPaymentStartedRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyWebPaymentStartedRequest& k);

/// \brief Contains a OCPP NotifyWebPaymentStartedResponse message
struct NotifyWebPaymentStartedResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyWebPaymentStartedResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyWebPaymentStartedResponse \p k to a given json object \p j
void to_json(json& j, const NotifyWebPaymentStartedResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyWebPaymentStartedResponse \p k
void from_json(const json& j, NotifyWebPaymentStartedResponse& k);

/// \brief Writes the string representation of the given NotifyWebPaymentStartedResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyWebPaymentStartedResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyWebPaymentStartedResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYWEBPAYMENTSTARTED_HPP
