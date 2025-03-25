#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/service_selection.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize service_selection messages") {

    GIVEN("Deserialize service_selection_req") {

        uint8_t doc_raw[] = {0x80, 0x84, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c, 0xed, 0xa1,
                             0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x01, 0x40, 0x08, 0x80};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceSelectionReq);

            const auto& msg = variant.get<message_20::ServiceSelectionRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42});
            REQUIRE(header.timestamp == 1692009443);

            REQUIRE(msg.selected_energy_transfer_service.service_id == message_20::datatypes::ServiceCategory::AC_BPT);
            REQUIRE(msg.selected_energy_transfer_service.parameter_set_id == 1);
        }
    }

    GIVEN("Serialize service_selection_req") {

        message_20::ServiceSelectionRequest req;

        req.header = message_20::Header{{0x04, 0xEB, 0xFF, 0x2C, 0x94, 0x59, 0xDB, 0x42}, 1692009443};
        req.selected_energy_transfer_service.service_id = message_20::datatypes::ServiceCategory::AC_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 1;

        std::vector<uint8_t> expected = {0x80, 0x84, 0x04, 0x02, 0x75, 0xff, 0x96, 0x4a, 0x2c, 0xed, 0xa1,
                                         0x0e, 0x38, 0x7e, 0x8a, 0x60, 0x62, 0x01, 0x40, 0x08, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    // TODO(sl): Adding test with service_selection_req selected_vas_list

    GIVEN("Serialize service_selection_res") {

        message_20::ServiceSelectionResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456323};
        res.response_code = message_20::datatypes::ResponseCode::OK;

        std::vector<uint8_t> expected = {0x80, 0x88, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                                         0x4d, 0x8c, 0x3b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize service_selection_res") {

        uint8_t doc_raw[] = {0x80, 0x88, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c,
                             0x4d, 0x8c, 0x3b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be decoded successfully") {

            REQUIRE(variant.get_type() == message_20::Type::ServiceSelectionRes);

            const auto& msg = variant.get<message_20::ServiceSelectionResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456323);

            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
        }
    }
}
