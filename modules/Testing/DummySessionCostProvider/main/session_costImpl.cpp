// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "session_costImpl.hpp"

#include <everest/helpers/helpers.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include <utils/date.hpp>

namespace {
std::string format_uint(uint32_t value) {
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << value;
    return ss.str();
}
} // namespace

namespace module {
namespace main {

void session_costImpl::init() {
}

void session_costImpl::ready() {
    types::session_cost::TariffMessage tariff_message;
    types::session_cost::SessionCost session_cost;

    tariff_message.identifier_type = types::display_message::IdentifierType::IdToken;

    const std::string msg_content_en = "Tariff Message content no. ";
    const std::string msg_content_de = "Tariff Message Inhalt Nr. ";

    session_cost.currency = types::money::Currency{.code = {types::money::CurrencyCode::EUR}, .decimals = 2};

    while (true) {
        msg_count++;

        tariff_message.messages.emplace_back(types::text_message::MessageContent{
            .content = msg_content_en + format_uint(msg_count), .language = std::string{"EN"}});
        tariff_message.messages.emplace_back(types::text_message::MessageContent{
            .content = msg_content_de + format_uint(msg_count), .language = std::string{"DE"}});

        tariff_message.ocpp_transaction_id = everest::helpers::get_base64_uuid();
        tariff_message.identifier_id = everest::helpers::get_base64_id();

        session_cost.session_id = format_uint(msg_count);
        switch (msg_count % 3) {
        case 0:
            session_cost.status = types::session_cost::SessionStatus::Running;
            break;
        case 1:
            session_cost.status = types::session_cost::SessionStatus::Idle;
            break;
        case 2:
            session_cost.status = types::session_cost::SessionStatus::Finished;
            break;
        }

        auto now = date::utc_clock::now();
        auto past = now - std::chrono::hours(3);

        auto now_datetime = Everest::Date::to_rfc3339(now);
        auto past_datetime = Everest::Date::to_rfc3339(past);

        session_cost.cost_chunks.emplace({types::session_cost::SessionCostChunk{.timestamp_from = past_datetime,
                                                                                .timestamp_to = now_datetime,
                                                                                .metervalue_from = {54321},
                                                                                .metervalue_to = {71233}}});

        EVLOG_info << "Publishing tariff message" << tariff_message;
        publish_tariff_message(tariff_message);
        EVLOG_info << "Publishing session cost" << session_cost;
        publish_session_cost(session_cost);

        tariff_message.messages.pop_back();
        tariff_message.messages.pop_back();

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(config.period_s * 1000)));
    }
}

} // namespace main
} // namespace module
