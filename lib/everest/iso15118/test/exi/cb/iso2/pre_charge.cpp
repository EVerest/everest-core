#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/pre_charge.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

#include <vector>

using namespace iso15118;

SCENARIO("Ser/Deserialize d2 pre charge messages") {
    GIVEN("Deserialize pre charge req") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x2, 0x0, 0xb6, 0xc8, 0x81, 0xce, 0xc2, 0x13, 0x4b, 0x51,
                             0x71, 0x0,  0x0, 0x2, 0x88, 0x0,  0xa0, 0x61, 0x80, 0x0,  0x0};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::PreChargeReq);

            const auto& msg = variant.get<d2::msg::PreChargeRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            // TODO(kd): Should EV/EVSEStatus also be tested here?

            REQUIRE(d2::msg::data_types::from_PhysicalValue(msg.ev_target_voltage) == 500);
            REQUIRE(d2::msg::data_types::from_PhysicalValue(msg.ev_target_current) == 0);
        }
    }
    GIVEN("Serialize pre charge res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        const auto res = d2::msg::PreChargeResponse{
            header,
            d2::msg::data_types::ResponseCode::OK,
            d2::msg::data_types::DC_EVSEStatus{
                {},
                d2::msg::data_types::isolationLevel::Valid,
                d2::msg::data_types::DC_EVSEStatusCode::EVSE_Ready,
            },
            d2::msg::data_types::from_float(498.5, d2::msg::data_types::UnitSymbol::V),
        };

        std::vector<uint8_t> expected = {0x80, 0x98, 0x2, 0x0, 0xb6, 0xc8, 0x81, 0xce, 0xc2, 0x13, 0x4b, 0x51,
                                         0x80, 0x0,  0x0, 0x0, 0x20, 0x40, 0x84, 0xf,  0x92, 0x60, 0x0};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
