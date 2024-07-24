// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <string>
#include <tuple>

#include "openssl_util.hpp"

#include <openssl/bio.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/store.h>
#include <openssl/ui.h>
#include <openssl/x509_vfy.h>

namespace {

openssl::log_handler_t s_log_handler{nullptr};

int add_error_str(const char* str, std::size_t len, void* u) {
    assert(u != nullptr);
    auto* list = reinterpret_cast<std::string*>(u);
    *list += '\n' + std::string(str, len);
    return 0;
}
} // namespace

namespace openssl {

void log(log_level_t level, const std::string& str) {
    std::string messages = {str};
    ERR_print_errors_cb(&add_error_str, &messages);
    if (s_log_handler == nullptr) {
        std::cerr << messages << std::endl;
    } else {
        s_log_handler(level, messages);
    }
}

log_handler_t set_log_handler(log_handler_t handler) {
    const auto tmp = s_log_handler;
    s_log_handler = handler;
    return tmp;
}

} // namespace openssl

namespace {

void DER_Signature_free(std::uint8_t* ptr) {
    OPENSSL_free(ptr);
}

template <typename DIGEST> bool sha_impl(const void* data, std::size_t len, DIGEST& digest, const EVP_MD* HASH) {
    std::array<std::uint8_t, EVP_MAX_MD_SIZE> buffer{};
    unsigned int digestlen{0};
    const auto res = EVP_Digest(data, len, buffer.data(), &digestlen, HASH, nullptr);
    if (res == 1) {
        if (digestlen == digest.size()) {
            std::memcpy(digest.data(), buffer.data(), digest.size());
        } else {
            openssl::log_error("EVP_Digest - size");
        }
    } else {
        openssl::log_error("EVP_Digest");
    }
    return res == 1;
}

template <typename DIGEST> bool sha(const void* data, std::size_t len, DIGEST& digest);

template <> bool sha(const void* data, std::size_t len, openssl::sha_256_digest_t& digest) {
    return sha_impl(data, len, digest, EVP_sha256());
}
template <> bool sha(const void* data, std::size_t len, openssl::sha_384_digest_t& digest) {
    return sha_impl<openssl::sha_384_digest_t>(data, len, digest, EVP_sha384());
}
template <> bool sha(const void* data, std::size_t len, openssl::sha_512_digest_t& digest) {
    return sha_impl(data, len, digest, EVP_sha512());
}

} // namespace

namespace openssl {

bool sign(evp_pkey_st* pkey, bn_t& r, bn_t& s, const sha_256_digest_t& digest) {
    bool bRes{false};
    std::array<std::uint8_t, signature_der_size> signature{};
    auto len = signature.size();
    bRes = sign(pkey, signature.data(), len, digest.data(), sha_256_digest_size);
    if (bRes) {
        bRes = signature_to_bn(r, s, signature.data(), len);
    }
    return bRes;
}

bool sign(EVP_PKEY* pkey, unsigned char* sig, std::size_t& siglen, const unsigned char* tbs, std::size_t tbslen) {
    bool bRes{true};

    auto* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        log_error("EVP_PKEY_CTX_new");
        bRes = false;
    }
    if (bRes && (EVP_PKEY_sign_init(ctx) != 1)) {
        log_error("EVP_PKEY_sign_init");
        bRes = false;
    }
    if (bRes && (EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256()) != 1)) {
        log_error("EVP_PKEY_CTX_set_signature_md");
        bRes = false;
    }
    if (bRes) {
        // calculate signature size
        std::size_t length{0};
        if (EVP_PKEY_sign(ctx, nullptr, &length, tbs, tbslen) != 1) {
            log_error("EVP_PKEY_sign - length");
            bRes = false;
        } else if (siglen < length) {
            log_error("EVP_PKEY_sign - length too small: " + std::to_string(length));
            bRes = false;
        }
        if (bRes) {
            const auto res = EVP_PKEY_sign(ctx, sig, &siglen, tbs, tbslen);
            if (res != 1) {
                log_error("EVP_PKEY_sign" + std::to_string(res));
                bRes = false;
            }
        }
    }
    EVP_PKEY_CTX_free(ctx);
    return bRes;
}

