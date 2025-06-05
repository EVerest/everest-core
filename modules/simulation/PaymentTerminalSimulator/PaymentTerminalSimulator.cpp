// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2024 Pionix GmbH and Contributors to EVerest

#include "PaymentTerminalSimulator.hpp"

#include <ctime>
#include <fmt/format.h>
#include <iomanip>
#include <numeric>
#include <string>

namespace {
std::string generate_banking_id() {
    static int id_int{555555};

    return fmt::format("{:#08x}", id_int++);
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
    m_simIfc = m_VPT;

    if (config.simulated_PT_has_open_preauthorizations) {
        m_simIfc->add_open_preauthorization(0, 1000);
        m_simIfc->add_open_preauthorization(1, 2000);
        m_simIfc->add_open_preauthorization(2, 3000);
    }

    m_PT_drv = std::make_unique<pterminal::PTDriver>(m_VPT);

    invoke_init(*p_token_provider);
}

void PaymentTerminalSimulator::ready() {
    r_total_amount_provider->subscribe_total_cost_amount(
        [this](types::payment::TotalCostAmount total_amount) { receive_final_cost(total_amount); });

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

void PaymentTerminalSimulator::receive_final_cost(types::payment::TotalCostAmount total_amount) {
    int32_t total_cost = total_amount.cost.value;

    if (not total_amount.final) {
        publish_costs_to_nodered(total_amount);

        print_costs_to_log(total_cost);
        return;
    }

    bool success = finalize_payment(total_amount);
    // TODO(CB): What if that failed?

    print_and_publish_to_nodered_finalization_result(total_amount, success);
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
            // token.id_token.type = types::authorization::IdTokenType::DirectPayment;  // TODO(CB): Use this line, once this enum value is defined in the authorization type!
            token.id_token.type = types::authorization::IdTokenType::Local;
            token.prevalidated = true;
            EVLOG_info << "Received an authentication record: Banking card";
            types::money::MoneyAmount preauthorized_amount{config.pre_authorization_amount};
            types::payment::BankingPreauthorization preauthorization{preauthorized_amount, token.id_token};
            bool success = r_total_amount_provider->call_cmd_announce_preauthorization(preauthorization);
            // TODO(CB): What if that failed!?

            // TODO(CB): Need a robust timer which can also be cancelled
            session_timeout_futures.push_back(std::async(std::launch::async, [this, id_token = token.id_token]() {
                auto scheduled_time = std::chrono::steady_clock::now() + std::chrono::seconds(config.session_timeout_s);
                std::this_thread::sleep_until(scheduled_time);
                bool success = r_total_amount_provider->call_cmd_withdraw_preauthorization(id_token);
                // TODO(CB): Log if not successfull?
            }));
            if (session_timeout_futures.back().valid()) {
                auto withdraw_time = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now() + std::chrono::seconds(config.session_timeout_s));
                EVLOG_info << "Announced token '" << token.id_token.value << "' for amount °"
                           << config.pre_authorization_amount << "° - will be withdrawn at "
                           << std::ctime(&withdraw_time) << " (in " << config.session_timeout_s << "s)";
            }

            // TODO(CB): Banking card data needs to be stored persistently:
            // [banking_id, amount, card_id, receipt_id, withdraw_time]
            // but deletion has to occure immediatly after completion of the payment
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

void PaymentTerminalSimulator::publish_costs_to_nodered(const types::payment::TotalCostAmount& total_amount) {
    std::ostringstream oss;
    oss << "Costs: ID=" << total_amount.id_token.value << "; cost="
        << format_money_str(total_amount.cost.value, types::money::Currency{types::money::CurrencyCode::EUR,
                                                                            2}); // TODO(CB): Change currency arg ...
    mqtt.publish(running_costs_mqtttopic, oss.str());
}

void PaymentTerminalSimulator::print_costs_to_log(int32_t total_cost) {
    EVLOG_info << "Received total cost: "
               << format_money_str(total_cost, types::money::Currency{types::money::CurrencyCode::EUR, 2});
}

bool PaymentTerminalSimulator::finalize_payment(const types::payment::TotalCostAmount& total_amount) {
    uint32_t reversal_amount = config.pre_authorization_amount - total_amount.cost.value;
    bool success = m_PT_drv->partial_reversal(total_amount.id_token.value, reversal_amount);

    if (!success) {
        // TODO(CB): ...
    }

    return success;
}

void PaymentTerminalSimulator::print_and_publish_to_nodered_finalization_result(
    const types::payment::TotalCostAmount& total_amount, bool success) {
    EVLOG_info << "Partial reversal " << (success ? "succeeded" : "failed") << ": ID " << total_amount.id_token.value
               << " cost: "
               << format_money_str(config.pre_authorization_amount,
                                   types::money::Currency{types::money::CurrencyCode::EUR, 2})
               << " - " // TODO(CB): Change currency arg ...
               << format_money_str(total_amount.cost.value, types::money::Currency{types::money::CurrencyCode::EUR, 2})
               << " = " // TODO(CB): Change currency arg ...
               << format_money_str(
                      config.pre_authorization_amount - total_amount.cost.value,
                      types::money::Currency{types::money::CurrencyCode::EUR, 2}); // TODO(CB): Change currency arg ...

    std::ostringstream oss;
    oss << "Reversal: ID=" << total_amount.id_token.value << "; reversal="
        << format_money_str(
               config.pre_authorization_amount - total_amount.cost.value,
               types::money::Currency{types::money::CurrencyCode::EUR, 2}) // TODO(CB): Change currency arg ...
        << ": " << (success ? "SUCCESS" : "FAILURE");
    mqtt.publish(partial_reversal_mqtttopic, oss.str());
}
} // namespace module
