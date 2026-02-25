#include <everest/util/math/comparison_original.hpp>
#include <gtest/gtest.h>

namespace everest::lib::util {

class ComparisonTest : public ::testing::Test {};

// --- Floating Point & almost_eq Tests ---

TEST_F(ComparisonTest, RangeLimit) {
    EXPECT_NEAR(range_limit<double>(1), 0.1, 1e-9);
    EXPECT_NEAR(range_limit<double>(3), 0.001, 1e-9);
    EXPECT_EQ(range_limit<double>(0), 1.0);
}

TEST_F(ComparisonTest, AlmostEqBasic) {
    // 3 digits of precision = 0.001 threshold
    EXPECT_TRUE((almost_eq<3>(1.0001, 1.0002)));
    EXPECT_FALSE((almost_eq<3>(1.0, 1.002)));
}

TEST_F(ComparisonTest, AlmostEqOptional) {
    std::optional<double> a = 1.0001;
    std::optional<double> b = 1.0002;
    std::optional<double> empty;

    EXPECT_TRUE(almost_eq<3>(a, b));
    EXPECT_TRUE(almost_eq<3>(empty, empty));
    EXPECT_FALSE(almost_eq<3>(a, empty));
}

// --- Min/Max Optional Tests ---

TEST_F(ComparisonTest, MinOptional) {
    std::optional<float> low = 10.0f;
    std::optional<float> high = 20.0f;
    std::optional<float> empty;

    // Optional & Optional
    EXPECT_EQ(min_optional(low, high).value(), 10.0f);
    EXPECT_EQ(min_optional(low, empty).value(), 10.0f);
    EXPECT_FALSE(min_optional(empty, empty).has_value());

    // Value & Optional
    EXPECT_EQ(min_optional(15.0f, high), 15.0f);
    EXPECT_EQ(min_optional(25.0f, high), 20.0f);
    EXPECT_EQ(min_optional(25.0f, empty), 25.0f);
}

TEST_F(ComparisonTest, MaxOptional) {
    std::optional<float> low = 10.0f;
    std::optional<float> empty;

    EXPECT_EQ(max_optional(low, 5.0f), 10.0f);
    EXPECT_EQ(max_optional(low, 15.0f), 15.0f);
    EXPECT_EQ(max_optional(empty, 15.0f), 15.0f);
}

// --- Clamping Tests ---

TEST_F(ComparisonTest, ClampOptional) {
    std::optional<double> min_limit = 10.0;
    std::optional<double> max_limit = 20.0;
    std::optional<double> no_limit;

    // Inside range
    EXPECT_DOUBLE_EQ(clamp_optional(15.0, min_limit, max_limit), 15.0);

    // Underflow
    EXPECT_DOUBLE_EQ(clamp_optional(5.0, min_limit, max_limit), 10.0);

    // Overflow
    EXPECT_DOUBLE_EQ(clamp_optional(25.0, min_limit, max_limit), 20.0);

    // One-sided clamping
    EXPECT_DOUBLE_EQ(clamp_optional(5.0, no_limit, max_limit), 5.0);
    EXPECT_DOUBLE_EQ(clamp_optional(25.0, min_limit, no_limit), 25.0);

    // No limits
    EXPECT_DOUBLE_EQ(clamp_optional(100.0, no_limit, no_limit), 100.0);
}

// --- Noise Range Tests ---

TEST_F(ComparisonTest, InNoiseRange) {
    EXPECT_TRUE(in_noise_range(10.0, 10.05, 0.1));
    EXPECT_FALSE(in_noise_range(10.0, 10.11, 0.1));
    // Exact boundary
    EXPECT_TRUE(in_noise_range(10.0, 10.1, 0.1));
}

} // namespace everest::lib::util
