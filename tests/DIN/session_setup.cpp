#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include <array>
#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>

SCENARIO("Encode and decode DIN session setup message") {

    // Exi Stream: 809a02000000000000000011d01a121dc983cd6000
    // XML:
    // <ns7:V2G_Message>
    //      <ns7:Header>
    //          <ns8:SessionID>0000000000000000</ns8:SessionID>
    //      </ns7:Header>
    //      <ns7:Body>
    //          <ns5:SessionSetupReq>
    //              <ns5:EVCCID>84877260F358</ns5:EVCCID>
    //          </ns5:SessionSetupReq>
    //      </ns7:Body>
    // </ns7:V2G_Message>

    GIVEN("Good case - Encode an SessionSetupReq document") {

        uint8_t doc_raw[] = "\x80"
                            "\x9a\x02\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x11\xd0\x1a\x12\x1d\xc9\x83\xcd\x60\x00";

        din_exiDocument request;
        init_din_exiDocument(&request);

        init_din_MessageHeaderType(&request.V2G_Message.Header);

        auto& header = request.V2G_Message.Header;
        header.SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;
        const auto session_id = std::vector<uint8_t>(8, 0);
        std::copy(session_id.begin(), session_id.end(), header.SessionID.bytes);

        init_din_BodyType(&request.V2G_Message.Body);
        auto& body = request.V2G_Message.Body;
        init_din_SessionSetupReqType(&body.SessionSetupReq);
        body.SessionSetupReq_isUsed = true;
        const auto evccid = std::array<uint8_t, 8> {0x84, 0x87, 0x72, 0x60, 0xF3, 0x58, 0x00, 0x00};
        std::copy(evccid.begin(), evccid.end(), body.SessionSetupReq.EVCCID.bytes);
        body.SessionSetupReq.EVCCID.bytesLen = 6;

        uint8_t stream[256] = {};
        exi_bitstream_t exi_stream_in;
        size_t pos1 = 0;
        int errn = 0;

        exi_bitstream_init(&exi_stream_in, stream, sizeof(stream), pos1, nullptr);

        THEN("It should be encoded succussfully") {

            REQUIRE(encode_din_exiDocument(&exi_stream_in, &request) == 0);

            const auto encoded_stream =
                std::vector<uint8_t>(stream, stream + exi_bitstream_get_length(&exi_stream_in) + 1);

            const auto expected_exi_stream = std::vector<uint8_t>(std::begin(doc_raw), std::end(doc_raw));

            REQUIRE(encoded_stream == expected_exi_stream);
        }
    }

    GIVEN("Good case - Decode an SessionSetupReq document") {

        uint8_t doc_raw[] = "\x80"
                            "\x9a\x02\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x11\xd0\x1a\x12\x1d\xc9\x83\xcd\x60\x00";
        exi_bitstream_t exi_stream_in;
        size_t pos1 = 0;
        int errn = 0;

        exi_bitstream_init(&exi_stream_in, reinterpret_cast<uint8_t*>(doc_raw), sizeof(doc_raw), pos1, nullptr);

        THEN("It should be decoded succussfully") {
            din_exiDocument request;

            REQUIRE(decode_din_exiDocument(&exi_stream_in, &request) == 0);

            // Check Header
            auto& header = request.V2G_Message.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == std::vector<uint8_t>({0, 0, 0, 0, 0, 0, 0, 0}));
            REQUIRE(header.Notification_isUsed == false);
            REQUIRE(header.Signature_isUsed == false);

            // Check Body
            REQUIRE(request.V2G_Message.Body.SessionSetupReq_isUsed == true);

            auto& session_setup_req = request.V2G_Message.Body.SessionSetupReq;

            const auto evccid = std::vector<uint8_t>(std::begin(session_setup_req.EVCCID.bytes),
                                                     std::end(session_setup_req.EVCCID.bytes));
            REQUIRE(evccid == std::vector<uint8_t>({0x84, 0x87, 0x72, 0x60, 0xF3, 0x58, 0x00, 0x00}));

            REQUIRE(session_setup_req.EVCCID.bytesLen == 6);
        }
    }

    // EXI stream: 80980214b22ca1afff8d8b51e020451114a9413960a914c4c8ccd0d4a8c412d71bd3c0c0
    // XML:
    // <ns7:V2G_Message>
    //     <ns7:Header>
    //         <ns8:SessionID>52C8B286BFFE362D</ns8:SessionID>
    //     </ns7:Header>
    //     <ns7:Body>
    //         <ns5:SessionSetupRes>
    //             <ns5:ResponseCode>OK_NewSessionEstablished</ns5:ResponseCode>
    //             <ns5:EVSEID>DE*PNX*E12345*1</ns5:EVSEID>
    //             <ns5:EVSETimeStamp>1675074582</ns5:EVSETimeStamp>
    //         </ns5:SessionSetupRes>
    //     </ns7:Body>
    // </ns7:V2G_Message>

