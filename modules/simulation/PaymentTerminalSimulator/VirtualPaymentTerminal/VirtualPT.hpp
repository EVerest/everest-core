/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <condition_variable>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "include/PTSimulationIfc.hpp"
#include "include/types.hpp"

namespace pterminal {
struct PTCardInteractionRecord {
    std::string card_id{};
    bool is_banking_card{false};
};

class PaymentTerminalIfc {
public:
    virtual ~PaymentTerminalIfc() = default;

    virtual std::vector<receipt_id_t> get_open_preauthorizations_ids() const = 0;
    virtual std::optional<PTCardInteractionRecord> read_card() = 0;
    virtual std::optional<receipt_id_t> request_preauthorization(std::string card_id, money_amount_t) = 0;
    virtual void cancel_read() = 0;
    virtual bool cancel_preauthorization(receipt_id_t) = 0;
    virtual bool partial_reversal(receipt_id_t, money_amount_t) = 0;
};

class VirtualPT : public PaymentTerminalIfc, public PTSimulationIfc {
public:
    VirtualPT() = default;
    ~VirtualPT() override = default;

    std::vector<receipt_id_t> get_open_preauthorizations_ids() const override;
    std::optional<PTCardInteractionRecord> read_card() override;
    std::optional<receipt_id_t> request_preauthorization(std::string card_id, money_amount_t amount) override;
    void cancel_read() override;
    bool cancel_preauthorization(receipt_id_t receipt_id) override;
    bool partial_reversal(receipt_id_t receipt_id, money_amount_t reversal) override;

    // Simulation Interface
    void add_open_preauthorization(const receipt_id_t& receipt_id, money_amount_t amount) override;
    void present_RFID_Card(const std::string& card_id) override;
    void present_Banking_Card(const std::string& card_id) override;
    void set_publish_preauthorizations_callback(const std::function<void(std::string)>& callback) override;

private:
    bool delete_preauthorization(receipt_id_t receipt_id);
    void publish_preauthorizations();
    void present_Card(const std::string& card_id, bool is_banking_card);

    std::optional<PTCardInteractionRecord> m_active_card{};
    std::map<receipt_id_t, money_amount_t> m_open_preauthorizations{};
    receipt_id_t m_next_receipt_id{0};

    bool m_new_card_input{false};
    bool m_cmd_cancelled{false};
    bool m_read_card_active{false};
    std::optional<PTCardInteractionRecord> m_presented_card_record{};
    std::condition_variable m_card_input_cv;
    std::mutex m_card_input_mtx;
    mutable std::mutex m_preauthmap_mtx; // mutable to allow use in const member functions

    std::function<void(std::string)> m_publish_preauth_callback{};
};

} // namespace virt_pterminal
