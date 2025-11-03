// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <comparators.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <evse_security_mock.hpp>
#include <ocpp/v2/ocsp_updater.hpp>

namespace ocpp {

ACTION_P2(SignalCallsComplete, return_value, semaphore) {
    semaphore->post();
    return return_value;
}
ACTION_P2(SignalCallsCompleteVoid, semaphore) {
    semaphore->post();
    return;
}

class ChargePointMock {
public:
    MOCK_METHOD(v2::GetCertificateStatusResponse, get_certificate_status, (v2::GetCertificateStatusRequest), ());
};

class OcspUpdaterTest : public ::testing::Test {
protected:
    void SetUp() override {
        this->charge_point = std::make_shared<ChargePointMock>();
        this->evse_security = std::make_shared<EvseSecurityMock>();
        this->status_update = [this](auto request) { return this->charge_point->get_certificate_status(request); };

        OCSPRequestData ocsp1;
        ocsp1.hashAlgorithm = HashAlgorithmEnumType::SHA256;
        ocsp1.issuerNameHash = "issuerHash1";
        ocsp1.issuerKeyHash = "issuerKey1";
        ocsp1.serialNumber = "serial1";
        ocsp1.responderUrl = "responder1";
        this->example_ocsp_data.push_back(ocsp1);

        OCSPRequestData ocsp2;
        ocsp2.hashAlgorithm = HashAlgorithmEnumType::SHA384;
        ocsp2.issuerNameHash = "issuerHash2";
        ocsp2.issuerKeyHash = "issuerKey2";
        ocsp2.serialNumber = "serial2";
        ocsp2.responderUrl = "responder2";
        this->example_ocsp_data.push_back(ocsp2);

        OCSPRequestData ocsp3;
        ocsp3.hashAlgorithm = HashAlgorithmEnumType::SHA512;
        ocsp3.issuerNameHash = "issuerHash3";
        ocsp3.issuerKeyHash = "issuerKey3";
        ocsp3.serialNumber = "serial3";
        ocsp3.responderUrl = "responder3";
        this->example_ocsp_data.push_back(ocsp3);

        CertificateHashDataType certificate_hash_data1;
        certificate_hash_data1.hashAlgorithm = HashAlgorithmEnumType::SHA256;
        certificate_hash_data1.issuerNameHash = "issuerHash1";
        certificate_hash_data1.issuerKeyHash = "issuerKey1";
        certificate_hash_data1.serialNumber = "serial1";
        this->example_hash_data.push_back(certificate_hash_data1);

        CertificateHashDataType certificate_hash_data2;
        certificate_hash_data2.hashAlgorithm = HashAlgorithmEnumType::SHA384;
        certificate_hash_data2.issuerNameHash = "issuerHash2";
        certificate_hash_data2.issuerKeyHash = "issuerKey2";
        certificate_hash_data2.serialNumber = "serial2";
        this->example_hash_data.push_back(certificate_hash_data2);

        CertificateHashDataType certificate_hash_data3;
        certificate_hash_data3.hashAlgorithm = HashAlgorithmEnumType::SHA512;
        certificate_hash_data3.issuerNameHash = "issuerHash3";
        certificate_hash_data3.issuerKeyHash = "issuerKey3";
        certificate_hash_data3.serialNumber = "serial3";
        this->example_hash_data.push_back(certificate_hash_data3);

        v2::GetCertificateStatusRequest example_get_cert_status_request_1;
        example_get_cert_status_request_1.ocspRequestData.hashAlgorithm = v2::HashAlgorithmEnum::SHA256;
        example_get_cert_status_request_1.ocspRequestData.issuerNameHash = "issuerHash1";
        example_get_cert_status_request_1.ocspRequestData.issuerKeyHash = "issuerKey1";
        example_get_cert_status_request_1.ocspRequestData.serialNumber = "serial1";
        example_get_cert_status_request_1.ocspRequestData.responderURL = "responder1";
        this->example_status_requests.push_back(example_get_cert_status_request_1);
        v2::GetCertificateStatusRequest example_get_cert_status_request_2;
        example_get_cert_status_request_2.ocspRequestData.hashAlgorithm = v2::HashAlgorithmEnum::SHA384;
        example_get_cert_status_request_2.ocspRequestData.issuerNameHash = "issuerHash2";
        example_get_cert_status_request_2.ocspRequestData.issuerKeyHash = "issuerKey2";
        example_get_cert_status_request_2.ocspRequestData.serialNumber = "serial2";
        example_get_cert_status_request_2.ocspRequestData.responderURL = "responder2";
        this->example_status_requests.push_back(example_get_cert_status_request_2);
        v2::GetCertificateStatusRequest example_get_cert_status_request_3;
        example_get_cert_status_request_3.ocspRequestData.hashAlgorithm = v2::HashAlgorithmEnum::SHA512;
        example_get_cert_status_request_3.ocspRequestData.issuerNameHash = "issuerHash3";
        example_get_cert_status_request_3.ocspRequestData.issuerKeyHash = "issuerKey3";
        example_get_cert_status_request_3.ocspRequestData.serialNumber = "serial3";
        example_get_cert_status_request_3.ocspRequestData.responderURL = "responder3";
        this->example_status_requests.push_back(example_get_cert_status_request_3);
    }

    void TearDown() override {
    }

