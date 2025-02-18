// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Offer.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

std::ostream& operator<<(std::ostream& out, const Offer& self) {
    /*out << "\nOffer:\n\nImport:\n------\n";
    for (auto e : self.import_offer) {
        out << e;
    }
    out << "\n\nExport:\n------\n";
    for (auto e : self.export_offer) {
        out << e;
    }*/

    const types::energy::NumberWithSource nonumber = {-9999.0, "Not set"};

    out << fmt::format("\033[1;34mOffer[0]: Import {}A ({}) {}W ({}) Export {}A ({}) {}W ({})\033[1;0m",
                       self.import_offer[0].limits_to_root.ac_max_current_A.value_or(nonumber).value,
                       self.import_offer[0].limits_to_root.ac_max_current_A.value_or(nonumber).source,
                       self.import_offer[0].limits_to_root.total_power_W.value_or(nonumber).value,
                       self.import_offer[0].limits_to_root.total_power_W.value_or(nonumber).source,
                       self.export_offer[0].limits_to_root.ac_max_current_A.value_or(nonumber).value,
                       self.export_offer[0].limits_to_root.ac_max_current_A.value_or(nonumber).source,
                       self.export_offer[0].limits_to_root.total_power_W.value_or(nonumber).value,
                       self.export_offer[0].limits_to_root.total_power_W.value_or(nonumber).source);
    return out;
}

template <class T> static void apply_one_limit_if_smaller(std::optional<T>& a, const std::optional<T>& b) {
    if (b.has_value()) {
        if (a.has_value()) {
            if (a.value().value > b.value().value) {
                a = b.value();
            } else if (a.value().value == b.value().value) {
                a.value().source += "," + b.value().source;
            }
        } else {
            a = b.value();
        }
    }
}

template <class T> static void apply_one_limit_if_greater(std::optional<T>& a, const std::optional<T>& b) {
    if (b.has_value()) {
        if (a.has_value()) {
            if (a.value().value < b.value().value) {
                a = b.value();
            } else if (a.value().value == b.value().value) {
                a.value().source += "," + b.value().source;
            }
        } else {
            a = b.value();
        }
    }
}

static void apply_limits(ScheduleReq& a, const ScheduleReq& b) {
    if (a.size() != b.size()) {
        EVLOG_error << fmt::format("apply_limits: a({}) and b({}) do not have the same size.", a.size(), b.size());
        return;
    }
    for (ScheduleReq::size_type i = 0; i < a.size(); i++) {
        // limits to leave are already merged to the root side, so we dont use them here
        apply_one_limit_if_smaller(a[i].limits_to_root.ac_max_current_A, b[i].limits_to_root.ac_max_current_A);
        apply_one_limit_if_smaller(a[i].limits_to_root.ac_max_phase_count, b[i].limits_to_root.ac_max_phase_count);
        apply_one_limit_if_smaller(a[i].limits_to_root.total_power_W, b[i].limits_to_root.total_power_W);
        apply_one_limit_if_greater(a[i].limits_to_root.ac_min_phase_count, b[i].limits_to_root.ac_min_phase_count);
        apply_one_limit_if_greater(a[i].limits_to_root.ac_min_current_A, b[i].limits_to_root.ac_min_current_A);

        // copy other information if any
        a[i].price_per_kwh = b[i].price_per_kwh;
        a[i].limits_to_root.ac_number_of_active_phases = b[i].limits_to_root.ac_number_of_active_phases;
    }
}

Offer::Offer(Market& market) {
    // create maximum offer for this market place
    create_offer_for_local_market(market);
}

// Recursive: start at leaf, walk to root and create empty root offer. On the way back, apply all limits of local
// marketplaces until we are at the leaf again.
void Offer::create_offer_for_local_market(Market& market) {

    if (!market.is_root()) {
        create_offer_for_local_market(*market.parent());
    } else {
        // initialize time slots
        import_offer = globals.empty_schedule_req;
        export_offer = globals.empty_schedule_req;
    }

    // limit offer with limits at this market place
    apply_limits(import_offer, market.get_available_energy_import());

    // limit offer with limits at this market place
    apply_limits(export_offer, market.get_available_energy_export());

    optimizer_target = market.energy_flow_request.optimizer_target;
}

} // namespace module
