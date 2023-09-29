// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SECURITY_HPP
#define EVSE_SECURITY_HPP

#include <filesystem>
#include <fstream>
#include <types.hpp>

namespace evse_security {

struct DirectoryPaths {
    std::filesystem::path csms_leaf_cert_directory;
    std::filesystem::path csms_leaf_key_directory;
    std::filesystem::path secc_leaf_cert_directory;
    std::filesystem::path secc_leaf_key_directory;
};
struct FilePaths {
    // bundle paths
    std::filesystem::path csms_ca_bundle;
    std::filesystem::path mf_ca_bundle;
    std::filesystem::path mo_ca_bundle;
    std::filesystem::path v2g_ca_bundle;

    DirectoryPaths directories;
};

/// @brief This class holds filesystem paths to CA bundle file locations and directories for leaf certificates
class EvseSecurity {

public:
    /// @brief Constructor initializes the certificate and key storage using the given \p file_paths for the different
    /// PKIs. For CA certificates CA bundle files must be specified. For the SECC and CSMS leaf certificates,
    /// directories are specified.
    /// @param file_paths specifies the certificate and key storage locations on the filesystem
    /// @param private_key_password optional password for encrypted private keys
    EvseSecurity(const FilePaths& file_paths, const std::optional<std::string>& private_key_password = std::nullopt);

    /// @brief Destructor
    ~EvseSecurity();

    /// @brief Installs the given \p certificate within the specified CA bundle file
    /// @param certificate PEM formatted CA certificate
    /// @param certificate_type specifies the CA certificate type
    /// @return result of the operation
    InstallCertificateResult install_ca_certificate(const std::string& certificate, CaCertificateType certificate_type);

    /// @brief Deletes the certificate specified by \p certificate_hash_data . If a CA certificate is specified, the
    /// certificate is removed from the bundle. If a leaf certificate is specified, the file will be removed from the
    /// filesystem
    /// @param certificate_hash_data specifies the certificate to be deleted
    /// @return result of the operation
    DeleteCertificateResult delete_certificate(const CertificateHashData& certificate_hash_data);

    /// @brief Verifies the given \p certificate_chain for the given \p certificate_type against the respective CA
    /// certificates for the leaf.
    /// @param certificate_chain PEM formatted certificate or certificate chain
    /// @param certificate_type type of the leaf certificate
    /// @return result of the operation
    InstallCertificateResult verify_certificate(const std::string& certificate_chain,
                                                const LeafCertificateType certificate_type);

    /// @brief Verifies the given \p certificate_chain for the given \p certificate_type against the respective CA
    /// certificates for the leaf and if valid installs the certificate on the filesystem. Before installing on the
    /// filesystem, this function checks if a private key is present for the given certificate on the filesystem.
    /// @param certificate_chain PEM formatted certificate or certificate chain
    /// @param certificate_type type of the leaf certificate
    /// @return result of the operation
    InstallCertificateResult update_leaf_certificate(const std::string& certificate_chain,
                                                     LeafCertificateType certificate_type);

    /// @brief Retrieves all certificates installed on the filesystem applying the \p certificate_type filter.
    /// @param certificate_types
    /// @return contains the certificate hash data chains of the requested \p certificate_type
    GetInstalledCertificatesResult get_installed_certificate(CertificateType certificate_type);

    /// @brief Retrieves all certificates installed on the filesystem applying the \p certificate_types filter.
    /// @param certificate_types
    /// @return contains the certificate hash data chains of the requested \p certificate_types
    GetInstalledCertificatesResult get_installed_certificates(const std::vector<CertificateType>& certificate_types);

    /// @brief Retrieves the OCSP request data of the V2G certificates
    /// @return contains OCSP request data
    OCSPRequestDataList get_ocsp_request_data();

    /// @brief Updates the OCSP cache for the given \p certificate_hash_data with the given \p ocsp_response
    /// @param certificate_hash_data identifies the certificate for which the \p ocsp_response is specified
    /// @param ocsp_response the actual OCSP data
    void update_ocsp_cache(const CertificateHashData& certificate_hash_data, const std::string& ocsp_response);

    /// @brief Indicates if a CA certificate for the given \p certificate_type is installed on the filesystem
    /// @param certificate_type
    /// @return true if CA certificate is present, else false
    bool is_ca_certificate_installed(CaCertificateType certificate_type);

    /// @brief Generates a certificate signing request for the given \p certificate_type , \p country , \p organization
    /// and \p common
    /// @param certificate_type
    /// @param country
    /// @param organization
    /// @param common
    /// @return the PEM formatted certificate signing request
    // are these 4 params enough for a full request? I guess there are much more
    std::string generate_certificate_signing_request(LeafCertificateType certificate_type, const std::string& country,
                                                     const std::string& organization, const std::string& common);

    /// @brief Searches the filesystem on the specified directories for the given \p certificate_type and retrieves the
    /// most recent certificate that is already valid and the respective key.  If no certificate is present or no key is
    /// matching the certificate, this function returns std::nullopt
    /// @param certificate_type type of the leaf certificate
    /// @param encoding specifies PEM or DER format
    /// @return contains response result
    GetKeyPairResult get_key_pair(LeafCertificateType certificate_type, EncodingFormat encoding);

    /// @brief Retrieves the PEM formatted CA bundle file for the given \p certificate_type
    /// @param certificate_type
    /// @return CA certificate file
    std::string get_verify_file(CaCertificateType certificate_type);

    /// @brief Gets the expiry day count for the leaf certificate of the given \p certificate_type
    /// @param certificate_type
    /// @return day count until the leaf certificate expires
    int get_leaf_expiry_days_count(LeafCertificateType certificate_type);

private:
    // why not reusing the FilePaths here directly (storage duplication)
    std::map<CaCertificateType, std::filesystem::path> ca_bundle_path_map;
    DirectoryPaths directories;

    // FIXME(piet): map passwords to encrypted private key files
    // is there only one password for all private keys?
    std::optional<std::string> private_key_password; // used to decrypt encrypted private keys;
};

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

} // namespace evse_security

#endif // EVSE_SECURITY_HPP
