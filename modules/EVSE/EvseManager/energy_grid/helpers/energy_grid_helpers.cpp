#include "energy_grid_helpers.hpp"
#include "energy_grid/energyImpl.hpp"
#include <everest/util/comparison.hpp>

namespace module {

template <class T, class U> auto almost_eq(T const& a, U const& b) {
    return everest::lib::util::almost_eq<1>(a, b);
}
// helper to find out if voltage changed (more then noise)
auto const voltage_changed = [](float val_a, float val_b) {
    return not everest::lib::util::in_noise_range(val_a, val_b, 1.0F);
};

bool almost_eq(types::power_supply_DC::Capabilities const& lhs, types::power_supply_DC::Capabilities const& rhs) {
    bool result = lhs.bidirectional == rhs.bidirectional and
                  almost_eq(lhs.current_regulation_tolerance_A, rhs.current_regulation_tolerance_A) and
                  almost_eq(lhs.peak_current_ripple_A, rhs.peak_current_ripple_A) and
                  almost_eq(lhs.max_export_voltage_V, rhs.max_export_voltage_V) and
                  almost_eq(lhs.min_export_voltage_V, rhs.min_export_voltage_V) and
                  almost_eq(lhs.max_export_current_A, rhs.max_export_current_A) and
                  almost_eq(lhs.min_export_current_A, rhs.min_export_current_A) and
                  almost_eq(lhs.max_export_power_W, rhs.max_export_power_W) and
                  almost_eq(lhs.max_import_voltage_V, rhs.max_import_voltage_V) and
                  almost_eq(lhs.min_import_voltage_V, rhs.min_import_voltage_V) and
                  almost_eq(lhs.max_import_current_A, rhs.max_import_current_A) and
                  almost_eq(lhs.min_import_current_A, rhs.min_import_current_A) and
                  almost_eq(lhs.max_import_power_W, rhs.max_import_power_W) and
                  almost_eq(lhs.conversion_efficiency_import, rhs.conversion_efficiency_import) and
                  almost_eq(lhs.conversion_efficiency_export, rhs.conversion_efficiency_export);
    return result;
}

bool uk_random_delay_is_relevant_state(Charger::EvseState charger_state) {
    auto result = charger_state not_eq Charger::EvseState::PrepareCharging and
                  charger_state not_eq Charger::EvseState::Charging and
                  charger_state not_eq Charger::EvseState::WaitingForAuthentication and
                  charger_state not_eq Charger::EvseState::WaitingForEnergy;
    return result;
}

void uk_random_delay_start_new_delay(UKRandomDelayStatus& random_delay) {
    random_delay.running = true;
    random_delay.start_time = date::utc_clock::now();
    auto random_delay_s = std::rand() % random_delay.max_duration.load().count();
    random_delay.end_time = std::chrono::steady_clock::now() + std::chrono::seconds(random_delay_s);
    EVLOG_info << "UK Smart Charging regulations: Starting random delay of " << random_delay_s << "s";
}

types::uk_random_delay::CountDown uk_random_delay_current_count_down(bool running, float enforced_limits,
                                                                     float limit_random_delay_when_started,
                                                                     UKRandomDelayStatus const& random_delay) {
    types::uk_random_delay::CountDown c;
    c.countdown_s = 0;
    c.current_limit_after_delay_A = enforced_limits;
    c.current_limit_during_delay_A = limit_random_delay_when_started;

    if (not running) {
        return c;
    }

    auto seconds_left =
        std::chrono::duration_cast<std::chrono::seconds>(random_delay.end_time - std::chrono::steady_clock::now())
            .count();

    if (seconds_left <= 0) {
        c.countdown_s = 0;
    } else {
        c.countdown_s = seconds_left;
        c.start_time = Everest::Date::to_rfc3339(random_delay.start_time);
    }
    return c;
}

float apply_uk_random_delay(UKRandomDelayStatus& random_delay, float& limit_random_delay_when_started, float& limit,
                            const float init_enforced_limit, const Charger::EvseState charger_state,
                            const float last_enforced_limit, const bool random_delay_needed,
                            std::function<void(types::uk_random_delay::CountDown const&)> const& publish_countdown) {

    if (not random_delay.enabled) {
        return init_enforced_limit;
    }

    float enforced_limit = init_enforced_limit;

    // Are we in a state where a random delay makes sense?
    if (uk_random_delay_is_relevant_state(charger_state)) {
        random_delay.running = false;
    }

    // Do we need to start a new random delay?
    // Ignore changes of less then 0.1 amps
    if (not random_delay.running and random_delay_needed) {
        uk_random_delay_start_new_delay(random_delay);
        limit_random_delay_when_started = last_enforced_limit;
    }

    auto c = uk_random_delay_current_count_down(random_delay.running, enforced_limit, limit_random_delay_when_started,
                                                random_delay);
    if (not random_delay.running) {
        publish_countdown(c);
        return enforced_limit;
    }

    // use limit from the time point when the random delay started
    limit = limit_random_delay_when_started;

    if (c.countdown_s == 0) {
        EVLOG_info << "UK Smart Charging regulations: Random delay elapsed.";
        random_delay.running = false;
    } else {
        EVLOG_debug << "Random delay running, " << c.countdown_s
                    << "s left. Applying the limit before the random delay (" << limit_random_delay_when_started
                    << "A) instead of requested limit (" << enforced_limit << "A)";
    }
    publish_countdown(c);
    return enforced_limit;
}

types::iso15118::DcEvseMaximumLimits prepare_evse_max_limits(types::energy::EnforcedLimits const& value,
                                                             energy_grid::LastValues const& last_values) {
    constexpr float min_on_voltage = 10.;
    types::iso15118::DcEvseMaximumLimits evse_max_limits;
    auto const& powersupply_capabilities = last_values.powersupply_capabilities;

    if (last_values.target_voltage > min_on_voltage) {
        // we use target_voltage here to calculate current limit.
        // If target_voltage is a lot higher then the actual voltage the
        // current limit is too low, i.e. charging will not reach the actual watt value.
        // FIXME: we could use some magic here that involves actual measured voltage as well.
        if (last_values.actual_voltage > min_on_voltage) {
            evse_max_limits.evse_maximum_current_limit =
                value.limits_root_side.total_power_W.value().value / last_values.actual_voltage;
        } else {
            evse_max_limits.evse_maximum_current_limit =
                value.limits_root_side.total_power_W.value().value / last_values.target_voltage;
        }
    } else {
        evse_max_limits.evse_maximum_current_limit = powersupply_capabilities.max_export_current_A;
    }

    if (evse_max_limits.evse_maximum_current_limit > powersupply_capabilities.max_export_current_A) {
        evse_max_limits.evse_maximum_current_limit = powersupply_capabilities.max_export_current_A;
    }

    if (powersupply_capabilities.max_import_current_A.has_value() &&
        evse_max_limits.evse_maximum_current_limit < -powersupply_capabilities.max_import_current_A.value()) {
        evse_max_limits.evse_maximum_current_limit = -powersupply_capabilities.max_import_current_A.value();
    }

    // now evse_max_limits.evse_maximum_current_limit is between
    // -max_import_current_A ... +max_export_current_A

    evse_max_limits.evse_maximum_power_limit = value.limits_root_side.total_power_W.value().value;
    if (evse_max_limits.evse_maximum_power_limit > powersupply_capabilities.max_export_power_W) {
        evse_max_limits.evse_maximum_power_limit = powersupply_capabilities.max_export_power_W;
    }

    if (powersupply_capabilities.max_import_power_W.has_value() &&
        evse_max_limits.evse_maximum_power_limit < -powersupply_capabilities.max_import_power_W.value()) {
        evse_max_limits.evse_maximum_power_limit = -powersupply_capabilities.max_import_power_W.value();
    }

    // now evse_max_limits.evse_maximum_power_limit is between
    // -max_import_power_W ... +max_export_power_W

    evse_max_limits.evse_maximum_voltage_limit = powersupply_capabilities.max_export_voltage_V;

    return evse_max_limits;
}

// FIXME: The origianl code inverts the power limit, when the current limit is non negative. Is that relevant at all?
bool prepare_export_to_grid(types::iso15118::DcEvseMaximumLimits& evse_max_limits, bool hack_allow_bpt_with_iso2,
                            bool sae_bidi_active) {
    // FIXME: we tell the ISO stack positive numbers for DIN spec and ISO-2 here in case of exporting to
    // grid. This needs to be fixed in the transition to -20 for BPT.

    if (evse_max_limits.evse_maximum_current_limit >= 0) {
        return false;
    }

    if (not(hack_allow_bpt_with_iso2 or sae_bidi_active)) {
        EVLOG_error << "Bidirectional export back to grid requested, but not supported. Enable "
                       "ISO-20 or set hack_allow_bpt_with_iso2 config option.";
        evse_max_limits.evse_maximum_power_limit = 0.;
        evse_max_limits.evse_maximum_current_limit = 0.;
        return false;
    }

    if (hack_allow_bpt_with_iso2) {
        evse_max_limits.evse_maximum_power_limit = std::abs(evse_max_limits.evse_maximum_power_limit);
        evse_max_limits.evse_maximum_current_limit = std::abs(evse_max_limits.evse_maximum_current_limit);
    }

    return true;
}

/**
 * @brief Check limits for changes and update on change
 * @return 'true' if last values were changed
 */
bool check_if_enforced_limits_changed_and_update(types::energy::EnforcedLimits const& value,
                                                 types::power_supply_DC::Capabilities const& powersupply_capabilities,

                                                 types::evse_manager::EVInfo const& ev_info,
                                                 energy_grid::LastValues& last) {
    bool values_changed = false;

    if (value.limits_root_side.total_power_W.has_value() and value.limits_root_side.ac_max_current_A.has_value()) {

        float watt_leave_side = value.limits_root_side.total_power_W->value;
        float ampere_root_side = value.limits_root_side.ac_max_current_A->value;
        float target_voltage = ev_info.target_voltage.value_or(0.);
        float actual_voltage = ev_info.present_voltage.value_or(0.);

        values_changed = not almost_eq(last.enforced_limits_watt, watt_leave_side) or
                         not almost_eq(last.enforced_limits_ampere, ampere_root_side) or
                         not almost_eq(last.powersupply_capabilities, powersupply_capabilities) or
                         not almost_eq(target_voltage, last.target_voltage) or
                         voltage_changed(actual_voltage, last.actual_voltage);
    }

    if (values_changed) {
        last.enforced_limits_ampere = value.limits_root_side.total_power_W->value;
        last.enforced_limits_watt = value.limits_root_side.ac_max_current_A->value;
        last.target_voltage = ev_info.target_voltage.value_or(0.);
        last.actual_voltage = ev_info.present_voltage.value_or(0.);
        last.powersupply_capabilities = powersupply_capabilities;
    }

    return values_changed;
}

void apply_number_of_phase_limit(int active_phasecount, bool supports_changing_phases_during_charging,
                                 std::optional<types::energy::IntegerWithSource> const& ac_max_phase_count,
                                 std::atomic_int& ac_nr_phases_active,
                                 std::function<bool(bool)> const& switch_three_phases_while_charging) {
    if (not ac_max_phase_count.has_value()) {
        return;
    }

    if (ac_max_phase_count->value == active_phasecount) {
        return;
    }

    if (not supports_changing_phases_during_charging) {
        EVLOG_error << fmt::format("Energy manager requests switching #ph from {} to {}, but switching phases during "
                                   "charging is not supported by HW.",
                                   active_phasecount, ac_max_phase_count->value);
        return;
    }

    if (not switch_three_phases_while_charging(ac_max_phase_count->value == 3)) {
        EVLOG_warning << fmt::format("3ph/1ph: Energymanager requests switching #ph from {} to {}, ignored.",
                                     active_phasecount, ac_max_phase_count->value);
        return;
    }

    ac_nr_phases_active = ac_max_phase_count->value;
    EVLOG_info << fmt::format("3ph/1ph: Switching #ph from {} to {}", active_phasecount, ac_max_phase_count->value);
}

float apply_AC_limit(types::energy::LimitsRes const& energy_limits, int ac_nr_phases_active, double ac_nominal_voltage,
                     std::function<void(float)> const& publish) {
    float limit = 0.;

    if (energy_limits.ac_max_current_A.has_value()) {
        limit = energy_limits.ac_max_current_A->value;
    }

    // apply watt limit
    if (energy_limits.total_power_W.has_value()) {
        publish(energy_limits.total_power_W->value);

        auto val = static_cast<float>(energy_limits.total_power_W->value / ac_nominal_voltage / ac_nr_phases_active);
        if (val < limit) {
            limit = val;
        }
    }
    return limit;
}

} // namespace module
