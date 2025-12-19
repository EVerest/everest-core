#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/variant.hpp>
#include <iso15118/message/d2/welding_detection.hpp>

#include "helper.hpp"

#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

using namespace iso15118;

SCENARIO("Ser/Deserialize d2 welding detection messages") {
    GIVEN("Deserialize welding detection req - minimal") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE,
                             0xC2, 0x13, 0x4B, 0x52, 0x11, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::WeldingDetectionReq);

            const auto& msg = variant.get<d2::msg::WeldingDetectionRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            // TODO(kd): Should EV/EVSEStatus also be tested here?
            REQUIRE(msg.ev_status.ev_ready == true);
            REQUIRE(msg.ev_status.ev_error_code == d2::msg::data_types::DC_EVErrorCode::NO_ERROR);
            REQUIRE(msg.ev_status.ev_ress_soc == 0);
        }
    }
    GIVEN("Serialize welding detection res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        const auto res = d2::msg::WeldingDetectionResponse{
            header,
            d2::msg::data_types::ResponseCode::OK,
            d2::msg::data_types::DC_EVSEStatus{
                {},
                std::nullopt,
                d2::msg::data_types::DC_EVSEStatusCode::EVSE_Ready,
            },
            d2::msg::data_types::from_float(50, d2::msg::data_types::UnitSymbol::V),
        };

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B,
                                         0x52, 0x20, 0x00, 0x00, 0x02, 0x10, 0x11, 0x02, 0x20, 0x9C, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
