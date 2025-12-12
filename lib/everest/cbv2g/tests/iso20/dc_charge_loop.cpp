#include <catch2/catch_test_macros.hpp>

#include <array>
#include <vector>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

#include "test_utils/codec.hpp"

// Exi Stream: 8034042d166f29fb80ea560aebdbfb3062810012006164000a02002400c800
// XML:
// <DC_ChargeLoopReq>
//     <Header>
//         <SessionID>5A2CDE53F701D4AC</SessionID>
//         <TimeStamp>1718607534</TimeStamp>
//     </Header>
//     <MeterInfoRequested>false</MeterInfoRequested>
//     <EVPresentVoltage>
//         <Exponent>0</Exponent>
//         <Value>400</Value>
//     </EVPresentVoltage>
//     <BPT_Scheduled_DC_CLReqControlMode>
//         <EVTargetCurrent>
//             <Exponent>0</Exponent>
//             <Value>20</Value>
//         </EVTargetCurrent>
//         <EVTargetVoltage>
//             <Exponent>0</Exponent>
//             <Value>400</Value>
//         </EVTargetVoltage>
//     </BPT_Scheduled_DC_CLReqControlMode>
// </DC_ChargeLoopReq>

// Exi Stream: 8038042d166f29fb80ea560b7bdbfb30620063f0680781fc2807c22230
// XML:
// <DC_ChargeLoopRes>
//     <Header>
//         <SessionID>5A2CDE53F701D4AC</SessionID>
//         <TimeStamp>1718607543</TimeStamp>
//     </Header>
//     <ResponseCode>OK</ ResponseCode>
//     <EVSEPresentCurrent>
//         <Exponent>-2</Exponent>
//         <Value>2000</Value>
//     </EVSEPresentCurrent>
//     <EVSEPresentVoltage>
//         <Exponent>-1</Exponent >
//         <Value>4000</Value >
//     </EVSEPresentVoltage>
//     <EVSEPowerLimitAchieved>true</EVSEPowerLimitAchieved>
//     <EVSECurrentLimitAchieved>true</EVSECurrentLimitAchieved>
//     <EVSEVoltageLimitAchieved>true</EVSEVoltageLimitAchieved>
//     <BPT_Scheduled_DC_CLResControlMode/>
// </ DC_ChargeLoopRes>

static void setup_header(struct iso20_dc_MessageHeaderType* header,
                         const std::array<uint8_t, iso20_dc_sessionIDType_BYTES_SIZE>& session_id, uint64_t timestamp) {
    init_iso20_dc_MessageHeaderType(header);
    header->SessionID.bytesLen = iso20_dc_sessionIDType_BYTES_SIZE;
    std::copy(session_id.begin(), session_id.end(), header->SessionID.bytes);
    header->TimeStamp = timestamp;
}

