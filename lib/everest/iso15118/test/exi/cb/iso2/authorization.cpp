#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/authorization.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Ser/Deserialize d2 authorization messages") {
    GIVEN("Deserialize authorization req") {
        // TODO(kd): Test deserialization of GenChallenge field

        uint8_t doc_raw[] = {0x80, 0x98, 0x2, 0x0, 0xb6, 0xc8, 0x81, 0xce, 0xc2, 0x13, 0x4b, 0x50, 0x8};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::AuthorizationReq);

            const auto& msg = variant.get<d2::msg::AuthorizationRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.id == "");
        }
    }
    GIVEN("Serialize authorization res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        const auto res = d2::msg::AuthorizationResponse{header, d2::msg::data_types::ResponseCode::OK,
                                                        d2::msg::data_types::EvseProcessing::Ongoing};

        std::vector<uint8_t> expected = {0x80, 0x98, 0x2,  0x0,  0xb6, 0xc8, 0x81, 0xce,
                                         0xc2, 0x13, 0x4b, 0x50, 0x10, 0x1,  0x0};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
