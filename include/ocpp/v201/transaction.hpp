// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_TRANSACTION_HANDLER_HPP
#define OCPP_V201_TRANSACTION_HANDLER_HPP

#include <ocpp/common/aligned_timer.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {

namespace v201 {

/// \brief Struct that enhances the OCPP Transaction by some meta data and functionality
struct EnhancedTransaction : public Transaction {
    IdToken id_token;
    std::optional<IdToken> group_id_token;
    std::optional<int32_t> reservation_id;
    int32_t seq_no = 0;
    std::optional<float> active_energy_import_start_value;
    bool check_max_active_import_energy;

    Everest::SteadyTimer sampled_tx_updated_meter_values_timer;
    Everest::SteadyTimer sampled_tx_ended_meter_values_timer;
    ClockAlignedTimer aligned_tx_updated_meter_values_timer;
    ClockAlignedTimer aligned_tx_ended_meter_values_timer;

    int32_t get_seq_no();
    Transaction get_transaction();
};
} // namespace v201

} // namespace ocpp

#endif // OCPP_V201_TRANSACTION_HANDLER_HPP
