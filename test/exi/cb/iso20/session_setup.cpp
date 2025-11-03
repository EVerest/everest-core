#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/session_setup.hpp>
#include <iso15118/message/variant.hpp>

#include <string>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize session setup messages") {

    GIVEN("Deserialize session_setup_req") {
        uint8_t doc_raw[] = {0x80, 0x8c, 0x4,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xc,  0x9f,
                             0x9c, 0x2b, 0xd0, 0x62, 0xb,  0x2b, 0xa6, 0xa4, 0xab, 0x18, 0x99, 0x19, 0x9a,
                             0x1a, 0x9b, 0x1b, 0x9c, 0x1c, 0x98, 0x20, 0xa1, 0x21, 0xa2, 0x22, 0xac, 0x0};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::SessionSetupReq);

            const auto& msg = variant.get<message_20::SessionSetupRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0});
            REQUIRE(header.timestamp == 1739635913);

            REQUIRE(msg.evccid == "WMIV1234567890ABCDEX");
        }
    }

    GIVEN("Serialize session_setup_res") {

        const auto header = message_20::Header{{0x2E, 0xFA, 0x18, 0x94, 0xDC, 0x7B, 0x90, 0x11}, 1739635913};

        const auto res = message_20::SessionSetupResponse{
            header, message_20::datatypes::ResponseCode::OK_NewSessionEstablished, "DE*PNX*E12345*1"};

        std::vector<uint8_t> expected = {0x80, 0x90, 0x4,  0x17, 0x7d, 0xc,  0x4a, 0x6e, 0x3d, 0xc8, 0x8,  0x8c,
                                         0x9f, 0x9c, 0x2b, 0xd0, 0x62, 0x4,  0x4,  0x51, 0x11, 0x4a, 0x94, 0x13,
                                         0x96, 0xa,  0x91, 0x4c, 0x4c, 0x8c, 0xcd, 0xd,  0x4a, 0x8c, 0x40};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize session_setup_res") {
        uint8_t doc_raw[] = {0x80, 0x90, 0x4,  0x17, 0x7d, 0xc,  0x4a, 0x6e, 0x3d, 0xc8, 0x8,  0x8c,
                             0x9f, 0x9c, 0x2b, 0xd0, 0x62, 0x4,  0x4,  0x51, 0x11, 0x4a, 0x94, 0x13,
                             0x96, 0xa,  0x91, 0x4c, 0x4c, 0x8c, 0xcd, 0xd,  0x4a, 0x8c, 0x40};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::SessionSetupRes);

            const auto& msg = variant.get<message_20::SessionSetupResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x2E, 0xFA, 0x18, 0x94, 0xDC, 0x7B, 0x90, 0x11});
            REQUIRE(header.timestamp == 1739635913);

            REQUIRE(msg.evseid == "DE*PNX*E12345*1");
        }
    }

    GIVEN("Serialize session_setup_req") {

        const auto header = message_20::Header{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 1739635913};

        const auto res = message_20::SessionSetupRequest{header, "WMIV1234567890ABCDEX"};

        std::vector<uint8_t> expected = {0x80, 0x8c, 0x4,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xc,  0x9f,
                                         0x9c, 0x2b, 0xd0, 0x62, 0xb,  0x2b, 0xa6, 0xa4, 0xab, 0x18, 0x99, 0x19, 0x9a,
                                         0x1a, 0x9b, 0x1b, 0x9c, 0x1c, 0x98, 0x20, 0xa1, 0x21, 0xa2, 0x22, 0xac, 0x0};
        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
