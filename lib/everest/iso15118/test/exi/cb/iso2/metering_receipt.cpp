#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/metering_receipt.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 metering receipt messages") {
    GIVEN("Deserialize metering receipt req - minmal") {

        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xF4, 0x10, 0x05,
                             0xB6, 0x44, 0x0E, 0x76, 0x10, 0x9A, 0x5A, 0x40, 0xB4, 0x64, 0xF4, 0xF5, 0xF4, 0xD4, 0x55,
                             0x44, 0x55, 0x20, 0x73, 0x83, 0x80, 0xF5, 0x85, 0x85, 0x21, 0x48, 0xC1, 0x8D, 0x21, 0x34,
                             0xD9, 0x31, 0xE4, 0xE4, 0xCD, 0x90, 0xCD, 0x8D, 0xD5, 0x95, 0x5C, 0xE4, 0xC5, 0x91, 0x21,
                             0x59, 0xA5, 0x69, 0x4C, 0xD5, 0xA9, 0x88, 0xC8, 0xC1, 0xD9, 0x90, 0xC9, 0x18, 0xC1, 0x64,
                             0xC9, 0x9C, 0xBD, 0x91, 0xA8, 0xC5, 0xB1, 0x8D, 0xB9, 0x29, 0x19, 0x89, 0xB9, 0x19, 0x29,
                             0x59, 0x5D, 0xC1, 0x4D, 0x5D, 0x45, 0xBC, 0xF4, 0x17, 0x80, 0x60, 0x64, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::MeteringReceiptReq);

            const auto& msg = variant.get<d2::msg::MeteringReceiptRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});
            REQUIRE(msg.meter_info.meter_id == "FOO_METER");
            REQUIRE(msg.meter_info.meter_reading.has_value());
            REQUIRE(msg.meter_info.meter_reading == 999);
            REQUIRE(msg.meter_info.meter_status.has_value());
            REQUIRE(msg.meter_info.meter_status == 444);
            REQUIRE(msg.meter_info.sig_meter_reading.has_value());
            REQUIRE(msg.meter_info.sig_meter_reading.value() ==
                    dt::SigMeterReading{"aaHR0cHM6Ly93d3cueW91dHViZS5jb20vd2F0Y2g/dj1lcnJFbnFJVWpSWQo="});
            REQUIRE(msg.meter_info.t_meter.has_value());
            REQUIRE(msg.meter_info.t_meter == 100);
        }
    }
    GIVEN("Deserialize metering receipt req - all fields present") {

        uint8_t doc_raw[] = {
            0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xF0, 0x11, 0xA5, 0x90, 0x08, 0x02,
            0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D, 0x00, 0x00, 0x0B, 0x46, 0x4F, 0x4F, 0x5F, 0x4D, 0x45, 0x54, 0x45,
            0x52, 0x07, 0x38, 0x38, 0x0F, 0x58, 0x58, 0x52, 0x14, 0x8C, 0x18, 0xD2, 0x13, 0x4D, 0x93, 0x1E, 0x4E, 0x4C,
            0xD9, 0x0C, 0xD8, 0xDD, 0x59, 0x55, 0xCE, 0x4C, 0x59, 0x12, 0x15, 0x9A, 0x56, 0x94, 0xCD, 0x5A, 0x98, 0x8C,
            0x8C, 0x1D, 0x99, 0x0C, 0x91, 0x8C, 0x16, 0x4C, 0x99, 0xCB, 0xD9, 0x1A, 0x8C, 0x5B, 0x18, 0xDB, 0x92, 0x91,
            0x98, 0x9B, 0x91, 0x92, 0x95, 0x95, 0xDC, 0x14, 0xD5, 0xD4, 0x5B, 0xCF, 0x41, 0x78, 0x06, 0x06, 0x40, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::MeteringReceiptReq);

            const auto& msg = variant.get<d2::msg::MeteringReceiptRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.id.has_value());
            REQUIRE(msg.id == "id");
            REQUIRE(msg.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});
            REQUIRE(msg.sa_schedule_tuple_id.has_value());
            REQUIRE(msg.sa_schedule_tuple_id == 1);
            REQUIRE(msg.meter_info.meter_id == "FOO_METER");
            REQUIRE(msg.meter_info.meter_reading.has_value());
            REQUIRE(msg.meter_info.meter_reading == 999);
            REQUIRE(msg.meter_info.meter_status.has_value());
            REQUIRE(msg.meter_info.meter_status == 444);
            REQUIRE(msg.meter_info.sig_meter_reading.has_value());
            REQUIRE(msg.meter_info.sig_meter_reading.value() ==
                    dt::SigMeterReading{"aaHR0cHM6Ly93d3cueW91dHViZS5jb20vd2F0Y2g/dj1lcnJFbnFJVWpSWQo="});
            REQUIRE(msg.meter_info.t_meter.has_value());
            REQUIRE(msg.meter_info.t_meter == 100);
        }
    }
    GIVEN("Serialize metering receipt res - AC") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::MeteringReceiptResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        auto status = dt::AcEvseStatus{};
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 100;
        status.rcd = true;
        res.ac_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2,
                                         0x13, 0x4B, 0x51, 0x00, 0x00, 0x64, 0x00, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
    GIVEN("Serialize metering receipt res - DC") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::MeteringReceiptResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        auto status = dt::DcEvseStatus{};
        status.evse_notification = dt::EvseNotification::None;
        status.notification_max_delay = 100;
        status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        res.dc_evse_status = status;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2,
                                         0x13, 0x4B, 0x51, 0x00, 0x04, 0x64, 0x01, 0x08, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
