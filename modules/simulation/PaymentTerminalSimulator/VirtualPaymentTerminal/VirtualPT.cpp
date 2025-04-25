/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "VirtualPaymentTerminal/VirtualPT.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>

namespace {

std::string map_to_string(const std::map<pterminal::receipt_id_t, pterminal::money_amount_t>& id_map) {
    auto csv_fold = [](std::string lhs, std::pair<pterminal::receipt_id_t, pterminal::money_amount_t> rhs) {
        return std::move(lhs) + ',' + std::to_string(rhs.first);
    };

    if (id_map.empty()) {
        return "[]";
    }

    return "[" +
           std::accumulate(std::next(id_map.begin()), id_map.end(), std::to_string(id_map.begin()->first), csv_fold) +
           "]";
}
} // namespace

namespace pterminal {

std::vector<receipt_id_t> VirtualPT::get_open_preauthorizations_ids() const {
    std::vector<receipt_id_t> keys{};
    std::scoped_lock<std::mutex> lock(m_preauthmap_mtx);
    for (const auto [id, value] : m_open_preauthorizations) {
        keys.push_back(id);
    }
    return keys;
}

std::optional<PTCardInteractionRecord> VirtualPT::read_card() {
    {
        std::scoped_lock<std::mutex> lock(m_card_input_mtx);
        m_cmd_cancelled = false;
        m_read_card_active = true;
    }
    m_active_card = std::nullopt;

    while (true) {
        std::unique_lock<std::mutex> lock(m_card_input_mtx);
        m_card_input_cv.wait(lock, [this] { return m_new_card_input || m_cmd_cancelled; });

        if (m_new_card_input) {
            m_active_card = m_presented_card_record;
        }
        if (m_new_card_input or m_cmd_cancelled) {
            m_read_card_active = false;
            m_new_card_input = false;
            break;
        }
    }

    return m_active_card;
}

std::optional<receipt_id_t> VirtualPT::request_preauthorization(std::string card_id, money_amount_t amount) {
    std::optional<receipt_id_t> result = std::nullopt;
    if (m_active_card && m_active_card->is_banking_card && m_active_card->card_id == card_id) {
        std::unique_lock<std::mutex> lock(m_preauthmap_mtx);
        result = m_next_receipt_id++;
        m_open_preauthorizations[result.value()] = amount;
        lock.unlock();
        publish_preauthorizations();
    }
    return result;
}

void VirtualPT::cancel_read() {
    std::scoped_lock<std::mutex> lock(m_card_input_mtx);
    m_cmd_cancelled = true;
    m_card_input_cv.notify_one();
}

bool VirtualPT::cancel_preauthorization(receipt_id_t receipt_id) {
    return delete_preauthorization(receipt_id);
}

bool VirtualPT::partial_reversal(receipt_id_t receipt_id, money_amount_t reversal) {
    std::unique_lock<std::mutex> lock(m_preauthmap_mtx);
    if (m_open_preauthorizations.count(receipt_id) == 0) {
        return false;
    }

    if (m_open_preauthorizations[receipt_id] < reversal) {
        return false;
    }

    lock.unlock();
    delete_preauthorization(receipt_id);

    return true;
}

void VirtualPT::add_open_preauthorization(const receipt_id_t& receipt_id, money_amount_t amount) {
    m_open_preauthorizations[receipt_id] = amount;
    if (not m_open_preauthorizations.empty()) {
        m_next_receipt_id = m_open_preauthorizations.rbegin()->first + 1;
    }
}

void VirtualPT::present_RFID_Card(const std::string& card_id) {
    present_Card(card_id, false);
}

void VirtualPT::present_Banking_Card(const std::string& card_id) {
    present_Card(card_id, true);
}

void VirtualPT::set_publish_preauthorizations_callback(const std::function<void(std::string)>& callback) {
    m_publish_preauth_callback = callback;
    publish_preauthorizations();
}

bool VirtualPT::delete_preauthorization(receipt_id_t id) {
    std::unique_lock<std::mutex> lock(m_preauthmap_mtx);
    auto no_of_removed_elements = m_open_preauthorizations.erase(id);
    lock.unlock();

    if (no_of_removed_elements > 0) {
        publish_preauthorizations();
        return true;
    }
    return false;
}

void VirtualPT::publish_preauthorizations() {
    if (m_publish_preauth_callback) {
        std::scoped_lock<std::mutex> lock(m_preauthmap_mtx);
        m_publish_preauth_callback(map_to_string(m_open_preauthorizations));
    }
}

void VirtualPT::present_Card(const std::string& card_id, bool is_banking_card) {
    std::scoped_lock<std::mutex> lock(m_card_input_mtx);
    if (m_read_card_active) {
        m_presented_card_record = {card_id, is_banking_card};
        m_new_card_input = true;
        m_card_input_cv.notify_one();
    }
}

} // namespace pterminal
