// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <fstream>
#include <iostream>
#include <regex>
#include <cctype>

#include <everest/logging.hpp>
#include <evse_utilities.hpp>

#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <x509_bundle.hpp>
#include <x509_wrapper.hpp>

namespace evse_security {

std::string x509_to_string(X509* x509) {
    if (x509) {
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

X509Wrapper::X509Wrapper(const fs::path& file, const EncodingFormat encoding) {
    if (fs::is_regular_file(file) == false) {
        throw CertificateLoadException("X509Wrapper can only load from files!");
    }

    fsstd::ifstream read(file, std::ios::binary);
    std::string certificate((std::istreambuf_iterator<char>(read)), std::istreambuf_iterator<char>());

    auto loaded = X509CertificateBundle::load_certificates(certificate, encoding);
    if (loaded.size() != 1) {
        std::string error = "X509Wrapper can only load a single certificate! Loaded: ";
        error += std::to_string(loaded.size());

        throw CertificateLoadException(error);
    }

    this->file = file;
    x509 = std::move(loaded[0]);
    update_validity();
}

X509Wrapper::X509Wrapper(const std::string& data, const EncodingFormat encoding) {
    auto loaded = X509CertificateBundle::load_certificates(data, encoding);
    if (loaded.size() != 1) {
        std::string error = "X509Wrapper can only load a single certificate! Loaded: ";
        error += std::to_string(loaded.size());

        throw CertificateLoadException(error);
    }

    x509 = std::move(loaded[0]);
    update_validity();
}

X509Wrapper::X509Wrapper(X509* x509) : x509(x509) {
    update_validity();
}

X509Wrapper::X509Wrapper(X509_ptr&& x509) : x509(std::move(x509)) {
    update_validity();
}

X509Wrapper::X509Wrapper(X509* x509, const fs::path& file) : x509(x509), file(file) {
    if (fs::is_regular_file(file) == false) {
        throw CertificateLoadException("X509Wrapper can only load from files!");
    }

    update_validity();
}

X509Wrapper::X509Wrapper(X509_ptr&& x509, const fs::path& file) : x509(std::move(x509)), file(file) {
    if (fs::is_regular_file(file) == false) {
        throw CertificateLoadException("X509Wrapper can only load from files!");
    }

    update_validity();
}

X509Wrapper::X509Wrapper(const X509Wrapper& other) :
    x509(X509_dup(other.x509.get())), file(other.file), valid_in(other.valid_in), valid_to(other.valid_to) {
}

X509Wrapper::~X509Wrapper() {
}


bool X509Wrapper::operator==(const X509Wrapper& other) const {
    return (X509_cmp(get(), other.get()) == 0);
}

void X509Wrapper::update_validity() {
    // For valid_in and valid_to
    ASN1_TIME* notBefore = X509_get_notBefore(get());
    ASN1_TIME* notAfter = X509_get_notAfter(get());

    int day, sec;
    ASN1_TIME_diff(&day, &sec, nullptr, notBefore);
    valid_in = std::chrono::duration_cast<std::chrono::seconds>(ossl_days_to_seconds(day)).count() +
               sec; // Convert days to seconds
    ASN1_TIME_diff(&day, &sec, nullptr, notAfter);
    valid_to = std::chrono::duration_cast<std::chrono::seconds>(ossl_days_to_seconds(day)).count() +
               sec; // Convert days to seconds
}

bool X509Wrapper::is_child(const X509Wrapper &parent) const {
    if ((this == &parent) || (*this == parent)) {
        return false;
    }

    X509_STORE_ptr store(X509_STORE_new());
    X509_STORE_add_cert(store.get(), parent.get());

    X509_STORE_CTX_ptr ctx(X509_STORE_CTX_new());
    X509_STORE_CTX_init(ctx.get(), store.get(), this->get(), NULL);
    X509_STORE_CTX_set_flags(ctx.get(), X509_V_FLAG_X509_STRICT);

    // If the parent is not a self-signed certificate, assume we have a partial chain
    if (parent.is_selfsigned() == false) {
        X509_STORE_CTX_set_flags(ctx.get(), X509_V_FLAG_PARTIAL_CHAIN);
    }

    return (X509_verify_cert(ctx.get()) == 1);
}

bool X509Wrapper::is_selfsigned() const {
    return (X509_self_signed(x509.get(), 0) == 1);
}

void X509Wrapper::reset(X509* _x509) {
    x509.reset(_x509);
}

int X509Wrapper::get_valid_in() const {
    return valid_in;
}

/// \brief Gets valid_in
int X509Wrapper::get_valid_to() const {
    return valid_to;
}

bool X509Wrapper::is_valid() const {
    // The valid_in must be in the past and the valid_to must be in the future
    return (get_valid_in() <= 0) && (get_valid_to() >= 0);
}

bool X509Wrapper::is_expired() const {
    return (get_valid_to() < 0);
}

std::optional<fs::path> X509Wrapper::get_file() const {
    return this->file;
}

std::string X509Wrapper::get_common_name() const {
    const X509* x509 = get();

    X509_NAME* subject = X509_get_subject_name(x509);
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
    X509_NAME* name = X509_get_issuer_name(get());
    X509_NAME_digest(name, EVP_sha256(), md, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)md[i];
    }
    return ss.str();
}

std::string X509Wrapper::get_serial_number() const {
    ASN1_INTEGER* serial_asn1 = X509_get_serialNumber(get());
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
    for (char & i : serial) { i = std::tolower(i); }

    BN_free(bn_serial);

    serial.erase(0, std::min(serial.find_first_not_of('0'), serial.size() - 1));
    return serial;
}

std::string X509Wrapper::get_key_hash() const {
    // TODO (ioan): Actually here we don't need OUR pubkey
    // hash but as per the spec we need the issuer's key hash
    unsigned char tmphash[SHA256_DIGEST_LENGTH];
    X509_pubkey_digest(get(), EVP_sha256(), tmphash, NULL);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)tmphash[i];
    }
    return ss.str();
}

CertificateHashData X509Wrapper::get_certificate_hash_data() const {
    if (!this->is_selfsigned()) {
        throw std::logic_error("get_certificate_hash_data must only be used on self-signed certs");
    }

    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = this->get_issuer_name_hash();
    certificate_hash_data.issuer_key_hash = this->get_key_hash();
    certificate_hash_data.serial_number = this->get_serial_number();
    return certificate_hash_data;
}

CertificateHashData X509Wrapper::get_certificate_hash_data(const X509Wrapper& issuer) const {
    if (!this->is_child(issuer)) {
        throw std::logic_error("The specified issuer is not the correct issuer for this certificate.");
    }

    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = this->get_issuer_name_hash();
    certificate_hash_data.issuer_key_hash = issuer.get_key_hash();
    certificate_hash_data.serial_number = this->get_serial_number();

    return certificate_hash_data;
}

std::string X509Wrapper::get_responder_url() const {
    const auto ocsp = X509_get1_ocsp(get());
    std::string responder_url;
    for (int i = 0; i < sk_OPENSSL_STRING_num(ocsp); i++) {
        responder_url.append(sk_OPENSSL_STRING_value(ocsp, i));
    }

    if (responder_url.empty()) {
        std::cout << "Could not retrieve OCSP Responder URL from certificate" << std::endl;
    }

    return responder_url;
}

std::string X509Wrapper::get_export_string() const {
    return x509_to_string(get());
}

} // namespace evse_security
