#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/dc_charge_parameter_discovery.hpp>
#include <iso15118/message/variant.hpp>

#include "helper.hpp"

using namespace iso15118;

SCENARIO("Se/Deserialize dc charge parameter discovery messages") {

    GIVEN("Deserialize dc_charge_parameter_discovery_req") {

        uint8_t doc_raw[] = {0x80, 0x3c, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d, 0x8c, 0x3b, 0xfe,
                             0x1b, 0x60, 0x62, 0x88, 0x10, 0x98, 0x75, 0x04, 0x00, 0x32, 0x02, 0x00, 0x2b, 0x00,
                             0x81, 0x00, 0x01, 0x40, 0x80, 0x08, 0x40, 0x70, 0x40, 0x00, 0x50, 0x80};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        message_20::Variant variant(io::v2gtp::PayloadType::Part20DC, stream_view);

        THEN("It should be deserialized succussfully") {
            REQUIRE(variant.get_type() == message_20::Type::DC_ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<message_20::DC_ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B});
            REQUIRE(header.timestamp == 1725456323);

            using DC_ModeReq = message_20::DC_ChargeParameterDiscoveryRequest::DC_CPDReqEnergyTransferMode;

            REQUIRE(std::holds_alternative<DC_ModeReq>(msg.transfer_mode));
            const auto& transfer_mode = std::get<DC_ModeReq>(msg.transfer_mode);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.max_charge_current) == 300);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.max_charge_power) == 150000);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.max_voltage) == 900);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.min_charge_current) == 10);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.min_charge_power) == 100);
            REQUIRE(message_20::from_RationalNumber(transfer_mode.min_voltage) == 10);
        }
    }

    // TODO(sl): Adding BPT_DC_CPDReqEnergyTransferMode tests

    GIVEN("Serialize dc_charge_parameter_discovery_res") {

        using DC_ModeRes = message_20::DC_ChargeParameterDiscoveryResponse::DC_CPDResEnergyTransferMode;

        message_20::DC_ChargeParameterDiscoveryResponse res;

        res.header = message_20::Header{{0x3D, 0x4C, 0xBF, 0x93, 0x37, 0x4E, 0xD8, 0x9B}, 1725456324};
        res.response_code = message_20::ResponseCode::OK;
        auto& mode = res.transfer_mode.emplace<DC_ModeRes>();
        mode.max_charge_current = {2000, -1};
        mode.max_charge_power = {2208, 1};
        mode.max_voltage = {9000, -1};
        mode.min_charge_current = {1000, -3};
        mode.min_charge_power = {2000, -1};
        mode.min_voltage = {2000, -1};

        std::vector<uint8_t> expected = {0x80, 0x40, 0x04, 0x1e, 0xa6, 0x5f, 0xc9, 0x9b, 0xa7, 0x6c, 0x4d,
                                         0x8c, 0x4b, 0xfe, 0x1b, 0x60, 0x62, 0x00, 0x44, 0x08, 0x50, 0x08,
                                         0x81, 0xfc, 0x34, 0x03, 0xc0, 0xfe, 0x1a, 0x01, 0xe0, 0x7d, 0x0e,
                                         0x80, 0x70, 0x3f, 0x85, 0x42, 0x30, 0x1f, 0xc3, 0x40, 0x3c, 0x40};

        THEN("It should be serialized succussfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    // TODO(sl): Adding BPT_DC_CPDResEnergyTransferMode tests
}
