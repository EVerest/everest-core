#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/session_stop.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize session stop messages") {

    GIVEN("Deserialize session_stop_req") {

        uint8_t doc_raw[] = {0x80, 0x94, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                             0x6c, 0x4d, 0x8d, 0x7b, 0xfe, 0x1b, 0x60, 0x62, 0x28};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::SessionStopReq);

            const auto& msg = variant.get<message_20::SessionStopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456343);

            REQUIRE(msg.charging_session == message_20::datatypes::ChargingSession::Terminate);
        }
    }

    GIVEN("Serialize session_stop_req") {

        message_20::SessionStopRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456343};
        req.charging_session = message_20::datatypes::ChargingSession::Terminate;

        std::vector<uint8_t> expected = {0x80, 0x94, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                                         0x6c, 0x4d, 0x8d, 0x7b, 0xfe, 0x1b, 0x60, 0x62, 0x28};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize session_stop_res") {

        message_20::SessionStopResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456343};
        res.response_code = message_20::datatypes::ResponseCode::OK;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8d, 0x7b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Deserialize session_stop_res") {

        uint8_t doc_raw[] = {0x80, 0x98, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                             0x4d, 0x8d, 0x7b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::SessionStopRes);

            const auto& msg = variant.get<message_20::SessionStopResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456343);

            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
        }
    }
}
