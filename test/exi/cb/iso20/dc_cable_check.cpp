#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_cable_check.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize dc cable check messages") {

    GIVEN("Deserialize dc_cable_check_req") {

        uint8_t doc_raw[] = {0x80, 0x2c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                             0x6c, 0x4d, 0x8c, 0x7b, 0xfe, 0x1b, 0x60, 0x62};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_CableCheckReq);

            const auto& msg = variant.get<message_20::DC_CableCheckRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456327);
        }
    }

    GIVEN("Serialize dc_cable_check_req") {

        message_20::DC_CableCheckRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456327};

        std::vector<uint8_t> expected = {0x80, 0x2c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7,
                                         0x6c, 0x4d, 0x8c, 0x7b, 0xfe, 0x1b, 0x60, 0x62};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize dc_cable_check_res ongoing") {

        message_20::DC_CableCheckResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456328};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.processing = message_20::datatypes::Processing::Ongoing;

        std::vector<uint8_t> expected = {0x80, 0x30, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0x8b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize dc_cable_check_res_ongoing") {

        uint8_t doc_raw[] = {0x80, 0x30, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                             0x4d, 0x8c, 0x8b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x10};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_CableCheckRes);

            const auto& msg = variant.get<message_20::DC_CableCheckResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456328);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.processing == message_20::datatypes::Processing::Ongoing);
        }
    }

    GIVEN("Serialize dc_cable_check_res finished") {

        message_20::DC_CableCheckResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456331};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.processing = message_20::datatypes::Processing::Finished;

        std::vector<uint8_t> expected = {0x80, 0x30, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0xbb, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize dc_cable_check_res_finished") {

        uint8_t doc_raw[] = {0x80, 0x30, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                             0x4d, 0x8c, 0xbb, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_CableCheckRes);

            const auto& msg = variant.get<message_20::DC_CableCheckResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456331);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.processing == message_20::datatypes::Processing::Finished);
        }
    }
}
