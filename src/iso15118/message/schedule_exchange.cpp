// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/schedule_exchange.hpp>

#include <type_traits>

#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Decoder.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <>
void convert(const struct iso20_EVPowerScheduleEntryType& in, ScheduleExchangeRequest::EVPowerScheduleEntry& out) {
    out.duration = in.Duration;
    convert(in.Power, out.power);
}

template <> void convert(const struct iso20_EVPowerScheduleType& in, ScheduleExchangeRequest::EVPowerSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    const auto& entries_in = in.EVPowerScheduleEntries.EVPowerScheduleEntry;
    out.entries.reserve(entries_in.arrayLen);
    for (auto i = 0; i < entries_in.arrayLen; ++i) {
        const auto& entry_in = entries_in.array[i];
        auto& entry_out = out.entries.emplace_back();
        convert(entry_in, entry_out);
    }
}

template <> void convert(const struct iso20_EVPriceRuleType& in, ScheduleExchangeRequest::EVPriceRule& out) {
    convert(in.EnergyFee, out.energy_fee);
    convert(in.PowerRangeStart, out.power_range_start);
}

template <> void convert(const struct iso20_EVPriceRuleStackType& in, ScheduleExchangeRequest::EVPriceRuleStack& out) {
    out.duration = in.Duration;

    const auto& rules_in = in.EVPriceRule;
    out.price_rules.reserve(rules_in.arrayLen);
    for (auto i = 0; i < rules_in.arrayLen; ++i) {
        const auto& rule_in = rules_in.array[i];
        auto& rule_out = out.price_rules.emplace_back();
        convert(rule_in, rule_out);
    }
}

template <>
void convert(const struct iso20_EVAbsolutePriceScheduleType& in,
             ScheduleExchangeRequest::EVAbsolutePriceSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    out.currency = CB2CPP_STRING(in.Currency);
    out.price_algorithm = CB2CPP_STRING(in.PriceAlgorithm);

    const auto& stacks_in = in.EVPriceRuleStacks.EVPriceRuleStack;
    out.price_rule_stacks.reserve(stacks_in.arrayLen);
    for (auto i = 0; i < stacks_in.arrayLen; ++i) {
        const auto& stack_in = stacks_in.array[i];
        auto& stack_out = out.price_rule_stacks.emplace_back();
        convert(stack_in, stack_out);
    }
}

template <> void convert(const struct iso20_EVEnergyOfferType& in, ScheduleExchangeRequest::EVEnergyOffer& out) {
    convert(in.EVPowerSchedule, out.power_schedule);
    convert(in.EVAbsolutePriceSchedule, out.absolute_price_schedule);
}

template <>
void convert(const struct iso20_Scheduled_SEReqControlModeType& in,
             ScheduleExchangeRequest::Scheduled_SEReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy);
    CB2CPP_CONVERT_IF_USED(in.EVEnergyOffer, out.energy_offer);
}

template <>
void convert(const struct iso20_Dynamic_SEReqControlModeType& in,
             ScheduleExchangeRequest::Dynamic_SEReqControlMode& out) {
    out.departure_time = in.DepartureTime;
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.minimum_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    convert(in.EVTargetEnergyRequest, out.target_energy);
    convert(in.EVMaximumEnergyRequest, out.max_energy);
    convert(in.EVMinimumEnergyRequest, out.min_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumV2XEnergyRequest, out.max_v2x_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumV2XEnergyRequest, out.min_v2x_energy);
}

template <> void convert(const struct iso20_ScheduleExchangeReqType& in, ScheduleExchangeRequest& out) {
    convert(in.Header, out.header);

    out.max_supporting_points = in.MaximumSupportingPoints;

    if (in.Dynamic_SEReqControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<ScheduleExchangeRequest::Dynamic_SEReqControlMode>();
        convert(in.Dynamic_SEReqControlMode, mode_out);
    } else if (in.Scheduled_SEReqControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<ScheduleExchangeRequest::Scheduled_SEReqControlMode>();
        convert(in.Scheduled_SEReqControlMode, mode_out);
    } else {
        throw std::runtime_error("No control mode selected in iso20_ScheduleExchangeReqType");
    }
}

template <> void convert(const ScheduleExchangeResponse::PowerSchedule& in, struct iso20_PowerScheduleType& out) {
    init_iso20_PowerScheduleType(&out);

    out.TimeAnchor = in.time_anchor;
    CPP2CB_CONVERT_IF_USED(in.available_energy, out.AvailableEnergy);
    CPP2CB_CONVERT_IF_USED(in.power_tolerance, out.PowerTolerance);

    if ((sizeof(out.PowerScheduleEntries.PowerScheduleEntry.array) /
         sizeof(out.PowerScheduleEntries.PowerScheduleEntry.array[0])) < in.entries.size()) {
        throw std::runtime_error("array is too large");
    }

    for (auto i = 0; in.entries.size(); i++) {
        auto& out_entry = out.PowerScheduleEntries.PowerScheduleEntry.array[i];
        const auto& in_entry = in.entries[i];

        out_entry.Duration = in_entry.duration;
        convert(in_entry.power, out_entry.Power);
        CPP2CB_CONVERT_IF_USED(in_entry.power_l2, out_entry.Power_L2);
        CPP2CB_CONVERT_IF_USED(in_entry.power_l3, out_entry.Power_L3);
    }
    out.PowerScheduleEntries.PowerScheduleEntry.arrayLen = in.entries.size();
}

