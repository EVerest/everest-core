// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "http_client.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

namespace module::main {

class HttpClientIntegrationTest : public ::testing::Test {};

static const std::string HOST = "localhost";
static int HTTP_PORT = 8000;
static int HTTPS_PORT = 8443;
const char* MOCK_API_TLS_CERT_CONTENTS = "MIIDazCCAlOgAwIBAgIUHDu1ZdpL229xmwqrmq/oq9YQaYwwDQYJKoZIhvcNAQEL"
                                         "BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM"
                                         "GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yMzA5MTQxMjAxMDhaFw0yNDA5"
                                         "MTMxMjAxMDhaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw"
                                         "HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB"
                                         "AQUAA4IBDwAwggEKAoIBAQDCES0SIQSMzKi6aIuLNkjXUj1/eGjuAV2qLcPiaRe3"
                                         "GYRy+tDS4wJb0JxdU2JYMzGrq3tNGcm6E/bXpkJWjB1znFhd+6wr077KV+ryMfBa"
                                         "QwE7uIOnj4XeIBRhU9QvgSF3vfvoxOEKsq6X+cXPlAelbnMrIXniL4lwLNJD2UAl"
                                         "eNYmJFKIJfZPmnNKLQwZkvIL8H5G134KMvOh2AVG1EHuzUBoKs72d77TI6UsITu9"
                                         "/PeATVxm9hhRpk1tuq/NLoUHTqgUPsfN83zeCm7buOOmQpJsypFz5lVmLmtq7YY3"
                                         "+vu4+IuUQegJUC+eXH+WXrh1gkCpNre/S7KgS0FWnJD3AgMBAAGjUzBRMB0GA1Ud"
                                         "DgQWBBQ/YFwElfxomN+kQvtf4tTjU4XGrzAfBgNVHSMEGDAWgBQ/YFwElfxomN+k"
                                         "Qvtf4tTjU4XGrzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAZ"
                                         "K9/4szwsoeTQbPxeDNKNeBRrdHhVOtC3PLP2O0eqZkogTFE4PhreL7S+Q4INbrUh"
                                         "Pw/mZ9FwfsyHVupJWUBgPZx9kSflAJHFG7rikY13UenLmYNU4lGsoJQEewLw+wT1"
                                         "jfJgW/LXZ2He1dMsp3IVyNjR62BtZyI4B9ArUxyILpSSsczk7XN4oEkWDCTATP7t"
                                         "VfsKaM6eIfSnY11g1koVjGy+YtdcO5GJ/6Q7va1BuT3PzD3GjcxPZfhVu3rJBupl"
                                         "0p0LoiBSxpcepMYag5zguxoyU78FKdShFyl5lnFUtAWVD9Hi1+M/znYwiXpS6EGc"
                                         "DW+bAzWAH3M1KKV2UUTa";
const std::string MOCK_API_TLS_CERT_NO_NEWLINES =
    std::string("-----BEGIN CERTIFICATE-----") + MOCK_API_TLS_CERT_CONTENTS + "-----END CERTIFICATE-----";
const std::string MOCK_API_TLS_CERT_BOTH_NEWLINES =
    std::string("-----BEGIN CERTIFICATE-----\n") + MOCK_API_TLS_CERT_CONTENTS + "\n-----END CERTIFICATE-----";
const std::string MOCK_API_TLS_CERT_FIRST_NEWLINE =
    std::string("-----BEGIN CERTIFICATE-----\n") + MOCK_API_TLS_CERT_CONTENTS + "-----END CERTIFICATE-----";
const std::string MOCK_API_TLS_CERT_SECOND_NEWLINE =
    std::string("-----BEGIN CERTIFICATE-----") + MOCK_API_TLS_CERT_CONTENTS + "\n-----END CERTIFICATE-----";

// \brief Test get_powermeter returns correct its status including meter_Id
TEST_F(HttpClientIntegrationTest, test_status) {

    HttpClient client(HOST, HTTP_PORT, "");
    auto res = client.get("/v1/status");

    EXPECT_EQ(200, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_THAT(json.at("meterId").size(), testing::Gt(0));
}

/// \brief Test get_powermeter returns correct live measure
TEST_F(HttpClientIntegrationTest, test_get_livemeasure) {

    HttpClient client(HOST, HTTP_PORT, "");
    auto res = client.get("/v1/livemeasure");

    EXPECT_EQ(200, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_THAT(json.at("timestamp").size(), testing::Gt(0));
}

TEST_F(HttpClientIntegrationTest, test_put_legal) {

    HttpClient client(HOST, HTTP_PORT, "");
    auto res = client.put("/v1/legal?transactionId=test_transaction", R"({"running": false})");

    EXPECT_EQ(200, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_EQ(json.at("transactionId"), "test_transaction");
}

TEST_F(HttpClientIntegrationTest, test_post_legal) {

    HttpClient client(HOST, HTTP_PORT, "");
    auto res = client.post("/v1/legal", R"({
                                                                     "evseId": "string",
                                                                     "transactionId": "test_transaction",
                                                                     "clientId": "string",
                                                                     "tariffId": 0,
                                                                     "cableId": 0,
                                                                     "userData": "string"
                                                                     })");

    EXPECT_EQ(201, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_EQ(json.at("transactionId"), "test_transaction");
    EXPECT_EQ(json.at("running").get<bool>(), true);
}

/// \brief Test get_powermeter returns correct live measure
TEST_F(HttpClientIntegrationTest, test_get_livemeasure_tls) {

    HttpClient client(HOST, HTTPS_PORT, MOCK_API_TLS_CERT_BOTH_NEWLINES);
    auto res = client.get("/v1/livemeasure");

    EXPECT_EQ(200, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_THAT(json.at("timestamp").size(), testing::Gt(0));
}

TEST_F(HttpClientIntegrationTest, test_put_legal_tls) {

    HttpClient client(HOST, HTTPS_PORT, MOCK_API_TLS_CERT_BOTH_NEWLINES);
    auto res = client.put("/v1/legal?transactionId=test_transaction", R"({"running": false})");

    EXPECT_EQ(200, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_EQ(json.at("transactionId"), "test_transaction");
}

TEST_F(HttpClientIntegrationTest, test_post_legal_tls) {

    HttpClient client(HOST, HTTPS_PORT, MOCK_API_TLS_CERT_BOTH_NEWLINES);
    auto res = client.post("/v1/legal", R"({
                                                                     "evseId": "string",
                                                                     "transactionId": "test_transaction",
                                                                     "clientId": "string",
                                                                     "tariffId": 0,
                                                                     "cableId": 0,
                                                                     "userData": "string"
                                                                     })");

    EXPECT_EQ(201, res.status_code);

    auto json = nlohmann::json::parse(res.body);

    EXPECT_EQ(json.at("transactionId"), "test_transaction");
    EXPECT_EQ(json.at("running").get<bool>(), true);
}

class HttpClientIntegrationTestWithCert : public ::testing::TestWithParam<std::string> {
protected:
    std::string cert;
};

/// \brief Test that the module fixes missing newlines correctly
TEST_P(HttpClientIntegrationTestWithCert, test_fix_missing_newlines_in_cert) {
    std::string cert = GetParam();
    HttpClient client(HOST, HTTPS_PORT, cert);
    auto res = client.get("/v1/livemeasure");
    EXPECT_EQ(200, res.status_code);
}

INSTANTIATE_TEST_SUITE_P(HttpFixCertNewlinesTests, HttpClientIntegrationTestWithCert,
                         ::testing::Values(MOCK_API_TLS_CERT_NO_NEWLINES, MOCK_API_TLS_CERT_FIRST_NEWLINE,
                                           MOCK_API_TLS_CERT_SECOND_NEWLINE));
} // namespace module::main
