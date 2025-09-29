// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "everest_api_types/utilities/codec.hpp"
#include <gtest/gtest.h>
#include <iterator>
#include <list>

/*This function standardises test for the interactions between serialization,
 * stream serialization and deserialization of objects.
 * It verifies, that serialization via serialize function and via std::ostream yields the same result;
 */
template <class T> T codec_test(T const& original_object) {
    // basic serialization
    std::string basic_serialization_result = serialize(original_object);

    // stream serialization
    std::stringstream serialization_stream;
    serialization_stream << original_object;
    auto stream_serialization_result = serialization_stream.str();
    EXPECT_EQ(stream_serialization_result, basic_serialization_result);

    // deserialization
    T result_object;
    auto success = everest::lib::API::deserialize(basic_serialization_result, result_object);
    EXPECT_TRUE(success);

    return result_object;
}

/* Runs the default codec_test for all objects in the list
 * This is used to run tests for all fields in a typical enum
 * in namespace everest::lib::API::V1_0::types
 */
template <class T> void codec_test_all(std::initializer_list<T> const& all_values) {
    for (auto const& i : all_values) {
        SCOPED_TRACE(i);
        T result_object = codec_test(i);
        EXPECT_EQ(i, result_object);
    }
}

/*
 * Convenience function to test equality of two optionals of a base type
 */
template <class T> void expect_opt_eq(std::optional<T> const& left, std::optional<T> const& right) {
    EXPECT_EQ(left.has_value(), right.has_value());
    if (right.has_value()) {
        EXPECT_EQ(right.value(), left.value());
    }
}

/*
 * Returns a struct of type T with all its mandatory fields set to example values
 * There ought to be a concrete implementation for every struct in everest_api_types
 */
template <class T> T generate(bool set_optional_fields = true);

/*
 * Works like the GTest function EXPECT_EQ(T first, T second) but it also supports structs as inputs
 * There ought to be a concrete implementation for every struct in everest_api_types
 */
template <class T> void verify(T original, T result);

/*
 * Performs serialization tests with the given object
 */
template <class T> void test(T original) {
    auto result = codec_test(original);
    verify(original, result);
}

/*
 * Convenience function to use a default struct of type T and perform serialization tests on it
 */
template <class T> void gen_test(bool set_optional_fields = true) {
    auto original = generate<T>(set_optional_fields);
    test(original);
}
