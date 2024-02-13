#include <catch2/catch_test_macros.hpp>

#include <string>

#include <cbv2g/app_handshake/appHand_Datatypes.h>
#include <cbv2g/app_handshake/appHand_Decoder.h>

SCENARIO("Encode and decode app protocol request messages") {

    GIVEN("Decode an AppProtocolReq document") {

        uint8_t doc_raw[] = "\x80"
                            "\x00\xf3\xab\x93\x71\xd3\x4b\x9b\x79\xd3\x9b\xa3\x21\xd3\x4b\x9b\x79"
                            "\xd1\x89\xa9\x89\x89\xc1\xd1\x69\x91\x81\xd2\x0a\x18\x01\x00\x00\x04"
                            "\x00\x40";

        exi_bitstream_t exi_stream_in;
        size_t pos1 = 0;
        int errn = 0;

        exi_bitstream_init(&exi_stream_in, reinterpret_cast<uint8_t*>(doc_raw), sizeof(doc_raw), pos1, nullptr);

        THEN("It should be decoded succussfully") {
            appHand_exiDocument request;

            REQUIRE(decode_appHand_exiDocument(&exi_stream_in, &request) == 0);
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

        uint8_t doc_raw[] = "\x80\x40\x00\x40";

        exi_bitstream_t exi_stream_in;
        size_t pos1 = 0;
        int errn = 0;

        exi_bitstream_init(&exi_stream_in, reinterpret_cast<uint8_t*>(doc_raw), sizeof(doc_raw), pos1, nullptr);

        THEN("It should be decoded succussfully") {
            appHand_exiDocument response;

            REQUIRE(decode_appHand_exiDocument(&exi_stream_in, &response) == 0);
            REQUIRE(response.supportedAppProtocolRes_isUsed == 1);

            REQUIRE(response.supportedAppProtocolRes.ResponseCode == appHand_responseCodeType_OK_SuccessfulNegotiation);
            REQUIRE(response.supportedAppProtocolRes.SchemaID_isUsed == true);
            REQUIRE(response.supportedAppProtocolRes.SchemaID == 1);
        }
    }
}