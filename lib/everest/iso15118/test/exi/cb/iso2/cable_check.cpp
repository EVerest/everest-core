#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/cable_check.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

using namespace iso15118;

SCENARIO("Ser/Deserialize d2 cable check messages") {
    GIVEN("Deserialize cable check req") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x2, 0x0, 0xb6, 0xc8, 0x81, 0xce, 0xc2, 0x13, 0x4b, 0x50, 0x31, 0x0, 0x0, 0x0};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::CableCheckReq);

            const auto& msg = variant.get<d2::msg::CableCheckRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            // TODO(kd): Should EV/EVSEStatus also be tested here?
            REQUIRE(msg.ev_status.ev_ready == true);
            REQUIRE(msg.ev_status.ev_error_code == d2::msg::data_types::DC_EVErrorCode::NO_ERROR);
            REQUIRE(msg.ev_status.ev_ress_soc == 0);
        }
    }
    GIVEN("Serialize cable check res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        const auto res = d2::msg::CableCheckResponse{
            header, d2::msg::data_types::ResponseCode::OK,
            d2::msg::data_types::DC_EVSEStatus{{},
                                               d2::msg::data_types::isolationLevel::Invalid,
                                               d2::msg::data_types::DC_EVSEStatusCode::EVSE_IsolationMonitoringActive},
            d2::msg::data_types::EVSEProcessing::Ongoing};

        std::vector<uint8_t> expected = {0x80, 0x98, 0x2,  0x0, 0xb6, 0xc8, 0x81, 0xce, 0xc2, 0x13,
                                         0x4b, 0x50, 0x40, 0x0, 0x0,  0x0,  0x1,  0x1,  0x0};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
