// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/money/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/money.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::money {

using CurrencyCode_Internal = ::types::money::CurrencyCode;
using CurrencyCode_External = CurrencyCode;

CurrencyCode_Internal to_internal_api(CurrencyCode_External const& val);
CurrencyCode_External to_external_api(CurrencyCode_Internal const& val);

using Currency_Internal = ::types::money::Currency;
using Currency_External = Currency;

Currency_Internal to_internal_api(Currency_External const& val);
Currency_External to_external_api(Currency_Internal const& val);

using MoneyAmount_Internal = ::types::money::MoneyAmount;
using MoneyAmount_External = MoneyAmount;

MoneyAmount_Internal to_internal_api(MoneyAmount_External const& val);
MoneyAmount_External to_external_api(MoneyAmount_Internal const& val);

using Price_Internal = ::types::money::Price;
using Price_External = Price;

Price_Internal to_internal_api(Price_External const& val);
Price_External to_external_api(Price_Internal const& val);

} // namespace everest::lib::API::V1_0::types::money
