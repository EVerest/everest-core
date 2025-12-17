#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>

#include "test_utils/codec.hpp"

SCENARIO("Encode and decode a DIN70121 service discovery message") {

    // Exi Stream: 809a0211d63f74d2297ac9119400
    // XML:
    // <ns7:V2G_Message>
    //   <ns7:Header>
    //     <ns8:SessionID>4758FDD348A5EB24</ns8:SessionID>
    //   </ns7:Header>
    //   <ns7:Body>
    //     <ns5:ServiceDiscoveryReq>
    //       <ns5:ServiceCategory>EVCharging</ns5:ServiceCategory>
    //     </ns5:ServiceDiscoveryReq>
    //   </ns7:Body>
    // </ns7:V2G_Message>

    GIVEN("Good case - Encode a ServiceDiscoveryReq document") {

        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a, 0xc9, 0x11, 0x94, 0x00};

        din_exiDocument request;
        init_din_exiDocument(&request);

        init_din_MessageHeaderType(&request.V2G_Message.Header);

        auto& header = request.V2G_Message.Header;
        header.SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;

        // copy the session id into the header
        const auto session_id = std::array<uint8_t, 8>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        std::copy(session_id.begin(), session_id.end(), header.SessionID.bytes);

        init_din_BodyType(&request.V2G_Message.Body);
        auto& body = request.V2G_Message.Body;

        // set the ServiceDiscoveryReqType
        init_din_ServiceDiscoveryReqType(&body.ServiceDiscoveryReq);
        body.ServiceDiscoveryReq_isUsed = true;

        // set the service category
        body.ServiceDiscoveryReq.ServiceScope_isUsed = false;
        body.ServiceDiscoveryReq.ServiceCategory_isUsed = true;
        body.ServiceDiscoveryReq.ServiceCategory = din_serviceCategoryType_EVCharging;

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode a ServiceDiscoveryReq document") {

        auto expected_session_id = std::vector<uint8_t>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a, 0xc9, 0x11, 0x94, 0x00};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<din_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            // Check Header
            const auto& header = request.V2G_Message.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);
            REQUIRE(header.Notification_isUsed == false);
            REQUIRE(header.Signature_isUsed == false);

            // Check Body
            const auto& body = request.V2G_Message.Body;
            REQUIRE(body.ServiceDiscoveryReq_isUsed == true);

            const auto& serviceDiscoveryReq = body.ServiceDiscoveryReq;
            REQUIRE(serviceDiscoveryReq.ServiceScope_isUsed == false);
            REQUIRE(serviceDiscoveryReq.ServiceCategory_isUsed == true);
        }
    }

    // EXI stream: 809a0211d63f74d2297ac911a00120024100c4
    // XML:
    // <ns7:V2G_Message>
    //   <ns7:Header>
    //     <ns8:SessionID>4758FDD348A5EB24</ns8:SessionID>
    //   </ns7:Header>
    //   <ns7:Body>
    //     <ns5:ServiceDiscoveryRes>
    //       <ns5:ResponseCode>OK</ns5:ResponseCode>
    //       <ns5:PaymentOptions>
    //         <ns6:PaymentOption>ExternalPayment</ns6:PaymentOption>
    //       </ns5:PaymentOptions>
    //       <ns5:ChargeService>
    //         <ns6:ServiceTag>
    //           <ns6:ServiceID>1</ns6:ServiceID>
    //           <ns6:ServiceCategory>EVCharging</ns6:ServiceCategory>
    //         </ns6:ServiceTag>
    //         <ns6:FreeService>false</ns6:FreeService>
    //         <ns6:EnergyTransferType>DC_extended</ns6:EnergyTransferType>
    //       </ns5:ChargeService>
    //     </ns5:ServiceDiscoveryRes>
    //   </ns7:Body>
    // </ns7:V2G_Message>

    GIVEN("Good case - Encode an ServiceDiscoveryRes document") {
        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a,
                             0xc9, 0x11, 0xa0, 0x01, 0x20, 0x02, 0x41, 0x00, 0xc4};

        din_exiDocument request;
        init_din_exiDocument(&request);

        init_din_MessageHeaderType(&request.V2G_Message.Header);

        auto& header = request.V2G_Message.Header;
        header.SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;

        // copy the session id into the header
        const auto session_id = std::array<uint8_t, 8>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        std::copy(session_id.begin(), session_id.end(), header.SessionID.bytes);

        init_din_BodyType(&request.V2G_Message.Body);
        auto& body = request.V2G_Message.Body;

        // set the ServiceDiscoveryResType
        init_din_ServiceDiscoveryResType(&body.ServiceDiscoveryRes);
        body.ServiceDiscoveryRes_isUsed = true;

        // set the response code
        body.ServiceDiscoveryRes.ResponseCode = din_responseCodeType_OK;

        // set the payment options
        auto& paymentOptions = body.ServiceDiscoveryRes.PaymentOptions;
        paymentOptions.PaymentOption.arrayLen = 1;
        const auto givenPaymentOptions = std::array<din_paymentOptionType, 1>{din_paymentOptionType_ExternalPayment};
        std::copy(givenPaymentOptions.begin(), givenPaymentOptions.end(), paymentOptions.PaymentOption.array);

        // set the charge service
        auto& chargeService = body.ServiceDiscoveryRes.ChargeService;
        chargeService.ServiceTag.ServiceID = 1;
        chargeService.ServiceTag.ServiceName_isUsed = false;
        chargeService.ServiceTag.ServiceScope_isUsed = false;
        chargeService.ServiceTag.ServiceCategory = din_serviceCategoryType_EVCharging;
        chargeService.FreeService = false;
        chargeService.EnergyTransferType = din_EVSESupportedEnergyTransferType_DC_extended;

        THEN("It should be encoded succussfully") {
            const auto result = test_utils::encode_and_compare(request, doc_raw, sizeof(doc_raw));

            REQUIRE(result.encoding_successful);
            REQUIRE(result.bitstream_match);
        }
    }

    GIVEN("Good case - Decode an ServiceDiscoveryRes document") {
        auto expected_session_id = std::vector<uint8_t>{0x47, 0x58, 0xFD, 0xD3, 0x48, 0xA5, 0xEB, 0x24};
        uint8_t doc_raw[] = {0x80, 0x9a, 0x02, 0x11, 0xd6, 0x3f, 0x74, 0xd2, 0x29, 0x7a,
                             0xc9, 0x11, 0xa0, 0x01, 0x20, 0x02, 0x41, 0x00, 0xc4};

        THEN("It should be decoded succussfully") {
            const auto result = test_utils::decode<din_exiDocument>(doc_raw, sizeof(doc_raw));
            REQUIRE(result.decoding_successful);

            const auto& request = result.value;

            // Check Header
            const auto& header = request.V2G_Message.Header;
            const auto session_id =
                std::vector<uint8_t>(std::begin(header.SessionID.bytes), std::end(header.SessionID.bytes));
            REQUIRE(session_id == expected_session_id);
            REQUIRE(header.Notification_isUsed == false);
            REQUIRE(header.Signature_isUsed == false);

            // Check Body
            const auto& body = request.V2G_Message.Body;
            REQUIRE(body.ServiceDiscoveryRes_isUsed == true);

            const auto& serviceDiscoveryRes = body.ServiceDiscoveryRes;

            // check the response code
            REQUIRE(serviceDiscoveryRes.ResponseCode == din_responseCodeType_OK);

            // check the payment options
            const auto expectedPaymentOptions =
                std::vector<din_paymentOptionType>{din_paymentOptionType_ExternalPayment};
            REQUIRE(serviceDiscoveryRes.PaymentOptions.PaymentOption.arrayLen == 1);
            const auto& paymentOptionsArray = serviceDiscoveryRes.PaymentOptions.PaymentOption.array;
            REQUIRE(paymentOptionsArray[0] == expectedPaymentOptions.at(0));

            // check the charge service
            const auto& chargeService = body.ServiceDiscoveryRes.ChargeService;

            // service tag
            REQUIRE(chargeService.ServiceTag.ServiceID == 1);
            REQUIRE(chargeService.ServiceTag.ServiceName_isUsed == false);
            REQUIRE(chargeService.ServiceTag.ServiceScope_isUsed == false);
            REQUIRE(chargeService.ServiceTag.ServiceCategory == din_serviceCategoryType_EVCharging);
            REQUIRE(chargeService.FreeService == false);
            REQUIRE(chargeService.EnergyTransferType == din_EVSESupportedEnergyTransferType_DC_extended);
        }
    }
}