template <> void convert(const ScheduleExchangeResponse::ChargingSchedule& in, struct iso20_ChargingScheduleType& out) {
    init_iso20_ChargingScheduleType(&out);

    convert(in.power_schedule, out.PowerSchedule);

    // todo(sl): price_schedule
}

template <> void convert(const ScheduleExchangeResponse::ScheduleTuple& in, struct iso20_ScheduleTupleType& out) {
    init_iso20_ScheduleTupleType(&out);

    out.ScheduleTupleID = in.schedule_tuple_id;
    convert(in.charging_schedule, out.ChargingSchedule);
    CPP2CB_CONVERT_IF_USED(in.discharging_schedule, out.DischargingSchedule);
}

struct ModeResponseVisitor {
    ModeResponseVisitor(iso20_ScheduleExchangeResType& res_) : res(res_){};
    void operator()(const ScheduleExchangeResponse::Dynamic_SEResControlMode& in) {
        init_iso20_Dynamic_SEResControlModeType(&res.Dynamic_SEResControlMode);
        CB_SET_USED(res.Dynamic_SEResControlMode);

        auto& out = res.Dynamic_SEResControlMode;

        CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime)
        CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
        CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);

        // Todo(sl): price_schedule
    }

    void operator()(const ScheduleExchangeResponse::Scheduled_SEResControlMode& in) {
        init_iso20_Scheduled_SEResControlModeType(&res.Scheduled_SEResControlMode);
        CB_SET_USED(res.Dynamic_SEResControlMode);

        auto& out = res.Scheduled_SEResControlMode;

        if ((sizeof(out.ScheduleTuple.array) / sizeof(out.ScheduleTuple.array[0])) < in.schedule_tuple.size()) {
            throw std::runtime_error("array is too large");
        }

        for (std::size_t i = 0; i < in.schedule_tuple.size(); i++) {
            convert(in.schedule_tuple[i], out.ScheduleTuple.array[i]);
        }
        out.ScheduleTuple.arrayLen = in.schedule_tuple.size();
    }

private:
    iso20_ScheduleExchangeResType& res;
};

template <> void convert(const ScheduleExchangeResponse& in, struct iso20_ScheduleExchangeResType& out) {
    init_iso20_ScheduleExchangeResType(&out);

    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    cb_convert_enum(in.processing, out.EVSEProcessing);

    // CPP2CB_ASSIGN_IF_USED(in.go_to_pause, out.GoToPause);
    // std::visit(ModeResponseVisitor(out), in.control_mode);

    // -----------------------------------------------------------------------------------

    out.GoToPause_isUsed = false;

    out.Dynamic_SEResControlMode_isUsed = false;
    out.Scheduled_SEResControlMode_isUsed = true;
    out.Scheduled_SEResControlMode.ScheduleTuple.arrayLen = 1;
    auto& tuple = out.Scheduled_SEResControlMode.ScheduleTuple.array[0];
    tuple.DischargingSchedule_isUsed = false;
    tuple.ChargingSchedule.AbsolutePriceSchedule_isUsed = false;
    tuple.ChargingSchedule.PriceLevelSchedule_isUsed = true;
    auto& price_schedule = tuple.ChargingSchedule.PriceLevelSchedule;
    price_schedule.TimeAnchor = 234;
    price_schedule.PriceScheduleID = 1;
    price_schedule.PriceScheduleDescription_isUsed = false;
    price_schedule.NumberOfPriceLevels = 0;
    price_schedule.PriceLevelScheduleEntries.PriceLevelScheduleEntry.arrayLen = 1;
    auto& price_entry = price_schedule.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array[0];
    price_entry.Duration = 23;
    price_entry.PriceLevel = 8;
    tuple.ScheduleTupleID = 1;
    auto& power_sched = tuple.ChargingSchedule.PowerSchedule;
    power_sched.AvailableEnergy_isUsed = false;
    power_sched.PowerTolerance_isUsed = false;
    power_sched.TimeAnchor = 23423;
    power_sched.PowerScheduleEntries.PowerScheduleEntry.arrayLen = 1;
    auto& entry = power_sched.PowerScheduleEntries.PowerScheduleEntry.array[0];
    entry.Duration = 23;
    entry.Power.Exponent = 2;
    entry.Power.Value = 10;
    entry.Power_L2_isUsed = 0;
    entry.Power_L3_isUsed = 0;
}

template <> void insert_type(VariantAccess& va, const struct iso20_ScheduleExchangeReqType& in) {
    va.insert_type<ScheduleExchangeRequest>(in);
};

template <> int serialize_to_exi(const ScheduleExchangeResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ScheduleExchangeRes);

    convert(in, doc.ScheduleExchangeRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ScheduleExchangeResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
