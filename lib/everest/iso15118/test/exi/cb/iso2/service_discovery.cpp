#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/d2/service_discovery.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

#include <cstdint>
#include <optional>

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 service discovery messages") {
    GIVEN("Deserialize service discovery req") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0xB0,
                             0x18, 0xA8, 0xCA, 0xE6, 0xE8, 0x40, 0xE6, 0xC6, 0xDE, 0xE0, 0xCA, 0x00, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ServiceDiscoveryReq);

            const auto& msg = variant.get<d2::msg::ServiceDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.service_scope.value() == "Test scope");
            REQUIRE(msg.service_category.value() == dt::ServiceCategory::EvCharging);
        }
    }
    GIVEN("Serialize service discovery res - minimal") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ServiceDiscoveryResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        res.payment_option_list = {
            dt::PaymentOption::ExternalPayment,
        };
        auto service = dt::ChargeService{};
        service.service_id = 1;
        service.supported_energy_transfer_mode = {dt::EnergyTransferMode::DC_extended};
        service.service_category = dt::ServiceCategory::EvCharging;
        service.FreeService = true;
        res.charge_service = service;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13,
                                         0x4B, 0x51, 0xC0, 0x01, 0x20, 0x04, 0x82, 0x83, 0x24};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Serialize service discovery res - all fields present") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ServiceDiscoveryResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        res.payment_option_list = {
            dt::PaymentOption::ExternalPayment,
        };
        auto chargeService = dt::ChargeService{};
        chargeService.service_id = 1;
        chargeService.service_name = "Service Foo";
        chargeService.supported_energy_transfer_mode = {dt::EnergyTransferMode::DC_extended};
        chargeService.service_scope = "Test scope";
        chargeService.service_category = dt::ServiceCategory::EvCharging;
        chargeService.FreeService = true;
        res.charge_service = chargeService;
        auto service = dt::Service{};
        service.service_id = 99;
        service.service_name = "Service Bar";
        service.service_scope = "Test scope 2";
        service.service_category = dt::ServiceCategory::Internet;
        service.FreeService = true;
        res.service_list = {service};

        std::vector<uint8_t> expected = {
            0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0xC0, 0x01, 0x20, 0x04, 0x03, 0x54,
            0xD9, 0x5C, 0x9D, 0x9A, 0x58, 0xD9, 0x48, 0x11, 0x9B, 0xDB, 0xC0, 0x01, 0x8A, 0x8C, 0xAE, 0x6E, 0x84, 0x0E,
            0x6C, 0x6D, 0xEE, 0x0C, 0xA2, 0x0C, 0x80, 0xC6, 0x01, 0xAA, 0x6C, 0xAE, 0x4E, 0xCD, 0x2C, 0x6C, 0xA4, 0x08,
            0x4C, 0x2E, 0x41, 0x00, 0xE5, 0x46, 0x57, 0x37, 0x42, 0x07, 0x36, 0x36, 0xF7, 0x06, 0x52, 0x03, 0x21, 0x10};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
