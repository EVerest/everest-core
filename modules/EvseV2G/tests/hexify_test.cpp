#include <gtest/gtest.h>
#include <tools.hpp>

#include <vector>
#include <sstream>
#include <iomanip>

namespace {

std::string ref_hex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;

    ss << std::hex << std::uppercase << std::setfill('0');

    for (size_t i = 0; i < bytes.size(); ++i) {

        if (i > 0) ss << ':';
        ss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }

    return ss.str();
}

TEST(Hexify, EmptyInput) {
    EXPECT_TRUE(hexify(nullptr, 0).empty());
    uint8_t b{};
    EXPECT_TRUE(hexify(&b, 0).empty());
}

TEST(Hexify, NullWithNonzeroLenReturnsEmpty) {
    EXPECT_TRUE(hexify(nullptr, 1).empty());
}

TEST(Hexify, SingleByteUppercaseNoColon) {
    uint8_t b0{0x00};
    uint8_t b1{0xAB};
    EXPECT_EQ(hexify(&b0, 1), "00");
    EXPECT_EQ(hexify(&b1, 1), "AB");
}

TEST(Hexify, MultipleBytesWithColonsAndLength) {
    const uint8_t v2[] = {0x01, 0x2C};
    EXPECT_EQ(hexify(v2, 2), "01:2C");

    const uint8_t v4[] = {0x00, 0x00, 0x0A, 0xFF};
    auto s = hexify(v4, 4);
    EXPECT_EQ(s, "00:00:0A:FF");
    EXPECT_EQ(s.size(), 3 * 4 - 1);
    EXPECT_EQ(std::count(s.begin(), s.end(), ':'), 3);
}

TEST(Hexify, LargerVectorMatchesReference) {
    std::vector<uint8_t> buf;
    buf.reserve(64);
    for (int i = 0; i < 64; ++i) buf.push_back(static_cast<uint8_t>(i));

    const auto got = hexify(buf.data(), buf.size());
    const auto ref = ref_hex(buf);
    EXPECT_EQ(got, ref);
}

TEST(Hexify, OverflowGuardReturnsEmptyWithoutLooping) {
    const std::size_t max = std::string{}.max_size();
    const std::size_t bound = (max / 3) + ((max % 3) == 2);

    // Provide a valid pointer; function should return before using it.
    uint8_t dummy = 0xAA;

    if (bound < std::numeric_limits<std::size_t>::max()) {
        const std::size_t too_big = bound + 1;
        const auto s = hexify(&dummy, too_big);
        EXPECT_TRUE(s.empty());
    } else {
        GTEST_SKIP() << "Platform size_t cannot represent bound+1; skipping overflow case.";
    }
}

} // namespace
