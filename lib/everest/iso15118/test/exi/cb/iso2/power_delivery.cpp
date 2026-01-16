#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/power_delivery.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

#include <vector>

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 power delivery messages") {
    GIVEN("Deserialize power delivery req - minimal") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE,
                             0xC2, 0x13, 0x4B, 0x51, 0x50, 0x00, 0x06, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::PowerDeliveryReq);

            const auto& msg = variant.get<d2::msg::PowerDeliveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.charge_progress == dt::ChargeProgress::Start);
            REQUIRE(msg.sa_schedule_tuple_id == 1);
        }
    }
    GIVEN("Deserialize power delivery req - all fields present") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0x50, 0x00,
                             0x00, 0x24, 0x07, 0x03, 0x0A, 0x01, 0x40, 0x84, 0x10, 0x01, 0x90, 0x22, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::PowerDeliveryReq);

            const auto& msg = variant.get<d2::msg::PowerDeliveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.charge_progress == dt::ChargeProgress::Start);
            REQUIRE(msg.sa_schedule_tuple_id == 1);
            REQUIRE(msg.dc_ev_power_delivery_parameter->bulk_charging_complete.has_value());
            REQUIRE(msg.dc_ev_power_delivery_parameter->bulk_charging_complete == true);
            REQUIRE(msg.dc_ev_power_delivery_parameter->charging_complete == true);
            REQUIRE(msg.dc_ev_power_delivery_parameter->dc_ev_status.ev_ready == true);
            REQUIRE(msg.dc_ev_power_delivery_parameter->dc_ev_status.ev_error_code == dt::DcEvErrorCode::NO_ERROR);
            REQUIRE(msg.dc_ev_power_delivery_parameter->dc_ev_status.ev_ress_soc == 50);
        }
    }
    GIVEN("Serialize power delivery res - AC") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::PowerDeliveryResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        auto status = dt::AcEvseStatus{};
        status.rcd = true;
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 10;
        res.ac_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2,
                                         0x13, 0x4B, 0x51, 0x60, 0x00, 0x0A, 0x00, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Serialize power delivery res - DC") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::PowerDeliveryResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        auto status = dt::DcEvseStatus{};
        status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 10;
        res.dc_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2,
                                         0x13, 0x4B, 0x51, 0x60, 0x04, 0x0A, 0x01, 0x08, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