    v2::cert_status_func status_update;
    std::shared_ptr<EvseSecurityMock> evse_security;
    std::shared_ptr<ChargePointMock> charge_point;

    std::vector<OCSPRequestData> example_ocsp_data;
    std::vector<v2::GetCertificateStatusRequest> example_status_requests;
    std::vector<CertificateHashDataType> example_hash_data;

    boost::interprocess::interprocess_semaphore calls_complete = boost::interprocess::interprocess_semaphore(0);
};

/// \brief Tests a successful update for multiple certs on boot
TEST_F(OcspUpdaterTest, test_success_boot_many) {
    auto ocsp_updater = std::make_unique<v2::OcspUpdater>(this->evse_security, this->status_update);

    testing::Sequence seq;
    v2::GetCertificateStatusResponse response_success;
    response_success.ocspResult = "EXAMPLE OCSP RESULT";
    response_success.status = v2::GetCertificateStatusEnum::Accepted;

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[0]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[1]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[2]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));

    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[0], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[1], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[2], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(SignalCallsCompleteVoid(&this->calls_complete));

    ocsp_updater->start();
    this->calls_complete.timed_wait(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
    ocsp_updater->stop();
}

/// \brief Tests retry logic on CSMS failure to update, multiple certs
TEST_F(OcspUpdaterTest, test_retry_boot_many) {
    auto ocsp_updater = std::make_unique<v2::OcspUpdater>(this->evse_security, this->status_update,
                                                          std::chrono::hours(167), std::chrono::seconds(0));

    testing::Sequence seq;
    v2::GetCertificateStatusResponse response_success;
    response_success.ocspResult = "EXAMPLE OCSP RESULT";
    response_success.status = v2::GetCertificateStatusEnum::Accepted;
    v2::GetCertificateStatusResponse response_fail_status;
    response_fail_status.status = v2::GetCertificateStatusEnum::Failed;
    v2::GetCertificateStatusResponse response_fail_empty;
    response_fail_empty.status = v2::GetCertificateStatusEnum::Accepted;

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[0]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_fail_status));

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[0]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[1]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_fail_empty));

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[0]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[1]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));
    EXPECT_CALL(*this->charge_point, get_certificate_status(this->example_status_requests[2]))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(response_success));

    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[0], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[1], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(this->example_hash_data[2], "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(SignalCallsCompleteVoid(&this->calls_complete));

    ocsp_updater->start();
    this->calls_complete.timed_wait(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
    ocsp_updater->stop();
}

/// \brief Tests certificates are re-verified over time
TEST_F(OcspUpdaterTest, test_reverify_logic) {
    auto ocsp_updater =
        std::make_unique<v2::OcspUpdater>(this->evse_security, this->status_update, std::chrono::seconds(0));

    testing::Sequence seq;
    v2::GetCertificateStatusResponse response_success;
    response_success.ocspResult = "EXAMPLE OCSP RESULT";
    response_success.status = v2::GetCertificateStatusEnum::Accepted;

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(testing::_))
        .Times(3)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(response_success));
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(3)
        .InSequence(seq)
        .WillRepeatedly(testing::Return());

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(testing::_))
        .Times(3)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(response_success));
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(2)
        .InSequence(seq)
        .WillRepeatedly(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(SignalCallsCompleteVoid(&this->calls_complete));

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .InSequence(seq)
        .WillRepeatedly(testing::Return(std::vector<OCSPRequestData>()));

    ocsp_updater->start();
    this->calls_complete.timed_wait(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
    ocsp_updater->stop();
}

/// \brief Tests triggering an update before the deadline
TEST_F(OcspUpdaterTest, test_trigger) {
    auto ocsp_updater = std::make_unique<v2::OcspUpdater>(this->evse_security, this->status_update);

    testing::Sequence seq;
    v2::GetCertificateStatusResponse response_success;
    response_success.ocspResult = "EXAMPLE OCSP RESULT";
    response_success.status = v2::GetCertificateStatusEnum::Accepted;

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(testing::_))
        .Times(3)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(response_success));
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(2)
        .InSequence(seq)
        .WillRepeatedly(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(SignalCallsCompleteVoid(&this->calls_complete));

    EXPECT_CALL(*this->evse_security, get_v2g_ocsp_request_data())
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(this->example_ocsp_data));
    EXPECT_CALL(*this->charge_point, get_certificate_status(testing::_))
        .Times(3)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(response_success));
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(2)
        .InSequence(seq)
        .WillRepeatedly(testing::Return());
    EXPECT_CALL(*this->evse_security, update_ocsp_cache(testing::_, "EXAMPLE OCSP RESULT"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(SignalCallsCompleteVoid(&this->calls_complete));

    ocsp_updater->start();
    this->calls_complete.timed_wait(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
    ocsp_updater->trigger_ocsp_cache_update();
    this->calls_complete.timed_wait(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
    ocsp_updater->stop();
}

/// \brief Triggering while the updater is not running should throw an exception
TEST_F(OcspUpdaterTest, test_exception_trigger_when_not_running) {
    auto ocsp_updater = std::make_unique<v2::OcspUpdater>(this->evse_security, this->status_update);

    ASSERT_THROW(ocsp_updater->trigger_ocsp_cache_update(), std::logic_error);
    ocsp_updater->start();
    ocsp_updater->stop();
    ASSERT_THROW(ocsp_updater->trigger_ocsp_cache_update(), std::logic_error);
}

} // namespace ocpp
