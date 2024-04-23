#include <catch2/catch_test_macros.hpp>

#include <cbv2g/exi_v2gtp.h>

SCENARIO("V2GTP header test") {
    GIVEN("A binary representation of a valid (pre -20) V2GTP header") {
        uint8_t header[] = {0x01, 0xfe, 0x80, 0x01, 0x00, 0x00, 0x00, 0x24};

        THEN("It should be decoded succussfully") {
            uint32_t payload_len;
            auto ec = V2GTP_ReadHeader(header, &payload_len);

            REQUIRE(ec == V2GTP_ERROR__NO_ERROR);
            REQUIRE(payload_len == 0x24);
        }
    }
}
