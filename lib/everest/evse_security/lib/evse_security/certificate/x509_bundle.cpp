// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <evse_security/certificate/x509_bundle.hpp>

#include <algorithm>
#include <fstream>

#include <everest/logging.hpp>
#include <evse_security/crypto/evse_crypto.hpp>
#include <evse_security/utils/evse_filesystem.hpp>

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

X509CertificateBundle::X509CertificateBundle(const std::string& certificate, const EncodingFormat encoding) :
    hierarchy_invalidated(true), source(X509CertificateSource::STRING) {
    add_certificates(certificate, encoding, std::nullopt);
}

X509CertificateBundle::X509CertificateBundle(const fs::path& path, const EncodingFormat encoding) :
    hierarchy_invalidated(true), path(path) {

    // Attempt creation
    filesystem_utils::create_file_or_dir_if_nonexistent(path);

    if (fs::is_directory(path)) {
        source = X509CertificateSource::DIRECTORY;

        // Iterate directory, not recursively since we might have the ocsp sub-directory
        // that contains the .der OCSP data that we don't have to use
        for (const auto& entry : fs::directory_iterator(path)) {
            if (is_certificate_file(entry)) {
                std::string certificate{};
                if (filesystem_utils::read_from_file(entry.path(), certificate)) {
                    add_certificates(certificate, encoding, entry.path());
                }
            }
        }
    } else if (is_certificate_file(path)) {
        source = X509CertificateSource::FILE;

        std::string certificate{};
        if (filesystem_utils::read_from_file(path, certificate)) {
            add_certificates(certificate, encoding, path);
        }
    } else {
        throw CertificateLoadException("Failed to create certificate info from path: " + path.string());
    }
}

std::vector<X509Wrapper> X509CertificateBundle::split() {
    std::vector<X509Wrapper> full_certificates;

    // Append all chains
    for (const auto& chains : certificates) {
        for (const auto& cert : chains.second) {
            full_certificates.push_back(cert);
        }
    }

    return full_certificates;
}

int X509CertificateBundle::get_certificate_count() const {
    int count = 0;
    for (const auto& chain : certificates) {
        count += chain.second.size();
    }

    return count;
}

int X509CertificateBundle::get_certificate_chains_count() const {
    return certificates.size();
}

void X509CertificateBundle::add_certificates(const std::string& data, const EncodingFormat encoding,
                                             const std::optional<fs::path>& path) {
    auto loaded = CryptoSupplier::load_certificates(data, encoding);
    auto& list = certificates[path.value_or(std::filesystem::path())];

    for (auto& x509 : loaded) {
        if (path.has_value()) {
            list.emplace_back(std::move(x509), path.value());
        } else {
            list.emplace_back(std::move(x509));
        }
    }
}

bool X509CertificateBundle::contains_certificate(const X509Wrapper& certificate) {
    // Search through all the chains
    for (const auto& chain : certificates) {
        for (const auto& certif : chain.second) {
            if (certif == certificate) {
                return true;
            }
        }
    }

    return false;
}

bool X509CertificateBundle::contains_certificate(const CertificateHashData& certificate_hash) {
    // Try an initial search for root certificates, else a hierarchy build will be required
    for (const auto& chain : certificates) {
        const bool found = std::find_if(std::begin(chain.second), std::end(chain.second), [&](const X509Wrapper& cert) {
                               return cert.is_selfsigned() && cert == certificate_hash;
                           }) != std::end(chain.second);

        if (found) {
            return true;
        }
    }

    // Nothing found, build the hierarchy and search by the issued hash
    X509CertificateHierarchy& hierarchy = get_certificate_hierarchy();
    return hierarchy.contains_certificate_hash(certificate_hash, true);
}

std::optional<X509Wrapper> X509CertificateBundle::find_certificate(const CertificateHashData& certificate_hash,
                                                                   bool case_insensitive_comparison) {
    // Try an initial search for root certificates, else a hierarchy build will be required
    for (const auto& chain : certificates) {
        for (const auto& certif : chain.second) {
            if (certif.is_selfsigned()) {
                bool matches = false;

                if (case_insensitive_comparison) {
                    const CertificateHashData certif_hash = certif.get_certificate_hash_data();
                    matches = certif_hash.case_insensitive_comparison(certificate_hash);
                } else {
                    matches = (certif == certificate_hash);
                }

                if (matches) {
                    return certif;
                }
            }
        }
    }

    // Nothing found, build the hierarchy and search by the issued hash
    X509CertificateHierarchy& hierarchy = get_certificate_hierarchy();
    return hierarchy.find_certificate(certificate_hash, case_insensitive_comparison);
}

std::vector<X509Wrapper> X509CertificateBundle::delete_certificate(const X509Wrapper& certificate, bool include_issued,
                                                                   bool include_top) {
    std::vector<X509Wrapper> to_delete;
    std::vector<X509Wrapper> deleted;

    if (include_issued || include_top) {
        // Include all descendants in the delete list
        auto& hierarchy = get_certificate_hierarchy();

        if (include_issued) {
            auto issued = hierarchy.collect_descendants(certificate);

            to_delete.insert(to_delete.end(), std::make_move_iterator(issued.begin()),
                             std::make_move_iterator(issued.end()));
        }

        if (include_top) {
            auto top = hierarchy.collect_top(certificate);

            to_delete.insert(to_delete.end(), std::make_move_iterator(top.begin()), std::make_move_iterator(top.end()));
        }
    }

    // Include default delete
    to_delete.push_back(certificate);

    for (auto& chains : certificates) {
        auto& certifs = chains.second;

        certifs.erase(std::remove_if(certifs.begin(), certifs.end(),
                                     [&](const auto& certif) {
                                         const bool found =
                                             std::find(to_delete.begin(), to_delete.end(), certif) != to_delete.end();

                                         if (found) {
                                             deleted.push_back(certif);
                                         }

                                         return found;
                                     }),
                      certifs.end());
    }

    // If we deleted any, invalidate the built hierarchy
    if (false == deleted.empty()) {
        invalidate_hierarchy();
    }

    return deleted;
}

