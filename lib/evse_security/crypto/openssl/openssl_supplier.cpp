// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <evse_security/crypto/openssl/openssl_supplier.hpp>
#include <evse_security/detail/openssl/openssl_types.hpp>
#include <evse_security/utils/evse_filesystem.hpp>

#include <everest/logging.hpp>

#include <chrono>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/x509v3.h>

namespace evse_security {

struct X509HandleOpenSSL : public X509Handle {
    X509HandleOpenSSL(X509* certificate) : x509(certificate) {
    }

    X509* get() {
        return x509.get();
    }

private:
    X509_ptr x509;
};

static X509* get(X509Handle* handle) {
    if (X509HandleOpenSSL* ssl_handle = dynamic_cast<X509HandleOpenSSL*>(handle)) {
        return ssl_handle->get();
    }

    return nullptr;
}

static CertificateValidationError to_certificate_error(const int ec) {
    switch (ec) {
    case X509_V_ERR_CERT_HAS_EXPIRED:
        return CertificateValidationError::Expired;
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
        return CertificateValidationError::InvalidSignature;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        return CertificateValidationError::IssuerNotFound;
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        return CertificateValidationError::InvalidLeafSignature;
    default:
        EVLOG_warning << X509_verify_cert_error_string(ec);
        return CertificateValidationError::Unknown;
    }
}

std::vector<X509Handle_ptr> OpenSSLSupplier::load_certificates(const std::string& data, const EncodingFormat encoding) {
    BIO_ptr bio(BIO_new_mem_buf(data.data(), static_cast<int>(data.size())));

    if (!bio) {
        throw CertificateLoadException("Failed to create BIO from data");
    }

    std::vector<X509Handle_ptr> certificates;

    if (encoding == EncodingFormat::PEM) {
        STACK_OF(X509_INFO)* allcerts = PEM_X509_INFO_read_bio(bio.get(), nullptr, nullptr, nullptr);

        if (allcerts) {
            for (int i = 0; i < sk_X509_INFO_num(allcerts); i++) {
                X509_INFO* xi = sk_X509_INFO_value(allcerts, i);

                if (xi && xi->x509) {
                    // Transfer ownership, safely, push_back since emplace_back can cause a memory leak
                    certificates.push_back(std::make_unique<X509HandleOpenSSL>(xi->x509));
                    xi->x509 = nullptr;
                }
            }

            sk_X509_INFO_pop_free(allcerts, X509_INFO_free);
        } else {
            throw CertificateLoadException("Certificate (PEM) parsing error");
        }
    } else if (encoding == EncodingFormat::DER) {
        X509* x509 = d2i_X509_bio(bio.get(), nullptr);

        if (x509) {
            certificates.push_back(std::make_unique<X509HandleOpenSSL>(x509));
        } else {
            throw CertificateLoadException("Certificate (DER) parsing error");
        }
    } else {
        throw CertificateLoadException("Unsupported encoding format");
    }

    return certificates;
}

std::string OpenSSLSupplier::x509_to_string(X509Handle* handle) {
    if (X509* x509 = get(handle)) {
        BIO_ptr bio_write(BIO_new(BIO_s_mem()));

        int rc = PEM_write_bio_X509(bio_write.get(), x509);

        if (rc == 1) {
            BUF_MEM* mem = NULL;
            BIO_get_mem_ptr(bio_write.get(), &mem);

            return std::string(mem->data, mem->length);
        }
    }

    return {};
}

std::string OpenSSLSupplier::x509_get_common_name(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return {};

    X509_NAME* subject = X509_get_subject_name(x509);
    int nid = OBJ_txt2nid("CN");
    int index = X509_NAME_get_index_by_NID(subject, nid, -1);

    if (index == -1) {
        return {};
    }

    X509_NAME_ENTRY* entry = X509_NAME_get_entry(subject, index);
    ASN1_STRING* ca_asn1 = X509_NAME_ENTRY_get_data(entry);

    if (ca_asn1 == nullptr) {
        return {};
    }

    const unsigned char* cn_str = ASN1_STRING_get0_data(ca_asn1);
    std::string common_name(reinterpret_cast<const char*>(cn_str), ASN1_STRING_length(ca_asn1));
    return common_name;
}

std::string OpenSSLSupplier::x509_get_issuer_name_hash(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return {};

    unsigned char md[SHA256_DIGEST_LENGTH];
    X509_NAME* name = X509_get_issuer_name(x509);
    X509_NAME_digest(name, EVP_sha256(), md, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)md[i];
    }
    return ss.str();
}

