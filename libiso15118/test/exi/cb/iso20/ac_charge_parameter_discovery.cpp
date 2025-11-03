#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/ac_charge_parameter_discovery.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize ac charge parameter discovery messages") {

    GIVEN("Deserialize ac_charge_parameter_discovery_req") {

        uint8_t doc_raw[] = {0x80, 0x10, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x3b,
                             0xfe, 0x1b, 0x60, 0x62, 0x07, 0xE0, 0x80, 0x19, 0x02, 0x00, 0x00, 0x00, 0x80,
                             0x00, 0x00, 0x3F, 0x06, 0x80, 0x78, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20AC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AC_ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<message_20::AC_ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456323);

            using AC_ModeReq = message_20::datatypes::AC_CPDReqEnergyTransferMode;

            REQUIRE(std::holds_alternative<AC_ModeReq>(msg.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeReq>(msg.transfer_mode);
            REQUIRE(message_20::datatypes::from_RationalNumber(transfer_mode.max_charge_power) == 32);
            REQUIRE(transfer_mode.max_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.max_charge_power_L2) == 0.0f);
            REQUIRE(transfer_mode.max_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.max_charge_power_L3) == 0.0f);
            REQUIRE(message_20::datatypes::from_RationalNumber(transfer_mode.min_charge_power) == 20);
            REQUIRE(transfer_mode.min_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.min_charge_power_L2) == 0.0f);
            REQUIRE(transfer_mode.min_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.min_charge_power_L3) == 0.0f);
        }
    }

    GIVEN("Serialize ac_charge_parameter_discovery_req") {

        using AC_ModeReq = message_20::datatypes::AC_CPDReqEnergyTransferMode;

        message_20::AC_ChargeParameterDiscoveryRequest req;

        req.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456323};
        auto& mode = req.transfer_mode.emplace<AC_ModeReq>();
        mode.max_charge_power = {3200, -2};
        mode.max_charge_power_L2 = {0, 0};
        mode.max_charge_power_L3 = {0, 0};
        mode.min_charge_power = {2000, -2};
        mode.min_charge_power_L2 = {0, 0};
        mode.min_charge_power_L3 = {0, 0};

        std::vector<uint8_t> expected = {0x80, 0x10, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x3b,
                                         0xfe, 0x1b, 0x60, 0x62, 0x07, 0xE0, 0x80, 0x19, 0x02, 0x00, 0x00, 0x00, 0x80,
                                         0x00, 0x00, 0x3F, 0x06, 0x80, 0x78, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(req) == expected);
        }
    }

    // TODO(sl): Adding BPT_AC_CPDReqEnergyTransferMode tests
    // TODO(rb): Adding BPT_AC_CPDResEnergyTransferMode tests

    GIVEN("Deserialize ac_charge_parameter_discovery_res") {

        uint8_t doc_raw[] = {0x80, 0x14, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x4b, 0xfe, 0x1b,
                             0x60, 0x62, 0x00, 0x04, 0x08, 0x50, 0x08, 0x81, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20,
                             0x43, 0x40, 0x3c, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20AC, stream_view);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == message_20::Type::AC_ChargeParameterDiscoveryRes);

            const auto& msg = variant.get<message_20::AC_ChargeParameterDiscoveryResponse>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456324);
            REQUIRE(msg.response_code == message_20::datatypes::ResponseCode::OK);

            using AC_ModeRes = message_20::datatypes::AC_CPDResEnergyTransferMode;

            REQUIRE(std::holds_alternative<AC_ModeRes>(msg.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeRes>(msg.transfer_mode);
            REQUIRE(message_20::datatypes::from_RationalNumber(transfer_mode.max_charge_power) == 22080);
            REQUIRE(transfer_mode.max_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.max_charge_power_L2) == 0.0f);
            REQUIRE(transfer_mode.max_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.max_charge_power_L3) == 0.0f);
            REQUIRE(message_20::datatypes::from_RationalNumber(transfer_mode.min_charge_power) == 20000);
            REQUIRE(transfer_mode.min_charge_power_L2.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.min_charge_power_L2) == 0.0f);
            REQUIRE(transfer_mode.min_charge_power_L3.has_value() == true);
            REQUIRE(message_20::datatypes::from_RationalNumber(*transfer_mode.min_charge_power_L3) == 0.0f);
        }
    }

    GIVEN("Serialize ac_charge_parameter_discovery_res") {

        using AC_ModeRes = message_20::datatypes::AC_CPDResEnergyTransferMode;

        message_20::AC_ChargeParameterDiscoveryResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456324};
        res.response_code = message_20::datatypes::ResponseCode::OK;
        auto& mode = res.transfer_mode.emplace<AC_ModeRes>();
        mode.max_charge_power = {2208, 1};
        mode.max_charge_power_L2 = {0, 0};
        mode.max_charge_power_L3 = {0, 0};
        mode.min_charge_power = {2000, 1};
        mode.min_charge_power_L2 = {0, 0};
        mode.min_charge_power_L3 = {0, 0};
        std::vector<uint8_t> expected = {0x80, 0x14, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d,
                                         0x8c, 0x4b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x04, 0x08, 0x50, 0x08,
                                         0x81, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x43, 0x40, 0x3c,
                                         0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    // TODO(sl): Adding BPT_AC_CPDResEnergyTransferMode tests
    // TODO(rb): Adding BPT_AC_CPDReqEnergyTransferMode tests
}
