/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <vector>

#include "SingleEvseAccountantTypes.hpp"

namespace SessionAccountant {

// TODO(CB): Write tests for this and try to misuse it -> determine a more robust design
class CostItemRecord {
public:
    void add_idle_item_as_first_item(const tstamp_t& start_time);
    void update_idle_cost_item(const tstamp_t& time_point);
    void update_energy_cost_item(const PowermeterUpdate& powermeter);
    void add_energy_item(const PowermeterUpdate& powermeter);
    void add_consecutive_energy_item();
    void add_consecutive_idle_item();
    const std::vector<CostItem>* get_items() const;
    int get_no_of_finalized_items() const;

private:
    void sanitize_last_item();
    void add_energy_item_to_cost_record(tstamp_t time_point, uint32_t meter_value);
    void add_idle_item_to_cost_record(tstamp_t time_point);

    std::vector<CostItem> m_cost_items{};
    int m_no_of_finalized_items{0};
};

} // namespace SessionAccountant