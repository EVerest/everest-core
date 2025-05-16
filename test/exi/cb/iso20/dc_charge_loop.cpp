#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_charge_loop.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

namespace dt = iso15118::message_20::datatypes;

SCENARIO("Se/Deserialize dc charge loop messages") {

    GIVEN("Deserialize dc_charge_loop_req") {

        uint8_t doc_raw[] = {0x80, 0x34, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb, 0xfe, 0x1b,
                             0x60, 0x62, 0x81, 0x00, 0x12, 0x00, 0x64, 0x64, 0x00, 0x0a, 0x02, 0x00, 0x24, 0x00, 0xca};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_ChargeLoopReq);

            const auto& msg = variant.get<message_20::DC_ChargeLoopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456333);
            REQUIRE(msg.meter_info_requested == false);
            REQUIRE(dt::from_RationalNumber(msg.present_voltage) == 400);

            using ScheduledMode = message_20::datatypes::Scheduled_DC_CLReqControlMode;

            REQUIRE(std::holds_alternative<ScheduledMode>(msg.control_mode));
            const auto& control_mode = std::get<ScheduledMode>(msg.control_mode);
            REQUIRE(dt::from_RationalNumber(control_mode.target_current) == 20);
            REQUIRE(dt::from_RationalNumber(control_mode.target_voltage) == 400);
        }
    }

    GIVEN("Serialize dc_charge_loop request") {

        message_20::DC_ChargeLoopRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456333};
        req.meter_info_requested = false;
        auto& control_mode = req.control_mode.emplace<message_20::datatypes::Scheduled_DC_CLReqControlMode>();
        control_mode.target_current = {20, 0};
        control_mode.target_voltage = {400, 0};

        req.present_voltage = {400, 0};

        std::vector<uint8_t> expected = {0x80, 0x34, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0xdb, 0xfe, 0x1b, 0x60, 0x62, 0x81, 0x00, 0x12,
                                         0x00, 0x64, 0x64, 0x00, 0x0a, 0x02, 0x00, 0x24, 0x00, 0xca};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Deserialize dc_charge_loop_req dynamic mode") {

        uint8_t doc_raw[] = {0x80, 0x34, 0x04, 0x32, 0x75, 0x76, 0x9e, 0xc7, 0x10, 0x64, 0xac, 0x8f, 0x0c, 0xfd,
                             0xab, 0x70, 0x62, 0x00, 0x51, 0x84, 0x02, 0x00, 0x24, 0x00, 0xc6, 0x90, 0x21, 0xe0,
                             0x5c, 0x08, 0x30, 0x3c, 0x04, 0x00, 0x00, 0x82, 0x04, 0x26, 0x1d, 0x41, 0x00, 0x00,
                             0x00, 0x80, 0x0a, 0xc0, 0x20, 0x40, 0x04, 0x20, 0x38, 0x20, 0x02, 0x58, 0x04, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_ChargeLoopReq);

            const auto& msg = variant.get<message_20::DC_ChargeLoopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x64, 0xEA, 0xED, 0x3D, 0x8E, 0x20, 0xC9, 0x59});
            REQUIRE(header.timestamp == 1727440880);
            REQUIRE(msg.meter_info_requested == false);
            REQUIRE(dt::from_RationalNumber(msg.present_voltage) == 400.0f);

            REQUIRE(msg.display_parameters.has_value() == true);
            const auto& display_parameters = msg.display_parameters.value();

            REQUIRE(display_parameters.present_soc.value_or(0) == 10);
            REQUIRE(display_parameters.charging_complete.value_or(true) == false);

            using DynamicMode = message_20::datatypes::Dynamic_DC_CLReqControlMode;

            REQUIRE(std::holds_alternative<DynamicMode>(msg.control_mode));
            const auto& control_mode = std::get<DynamicMode>(msg.control_mode);

            REQUIRE(dt::from_RationalNumber(control_mode.target_energy_request) == 60000.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_energy_request) == 60000.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.min_energy_request) == 1.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_charge_power) == 150000.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.min_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_charge_current) == 300.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_voltage) == 900.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.min_voltage) == 150.0f);
        }
    }

    GIVEN("Serialize dc_charge_loop request dynamic mode") {

        message_20::DC_ChargeLoopRequest req;

        req.header = message_20::Header{{0x64, 0xEA, 0xED, 0x3D, 0x8E, 0x20, 0xC9, 0x59}, 1727440880};
        req.meter_info_requested = false;
        auto& control_mode = req.control_mode.emplace<message_20::datatypes::Dynamic_DC_CLReqControlMode>();

        req.present_voltage = {400, 0};
        control_mode.target_energy_request = {6000, 1};
        control_mode.max_energy_request = {60, 3};
        control_mode.min_energy_request = {1, 0};
        control_mode.max_charge_power = {15000, 1};
        control_mode.min_charge_power = {0, 0};
        control_mode.max_charge_current = {300, 0};
        control_mode.max_voltage = {900, 0};
        control_mode.min_voltage = {150, 0};
        req.display_parameters.emplace();
        req.display_parameters->present_soc = 10;
        req.display_parameters->charging_complete = false;
        std::vector<uint8_t> expected = {0x80, 0x34, 0x04, 0x32, 0x75, 0x76, 0x9e, 0xc7, 0x10, 0x64, 0xac, 0x8f,
                                         0x0c, 0xfd, 0xab, 0x70, 0x62, 0x00, 0x51, 0x84, 0x02, 0x00, 0x24, 0x00,
                                         0xc6, 0x90, 0x21, 0xe0, 0x5c, 0x08, 0x30, 0x3c, 0x04, 0x00, 0x00, 0x82,
                                         0x04, 0x26, 0x1d, 0x41, 0x00, 0x00, 0x00, 0x80, 0x0a, 0xc0, 0x20, 0x40,
                                         0x04, 0x20, 0x38, 0x20, 0x02, 0x58, 0x04, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize dc_charge_loop_res ongoing") {

        message_20::DC_ChargeLoopResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456334};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.control_mode.emplace<message_20::datatypes::Scheduled_DC_CLResControlMode>();
        res.current_limit_achieved = true;
        res.power_limit_achieved = true;
        res.voltage_limit_achieved = true;
        res.present_current = {1000, -3};
        res.present_voltage = {4000, -1};

        std::vector<uint8_t> expected = {0x80, 0x38, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0xeb, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x63, 0xe8,
                                         0x74, 0x03, 0x81, 0xfc, 0x28, 0x07, 0xc2, 0x22, 0x90};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize dc_charge_loop res ongoing") {

        uint8_t doc_raw[] = {0x80, 0x38, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xeb, 0xfe, 0x1b,
                             0x60, 0x62, 0x00, 0x63, 0xe8, 0x74, 0x03, 0x81, 0xfc, 0x28, 0x07, 0xc2, 0x22, 0x90};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_ChargeLoopRes);

            const auto& msg = variant.get<message_20::DC_ChargeLoopResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456334);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.current_limit_achieved == true);
            REQUIRE(msg.power_limit_achieved == true);
            REQUIRE(msg.voltage_limit_achieved == true);
            REQUIRE(dt::from_RationalNumber(msg.present_current) == 1);
            REQUIRE(dt::from_RationalNumber(msg.present_voltage) == 400);
            REQUIRE(std::holds_alternative<message_20::datatypes::Scheduled_DC_CLResControlMode>(msg.control_mode));
            const auto& control_mode = std::get<message_20::datatypes::Scheduled_DC_CLResControlMode>(msg.control_mode);
            REQUIRE(control_mode.max_charge_current.has_value() == false);
            REQUIRE(control_mode.max_charge_power.has_value() == false);
            REQUIRE(control_mode.max_voltage.has_value() == false);
            REQUIRE(control_mode.min_charge_power.has_value() == false);
        }
    }
}
