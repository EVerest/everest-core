// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <everest/timer.hpp>

#include <evse_security/evse_types.hpp>
#include <evse_security/utils/evse_filesystem_types.hpp>

#include <map>
#include <mutex>

#ifdef BUILD_TESTING_EVSE_SECURITY
#include <gtest/gtest_prod.h>
#endif

namespace evse_security {

struct LinkPaths {
    fs::path secc_leaf_cert_link;
    fs::path secc_leaf_key_link;
    fs::path cpo_cert_chain_link;
};

struct DirectoryPaths {
    fs::path csms_leaf_cert_directory;
    fs::path csms_leaf_key_directory;
    fs::path secc_leaf_cert_directory;
    fs::path secc_leaf_key_directory;
};
struct FilePaths {
    // bundle paths
    fs::path csms_ca_bundle;
    fs::path mf_ca_bundle;
    fs::path mo_ca_bundle;
    fs::path v2g_ca_bundle;

    DirectoryPaths directories;
    LinkPaths links;
};

// Unchangeable security limit for certificate deletion, a min entry count will be always kept (newest)
static constexpr std::size_t DEFAULT_MINIMUM_CERTIFICATE_ENTRIES = 10;
// 50 MB default limit for filesystem usage
static constexpr std::uintmax_t DEFAULT_MAX_FILESYSTEM_SIZE = 1024 * 1024 * 50;
// Default maximum 2000 certificate entries
static constexpr std::uintmax_t DEFAULT_MAX_CERTIFICATE_ENTRIES = 2000;

// Expiry for CSRs that did not receive a response CSR, 60 minutes
static std::chrono::seconds DEFAULT_CSR_EXPIRY(3600);

// Garbage collect default time, 20 minutes
static std::chrono::seconds DEFAULT_GARBAGE_COLLECT_TIME(20 * 60);

/// @brief This class holds filesystem paths to CA bundle file locations and directories for leaf certificates
class EvseSecurity {

public:
    /// @brief Constructor initializes the certificate and key storage using the given \p file_paths for the different
    /// PKIs. For CA certificates CA bundle files must be specified. For the SECC and CSMS leaf certificates,
    /// directories are specified.
    /// @param file_paths specifies the certificate and key storage locations on the filesystem
    /// @param private_key_password optional password for encrypted private keys
    EvseSecurity(const FilePaths& file_paths, const std::optional<std::string>& private_key_password = std::nullopt,
                 const std::optional<std::uintmax_t>& max_fs_usage_bytes = std::nullopt,
                 const std::optional<std::uintmax_t>& max_fs_certificate_store_entries = std::nullopt,
                 const std::optional<std::chrono::seconds>& csr_expiry = std::nullopt,
                 const std::optional<std::chrono::seconds>& garbage_collect_time = std::nullopt);

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

    /// @brief Retrieves the certificate count applying the \p certificate_types filter.
    int get_count_of_installed_certificates(const std::vector<CertificateType>& certificate_types);

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

    /// @brief Should be invoked when a certificate CSR was not properly generated by the CSMS
    /// and that the pairing key that was generated should be deleted
    void certificate_signing_request_failed(const std::string& csr, LeafCertificateType certificate_type);

    /// @brief Generates a certificate signing request for the given \p certificate_type , \p country , \p organization
    /// and \p common , uses the TPM if \p use_tpm is true
    /// @param certificate_type
    /// @param country
    /// @param organization
    /// @param common
    /// @param use_tpm  If the TPM should be used for the CSR request
    /// @return the PEM formatted certificate signing request
    std::string generate_certificate_signing_request(LeafCertificateType certificate_type, const std::string& country,
                                                     const std::string& organization, const std::string& common,
                                                     bool use_tpm);

    /// @brief Generates a certificate signing request for the given \p certificate_type , \p country , \p organization
    /// and \p common without using the TPM
    /// @param certificate_type
    /// @param country
    /// @param organization
    /// @param common
    /// @return the PEM formatted certificate signing request
    std::string generate_certificate_signing_request(LeafCertificateType certificate_type, const std::string& country,
                                                     const std::string& organization, const std::string& common);

    /// @brief Searches the filesystem on the specified directories for the given \p certificate_type and retrieves the
    /// most recent certificate that is already valid and the respective key.  If no certificate is present or no key is
    /// matching the certificate, this function returns std::nullopt
    /// @param certificate_type type of the leaf certificate
    /// @param encoding specifies PEM or DER format
    /// @return contains response result
    GetKeyPairResult get_key_pair(LeafCertificateType certificate_type, EncodingFormat encoding);

    /// @brief Checks and updates the symlinks for the V2G leaf certificates and keys to the most recent valid one
    /// @return true if one of the links was updated
    bool update_certificate_links(LeafCertificateType certificate_type);

    /// @brief Retrieves the PEM formatted CA bundle file for the given \p certificate_type
    /// @param certificate_type
    /// @return CA certificate file
    std::string get_verify_file(CaCertificateType certificate_type);

    /// @brief Gets the expiry day count for the leaf certificate of the given \p certificate_type
    /// @param certificate_type
    /// @return day count until the leaf certificate expires
    int get_leaf_expiry_days_count(LeafCertificateType certificate_type);

    /// @brief Collects and deletes unfulfilled CSR private keys. If also deleting the expired
    /// certificates, make sure the system clock is properly set for detecting expired certificates
    void garbage_collect();

    /// @brief Verifies the file at the given \p path using the provided \p signing_certificate and \p signature
    /// @param path
    /// @param signing_certificate
    /// @param signature
    /// @return true if the verification was successful, false if not
    static bool verify_file_signature(const fs::path& path, const std::string& signing_certificate,
                                      const std::string signature);

private:
    // Internal versions of the functions do not lock the mutex
    InstallCertificateResult verify_certificate_internal(const std::string& certificate_chain,
                                                         LeafCertificateType certificate_type);
    GetKeyPairResult get_key_pair_internal(LeafCertificateType certificate_type, EncodingFormat encoding);

    /// @brief Determines if the total filesize of certificates is > than the max_filesystem_usage bytes
    bool is_filesystem_full();

private:
    static std::mutex security_mutex;

    // why not reusing the FilePaths here directly (storage duplication)
    std::map<CaCertificateType, fs::path> ca_bundle_path_map;
    DirectoryPaths directories;
    LinkPaths links;

    // CSRs that were generated and require an expiry time
    std::map<fs::path, std::chrono::time_point<std::chrono::steady_clock>> managed_csr;

    // Maximum filesystem usage
    std::uintmax_t max_fs_usage_bytes;
    // Maximum filesystem certificate entries
    std::uintmax_t max_fs_certificate_store_entries;
    // Default csr expiry in seconds
    std::chrono::seconds csr_expiry;
    // Default time to garbage collect
    std::chrono::seconds garbage_collect_time;

    // GC timer
    Everest::SteadyTimer garbage_collect_timer;

    // FIXME(piet): map passwords to encrypted private key files
    // is there only one password for all private keys?
    std::optional<std::string> private_key_password; // used to decrypt encrypted private keys

private:
// Define here all tests that require internal function usage
#ifdef BUILD_TESTING_EVSE_SECURITY
    FRIEND_TEST(EvseSecurityTests, verify_full_filesystem_install_reject);
    FRIEND_TEST(EvseSecurityTests, verify_full_filesystem);
    FRIEND_TEST(EvseSecurityTests, verify_expired_csr_deletion);
    FRIEND_TEST(EvseSecurityTests, verify_expired_leaf_deletion);
#endif
};

} // namespace evse_security
