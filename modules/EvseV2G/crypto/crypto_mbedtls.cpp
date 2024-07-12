// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#include <cstddef>
#include <string>

#include "crypto_mbedtls.hpp"
#include "iso_server.hpp"
#include "log.hpp"

#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/exi_v2gtp.h> //for V2GTP_HEADER_LENGTHs
#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>
#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <mbedtls/base64.h>
#include <mbedtls/error.h>
#include <mbedtls/oid.h> // To extract the emaid
#include <mbedtls/sha256.h>

namespace {

constexpr std::size_t DIGEST_SIZE = 32;
constexpr std::size_t MQTT_MAX_PAYLOAD_SIZE = 268435455;
constexpr std::size_t MAX_EMAID_LEN = 18;

bool getSubjectData(const mbedtls_x509_name* ASubject, const char* AAttrName, const mbedtls_asn1_buf** AVal);
int debug_verify_cert(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags);
void printMbedVerifyErrorCode(int AErr, uint32_t AFlags);
bool base64_decode(const char* text, std::size_t len, std::uint8_t* out_data, std::size_t& out_len);
std::string base64_encode(const std::uint8_t* data, std::size_t len, bool newLine);

bool getSubjectData(const mbedtls_x509_name* ASubject, const char* AAttrName, const mbedtls_asn1_buf** AVal) {
    const char* attrName = NULL;

    while (NULL != ASubject) {
        if ((0 == mbedtls_oid_get_attr_short_name(&ASubject->oid, &attrName)) && (0 == strcmp(attrName, AAttrName))) {

            *AVal = &ASubject->val;
            return true;
        } else {
            ASubject = ASubject->next;
        }
    }
    *AVal = NULL;
    return false;
}

/*!
 * \brief debug_verify_cert Function is from https://github.com/aws/aws-iot-device-sdk-embedded-C/blob
 * /master/platform/linux/mbedtls/network_mbedtls_wrapper.c to debug certificate verification
 * \param data
 * \param crt
 * \param depth
 * \param flags
 * \return
 */
int debug_verify_cert(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags) {
    char buf[1024];
    ((void)data);

    dlog(DLOG_LEVEL_INFO, "\nVerify requested for (Depth %d):\n", depth);
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    dlog(DLOG_LEVEL_INFO, "%s", buf);

    if ((*flags) == 0)
        dlog(DLOG_LEVEL_INFO, "  This certificate has no flags\n");
    else {
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
        dlog(DLOG_LEVEL_INFO, "%s\n", buf);
    }

    return (0);
}

/*!
 * \brief printMbedVerifyErrorCode This functions prints the mbedTls specific error code.
 * \param AErr is the return value of the mbed verify function
 * \param AFlags includes the flags of the verification.
 */
void printMbedVerifyErrorCode(int AErr, uint32_t AFlags) {
    dlog(DLOG_LEVEL_ERROR, "Failed to verify certificate (err: 0x%08x flags: 0x%08x)", AErr, AFlags);
    if (AErr == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        if (AFlags & MBEDTLS_X509_BADCERT_EXPIRED)
            dlog(DLOG_LEVEL_ERROR, "CERT_EXPIRED");
        else if (AFlags & MBEDTLS_X509_BADCERT_REVOKED)
            dlog(DLOG_LEVEL_ERROR, "CERT_REVOKED");
        else if (AFlags & MBEDTLS_X509_BADCERT_CN_MISMATCH)
            dlog(DLOG_LEVEL_ERROR, "CN_MISMATCH");
        else if (AFlags & MBEDTLS_X509_BADCERT_NOT_TRUSTED)
            dlog(DLOG_LEVEL_ERROR, "CERT_NOT_TRUSTED");
        else if (AFlags & MBEDTLS_X509_BADCRL_NOT_TRUSTED)
            dlog(DLOG_LEVEL_ERROR, "CRL_NOT_TRUSTED");
        else if (AFlags & MBEDTLS_X509_BADCRL_EXPIRED)
            dlog(DLOG_LEVEL_ERROR, "CRL_EXPIRED");
        else if (AFlags & MBEDTLS_X509_BADCERT_MISSING)
            dlog(DLOG_LEVEL_ERROR, "CERT_MISSING");
        else if (AFlags & MBEDTLS_X509_BADCERT_SKIP_VERIFY)
            dlog(DLOG_LEVEL_ERROR, "SKIP_VERIFY");
        else if (AFlags & MBEDTLS_X509_BADCERT_OTHER)
            dlog(DLOG_LEVEL_ERROR, "CERT_OTHER");
        else if (AFlags & MBEDTLS_X509_BADCERT_FUTURE)
            dlog(DLOG_LEVEL_ERROR, "CERT_FUTURE");
        else if (AFlags & MBEDTLS_X509_BADCRL_FUTURE)
            dlog(DLOG_LEVEL_ERROR, "CRL_FUTURE");
        else if (AFlags & MBEDTLS_X509_BADCERT_KEY_USAGE)
            dlog(DLOG_LEVEL_ERROR, "KEY_USAGE");
        else if (AFlags & MBEDTLS_X509_BADCERT_EXT_KEY_USAGE)
            dlog(DLOG_LEVEL_ERROR, "EXT_KEY_USAGE");
        else if (AFlags & MBEDTLS_X509_BADCERT_NS_CERT_TYPE)
            dlog(DLOG_LEVEL_ERROR, "NS_CERT_TYPE");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_MD)
            dlog(DLOG_LEVEL_ERROR, "BAD_MD");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_PK)
            dlog(DLOG_LEVEL_ERROR, "BAD_PK");
        else if (AFlags & MBEDTLS_X509_BADCERT_BAD_KEY)
            dlog(DLOG_LEVEL_ERROR, "BAD_KEY");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_MD)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_MD");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_PK)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_PK");
        else if (AFlags & MBEDTLS_X509_BADCRL_BAD_KEY)
            dlog(DLOG_LEVEL_ERROR, "CRL_BAD_KEY");
    }
}

