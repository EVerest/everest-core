#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_pre_charge.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize dc pre charge messages") {

    GIVEN("Deserialize dc_pre_charge_req") {

        uint8_t doc_raw[] = {0x80, 0x44, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0xbb,
                             0xfe, 0x1b, 0x60, 0x62, 0x21, 0x00, 0x12, 0x00, 0x60, 0x80, 0x09, 0x00, 0x30};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized succussfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_PreChargeReq);

            const auto& msg = variant.get<message_20::DC_PreChargeRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456331);
            REQUIRE(msg.processing == message_20::Processing::Ongoing);
            REQUIRE(message_20::from_RationalNumber(msg.present_voltage) == 400);
            REQUIRE(message_20::from_RationalNumber(msg.target_voltage) == 400);
        }
    }

    GIVEN("Serialize dc_pre_charge_res") {

        message_20::DC_PreChargeResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456332};
        res.response_code = message_20::ResponseCode::OK;
        res.present_voltage = {4000, -1};

        std::vector<uint8_t> expected = {0x80, 0x48, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c,
                                         0xcb, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x0f, 0xe1, 0x40, 0x3e, 0x00};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