std::string OpenSSLSupplier::x509_get_serial_number(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return {};

    ASN1_INTEGER* serial_asn1 = X509_get_serialNumber(x509);
    if (serial_asn1 == nullptr) {
        ERR_print_errors_fp(stderr);
        return {};
    }

    BIGNUM* bn_serial = ASN1_INTEGER_to_BN(serial_asn1, NULL);

    if (bn_serial == nullptr) {
        ERR_print_errors_fp(stderr);
        return {};
    }

    char* hex_serial = BN_bn2hex(bn_serial);

    if (hex_serial == nullptr) {
        ERR_print_errors_fp(stderr);
        return {};
    }

    std::string serial(hex_serial);
    for (char& i : serial) {
        i = std::tolower(i);
    }

    BN_free(bn_serial);
    OPENSSL_free(hex_serial);

    serial.erase(0, std::min(serial.find_first_not_of('0'), serial.size() - 1));
    return serial;
}

std::string OpenSSLSupplier::x509_get_key_hash(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return {};

    unsigned char tmphash[SHA256_DIGEST_LENGTH];
    X509_pubkey_digest(x509, EVP_sha256(), tmphash, NULL);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)tmphash[i];
    }

    return ss.str();
}

std::string OpenSSLSupplier::x509_get_responder_url(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return {};

    const auto ocsp = X509_get1_ocsp(x509);
    std::string responder_url;
    for (int i = 0; i < sk_OPENSSL_STRING_num(ocsp); i++) {
        responder_url.append(sk_OPENSSL_STRING_value(ocsp, i));
    }

    if (responder_url.empty()) {
        EVLOG_warning << "Could not retrieve OCSP Responder URL from certificate";
    }

    return responder_url;
}

void OpenSSLSupplier::x509_get_validity(X509Handle* handle, std::int64_t& out_valid_in, std::int64_t& out_valid_to) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return;

    // For valid_in and valid_to
    ASN1_TIME* notBefore = X509_get_notBefore(x509);
    ASN1_TIME* notAfter = X509_get_notAfter(x509);

    int day, sec;
    ASN1_TIME_diff(&day, &sec, nullptr, notBefore);
    out_valid_in =
        std::chrono::duration_cast<std::chrono::seconds>(days_to_seconds(day)).count() + sec; // Convert days to seconds
    ASN1_TIME_diff(&day, &sec, nullptr, notAfter);
    out_valid_to =
        std::chrono::duration_cast<std::chrono::seconds>(days_to_seconds(day)).count() + sec; // Convert days to seconds
}

bool OpenSSLSupplier::x509_is_child(X509Handle* child, X509Handle* parent) {
    // A certif can't be it's own parent, use is_selfsigned if that is intended
    if (child == parent)
        return false;

    X509* x509_parent = get(parent);
    X509* x509_child = get(child);

    if (x509_parent == nullptr || x509_child == nullptr)
        return false;

    X509_STORE_ptr store(X509_STORE_new());
    X509_STORE_add_cert(store.get(), x509_parent);

    X509_STORE_CTX_ptr ctx(X509_STORE_CTX_new());
    X509_STORE_CTX_init(ctx.get(), store.get(), x509_child, NULL);

    // If the parent is not a self-signed certificate, assume we have a partial chain
    if (x509_is_selfsigned(parent) == false) {
        // TODO(ioan): see if this strict flag is required
        X509_STORE_CTX_set_flags(ctx.get(), X509_V_FLAG_X509_STRICT);
        X509_STORE_CTX_set_flags(ctx.get(), X509_V_FLAG_PARTIAL_CHAIN);
    }

    return (X509_verify_cert(ctx.get()) == 1);
}

