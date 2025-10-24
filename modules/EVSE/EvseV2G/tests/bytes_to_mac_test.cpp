#include <gtest/gtest.h>
#include <tools.hpp>

namespace {

TEST(FormatMacAddress, FullSixByteAddress) {
    const uint8_t mac[6] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};
    EXPECT_EQ(to_mac_string(mac, 6, '?'), "01:23:45:67:89:AB");
}

TEST(FormatMacAddress, PartialThreeBytesWithFill) {
    const uint8_t mac[3] = {0xDE, 0xAD, 0xBE};
    EXPECT_EQ(to_mac_string(mac, 3, '?'), "DE:AD:BE:??:??:??");
}

TEST(FormatMacAddress, PartialThreeBytesEarlyTerminate) {
    const uint8_t mac[3] = {0xDE, 0xAD, 0xBE};
    EXPECT_EQ(to_mac_string(mac, 3, '\0'), "DE:AD:BE");
}

TEST(FormatMacAddress, NoDataWithFill) {
    const uint8_t dummy[6] = {0};
    EXPECT_EQ(to_mac_string(dummy, 0, 'X'), "XX:XX:XX:XX:XX:XX");
}

TEST(FormatMacAddress, NoDataEarlyTerminate) {
    const uint8_t dummy[6] = {0};
    EXPECT_TRUE(to_mac_string(dummy, 0, '\0').empty());
}

TEST(FormatMacAddress, NullDataPointer) {
    EXPECT_EQ(to_mac_string(nullptr, 0, '?'), "??:??:??:??:??:??");
    EXPECT_TRUE(to_mac_string(nullptr, 0, '\0').empty());
}

TEST(FormatMacAddress, IgnoreExtraBytesBeyondSix) {
    const uint8_t mac[8] = {0xAA, 0xBB, 0xCC, 0x00, 0x11, 0x22, 0x33, 0x44};
    EXPECT_EQ(to_mac_string(mac, 8, '?'), "AA:BB:CC:00:11:22");
}

TEST(FormatMacAddress, UppercaseHexAndProperDelimiters) {
    const uint8_t mac[6] = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    EXPECT_EQ(to_mac_string(mac, 6, '?'), "0A:0B:0C:0D:0E:0F");
}

TEST(FormatMacAddress, PartialProgressiveFill) {
    const uint8_t mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    EXPECT_EQ(to_mac_string(mac, 1, '?'), "AA:??:??:??:??:??");
    EXPECT_EQ(to_mac_string(mac, 2, '?'), "AA:BB:??:??:??:??");
    EXPECT_EQ(to_mac_string(mac, 5, '?'), "AA:BB:CC:DD:EE:??");
}

TEST(FormatMacAddress, PartialProgressiveEarlyTerminate) {
    const uint8_t mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    EXPECT_EQ(to_mac_string(mac, 1, '\0'), "AA");
    EXPECT_EQ(to_mac_string(mac, 2, '\0'), "AA:BB");
    EXPECT_EQ(to_mac_string(mac, 5, '\0'), "AA:BB:CC:DD:EE");
}

} // namespace
