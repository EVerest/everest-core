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

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDiscoveryReq);

            const auto& msg = variant.get<message_20::ServiceDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42});
            REQUIRE(header.timestamp == 1692009443);
        }
    }

    GIVEN("Deserialize service_discovery_req_with_supported_service_ids") {

        uint8_t doc_raw[] = {0x80, 0x7c, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c, 0xed,
                             0xa1, 0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x00, 0x44};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDiscoveryReq);

            const auto& msg = variant.get<message_20::ServiceDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42});
            REQUIRE(header.timestamp == 1692009443);
            REQUIRE(msg.supported_service_ids.has_value() == true);
            REQUIRE(msg.supported_service_ids.value().size() == 1);
            REQUIRE(msg.supported_service_ids.value()[0] == 2);
        }
    }

    GIVEN("Serialize service_discovery_req") {

        message_20::ServiceDiscoveryRequest req;

        req.header = message_20::Header{{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42}, 1692009443};

        std::vector<uint8_t> expected = {0x80, 0x7c, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c,
                                         0xed, 0xa1, 0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize service_discovery_req_with_supported_service_ids") {

        message_20::ServiceDiscoveryRequest req;

        req.header = message_20::Header{{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42}, 1692009443};
        req.supported_service_ids = {2};

        std::vector<uint8_t> expected = {0x80, 0x7c, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c, 0xed,
                                         0xa1, 0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x00, 0x44};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    GIVEN("Serialize service_discovery_res") {

        message_20::ServiceDiscoveryResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456322};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.service_renegotiation_supported = false;
        res.energy_transfer_service_list = {{message_20::datatypes::ServiceCategory::DC, false},
                                            {message_20::datatypes::ServiceCategory::DC_BPT, false}};

        std::vector<uint8_t> expected = {0x80, 0x80, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c,
                                         0x2b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00, 0x02, 0x00, 0x01, 0x80, 0x50};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize service_discovery_res_with_vas_list") {

        message_20::ServiceDiscoveryResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456322};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.service_renegotiation_supported = false;
        res.energy_transfer_service_list = {{message_20::datatypes::ServiceCategory::DC, false},
                                            {message_20::datatypes::ServiceCategory::DC_BPT, false}};
        res.vas_list = {{message_20::datatypes::ServiceCategory::Internet, true}};

        std::vector<uint8_t> expected = {0x80, 0x80, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x2b,
                                         0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00, 0x02, 0x00, 0x01, 0x80, 0x40, 0x82, 0x22};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize service_discovery_res") {

        uint8_t doc_raw[] = {0x80, 0x80, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c,
                             0x2b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00, 0x02, 0x00, 0x01, 0x80, 0x50};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDiscoveryRes);

            const auto& msg = variant.get<message_20::ServiceDiscoveryResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456322);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.service_renegotiation_supported == false);
            REQUIRE(msg.energy_transfer_service_list.size() == 2);
            REQUIRE(msg.energy_transfer_service_list[0].service_id == message_20::datatypes::ServiceCategory::DC);
            REQUIRE(msg.energy_transfer_service_list[0].free_service == false);
            REQUIRE(msg.energy_transfer_service_list[1].service_id == message_20::datatypes::ServiceCategory::DC_BPT);
            REQUIRE(msg.energy_transfer_service_list[1].free_service == false);
        }
    }

    GIVEN("Deserialize service_discovery_res_with_vas_list") {

        uint8_t doc_raw[] = {0x80, 0x80, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x2b,
                             0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00, 0x02, 0x00, 0x01, 0x80, 0x40, 0x82, 0x22};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceDiscoveryRes);

            const auto& msg = variant.get<message_20::ServiceDiscoveryResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456322);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
            REQUIRE(msg.service_renegotiation_supported == false);
            REQUIRE(msg.energy_transfer_service_list.size() == 2);
            REQUIRE(msg.energy_transfer_service_list[0].service_id == message_20::datatypes::ServiceCategory::DC);
            REQUIRE(msg.energy_transfer_service_list[0].free_service == false);
            REQUIRE(msg.energy_transfer_service_list[1].service_id == message_20::datatypes::ServiceCategory::DC_BPT);
            REQUIRE(msg.energy_transfer_service_list[1].free_service == false);
            REQUIRE(msg.vas_list.value().size() == 1);
            REQUIRE(msg.vas_list.value()[0].service_id == message_20::datatypes::ServiceCategory::Internet);
            REQUIRE(msg.vas_list.value()[0].free_service == true);
        }
    }
}
