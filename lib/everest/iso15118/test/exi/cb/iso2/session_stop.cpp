#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/session_stop.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 session stop messages") {
    GIVEN("Deserialize session stop req") {

        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0xF0, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::SessionStopReq);

            const auto& msg = variant.get<d2::msg::SessionStopRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.charging_session == dt::ChargingSession::Terminate);
        }
    }
    GIVEN("Serialize session stop res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        const auto res = d2::msg::SessionStopResponse{
            header,
            d2::msg::data_types::ResponseCode::OK,
        };

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81,
                                         0xCE, 0xC2, 0x13, 0x4B, 0x52, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
