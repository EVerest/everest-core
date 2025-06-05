/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "include/types.hpp"

namespace pterminal {

// Forward declarations
class PaymentTerminalIfc;
struct ConcurrencyControl;

class PTDriver {
public:
    struct CardData {
        std::optional<authorization_id_t> auth_id{};
        card_id_t card_id{};
        bool is_banking_card{false};
    };

    struct Preauthorization {
        receipt_id_t receipt_id{INVALID_RECEIPT_ID};
        card_id_t card_id{};
        money_amount_t amount{0};
    };

    explicit PTDriver(std::shared_ptr<PaymentTerminalIfc> vpt);
    ~PTDriver();

    std::optional<CardData> read_card();
    void cancel_read_card();
    std::optional<Preauthorization> request_preauthorization_for_id(const bank_session_token_t& bank_session_id,
                                                                    money_amount_t amount);
    bool cancel_preauthorization(const bank_session_token_t& bank_session_id);
    bool partial_reversal(const bank_session_token_t& bank_session_id, money_amount_t reversal_amount);
    bool restore_preauthorization(const bank_session_token_t& bank_session_id, const Preauthorization& preauthorization);

private:
    void delete_preauth_from_local_list(const bank_session_token_t& bank_session_id);
    void interrupt_read_card();
    void continue_read_card();
    std::optional<receipt_id_t> get_receipt_id_from_local_list(const bank_session_token_t& bank_session_id);
    std::optional<bank_session_token_t> find_key_for_card_id(const card_id_t& card_id);

    std::shared_ptr<PaymentTerminalIfc> m_VPT;
    std::unique_ptr<ConcurrencyControl> m_cc;
    std::map<bank_session_token_t, Preauthorization> m_open_bank_preauths;
    std::optional<CardData> m_active_card{};
    std::vector<receipt_id_t> m_unmatched_open_preauthorizations;
};

} // namespace pterminal
