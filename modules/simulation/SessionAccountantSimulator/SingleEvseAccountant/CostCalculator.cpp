/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "CostCalculator.hpp"

#include <chrono>
#include <vector>

#include <utils/date.hpp> // to_rfc3339 date conversion function from EVerest framework

#include "CostItemRecord.hpp"

namespace {

int32_t get_items_duration_in_minutes(const SessionAccountant::CostItem& cost_item) {
    SessionAccountant::tstamp_t start_time = cost_item.time_point_from;
    SessionAccountant::tstamp_t stop_time = cost_item.time_point_to;
    return std::chrono::duration_cast<std::chrono::minutes>(stop_time - start_time).count();
}

int32_t calc_cost_for_idle(int32_t duration_minutes, const SessionAccountant::Tariff& tariff) {
    const int32_t MINUTES_PER_HOUR{60};

    if (duration_minutes <= tariff.idle_grace_minutes) {
        return 0;
    }

    // TODO(CB): Why does this give price updates with to large steps (> 1 mau)?
    return ((duration_minutes - tariff.idle_grace_minutes) * tariff.idle_hourly_price_mau) / MINUTES_PER_HOUR;
}

int32_t get_items_energy_in_Wh(const SessionAccountant::CostItem& cost_item) {
    return cost_item.metervalue_Wh_to - cost_item.metervalue_Wh_from;
}

int32_t calc_cost_for_energy(int32_t energy_Wh, const SessionAccountant::Tariff& tariff) {
    const int32_t UNIT_SCALING_Wh_vs_kWh{1000};
    const int32_t UNIT_SCALING_milli{1000};
    int32_t money_amount = (energy_Wh * tariff.energy_price_kWh_millimau) / UNIT_SCALING_Wh_vs_kWh / UNIT_SCALING_milli;
    return static_cast<int32_t>(money_amount);
}

types::session_cost::SessionCostChunk sessionCostChunk_from_CostItem(const SessionAccountant::CostItem& cost_item,
                                                                     const SessionAccountant::Tariff& tariff) {
    types::session_cost::SessionCostChunk cost_chunk;
    cost_chunk.timestamp_from = Everest::Date::to_rfc3339(cost_item.time_point_from);
    cost_chunk.timestamp_to = Everest::Date::to_rfc3339(cost_item.time_point_to);
    cost_chunk.metervalue_from = cost_item.metervalue_Wh_from;
    cost_chunk.metervalue_to = cost_item.metervalue_Wh_to;
    switch (cost_item.category) {
    case SessionAccountant::ItemCategory::ChargedEnergy: {
        cost_chunk.category = types::session_cost::CostCategory::Energy;
        int32_t energy = get_items_energy_in_Wh(cost_item);
        cost_chunk.cost = types::money::MoneyAmount{.value = calc_cost_for_energy(energy, tariff)};
        break;
    }
    case SessionAccountant::ItemCategory::IdleTime: {
        cost_chunk.category = types::session_cost::CostCategory::Time;
        int32_t time_delta_minutes = get_items_duration_in_minutes(cost_item);
        cost_chunk.cost = types::money::MoneyAmount{.value = calc_cost_for_idle(time_delta_minutes, tariff)};
        break;
    }
    case SessionAccountant::ItemCategory::Other: {
        cost_chunk.category = types::session_cost::CostCategory::Other;
        break;
    }
    }

    return cost_chunk;
}

} // namespace

namespace SessionAccountant {

enum class SessionStage {
    PRE_SESSION,
    PRE_CHARGE_IDLE,
    CHARGING,
    POST_CHARGE_IDLE
};

CostCalculator::CostCalculator(const Tariff& tariff) :
    m_tariff(tariff),
    m_session_stage(SessionStage::PRE_SESSION),
    m_cost_record(std::make_unique<CostItemRecord>()),
    m_no_of_finalized_cost_chunks(0) {
    m_cost_record->add_idle_item_as_first_item(date::utc_clock::now());
    m_session_stage = SessionStage::PRE_CHARGE_IDLE;
}

// Destructor set to default here, because of the forward declarations
CostCalculator::~CostCalculator() = default;

void CostCalculator::handle_tariff_update(const Tariff& tariff) {
    if (is_charging() && m_tariff.energy_price_kWh_millimau != tariff.energy_price_kWh_millimau) {
        m_cost_record->add_consecutive_energy_item();
    }
    if (is_idle() && m_tariff.idle_hourly_price_mau != tariff.idle_hourly_price_mau) {
        m_cost_record->add_consecutive_idle_item();
    }

    // We only allow a change of energy price and hourly rates, but not of grace minutes
    m_tariff.energy_price_kWh_millimau = tariff.energy_price_kWh_millimau;
    m_tariff.idle_hourly_price_mau = tariff.idle_hourly_price_mau;
}

void CostCalculator::handle_powermeter_update(const PowermeterUpdate& powermeter) {
    if (is_charging()) {
        m_cost_record->update_energy_cost_item(powermeter);

        update_cost_chunks();
    }
}

void CostCalculator::handle_time_update(const tstamp_t& tstamp) {
    // TODO(CB): Here one could allow for free idle before charging (by modifying the tariff)
    if (is_idle()) {
        m_cost_record->update_idle_cost_item(tstamp);

        update_cost_chunks();
    }
}

void CostCalculator::start_idle(const tstamp_t& tstamp) {
    if (m_session_stage != SessionStage::PRE_SESSION) {
        // TODO(CB): Throw?
        return;
    }
    m_cost_record->add_idle_item_as_first_item(tstamp);
}

void CostCalculator::start_charging(const PowermeterUpdate& powermeter) {
    if (is_charging()) {
        // TODO(CB): throw?
        return;
    }
    if (is_idle()) {
        m_cost_record->update_idle_cost_item(powermeter.time_point);
    }
    m_cost_record->add_energy_item(powermeter);

    update_cost_chunks();

    m_session_stage = SessionStage::CHARGING;
}

void CostCalculator::stop_charging(const PowermeterUpdate& powermeter) {
    if (is_idle()) {
        // TODO(CB): Throw?
        return;
    }
    if (is_charging()) {
        m_cost_record->update_energy_cost_item(powermeter);
        m_cost_record->add_consecutive_idle_item();

        update_cost_chunks();
    }
    m_session_stage = SessionStage::POST_CHARGE_IDLE;
}

bool CostCalculator::is_charging() const {
    return m_session_stage == SessionAccountant::SessionStage::CHARGING;
}

bool CostCalculator::is_idle() const {
    return m_session_stage == SessionStage::PRE_CHARGE_IDLE || m_session_stage == SessionStage::POST_CHARGE_IDLE;
}

void CostCalculator::update_cost_chunks() {
    const std::vector<CostItem>* cost_items = m_cost_record->get_items();
    int no_of_finalized_items = m_cost_record->get_no_of_finalized_items();

    int no_of_items = cost_items->size();

    if (no_of_items == m_no_of_finalized_cost_chunks) {
        return;
    }

    for (int i = m_no_of_finalized_cost_chunks; i < m_cost_chunks.size(); ++i) {
        m_cost_chunks.pop_back();
    }

    for (auto it = cost_items->cbegin() + m_no_of_finalized_cost_chunks; it != cost_items->cend(); ++it) {
        m_cost_chunks.push_back(sessionCostChunk_from_CostItem(*it, m_tariff));
    }

    m_no_of_finalized_cost_chunks = no_of_finalized_items;
}

const std::vector<types::session_cost::SessionCostChunk>* CostCalculator::get_cost_chunks() const {
    return &m_cost_chunks;
}
} // namespace SessionAccountant