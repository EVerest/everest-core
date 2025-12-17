#include <catch2/catch_test_macros.hpp>

#include <string>

#include <cbv2g/app_handshake/appHand_Datatypes.h>
#include <cbv2g/app_handshake/appHand_Decoder.h>

#include "test_utils/codec.hpp"

SCENARIO("Encode and decode app protocol request messages") {

    GIVEN("Decode an AppProtocolReq document") {

        uint8_t doc_raw[] = {0x80, 0x00, 0xf3, 0xab, 0x93, 0x71, 0xd3, 0x4b, 0x9b, 0x79, 0xd3, 0x9b, 0xa3,
                             0x21, 0xd3, 0x4b, 0x9b, 0x79, 0xd1, 0x89, 0xa9, 0x89, 0x89, 0xc1, 0xd1, 0x69,
                             0x91, 0x81, 0xd2, 0x0a, 0x18, 0x01, 0x00, 0x00, 0x04, 0x00, 0x40};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<appHand_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            REQUIRE(request.supportedAppProtocolReq_isUsed == 1);

            REQUIRE(request.supportedAppProtocolReq.AppProtocol.arrayLen == 1);

            const auto& ap = request.supportedAppProtocolReq.AppProtocol.array[0];

            REQUIRE(ap.VersionNumberMajor == 1);
            REQUIRE(ap.VersionNumberMinor == 0);
            REQUIRE(ap.SchemaID == 1);
            REQUIRE(ap.Priority == 1);

            const auto protocol_namespace = std::string(ap.ProtocolNamespace.characters);
            REQUIRE(protocol_namespace == "urn:iso:std:iso:15118:-20:AC");
        }
    }
}

SCENARIO("Encode and decode app protocol response messages") {

    GIVEN("Decode an AppProtocolRes document") {

        uint8_t doc_raw[] = {0x80, 0x40, 0x00, 0x40};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<appHand_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& response = result.value;

            REQUIRE(response.supportedAppProtocolRes_isUsed == 1);

            REQUIRE(response.supportedAppProtocolRes.ResponseCode == appHand_responseCodeType_OK_SuccessfulNegotiation);
            REQUIRE(response.supportedAppProtocolRes.SchemaID_isUsed == true);
            REQUIRE(response.supportedAppProtocolRes.SchemaID == 1);
        }
    }
}