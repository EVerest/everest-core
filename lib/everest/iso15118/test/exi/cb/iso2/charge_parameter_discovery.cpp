#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iso15118/message/d2/charge_parameter_discovery.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 charge parameter discovery messages") {
    // Tests of ChargeParameterDiscovery req/res were split into multiple cases, due to the complicated structure of the
    // messages.

    GIVEN("Deserialize charge parameter discovery req - AC minimal") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0x94, 0xC2,
                             0x61, 0x41, 0x90, 0x18, 0x40, 0x0A, 0x03, 0x0C, 0x19, 0x00, 0xC3, 0x00, 0x10, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<d2::msg::ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.requested_energy_transfer_mode == dt::EnergyTransferMode::DC_extended);
            REQUIRE(msg.ac_ev_charge_parameter.has_value());
            REQUIRE_FALSE(msg.dc_ev_charge_parameter.has_value());

            const auto& param = msg.ac_ev_charge_parameter.value();
            REQUIRE(dt::from_PhysicalValue(param.e_amount) == 100000);
            REQUIRE(dt::from_PhysicalValue(param.ev_max_voltage) == 10000);
            REQUIRE(dt::from_PhysicalValue(param.ev_max_current) == 100);
            REQUIRE(dt::from_PhysicalValue(param.ev_min_current) == 1);
        }
    }

    GIVEN("Deserialize charge parameter discovery req - AC all fields present") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0x90, 0x0A, 0x18,
                             0x0C, 0x81, 0x85, 0x06, 0x40, 0x61, 0x00, 0x28, 0x0C, 0x30, 0x64, 0x03, 0x0C, 0x00, 0x40};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<d2::msg::ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.requested_energy_transfer_mode == dt::EnergyTransferMode::DC_extended);

            REQUIRE(msg.max_entries_sa_schedule_tuple.has_value());
            REQUIRE(msg.max_entries_sa_schedule_tuple == 5);

            REQUIRE(msg.ac_ev_charge_parameter.has_value());
            REQUIRE_FALSE(msg.dc_ev_charge_parameter.has_value());

            const auto& param = msg.ac_ev_charge_parameter.value();
            REQUIRE(dt::from_PhysicalValue(param.e_amount) == 100000);
            REQUIRE(dt::from_PhysicalValue(param.ev_max_voltage) == 10000);
            REQUIRE(dt::from_PhysicalValue(param.ev_max_current) == 100);
            REQUIRE(dt::from_PhysicalValue(param.ev_min_current) == 1);
            REQUIRE(param.departure_time.has_value());
            REQUIRE(param.departure_time.value() == 100);
        }
    }

    GIVEN("Deserialize charge parameter discovery req - DC minimal") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0x94,
                             0xCA, 0x40, 0x06, 0x40, 0x61, 0x83, 0x20, 0x8C, 0x40, 0xF4, 0x03, 0x20};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<d2::msg::ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.requested_energy_transfer_mode == dt::EnergyTransferMode::DC_extended);

            REQUIRE(msg.dc_ev_charge_parameter.has_value());
            REQUIRE_FALSE(msg.ac_ev_charge_parameter.has_value());

            const auto& param = msg.dc_ev_charge_parameter.value();
            REQUIRE(param.dc_ev_status.ev_error_code == dt::DcEvErrorCode::NO_ERROR);
            REQUIRE(param.dc_ev_status.ev_ready == true);
            REQUIRE(param.dc_ev_status.ev_ress_soc == 50);
            REQUIRE(dt::from_PhysicalValue(param.ev_maximum_current_limit) == 100);
            REQUIRE(dt::from_PhysicalValue(param.ev_maximum_voltage_limit) == 500);
        }
    }

    GIVEN("Deserialize charge parameter discovery req - DC all fields present") {

        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0x90, 0x0A,
                             0x19, 0x19, 0x00, 0x21, 0x00, 0x19, 0x01, 0x86, 0x0C, 0x80, 0x61, 0x41, 0x90, 0x0C,
                             0x40, 0xF4, 0x03, 0x01, 0x86, 0x06, 0x40, 0x18, 0x60, 0x64, 0x06, 0x40, 0xA0, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ChargeParameterDiscoveryReq);

            const auto& msg = variant.get<d2::msg::ChargeParameterDiscoveryRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.requested_energy_transfer_mode == dt::EnergyTransferMode::DC_extended);

            REQUIRE(msg.max_entries_sa_schedule_tuple.has_value());
            REQUIRE(msg.max_entries_sa_schedule_tuple == 5);

            REQUIRE(msg.dc_ev_charge_parameter.has_value());
            REQUIRE_FALSE(msg.ac_ev_charge_parameter.has_value());

            const auto& param = msg.dc_ev_charge_parameter.value();
            REQUIRE(param.dc_ev_status.ev_error_code == dt::DcEvErrorCode::NO_ERROR);
            REQUIRE(param.dc_ev_status.ev_ready == true);
            REQUIRE(param.dc_ev_status.ev_ress_soc == 50);
            REQUIRE(dt::from_PhysicalValue(param.ev_maximum_current_limit) == 100);
            REQUIRE(dt::from_PhysicalValue(param.ev_maximum_voltage_limit) == 500);
            REQUIRE(param.departure_time.has_value());
            REQUIRE(param.departure_time.value() == 200);

            REQUIRE(param.ev_maximum_power_limit.has_value());
            REQUIRE(dt::from_PhysicalValue(param.ev_maximum_power_limit.value()) == 100000);
            REQUIRE(param.ev_energy_capacity.has_value());
            REQUIRE(dt::from_PhysicalValue(param.ev_energy_capacity.value()) == 100000);
            REQUIRE(param.ev_energy_request.has_value());
            REQUIRE(dt::from_PhysicalValue(param.ev_energy_request.value()) == 100000);
            REQUIRE(param.full_soc.has_value());
            REQUIRE(param.full_soc.value() == 100);
            REQUIRE(param.bulk_soc.has_value());
            REQUIRE(param.bulk_soc.value() == 80);
        }
    }

    GIVEN("Serialize charge parameter discovery res - AC minimal") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ChargeParameterDiscoveryResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        res.evse_processing = d2::msg::data_types::EvseProcessing::Finished;
        auto param = dt::AcEvseChargeParameter{};
        param.ac_evse_status.evse_notification = dt::EvseNotification::None;
        param.ac_evse_status.notification_max_delay = 10;
        param.ac_evse_status.rcd = true;
        param.evse_nominal_voltage = dt::PhysicalValue{400, 0, dt::UnitSymbol::V};
        param.evse_max_current = dt::PhysicalValue{100, 0, dt::UnitSymbol::A};
        res.ac_evse_charge_parameter = param;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xA0,
                                         0x00, 0x20, 0x14, 0x01, 0x03, 0x10, 0x24, 0x00, 0xC0, 0xC3, 0x06, 0x40, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize charge parameter discovery res - DC minimal") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ChargeParameterDiscoveryResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        res.evse_processing = d2::msg::data_types::EvseProcessing::Finished;
        auto param = dt::DcEvseChargeParameter{};
        param.dc_evse_status.evse_notification = dt::EvseNotification::None;
        param.dc_evse_status.notification_max_delay = 10;
        param.dc_evse_status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        param.evse_maximum_current_limit = dt::PhysicalValue{100, 0, dt::UnitSymbol::A};
        param.evse_maximum_power_limit = dt::PhysicalValue{100, 3, dt::UnitSymbol::W};
        param.evse_maximum_voltage_limit = dt::PhysicalValue{1000, 0, dt::UnitSymbol::V};
        param.evse_minimum_current_limit = dt::PhysicalValue{1, 0, dt::UnitSymbol::A};
        param.evse_minimum_voltage_limit = dt::PhysicalValue{150, 0, dt::UnitSymbol::V};
        param.evse_peak_current_ripple = dt::PhysicalValue{1, 0, dt::UnitSymbol::A};
        res.dc_evse_charge_parameter = param;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13,
                                         0x4B, 0x50, 0xA0, 0x00, 0x30, 0x14, 0x02, 0x10, 0x30, 0xC1,
                                         0x90, 0x18, 0x50, 0x64, 0x03, 0x10, 0x3A, 0x01, 0xC0, 0xC3,
                                         0x00, 0x10, 0x31, 0x02, 0x58, 0x04, 0x46, 0x18, 0x00, 0x88};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize charge parameter discovery res - DC all fields present") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ChargeParameterDiscoveryResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        res.evse_processing = d2::msg::data_types::EvseProcessing::Finished;
        auto param = dt::DcEvseChargeParameter{};
        param.dc_evse_status.evse_notification = dt::EvseNotification::None;
        param.dc_evse_status.notification_max_delay = 10;
        param.dc_evse_status.evse_status_code = dt::DcEvseStatusCode::EVSE_Ready;
        param.dc_evse_status.evse_isolation_status = dt::IsolationLevel::Valid;
        param.evse_maximum_current_limit = dt::PhysicalValue{100, 0, dt::UnitSymbol::A};
        param.evse_maximum_power_limit = dt::PhysicalValue{100, 3, dt::UnitSymbol::W};
        param.evse_maximum_voltage_limit = dt::PhysicalValue{1000, 0, dt::UnitSymbol::V};
        param.evse_minimum_current_limit = dt::PhysicalValue{1, 0, dt::UnitSymbol::A};
        param.evse_minimum_voltage_limit = dt::PhysicalValue{150, 0, dt::UnitSymbol::V};
        param.evse_peak_current_ripple = dt::PhysicalValue{1, 0, dt::UnitSymbol::A};
        param.evse_current_regulation_tolerance = dt::PhysicalValue{5, -1, dt::UnitSymbol::A};
        param.evse_energy_to_be_delivered = dt::PhysicalValue{100, 3, dt::UnitSymbol::Wh};
        res.dc_evse_charge_parameter = param;

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50,
                                         0xA0, 0x00, 0x30, 0x14, 0x00, 0x20, 0x40, 0xC3, 0x06, 0x40, 0x61, 0x41,
                                         0x90, 0x0C, 0x40, 0xE8, 0x07, 0x03, 0x0C, 0x00, 0x40, 0xC4, 0x09, 0x60,
                                         0x10, 0x10, 0x60, 0x0A, 0x06, 0x18, 0x00, 0x81, 0x86, 0x06, 0x40, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }

    GIVEN("Serialize charge parameter discovery res - AC minimal with SA schedules") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ChargeParameterDiscoveryResponse{};
        res.header = header;
        res.response_code = d2::msg::data_types::ResponseCode::OK;
        res.evse_processing = d2::msg::data_types::EvseProcessing::Finished;
        auto param = dt::AcEvseChargeParameter{};
        param.ac_evse_status.evse_notification = dt::EvseNotification::None;
        param.ac_evse_status.notification_max_delay = 10;
        param.ac_evse_status.rcd = true;
        param.evse_nominal_voltage = dt::PhysicalValue{400, 0, dt::UnitSymbol::V};
        param.evse_max_current = dt::PhysicalValue{100, 0, dt::UnitSymbol::A};
        res.ac_evse_charge_parameter = param;

        auto tuple = dt::SaScheduleTuple{};
        tuple.sa_schedule_tuple_id = 99;

        auto pmax = dt::PMaxScheduleEntry{};
        pmax.time_interval.start = 0;
        pmax.time_interval.duration = 3600;
        pmax.p_max = dt::PhysicalValue{100, 3, dt::UnitSymbol::W};
        tuple.pmax_schedule = {pmax};

        auto tariff = dt::SalesTariff{};
        tariff.id = "Foo";
        tariff.sales_tariff_id = 1;
        tariff.sales_tariff_description = "Bar";
        tariff.num_e_price_levels = 1;
        auto tariffEntry = dt::SalesTariffEntry{};
        tariffEntry.time_interval.start = 0;
        tariffEntry.time_interval.duration = 3600;
        tariffEntry.e_price_level = 1;
        auto consumptionCost = dt::ConsumptionCost{};
        consumptionCost.start_value = dt::PhysicalValue{0, 0, dt::UnitSymbol::W};
        consumptionCost.cost.push_back(dt::Cost{50, 3, dt::CostKind::RelativePricePercentage});
        tariffEntry.consumption_cost = {consumptionCost};
        tariff.sales_tariff_entry = {tariffEntry};
        tuple.sales_tariff = tariff;

        res.sa_schedule_list = {tuple};

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x50, 0xA0,
                                         0x00, 0x00, 0xC4, 0x00, 0x00, 0x24, 0x07, 0x01, 0x85, 0x06, 0x40, 0x80, 0x2A,
                                         0x33, 0x7B, 0x78, 0x00, 0x00, 0xA8, 0x4C, 0x2E, 0x40, 0x02, 0x00, 0x00, 0x48,
                                         0x0E, 0x00, 0x04, 0x06, 0x28, 0x00, 0x00, 0x06, 0x41, 0x85, 0x48, 0x02, 0x80,
                                         0x20, 0x62, 0x04, 0x80, 0x18, 0x18, 0x60, 0xC8, 0x00};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected);
        }
    }
}
