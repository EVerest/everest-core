// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <gmock/gmock.h>

#include <ocpp/v2/ocpp_enums.hpp>

MATCHER_P2(PeriodEquals, start, limit,
           "Period start " + testing::DescribeMatcher<int32_t>(start, negation) + " and limit " +
               testing::DescribeMatcher<float>(limit, negation)) {
    return ExplainMatchResult(start, arg.startPeriod, result_listener) &&
           ExplainMatchResult(limit, arg.limit, result_listener);
}

MATCHER_P3(PeriodEqualsWithPhases, start, limit, phases,
           "Period start " + testing::DescribeMatcher<int32_t>(start, negation) + " and limit " +
               testing::DescribeMatcher<float>(limit, negation) + " and phases " +
               testing::DescribeMatcher<std::optional<int32_t>>(phases, negation)) {
    return ExplainMatchResult(start, arg.startPeriod, result_listener) &&
           ExplainMatchResult(limit, arg.limit, result_listener) &&
           ExplainMatchResult(phases, arg.numberPhases, result_listener);
}