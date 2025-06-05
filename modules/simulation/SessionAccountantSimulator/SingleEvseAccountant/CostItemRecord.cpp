/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "CostItemRecord.hpp"

#include <utility>

namespace {

bool last_item_is_energy_item(const std::vector<SessionAccountant::CostItem>& items) {
    if (items.empty()) {
        return false;
    }
    return items.back().category == SessionAccountant::ItemCategory::ChargedEnergy;
}

bool last_item_is_idle_item(const std::vector<SessionAccountant::CostItem>& items) {
    if (items.empty()) {
        return false;
    }
    return items.back().category == SessionAccountant::ItemCategory::IdleTime;
}

bool item_from_and_to_are_equal(const SessionAccountant::CostItem& item) {
    return ((item.time_point_from == item.time_point_to) && (item.metervalue_Wh_from == item.metervalue_Wh_to));
}

} // namespace

namespace SessionAccountant {

void CostItemRecord::add_idle_item_as_first_item(const tstamp_t& start_time) {
    if (!m_cost_items.empty()) {
        // TODO(CB): THROW?
        return;
    }

    add_idle_item_to_cost_record(start_time);
}

void CostItemRecord::update_idle_cost_item(const tstamp_t& time_point) {
    if (last_item_is_idle_item(m_cost_items)) {
        // TODO(CB): Test if time_point_to > time_point_from
        // But that could be a problem if another modules message took a while to arrive here...
        m_cost_items.back().time_point_to = time_point;
    }
}

void CostItemRecord::update_energy_cost_item(const PowermeterUpdate& powermeter) {
    if (last_item_is_energy_item(m_cost_items)) {
        m_cost_items.back().metervalue_Wh_to = static_cast<uint32_t>(powermeter.import_Wh);
        m_cost_items.back().time_point_to = powermeter.time_point;
    }
}

void CostItemRecord::add_energy_item(const PowermeterUpdate& powermeter) {
    sanitize_last_item();

    add_energy_item_to_cost_record(powermeter.time_point, static_cast<uint32_t>(powermeter.import_Wh));

    m_no_of_finalized_items = m_cost_items.size() - 1;
}

void CostItemRecord::add_consecutive_energy_item() {
    if (last_item_is_energy_item(m_cost_items)) {
        if (item_from_and_to_are_equal(m_cost_items.back())) {
            // Do nothing, leave existing item in place to be used as the new one
            return;
        }

        add_energy_item_to_cost_record(m_cost_items.back().time_point_to, m_cost_items.back().metervalue_Wh_to);

        m_no_of_finalized_items = m_cost_items.size() - 1;
    } else {
        // TODO(CB): Throw?
    }
}

void CostItemRecord::add_consecutive_idle_item() {
    if (m_cost_items.empty()) {
        // TODO(CB): Throw?
        return;
    }

    if (item_from_and_to_are_equal(m_cost_items.back())) {
        if (last_item_is_idle_item(m_cost_items)) {
            // Do nothing, leave existing item in place to be used as the new one
        } else if (last_item_is_energy_item(m_cost_items)) {
            auto t_point = m_cost_items.back().time_point_to;
            m_cost_items.pop_back();
            add_idle_item_to_cost_record(t_point);
        }
    } else {
        add_idle_item_to_cost_record(m_cost_items.back().time_point_to);
    }

    m_no_of_finalized_items = m_cost_items.size() - 1;
}

const std::vector<CostItem>* CostItemRecord::get_items() const {
    // TODO(CB): Returning a ref to a member is a bad idea (use shared_ptr)
    return &m_cost_items;
}

int CostItemRecord::get_no_of_finalized_items() const {
    return m_no_of_finalized_items;
}

void CostItemRecord::sanitize_last_item() {
    if (m_cost_items.empty()) {
        return;
    }

    if (item_from_and_to_are_equal(m_cost_items.back())) {
        m_cost_items.pop_back();
    }
}

void CostItemRecord::add_energy_item_to_cost_record(tstamp_t time_point, uint32_t meter_value) {
    auto& new_item = m_cost_items.emplace_back(CostItem{});
    new_item.time_point_from = time_point;
    new_item.time_point_to = time_point;
    new_item.metervalue_Wh_from = meter_value;
    new_item.metervalue_Wh_to = meter_value;
    new_item.category = ItemCategory::ChargedEnergy;
}

void CostItemRecord::add_idle_item_to_cost_record(tstamp_t time_point) {
    auto& new_item = m_cost_items.emplace_back(CostItem{});
    new_item.time_point_from = time_point;
    new_item.time_point_to = time_point;
    new_item.category = ItemCategory::IdleTime;
}
} // namespace SessionAccountant