bool base64_decode(const char* text, std::size_t len, std::uint8_t* out_data, std::size_t& out_len) {
    bool bResult = true;
    const std::size_t dlen = out_len;
    const auto rv = mbedtls_base64_decode(out_data, dlen, &out_len, reinterpret_cast<const std::uint8_t*>(text), len);
    if (rv != 0) {
        std::array<char, 256> strerr{};
        mbedtls_strerror(rv, strerr.data(), strerr.size());
        dlog(DLOG_LEVEL_ERROR, "Failed to decode base64 stream (-0x%04x) %s", rv, strerr);
        bResult = false;
    }
    return bResult;
}

std::string base64_encode(const std::uint8_t* data, std::size_t len, bool /* newLine */) {
    std::string result;
    std::size_t olen{0};
    mbedtls_base64_encode(nullptr, 0, &olen, data, len);

    if (MQTT_MAX_PAYLOAD_SIZE < olen) {
        dlog(DLOG_LEVEL_ERROR, "Mqtt payload size exceeded!");
    } else {
        result = std::string(olen, '\0');

        if ((mbedtls_base64_encode(reinterpret_cast<std::uint8_t*>(result.data()), result.size(), &olen, data, len) !=
             0)) {
            result.clear();
            dlog(DLOG_LEVEL_ERROR, "Unable to base64 encode");
        }
    }

    return result;
}

} // namespace

