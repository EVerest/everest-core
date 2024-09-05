#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/service_discovery.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize service_discovery messages") {

    GIVEN("Deserialize service_discovery_req") {

        uint8_t doc_raw[] = {0x80, 0x7c, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c,
                             0xed, 0xa1, 0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x80};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded succussfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDiscoveryReq);

            const auto& msg = variant.get<message_20::ServiceDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42});
            REQUIRE(header.timestamp == 1692009443);
        }
    }

    // TODO(sl): Adding test with service_discovery_req supported_service_ids

    GIVEN("Serialize service_discovery_res") {

        message_20::ServiceDiscoveryResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456322};
        res.response_code = message_20::ResponseCode::OK;
        res.service_renegotiation_supported = false;
        res.energy_transfer_service_list = {{message_20::ServiceCategory::DC, false},
                                            {message_20::ServiceCategory::DC_BPT, false}};

        std::vector<uint8_t> expected = {0x80, 0x80, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c,
                                         0x2b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00, 0x02, 0x00, 0x01, 0x80, 0x50};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
