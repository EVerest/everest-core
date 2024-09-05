#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_charge_loop.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize dc charge loop messages") {

    GIVEN("Deserialize dc_charge_loop_req") {

        uint8_t doc_raw[] = {0x80, 0x34, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xdb, 0xfe, 0x1b,
                             0x60, 0x62, 0x81, 0x00, 0x12, 0x00, 0x64, 0x64, 0x00, 0x0a, 0x02, 0x00, 0x24, 0x00, 0xca};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized succussfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_ChargeLoopReq);

            const auto& msg = variant.get<message_20::DC_ChargeLoopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456333);
            REQUIRE(msg.meter_info_requested == false);
            REQUIRE(message_20::from_RationalNumber(msg.present_voltage) == 400);

            using ScheduledMode = message_20::DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode;

            REQUIRE(std::holds_alternative<ScheduledMode>(msg.control_mode));
            const auto& control_mode = std::get<ScheduledMode>(msg.control_mode);
            REQUIRE(message_20::from_RationalNumber(control_mode.target_current) == 20);
            REQUIRE(message_20::from_RationalNumber(control_mode.target_voltage) == 400);
        }
    }

    GIVEN("Serialize dc_charge_loop ongoing") {

        message_20::DC_ChargeLoopResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456334};
        res.response_code = message_20::ResponseCode::OK;
        res.control_mode.emplace<message_20::DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode>();
        res.current_limit_achieved = true;
        res.power_limit_achieved = true;
        res.voltage_limit_achieved = true;
        res.present_current = {1000, -3};
        res.present_voltage = {4000, -1};

        std::vector<uint8_t> expected = {0x80, 0x38, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0xeb, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x63, 0xe8,
                                         0x74, 0x03, 0x81, 0xfc, 0x28, 0x07, 0xc2, 0x22, 0x90};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
