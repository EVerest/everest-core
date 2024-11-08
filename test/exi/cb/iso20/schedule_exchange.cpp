#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/schedule_exchange.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize schedule_exchange messages") {

    GIVEN("Serialize schedule_exchange_req - scheduled mode") {
        uint8_t doc_raw[] = {0x80, 0x6c, 0x04, 0x23, 0xfe, 0x9d, 0xa7, 0x89, 0x92, 0xab, 0xe5, 0x0c, 0xee, 0x2c, 0x4b,
                             0x70, 0x62, 0x7e, 0x84, 0x28, 0x0e, 0x00, 0x83, 0x00, 0xa0, 0x10, 0x60, 0x28, 0x03, 0xf0,
                             0x02, 0x80, 0x00, 0x04, 0x80, 0xe0, 0x41, 0x80, 0x50, 0x40, 0x00, 0x02, 0xa2, 0xaa, 0xa9,
                             0x03, 0x27, 0x57, 0x26, 0xe3, 0xa6, 0x97, 0x36, 0xf3, 0xa7, 0x37, 0x46, 0x43, 0xa6, 0x97,
                             0x36, 0xf3, 0xa3, 0x13, 0x53, 0x13, 0x13, 0x83, 0xa2, 0xd3, 0x23, 0x03, 0xa5, 0x07, 0x26,
                             0x96, 0x36, 0x54, 0x16, 0xc6, 0x76, 0xf7, 0x26, 0x97, 0x46, 0x86, 0xd3, 0xa3, 0x12, 0xd5,
                             0x06, 0xf7, 0x76, 0x57, 0x20, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x40};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded succussfully") {

            REQUIRE(variant.get_type() == message_20::Type::ScheduleExchangeReq);

            const auto& msg = variant.get<message_20::ScheduleExchangeRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x47, 0xFD, 0x3B, 0x4F, 0x13, 0x25, 0x57, 0xCA});
            REQUIRE(header.timestamp == 1727082830);

            REQUIRE(msg.max_supporting_points == 1024);

            REQUIRE(std::holds_alternative<message_20::ScheduleExchangeRequest::Scheduled_SEReqControlMode>(
                msg.control_mode));
            auto& control_mode =
                std::get<message_20::ScheduleExchangeRequest::Scheduled_SEReqControlMode>(msg.control_mode);

            REQUIRE(control_mode.departure_time.has_value() == true);
            REQUIRE(control_mode.departure_time == 7200);
            REQUIRE(control_mode.target_energy.has_value() == true);
            REQUIRE(message_20::from_RationalNumber(*control_mode.target_energy) == 10000.0f);
            REQUIRE(control_mode.max_energy.has_value() == true);
            REQUIRE(message_20::from_RationalNumber(*control_mode.max_energy) == 20000.0f);
            REQUIRE(control_mode.min_energy.has_value() == true);
            REQUIRE(message_20::from_RationalNumber(*control_mode.min_energy) == 0.05f);

            REQUIRE(control_mode.energy_offer.has_value() == true);
            auto& ev_energy_offer = control_mode.energy_offer.value();
            REQUIRE(ev_energy_offer.power_schedule.time_anchor == 0);
            REQUIRE(ev_energy_offer.power_schedule.entries.size() == 1);
            REQUIRE(ev_energy_offer.power_schedule.entries.at(0).duration == 3600);
            REQUIRE(message_20::from_RationalNumber(ev_energy_offer.power_schedule.entries.at(0).power) == 10000.0f);

            REQUIRE(ev_energy_offer.absolute_price_schedule.time_anchor == 0);
            REQUIRE(ev_energy_offer.absolute_price_schedule.currency == "EUR");
            REQUIRE(ev_energy_offer.absolute_price_schedule.price_algorithm ==
                    "urn:iso:std:iso:15118:-20:PriceAlgorithm:1-Power");
            REQUIRE(ev_energy_offer.absolute_price_schedule.price_rule_stacks.size() == 1);
            REQUIRE(ev_energy_offer.absolute_price_schedule.price_rule_stacks.at(0).duration == 0);
            REQUIRE(ev_energy_offer.absolute_price_schedule.price_rule_stacks.at(0).price_rules.size() == 1);
            REQUIRE(message_20::from_RationalNumber(
                        ev_energy_offer.absolute_price_schedule.price_rule_stacks.at(0).price_rules.at(0).energy_fee) ==
                    0);
            REQUIRE(message_20::from_RationalNumber(ev_energy_offer.absolute_price_schedule.price_rule_stacks.at(0)
                                                        .price_rules.at(0)
                                                        .power_range_start) == 0);
        }
    }

    GIVEN("Serialize schedule_exchange_req - dynamic mode") {
        uint8_t doc_raw[] = {0x80, 0x6c, 0x04, 0x1c, 0x90, 0x58, 0x02, 0x37, 0x25, 0x7c, 0x84, 0x8d, 0x6b, 0x0c,
                             0x4b, 0x70, 0x62, 0x7e, 0x80, 0xa0, 0x38, 0x03, 0xc1, 0x40, 0x20, 0xc0, 0xa0, 0x10,
                             0x60, 0x78, 0x08, 0x31, 0x13, 0x02, 0x0c, 0x01, 0x40, 0x80, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded succussfully") {

            REQUIRE(variant.get_type() == message_20::Type::ScheduleExchangeReq);

            const auto& msg = variant.get<message_20::ScheduleExchangeRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x39, 0x20, 0xB0, 0x04, 0x6E, 0x4A, 0xF9, 0x09});
            REQUIRE(header.timestamp == 1727076438);

            REQUIRE(msg.max_supporting_points == 1024);

            REQUIRE(std::holds_alternative<message_20::ScheduleExchangeRequest::Dynamic_SEReqControlMode>(
                msg.control_mode));
            auto& control_mode =
                std::get<message_20::ScheduleExchangeRequest::Dynamic_SEReqControlMode>(msg.control_mode);

            REQUIRE(control_mode.departure_time == 7200);
            REQUIRE(control_mode.minimum_soc == 30);
            REQUIRE(control_mode.target_soc == 80);
            REQUIRE(message_20::from_RationalNumber(control_mode.target_energy) == 40000.0f);
            REQUIRE(message_20::from_RationalNumber(control_mode.max_energy) == 60000.0f);
            REQUIRE(message_20::from_RationalNumber(control_mode.min_energy) == -20000.0f);
            REQUIRE(control_mode.max_v2x_energy.has_value() == true);
            REQUIRE(message_20::from_RationalNumber(*control_mode.max_v2x_energy) == 5000.0f);
            REQUIRE(control_mode.min_v2x_energy.has_value() == true);
            REQUIRE(message_20::from_RationalNumber(*control_mode.min_v2x_energy) == 0.0f);
        }
    }

    GIVEN("Serialize schedule_exchange_res - scheduled mode - no price") {

        message_20::ScheduleExchangeResponse res;

        res.header = message_20::Header{{0x47, 0xFD, 0x3B, 0x4F, 0x13, 0x25, 0x57, 0xCA}, 1727082831};
        res.response_code = message_20::ResponseCode::OK;
        res.processing = message_20::Processing::Finished;
        auto& control_mode =
            res.control_mode.emplace<message_20::ScheduleExchangeResponse::Scheduled_SEResControlMode>();
        message_20::ScheduleExchangeResponse::ScheduleTuple tuple;

        tuple.schedule_tuple_id = 1;
        tuple.charging_schedule.power_schedule.time_anchor = 1727082831;
        tuple.charging_schedule.power_schedule.entries.push_back({86400, {2208, 1}, std::nullopt, std::nullopt});

        control_mode.schedule_tuple.push_back(tuple);

        std::vector<uint8_t> expected = {0x80, 0x70, 0x04, 0x23, 0xfe, 0x9d, 0xa7, 0x89, 0x92, 0xab, 0xe5, 0x0c,
                                         0xfe, 0x2c, 0x4b, 0x70, 0x62, 0x00, 0x04, 0x00, 0x41, 0x9f, 0xc5, 0x89,
                                         0x6e, 0x0c, 0x84, 0x05, 0x18, 0x28, 0x40, 0x85, 0x00, 0x89, 0x29, 0x40};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize schedule_exchange_res - scheduled mode - price level") {
        message_20::ScheduleExchangeResponse res;

        res.header = message_20::Header{{0x47, 0xFD, 0x3B, 0x4F, 0x13, 0x25, 0x57, 0xCA}, 1727082831};
        res.response_code = message_20::ResponseCode::OK;
        res.processing = message_20::Processing::Finished;
        auto& control_mode =
            res.control_mode.emplace<message_20::ScheduleExchangeResponse::Scheduled_SEResControlMode>();
        message_20::ScheduleExchangeResponse::ScheduleTuple tuple;

        tuple.schedule_tuple_id = 1;
        tuple.charging_schedule.power_schedule.time_anchor = 1727082831;
        tuple.charging_schedule.power_schedule.entries.push_back({86400, {2208, 1}, std::nullopt, std::nullopt});

        auto& price_level =
            tuple.charging_schedule.price_schedule.emplace<message_20::ScheduleExchangeResponse::PriceLevelSchedule>();
        price_level.time_anchor = 1727082831;
        price_level.price_schedule_id = 1;
        price_level.number_of_price_levels = 0;
        price_level.price_level_schedule_entries.push_back({23, 8});

        control_mode.schedule_tuple.push_back(tuple);

        std::vector<uint8_t> expected = {0x80, 0x70, 0x04, 0x23, 0xfe, 0x9d, 0xa7, 0x89, 0x92, 0xab, 0xe5, 0x0c,
                                         0xfe, 0x2c, 0x4b, 0x70, 0x62, 0x00, 0x04, 0x00, 0x41, 0x9f, 0xc5, 0x89,
                                         0x6e, 0x0c, 0x84, 0x05, 0x18, 0x28, 0x40, 0x85, 0x00, 0x89, 0x25, 0x67,
                                         0xf1, 0x62, 0x5b, 0x83, 0x00, 0x12, 0x00, 0x00, 0xb8, 0x08, 0x11, 0x40};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize schedule_exchange_res - scheduled mode - absolute price") {
        // TODO(sl): Add test + generate exi stream
    }

    GIVEN("Serialize schedule_exchange_res - dynamic mode") {
        message_20::ScheduleExchangeResponse res;

        res.header = message_20::Header{{0x39, 0x20, 0xB0, 0x04, 0x6E, 0x4A, 0xF9, 0x09}, 1727076439};
        res.response_code = message_20::ResponseCode::OK;
        res.processing = message_20::Processing::Finished;
        auto& control_mode = res.control_mode.emplace<message_20::ScheduleExchangeResponse::Dynamic_SEResControlMode>();
        control_mode.departure_time = 2000;

        std::vector<uint8_t> expected = {0x80, 0x70, 0x04, 0x1c, 0x90, 0x58, 0x02, 0x37, 0x25, 0x7c, 0x84,
                                         0x8d, 0x7b, 0x0c, 0x4b, 0x70, 0x62, 0x00, 0x02, 0x1a, 0x01, 0xe8};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
