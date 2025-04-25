/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include <date/date.h>
#include <date/tz.h>

#include "generated/types/money.hpp"
#include "generated/types/powermeter.hpp"
#include "generated/types/session_cost.hpp"

#include "Timer/SimpleTimeout.hpp"

namespace SessionAccountant {

using tstamp_t = std::chrono::time_point<date::utc_clock>;

struct Tariff {
    int32_t energy_price_kWh_millimau;
    int32_t idle_grace_minutes;
    int32_t idle_hourly_price_mau;
    int32_t reserved_amount_of_money_mau;
    types::money::Currency currency;
};
struct PowermeterUpdate {
    std::chrono::time_point<date::utc_clock> time_point{}; ///< Timestamp of measurement
    float import_Wh{};                                     ///< Imported energy (from grid)
    float export_Wh{};                                     ///< Exported energy (to grid)
    float current_power_W{}; ///< Instantaneous power. Negative values are exported, positive values imported Energy.
};

enum class SessionStatus {
    Active,
    Passive,
    Done
};

enum class ItemCategory {
    ChargedEnergy,
    IdleTime,
    Other,
};

std::ostream& operator<<(std::ostream& oss, const ItemCategory& cat);

struct CostItem {
    tstamp_t time_point_from{};
    tstamp_t time_point_to{};
    uint32_t metervalue_Wh_from{0};
    uint32_t metervalue_Wh_to{0};
    ItemCategory category{ItemCategory::Other};
};

std::ostream& operator<<(std::ostream& oss, const CostItem& item);

enum class StopReason {
    timeLimit,
    pmeter_timeout,
    creditLimit
};

struct SessionContext {
    uint32_t evse_id{};                      // ID of EVSE: should be constant
    std::optional<std::string> session_id{}; // ID of session: unique per charging session
    std::optional<float> energy_meter_begin{};
    std::optional<float> energy_meter_final{};
};

// FIXME(CB): Remove from here?
SessionAccountant::PowermeterUpdate extract_Powermeter_data(const types::powermeter::Powermeter& pmeter);

} // namespace SessionAccountant
