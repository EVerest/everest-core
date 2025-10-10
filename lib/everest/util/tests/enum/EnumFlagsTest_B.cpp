// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <everest/util/enum/EnumFlags.hpp>

namespace {
using namespace everest::lib::util;

// needs an 8-bit value
enum class small : std::uint8_t {
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    last = seven,
};

// needs an 8-bit value
enum class full : std::uint8_t {
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    last = eight,
};

// needs an 16-bit value
enum class large : std::uint8_t {
    zero,
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    last = eight,
};

static_assert(sizeof(full) == sizeof(std::uint8_t));
static_assert(sizeof(SelectedUInt<full>) == sizeof(std::uint8_t));

static_assert(sizeof(large) == sizeof(std::uint8_t));
static_assert(sizeof(SelectedUInt<large>) == sizeof(std::uint16_t));

TEST(EnumFlags, InitFull) {
    EnumFlags<full> flags;

    EXPECT_EQ(flags.get(), 0);
    EXPECT_TRUE(flags.all_reset());
    EXPECT_FALSE(flags.any_set());
    EXPECT_FALSE(flags.all_set());

    EXPECT_FALSE(flags.is_set(full::one));
    EXPECT_FALSE(flags.is_set(full::two));
    EXPECT_FALSE(flags.is_set(full::three));
    EXPECT_FALSE(flags.is_set(full::four));
    EXPECT_FALSE(flags.is_set(full::five));
    EXPECT_FALSE(flags.is_set(full::six));
    EXPECT_FALSE(flags.is_set(full::seven));
    EXPECT_FALSE(flags.is_set(full::eight));

    EXPECT_TRUE(flags.is_reset(full::one));
    EXPECT_TRUE(flags.is_reset(full::two));
    EXPECT_TRUE(flags.is_reset(full::three));
    EXPECT_TRUE(flags.is_reset(full::four));
    EXPECT_TRUE(flags.is_reset(full::five));
    EXPECT_TRUE(flags.is_reset(full::six));
    EXPECT_TRUE(flags.is_reset(full::seven));
    EXPECT_TRUE(flags.is_reset(full::eight));

    flags.set(full::one);
    EXPECT_EQ(flags.get(), 1);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_FALSE(flags.all_set());

    EXPECT_TRUE(flags.is_set(full::one));
    EXPECT_FALSE(flags.is_set(full::two));
    EXPECT_FALSE(flags.is_set(full::three));
    EXPECT_FALSE(flags.is_set(full::four));
    EXPECT_FALSE(flags.is_set(full::five));
    EXPECT_FALSE(flags.is_set(full::six));
    EXPECT_FALSE(flags.is_set(full::seven));
    EXPECT_FALSE(flags.is_set(full::eight));

    EXPECT_FALSE(flags.is_reset(full::one));
    EXPECT_TRUE(flags.is_reset(full::two));
    EXPECT_TRUE(flags.is_reset(full::three));
    EXPECT_TRUE(flags.is_reset(full::four));
    EXPECT_TRUE(flags.is_reset(full::five));
    EXPECT_TRUE(flags.is_reset(full::six));
    EXPECT_TRUE(flags.is_reset(full::seven));
    EXPECT_TRUE(flags.is_reset(full::eight));

    flags.set(full::two, full::three, full::four);
    EXPECT_EQ(flags.get(), 0b1111);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_FALSE(flags.all_set());

    EXPECT_TRUE(flags.is_set(full::one));
    EXPECT_TRUE(flags.is_set(full::two));
    EXPECT_TRUE(flags.is_set(full::three));
    EXPECT_TRUE(flags.is_set(full::four));
    EXPECT_FALSE(flags.is_set(full::five));
    EXPECT_FALSE(flags.is_set(full::six));
    EXPECT_FALSE(flags.is_set(full::seven));
    EXPECT_FALSE(flags.is_set(full::eight));

    EXPECT_FALSE(flags.is_reset(full::one));
    EXPECT_FALSE(flags.is_reset(full::two));
    EXPECT_FALSE(flags.is_reset(full::three));
    EXPECT_FALSE(flags.is_reset(full::four));
    EXPECT_TRUE(flags.is_reset(full::five));
    EXPECT_TRUE(flags.is_reset(full::six));
    EXPECT_TRUE(flags.is_reset(full::seven));
    EXPECT_TRUE(flags.is_reset(full::eight));

    flags.set(full::five, full::six, full::seven, full::eight);
    EXPECT_EQ(flags.get(), 0xff);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_TRUE(flags.all_set());

    EXPECT_TRUE(flags.is_set(full::one));
    EXPECT_TRUE(flags.is_set(full::two));
    EXPECT_TRUE(flags.is_set(full::three));
    EXPECT_TRUE(flags.is_set(full::four));
    EXPECT_TRUE(flags.is_set(full::five));
    EXPECT_TRUE(flags.is_set(full::six));
    EXPECT_TRUE(flags.is_set(full::seven));
    EXPECT_TRUE(flags.is_set(full::eight));

    EXPECT_FALSE(flags.is_reset(full::one));
    EXPECT_FALSE(flags.is_reset(full::two));
    EXPECT_FALSE(flags.is_reset(full::three));
    EXPECT_FALSE(flags.is_reset(full::four));
    EXPECT_FALSE(flags.is_reset(full::five));
    EXPECT_FALSE(flags.is_reset(full::six));
    EXPECT_FALSE(flags.is_reset(full::seven));
    EXPECT_FALSE(flags.is_reset(full::eight));

    flags.reset(full::one, full::eight);
    EXPECT_EQ(flags.get(), 0b01111110);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_FALSE(flags.all_set());
    EXPECT_FALSE(flags.is_set(full::one, full::eight));
    EXPECT_FALSE(flags.is_set(full::one, full::eight, full::five));
    EXPECT_TRUE(flags.is_set(full::two, full::five, full::seven));

    flags.set(0xfe);
    EXPECT_EQ(flags.get(), 0b11111110);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_FALSE(flags.all_set());

    flags.set(full::one);
    EXPECT_EQ(flags.get(), 0b11111111);
    EXPECT_FALSE(flags.all_reset());
    EXPECT_TRUE(flags.any_set());
    EXPECT_TRUE(flags.all_set());
}

TEST(EnumFlags, Set) {
    EnumFlags<small> sflags;
    EXPECT_TRUE(sflags.all_reset());
    sflags.set();
    EXPECT_TRUE(sflags.all_set());
    EXPECT_EQ(sflags.get(), 0b01111111);

    EnumFlags<full> flags;
    EXPECT_TRUE(flags.all_reset());
    EXPECT_FALSE(flags.any_set());

    flags.set(full::one);
    EXPECT_EQ(flags.get(), 0b1);

    flags.reset();
    flags.set(full::one, full::two);
    EXPECT_EQ(flags.get(), 0b11);

    flags.reset();
    flags.set(full::one, full::two, full::three);
    EXPECT_EQ(flags.get(), 0b111);

    flags.reset();
    flags.set(full::one, full::two, full::three, full::four);
    EXPECT_EQ(flags.get(), 0b1111);
}

TEST(EnumFlags, Reset) {
    EnumFlags<full> flags;
    flags.set();
    EXPECT_TRUE(flags.all_set());
    EXPECT_FALSE(flags.any_reset());

    flags.reset(full::one);
    EXPECT_EQ(flags.get(), 0b11111110);

    flags.set();
    flags.reset(full::one, full::two);
    EXPECT_EQ(flags.get(), 0b11111100);

    flags.set();
    flags.reset(full::one, full::two, full::three);
    EXPECT_EQ(flags.get(), 0b11111000);

    flags.set();
    flags.reset(full::one, full::two, full::three, full::four);
    EXPECT_EQ(flags.get(), 0b11110000);
}

TEST(EnumFlags, AnySet) {
    EnumFlags<full> flags;
    flags.set(0x7e);
    EXPECT_EQ(flags.get(), 0b01111110);

    EXPECT_TRUE(flags.is_set(full::two));
    EXPECT_FALSE(flags.is_set(full::one, full::two));
    EXPECT_FALSE(flags.is_set(full::two, full::one));
    EXPECT_FALSE(flags.is_set(full::one, full::two, full::three));
    EXPECT_FALSE(flags.is_set(full::three, full::two, full::one));

    EXPECT_TRUE(flags.is_any_set(full::one, full::two));
    EXPECT_TRUE(flags.is_any_set(full::two, full::one));
    EXPECT_TRUE(flags.is_any_set(full::one, full::two, full::three));
    EXPECT_TRUE(flags.is_any_set(full::one, full::three, full::two));
    EXPECT_TRUE(flags.is_any_set(full::three, full::two, full::one));

    EXPECT_FALSE(flags.is_any_set(full::one, full::eight));
    EXPECT_TRUE(flags.is_any_set(full::eight, full::two, full::one));
}

TEST(EnumFlags, AnyReSet) {
    EnumFlags<full> flags;
    flags.set(0x7e);
    EXPECT_EQ(flags.get(), 0b01111110);

    EXPECT_TRUE(flags.is_reset(full::one));
    EXPECT_TRUE(flags.is_set(full::two));
    EXPECT_FALSE(flags.is_reset(full::one, full::two));
    EXPECT_FALSE(flags.is_reset(full::two, full::one));
    EXPECT_FALSE(flags.is_reset(full::one, full::two, full::three));
    EXPECT_FALSE(flags.is_reset(full::three, full::two, full::one));

    EXPECT_TRUE(flags.is_any_reset(full::one, full::two));
    EXPECT_TRUE(flags.is_any_reset(full::two, full::one));
    EXPECT_TRUE(flags.is_any_reset(full::one, full::two, full::three));
    EXPECT_TRUE(flags.is_any_reset(full::three, full::two, full::one));
}

} // namespace
