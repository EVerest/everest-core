// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <evse_security/certificate/x509_wrapper.hpp>

#include <cctype>
#include <fstream>
#include <iostream>
#include <regex>

#include <everest/logging.hpp>
#include <evse_security/crypto/evse_crypto.hpp>
#include <evse_security/utils/evse_filesystem.hpp>

namespace evse_security {

X509Wrapper::X509Wrapper(const fs::path& file, const EncodingFormat encoding) {
    if (fs::is_regular_file(file) == false) {
        throw CertificateLoadException("X509Wrapper can only load from files!");
    }

    fsstd::ifstream read(file, std::ios::binary);
    std::string certificate((std::istreambuf_iterator<char>(read)), std::istreambuf_iterator<char>());

    auto loaded = CryptoSupplier::load_certificates(certificate, encoding);
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
    auto loaded = CryptoSupplier::load_certificates(data, encoding);
    if (loaded.size() != 1) {
        std::string error = "X509Wrapper can only load a single certificate! Loaded: ";
        error += std::to_string(loaded.size());

        throw CertificateLoadException(error);
    }

    x509 = std::move(loaded[0]);
    update_validity();
}

X509Wrapper::X509Wrapper(X509Handle_ptr&& x509) : x509(std::move(x509)) {
    update_validity();
}

X509Wrapper::X509Wrapper(X509Handle_ptr&& x509, const fs::path& file) : x509(std::move(x509)), file(file) {
    if (fs::is_regular_file(file) == false) {
        throw CertificateLoadException("X509Wrapper can only load from files!");
    }

    update_validity();
}

X509Wrapper::X509Wrapper(const X509Wrapper& other) :
    x509(std::move(CryptoSupplier::x509_duplicate_unique(other.get()))),
    file(other.file),
    valid_in(other.valid_in),
    valid_to(other.valid_to) {
}

X509Wrapper::~X509Wrapper() {
}

bool X509Wrapper::operator==(const X509Wrapper& other) const {
    if (this == &other)
        return true;

    return CryptoSupplier::x509_is_equal(get(), other.get());
}

void X509Wrapper::update_validity() {
    CryptoSupplier::x509_get_validity(get(), valid_in, valid_to);
}

bool X509Wrapper::is_child(const X509Wrapper& parent) const {
    // A certif can't be it's own parent, use is_selfsigned if that is intended
    if (this == &parent)
        return false;

    return CryptoSupplier::x509_is_child(get(), parent.get());
}

bool X509Wrapper::is_selfsigned() const {
    return CryptoSupplier::x509_is_selfsigned(get());
}

int64_t X509Wrapper::get_valid_in() const {
    return valid_in;
}

/// \brief Gets valid_in
int64_t X509Wrapper::get_valid_to() const {
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

void X509Wrapper::set_file(fs::path& path) {
    if (fs::is_directory(path))
        throw std::logic_error("update_file must only be used for files, not directories!");

    file = path;
}

X509CertificateSource X509Wrapper::get_source() const {
    if (file.has_value()) {
        return X509CertificateSource::FILE;
    } else {
        return X509CertificateSource::STRING;
    }
}

std::string X509Wrapper::get_common_name() const {
    return CryptoSupplier::x509_get_common_name(get());
}

std::string X509Wrapper::get_issuer_name_hash() const {
    return CryptoSupplier::x509_get_issuer_name_hash(get());
}

std::string X509Wrapper::get_serial_number() const {
    return CryptoSupplier::x509_get_serial_number(get());
}

std::string X509Wrapper::get_issuer_key_hash() const {
    if (is_selfsigned()) {
        return get_key_hash();
    } else {
        throw std::logic_error("get_issuer_key_hash must only be used on self-signed certs");
    }
}

std::string X509Wrapper::get_key_hash() const {
    return CryptoSupplier::x509_get_key_hash(get());
}

CertificateHashData X509Wrapper::get_certificate_hash_data() const {
    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = this->get_issuer_name_hash();
    certificate_hash_data.issuer_key_hash = this->get_issuer_key_hash();
    certificate_hash_data.serial_number = this->get_serial_number();
    return certificate_hash_data;
}

CertificateHashData X509Wrapper::get_certificate_hash_data(const X509Wrapper& issuer) const {
    if (CryptoSupplier::x509_is_child(get(), issuer.get()) == false) {
        throw std::logic_error("The specified issuer is not the correct issuer for this certificate.");
    }

    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = this->get_issuer_name_hash();
    // Issuer key hash
    certificate_hash_data.issuer_key_hash = issuer.get_key_hash();
    certificate_hash_data.serial_number = this->get_serial_number();

    return certificate_hash_data;
}

std::string X509Wrapper::get_responder_url() const {
    return CryptoSupplier::x509_get_responder_url(get());
}

std::string X509Wrapper::get_export_string() const {
    return CryptoSupplier::x509_to_string(get());
}

} // namespace evse_security
