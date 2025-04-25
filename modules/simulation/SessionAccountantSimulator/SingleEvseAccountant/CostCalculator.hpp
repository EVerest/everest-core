/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "generated/types/session_cost.hpp"

// FIXME(CB): split types to hide types which are not required here?
#include "SingleEvseAccountantTypes.hpp"

namespace SessionAccountant {

// forward declaration
enum class SessionStage;
class CostItemRecord;

class CostCalculator {
public:
    explicit CostCalculator(const Tariff& tariff);
    ~CostCalculator();

    void start_idle(const tstamp_t& tstamp);
    void start_charging(const PowermeterUpdate& powermeter);
    void stop_charging(const PowermeterUpdate& powermeter);

    void handle_powermeter_update(const PowermeterUpdate& powermeter);
    void handle_time_update(const tstamp_t& tstamp);
    void handle_tariff_update(const Tariff& tariff);

    const std::vector<types::session_cost::SessionCostChunk>* get_cost_chunks() const;

private:
    void update_cost_chunks();
    bool is_charging() const;
    bool is_idle() const;

    Tariff m_tariff;
    SessionStage m_session_stage;
    std::unique_ptr<CostItemRecord> m_cost_record;
    std::vector<types::session_cost::SessionCostChunk> m_cost_chunks;
    int m_no_of_finalized_cost_chunks{0};
};
} // namespace SessionAccountant