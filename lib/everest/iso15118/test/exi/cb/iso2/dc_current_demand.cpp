#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/dc_current_demand.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

#include <vector>

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 current demand messages") {
    GIVEN("Deserialize current demand req - minimal") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xD1,
                             0x00, 0x28, 0x01, 0x06, 0x19, 0xA0, 0x24, 0x50, 0x84, 0x0E, 0x31, 0xB0};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::CurrentDemandReq);

            const auto& msg = variant.get<d2::msg::DC_CurrentDemandRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.ev_status.ev_ready == true);
            REQUIRE(msg.ev_status.ev_error_code == dt::DcEvErrorCode::NO_ERROR);
            REQUIRE(msg.ev_status.ev_ress_soc == 80);

            REQUIRE(dt::from_PhysicalValue(msg.ev_target_current) == 20.5);
            REQUIRE(msg.ev_maximum_voltage_limit == std::nullopt);
            REQUIRE(msg.ev_maximum_current_limit == std::nullopt);
            REQUIRE(msg.ev_maximum_power_limit == std::nullopt);
            REQUIRE(msg.bulk_charging_complete == std::nullopt);
            REQUIRE(msg.charging_complete == true);
            REQUIRE(msg.remaining_time_to_full_soc == std::nullopt);
            REQUIRE(msg.remaining_time_to_bulk_soc == std::nullopt);
            REQUIRE(dt::from_PhysicalValue(msg.ev_target_voltage) == 355.5);
        }
    }
    GIVEN("Deserialize current demand req - all fields present") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50,
                             0xD1, 0x00, 0x28, 0x01, 0x06, 0x19, 0xA0, 0x20, 0x18, 0x81, 0xD0, 0x0E,
                             0x01, 0x86, 0x19, 0x00, 0x20, 0x31, 0x42, 0x41, 0x38, 0x11, 0x04, 0x08,
                             0x3C, 0x0B, 0x80, 0x81, 0x05, 0x00, 0xF8, 0x10, 0x81, 0xC6, 0x36};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::CurrentDemandReq);

            const auto& msg = variant.get<d2::msg::DC_CurrentDemandRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.ev_status.ev_ready == true);
            REQUIRE(msg.ev_status.ev_error_code == dt::DcEvErrorCode::NO_ERROR);
            REQUIRE(msg.ev_status.ev_ress_soc == 80);

            REQUIRE(dt::from_PhysicalValue(msg.ev_target_current) == 20.5);
            REQUIRE(msg.ev_maximum_voltage_limit.has_value());
            REQUIRE(dt::from_PhysicalValue(msg.ev_maximum_voltage_limit.value()) == 1000);
            REQUIRE(msg.ev_maximum_current_limit.has_value());
            REQUIRE(dt::from_PhysicalValue(msg.ev_maximum_current_limit.value()) == 200);
            REQUIRE(msg.ev_maximum_power_limit.has_value());
            REQUIRE(dt::from_PhysicalValue(msg.ev_maximum_power_limit.value()) == 10000);
            REQUIRE(msg.bulk_charging_complete.has_value());
            REQUIRE(msg.bulk_charging_complete.value() == true);
            REQUIRE(msg.charging_complete.has_value());
            REQUIRE(msg.charging_complete.value() == true);
            REQUIRE(msg.remaining_time_to_bulk_soc.has_value());
            REQUIRE(dt::from_PhysicalValue(msg.remaining_time_to_full_soc.value()) == 60000);
            REQUIRE(msg.remaining_time_to_full_soc.has_value());
            REQUIRE(dt::from_PhysicalValue(msg.remaining_time_to_bulk_soc.value()) == 40000);
            REQUIRE(dt::from_PhysicalValue(msg.ev_target_voltage) == 355.5);
        }
    }
    GIVEN("Serialize current demand res - minimal") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::DC_CurrentDemandResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        auto status = dt::DcEvseStatus{};
        status.evse_isolation_status = dt::IsolationLevel::Valid;
        status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        res.evse_status = status;
        res.evse_present_voltage = dt::from_float(300.5, d2::msg::data_types::UnitSymbol::V);
        res.evse_present_current = dt::from_float(5.5, d2::msg::data_types::UnitSymbol::A);
        res.evse_current_limit_achieved = true;
        res.evse_voltage_limit_achieved = true;
        res.evse_power_limit_achieved = true;
        res.evse_id = "000000";
        res.sa_schedule_tuple_id = 3;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xE0,
                                         0x00, 0x00, 0x00, 0x20, 0x40, 0x84, 0x0B, 0xD1, 0x70, 0x00, 0xC3, 0xF0, 0xA8,
                                         0x22, 0x26, 0x08, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x48};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Serialize current demand res - all fields present") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::DC_CurrentDemandResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        auto status = dt::DcEvseStatus{};
        status.evse_isolation_status = dt::IsolationLevel::Valid;
        status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        res.evse_status = status;
        res.evse_present_voltage = dt::from_float(300.5, d2::msg::data_types::UnitSymbol::V);
        res.evse_present_current = dt::from_float(5.5, d2::msg::data_types::UnitSymbol::A);
        res.evse_current_limit_achieved = true;
        res.evse_voltage_limit_achieved = true;
        res.evse_power_limit_achieved = true;
        res.evse_id = "000000";
        res.sa_schedule_tuple_id = 3;
        res.evse_maximum_voltage_limit = dt::from_float(1000, d2::msg::data_types::UnitSymbol::V);
        res.evse_maximum_current_limit = dt::from_float(100, d2::msg::data_types::UnitSymbol::A);
        res.evse_maximum_power_limit = dt::from_float(100000, d2::msg::data_types::UnitSymbol::W);
        auto meterInfo = dt::MeterInfo{};
        meterInfo.meter_id = "FOO_METER";
        meterInfo.meter_reading = 999;
        meterInfo.sig_meter_reading = std::vector<uint8_t>{
            0xAA,
            0xBB,
        };
        meterInfo.meter_status = 444;
        meterInfo.t_meter = 100;
        res.meter_info = meterInfo;
        res.receipt_required = true;

        std::vector<uint8_t> expected = {
            0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xE0, 0x00, 0x00, 0x00, 0x20, 0x40,
            0x84, 0x0B, 0xD1, 0x70, 0x00, 0xC3, 0xF0, 0xA8, 0x22, 0x20, 0x31, 0x03, 0xA0, 0x1C, 0x04, 0x18, 0x74, 0x03,
            0x81, 0x45, 0x0E, 0x80, 0x70, 0x08, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x40, 0x0B, 0x46, 0x4F, 0x4F,
            0x5F, 0x4D, 0x45, 0x54, 0x45, 0x52, 0x07, 0x38, 0x38, 0x00, 0xAA, 0xAE, 0xC1, 0x78, 0x06, 0x06, 0x40, 0x40};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected); // FIXME: Cannot get this to work.
        }
    }
}