namespace crypto::mbedtls {

/*!
 * \brief check_iso2_signature This function validates the ISO signature
 * \param iso2_signature is the signature of the ISO EXI fragment
 * \param public_key is the public key to validate the signature against the ISO EXI fragment
 * \param iso2_exi_fragment iso2_exi_fragment is the ISO EXI fragment
 */
bool check_iso2_signature(const struct iso2_SignatureType* iso2_signature, mbedtls_ecdsa_context& public_key,
                          struct iso2_exiFragment* iso2_exi_fragment) {
    /** Digest check **/
    int err = 0;
    const struct iso2_SignatureType* sig = iso2_signature;
    unsigned char buf[MAX_EXI_SIZE];
    const struct iso2_ReferenceType* req_ref = &sig->SignedInfo.Reference.array[0];
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, buf, MAX_EXI_SIZE, 0, NULL);
    uint8_t digest[DIGEST_SIZE];
    err = encode_iso2_exiFragment(&stream, iso2_exi_fragment);
    if (err != 0) {
        dlog(DLOG_LEVEL_ERROR, "Unable to encode fragment, error code = %d", err);
        return false;
    }
    uint32_t frag_data_len = exi_bitstream_get_length(&stream);
    mbedtls_sha256(buf, frag_data_len, digest, 0);

    if (req_ref->DigestValue.bytesLen != DIGEST_SIZE) {
        dlog(DLOG_LEVEL_ERROR, "Invalid digest length %u in signature", req_ref->DigestValue.bytesLen);
        return false;
    }

    if (memcmp(req_ref->DigestValue.bytes, digest, DIGEST_SIZE) != 0) {
        dlog(DLOG_LEVEL_ERROR, "Invalid digest in signature");
        return false;
    }

    /** Validate signature **/
    struct iso2_xmldsigFragment sig_fragment;
    init_iso2_xmldsigFragment(&sig_fragment);
    sig_fragment.SignedInfo_isUsed = 1;
    sig_fragment.SignedInfo = sig->SignedInfo;

    /** \req [V2G2-771] Don't use following fields */
    sig_fragment.SignedInfo.Id_isUsed = 0;
    sig_fragment.SignedInfo.CanonicalizationMethod.ANY_isUsed = 0;
    sig_fragment.SignedInfo.SignatureMethod.HMACOutputLength_isUsed = 0;
    sig_fragment.SignedInfo.SignatureMethod.ANY_isUsed = 0;
    for (auto* ref = sig_fragment.SignedInfo.Reference.array;
         ref != (sig_fragment.SignedInfo.Reference.array + sig_fragment.SignedInfo.Reference.arrayLen); ++ref) {
        ref->Type_isUsed = 0;
        ref->Transforms.Transform.ANY_isUsed = 0;
        ref->Transforms.Transform.XPath_isUsed = 0;
        ref->DigestMethod.ANY_isUsed = 0;
    }

    stream.byte_pos = 0;
    stream.bit_count = 0;
    err = encode_iso2_xmldsigFragment(&stream, &sig_fragment);

    if (err != 0) {
        dlog(DLOG_LEVEL_ERROR, "Unable to encode XML signature fragment, error code = %d", err);
        return false;
    }
    uint32_t sign_info_fragmen_len = exi_bitstream_get_length(&stream);

    /* Hash the signature */
    mbedtls_sha256(buf, sign_info_fragmen_len, digest, 0);

    /* Validate the ecdsa signature using the public key */
    if (sig->SignatureValue.CONTENT.bytesLen == 0) {
        dlog(DLOG_LEVEL_ERROR, "Signature len is invalid (%i)", sig->SignatureValue.CONTENT.bytesLen);
        return false;
    }

    /* Init mbedtls parameter */
    mbedtls_ecp_group ecp_group;
    mbedtls_ecp_group_init(&ecp_group);

    mbedtls_mpi mpi_r;
    mbedtls_mpi_init(&mpi_r);
    mbedtls_mpi mpi_s;
    mbedtls_mpi_init(&mpi_s);

