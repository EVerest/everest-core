#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/session_setup.hpp>
#include <iso15118/message/variant.hpp>

#include <string>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize session setup messages") {

    GIVEN("Deserialize session_setup_req") {
        uint8_t doc_raw[] = {0x80, 0x8c, 0x00, 0x80, 0x0d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x0b,
                             0x2b, 0xa6, 0xa4, 0xab, 0x18, 0x99, 0x19, 0x9a, 0x1a, 0x9b, 0x1b,
                             0x9c, 0x1c, 0x98, 0x20, 0xa1, 0x21, 0xa2, 0x22, 0xac, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized succussfully") {
            REQUIRE(variant.get_type() == message_20::Type::SessionSetupReq);

            const auto& msg = variant.get<message_20::SessionSetupRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0});
            REQUIRE(header.timestamp == 1691411798);

            REQUIRE(msg.evccid == "WMIV1234567890ABCDEX");
        }
    }

    GIVEN("Serialize session_setup_res") {

        const auto header = message_20::Header{{0xF2, 0x19, 0x15, 0xB9, 0xDD, 0xDC, 0x12, 0xD1}, 1691411798};

        const auto res = message_20::SessionSetupResponse{
            header, message_20::datatypes::ResponseCode::OK_NewSessionEstablished, "UK123E1234"};

        std::vector<uint8_t> expected = {0x80, 0x90, 0x04, 0x79, 0x0c, 0x8a, 0xdc, 0xee, 0xee, 0x09,
                                         0x68, 0x8d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x04, 0x03, 0x15,
                                         0x52, 0xcc, 0x4c, 0x8c, 0xd1, 0x4c, 0x4c, 0x8c, 0xcd, 0x00};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
