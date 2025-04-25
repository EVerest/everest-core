/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#include "PaymentTerminalSimulator.hpp"

#include <fmt/format.h>
#include <iomanip>
#include <numeric>
#include <string>

namespace {
std::string generate_banking_id() {
    static int id_int{555555};

    return fmt::format("{:#08x}", id_int++);
}

uint32_t calc_total_cost(const types::session_cost::SessionCost& session_cost) {
    auto sum_optionals = [](uint32_t lhs, std::optional<types::session_cost::SessionCostChunk> rhs) {
        return lhs + (rhs ? (rhs->cost ? rhs->cost->value : 0) : 0);
    };

    auto& cost_chunks = session_cost.cost_chunks;

    uint32_t total_cost{0};
    if (cost_chunks) {
        total_cost = std::accumulate(cost_chunks->cbegin(), cost_chunks->cend(), 0, sum_optionals);
    }
    return total_cost;
}

int32_t pow10(int32_t exponent) {
    int32_t result = 1;
    for (auto i = 0; i < exponent; i++) {
        result *= 10;
    }
    return result;
}

std::string format_money_str(int32_t cost, const types::money::Currency& currency) {
    std::ostringstream oss;
    int32_t divider = pow10(currency.decimals.value_or(0));

    if (divider == 1) {
        oss << cost;
    } else {
        oss << cost / divider << "." << std::setw(2) << std::setfill('0') << cost % divider;
    }
    oss << " " << (currency.code.has_value() ? types::money::currency_code_to_string(currency.code.value()) : "");

    return oss.str();
}

} // namespace

