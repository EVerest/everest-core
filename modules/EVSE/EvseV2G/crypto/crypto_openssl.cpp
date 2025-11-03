// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <array>
#include <cassert>

#include "crypto_openssl.hpp"
#include "iso_server.hpp"
#include "log.hpp"

#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/exi_v2gtp.h> //for V2GTP_HEADER_LENGTHs
#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>
#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/store.h>
#include <openssl/x509.h>

namespace crypto ::openssl {
using ::openssl::bn_const_t;
using ::openssl::bn_t;
using ::openssl::log_error;
using ::openssl::sha_256;
using ::openssl::sha_256_digest_t;
using ::openssl::verify;

bool check_iso2_signature(const struct iso2_SignatureType* iso2_signature, EVP_PKEY* pkey,
                          struct iso2_exiFragment* iso2_exi_fragment) {
    assert(pkey != nullptr);
    assert(iso2_signature != nullptr);
    assert(iso2_exi_fragment != nullptr);

    bool bRes{true};

    // signature information
    const struct iso2_ReferenceType* req_ref = &iso2_signature->SignedInfo.Reference.array[0];
    const auto signature_len = iso2_signature->SignatureValue.CONTENT.bytesLen;
    const auto* signature = &iso2_signature->SignatureValue.CONTENT.bytes[0];

    // build data to check signature against
    std::array<std::uint8_t, MAX_EXI_SIZE> exi_buffer{};
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, exi_buffer.data(), MAX_EXI_SIZE, 0, NULL);

    auto err = encode_iso2_exiFragment(&stream, iso2_exi_fragment);
    if (err != 0) {
        dlog(DLOG_LEVEL_ERROR, "Unable to encode fragment, error code = %d", err);
        bRes = false;
    }

    sha_256_digest_t digest;

    // calculate hash of data
    if (bRes) {
        const auto frag_data_len = exi_bitstream_get_length(&stream);
        bRes = sha_256(exi_buffer.data(), frag_data_len, digest);
    }

    // check hash matches the value in the message
    if (bRes) {
        if (req_ref->DigestValue.bytesLen != digest.size()) {
            dlog(DLOG_LEVEL_ERROR, "Invalid digest length %u in signature", req_ref->DigestValue.bytesLen);
            bRes = false;
        }
    }
    if (bRes) {
        if (std::memcmp(req_ref->DigestValue.bytes, digest.data(), digest.size()) != 0) {
            dlog(DLOG_LEVEL_ERROR, "Invalid digest in signature");
            bRes = false;
        }
    }

    // verify the signature
    if (bRes) {
        struct iso2_xmldsigFragment sig_fragment {};
        init_iso2_xmldsigFragment(&sig_fragment);
        sig_fragment.SignedInfo_isUsed = 1;
        sig_fragment.SignedInfo = iso2_signature->SignedInfo;

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
            bRes = false;
        }
    }

    if (bRes) {
        // hash again (different data) buffer_pos has been updated ...
        const auto frag_data_len = exi_bitstream_get_length(&stream);
        bRes = sha_256(exi_buffer.data(), frag_data_len, digest);
    }

    if (bRes) {
        /* Validate the ecdsa signature using the public key */
        if (signature_len != ::openssl::signature_size) {
            dlog(DLOG_LEVEL_ERROR, "Signature len is invalid (%i)", signature_len);
            bRes = false;
        }
    }

    if (bRes) {
        const std::uint8_t* r = &signature[0];
        const std::uint8_t* s = &signature[32];
        bRes = verify(pkey, r, s, digest);
    }

    return bRes;
}

bool load_contract_root_cert(::openssl::certificate_list& trust_anchors, const char* V2G_file_path,
                             const char* MO_file_path) {
    trust_anchors.clear();

    auto mo_certs = ::openssl::load_certificates(MO_file_path);
    trust_anchors = std::move(mo_certs);

    auto v2g_certs = ::openssl::load_certificates(V2G_file_path);
    trust_anchors.insert(trust_anchors.end(), std::make_move_iterator(v2g_certs.begin()),
                         std::make_move_iterator(v2g_certs.end()));

    if (trust_anchors.empty()) {
        log_error("Unable to load any MO or V2G root(s)");
    }

    return !trust_anchors.empty();
}

int load_certificate(::openssl::certificate_list* chain, const std::uint8_t* bytes, std::uint16_t bytesLen) {
    assert(chain != nullptr);
    int result{-1};

    auto tmp_cert = ::openssl::der_to_certificate(bytes, bytesLen);
    if (tmp_cert != nullptr) {
        chain->push_back(std::move(tmp_cert));
        result = 0;
    }

    return result;
}

int parse_contract_certificate(::openssl::certificate_ptr& crt, const std::uint8_t* buf, std::size_t buflen) {
    crt = ::openssl::der_to_certificate(buf, buflen);
    return (crt == nullptr) ? -1 : 0;
}

std::string getEmaidFromContractCert(const ::openssl::certificate_ptr& crt) {
    std::string cert_emaid;
    const auto subject = ::openssl::certificate_subject(crt.get());
    if (auto itt = subject.find("CN"); itt != subject.end()) {
        cert_emaid = itt->second;
    }

    return cert_emaid;
}

std::string chain_to_pem(const ::openssl::certificate_ptr& cert, const ::openssl::certificate_list* chain) {
    assert(chain != nullptr);

    std::string contract_cert_chain_pem(::openssl::certificate_to_pem(cert.get()));
    for (const auto& crt : *chain) {
        const auto pem = ::openssl::certificate_to_pem(crt.get());
        if (pem.empty()) {
            dlog(DLOG_LEVEL_ERROR, "Unable to encode certificate chain");
            break;
        }
        contract_cert_chain_pem.append(pem);
    }

    return contract_cert_chain_pem;
}

verify_result_t verify_certificate(const ::openssl::certificate_ptr& cert, const ::openssl::certificate_list* chain,
                                   const char* v2g_root_cert_path, const char* mo_root_cert_path,
                                   bool /* debugMode */) {
    assert(chain != nullptr);

    verify_result_t result{verify_result_t::Verified};
    ::openssl::certificate_list trust_anchors;

    if (!load_contract_root_cert(trust_anchors, v2g_root_cert_path, mo_root_cert_path)) {
        result = verify_result_t::NoCertificateAvailable;
    } else {
        result = ::openssl::verify_certificate(cert.get(), trust_anchors, *chain);
    }

    return result;
}

} // namespace crypto::openssl
