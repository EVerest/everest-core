#include "../serialization.hpp"
#include <generated/types/serial_comm_hub_requests.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace types::serial_comm_hub_requests;
using Request = types::serial_comm_hub_requests::Result;

/// @brief Typed test
template <typename T> struct DeserializationFixture : public testing::Test {
    using Type = T;
};

using DeserializationTypes = testing::Types<uint8_t, char, uint16_t, int16_t, uint32_t, int32_t, size_t, std::string>;

TYPED_TEST_SUITE(DeserializationFixture, DeserializationTypes);
TYPED_TEST(DeserializationFixture, Invalid) {
    // Empty messages are throwing.
    ASSERT_ANY_THROW(deserialize<typename TestFixture::Type>({}));
}

using SmallTypes = testing::Types<uint8_t, uint16_t>;
template <typename T> using SmallTypesFixture = DeserializationFixture<T>;
TYPED_TEST_SUITE(SmallTypesFixture, SmallTypes);

TYPED_TEST(SmallTypesFixture, Integral) {
    using Type = typename TestFixture::Type;
    Request req{StatusCodeEnum::Success, std::vector<int>{0xdead}};
    ASSERT_EQ(deserialize<Type>(req), static_cast<Type>(0xdead));

    req.value.value().push_back(1);
    ASSERT_ANY_THROW(deserialize<Type>(req));

    req.value.value().clear();
    ASSERT_ANY_THROW(deserialize<Type>(req));
}

using MediumTypes = testing::Types<uint32_t, float>;
template <typename T> using MediumTypesFixture = DeserializationFixture<T>;
TYPED_TEST_SUITE(MediumTypesFixture, MediumTypes);

TYPED_TEST(MediumTypesFixture, MediumTypes) {

    using Type = typename TestFixture::Type;
    Request req{StatusCodeEnum::Success, std::vector<int>{0xdead, 0xbeef}};
    ASSERT_EQ(deserialize<Type>(req), static_cast<Type>(0xdeadbeef));

    req.value.value().push_back(1);
    ASSERT_ANY_THROW(deserialize<Type>(req));

    req.value.value().clear();
    ASSERT_ANY_THROW(deserialize<Type>(req));
}

TEST(Serialization, Integral) {
    ASSERT_EQ(serialize(uint8_t{0xde}).data, std::vector<int32_t>{0xde});
    ASSERT_EQ(serialize(uint16_t{0xdead}).data, std::vector<int32_t>{0xdead});
    {

        const std::vector<int32_t> expected{0xdead, 0xbeef};
        ASSERT_EQ(serialize(uint32_t{0xdeadbeef}).data, expected);
    }

    {
        const std::vector<int32_t> expected{0, 0, 0xdead, 0xbeef};
        ASSERT_EQ(serialize(size_t{0xdeadbeef}).data, expected);
    }
}

TEST(Serialization, Vector) {
    ASSERT_EQ(serialize(std::vector<uint8_t>{0xde}).data, std::vector<int32_t>{0xde});
    {
        const std::vector<int32_t> expected{0xdead, 0xbe};
        ASSERT_EQ(serialize(std::vector<uint8_t>{0xde, 0xad, 0xbe}).data, expected);
    }

    ASSERT_EQ(serialize(std::vector<uint16_t>{0xdead}).data, std::vector<int32_t>{0xdead});
    {
        const std::vector<int32_t> expected{0xdead, 0xbeef};
        ASSERT_EQ(serialize(std::vector<uint16_t>{0xdead, 0xbeef}).data, expected);
    }

    {
        const std::vector<int32_t> expected{0xdead, 0xbeef, 0, 0xacdc};
        ASSERT_EQ(serialize(std::vector<uint32_t>{0xdeadbeef, 0xacdc}).data, expected);
    }

    {
        const std::vector<int32_t> expected{0, 0, 0xdead, 0xbeef};
        ASSERT_EQ(serialize(std::vector<size_t>{0xdeadbeef}).data, expected);
    }
}