// TODO: These tests should work but at the moment the exi stream is broken.
//
//    GIVEN("Good case - Encode an SessionSetupRes document") {
//        uint8_t doc_raw[] = "\x80\x98\x02\x14\xb2\x2c\xa1\xaf\xff\x8d\x8b\x51\xe0\x20\x45\x11\x14\xa9\x41\x39\x60\xa9"
//                            "\x14\xc4\xc8\xcc\xd0\xd4\xa8\xc4\x12\xd7\x1b\xd3\xc0\xc0";
//
//        din_exiDocument request;
//        init_din_exiDocument(&request);
//
//        init_din_MessageHeaderType(&request.V2G_Message.Header);
//
//        auto& header = request.V2G_Message.Header;
//        header.SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;
//        const auto session_id = std::array<uint8_t, 8>{0x52, 0xC8, 0xB2, 0x86, 0xBF, 0xFE, 0x36, 0x2D};
//        std::copy(session_id.begin(), session_id.end(), header.SessionID.bytes);
//
//        init_din_BodyType(&request.V2G_Message.Body);
//        auto& body = request.V2G_Message.Body;
//        init_din_SessionSetupResType(&body.SessionSetupRes);
//        body.SessionSetupRes_isUsed = true;
//
//        // set the response code
//        body.SessionSetupRes.ResponseCode = din_responseCodeType_OK_NewSessionEstablished;
//
//        // set the EVSE ID
//        const auto evse_id =
//            std::array<uint8_t, 15>{'D', 'E', '*', 'P', 'N', 'X', '*', 'E', '1', '2', '3', '4', '5', '*', '1'};
//        std::copy(evse_id.begin(), evse_id.end(), body.SessionSetupRes.EVSEID.bytes);
//        body.SessionSetupRes.EVSEID.bytesLen = evse_id.size();
//
//        // set the EVSE timestamp
//        body.SessionSetupRes.DateTimeNow_isUsed = true;
//        body.SessionSetupRes.DateTimeNow = 1675074582;
//
//        uint8_t stream[256] = {};
//        exi_bitstream_t exi_stream_in;
//        size_t pos1 = 0;
//        int errn = 0;
//
//        exi_bitstream_init(&exi_stream_in, stream, sizeof(stream), pos1, nullptr);
//
//        THEN("It should be encoded succussfully") {
//
//            REQUIRE(encode_din_exiDocument(&exi_stream_in, &request) == 0);
//
//            const auto encoded_stream =
//                std::vector<uint8_t>(stream, stream + exi_bitstream_get_length(&exi_stream_in) + 1);
//
//            const auto expected_exi_stream = std::vector<uint8_t>(std::begin(doc_raw), std::end(doc_raw));
//
//            //            REQUIRE(encoded_stream == expected_exi_stream);
//        }
//    }
//
//    GIVEN("Good case - Decode an SessionSetupRes document") {
//
//        uint8_t doc_raw[] = "\x80\x98\x02\x14\xb2\x2c\xa1\xaf\xff\x8d\x8b\x51\xe0\x20\x45\x11\x14\xa9\x41\x39\x60\xa9"
//                            "\x14\xc4\xc8\xcc\xd0\xd4\xa8\xc4\x12\xd7\x1b\xd3\xc0\xc0";
//
//        exi_bitstream_t exi_stream_in;
//        size_t pos1 = 0;
//        int errn = 0;
//
//        exi_bitstream_init(&exi_stream_in, reinterpret_cast<uint8_t*>(doc_raw), sizeof(doc_raw), pos1, nullptr);
//
//        THEN("It should be decoded succussfully") {
//            din_exiDocument request;
//
//            REQUIRE(decode_din_exiDocument(&exi_stream_in, &request) == 0);
//
//            // Check Header
//            auto& header = request.V2G_Message.Header;
//            const auto expected_session_id = std::vector<uint8_t>{0x52, 0xC8, 0xB2, 0x86, 0xBF, 0xFE, 0x36, 0x2D};
//            const auto session_id =
//                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
//            REQUIRE(session_id == expected_session_id);
//            REQUIRE(header.Notification_isUsed == false);
//            REQUIRE(header.Signature_isUsed == false);
//
//            // Check Body
//            REQUIRE(request.V2G_Message.Body.SessionSetupRes_isUsed == true);
//
//            auto& session_setup_res = request.V2G_Message.Body.SessionSetupRes;
//
//            // check the response code
//            REQUIRE(session_setup_res.ResponseCode == din_responseCodeType_OK_NewSessionEstablished);
//
//            // check the EVSE ID
//            const auto evse_id = std::vector<uint8_t>(std::begin(session_setup_res.EVSEID.bytes),
//                                                      std::end(session_setup_res.EVSEID.bytes));
//            REQUIRE(evse_id ==
//                    std::vector<uint8_t>({'D', 'E', '*', 'P', 'N', 'X', '*', 'E', '1', '2', '3', '4', '5', '*', '1'}));
//            REQUIRE(session_setup_res.EVSEID.bytesLen == 15);
//
//            // check the EVSE timestamp
//            REQUIRE(session_setup_res.DateTimeNow_isUsed == true);
//            REQUIRE(session_setup_res.DateTimeNow == 1675074582);
//        }
//    }
}