namespace module {

void PaymentTerminalSimulator::init() {
    m_VPT = std::make_shared<pterminal::VirtualPT>();
    if (config.simulated_PT_has_open_preauthorizations) {
        m_VPT->add_open_preauthorization(0, 1000);
        m_VPT->add_open_preauthorization(1, 2000);
        m_VPT->add_open_preauthorization(2, 3000);
    }
    m_simIfc = m_VPT;
    m_PT_drv = std::make_unique<pterminal::PTDriver>(m_VPT);

    invoke_init(*p_token_provider);
}

void PaymentTerminalSimulator::ready() {
    r_cost_provider->subscribe_session_cost(
        [this](types::session_cost::SessionCost cost) { receive_session_cost(cost); });

    invoke_ready(*p_token_provider);

    mqtt.subscribe(bankcard_mqtttopic, [this](const std::string& card_id) { m_simIfc->present_Banking_Card(card_id); });
    mqtt.subscribe(rfidcard_mqtttopic, [this](const std::string& card_id) { m_simIfc->present_RFID_Card(card_id); });

    m_simIfc->set_publish_preauthorizations_callback(
        [this](std::string data) { mqtt.publish(preauthorizations_mqtttopic, data); });

    // Clear the external mqtt topics
    mqtt.publish(running_costs_mqtttopic, "");
    mqtt.publish(partial_reversal_mqtttopic, "");
    mqtt.publish(preauthorizations_mqtttopic, "");

    read_cards_forever();
}

void PaymentTerminalSimulator::receive_session_cost(types::session_cost::SessionCost session_cost) {
    if (!session_cost.id_tag ||
        session_cost.id_tag->authorization_type != types::authorization::AuthorizationType::BankCard) {
        return;
    }

    int32_t total_cost = calc_total_cost(session_cost);

    publish_costs_to_nodered(session_cost, total_cost);

    if (session_cost.status != types::session_cost::SessionStatus::Finished) {
        return;
    }

    print_costs_to_log(session_cost, total_cost);

    bool success = finalize_payment(session_cost, total_cost);

    print_and_publish_to_nodered_finalization_result(session_cost, total_cost, success);
}

void PaymentTerminalSimulator::read_cards_forever() {
    // TODO(CB): Allow to be cancelled
    while (true) {
        auto card_data = m_PT_drv->read_card();
        if (!card_data) {
            continue;
        }

        types::authorization::ProvidedIdToken token{};

        if (card_data->is_banking_card) {
            std::string banking_id = generate_banking_id();
            auto auth_record = m_PT_drv->request_preauthorization_for_id(banking_id, config.pre_authorization_amount);
            if (!auth_record) {
                EVLOG_info
                    << "Received an authentication record: Banking card - could NOT pre-authorize a transaction!";
                continue;
            }
            token.id_token.value = banking_id;
            token.authorization_type = types::authorization::AuthorizationType::BankCard;
            // TODO(CB): Is "Local" the correct token type?
            token.id_token.type = types::authorization::IdTokenType::Local;
            token.prevalidated = true;
            EVLOG_info << "Received an authentication record: Banking card";
            // TODO(CB): Banking card data needs to be stored persistently:
            // [banking_id, amount, card_id, receipt_id]
            // deletion has to occure immediatly after completion of the payment
        } else {
            token.authorization_type = types::authorization::AuthorizationType::RFID;
            if (not card_data->auth_id.has_value()) {
                EVLOG_info << "Received an invalid authentication record";
                return;
            }
            token.id_token.value = card_data->auth_id.value();
            token.id_token.type = types::authorization::IdTokenType::ISO14443;
            token.prevalidated = false;
            EVLOG_info << "Received an authentication record: RFID card";
        }

        p_token_provider->publish_provided_token(token);
    }
}

void PaymentTerminalSimulator::publish_costs_to_nodered(const types::session_cost::SessionCost& session_cost,
                                                        int32_t total_cost) {
    std::ostringstream oss;
    oss << "Costs: ID=" << session_cost.id_tag->id_token.value
        << "; cost=" << format_money_str(total_cost, session_cost.currency);
    mqtt.publish(running_costs_mqtttopic, oss.str());
}

void PaymentTerminalSimulator::print_costs_to_log(const types::session_cost::SessionCost& session_cost,
                                                  int32_t total_cost) {
    EVLOG_info << "Received session_cost: Total cost: " << format_money_str(total_cost, session_cost.currency)
               << ", status: " << session_cost.status
               << "; # cost chunks: " << (session_cost.cost_chunks ? session_cost.cost_chunks->size() : 0)
               << "; session ID: '" << session_cost.session_id << "'; Authorization Token: "
               << (session_cost.id_tag ? session_cost.id_tag->id_token.value : std::string("<NONE>"));

    int count = 0;
    for (const auto& chunk : session_cost.cost_chunks.value_or(std::vector<types::session_cost::SessionCostChunk>{})) {
        count += 1;
        EVLOG_info << count << ".: '" << chunk.timestamp_from.value_or("") << "' to '"
                   << chunk.timestamp_to.value_or("") << "'; " << chunk.metervalue_from.value_or(0) << " Wh to "
                   << chunk.metervalue_to.value_or(0) << " Wh; type: "
                   << (chunk.category.has_value() ? types::session_cost::cost_category_to_string(chunk.category.value())
                                                  : "<NONE>")
                   << "; Cost: "
                   << (chunk.cost.has_value() ? format_money_str(chunk.cost.value().value, session_cost.currency)
                                              : "<NONE>");
    }
}

bool PaymentTerminalSimulator::finalize_payment(const types::session_cost::SessionCost& session_cost,
                                                int32_t total_cost) {
    uint32_t reversal_amount = config.pre_authorization_amount - total_cost;
    bool success = false;
    { success = m_PT_drv->partial_reversal(session_cost.id_tag->id_token.value, reversal_amount); }

    if (!success) {
        // TODO(CB): ...
    }

    return success;
}

void PaymentTerminalSimulator::print_and_publish_to_nodered_finalization_result(
    const types::session_cost::SessionCost& session_cost, int32_t total_cost, bool success) {
    EVLOG_info << "Partial reversal " << (success ? "succeeded" : "failed") << ": ID "
               << session_cost.id_tag->id_token.value
               << " cost: " << format_money_str(config.pre_authorization_amount, session_cost.currency) << " - "
               << format_money_str(total_cost, session_cost.currency) << " = "
               << format_money_str(config.pre_authorization_amount - total_cost, session_cost.currency);

    std::ostringstream oss;
    oss << "Reversal: ID=" << session_cost.id_tag->id_token.value
        << "; reversal=" << format_money_str(config.pre_authorization_amount - total_cost, session_cost.currency)
        << ": " << (success ? "SUCCESS" : "FAILURE");
    mqtt.publish(partial_reversal_mqtttopic, oss.str());
}
} // namespace module