    mbedtls_mpi_read_binary(&mpi_r, (const unsigned char*)&sig->SignatureValue.CONTENT.bytes[0],
                            sig->SignatureValue.CONTENT.bytesLen / 2);
    mbedtls_mpi_read_binary(
        &mpi_s, (const unsigned char*)&sig->SignatureValue.CONTENT.bytes[sig->SignatureValue.CONTENT.bytesLen / 2],
        sig->SignatureValue.CONTENT.bytesLen / 2);

    err = mbedtls_ecp_group_load(&ecp_group, MBEDTLS_ECP_DP_SECP256R1);

    if (err == 0) {
        err = mbedtls_ecdsa_verify(&ecp_group, static_cast<const unsigned char*>(digest), 32, &public_key.Q, &mpi_r,
                                   &mpi_s);
    }

    mbedtls_ecp_group_free(&ecp_group);
    mbedtls_mpi_free(&mpi_r);
    mbedtls_mpi_free(&mpi_s);

    if (err != 0) {
        char error_buf[100];
        mbedtls_strerror(err, error_buf, sizeof(error_buf));
        dlog(DLOG_LEVEL_ERROR, "Invalid signature, error code = -0x%08x, %s", err, error_buf);
        return false;
    }

    return true;
}

bool load_contract_root_cert(Certificate_ptr& contract_root_crt, const char* V2G_file_path, const char* MO_file_path) {
    int rv = 0;

    if ((rv = mbedtls_x509_crt_parse_file(contract_root_crt.get(), MO_file_path)) != 0) {
        char strerr[256];
        mbedtls_strerror(rv, strerr, sizeof(strerr));
        dlog(DLOG_LEVEL_WARNING, "Unable to parse MO (%s) root certificate. (err: -0x%04x - %s)", MO_file_path, -rv,
             strerr);
        dlog(DLOG_LEVEL_INFO, "Attempting to parse V2G root certificate..");

        if ((rv = mbedtls_x509_crt_parse_file(contract_root_crt.get(), V2G_file_path)) != 0) {
            mbedtls_strerror(rv, strerr, sizeof(strerr));
            dlog(DLOG_LEVEL_ERROR, "Unable to parse V2G (%s) root certificate. (err: -0x%04x - %s)", V2G_file_path, -rv,
                 strerr);
        }
    }

    return (rv != 0) ? false : true;
}

void free_connection_crypto_data(v2g_connection* conn) {
    mbedtls_ecdsa_free(&conn->ctx->session.contract.pubkey);
    mbedtls_ecdsa_init(&conn->ctx->session.contract.pubkey);
}

int load_certificate(Certificate_ptr& crt, const std::uint8_t* bytes, std::uint16_t bytesLen) {
    int err{-1};

    // Parse contract leaf certificate
    if (bytesLen != 0) {
        err = mbedtls_x509_crt_parse(crt.get(), bytes, bytesLen);
        if (err != 0) {
            char strerr[256];
            mbedtls_strerror(err, strerr, std::string(strerr).size());
            dlog(DLOG_LEVEL_ERROR, "handle_payment_detail: invalid certificate received in req: %s", strerr);
        }

    } else {
        dlog(DLOG_LEVEL_ERROR, "No certificate received!");
    }

    return err;
}

int parse_contract_certificate(Certificate_ptr& crt, const std::uint8_t* bytes, std::size_t bytesLen) {
    auto err = mbedtls_x509_crt_parse(crt.get(), bytes, bytesLen);

    if (err != 0) {
        char strerr[256];
        mbedtls_strerror(err, strerr, std::string(strerr).size());
        dlog(DLOG_LEVEL_ERROR, "handle_payment_detail: invalid certificate received in req: %s", strerr);
    }

    return err;
}

/*!
 * \brief getEmaidFromContractCert This function extracts the emaid from an subject of a contract certificate.
 * \param ASubject is the subject of the contract certificate.
 * \return Returns the length of the extracted emaid.
 */
