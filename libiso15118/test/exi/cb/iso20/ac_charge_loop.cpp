#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/ac_charge_loop.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

namespace dt = iso15118::message_20::datatypes;

SCENARIO("Se/Deserialize ac charge loop messages") {

    GIVEN("Deserialize ac_charge_loop_req_scheduled mode") {

        uint8_t doc_raw[] = {0x80, 0x08, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb, 0xfe, 0x1b,
                             0x60, 0x62, 0x88, 0x04, 0x00, 0x5c, 0xb0, 0x00, 0x40, 0x07, 0x02, 0xe8, 0x04, 0x00, 0x00,
                             0x80, 0x7e, 0x08, 0x01, 0x90, 0x10, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3f, 0x06, 0x80,
                             0x78, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02, 0x00, 0x38, 0x17, 0x40, 0x40, 0x00, 0x00,
                             0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20AC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AC_ChargeLoopReq);

            const auto& msg = variant.get<message_20::AC_ChargeLoopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456333);
            REQUIRE(msg.meter_info_requested == false);

            using ScheduledMode = message_20::datatypes::Scheduled_AC_CLReqControlMode;

            REQUIRE(std::holds_alternative<ScheduledMode>(msg.control_mode));
            const auto& control_mode = std::get<ScheduledMode>(msg.control_mode);
            REQUIRE(control_mode.target_energy_request.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_energy_request) == 12345.0f);
            REQUIRE(control_mode.max_energy_request.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.max_energy_request) == 12000.0f);
            REQUIRE(control_mode.min_energy_request.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.min_energy_request) == 1.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.present_active_power) == 12000.0f);
            REQUIRE(control_mode.present_active_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L2) == 0.0f);
            REQUIRE(control_mode.present_active_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L3) == 0.0f);
            REQUIRE(control_mode.present_reactive_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_reactive_power) == 0.0f);
            REQUIRE(control_mode.present_reactive_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_reactive_power_L2) == 0.0f);
            REQUIRE(control_mode.present_reactive_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_reactive_power_L3) == 0.0f);
            REQUIRE(control_mode.max_charge_power.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.max_charge_power) == 32);
            REQUIRE(control_mode.max_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.max_charge_power_L2) == 0.0f);
            REQUIRE(control_mode.max_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.max_charge_power_L3) == 0.0f);
            REQUIRE(control_mode.min_charge_power.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.min_charge_power) == 20);
            REQUIRE(control_mode.min_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.min_charge_power_L2) == 0.0f);
            REQUIRE(control_mode.min_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.min_charge_power_L3) == 0.0f);
        }
    }

    GIVEN("Serialize ac_charge_loop request_scheduled mode") {

        message_20::AC_ChargeLoopRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456333};
        req.meter_info_requested = false;
        auto& control_mode = req.control_mode.emplace<message_20::datatypes::Scheduled_AC_CLReqControlMode>();

        control_mode.target_energy_request = {12345, 0};
        control_mode.max_energy_request = {12000, 0};
        control_mode.min_energy_request = {1, 0};
        control_mode.max_charge_power = {3200, -2};
        control_mode.max_charge_power_L2 = {0, 0};
        control_mode.max_charge_power_L3 = {0, 0};
        control_mode.min_charge_power = {2000, -2};
        control_mode.min_charge_power_L2 = {0, 0};
        control_mode.min_charge_power_L3 = {0, 0};
        control_mode.present_active_power = {12000, 0};
        control_mode.present_active_power_L2 = {0, 0};
        control_mode.present_active_power_L3 = {0, 0};
        control_mode.present_reactive_power = {0, 0};
        control_mode.present_reactive_power_L2 = {0, 0};
        control_mode.present_reactive_power_L3 = {0, 0};

        std::vector<uint8_t> expected = {0x80, 0x08, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb,
                                         0xfe, 0x1b, 0x60, 0x62, 0x88, 0x04, 0x00, 0x5c, 0xb0, 0x00, 0x40, 0x07, 0x02,
                                         0xe8, 0x04, 0x00, 0x00, 0x80, 0x7e, 0x08, 0x01, 0x90, 0x10, 0x00, 0x00, 0x02,
                                         0x00, 0x00, 0x00, 0x3f, 0x06, 0x80, 0x78, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00,
                                         0x02, 0x00, 0x38, 0x17, 0x40, 0x40, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x00,
                                         0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Deserialize ac_charge_loop_req dynamic mode") {

        uint8_t doc_raw[] = {0x80, 0x08, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb, 0xfe,
                             0x1b, 0x60, 0x62, 0x00, 0x51, 0x84, 0x0c, 0x78, 0x67, 0xed, 0x5b, 0x83, 0x04, 0x08,
                             0x78, 0x17, 0x02, 0x04, 0x3c, 0x0b, 0x81, 0x00, 0x00, 0x20, 0x82, 0x0d, 0xc0, 0xb0,
                             0x20, 0x00, 0x00, 0x08, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40,
                             0x00, 0x00, 0x20, 0x03, 0x81, 0x74, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00,
                             0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20AC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AC_ChargeLoopReq);

            const auto& msg = variant.get<message_20::AC_ChargeLoopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456333);
            REQUIRE(msg.meter_info_requested == false);

            REQUIRE(msg.display_parameters.has_value() == true);
            const auto& display_parameters = msg.display_parameters.value();

            REQUIRE(display_parameters.present_soc.value_or(0) == 10);
            REQUIRE(display_parameters.charging_complete.value_or(true) == false);

            using DynamicMode = message_20::datatypes::Dynamic_AC_CLReqControlMode;

            REQUIRE(std::holds_alternative<DynamicMode>(msg.control_mode));
            const auto& control_mode = std::get<DynamicMode>(msg.control_mode);

            REQUIRE(control_mode.departure_time.has_value() == true);
            REQUIRE(control_mode.departure_time.value() == 1727440880);
            REQUIRE(dt::from_RationalNumber(control_mode.target_energy_request) == 60000.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_energy_request) == 60000.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.min_energy_request) == 1.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.max_charge_power) == 150000.0f);
            REQUIRE(control_mode.max_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.max_charge_power_L2) == 0.0f);
            REQUIRE(control_mode.max_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.max_charge_power_L3) == 0.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.min_charge_power) == 0.0f);
            REQUIRE(control_mode.min_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.min_charge_power_L2) == 0.0f);
            REQUIRE(control_mode.min_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*control_mode.min_charge_power_L3) == 0.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.present_active_power) == 12000.0f);
            REQUIRE(control_mode.present_active_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L2) == 0.0f);
            REQUIRE(control_mode.present_active_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L3) == 0.0f);
            REQUIRE(dt::from_RationalNumber(control_mode.present_reactive_power) == 0.0f);
            REQUIRE(control_mode.present_reactive_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_reactive_power_L2) == 0.0f);
            REQUIRE(control_mode.present_reactive_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_reactive_power_L3) == 0.0f);
        }
    }

    GIVEN("Serialize ac_charge_loop request_dynamic mode") {

        message_20::AC_ChargeLoopRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456333};
        req.meter_info_requested = false;
        auto& disparam = req.display_parameters.emplace();
        disparam.present_soc = 10;
        disparam.charging_complete = false;
        auto& control_mode = req.control_mode.emplace<message_20::datatypes::Dynamic_AC_CLReqControlMode>();
        control_mode.departure_time = 1727440880;
        control_mode.target_energy_request = {6000, 1};
        control_mode.max_energy_request = {6000, 1};
        control_mode.min_energy_request = {1, 0};
        control_mode.max_charge_power = {1500, 2};
        control_mode.max_charge_power_L2 = {0, 0};
        control_mode.max_charge_power_L3 = {0, 0};
        control_mode.min_charge_power = {0, 0};
        control_mode.min_charge_power_L2 = {0, 0};
        control_mode.min_charge_power_L3 = {0, 0};
        control_mode.present_active_power = {12000, 0};
        control_mode.present_active_power_L2 = {0, 0};
        control_mode.present_active_power_L3 = {0, 0};
        control_mode.present_reactive_power = {0, 0};
        control_mode.present_reactive_power_L2 = {0, 0};
        control_mode.present_reactive_power_L3 = {0, 0};

        std::vector<uint8_t> expected = {
            0x80, 0x08, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb, 0xfe, 0x1b, 0x60,
            0x62, 0x00, 0x51, 0x84, 0x0c, 0x78, 0x67, 0xed, 0x5b, 0x83, 0x04, 0x08, 0x78, 0x17, 0x02, 0x04,
            0x3c, 0x0b, 0x81, 0x00, 0x00, 0x20, 0x82, 0x0d, 0xc0, 0xb0, 0x20, 0x00, 0x00, 0x08, 0x00, 0x00,
            0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x03, 0x81, 0x74, 0x08, 0x00,
            0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Deserialize ac_charge_loop_res scheduled mode") {
        uint8_t doc_raw[] = {0x80, 0x0c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xeb,
                             0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10, 0x0c, 0xc4, 0x69, 0x04, 0xb1, 0x20, 0x00,
                             0xc8, 0x80, 0x40, 0x07, 0x02, 0xe8, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
                             0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x70,
                             0x2e, 0x81, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20AC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AC_ChargeLoopRes);

            const auto& msg = variant.get<message_20::AC_ChargeLoopResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456334);

            REQUIRE(msg.meter_info.has_value() == true);
            const auto& meter_info = msg.meter_info.value();

            REQUIRE(meter_info.meter_id == "1");
            REQUIRE(meter_info.charged_energy_reading_wh == 1234);

            using ScheduledMode = message_20::datatypes::Scheduled_AC_CLResControlMode;

            REQUIRE(std::holds_alternative<ScheduledMode>(msg.control_mode));
            const auto& control_mode = std::get<ScheduledMode>(msg.control_mode);

            REQUIRE(control_mode.target_active_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_active_power) == 12000.0f);
            REQUIRE(control_mode.target_active_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_active_power_L2) == 0.0f);
            REQUIRE(control_mode.target_active_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_active_power_L3) == 0.0f);
            REQUIRE(control_mode.target_reactive_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_reactive_power) == 0.0f);
            REQUIRE(control_mode.target_reactive_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_reactive_power_L2) == 0.0f);
            REQUIRE(control_mode.target_reactive_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.target_reactive_power_L3) == 0.0f);
            REQUIRE(control_mode.present_active_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power) == 12000.0f);
            REQUIRE(control_mode.present_active_power_L2.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L2) == 0.0f);
            REQUIRE(control_mode.present_active_power_L3.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*control_mode.present_active_power_L3) == 0.0f);
        }
    }

    GIVEN("Serialize ac_charge_loop response scheduled") {

        message_20::AC_ChargeLoopResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456334};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.target_frequency = {50, 0};
        auto& met_info = res.meter_info.emplace();
        met_info.meter_id = "1";
        met_info.charged_energy_reading_wh = 1234;

        auto& res_ctrl_mode = res.control_mode.emplace<message_20::datatypes::Scheduled_AC_CLResControlMode>();
        res_ctrl_mode.target_active_power = {12000, 0};
        res_ctrl_mode.target_active_power_L2 = {0, 0};
        res_ctrl_mode.target_active_power_L3 = {0, 0};
        res_ctrl_mode.target_reactive_power = {0, 0};
        res_ctrl_mode.target_reactive_power_L2 = {0, 0};
        res_ctrl_mode.target_reactive_power_L3 = {0, 0};
        res_ctrl_mode.present_active_power = {12000, 0};
        res_ctrl_mode.present_active_power_L2 = {0, 0};
        res_ctrl_mode.present_active_power_L3 = {0, 0};

        std::vector<uint8_t> expected = {0x80, 0x0c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xeb,
                                         0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10, 0x0c, 0xc4, 0x69, 0x04, 0xb1, 0x20, 0x00,
                                         0xc8, 0x80, 0x40, 0x07, 0x02, 0xe8, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
                                         0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x70,
                                         0x2e, 0x81, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
