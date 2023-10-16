
// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef X509_BUNDLE_HPP
#define X509_BUNDLE_HPP

#include <x509_wrapper.hpp>

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

/// @brief X509 certificate bundle, used for holding multiple X509Wrappers. Supports
/// operations like add/delete importing/exporting certificates. Can use either a
/// directory with multiple certificates or a single file with one or more certificates
/// in it. A directory that contains certificate bundle files will not work, the entry
/// will be ignored
class X509CertificateBundle {
public:
    X509CertificateBundle(const std::filesystem::path& path, const EncodingFormat encoding);
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
    int get_certificate_count() const {
        return certificates.size();
    }

    std::filesystem::path get_path() const {
        return path;
    }

    X509CertificateSource get_source() const {
        return source;
    }

public:
    /// @brief Splits the certificate (chain) into single certificates
    /// @return vector containing single certificates
    std::vector<X509Wrapper> split();

    /// @brief If we already have the certificate
    bool contains_certificate(const X509Wrapper& certificate) const;
    /// @brief If we already have the certificate
    bool contains_certificate(const CertificateHashData& certificate_hash) const;

    /// @brief Updates a single certificate in the chain. Only in memory, use @ref export_certificates to filesystem
    void add_certificate(X509Wrapper&& certificate);

    /// @brief Updates a single certificate in the chain. Only in memory, use @ref export_certificates to filesystem
    /// export
    /// @param certificate certificate to update
    /// @return true if the certificate was found and updated, false otherwise. If true is returned the provided
    /// certificate is invalidated
    bool update_certificate(X509Wrapper&& certificate);

    /// @brief Deletes a single certificate in the chain. Only in memory, use @ref export_certificates to filesystem
    /// export
    /// @return true if the certificate was found and deleted, false otherwise
    bool delete_certificate(const X509Wrapper& certificate);
    bool delete_certificate(const CertificateHashData& data);

    /// @brief Deletes all certificates. Only in memory, use @ref export_certificates to filesystem export
    void delete_all_certificates();

    /// @brief Returns a full exportable representation of a certificate bundle file in PEM format
    std::string to_export_string() const;

    /// @brief Exports the full certificate chain either as individual files if it is using a directory
    /// or as a bundle if it uses a bundle file, at the initially provided path. Also deletes/adds the updated
    /// certificates
    /// @return true on success, false otherwise
    bool export_certificates();

    /// @brief Syncs the file structure with the certificate store adding certificates that are not found on the
    /// storage and deleting the certificates that are not contained in this bundle
    bool sync_to_certificate_store();

public:
    const X509Wrapper& get_at(int index) {
        return certificates.at(index);
    }

    /// @brief returns the latest valid certificate within this bundle
    X509Wrapper get_latest_valid_certificate();

public:
    X509CertificateBundle& operator=(X509CertificateBundle&& other) = default;

public:
    /// @brief Loads all certificates from the string data that can contain multiple cetifs
    static std::vector<X509_ptr> load_certificates(const std::string& data, const EncodingFormat encoding);
    /// @brief Returns the latest valid certif that we might contain
    static X509Wrapper get_latest_valid_certificate(const std::vector<X509Wrapper>& certificates);

    static bool is_certificate_file(const std::filesystem::path& file) {
        return std::filesystem::is_regular_file(file) &&
               ((file.extension() == PEM_EXTENSION) || (file.extension() == DER_EXTENSION));
    }

private:
    /// @brief Adds to our certificate list the certificates found in the file
    /// @return number of added certificates
    void add_certifcates(const std::string& data, const EncodingFormat encoding,
                         const std::optional<std::filesystem::path>& path);

private:
    // Certificates in this chain, can only be loaded either from a bundle or a dir folder, never combined
    std::vector<X509Wrapper> certificates;
    // Relevant bundle or directory for this certificate chain
    std::filesystem::path path;
    // Source from where we created the certificates. If 'string' the 'export' functions will not work
    X509CertificateSource source;
};

/// @brief Unlike the bundle, this is loaded only from a directory that can contain a
/// combination of bundle files and single files. Used for easier operations on the
/// directory structures. All bundles will use individual files instead of directories
class X509CertificateDirectory {
public:
    X509CertificateDirectory(const std::filesystem::path& directory);

public:
    /// @brief Iterates through all the contained bundles, while the provided function
    /// returns true
    template <typename function> void for_each(function func) {
        for (const auto& bundle : bundles) {
            if (!func(bundle))
                break;
        }
    }

private:
    std::vector<X509CertificateBundle> bundles;
    std::filesystem::path directory;
};

} // namespace evse_security

#endif // X509_BUNDLE_HPP
