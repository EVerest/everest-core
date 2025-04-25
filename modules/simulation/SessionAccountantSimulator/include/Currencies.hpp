/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include <map>

// #include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/types/money.hpp>

namespace {
const std::map<int, types::money::Currency> code2currency_map{{978, {types::money::CurrencyCode::EUR, 2}}};
}

namespace currencies {

types::money::Currency currency_for_numeric_code(int numeric_code) {
    if (code2currency_map.count(numeric_code) > 0) {
        return code2currency_map.at(numeric_code);
    } else {
        // FIXME(CB): Be more concise ....
        throw 1;
    }
}

} // namespace currencies
