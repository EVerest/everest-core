// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <fstream>
#include <iostream>
#include <regex>

#include <everest/logging.hpp>

#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <x509_wrapper.hpp>

namespace evse_security {

using BIO_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

std::string x509_to_string(X509* x509) {
    if (x509) {
        BIO_ptr bio_write(BIO_new(BIO_s_mem()), ::BIO_free);

        int rc = PEM_write_bio_X509(bio_write.get(), x509);

        if (rc == 1) {
            BUF_MEM* mem = NULL;
            BIO_get_mem_ptr(bio_write.get(), &mem);

            return std::string(mem->data, mem->length);
        }
    }

    return {};
}

void X509Wrapper::update_validity() {
    // For valid_in and valid_to
    ASN1_TIME* notBefore = X509_get_notBefore(x509[0].get());
    ASN1_TIME* notAfter = X509_get_notAfter(x509[0].get());
    int day, sec;
    ASN1_TIME_diff(&day, &sec, notBefore, nullptr);
    this->valid_in = day * 86400 + sec; // Convert days to seconds
    ASN1_TIME_diff(&day, &sec, nullptr, notAfter);
    this->valid_to = day * 86400 + sec; // Convert days to seconds
}

// Load a certificate from a string using the specified encoding.
void X509Wrapper::load_certificate(const std::string& data, const EncodingFormat encoding) {
    BIO_ptr bio(BIO_new_mem_buf(data.data(), static_cast<int>(data.size())), ::BIO_free);

    if (!bio) {
        throw CertificateLoadException("Failed to create BIO from data");
    }

    if (encoding == EncodingFormat::PEM) {
        STACK_OF(X509_INFO)* allcerts = PEM_X509_INFO_read_bio(bio.get(), nullptr, nullptr, nullptr);

        if (allcerts) {
            for (int i = 0; i < sk_X509_INFO_num(allcerts); i++) {
                X509_INFO* xi = sk_X509_INFO_value(allcerts, i);

                if (xi && xi->x509) {
                    // Transfer owneship
                    x509.emplace_back(xi->x509, ::X509_free);
                    xi->x509 = nullptr;
                }
            }

            sk_X509_INFO_pop_free(allcerts, X509_INFO_free);
        } else {
            throw CertificateLoadException("Unsupported encoding format");
        }
    } else if (encoding == EncodingFormat::DER) {
        x509.emplace_back(d2i_X509_bio(bio.get(), nullptr), ::X509_free);
    } else {
        throw CertificateLoadException("Unsupported encoding format");
    }

    if (!x509.size()) {
        throw CertificateLoadException("Failed to read X509 from BIO");
    }

    update_validity();
}

X509Wrapper::X509Wrapper(const std::string& certificate, const EncodingFormat encoding) {
    load_certificate(certificate, encoding);
}

X509Wrapper::X509Wrapper(const std::filesystem::path& path, const EncodingFormat encoding) {
    // TODO: Directory support
    if (std::filesystem::is_directory(path)) {

    } else {
        std::ifstream file(path, std::ios::binary);
        std::string certificate((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        load_certificate(certificate, encoding);
    }

    this->path = path;
}

X509Wrapper::X509Wrapper(X509* x509) {
    this->x509.emplace_back(x509, ::X509_free);
    update_validity();
}

X509Wrapper::X509Wrapper(const X509Wrapper& other) {
    for (const auto& cert : other.x509) {
        X509* dup_cert = X509_dup(cert.get());
        x509.emplace_back(dup_cert, ::X509_free);
    }

    if (other.path) {
        path = other.path.value();
    }

    valid_in = other.valid_in;
    valid_to = other.valid_to;
}

X509Wrapper::~X509Wrapper() {
}

X509* X509Wrapper::get() const {
    return x509[0].get();
}

void X509Wrapper::reset(X509* _x509) {
    x509[0].reset(_x509);
}

std::vector<X509Wrapper> X509Wrapper::split() {
    std::vector<X509Wrapper> certificates;

    for (const auto& cert : x509) {
        // Duplicates since a X509Wrapper requires exclusive ownership
        X509* dup_cert = X509_dup(cert.get());
        certificates.emplace_back(dup_cert);

        if (this->path.has_value()) {
            certificates.back().path = this->path.value();
        }
    }

    return certificates;
}

int X509Wrapper::get_valid_in() const {
    return this->valid_in;
}

/// \brief Gets valid_in
int X509Wrapper::get_valid_to() const {
    return this->valid_to;
}

std::string X509Wrapper::get_str() const {
    return this->to_base64_string();
}

std::optional<std::filesystem::path> X509Wrapper::get_path() const {
    return this->path;
}

std::string X509Wrapper::get_common_name() const {
    X509_NAME* subject = X509_get_subject_name(this->x509[0].get());
    int nid = OBJ_txt2nid("CN");
    int index = X509_NAME_get_index_by_NID(subject, nid, -1);

    if (index == -1) {
        return "";
    }

    X509_NAME_ENTRY* entry = X509_NAME_get_entry(subject, index);
    ASN1_STRING* ca_asn1 = X509_NAME_ENTRY_get_data(entry);

    if (ca_asn1 == nullptr) {
        return "";
    }

    const unsigned char* cn_str = ASN1_STRING_get0_data(ca_asn1);
    std::string common_name(reinterpret_cast<const char*>(cn_str), ASN1_STRING_length(ca_asn1));
    return common_name;
}

std::string X509Wrapper::get_issuer_name_hash() const {
    unsigned char md[SHA256_DIGEST_LENGTH];
    X509_NAME* name = X509_get_issuer_name(this->x509[0].get());
    X509_NAME_digest(name, EVP_sha256(), md, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)md[i];
    }
    return ss.str();
}

std::string X509Wrapper::get_serial_number() const {
    ASN1_INTEGER* serial_asn1 = X509_get_serialNumber(this->x509[0].get());
    if (serial_asn1 == nullptr) {
        ERR_print_errors_fp(stderr);
        return "";
    }

    BIGNUM* bn_serial = ASN1_INTEGER_to_BN(serial_asn1, NULL);

    if (bn_serial == nullptr) {
        ERR_print_errors_fp(stderr);
        return "";
    }

    char* hex_serial = BN_bn2hex(bn_serial);

    if (hex_serial == nullptr) {
        ERR_print_errors_fp(stderr);
        return "";
    }

    std::string serial(hex_serial);

    BN_free(bn_serial);

    serial.erase(0, std::min(serial.find_first_not_of('0'), serial.size() - 1));
    return serial;
}

std::string X509Wrapper::get_issuer_key_hash() const {
    unsigned char tmphash[SHA256_DIGEST_LENGTH];
    X509_pubkey_digest(this->x509[0].get(), EVP_sha256(), tmphash, NULL);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)tmphash[i];
    }
    return ss.str();
}

CertificateHashData X509Wrapper::get_certificate_hash_data() const {
    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = this->get_issuer_name_hash();
    certificate_hash_data.issuer_key_hash = this->get_issuer_key_hash();
    certificate_hash_data.serial_number = this->get_serial_number();
    return certificate_hash_data;
}

std::string X509Wrapper::get_responder_url() const {
    const auto ocsp = X509_get1_ocsp(this->x509[0].get());
    std::string responder_url;
    for (int i = 0; i < sk_OPENSSL_STRING_num(ocsp); i++) {
        responder_url.append(sk_OPENSSL_STRING_value(ocsp, i));
    }

    if (responder_url.empty()) {
        std::cout << "Could not retrieve OCSP Responder URL from certificate" << std::endl;
    }

    return responder_url;
}

std::string X509Wrapper::to_base64_string() const {
    return x509_to_string(x509[0].get());
}

} // namespace evse_security