bool OpenSSLSupplier::x509_is_selfsigned(X509Handle* handle) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return false;

// X509_self_signed() was added in OpenSSL 3.0, use a workaround for earlier versions
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    return (X509_self_signed(x509, 0) == 1);
#else
    return (X509_verify(x509, X509_get_pubkey(x509)));
#endif
}

bool OpenSSLSupplier::x509_is_equal(X509Handle* a, X509Handle* b) {
    return (X509_cmp(get(a), get(b)) == 0);
}

X509Handle_ptr OpenSSLSupplier::x509_duplicate_unique(X509Handle* handle) {
    return std::make_unique<X509HandleOpenSSL>(X509_dup(get(handle)));
}

CertificateValidationError OpenSSLSupplier::x509_verify_certificate_chain(X509Handle* target,
                                                                          const std::vector<X509Handle*>& parents,
                                                                          bool allow_future_certificates,
                                                                          const std::optional<fs::path> dir_path,
                                                                          const std::optional<fs::path> file_path) {
    X509_STORE_ptr store_ptr(X509_STORE_new());
    X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new());

    for (size_t i = 0; i < parents.size(); i++) {
        X509_STORE_add_cert(store_ptr.get(), get(parents[i]));
    }

    if (dir_path.has_value() || file_path.has_value()) {
        const char* c_dir_path = dir_path.has_value() ? dir_path.value().c_str() : nullptr;
        const char* c_file_path = file_path.has_value() ? file_path.value().c_str() : nullptr;

        X509_STORE_load_locations(store_ptr.get(), c_file_path, c_dir_path);
    }

    X509_STORE_CTX_init(store_ctx_ptr.get(), store_ptr.get(), get(target), NULL);

    if (allow_future_certificates) {
        // Manually check if cert is expired
        int day, sec;
        ASN1_TIME_diff(&day, &sec, nullptr, X509_get_notAfter(get(target)));
        if (day < 0 || sec < 0) {
            // certificate is expired
            return CertificateValidationError::Expired;
        }
        // certificate is not expired, but may not be valid yet. Since we allow future certs, disable time checks.
        X509_STORE_CTX_set_flags(store_ctx_ptr.get(), X509_V_FLAG_NO_CHECK_TIME);
    }

    // verifies the certificate chain based on ctx
    // verifies the certificate has not expired and is already valid
    if (X509_verify_cert(store_ctx_ptr.get()) != 1) {
        int ec = X509_STORE_CTX_get_error(store_ctx_ptr.get());
        return to_certificate_error(ec);
    }

    return CertificateValidationError::NoError;
}

bool OpenSSLSupplier::x509_check_private_key(X509Handle* handle, std::string private_key,
                                             std::optional<std::string> password) {
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return false;

    BIO_ptr bio(BIO_new_mem_buf(private_key.c_str(), -1));
    // Passing password string since if NULL is provided, the password CB will be called
    EVP_PKEY_ptr evp_pkey(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, (void*)password.value_or("").c_str()));

    if (!evp_pkey) {
        EVLOG_warning << "Invalid evp_pkey: " << private_key << " error: " << ERR_error_string(ERR_get_error(), NULL)
                      << " Password configured correctly?";

        return false;
    }

    return (X509_check_private_key(x509, evp_pkey.get()) == 1);
}

