#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/service_detail.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize service_detail messages") {

    GIVEN("Deserialize service_detail_req") {

        uint8_t doc_raw[] = {0x80, 0x74, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c, 0xed,
                             0xa1, 0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x02, 0x80};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded succussfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDetailReq);

            const auto& msg = variant.get<message_20::ServiceDetailRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42});
            REQUIRE(header.timestamp == 1692009443);

            REQUIRE(msg.service == message_20::datatypes::ServiceCategory::AC_BPT);
        }
    }

    GIVEN("Serialize service_detail_res") {

        message_20::ServiceDetailResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456323};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.service = message_20::datatypes::ServiceCategory::DC;

        const auto list = message_20::datatypes::DcParameterList{
            message_20::datatypes::DcConnector::Extended, message_20::datatypes::ControlMode::Scheduled,
            message_20::datatypes::MobilityNeedsMode::ProvidedByEvcc, message_20::datatypes::Pricing::NoPricing};
        res.service_parameter_list = {message_20::datatypes::ParameterSet(0, list)};

        std::vector<uint8_t> expected = {
            0x80, 0x78, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x3b, 0xfe, 0x1b, 0x60,
            0x62, 0x00, 0x00, 0x80, 0x00, 0x02, 0xd0, 0xdb, 0xdb, 0x9b, 0x99, 0x58, 0xdd, 0x1b, 0xdc, 0x98,
            0x04, 0x00, 0xd4, 0x36, 0xf6, 0xe7, 0x47, 0x26, 0xf6, 0xc4, 0xd6, 0xf6, 0x46, 0x56, 0x00, 0x80,
            0x4d, 0x35, 0xbd, 0x89, 0xa5, 0xb1, 0xa5, 0xd1, 0xe5, 0x39, 0x95, 0x95, 0x91, 0xcd, 0x35, 0xbd,
            0x91, 0x95, 0x80, 0x20, 0x09, 0x50, 0x72, 0x69, 0x63, 0x69, 0x6e, 0x67, 0x60, 0x00, 0xa0};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
