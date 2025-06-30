// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TerminalCostAndPriceMessage.hpp"

namespace module {

void TerminalCostAndPriceMessage::init() {
    this->r_session_cost->subscribe_tariff_message([](const types::session_cost::TariffMessage& message) {
        for (const types::text_message::MessageContent& message : message.messages) {
            EVLOG_info << "Charging price message"
                       << (message.language.has_value() ? " (" + message.language.value() + ")" : "") << ": "
                       << message.content;
        }
    });

    this->r_session_cost->subscribe_session_cost([](const types::session_cost::SessionCost& session_cost) {
        if (!session_cost.cost_chunks.has_value()) {
            EVLOG_warning << "No session cost chunks provided in session cost.";
            return;
        }

        uint32_t number_of_decimals = 0;
        if (session_cost.currency.decimals.has_value()) {
            if (session_cost.currency.decimals.value() < 0) {
                EVLOG_warning << "Number of decimals for currency can not be negative.";
            } else {
                number_of_decimals = static_cast<uint32_t>(session_cost.currency.decimals.value());
            }
        }

        EVLOG_info << "Session cost status for session id " << session_cost.session_id << ": "
                   << session_status_to_string(session_cost.status);
        for (const types::session_cost::SessionCostChunk& chunk : session_cost.cost_chunks.value()) {
            if (chunk.cost.has_value()) {
                EVLOG_info << "Session cost until now: "
                           << static_cast<double>(chunk.cost.value().value) / (pow(10, number_of_decimals));
            }
        }

        if (session_cost.charging_price.has_value()) {
            for (const types::session_cost::ChargingPriceComponent& charging_price :
                 session_cost.charging_price.value()) {
                std::string category;
                double price = 0;
                if (charging_price.category.has_value()) {
                    category = cost_category_to_string(charging_price.category.value());
                }

                if (charging_price.price.has_value()) {
                    int decimals = 0;
                    if (charging_price.price.value().currency.decimals.has_value()) {
                        decimals = charging_price.price.value().currency.decimals.value();
                    }
                    price = static_cast<double>(charging_price.price.value().value.value) / pow(10, decimals);
                }

                EVLOG_info << "Charging price for category " << category << ": " << price << std::endl;
            }
        }

        if (session_cost.message.has_value()) {
            for (const types::text_message::MessageContent& message : session_cost.message.value()) {
                EVLOG_info << "Charging price message"
                           << (message.language.has_value() ? " (" + message.language.value() + ")" : "") << ": "
                           << message.content;
            }
        }
    });
}

void TerminalCostAndPriceMessage::ready() {
    invoke_ready(*p_main);
}

} // namespace module