bool OpenSSLSupplier::x509_verify_signature(X509Handle* handle, const std::vector<std::byte>& signature,
                                            const std::vector<std::byte>& data) {
    // extract public key
    X509* x509 = get(handle);

    if (x509 == nullptr)
        return false;

    EVP_PKEY_ptr public_key_ptr(X509_get_pubkey(x509));

    if (!public_key_ptr.get()) {
        EVLOG_error << "Error during X509_get_pubkey";
        return false;
    }

    // verify file signature
    EVP_PKEY_CTX_ptr public_key_context_ptr(EVP_PKEY_CTX_new(public_key_ptr.get(), nullptr));

    if (!public_key_context_ptr.get()) {
        EVLOG_error << "Error setting up public key context";
        return false;
    }

    if (EVP_PKEY_verify_init(public_key_context_ptr.get()) <= 0) {
        EVLOG_error << "Error during EVP_PKEY_verify_init";
        return false;
    }

    if (EVP_PKEY_CTX_set_signature_md(public_key_context_ptr.get(), EVP_sha256()) <= 0) {
        EVLOG_error << "Error during EVP_PKEY_CTX_set_signature_md";
        return false;
    };

    int result = EVP_PKEY_verify(public_key_context_ptr.get(), reinterpret_cast<const unsigned char*>(signature.data()),
                                 signature.size(), reinterpret_cast<const unsigned char*>(data.data()), data.size());

    EVP_cleanup();

    if (result != 1) {
        EVLOG_error << "Failure to verify: " << result;
        return false;
    } else {
        EVLOG_debug << "Successful verification";
        return true;
    }
}