std::vector<X509Wrapper> X509CertificateBundle::delete_certificate(const CertificateHashData& data, bool include_issued,
                                                                   bool include_top) {
    auto& hierarchy = get_certificate_hierarchy();

    std::optional<X509Wrapper> to_delete = hierarchy.find_certificate(data, true /* = Case insensitive search */);
    if (to_delete.has_value()) {
        return delete_certificate(to_delete.value(), include_issued, include_top);
    }

    return {};
}

void X509CertificateBundle::delete_all_certificates() {
    certificates.clear();
}

void X509CertificateBundle::add_certificate(X509Wrapper&& certificate) {
    if (source == X509CertificateSource::DIRECTORY) {
        // If it is in directory mode only allow sub-directories of that directory
        const std::filesystem::path certif_path = certificate.get_file().value_or(std::filesystem::path());

        if (filesystem_utils::is_subdirectory(path, certif_path)) {
            certificates[certif_path].push_back(std::move(certificate));
            invalidate_hierarchy();
        } else {
            throw InvalidOperationException(
                "Added certificate with directory bundle, must be subdir of the main directory: " + path.string());
        }
    } else {
        // The bundle came from a file, so there is only one file we could add the certificate to
        certificates.begin()->second.push_back(certificate);
        invalidate_hierarchy();
    }
}

void X509CertificateBundle::add_certificate_unique(X509Wrapper&& certificate) {
    if (!contains_certificate(certificate)) {
        add_certificate(std::move(certificate));
        invalidate_hierarchy();
    }
}

bool X509CertificateBundle::update_certificate(X509Wrapper&& certificate) {
    for (auto& chain : certificates) {
        for (auto& certif : chain.second) {
            if (certif == certificate) {
                certif = std::move(certificate);
                invalidate_hierarchy();

                return true;
            }
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
        for (auto& chains : certificates) {
            // Ignore empty chains (the file was deleted)
            if (chains.second.empty()) {
                continue;
            }

            // Each chain is a single file
            if (!filesystem_utils::write_to_file(chains.first, to_export_string(chains.first), std::ios::trunc)) {
                exported_all = false;
            }
        }

        return exported_all;
    }
    if (source == X509CertificateSource::FILE) {
        // write to a separate file to minimise corruption and data loss; then rename
        namespace fs = std::filesystem;
        bool result{false};

        try {
            const auto tmp_file = path.string() + '$';
            fs::remove(tmp_file);

            // We're using a single file, no need to check for deleted certificates
            result = filesystem_utils::write_to_file(tmp_file, to_export_string(), std::ios::trunc);

            fs::rename(tmp_file, path);
        } catch (const fs::filesystem_error& ex) {
            EVLOG_error << "Error export_certificates: " << ex.path1() << ' ' << ex.path2() << ": " << ex.what();
        }
        return result;
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
        X509CertificateBundle fs_certificates(path, EncodingFormat::PEM);
        bool success = true;

        // Delete filesystem certificate chains missing from our map
        for (const auto& fs_chain : fs_certificates.certificates) {
            if (certificates.find(fs_chain.first) == certificates.end()) {
                // fs certif chain not existing in our certificate list, delete
                if (!filesystem_utils::delete_file(fs_chain.first)) {
                    success = false;
                }
            }
        }

        // Add the certificates that are not existing in the filesystem. Each chain represents a single file
        for (const auto& chain : certificates) {
            if (chain.second.empty()) {
                // If it's an empty chain, delete
                if (!filesystem_utils::delete_file(chain.first)) {
                    success = false;
                }
            } else if (fs_certificates.certificates.find(chain.first) == fs_certificates.certificates.end()) {
                // Certif not existing in fs certificates write it out
                if (!filesystem_utils::write_to_file(chain.first, to_export_string(chain.first), std::ios::trunc)) {
                    success = false;
                }
            }
        }

        // After fs deletion erase all empty files from our certificate list, so that we don't write them out
        for (auto first = certificates.begin(); first != certificates.end();) {
            if (first->second.empty()) {
                first = certificates.erase(first);
            } else {
                ++first;
            }
        }

        return success;
    }
    if (source == X509CertificateSource::FILE) {
        // Delete source file if we're empty
        if (certificates.empty()) {
            return filesystem_utils::delete_file(path);
        }

        return true;
    }

    return false;
}

X509Wrapper X509CertificateBundle::get_latest_valid_certificate() {
    return get_latest_valid_certificate(split());
}

void X509CertificateBundle::invalidate_hierarchy() {
    hierarchy_invalidated = true;
}

X509CertificateHierarchy& X509CertificateBundle::get_certificate_hierarchy() {
    if (hierarchy_invalidated) {
        EVLOG_info << "Building new certificate hierarchy!";
        hierarchy_invalidated = false;

        auto certificates = split();
        hierarchy = X509CertificateHierarchy::build_hierarchy(certificates);
    }

    return hierarchy;
}

std::string X509CertificateBundle::to_export_string() const {
    std::string export_string;

    for (auto& chain : certificates) {
        for (auto& certificate : chain.second) {
            export_string += certificate.get_export_string();
        }
    }

    return export_string;
}

std::string X509CertificateBundle::to_export_string(const std::filesystem::path& chain) const {
    std::string export_string;

    auto found = certificates.find(chain);
    if (found != certificates.end()) {
        for (auto& certificate : found->second) {
            export_string += certificate.get_export_string();
        }
    }

    return export_string;
}

} // namespace evse_security
