/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "PaymentTerminalDriver/PTDriver.hpp"

#include <algorithm>
#include <condition_variable>
#include <mutex>

#include "VirtualPaymentTerminal/VirtualPT.hpp"

namespace pterminal {

struct ConcurrencyControl {
    bool cancel_read{false};
    bool read_interrupted{false};
    bool reading{false};
    std::condition_variable interrupt_read_cv;
    std::mutex terminal_access_mtx;
    std::mutex cancel_read_mtx;
    std::mutex preauthorizations_mtx;
};

PTDriver::PTDriver(std::shared_ptr<PaymentTerminalIfc> vpt) : m_VPT(vpt), m_cc(std::make_unique<ConcurrencyControl>()) {
    m_unmatched_open_preauthorizations = m_VPT->get_open_preauthorizations_ids();
}

PTDriver::~PTDriver() = default;

std::optional<PTDriver::CardData> PTDriver::read_card() {
    std::optional<PTCardInteractionRecord> card_record{};

    while (true) {
        if (std::scoped_lock<std::mutex> lock(m_cc->cancel_read_mtx); (m_cc->cancel_read or card_record.has_value())) {
            break;
        }

        {
            std::unique_lock<std::mutex> terminal_lock(m_cc->terminal_access_mtx);
            if (m_cc->read_interrupted) {
                m_cc->reading = false;
                m_cc->interrupt_read_cv.notify_all();
                m_cc->interrupt_read_cv.wait(terminal_lock, [this] { return not m_cc->read_interrupted; });
            }
            m_cc->reading = true;
        }

        card_record = m_VPT->read_card();
    }
    {
        std::scoped_lock<std::mutex> terminal_lock(m_cc->terminal_access_mtx);
        m_cc->reading = false;
    }

    if (std::scoped_lock<std::mutex> lock(m_cc->cancel_read_mtx); m_cc->cancel_read or not card_record.has_value()) {
        m_active_card = std::nullopt;
        return std::nullopt;
    }

    CardData result{.auth_id = card_record->is_banking_card ? std::nullopt
                                                            : std::optional<authorization_id_t>{card_record->card_id},
                    .card_id = card_record->card_id,
                    .is_banking_card = card_record->is_banking_card};

    if (card_record->is_banking_card) {
        auto key = find_key_for_card_id(card_record->card_id);
        if (key.has_value()) {
            result.auth_id = key;
        }
    }

    m_active_card = card_record->is_banking_card ? std::optional<CardData>(result) : std::nullopt;

    return result;
}

void PTDriver::cancel_read_card() {
    std::scoped_lock<std::mutex> lock(m_cc->cancel_read_mtx);
    m_cc->cancel_read = true;
    m_VPT->cancel_read();
}

// Requests a new (pre-)authorization:
// requires a banking card to be the currently active card.
std::optional<PTDriver::Preauthorization>
PTDriver::request_preauthorization_for_id(const bank_session_token_t& bank_session_id, money_amount_t amount) {
    if (!m_active_card || !m_active_card->is_banking_card) {
        return std::nullopt;
    }
    if (std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
        m_open_bank_preauths.count(bank_session_id) > 0) {
        return std::nullopt;
    }

    interrupt_read_card();
    auto receipt_id = m_VPT->request_preauthorization(m_active_card->card_id, amount);
    continue_read_card();

    std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
    if (!receipt_id) {
        return std::nullopt;
    } else {
        // banking cards are identified in EVerest by the externally generated bank_session_id
        // and they need to be remembered in order to be able to close open preauthorizations later
        m_open_bank_preauths[bank_session_id] = {
            .receipt_id = receipt_id.value(), .card_id = m_active_card->card_id, .amount = amount};
    }

    return {m_open_bank_preauths[bank_session_id]};
}

bool PTDriver::cancel_preauthorization(const bank_session_token_t& bank_session_id) {
    auto r_id = get_receipt_id_from_local_list(bank_session_id);
    if (not r_id) {
        return false;
    }

    interrupt_read_card();
    bool success = m_VPT->cancel_preauthorization(r_id.value());
    continue_read_card();

    if (success) {
        delete_preauth_from_local_list(bank_session_id);
    }

    return success;
}

bool PTDriver::partial_reversal(const bank_session_token_t& bank_session_id, money_amount_t reversal_amount) {
    auto r_id = get_receipt_id_from_local_list(bank_session_id);
    if (not r_id) {
        return false;
    }

    interrupt_read_card();
    bool success = m_VPT->partial_reversal(r_id.value(), reversal_amount);
    continue_read_card();

    if (success) {
        delete_preauth_from_local_list(bank_session_id);
    }
    return success;
}

bool PTDriver::restore_preauthorization(const bank_session_token_t& bank_session_id,
                                        const Preauthorization& preauthorization) {
    std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
    auto unmatched_receipt_id = std::find(m_unmatched_open_preauthorizations.begin(),
                                          m_unmatched_open_preauthorizations.end(), preauthorization.receipt_id);
    if (unmatched_receipt_id == m_unmatched_open_preauthorizations.end()) {
        return false;
    }
    if (m_open_bank_preauths.count(bank_session_id) > 0) {
        return false;
    }

    m_open_bank_preauths[bank_session_id] = preauthorization;
    m_unmatched_open_preauthorizations.erase(unmatched_receipt_id);

    return true;
}

void PTDriver::delete_preauth_from_local_list(const bank_session_token_t& bank_session_id) {
    std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
    m_open_bank_preauths.erase(bank_session_id);
}

void PTDriver::interrupt_read_card() {
    std::unique_lock<std::mutex> terminal_lock(m_cc->terminal_access_mtx);
    if (m_cc->read_interrupted) {
        m_cc->interrupt_read_cv.wait(terminal_lock, [this] { return not m_cc->read_interrupted; });
    }
    m_cc->read_interrupted = true;
    if (m_cc->reading) {
        m_VPT->cancel_read();
        m_cc->interrupt_read_cv.wait(terminal_lock, [this] { return not m_cc->reading; });
    }
}

void PTDriver::continue_read_card() {
    std::scoped_lock<std::mutex> terminal_lock(m_cc->terminal_access_mtx);
    m_cc->read_interrupted = false;
    m_cc->interrupt_read_cv.notify_all();
}

std::optional<receipt_id_t> PTDriver::get_receipt_id_from_local_list(const bank_session_token_t& bank_session_id) {
    std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
    if (m_open_bank_preauths.count(bank_session_id) == 0) {
        return std::nullopt;
    }

    return {m_open_bank_preauths[bank_session_id].receipt_id};
}

std::optional<bank_session_token_t> PTDriver::find_key_for_card_id(const card_id_t& card_id) {
    std::scoped_lock<std::mutex> lock(m_cc->preauthorizations_mtx);
    for (const auto& [key, value] : m_open_bank_preauths)
        if (value.card_id == card_id)
            return {key};
    return std::nullopt;
}

} // namespace pterminal