bool verify(evp_pkey_st* pkey, const bn_t& r, const bn_t& s, const sha_256_digest_t& digest) {
    return verify(pkey, r.data(), s.data(), digest);
}

bool verify(evp_pkey_st* pkey, const std::uint8_t* r, const std::uint8_t* s, const sha_256_digest_t& digest) {
    bool bRes{false};
    auto [signature, len] = bn_to_signature(r, s);
    if ((signature != nullptr) && (len > 0)) {
        bRes = verify(pkey, signature.get(), len, digest.data(), sha_256_digest_size);
    }
    return bRes;
}

bool verify(EVP_PKEY* pkey, const unsigned char* sig, std::size_t siglen, const unsigned char* tbs,
            std::size_t tbslen) {
    bool bRes{true};

    auto* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (ctx == nullptr) {
        log_error("EVP_PKEY_CTX_new");
        bRes = false;
    }
    if (bRes && (EVP_PKEY_verify_init(ctx) != 1)) {
        log_error("EVP_PKEY_verify_init");
        bRes = false;
    }
    if (bRes && (EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256()) != 1)) {
        log_error("EVP_PKEY_CTX_set_signature_md");
        bRes = false;
    }
    if (bRes) {
        const auto res = EVP_PKEY_verify(ctx, sig, siglen, tbs, tbslen);
        if (res != 1) {
            log_error("EVP_PKEY_verify: " + std::to_string(res));
            bRes = false;
        }
    }
    EVP_PKEY_CTX_free(ctx);
    return bRes;
}

bool sha_256(const void* data, std::size_t len, sha_256_digest_t& digest) {
    return sha(data, len, digest);
}

bool sha_384(const void* data, std::size_t len, sha_384_digest_t& digest) {
    return sha(data, len, digest);
}

bool sha_512(const void* data, std::size_t len, sha_512_digest_t& digest) {
    return sha(data, len, digest);
}

std::vector<std::uint8_t> base64_decode(const char* text, std::size_t len) {
    assert(text != nullptr);
    assert(len > 0);

    // remove \n
    auto input = std::make_unique<std::uint8_t[]>(len);
    std::size_t input_len{0};

    for (std::size_t i = 0; i < len; i++) {
        const auto item = text[i];
        if (item != '\n') {
            input.get()[input_len++] = item;
        }
    }

    auto* b64 = BIO_new(BIO_f_base64());
    auto* mem = BIO_new_mem_buf(input.get(), static_cast<int>(input_len));
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64, mem);

    std::size_t output_len{0};
    int read_len{0};
    std::array<char, 256> buffer{};
    std::vector<std::uint8_t> result(len);

    while ((read_len = BIO_read(b64, buffer.data(), buffer.size())) > 0) {
        if ((output_len + read_len) <= result.size()) {
            std::memcpy(&result[output_len], buffer.data(), read_len);
            output_len += static_cast<std::size_t>(read_len);
        } else {
            // decoded data is larger than the input - can't happen!
            output_len = 0;
            break;
        }
    }

    result.resize(output_len);
    BIO_free_all(b64);
    return result;
}

bool base64_decode(const char* text, std::size_t len, std::uint8_t* out_data, std::size_t& out_len) {
    assert(out_data != nullptr);

    bool bResult = false;
    auto res = base64_decode(text, len);
    if ((res.size() > 0) && (res.size() <= out_len)) {
        std::memcpy(out_data, res.data(), res.size());
        out_len = res.size();
        bResult = true;
    }
    return bResult;
}

std::string base64_encode(const std::uint8_t* data, std::size_t len, bool newLine) {
    assert(data != nullptr);
    assert(len > 0);

    auto* b64 = BIO_new(BIO_f_base64());
    auto* mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    BIO_write(b64, data, static_cast<int>(len));
    BIO_flush(b64);

    char* ptr{nullptr};
    const auto size = BIO_get_mem_data(mem, &ptr);

    std::string result(ptr, size);
    BIO_free_all(b64);
    return result;
}

std::tuple<DER_Signature_ptr, std::size_t> bn_to_signature(const bn_t& r, const bn_t& s) {
    return bn_to_signature(r.data(), s.data());
};

