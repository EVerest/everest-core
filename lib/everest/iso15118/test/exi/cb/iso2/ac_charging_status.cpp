#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/ac_charging_status.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 charging status messages") {
    GIVEN("Deserialize charging status req") {

        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xB0};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ChargingStatusReq);

            const auto& msg = variant.get<d2::msg::AC_ChargingStatusRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});
        }
    }
    GIVEN("Serialize charging status res - minimal") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::AC_ChargingStatusResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        res.evse_id = "000000";
        res.sa_schedule_tuple_id = 1;
        auto status = dt::AcEvseStatus{};
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 100;
        status.rcd = true;
        res.ac_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xC0,
                                         0x00, 0x20, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x18, 0xC8, 0x01, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Serialize charging status res - all fields present") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::AC_ChargingStatusResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        res.evse_id = "000000";
        res.sa_schedule_tuple_id = 1;
        res.evse_max_current = dt::PhysicalValue{100, 0, dt::UnitSymbol::A};
        auto meterInfo = dt::MeterInfo{};
        meterInfo.meter_id = "FOO_METER";
        meterInfo.sig_meter_reading = {0xAA, 0xBB};
        meterInfo.meter_reading = 999;
        meterInfo.meter_status = 444;
        meterInfo.t_meter = 100;
        res.meter_info = meterInfo;
        res.receipt_required = true;
        auto status = dt::AcEvseStatus{};
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 100;
        status.rcd = true;
        res.ac_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xC0,
                                         0x00, 0x20, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0xC3, 0x06, 0x40,
                                         0x02, 0xD1, 0x93, 0xD3, 0xD7, 0xD3, 0x51, 0x55, 0x11, 0x54, 0x81, 0xCE, 0x0E,
                                         0x00, 0x2A, 0xAB, 0xB0, 0x5E, 0x01, 0x81, 0x90, 0x10, 0x64, 0x00, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
