#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_welding_detection.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize dc welding detection messages") {

    GIVEN("Deserialize dc welding detection req") {

        uint8_t doc_raw[] = {0x80, 0x4c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                             0x6c, 0x4d, 0x8d, 0x5b, 0xfe, 0x1b, 0x60, 0x62, 0x20};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_WeldingDetectionReq);

            const auto& msg = variant.get<message_20::DC_WeldingDetectionRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456341);

            REQUIRE(msg.processing == message_20::datatypes::Processing::Ongoing);
        }
    }

    GIVEN("Serialize dc welding detection req") {

        message_20::DC_WeldingDetectionRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456341};
        req.processing = message_20::datatypes::Processing::Ongoing;

        std::vector<uint8_t> expected = {0x80, 0x4c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                                         0x6c, 0x4d, 0x8d, 0x5b, 0xfe, 0x1b, 0x60, 0x62, 0x20};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize dc_welding_detection res") {

        message_20::DC_WeldingDetectionResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456341};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.present_voltage = {0, 0};

        std::vector<uint8_t> expected = {0x80, 0x50, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d,
                                         0x8d, 0x5b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10, 0x00, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize dc welding detection res") {

        uint8_t doc_raw[] = {0x80, 0x50, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d,
                             0x8d, 0x5b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_WeldingDetectionRes);

            const auto& msg = variant.get<message_20::DC_WeldingDetectionResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456341);

            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.present_voltage.value == 0);
            REQUIRE(msg.present_voltage.exponent == 0);
        }
    }
}
