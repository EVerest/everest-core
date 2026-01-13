// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef V16_UTILS_HPP
#define V16_UTILS_HPP

#include <ocpp/common/call_types.hpp>
#include <ocpp/v16/messages/StopTransaction.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

#include <string>
#include <vector>

namespace ocpp::v16::utils {

size_t get_message_size(const ocpp::Call<StopTransactionRequest>& call);

/// \brief Drops every second entry from transactionData as long as the message size of the \p call is greater than the
/// \p max_message_size
void drop_transaction_data(size_t max_message_size, ocpp::Call<StopTransactionRequest>& call);

/// \brief Determines if a given \p security_event is critical as defined in the OCPP 1.6 security whitepaper
bool is_critical(const std::string& security_event);

/// \brief split a string into a vector of strings
/// \note will contain empty strings where a separator is repeated
std::vector<std::string> split_string(char separator, const std::string& csl);

/// \brief convert a vector into a comma separated list in a string
std::string to_csl(const std::vector<std::string>& vec);
/// \brief convert a comma separated list in a string to a vector of strings
/// \note will not contain empty strings when a comma is repeated
std::vector<std::string> from_csl(const std::string& csl);

} // namespace ocpp::v16::utils

#endif
