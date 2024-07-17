// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DisplayMessage.hpp"

namespace module {

void DisplayMessage::init() {
    invoke_init(*p_display_message);
    this->r_session_cost->subscribe_session_cost([](const types::session_cost::SessionCost& session_cost) {
        if (!session_cost.cost_chunks.has_value()) {
            EVLOG_warning << "No session cost chunks provided in session cost.";
            return;
        }

        EVLOG_debug << "Session cost status: " << session_status_to_string(session_cost.status);
        for (const types::session_cost::SessionCostChunk& chunk : session_cost.cost_chunks.value()) {
            if (chunk.cost.has_value()) {
                EVLOG_info << "Session cost until now: " << static_cast<double>(chunk.cost.value().value) / 100.0;
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
                    price = static_cast<double>(charging_price.price.value().value.value) / (10 ^ decimals);
                }

                EVLOG_info << "Charging price for category " << category << ": " << price << std::endl;
            }
        }
    });

    // this->p_display_message
}

void DisplayMessage::ready() {
    invoke_ready(*p_display_message);
}

} // namespace module