bool OpenSSLSupplier::x509_generate_csr(const CertificateSigningRequestInfo& generation_info, std::string& out_csr) {
    // csr req
    // Ignore deprecation warnings on the EC gen functions since we need OpenSSL 1.1 support
    X509_REQ_ptr x509ReqPtr(X509_REQ_new());
    EVP_PKEY_ptr evpKey(EVP_PKEY_new());
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    X509_NAME* x509Name = X509_REQ_get_subject_name(x509ReqPtr.get());

    // generate ec key pair
    EC_KEY_generate_key(ecKey);
    EVP_PKEY_assign_EC_KEY(evpKey.get(), ecKey);

    // write private key to file
    if (generation_info.private_key_file.has_value()) {
        BIO_ptr prkey_bio(BIO_new_file(generation_info.private_key_file.value().c_str(), "w"));

        int success;
        if (generation_info.private_key_pass.has_value()) {
            success = PEM_write_bio_PrivateKey(prkey_bio.get(), evpKey.get(), EVP_aes_128_cbc(), NULL, 0, NULL,
                                               (void*)generation_info.private_key_pass.value().c_str());
        } else {
            success = PEM_write_bio_PrivateKey(prkey_bio.get(), evpKey.get(), NULL, NULL, 0, NULL, NULL);
        }

        if (!success) {
            EVLOG_error << "Failed to write private key";
            return false;
        }
    }

    // set version of x509 req
    int n_version = generation_info.n_version;
    X509_REQ_set_version(x509ReqPtr.get(), n_version);

    // set subject of x509 req
    X509_NAME_add_entry_by_txt(x509Name, "C", MBSTRING_ASC,
                               reinterpret_cast<const unsigned char*>(generation_info.country.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "O", MBSTRING_ASC,
                               reinterpret_cast<const unsigned char*>(generation_info.organization.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "CN", MBSTRING_ASC,
                               reinterpret_cast<const unsigned char*>(generation_info.commonName.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "DC", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("CPO"), -1, -1, 0);
    // set public key of x509 req
    X509_REQ_set_pubkey(x509ReqPtr.get(), evpKey.get());

    STACK_OF(X509_EXTENSION)* extensions = sk_X509_EXTENSION_new_null();
    X509_EXTENSION* ext_key_usage = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, "digitalSignature, keyAgreement");
    X509_EXTENSION* ext_basic_constraints = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, "critical,CA:false");
    sk_X509_EXTENSION_push(extensions, ext_key_usage);
    sk_X509_EXTENSION_push(extensions, ext_basic_constraints);

    X509_REQ_add_extensions(x509ReqPtr.get(), extensions);

    // set sign key of x509 req
    X509_REQ_sign(x509ReqPtr.get(), evpKey.get(), EVP_sha256());

    // write csr
    BIO_ptr bio(BIO_new(BIO_s_mem()));
    PEM_write_bio_X509_REQ(bio.get(), x509ReqPtr.get());

    BUF_MEM* mem_csr = NULL;
    BIO_get_mem_ptr(bio.get(), &mem_csr);

    out_csr = std::string(mem_csr->data, mem_csr->length);
    return true;
}

bool OpenSSLSupplier::digest_file_sha256(const fs::path& path, std::vector<std::byte>& out_digest) {
    EVP_MD_CTX_ptr md_context_ptr(EVP_MD_CTX_create());
    if (!md_context_ptr.get()) {
        EVLOG_error << "Could not create EVP_MD_CTX";
        return false;
    }

    const EVP_MD* md = EVP_get_digestbyname("SHA256");
    if (EVP_DigestInit_ex(md_context_ptr.get(), md, nullptr) == 0) {
        EVLOG_error << "Error during EVP_DigestInit_ex";
        return false;
    }

    bool digest_error = false;

    unsigned int sha256_out_length;
    std::byte sha256_out[EVP_MAX_MD_SIZE];

    // calculate sha256 of file
    bool processed_file = filesystem_utils::process_file(
        path, BUFSIZ, [&](const std::byte* bytes, std::size_t read, bool last_chunk) -> bool {
            if (read > 0) {
                if (EVP_DigestUpdate(md_context_ptr.get(), bytes, read) == 0) {
                    EVLOG_error << "Error during EVP_DigestUpdate";
                    digest_error = true;
                    return false;
                }
            }

            if (last_chunk) {
                if (EVP_DigestFinal_ex(md_context_ptr.get(), reinterpret_cast<unsigned char*>(sha256_out),
                                       &sha256_out_length) == 0) {
                    EVLOG_error << "Error during EVP_DigestFinal_ex";
                    digest_error = true;
                    return false;
                }
            }

            return true;
        });

    if ((processed_file == false) || (digest_error == true)) {
        EVLOG_error << "Could not digest file at: " << path.string();
        return false;
    }

    out_digest.clear();
    out_digest.insert(std::end(out_digest), sha256_out, sha256_out + sha256_out_length);

    return true;
}

bool OpenSSLSupplier::decode_base64_signature(const std::string& signature, std::vector<std::byte>& out_decoded) {
    // decode base64 encoded signature
    EVP_ENCODE_CTX_ptr base64_decode_context_ptr(EVP_ENCODE_CTX_new());
    if (!base64_decode_context_ptr.get()) {
        EVLOG_error << "Error during EVP_ENCODE_CTX_new";
        return false;
    }

    EVP_DecodeInit(base64_decode_context_ptr.get());
    if (!base64_decode_context_ptr.get()) {
        EVLOG_error << "Error during EVP_DecodeInit";
        return false;
    }

    const unsigned char* signature_str = reinterpret_cast<const unsigned char*>(signature.data());
    int base64_length = signature.size();
    std::byte signature_out[base64_length];

    int signature_out_length;
    if (EVP_DecodeUpdate(base64_decode_context_ptr.get(), reinterpret_cast<unsigned char*>(signature_out),
                         &signature_out_length, signature_str, base64_length) < 0) {
        EVLOG_error << "Error during DecodeUpdate";
        return false;
    }

    int decode_final_out;
    if (EVP_DecodeFinal(base64_decode_context_ptr.get(), reinterpret_cast<unsigned char*>(signature_out),
                        &decode_final_out) < 0) {
        EVLOG_error << "Error during EVP_DecodeFinal";
        return false;
    }

    out_decoded.clear();
    out_decoded.insert(std::end(out_decoded), signature_out, signature_out + signature_out_length);

    return true;
}

} // namespace evse_security