std::string getEmaidFromContractCert(const Certificate_ptr& crt) {
    const mbedtls_x509_name* ASubject = &crt.get()->subject;
    const mbedtls_asn1_buf* val = NULL;
    std::size_t certEmaidLen = 0;
    char certEmaid[MAX_EMAID_LEN + 1];

    std::string result;
    if (true == getSubjectData(ASubject, "CN", &val)) {
        /* Check the emaid length within the certificate */
        certEmaidLen = MAX_EMAID_LEN < val->len ? 0 : val->len;
        strncpy(certEmaid, reinterpret_cast<const char*>(val->p), certEmaidLen);
        certEmaid[certEmaidLen] = '\0';
        result = std::string{&certEmaid[0], certEmaidLen};
    } else {
        dlog(DLOG_LEVEL_WARNING, "No CN found in subject of the contract certificate");
    }

    return result;
}

std::string chain_to_pem(const Certificate_ptr& contract_crt, const void* /* chain */) {
    const mbedtls_x509_crt* crt = contract_crt.get();
    std::string contract_cert_chain_pem;

    while (crt != nullptr && crt->version != 0) {
        const auto pem = certificate_to_pem(crt);
        if (pem.empty()) {
            dlog(DLOG_LEVEL_ERROR, "Unable to encode certificate chain");
            break;
        }
        contract_cert_chain_pem.append(pem);
        crt = crt->next;
    }

    return contract_cert_chain_pem;
}

verify_result_t verify_certificate(Certificate_ptr& contract_crt, const void* chain, const char* v2g_root_cert_path,
                                   const char* mo_root_cert_path, bool debugMode) {
    Certificate_ptr contract_root_crt;
    uint32_t flags;
    verify_result_t result{verify_result_t::verified};

    /* Load supported V2G/MO root certificates */
    if (load_contract_root_cert(contract_root_crt, v2g_root_cert_path, mo_root_cert_path)) {
        // === Verify the retrieved contract ECDSA key against the root cert ===
        const int err = mbedtls_x509_crt_verify(contract_crt.get(), contract_root_crt.get(), NULL, NULL, &flags,
                                                (debugMode) ? debug_verify_cert : NULL, NULL);
        if (err != 0) {
            printMbedVerifyErrorCode(err, flags);
            dlog(DLOG_LEVEL_ERROR, "Validation of the contract certificate failed!");
            if ((err == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) && (flags & MBEDTLS_X509_BADCERT_EXPIRED)) {
                result = verify_result_t::CertificateExpired;
            } else if ((err == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) && (flags & MBEDTLS_X509_BADCERT_REVOKED)) {
                result = verify_result_t::CertificateRevoked;
            } else {
                result = verify_result_t::CertChainError;
            }
        }

    } else {
        result = verify_result_t::NoCertificateAvailable;
    }

    return result;
}

int get_public_key(mbedtls_ecdsa_context* pubkey, mbedtls_pk_context& pk) {
    // Convert the public key in the certificate to a mbed TLS ECDSA public key
    // This also verifies that it's an ECDSA key and not an RSA key
    int err = mbedtls_ecdsa_from_keypair(pubkey, mbedtls_pk_ec(pk));
    if (err != 0) {
        char strerr[256];
        mbedtls_strerror(err, strerr, std::string(strerr).size());
        dlog(DLOG_LEVEL_ERROR, "Could not retrieve ecdsa public key from certificate keypair: %s", strerr);
    }

    return err;
}

std::string certificate_to_pem(const mbedtls_x509_crt* crt) {
    std::string result;
    auto cert_b64 = base64_encode(crt->raw.p, crt->raw.len, true);
    if (cert_b64.empty()) {
        dlog(DLOG_LEVEL_ERROR, "Unable to convert certificate to PEM");
    } else {
        result.append("-----BEGIN CERTIFICATE-----\n");
        result.append(cert_b64);
        result.append("\n-----END CERTIFICATE-----\n");
    }
    return result;
}

} // namespace crypto::mbedtls
