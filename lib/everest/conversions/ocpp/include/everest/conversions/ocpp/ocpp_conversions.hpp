#pragma once

#include <generated/types/display_message.hpp>
#include <generated/types/money.hpp>
#include <generated/types/reservation.hpp>
#include <generated/types/session_cost.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

namespace ocpp_conversions {
types::display_message::MessagePriorityEnum
to_everest_display_message_priority(const ocpp::v2::MessagePriorityEnum& priority);
ocpp::v2::MessagePriorityEnum to_ocpp_201_message_priority(const types::display_message::MessagePriorityEnum& priority);
types::display_message::MessageStateEnum to_everest_display_message_state(const ocpp::v2::MessageStateEnum& state);
ocpp::v2::MessageStateEnum to_ocpp_201_display_message_state(const types::display_message::MessageStateEnum& state);

types::text_message::MessageFormat to_everest_display_message_format(const ocpp::v2::MessageFormatEnum& message_format);
ocpp::v2::MessageFormatEnum to_ocpp_201_message_format_enum(const types::text_message::MessageFormat& format);

ocpp::IdentifierType to_ocpp_identifiertype_enum(const types::display_message::IdentifierType identifier_type);
types::display_message::IdentifierType to_everest_identifier_type_enum(const ocpp::IdentifierType identifier_type);

types::text_message::MessageContent
to_everest_display_message_content(const ocpp::DisplayMessageContent& message_content);

types::display_message::DisplayMessage to_everest_display_message(const ocpp::DisplayMessage& display_message);
ocpp::DisplayMessage to_ocpp_display_message(const types::display_message::DisplayMessage& display_message);

types::session_cost::TariffMessage to_everest_tariff_message(const ocpp::TariffMessage& tariff_message);

types::session_cost::SessionStatus to_everest_running_cost_state(const ocpp::RunningCostState& state);

types::session_cost::SessionCostChunk create_session_cost_chunk(const double& price, const uint32_t& number_of_decimals,
                                                                const std::optional<ocpp::DateTime>& timestamp,
                                                                const std::optional<uint32_t>& meter_value);

types::money::Price create_price(const double& price, const uint32_t& number_of_decimals,
                                 std::optional<types::money::CurrencyCode> currency_code);

types::session_cost::ChargingPriceComponent
create_charging_price_component(const double& price, const uint32_t& number_of_decimals,
                                const types::session_cost::CostCategory category,
                                std::optional<types::money::CurrencyCode> currency_code);

types::session_cost::SessionCost create_session_cost(const ocpp::RunningCost& running_cost,
                                                     const uint32_t number_of_decimals,
                                                     std::optional<types::money::CurrencyCode> currency_code);

/// \brief Convert given \p datetime_string in RFC3339 format to a OCPP DateTime. If this is not possible return the
/// current datetime
ocpp::DateTime to_ocpp_datetime_or_now(const std::string& datetime_string);

ocpp::ReservationCheckStatus to_ocpp_reservation_check_status(const types::reservation::ReservationCheckStatus& status);

}; // namespace ocpp_conversions
