/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "SingleEvseAccountantTypes.hpp"

#include <utils/date.hpp>

namespace SessionAccountant {

std::ostream& operator<<(std::ostream& oss, const ItemCategory& cat) {
    switch (cat) {
    case ItemCategory::ChargedEnergy:
        oss << "ChargedEnergy";
        break;
    case ItemCategory::IdleTime:
        oss << "IdleTime";
        break;
    case ItemCategory::Other:
        oss << "Other";
        break;
    }
    return oss;
}

std::ostream& operator<<(std::ostream& oss, const CostItem& item) {
    oss << item.time_point_from << " -> " << item.time_point_to << ": " << item.metervalue_Wh_from << " Wh -> "
        << item.metervalue_Wh_to << " Wh; "
        << "'" << item.category << "'";

    return oss;
}

// FIXME(CB): define somewhere else?
// FIXME(CB): use sth. more complex, like a function or class which can also calculate current_power_W from consecutive
// Wh_import.total values
PowermeterUpdate extract_Powermeter_data(const types::powermeter::Powermeter& pmeter) {
    SessionAccountant::PowermeterUpdate ret{.time_point = Everest::Date::from_rfc3339(pmeter.timestamp),
                                            .import_Wh = pmeter.energy_Wh_import.total,
                                            .export_Wh = pmeter.energy_Wh_export ? pmeter.energy_Wh_export->total : 0,
                                            // FIXME(CB): Using a magic sentinel value - bad!
                                            .current_power_W = pmeter.power_W ? pmeter.power_W->total : -9999999999};
    return ret;
}

} // namespace SessionAccountant