SCENARIO("Encode and decode ISO15118-20 DC charge loop message") {

    GIVEN("Good case - Encode correct bpt control (DcChargeLoopReq)") {
        uint8_t doc_raw[] = {0x80, 0x34, 0x04, 0x2d, 0x16, 0x6f, 0x29, 0xfb, 0x80, 0xea, 0x56,
                             0x0a, 0xeb, 0xdb, 0xfb, 0x30, 0x62, 0x81, 0x00, 0x12, 0x00, 0x61,
                             0x64, 0x00, 0x0a, 0x02, 0x00, 0x24, 0x00, 0xc8, 0x00};

        iso20_dc_exiDocument request;
        init_iso20_dc_exiDocument(&request);

        request.DC_ChargeLoopReq_isUsed = true;
        init_iso20_dc_DC_ChargeLoopReqType(&request.DC_ChargeLoopReq);

        const auto session_id =
            std::array<uint8_t, iso20_dc_sessionIDType_BYTES_SIZE>{0x5A, 0x2C, 0xDE, 0x53, 0xF7, 0x01, 0xD4, 0xAC};
        uint64_t timestamp = 1718607534;
        setup_header(&request.DC_ChargeLoopReq.Header, session_id, timestamp);

        auto& charge_loop = request.DC_ChargeLoopReq;

        charge_loop.MeterInfoRequested = false;
        charge_loop.EVPresentVoltage = {0, 400};

        charge_loop.BPT_Scheduled_DC_CLReqControlMode_isUsed = true;
        init_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(&charge_loop.BPT_Scheduled_DC_CLReqControlMode);
        charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetCurrent = {0, 20};
        charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetVoltage = {0, 400};

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Encode correct bpt control (DcChargeLoopRes)") {

        uint8_t doc_raw[] = {0x80, 0x38, 0x04, 0x2d, 0x16, 0x6f, 0x29, 0xfb, 0x80, 0xea, 0x56, 0x0b, 0x7b, 0xdb, 0xfb,
                             0x30, 0x62, 0x00, 0x63, 0xf0, 0x68, 0x07, 0x81, 0xfc, 0x28, 0x07, 0xc2, 0x22, 0x30};

        iso20_dc_exiDocument response;
        init_iso20_dc_exiDocument(&response);

        response.DC_ChargeLoopRes_isUsed = true;
        init_iso20_dc_DC_ChargeLoopResType(&response.DC_ChargeLoopRes);

        const auto session_id =
            std::array<uint8_t, iso20_dc_sessionIDType_BYTES_SIZE>{0x5A, 0x2C, 0xDE, 0x53, 0xF7, 0x01, 0xD4, 0xAC};
        uint64_t timestamp = 1718607543;
        setup_header(&response.DC_ChargeLoopReq.Header, session_id, timestamp);

        auto& charge_loop = response.DC_ChargeLoopRes;

        charge_loop.ResponseCode = iso20_dc_responseCodeType_OK;
        charge_loop.EVSEPresentCurrent = {-2, 2000};
        charge_loop.EVSEPresentVoltage = {-1, 4000};
        charge_loop.EVSEPowerLimitAchieved = true;
        charge_loop.EVSECurrentLimitAchieved = true;
        charge_loop.EVSEVoltageLimitAchieved = true;

        charge_loop.BPT_Scheduled_DC_CLResControlMode_isUsed = true;
        charge_loop.BPT_Scheduled_DC_CLResControlMode = {};

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(response, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode correct bpt control (DcChargeLoopReq)") {
        const auto expected_session_id = std::vector<uint8_t>{0x5A, 0x2C, 0xDE, 0x53, 0xF7, 0x01, 0xD4, 0xAC};

        uint8_t doc_raw[] = {0x80, 0x34, 0x04, 0x2d, 0x16, 0x6f, 0x29, 0xfb, 0x80, 0xea, 0x56,
                             0x0a, 0xeb, 0xdb, 0xfb, 0x30, 0x62, 0x81, 0x00, 0x12, 0x00, 0x61,
                             0x64, 0x00, 0x0a, 0x02, 0x00, 0x24, 0x00, 0xc8, 0x00};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<iso20_dc_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            REQUIRE(request.DC_ChargeLoopReq_isUsed == true);
            // Check Header
            const auto& header = request.DC_ChargeLoopReq.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);

            REQUIRE(header.TimeStamp == 1718607534);

            // Check Body
            const auto& charge_loop = request.DC_ChargeLoopReq;
            REQUIRE(charge_loop.MeterInfoRequested == false);
            REQUIRE(charge_loop.EVPresentVoltage.Exponent == 0);
            REQUIRE(charge_loop.EVPresentVoltage.Value == 400);

            REQUIRE(charge_loop.BPT_Scheduled_DC_CLReqControlMode_isUsed == true);
            REQUIRE(charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetCurrent.Exponent == 0);
            REQUIRE(charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetCurrent.Value == 20);
            REQUIRE(charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetVoltage.Exponent == 0);
            REQUIRE(charge_loop.BPT_Scheduled_DC_CLReqControlMode.EVTargetVoltage.Value == 400);
        }
    }

    GIVEN("Good case - Decode correct bpt control (DcChargeLoopRes)") {

        const auto expected_session_id = std::vector<uint8_t>{0x5A, 0x2C, 0xDE, 0x53, 0xF7, 0x01, 0xD4, 0xAC};

        uint8_t doc_raw[] = {0x80, 0x38, 0x04, 0x2d, 0x16, 0x6f, 0x29, 0xfb, 0x80, 0xea, 0x56, 0x0b, 0x7b, 0xdb, 0xfb,
                             0x30, 0x62, 0x00, 0x63, 0xf0, 0x68, 0x07, 0x81, 0xfc, 0x28, 0x07, 0xc2, 0x22, 0x30};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<iso20_dc_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            REQUIRE(request.DC_ChargeLoopRes_isUsed == true);
            // Check Header
            const auto& header = request.DC_ChargeLoopRes.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);

            REQUIRE(header.TimeStamp == 1718607543);

            // Check Body
            const auto& charge_loop = request.DC_ChargeLoopRes;
            REQUIRE(charge_loop.ResponseCode == iso20_dc_responseCodeType_OK);
            REQUIRE(charge_loop.EVSEPresentCurrent.Exponent == -2);
            REQUIRE(charge_loop.EVSEPresentCurrent.Value == 2000);
            REQUIRE(charge_loop.EVSEPresentVoltage.Exponent == -1);
            REQUIRE(charge_loop.EVSEPresentVoltage.Value == 4000);
            REQUIRE(charge_loop.EVSEPowerLimitAchieved == true);
            REQUIRE(charge_loop.EVSECurrentLimitAchieved == true);
            REQUIRE(charge_loop.EVSEVoltageLimitAchieved == true);
            REQUIRE(charge_loop.BPT_Scheduled_DC_CLResControlMode_isUsed == true);
        }
    }
}
