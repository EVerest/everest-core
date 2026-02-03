// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <optional>
#include <vector>

namespace iso15118::d2::msg {

namespace data_types {
enum class EnergyTransferMode {
    AcSinglePhaseCore,
    AcThreePhaseCore,
    DcCore,
    DcExtended,
    DcComboCore,
    DcUnique
};

enum class CostKind {
    RelativePricePercentage,
    RenewableGenerationPercentage,
    CarbonDioxideEmission
};

struct EvChargeParameter {
    std::optional<uint32_t> departure_time; // Seconds to departure
};

struct AcEvChargeParameter : EvChargeParameter {
    PhysicalValue e_amount; // Wh
    PhysicalValue ev_max_voltage;
    PhysicalValue ev_max_current;
    PhysicalValue ev_min_current;
};

struct DcEvChargeParameter : EvChargeParameter {
    DcEvStatus dc_ev_status;
    PhysicalValue ev_maximum_current_limit;
    std::optional<PhysicalValue> ev_maximum_power_limit;
    PhysicalValue ev_maximum_voltage_limit;
    std::optional<PhysicalValue> ev_energy_capacity; // Wh
    std::optional<PhysicalValue> ev_energy_request;  // Wh
    std::optional<PercentValue> full_soc;
    std::optional<PercentValue> bulk_soc;
};

struct RelativeTimeInterval {
    uint32_t start;                   // Start in s from now
    std::optional<uint32_t> duration; // Duration in s
};

struct Entry {
    RelativeTimeInterval time_interval;
};

struct Cost {
    uint32_t amount;                         // cost per kWh
    std::optional<int8_t> amount_multiplier; // [-3 - 3]
    CostKind cost_kind;
};

struct ConsumptionCost {
    PhysicalValue start_value; // W
    std::vector<Cost> cost;    // [1 - 3]
};

struct SalesTariffEntry : Entry {
    std::optional<uint8_t> e_price_level;
    std::vector<ConsumptionCost> consumption_cost; // [1 - 3]
};
constexpr auto SalesTariffEntryConsumptionCostMaxLength = 3;

struct SalesTariff {
    std::optional<std::string> id;
    SAScheduleTupleID sales_tariff_id;
    std::optional<std::string> sales_tariff_description; // MaxLength: 32
    std::optional<uint8_t> num_e_price_levels;
    std::vector<SalesTariffEntry> sales_tariff_entry; // [1 - 1024]
};
constexpr auto SalesTariffEntryMaxLength = 1024;

struct PMaxScheduleEntry : Entry {
    PhysicalValue p_max; // W
};
using PMaxSchedule = std::vector<PMaxScheduleEntry>; // [1 - 1024]
constexpr auto PMaxScheduleMaxLength = 1024;

struct SaScheduleTuple {
    SAScheduleTupleID sa_schedule_tuple_id;
    PMaxSchedule pmax_schedule;
    std::optional<SalesTariff> sales_tariff;
};
using SaSchedules = std::vector<SaScheduleTuple>; // [1 - 3]

struct AcEvseChargeParameter {
    AcEvseStatus ac_evse_status;
    PhysicalValue evse_nominal_voltage;
    PhysicalValue evse_max_current;
};

struct DcEvseChargeParameter {
    DcEvseStatus dc_evse_status;
    PhysicalValue evse_maximum_current_limit;
    PhysicalValue evse_maximum_power_limit;
    PhysicalValue evse_maximum_voltage_limit;
    PhysicalValue evse_minimum_current_limit;
    PhysicalValue evse_minimum_voltage_limit;
    std::optional<PhysicalValue> evse_current_regulation_tolerance;
    PhysicalValue evse_peak_current_ripple;
    std::optional<PhysicalValue> evse_energy_to_be_delivered;
};

} // namespace data_types

struct ChargeParameterDiscoveryRequest {
    Header header;
    std::optional<uint16_t> max_entries_sa_schedule_tuple;
    data_types::EnergyTransferMode requested_energy_transfer_mode;
    std::optional<data_types::AcEvChargeParameter> ac_ev_charge_parameter;
    std::optional<data_types::DcEvChargeParameter> dc_ev_charge_parameter;
};

struct ChargeParameterDiscoveryResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::EvseProcessing evse_processing;
    std::optional<data_types::SaSchedules> sa_schedule_list; // The SECC shall only omit the parameter 'SAScheduleList'
                                                             // in case EVSEProcessing is set to 'Ongoing'.
    std::optional<data_types::AcEvseChargeParameter> ac_evse_charge_parameter;
    std::optional<data_types::DcEvseChargeParameter> dc_evse_charge_parameter;
};

} // namespace iso15118::d2::msg
