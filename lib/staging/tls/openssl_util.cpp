// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "openssl_util.hpp"

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/store.h>
#include <openssl/types.h>
#include <openssl/ui.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

namespace {

openssl::log_handler_t s_log_handler{nullptr}; //!< logs are passed to this function

/**
 * \brief add OpenSSL error information to the string
 * \param[in] str is the OpenSSL \n\0 terminated error string
 * \param[in] len is the length of the string including the \n
 * \param[in] u is user data - a std::string to append to
 * \return 0 on success
 */
int add_error_str(const char* str, std::size_t len, void* u) {
    assert(u != nullptr);
    auto* list = reinterpret_cast<std::string*>(u);
    *list += '\n' + std::string(str, len - 1);
    return 0;
}

/**
 * \brief present a password for private key files
 * \param[in] buf is a pointer for where to place the password
 * \param[in] size is the size of buf i.e. max password size
 * \param[in] rwflag to indicate whether a file os being read or written
 * \param[in] u is user data which must be set to the required password
 * \return the length of the password written to buf or -1 on error
 * \note this callback is used to prevent attempts to prompt for a password
 *       from a terminal
 */
int password_cb(char* buf, int size, int rwflag, void* u) {
    int result{-1};

    if ((u != nullptr) && (buf != nullptr)) {
        const auto len = std::strlen(static_cast<const char*>(u));
        if (len <= size) {
            std::memcpy(buf, u, len);
            result = len;
        }
    }

    return result;
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

/**
 * \brief calculate a SHA digest
 * \param[in] data is the data to hash
 * \param[in] len is the length of the data to hash
 * \param[out] digest is the calculated digest
 * \param[in] HASH is the type of digest e.g. SHA1, SHA256 ...
 * \return true on success
 */
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

template <> bool sha(const void* data, std::size_t len, openssl::sha_1_digest_t& digest) {
    return sha_impl(data, len, digest, EVP_sha1());
}
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

DER::DER(std::size_t size) : DER(nullptr, size) {
}

DER::DER(const der_underlying_t* src, std::size_t size) {
    auto* tmp = static_cast<std::uint8_t*>(OPENSSL_malloc(size));
    if (tmp != nullptr) {
        ptr = der_ptr{tmp, &DER::free};
        len = size;
        if (src != nullptr) {
            std::memcpy(ptr.get(), src, size);
        } else {
            std::memset(ptr.get(), 0, size);
        }
    }
}

DER::DER(const DER& obj) : DER(obj.ptr.get(), obj.len) {
}

DER& DER::operator=(const DER& obj) {
    if (&obj != this) {
        *this = DER(obj);
    }
    return *this;
}

DER::DER(DER&& obj) noexcept : ptr(std::move(obj.ptr)), len(obj.len) {
    obj.len = 0;
}

DER& DER::operator=(DER&& obj) noexcept {
    if (this != &obj) {
        ptr = std::move(obj.ptr);
        len = obj.len;
        obj.len = 0;
    }
    return *this;
}

bool DER::operator==(const DER& rhs) const {
    if (&rhs == this) {
        return true;
    }

    bool result{false};
    const auto* lhs_p = ptr.get();
    const auto* rhs_p = rhs.ptr.get();

    if ((lhs_p != nullptr) && (rhs_p != nullptr)) {
        result = len == rhs.len;
        result = result && (std::memcmp(lhs_p, rhs_p, len) == 0);
    }
    return result;
}

bool DER::operator==(const der_underlying_t* rhs) const {
    return ptr.get() == rhs;
}

DER::operator bool() const {
    return (ptr.get() != nullptr) && (len > 0);
}

der_underlying_t* DER::dup(const DER& obj) {
    auto* ptr = static_cast<std::uint8_t*>(OPENSSL_malloc(obj.len));
    if ((ptr != nullptr) && (obj.ptr != nullptr)) {
        std::memcpy(ptr, obj.ptr.get(), obj.len);
    }
    return ptr;
}

void DER::free(der_underlying_t* ptr) {
    OPENSSL_free(ptr);
}

bool sign(EVP_PKEY* pkey, bn_t& r, bn_t& s, const sha_256_digest_t& digest) {
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

bool verify(EVP_PKEY* pkey, const bn_t& r, const bn_t& s, const sha_256_digest_t& digest) {
    return verify(pkey, r.data(), s.data(), digest);
}

bool verify(EVP_PKEY* pkey, const std::uint8_t* r, const std::uint8_t* s, const sha_256_digest_t& digest) {
    bool bRes{false};
    auto signature = bn_to_signature(r, s);
    if (signature) {
        bRes = verify(pkey, signature.get(), signature.size(), digest.data(), sha_256_digest_size);
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

bool sha_1(const void* data, std::size_t len, sha_1_digest_t& digest) {
    return sha(data, len, digest);
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

pkey_ptr load_private_key(const char* filename, const char* password) {
    pkey_ptr private_key{nullptr, nullptr};
    auto* bio = BIO_new_file(filename, "r");
    if (bio != nullptr) {
        // password is passed to password_cb() as parameter u which is never
        // written to, hence const_cast is okay
        auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, &password_cb, const_cast<char*>(password));
        if (pkey != nullptr) {
            private_key = pkey_ptr{pkey, &EVP_PKEY_free};
        }
        BIO_free(bio);
    }
    return private_key;
}

DER bn_to_signature(const bn_t& r, const bn_t& s) {
    return bn_to_signature(r.data(), s.data());
};

DER bn_to_signature(const std::uint8_t* r, const std::uint8_t* s) {
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
    // move sig_p to DER
    return {der_ptr{sig_p, &DER::free}, signature_len};
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

certificate_list load_certificates(const char* filename) {
    certificate_list result{};
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

certificate_list load_certificates(const std::vector<const char*>& filenames) {
    certificate_list result{};
    for (const auto* i : filenames) {
        auto tmp = load_certificates(i);
        std::move(tmp.begin(), tmp.end(), std::back_inserter(result));
    }
    return result;
}

chain_info_t load_certificates(const char* leaf_file, const char* chain_file, const char* root_file) {
    certificate_ptr leaf_cert{nullptr, nullptr};
    auto leaf = load_certificates(leaf_file);
    auto chain = load_certificates(chain_file);
    auto root = load_certificates(root_file);

    if (leaf.empty()) {
        if (!chain.empty()) {
            leaf_cert.swap(chain[0]);
            chain.erase(chain.begin());
        }
    } else {
        leaf_cert.swap(leaf[0]);
    }

    if (leaf_cert && !root.empty()) {
        if (verify_certificate(leaf_cert.get(), root, chain) == verify_result_t::Verified) {
            return {std::move(leaf_cert), std::move(chain), std::move(root)};
        }
    }

    return {{nullptr, nullptr}, {}, {}};
}

chain_info_list_t load_certificates(const chain_filenames_list_t& chains) {
    chain_info_list_t result;
    result.reserve(chains.size());
    for (const auto& chain : chains) {
        result.emplace_back(load_certificates(chain));
    }
    return result;
}

bool verify_certificate_key(const X509* cert, const EVP_PKEY* pkey) {
    return X509_check_private_key(cert, pkey) == 1;
}

bool verify_chain(const chain_info_t& chain) {
    return verify_certificate(chain.leaf.get(), chain.trust_anchors, chain.chain) == verify_result_t::Verified;
}

bool verify_chain(const chain_t& chain) {
    bool result = verify_chain(chain.chain);
    result = result && verify_certificate_key(chain.chain.leaf.get(), chain.private_key.get());
    return result;
}

bool use_certificate_and_key(SSL* ssl, const chain_t& chain) {
    assert(ssl != nullptr);

    bool result{false};
    SSL_certs_clear(ssl);
    auto* untrusted = sk_X509_new_null();
    if (untrusted == nullptr) {
        log_error("sk_X509_new_null");
    } else {
        for (const auto& cert : chain.chain.chain) {
            if (X509_add_cert(untrusted, cert.get(),
                              X509_ADD_FLAG_UP_REF | X509_ADD_FLAG_NO_DUP | X509_ADD_FLAG_NO_SS) != 1) {
                log_error("X509_add_cert");
            }
        }
        result = SSL_use_cert_and_key(ssl, chain.chain.leaf.get(), chain.private_key.get(), untrusted, 1) == 1;
        if (!result) {
            log_error("SSL_use_cert_and_key");
        }
        sk_X509_pop_free(untrusted, X509_free);
    }
    return result;
}

std::string certificate_to_pem(const X509* cert) {
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

certificate_ptr der_to_certificate(const std::uint8_t* der, std::size_t len) {
    certificate_ptr result{nullptr, nullptr};
    const auto* ptr = der;
    auto* cert = d2i_X509(nullptr, &ptr, static_cast<std::int64_t>(len));
    if (cert == nullptr) {
        log_error("d2i_X509");
    } else {
        result = certificate_ptr{cert, &X509_free};
    }
    return result;
}

verify_result_t verify_certificate(const X509* cert, const certificate_list& trust_anchors,
                                   const certificate_list& untrusted) {
    verify_result_t result = verify_result_t::Verified;
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

    if (result == verify_result_t::Verified) {
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
                result = verify_result_t::Verified;
            }
        }
    }

    X509_STORE_CTX_free(store_ctx);
    X509_STORE_free(ta_store);
    sk_X509_pop_free(chain, X509_free);
    X509_free(target);
    return result;
}

std::map<std::string, std::string> certificate_subject(const X509* cert) {
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

DER certificate_subject_der(const X509* cert) {
    assert(cert != nullptr);

    int len{0};
    unsigned char* data{nullptr};

    // DO NOT FREE - internal pointers to certificate
    const auto* subject = X509_get_subject_name(cert);
    if (subject != nullptr) {
        len = i2d_X509_NAME(subject, &data);
    }

    // move data to DER
    return {der_ptr{data, &DER::free}, static_cast<std::size_t>(len)};
}

pkey_ptr certificate_public_key(X509* cert) {
    pkey_ptr result{nullptr, nullptr};
    auto* pkey = X509_get_pubkey(cert);
    if (pkey == nullptr) {
        log_error("X509_get_pubkey");
    } else {
        result = pkey_ptr(pkey, &EVP_PKEY_free);
    }
    return result;
}

bool certificate_sha_1(openssl::sha_1_digest_t& digest, const X509* cert) {
    assert(cert != nullptr);

    bool bResult{false};
    const ASN1_BIT_STRING* signature{nullptr};
    const X509_ALGOR* alg{nullptr};
    X509_get0_signature(&signature, &alg, cert);
    if (signature != nullptr) {
        unsigned char* data{nullptr};
        const auto len = i2d_ASN1_BIT_STRING(signature, &data);
        if (len > 0) {
            bResult = openssl::sha_1(data, len, digest);
        }
        OPENSSL_free(data);
    }

    return bResult;
}

bool certificate_subject_public_key_sha_1(openssl::sha_1_digest_t& digest, const X509* cert) {
    assert(cert != nullptr);

    bool bResult{false};
    const auto* pubkey = X509_get_X509_PUBKEY(cert);
    if (pubkey != nullptr) {
        unsigned char* data{nullptr};
        const auto len = i2d_X509_PUBKEY(pubkey, &data);
        if (len > 0) {
            bResult = openssl::sha_1(data, len, digest);
        }
        OPENSSL_free(data);
    }

    return bResult;
}

} // namespace openssl
