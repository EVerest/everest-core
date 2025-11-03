// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/ac_charge_parameter_discovery.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

using AC_ModeReq = dt::AC_CPDReqEnergyTransferMode;
using BPT_AC_ModeReq = dt::BPT_AC_CPDReqEnergyTransferMode;

using AC_ModeRes = dt::AC_CPDResEnergyTransferMode;
using BPT_AC_ModeRes = dt::BPT_AC_CPDResEnergyTransferMode;

SCENARIO("AC charge parameter discovery state handling") {
    GIVEN("Bad Case - Unknown session") {

        auto session = d20::Session();

        const auto evse_id = std::string("everest se");
        const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC};
        const auto cert_install{false};
        const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
        const std::vector<uint16_t> vas_services{};
        const d20::DcTransferLimits dc_limits;
        const d20::AcTransferLimits ac_limits;
        const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
            {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

        const d20::EvseSetupConfig evse_setup{
            evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
            ac_limits, control_mobility_modes};

        message_20::AC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<AC_ModeReq>();
        req_out.max_charge_power = {11, 3};
        req_out.min_charge_power = {23, 2};

        const auto res = d20::state::handle_request(req, d20::Session(), d20::SessionConfig(evse_setup).ac_limits,
                                                    iso15118::d20::AcPresentPower{});

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);

            REQUIRE(std::holds_alternative<AC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeRes>(res.transfer_mode);
            REQUIRE(dt::from_RationalNumber(transfer_mode.max_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.nominal_frequency) == 0.0f);
        }
    }

    GIVEN("Bad Case: e.g. ac transfer mod instead of ac_bpt transfer mod - FAILED_WrongChargeParameter") {

        const auto service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        const auto evse_id = std::string("everest se");
        const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC};
        const auto cert_install{false};
        const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
        const std::vector<uint16_t> vas_services{};
        const d20::DcTransferLimits dc_limits;
        const d20::AcTransferLimits ac_limits;
        const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
            {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

        const d20::EvseSetupConfig evse_setup{
            evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
            ac_limits, control_mobility_modes};

        auto session = d20::Session(service_parameters);

        message_20::AC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<AC_ModeReq>();
        req_out.max_charge_power = {11, 3};
        req_out.min_charge_power = {23, 2};

        const auto res = d20::state::handle_request(req, session, d20::SessionConfig(evse_setup).ac_limits,
                                                    iso15118::d20::AcPresentPower{});

        THEN("ResponseCode: FAILED_WrongChargeParameter, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_WrongChargeParameter);

            REQUIRE(std::holds_alternative<AC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeRes>(res.transfer_mode);
            REQUIRE(dt::from_RationalNumber(transfer_mode.max_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.nominal_frequency) == 0.0f);
        }
    }

    GIVEN("Bad Case: e.g. AC_BPT transfer mod instead of ac transfer mod - FAILED_WrongChargeParameter") {

        const auto service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        const auto evse_id = std::string("everest se");
        const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC};
        const auto cert_install{false};
        const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
        const std::vector<uint16_t> vas_services{};
        const d20::DcTransferLimits dc_limits;
        const d20::AcTransferLimits ac_limits;
        const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
            {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

        const d20::EvseSetupConfig evse_setup{
            evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
            ac_limits, control_mobility_modes};

        auto session = d20::Session(service_parameters);

        message_20::AC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<BPT_AC_ModeReq>();
        req_out.max_charge_power = {11, 3};
        req_out.min_charge_power = {23, 2};
        req_out.max_discharge_power = {11, 3};
        req_out.min_discharge_power = {23, 2};

        const auto res = d20::state::handle_request(req, session, d20::SessionConfig(evse_setup).ac_limits,
                                                    iso15118::d20::AcPresentPower{});

        THEN("ResponseCode: FAILED_WrongChargeParameter, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_WrongChargeParameter);

            REQUIRE(std::holds_alternative<AC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeRes>(res.transfer_mode);
            REQUIRE(dt::from_RationalNumber(transfer_mode.max_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_charge_power) == 0.0f);
            REQUIRE(dt::from_RationalNumber(transfer_mode.nominal_frequency) == 0.0f);
        }
    }

    GIVEN("Good Case: AC") {

        const auto service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        auto session = d20::Session(service_parameters);

        const auto evse_id = std::string("everest se");
        const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC};
        const auto cert_install{false};
        const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
        const std::vector<uint16_t> vas_services{};
        const d20::DcTransferLimits dc_limits;
        const d20::AcTransferLimits ac_limits;
        const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
            {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

        const d20::EvseSetupConfig evse_setup{
            evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
            ac_limits, control_mobility_modes};

        d20::SessionConfig config = d20::SessionConfig(evse_setup);

        float max_charge_power = 11000.0f;
        float min_charge_power = 11000.0f;
        float nominal_frequency = 50.0f;
        float power_ramp_limitation = 2.0f;

        config.ac_limits.charge_power.max = dt::from_float(max_charge_power);
        config.ac_limits.charge_power.min = dt::from_float(min_charge_power);
        config.ac_limits.nominal_frequency = dt::from_float(nominal_frequency);
        config.ac_limits.power_ramp_limitation = dt::from_float(power_ramp_limitation);

        message_20::AC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<AC_ModeReq>();
        req_out.max_charge_power = {11, 3};
        req_out.min_charge_power = {23, 2};

        const auto res = d20::state::handle_request(req, session, config.ac_limits, iso15118::d20::AcPresentPower{});

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            REQUIRE(std::holds_alternative<AC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<AC_ModeRes>(res.transfer_mode);

            REQUIRE(dt::from_RationalNumber(transfer_mode.max_charge_power) == max_charge_power);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_charge_power) == min_charge_power);
            REQUIRE(dt::from_RationalNumber(transfer_mode.nominal_frequency) == nominal_frequency);
            REQUIRE(transfer_mode.power_ramp_limitation.has_value() == true);
            REQUIRE(dt::from_RationalNumber(transfer_mode.power_ramp_limitation.value()) == power_ramp_limitation);
        }
    }

    GIVEN("Good Case: AC_BPT") {

        const auto service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        auto session = d20::Session(service_parameters);

        const auto evse_id = std::string("everest se");
        const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC};
        const auto cert_install{false};
        const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
        const std::vector<uint16_t> vas_services{};
        const d20::DcTransferLimits dc_limits;
        const d20::AcTransferLimits ac_limits;
        const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
            {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

        const d20::EvseSetupConfig evse_setup{
            evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
            ac_limits, control_mobility_modes};

        auto config = d20::SessionConfig(evse_setup);

        float max_charge_power = 11000.0f;
        float min_charge_power = 11000.0f;
        float nominal_frequency = 50.0f;
        float power_ramp_limitation = 2.0f;
        float max_discharge_power = 6000.0f;
        float min_discharge_power = 3000.0f;

        config.ac_limits.charge_power.max = dt::from_float(max_charge_power);
        config.ac_limits.charge_power.min = dt::from_float(min_charge_power);
        config.ac_limits.nominal_frequency = dt::from_float(nominal_frequency);
        config.ac_limits.power_ramp_limitation = dt::from_float(power_ramp_limitation);

        d20::Limit<dt::RationalNumber> discharge_power;
        discharge_power.max = dt::from_float(max_discharge_power);
        discharge_power.min = dt::from_float(min_discharge_power);
        config.ac_limits.discharge_power = discharge_power;

        message_20::AC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<BPT_AC_ModeReq>();
        req_out.max_charge_power = {11, 3};
        req_out.min_charge_power = {23, 2};
        req_out.max_discharge_power = {11, 3};
        req_out.min_discharge_power = {23, 2};

        const auto res = d20::state::handle_request(req, session, config.ac_limits, iso15118::d20::AcPresentPower{});

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            REQUIRE(std::holds_alternative<BPT_AC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<BPT_AC_ModeRes>(res.transfer_mode);

            REQUIRE(dt::from_RationalNumber(transfer_mode.max_charge_power) == max_charge_power);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_charge_power) == min_charge_power);
            REQUIRE(dt::from_RationalNumber(transfer_mode.nominal_frequency) == nominal_frequency);
            REQUIRE(transfer_mode.power_ramp_limitation.has_value() == true);
            REQUIRE(dt::from_RationalNumber(transfer_mode.power_ramp_limitation.value()) == power_ramp_limitation);
            REQUIRE(dt::from_RationalNumber(transfer_mode.max_discharge_power) == max_discharge_power);
            REQUIRE(dt::from_RationalNumber(transfer_mode.min_discharge_power) == min_discharge_power);
        }
    }

    GIVEN("Bad Case: EV Parameter does not fit in evse parameters - FAILED_WrongChargeParameter") {
    }

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // todo(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // todo(sl): not here
}
