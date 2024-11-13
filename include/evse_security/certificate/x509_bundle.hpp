// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <algorithm>
#include <map>

#include <evse_security/certificate/x509_hierarchy.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>

namespace evse_security {

/// @brief Custom exception that is thrown when no private key could be found for a selected certificate
class NoPrivateKeyException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// @brief Custom exception that is thrown when no valid certificate could be found for the specified filesystem
/// locations
class NoCertificateValidException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// @brief Custom exception that is thrown when a invalid operation is with the current state
/// on the certificate bundle
class InvalidOperationException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// @brief X509 certificate bundle, used for holding multiple X509Wrappers. Supports
/// operations like add/delete importing/exporting certificates. Can use either a
/// directory with multiple certificates (or bundles of certificates) or a single
/// file with one or more certificates in it.
class X509CertificateBundle {
public:
    X509CertificateBundle(const fs::path& path, const EncodingFormat encoding);
    X509CertificateBundle(const std::string& certificate, const EncodingFormat encoding);

    X509CertificateBundle(X509CertificateBundle&& other) = default;
    X509CertificateBundle(const X509CertificateBundle& other) = delete;

public:
    /// @brief Gets if this certificate bundle comes from a single certificate bundle file
    /// @return
    bool is_using_bundle_file() const {
        return (source == X509CertificateSource::FILE);
    }

    /// @brief Gets if this certificate bundle comes from an entire directory
    /// @return
    bool is_using_directory() const {
        return (source == X509CertificateSource::DIRECTORY);
    }

    /// @return True if multiple certificates are contained within
    bool is_bundle() const {
        return (get_certificate_count() > 1);
    }

    bool empty() const {
        return certificates.empty();
    }

    /// @return Contained certificate count
    int get_certificate_count() const;

    /// @return Contained certificate chains count
    int get_certificate_chains_count() const;

    fs::path get_path() const {
        return path;
    }

    X509CertificateSource get_source() const {
        return source;
    }

    /// @brief Iterates through all the contained certificate chains (file, certificates)
    /// while the provided function returns true
    template <typename function> void for_each_chain(function func) {
        for (const auto& chain : certificates) {
            if (!func(chain.first, chain.second))
                break;
        }
    }

    /// @brief Same as 'for_each_chain' but it also uses a predicate for ordering
    template <typename function, typename ordering> void for_each_chain_ordered(function func, ordering order) {
        struct Chain {
            const fs::path* path;
            const std::vector<X509Wrapper>* certificates;
        };

        std::vector<Chain> ordered;

        for (auto& [path, certs] : certificates) {
            ordered.push_back(Chain{&path, &certs});
        }

        std::sort(std::begin(ordered), std::end(ordered),
                  [&order](Chain& a, Chain& b) { return order(*a.certificates, *b.certificates); });

        for (const auto& chain : ordered) {
            if (!func(*chain.path, *chain.certificates))
                break;
        }
    }

public:
    /// @brief Splits the certificate (chain) into single certificates
    /// @return vector containing single certificates
    std::vector<X509Wrapper> split();

    /// @brief If we already have the certificate
    bool contains_certificate(const X509Wrapper& certificate);
    /// @brief If we already have the certificate hash
    bool contains_certificate(const CertificateHashData& certificate_hash);

    /// @brief Finds a certificate based on its hash
    X509Wrapper find_certificate(const CertificateHashData& certificate_hash, bool case_insensitive_comparison = false);

    /// @brief Adds a single certificate in the bundle. Only in memory, use @ref export_certificates to filesystem
    void add_certificate(X509Wrapper&& certificate);

    /// @brief Adds a single certificate in the bundle, only if it is not contained
    /// already. Only in memory, use @ref export_certificates to filesystem
    void add_certificate_unique(X509Wrapper&& certificate);

    /// @brief Updates an already existing certificate if it is found
    bool update_certificate(X509Wrapper&& certificate);

    /// @brief Deletes all instances of the provided certificate. Only in memory, use @ref export_certificates
    /// to filesystem export
    /// @param include_issued If true the child certificates will also be deleted, if any are found
    /// @return count of certificates deleted
    int delete_certificate(const X509Wrapper& certificate, bool include_issued);

    /// @brief Deletes all certificates with the  provided certificate hash. Only in memory,
    /// use @ref export_certificates to filesystem export
    /// @param include_issued If true the child certificates will also be deleted, if any are found
    /// @return count of certificates deleted
    int delete_certificate(const CertificateHashData& data, bool include_issued);

    /// @brief Deletes all certificates. Only in memory, use @ref export_certificates to filesystem export
    void delete_all_certificates();

    /// @brief Returns a full exportable representation of a certificate bundle file in PEM format
    std::string to_export_string() const;

    /// @brief Returns a full exportable representation of a certificate sub-chain, if found
    std::string to_export_string(const std::filesystem::path& chain) const;

    /// @brief Exports the full certificate chain either as individual files if it is using a directory
    /// or as a bundle if it uses a bundle file, at the initially provided path. Also deletes/adds the updated
    /// certificates
    /// @return true on success, false otherwise
    bool export_certificates();

    /// @brief Syncs the file structure with the certificate store adding certificates that are not found on the
    /// storage and deleting the certificates that are not contained in this bundle
    bool sync_to_certificate_store();

public:
    /// @brief returns the latest valid certificate within this bundle
    X509Wrapper get_latest_valid_certificate();

    /// @brief Utility that returns current the certificate hierarchy of this bundle
    /// Invalidated on any add/delete operation
    X509CertificateHierarchy& get_certificate_hierarchy();

public:
    X509CertificateBundle& operator=(X509CertificateBundle&& other) = default;

public:
    /// @brief Returns the latest valid certif that we might contain
    static X509Wrapper get_latest_valid_certificate(const std::vector<X509Wrapper>& certificates);

    static bool is_certificate_file(const fs::path& file) {
        return fs::is_regular_file(file) &&
               ((file.extension() == PEM_EXTENSION) || (file.extension() == DER_EXTENSION));
    }

private:
    /// @brief Adds to our certificate list the certificates found in the file
    /// @return number of added certificates
    void add_certificates(const std::string& data, const EncodingFormat encoding, const std::optional<fs::path>& path);

    /// @brief operation to be executed after each add/delete to this bundle
    void invalidate_hierarchy();

private:
    // Structure of the bundle - maps files to the certificates stored in them
    // For certificates coming from a string, uses a default empty path
    std::map<fs::path, std::vector<X509Wrapper>> certificates;
    // Relevant bundle file or directory for the certificates
    fs::path path;
    // Source from where we created the certificates. If 'string' the 'export' functions will not work
    X509CertificateSource source;

    // Cached certificate hierarchy, invalidated on any operation
    X509CertificateHierarchy hierarchy;
    bool hierarchy_invalidated;
};

} // namespace evse_security
