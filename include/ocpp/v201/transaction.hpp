// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_TRANSACTION_HANDLER_HPP
#define OCPP_V201_TRANSACTION_HANDLER_HPP

#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {

namespace v201 {

/// \brief Struct that enhances the OCPP Transaction by some meta data and functionality
struct EnhancedTransaction : public Transaction {
    std::vector<MeterValue> meter_values;
    IdToken id_token;
    std::optional<int32_t> reservation_id;
    int32_t seq_no = 0;
    int32_t get_seq_no();
    Transaction get_transaction();
};
} // namespace v201

} // namespace ocpp

#endif // OCPP_V201_TRANSACTION_HANDLER_HPP