std::tuple<DER_Signature_ptr, std::size_t> bn_to_signature(const std::uint8_t* r, const std::uint8_t* s) {
    std::uint8_t* sig_p{nullptr};
    std::size_t signature_len{0};
    BIGNUM* rbn{nullptr};
    BIGNUM* sbn{nullptr};

    auto* signature = ECDSA_SIG_new();
    if (signature == nullptr) {
        log_error("ECDSA_SIG_new");
    } else {
        rbn = BN_bin2bn(r, signature_n_size, nullptr);
        sbn = BN_bin2bn(s, signature_n_size, nullptr);
    }

    if (rbn != nullptr && sbn != nullptr) {
        if (ECDSA_SIG_set0(signature, rbn, sbn) == 1) {
            /* Set these to NULL since they are now owned by obj */
            rbn = sbn = nullptr;
            signature_len = i2d_ECDSA_SIG(signature, &sig_p);
            if (signature_len == 0) {
                log_error("i2d_ECDSA_SIG");
            }
        } else {
            log_error("ECDSA_SIG_set0");
        }
    }

    BN_free(rbn);
    BN_free(sbn);
    ECDSA_SIG_free(signature);
    return {DER_Signature_ptr(sig_p, &DER_Signature_free), signature_len};
};

bool signature_to_bn(bn_t& r, bn_t& s, const std::uint8_t* sig_p, std::size_t len) {
    bool bRes{false};

    auto* signature = d2i_ECDSA_SIG(nullptr, &sig_p, static_cast<long>(len));
    if (signature == nullptr) {
        log_error("d2i_ECDSA_SIG");
    } else {
        const auto* rbn = ECDSA_SIG_get0_r(signature);
        const auto* sbn = ECDSA_SIG_get0_s(signature);

        bRes = BN_bn2binpad(rbn, r.data(), static_cast<int>(r.size())) != -1;
        bRes = bRes && BN_bn2binpad(sbn, s.data(), static_cast<int>(s.size())) != -1;
        if (!bRes) {
            log_error("BN_bn2binpad");
        }
    }

    ECDSA_SIG_free(signature);
    return bRes;
};

std::vector<Certificate_ptr> load_certificates(const char* filename) {
    std::vector<Certificate_ptr> result{};

    if (filename != nullptr) {
        auto* store = OSSL_STORE_open(filename, UI_null(), nullptr, nullptr, nullptr);
        if (store != nullptr) {
            while (OSSL_STORE_eof(store) != 1) {
                auto* info = OSSL_STORE_load(store);

                if (info != nullptr) {
                    if (OSSL_STORE_error(store) == 1) {
                        log_error("OSSL_STORE_load");
                    } else {
                        const auto type = OSSL_STORE_INFO_get_type(info);

                        if (type == OSSL_STORE_INFO_CERT) {
                            // get a copy of the certificate
                            auto cert = OSSL_STORE_INFO_get1_CERT(info);
                            result.push_back({cert, &X509_free});
                        }
                    }
                }

                OSSL_STORE_INFO_free(info);
            }
        }

        OSSL_STORE_close(store);
    }
    return result;
}

std::string certificate_to_pem(const x509_st* cert) {
    assert(cert != nullptr);

    auto* mem = BIO_new(BIO_s_mem());
    std::string result;
    if (PEM_write_bio_X509(mem, cert) != 1) {
        log_error("PEM_write_bio_X509");
    } else {
        BIO_flush(mem);

        char* ptr{nullptr};
        const auto size = BIO_get_mem_data(mem, &ptr);

        result = std::string(ptr, size);
    }
    BIO_free(mem);
    return result;
}

Certificate_ptr der_to_certificate(const std::uint8_t* der, std::size_t len) {
    Certificate_ptr result{nullptr, nullptr};
    const auto* ptr = der;
    auto* cert = d2i_X509(nullptr, &ptr, static_cast<std::int64_t>(len));
    if (cert == nullptr) {
        log_error("d2i_X509");
    } else {
        result = Certificate_ptr{cert, &X509_free};
    }
    return result;
}

