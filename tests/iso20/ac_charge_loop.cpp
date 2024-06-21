#include <catch2/catch_test_macros.hpp>

#include <array>
#include <vector>

#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Encoder.h>

#include "test_utils/codec.hpp"

// Exi Stream: 8008041e9869d6a61dc1ef895b9b4a8062832418640096
// XML:
// <AC_ChargeLoopReq>
//     <Header>
//         <SessionID>3D30D3AD4C3B83DF</SessionID>
//         <TimeStamp>1695358101</TimeStamp>
//     </Header>
//     <MeterInfoRequested>false</MeterInfoRequested>
//     <BPT_Scheduled_AC_CLReqControlMode>
//         <EVPresentActivePower>
//             <Exponent>3</Exponent>
//             <Value>200</Value>
//         </EVPresentActivePower>
//     </BPT_Scheduled_AC_CLReqControlMode>
// </AC_ChargeLoopReq>

// Exi Stream: 800c041e9869d6a61dc1ef895b9b4a8062005900
// XML:
// <AC_ChargeLoopRes>
//     <Header>
//         <SessionID>3D30D3AD4C3B83DF</SessionID>
//         <TimeStamp>1695358101</TimeStamp>
//     </Header>
//     <ResponseCode>OK</ResponseCode>
//     <BPT_Scheduled_AC_CLResControlMode/>
// </AC_ChargeLoopRes>

static void setup_header(struct iso20_ac_MessageHeaderType* header,
                         const std::array<uint8_t, iso20_ac_sessionIDType_BYTES_SIZE>& session_id, uint64_t timestamp) {
    init_iso20_ac_MessageHeaderType(header);
    header->SessionID.bytesLen = iso20_ac_sessionIDType_BYTES_SIZE;
    std::copy(session_id.begin(), session_id.end(), header->SessionID.bytes);
    header->TimeStamp = timestamp;
}

SCENARIO("Encode and decode ISO15118-20 AC charge loop message") {

    GIVEN("Good case - Encode correct bpt control (AcChargeLoopReq)") {
        uint8_t doc_raw[] = {0x80, 0x08, 0x04, 0x1e, 0x98, 0x69, 0xd6, 0xa6, 0x1d, 0xc1, 0xef, 0x89,
                             0x5b, 0x9b, 0x4a, 0x80, 0x62, 0x83, 0x24, 0x18, 0x64, 0x00, 0x96};

        iso20_ac_exiDocument request;
        init_iso20_ac_exiDocument(&request);

        request.AC_ChargeLoopReq_isUsed = true;
        init_iso20_ac_AC_ChargeLoopReqType(&request.AC_ChargeLoopReq);

        const auto session_id =
            std::array<uint8_t, iso20_ac_sessionIDType_BYTES_SIZE>{0x3D, 0x30, 0xD3, 0xAD, 0x4C, 0x3B, 0x83, 0xDF};
        const uint64_t timestamp = 1695358101;
        setup_header(&request.AC_ChargeLoopReq.Header, session_id, timestamp);

        auto& charge_loop = request.AC_ChargeLoopReq;

        charge_loop.MeterInfoRequested = false;

        charge_loop.BPT_Scheduled_AC_CLReqControlMode_isUsed = true;
        init_iso20_ac_BPT_Scheduled_AC_CLReqControlModeType(&charge_loop.BPT_Scheduled_AC_CLReqControlMode);
        charge_loop.BPT_Scheduled_AC_CLReqControlMode.EVPresentActivePower = {3, 200};

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Encode correct bpt control (AcChargeLoopRes)") {

        uint8_t doc_raw[] = {0x80, 0x0c, 0x04, 0x1e, 0x98, 0x69, 0xd6, 0xa6, 0x1d, 0xc1,
                             0xef, 0x89, 0x5b, 0x9b, 0x4a, 0x80, 0x62, 0x00, 0x59, 0x00};

        iso20_ac_exiDocument response;
        init_iso20_ac_exiDocument(&response);

        response.AC_ChargeLoopRes_isUsed = true;
        init_iso20_ac_AC_ChargeLoopResType(&response.AC_ChargeLoopRes);

        const auto session_id =
            std::array<uint8_t, iso20_ac_sessionIDType_BYTES_SIZE>{0x3D, 0x30, 0xD3, 0xAD, 0x4C, 0x3B, 0x83, 0xDF};
        const uint64_t timestamp = 1695358101;
        setup_header(&response.AC_ChargeLoopRes.Header, session_id, timestamp);

        auto& charge_loop = response.AC_ChargeLoopRes;
        charge_loop.ResponseCode = iso20_ac_responseCodeType_OK;
        charge_loop.BPT_Scheduled_AC_CLResControlMode_isUsed = true;
        charge_loop.BPT_Scheduled_AC_CLResControlMode = {};

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(response, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode correct bpt control (AcChargeLoopReq)") {
        const auto expected_session_id = std::vector<uint8_t>{0x3D, 0x30, 0xD3, 0xAD, 0x4C, 0x3B, 0x83, 0xDF};

        uint8_t doc_raw[] = {0x80, 0x08, 0x04, 0x1e, 0x98, 0x69, 0xd6, 0xa6, 0x1d, 0xc1, 0xef, 0x89,
                             0x5b, 0x9b, 0x4a, 0x80, 0x62, 0x83, 0x24, 0x18, 0x64, 0x00, 0x96};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<iso20_ac_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            REQUIRE(request.AC_ChargeLoopReq_isUsed == true);
            // Check Header
            const auto& header = request.AC_ChargeLoopReq.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);

            REQUIRE(header.TimeStamp == 1695358101);

            // Check Body
            const auto& charge_loop = request.AC_ChargeLoopReq;
            REQUIRE(charge_loop.MeterInfoRequested == false);

            REQUIRE(charge_loop.BPT_Scheduled_AC_CLReqControlMode_isUsed == true);
            REQUIRE(charge_loop.BPT_Scheduled_AC_CLReqControlMode.EVPresentActivePower.Exponent == 3);
            REQUIRE(charge_loop.BPT_Scheduled_AC_CLReqControlMode.EVPresentActivePower.Value == 200);
        }
    }

    GIVEN("Good case - Decode correct bpt control (AcChargeLoopRes)") {

        const auto expected_session_id = std::vector<uint8_t>{0x3D, 0x30, 0xD3, 0xAD, 0x4C, 0x3B, 0x83, 0xDF};

        uint8_t doc_raw[] = {0x80, 0x0c, 0x04, 0x1e, 0x98, 0x69, 0xd6, 0xa6, 0x1d, 0xc1,
                             0xef, 0x89, 0x5b, 0x9b, 0x4a, 0x80, 0x62, 0x00, 0x59, 0x00};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<iso20_ac_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            REQUIRE(request.AC_ChargeLoopRes_isUsed == true);
            // Check Header
            const auto& header = request.AC_ChargeLoopRes.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);

            REQUIRE(header.TimeStamp == 1695358101);

            // Check Body
            const auto& charge_loop = request.AC_ChargeLoopRes;
            REQUIRE(charge_loop.ResponseCode == iso20_ac_responseCodeType_OK);
            REQUIRE(charge_loop.BPT_Scheduled_AC_CLResControlMode_isUsed == true);
        }
    }
}
