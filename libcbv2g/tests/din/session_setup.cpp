#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>

#include "test_utils/codec.hpp"

SCENARIO("Encode and decode DIN70121 session setup message") {

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

        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x11, 0xd0, 0x1a, 0x12, 0x1d, 0xc9, 0x83, 0xcd, 0x60, 0x00};

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
        const auto evccid = std::array<uint8_t, 8>{0x84, 0x87, 0x72, 0x60, 0xF3, 0x58, 0x00, 0x00};
        std::copy(evccid.begin(), evccid.end(), body.SessionSetupReq.EVCCID.bytes);
        body.SessionSetupReq.EVCCID.bytesLen = 6;

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode an SessionSetupReq document") {

        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x11, 0xd0, 0x1a, 0x12, 0x1d, 0xc9, 0x83, 0xcd, 0x60, 0x00};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<din_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

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

    // EXI stream: 809a0211d63f74d2297ac911e0201526a2698d8017da353360c0
    // XML:
    // <ns7:V2G_Message>
    //     <ns7:Header>
    //         <ns8:SessionID>4758FDD348A5EB24</ns8:SessionID>
    //     </ns7:Header>
    //     <ns7:Body>
    //         <ns5:SessionSetupRes>
    //             <ns5:ResponseCode>OK_NewSessionEstablished</ns5:ResponseCode>
    //             <ns5:EVSEID>49A89A6360</ns5:EVSEID>
    //             <ns5:DateTimeNow>1667918014</ns5:EVSETimeStamp>
    //         </ns5:SessionSetupRes>
    //     </ns7:Body>
    // </ns7:V2G_Message>

    GIVEN("Good case - Encode an SessionSetupRes document") {
        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a, 0xc9, 0x11, 0xe0,
                             0x20, 0x15, 0x26, 0xa2, 0x69, 0x8d, 0x80, 0x17, 0xda, 0x35, 0x33, 0x60, 0xc0};

        din_exiDocument request;
        init_din_exiDocument(&request);

        init_din_MessageHeaderType(&request.V2G_Message.Header);

        auto& header = request.V2G_Message.Header;
        header.SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;
        const auto session_id = std::array<uint8_t, 8>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        std::copy(session_id.begin(), session_id.end(), header.SessionID.bytes);

        init_din_BodyType(&request.V2G_Message.Body);
        auto& body = request.V2G_Message.Body;
        init_din_SessionSetupResType(&body.SessionSetupRes);
        body.SessionSetupRes_isUsed = true;

        // set the response code
        body.SessionSetupRes.ResponseCode = din_responseCodeType_OK_NewSessionEstablished;

        // set the EVSE ID
        const auto evse_id = std::array<uint8_t, 5>{0x49, 0xA8, 0x9A, 0x63, 0x60};
        std::copy(evse_id.begin(), evse_id.end(), body.SessionSetupRes.EVSEID.bytes);
        body.SessionSetupRes.EVSEID.bytesLen = evse_id.size();

        // set the EVSE timestamp
        body.SessionSetupRes.DateTimeNow_isUsed = true;
        body.SessionSetupRes.DateTimeNow = 1667918014;

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode an SessionSetupRes document") {

        const auto expected_session_id = std::vector<uint8_t>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a, 0xc9, 0x11, 0xe0,
                             0x20, 0x15, 0x26, 0xa2, 0x69, 0x8d, 0x80, 0x17, 0xda, 0x35, 0x33, 0x60, 0xc0};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<din_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            // Check Header
            auto& header = request.V2G_Message.Header;

            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);
            REQUIRE(header.Notification_isUsed == false);
            REQUIRE(header.Signature_isUsed == false);

            // Check Body
            REQUIRE(request.V2G_Message.Body.SessionSetupRes_isUsed == true);
            const auto& session_setup_res = request.V2G_Message.Body.SessionSetupRes;

            // check the response code
            REQUIRE(session_setup_res.ResponseCode == din_responseCodeType_OK_NewSessionEstablished);

            // check the EVSE ID
            REQUIRE(session_setup_res.EVSEID.bytesLen == 5);
            const auto evse_id =
                std::vector<uint8_t>(std::begin(session_setup_res.EVSEID.bytes),
                                     std::begin(session_setup_res.EVSEID.bytes) + session_setup_res.EVSEID.bytesLen);
            REQUIRE(evse_id == std::vector<uint8_t>({0x49, 0xA8, 0x9A, 0x63, 0x60}));

            // check the EVSE timestamp
            REQUIRE(session_setup_res.DateTimeNow_isUsed == true);
            REQUIRE(session_setup_res.DateTimeNow == 1667918014);
        }
    }
}