verify_result_t verify_certificate(const x509_st* cert, const CertificateList& trust_anchors,
                                   const CertificateList& untrusted) {
    verify_result_t result = verify_result_t::verified;
    auto* store_ctx = X509_STORE_CTX_new();
    auto* ta_store = X509_STORE_new();
    auto* chain = sk_X509_new_null();
    X509* target{nullptr};

    if (store_ctx == nullptr) {
        log_error("X509_STORE_CTX_new");
        result = verify_result_t::OtherError;
    }

    if (ta_store == nullptr) {
        log_error("X509_STORE_new");
        result = verify_result_t::OtherError;
    }

    if (chain == nullptr) {
        log_error("sk_X509_new_null");
        result = verify_result_t::OtherError;
    }

    if (cert != nullptr) {
        target = X509_dup(cert);
        if (target == nullptr) {
            log_error("X509_dup");
            result = verify_result_t::OtherError;
        }
    }

    if (result == verify_result_t::verified) {
        result = verify_result_t::OtherError;

        for (const auto& i : trust_anchors) {
            if (X509_STORE_add_cert(ta_store, i.get()) != 1) {
                log_error("X509_STORE_add_cert");
            }
        }

        for (const auto& j : untrusted) {
            if (X509_add_cert(chain, j.get(), X509_ADD_FLAG_UP_REF | X509_ADD_FLAG_NO_DUP | X509_ADD_FLAG_NO_SS) != 1) {
                log_error("X509_add_cert");
            }
        }

        if (X509_STORE_CTX_init(store_ctx, ta_store, target, chain) != 1) {
            log_error("X509_STORE_CTX_init");
        } else {
            if (X509_STORE_CTX_verify(store_ctx) != 1) {
                const auto err = X509_STORE_CTX_get_error(store_ctx);
                if (err != X509_V_OK) {
                    log_error("X509_STORE_CTX_verify (" + std::to_string(X509_STORE_CTX_get_error_depth(store_ctx)) +
                              ") " + X509_verify_cert_error_string(err));
                }

                switch (err) {
                case X509_V_ERR_CERT_CHAIN_TOO_LONG:
                case X509_V_ERR_CERT_SIGNATURE_FAILURE:
                case X509_V_ERR_CERT_UNTRUSTED:
                case X509_V_ERR_PATH_LENGTH_EXCEEDED:
                case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
                case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
                case X509_V_ERR_UNSPECIFIED:
                    result = verify_result_t::CertChainError;
                    break;
                case X509_V_ERR_CERT_HAS_EXPIRED:
                case X509_V_ERR_CERT_NOT_YET_VALID:
                    result = verify_result_t::CertificateExpired;
                    break;
                case X509_V_ERR_CERT_REVOKED:
                    result = verify_result_t::CertificateRevoked;
                    break;
                case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
                case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
                    result = verify_result_t::NoCertificateAvailable;
                    break;
                default:
                    break;
                }
            } else {
                result = verify_result_t::verified;
            }
        }
    }

    X509_STORE_CTX_free(store_ctx);
    X509_STORE_free(ta_store);
    sk_X509_pop_free(chain, X509_free);
    X509_free(target);
    return result;
}

std::map<std::string, std::string> certificate_subject(const x509_st* cert) {
    assert(cert != nullptr);
    std::map<std::string, std::string> result;

    // DO NOT FREE - internal pointers to certificate
    const auto* subject = X509_get_subject_name(cert);
    if (subject != nullptr) {
        for (int i = 0; i < X509_NAME_entry_count(subject); i++) {
            const auto* name_entry = X509_NAME_get_entry(subject, i);
            if (name_entry != nullptr) {
                const auto* object = X509_NAME_ENTRY_get_object(name_entry);
                const auto* data = X509_NAME_ENTRY_get_data(name_entry);
                if ((object != nullptr) && (data != nullptr)) {
                    std::string name(OBJ_nid2sn(OBJ_obj2nid(object)));
                    std::string value(reinterpret_cast<const char*>(ASN1_STRING_get0_data(data)),
                                      ASN1_STRING_length(data));
                    result[name] = value;
                }
            }
        }
    }

    return result;
}

PKey_ptr certificate_public_key(x509_st* cert) {
    PKey_ptr result{nullptr, nullptr};
    auto* pkey = X509_get_pubkey(cert);
    if (pkey == nullptr) {
        log_error("X509_get_pubkey");
    } else {
        result = PKey_ptr(pkey, &EVP_PKEY_free);
    }
    return result;
}

} // namespace openssl
