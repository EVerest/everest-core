#pragma once
#include <Charger.hpp>
#include <functional>
#include <generated/types/uk_random_delay.hpp>

namespace module {
namespace energy_grid {
struct LastValues;
} // namespace energy_grid

struct UKRandomDelayStatus;

bool almost_eq(types::power_supply_DC::Capabilities const& lhs, types::power_supply_DC::Capabilities const& rhs);

bool uk_random_delay_is_relevant_state(Charger::EvseState charger_state);

void uk_random_delay_start_new_delay(UKRandomDelayStatus& random_delay);

types::uk_random_delay::CountDown uk_random_delay_current_count_down(bool running, float enforced_limits,
                                                                     float limit_random_delay_when_started,
                                                                     UKRandomDelayStatus const& random_delay);

float apply_uk_random_delay(UKRandomDelayStatus& random_delay, float& limit_random_delay_when_started, float& limit,
                            const float init_enforced_limit, const Charger::EvseState charger_state,
                            const float last_enforced_limit, const bool random_delay_needed,
                            std::function<void(types::uk_random_delay::CountDown const&)> const& publish_countdown);

types::iso15118::DcEvseMaximumLimits prepare_evse_max_limits(types::energy::EnforcedLimits const& value,
                                                             energy_grid::LastValues const& last_values);

bool prepare_export_to_grid(types::iso15118::DcEvseMaximumLimits& evse_max_limits, bool hack_allow_bpt_with_iso2,
                            bool sae_bidi_active);

bool check_if_enforced_limits_changed_and_update(types::energy::EnforcedLimits const& value,
                                                 types::power_supply_DC::Capabilities const& powersupply_capabilities,

                                                 types::evse_manager::EVInfo const& ev_info,
                                                 energy_grid::LastValues& last);

void apply_number_of_phase_limit(int active_phasecount, bool supports_changing_phases_during_charging,
                                 std::optional<types::energy::IntegerWithSource> const& ac_max_phase_count,
                                 std::atomic_int& ac_nr_phases_active,
                                 std::function<bool(bool)> const& switch_three_phases_while_charging);

float apply_AC_limit(types::energy::LimitsRes const& energy_limits, int ac_nr_phases_active, double ac_nominal_voltage,
                     std::function<void(float)> const& publish);
} // namespace module
