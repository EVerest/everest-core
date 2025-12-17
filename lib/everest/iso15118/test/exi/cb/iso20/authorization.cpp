#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/authorization.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize authorization messages") {

    GIVEN("Deserialize authorization_req eim") {

        uint8_t doc_raw[] = {0x80, 0x00, 0x04, 0x79, 0x0c, 0x8a, 0xdc, 0xee, 0xee,
                             0x09, 0x68, 0x8d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AuthorizationReq);

            const auto& msg = variant.get<message_20::AuthorizationRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0xF2, 0x19, 0x15, 0xB9, 0xDD, 0xDC, 0x12, 0xD1});
            REQUIRE(header.timestamp == 1691411798);

            REQUIRE(msg.selected_authorization_service == message_20::datatypes::Authorization::EIM);
            REQUIRE(std::holds_alternative<message_20::datatypes::EIM_ASReqAuthorizationMode>(msg.authorization_mode));
        }
    }

    // TODO(sl): Adding authorization_req pnc tests

    GIVEN("Serialize authorization_res") {

        message_20::AuthorizationResponse res;

        res.header = message_20::Header{{0xF2, 0x19, 0x15, 0xB9, 0xDD, 0xDC, 0x12, 0xD1}, 1691411798};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        res.evse_processing = message_20::datatypes::Processing::Finished;

        std::vector<uint8_t> expected = {0x80, 0x04, 0x04, 0x79, 0x0c, 0x8a, 0xdc, 0xee, 0xee, 0x09,
                                         0x68, 0x8d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Deserialize authorization_res eim") {

        uint8_t doc_raw[] = {0x80, 0x04, 0x04, 0x79, 0x0c, 0x8a, 0xdc, 0xee, 0xee, 0x09,
                             0x68, 0x8d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20Main, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AuthorizationRes);

            const auto& msg = variant.get<message_20::AuthorizationResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0xF2, 0x19, 0x15, 0xB9, 0xDD, 0xDC, 0x12, 0xD1});
            REQUIRE(header.timestamp == 1691411798);

            REQUIRE(msg.evse_processing == message_20::datatypes::Processing::Finished);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);
        }
    }

    GIVEN("Serialize authorization_req eim") {

        message_20::AuthorizationRequest req;

        req.header = message_20::Header{{0xF2, 0x19, 0x15, 0xB9, 0xDD, 0xDC, 0x12, 0xD1}, 1691411798};
        req.selected_authorization_service = message_20::datatypes::Authorization::EIM;
        req.authorization_mode = message_20::datatypes::EIM_ASReqAuthorizationMode{};
        // Todo(sl): Adding certificate

        std::vector<uint8_t> expected = {0x80, 0x00, 0x04, 0x79, 0x0c, 0x8a, 0xdc, 0xee, 0xee,
                                         0x09, 0x68, 0x8d, 0x6c, 0xac, 0x3a, 0x60, 0x62, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }
}
