// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <evse_utilities.hpp>
#include <x509_bundle.hpp>

#include <algorithm>
#include <openssl/err.h>
#include <openssl/x509v3.h>

namespace evse_security {

X509Wrapper X509CertificateBundle::get_latest_valid_certificate(const std::vector<X509Wrapper>& certificates) {
    // Filter certificates with valid_in > 0
    std::vector<X509Wrapper> valid_certificates;
    for (const auto& cert : certificates) {
        if (cert.is_valid()) {
            valid_certificates.push_back(cert);
        }
    }

    if (valid_certificates.empty()) {
        // No valid certificates found
        throw NoCertificateValidException("No valid certificates available.");
    }

    // Find the certificate with the latest valid_in
    auto latest_certificate = std::max_element(
        valid_certificates.begin(), valid_certificates.end(),
        [](const X509Wrapper& cert1, const X509Wrapper& cert2) { return cert1.get_valid_in() < cert2.get_valid_in(); });

    return *latest_certificate;
}

std::vector<X509_ptr> X509CertificateBundle::load_certificates(const std::string& data, const EncodingFormat encoding) {
    BIO_ptr bio(BIO_new_mem_buf(data.data(), static_cast<int>(data.size())));

    if (!bio) {
        throw CertificateLoadException("Failed to create BIO from data");
    }

    std::vector<X509_ptr> certificates;

    if (encoding == EncodingFormat::PEM) {
        STACK_OF(X509_INFO)* allcerts = PEM_X509_INFO_read_bio(bio.get(), nullptr, nullptr, nullptr);

        if (allcerts) {
            for (int i = 0; i < sk_X509_INFO_num(allcerts); i++) {
                X509_INFO* xi = sk_X509_INFO_value(allcerts, i);

                if (xi && xi->x509) {
                    // Transfer owneship
                    certificates.emplace_back(xi->x509);
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
            certificates.emplace_back(x509);
        } else {
            throw CertificateLoadException("Certificate (DER) parsing error");
        }
    } else {
        throw CertificateLoadException("Unsupported encoding format");
    }

    return certificates;
}

X509CertificateBundle::X509CertificateBundle(const std::string& certificate, const EncodingFormat encoding) {
    source = X509CertificateSource::STRING;
    add_certifcates(certificate, encoding, std::nullopt);
}

X509CertificateBundle::X509CertificateBundle(const fs::path& path, const EncodingFormat encoding) {
    this->path = path;

    if (fs::is_directory(path)) {
        source = X509CertificateSource::DIRECTORY;

        // Iterate directory
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (is_certificate_file(entry)) {
                std::string certificate;
                if (EvseUtils::read_from_file(entry.path(), certificate))
                    add_certifcates(certificate, encoding, entry.path());
            }
        }
    } else if (is_certificate_file(path)) {
        source = X509CertificateSource::FILE;

        std::string certificate;
        if (EvseUtils::read_from_file(path, certificate))
            add_certifcates(certificate, encoding, path);
    } else {
        throw CertificateLoadException("Failed to create certificate info from path: " + path.string());
    }
}

void X509CertificateBundle::add_certifcates(const std::string& data, const EncodingFormat encoding,
                                            const std::optional<fs::path>& path) {
    auto loaded = load_certificates(data, encoding);

    // If we are using a directory we can't load certificate bundles
    if (is_using_directory() && loaded.size() > 1) {
        EVLOG_warning << "Failed to read single certificate from directory file, ignoring entry!";
        return;
    }

    for (auto& x509 : loaded) {
        if (path.has_value())
            certificates.emplace_back(std::move(x509), path.value());
        else
            certificates.emplace_back(std::move(x509));
    }
}

std::vector<X509Wrapper> X509CertificateBundle::split() {
    return certificates;
}

bool X509CertificateBundle::contains_certificate(const X509Wrapper& certificate) const {
    for (const auto& certif : certificates) {
        if (certif == certificate)
            return true;
    }

    return false;
}

bool X509CertificateBundle::contains_certificate(const CertificateHashData& certificate_hash) const {
    for (const auto& certif : certificates) {
        if (certif == certificate_hash)
            return true;
    }

    return false;
}

bool X509CertificateBundle::delete_certificate(const X509Wrapper& certificate) {
    return delete_certificate(certificate.get_certificate_hash_data());
}

bool X509CertificateBundle::delete_certificate(const CertificateHashData& data) {
    for (auto it = certificates.begin(); it != certificates.end(); ++it) {
        if (*it == data) {
            certificates.erase(it);
            return true;
        }
    }

    return false;
}

void X509CertificateBundle::delete_all_certificates() {
    certificates.clear();
}

void X509CertificateBundle::add_certificate(X509Wrapper&& certificate) {
    certificates.push_back(certificate);
}

bool X509CertificateBundle::update_certificate(X509Wrapper&& certificate) {
    for (int i = 0; i < certificates.size(); ++i) {
        if (certificates[i] == certificate) {
            certificates.at(i) = std::move(certificate);
            return true;
        }
    }

    return false;
}

bool X509CertificateBundle::export_certificates() {
    if (source == X509CertificateSource::STRING) {
        EVLOG_error << "Export for string is invalid!";
        return false;
    }

    // Add/delete certifs
    if (!sync_to_certificate_store()) {
        EVLOG_error << "Sync to certificate store failed!";
        return false;
    }

    if (source == X509CertificateSource::DIRECTORY) {
        bool exported_all = true;

        // Write updated certificates
        for (auto& certificate : certificates) {
            if (certificate.get_file().has_value()) {
                if (!EvseUtils::write_to_file(certificate.get_file().value(), certificate.get_export_string(),
                                              std::ios::trunc)) {
                    exported_all = false;
                }
            } else {
                exported_all = false;
            }
        }

        return exported_all;
    } else if (source == X509CertificateSource::FILE) {
        // We're using a single file, no need to check for deleted certificates
        return EvseUtils::write_to_file(path, to_export_string(), std::ios::trunc);
    }

    return false;
}

bool X509CertificateBundle::sync_to_certificate_store() {
    if (source == X509CertificateSource::STRING) {
        EVLOG_error << "Sync for string is invalid!";
        return false;
    }

    if (source == X509CertificateSource::DIRECTORY) {
        // Get existing certificates from filesystem
        std::vector<X509Wrapper> fs_certificates = X509CertificateBundle(path, EncodingFormat::PEM).split();

        bool success = true;
        // Delete filesystem certificates missing from our list
        for (const auto& fs_certif : fs_certificates) {
            if (std::find(certificates.begin(), certificates.end(), fs_certif) == certificates.end()) {
                // fs certif not existing in our certificate list, delete
                if (!EvseUtils::delete_file(fs_certif.get_file().value()))
                    success = false;
            }
        }

        // Add the certificates that are not existing in the filesystem
        for (const auto& certif : certificates) {
            if (std::find(fs_certificates.begin(), fs_certificates.end(), certif) == fs_certificates.end()) {
                // Certif not existing in fs certificates write it out
                if (!EvseUtils::write_to_file(certif.get_file().value(), certif.get_export_string(), std::ios::trunc))
                    success = false;
            }
        }

        return success;
    } else if (source == X509CertificateSource::FILE) {
        // Delete source file if we're empty
        if (certificates.empty()) {
            return EvseUtils::delete_file(path);
        }
    }

    return true;
}

X509Wrapper X509CertificateBundle::get_latest_valid_certificate() {
    return get_latest_valid_certificate(certificates);
}

std::string X509CertificateBundle::to_export_string() const {
    std::string export_string;

    for (auto& certificate : certificates) {
        export_string += certificate.get_export_string();
    }

    return export_string;
}

X509CertificateDirectory::X509CertificateDirectory(const fs::path& directory) {
    if (fs::is_directory(directory)) {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (X509CertificateBundle::is_certificate_file(entry)) {
                bundles.emplace_back(entry, EncodingFormat::PEM);
            }
        }
    } else {
        throw CertificateLoadException("X509CertificateDirectory can only load from directories!");
    }
}

} // namespace evse_security
