// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <everest/timer.hpp>

#include <evse_security/crypto/evse_crypto.hpp>
#include <evse_security/evse_types.hpp>
#include <evse_security/utils/evse_filesystem_types.hpp>

#include <map>
#include <mutex>
#include <set>

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
    fs::path csms_leaf_cert_directory; /**< csms leaf certificate for OCPP shall be located in this directory */
    fs::path csms_leaf_key_directory;  /**< csms leaf key shall be located in this directory */
    fs::path secc_leaf_cert_directory; /**< secc leaf certificate for ISO15118 shall be located in this directory */
    fs::path secc_leaf_key_directory;  /**< secc leaf key shall be located in this directory */
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

struct CertificateQueryParams {
    LeafCertificateType certificate_type;
    EncodingFormat encoding{EncodingFormat::PEM};
    /// if OCSP information should be included
    bool include_ocsp{false};
    /// if the root certificate of the leaf should be included in the returned list
    bool include_root{false};
    /// if true, all valid leafs will be included, sorted in order, with the newest being
    /// first. If false, only the newest one will be returned
    bool include_all_valid{false};
    /// if true, will remove all duplicates found, since we can find a leaf for example
    /// in 2 files, one in 'leaf_single' and one in 'leaf_chain'. For delete routines
    /// we need both files returned, while for queries (v2g_chain) we don't need duplicates
    bool remove_duplicates{false};
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
    /// PKIs. For CA certificates either CA bundle files or directories containing the certificates can be specified.
    /// For the SECC and CSMS leaf certificates, directories must be specified.
    /// @param file_paths specifies the certificate and key storage locations on the filesystem
    /// @param private_key_password optional password for encrypted private keys
    /// @param max_fs_usage_bytes optional maximum filesystem usage for certificates. Defaults to
    /// 'DEFAULT_MAX_FILESYSTEM_SIZE'
    /// @param max_fs_certificate_store_entries optional maximum certificate entries. Defaults to
    /// 'DEFAULT_MAX_CERTIFICATE_ENTRIES'
    /// @param csr_expiry optional expiry time for a CSR entry. If the time is exceeded it will be deleted. Defaults to
    /// 'DEFAULT_CSR_EXPIRY'
    /// @param garbage_collect_time optional garbage collect time. How often we will delete expired CSRs and
    /// certificates. Defaults to 'DEFAULT_GARBAGE_COLLECT_TIME'
    EvseSecurity(const FilePaths& file_paths, const std::optional<std::string>& private_key_password = std::nullopt,
                 const std::optional<std::uintmax_t>& max_fs_usage_bytes = std::nullopt,
                 const std::optional<std::uintmax_t>& max_fs_certificate_store_entries = std::nullopt,
                 const std::optional<std::chrono::seconds>& csr_expiry = std::nullopt,
                 const std::optional<std::chrono::seconds>& garbage_collect_time = std::nullopt);

    /// @brief Destructor
    ~EvseSecurity();

    /// @brief Installs the given \p certificate within the specified CA bundle file or directory if directories are
    /// used. If the certificate already exists it will only be updated
    /// @param certificate PEM formatted CA certificate
    /// @param certificate_type specifies the CA certificate type
    /// @return result of the operation
    InstallCertificateResult install_ca_certificate(const std::string& certificate, CaCertificateType certificate_type);

    /// @brief Deletes the certificate specified by \p certificate_hash_data . If a CA certificate is specified, the
    /// certificate is removed from the bundle or directory. If a leaf certificate is specified, the file will be
    /// removed from the filesystem. It will also delete all certificates issued by this certificate, so that no invalid
    /// hierarchies persisted on the filesystem
    /// @param certificate_hash_data specifies the certificate to be deleted
    /// @return result of the operation
    DeleteResult delete_certificate(const CertificateHashData& certificate_hash_data);

    /// @brief Verifies the given \p certificate_chain against the respective CA
    /// trust anchors (indicated by the given \p certificate_type) for the leaf.
    /// @param certificate_chain PEM formatted certificate or certificate chain
    /// @param certificate_type type of the root certificate for which the chain is verified
    /// @return result of the operation
    CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                   const LeafCertificateType certificate_type);

    /// @brief Verifies the given \p certificate_chain against the respective CA
    /// trust anchors (indicated by the given \p certificate_types) for the leaf.
    /// @param certificate_chain PEM formatted certificate or certificate chain
    /// @param certificate_types types of the root certificates for which the chain is verified
    /// @return result of the operation
    CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                   const std::vector<LeafCertificateType>& certificate_types);

    /// @brief Verifies the given \p certificate_chain for the given \p certificate_type against the respective CA
    /// certificates for the leaf and if valid installs the certificate on the filesystem. Before installing on the
    /// filesystem, this function checks if a private key is present for the given certificate on the filesystem. Two
    /// files are installed, one containing the single leaf (presuming it is the first in the chain) and also the full
    /// certificate chain. The \ref get_key_pair function will return a path to both files if they exist, the one
    /// containing the single leaf, and the file containing the leaf including the SUBCAs if present
    /// @param certificate_chain PEM formatted certificate or certificate chain
    /// @param certificate_type type of the leaf certificate
    /// @return result of the operation
    InstallCertificateResult update_leaf_certificate(const std::string& certificate_chain,
                                                     LeafCertificateType certificate_type);

    /// @brief Retrieves all certificates installed on the filesystem applying the \p certificate_type filter.
    /// In the case of the 'V2GCertificateChain', it will return all valid leafs chains, with the newest being
    /// the first in the list
    /// @param certificate_types
    /// @return contains the certificate hash data chains of the requested \p certificate_type
    GetInstalledCertificatesResult get_installed_certificate(CertificateType certificate_type);

    /// @brief Retrieves all certificates installed on the filesystem applying the \p certificate_types filter.
    /// @param certificate_types
    /// @return contains the certificate hash data chains of the requested \p certificate_types
    GetInstalledCertificatesResult get_installed_certificates(const std::vector<CertificateType>& certificate_types);

    /// @brief Retrieves the certificate count applying the \p certificate_types filter.
    int get_count_of_installed_certificates(const std::vector<CertificateType>& certificate_types);

    /// @brief Command to retrieve the OCSP request data of the V2G certificates (SubCAs and possibly V2G leaf)
    /// @return contains OCSP request data
    OCSPRequestDataList get_v2g_ocsp_request_data();

    /// @brief Retrieves the OCSP request data of the given MO contract \p certificate_chain. Since
    /// the provided MO chain can be signed by both the MO_ROOT and the V2G_ROOT, the received chain
    /// will be tested against both
    /// @param certificate_chain PEM formatted MO certificate or MO certificate chain
    /// @return contains OCSP request data
    OCSPRequestDataList get_mo_ocsp_request_data(const std::string& certificate_chain);

    /// @brief Updates the OCSP cache for the given \p certificate_hash_data with the given \p ocsp_response
    /// @param certificate_hash_data identifies the certificate for which the \p ocsp_response is specified
    /// @param ocsp_response the actual OCSP data
    void update_ocsp_cache(const CertificateHashData& certificate_hash_data, const std::string& ocsp_response);

    // TODO: Switch to path
    /// @brief Retrieves from the OCSP cache for the given \p certificate_hash_data
    /// @param certificate_hash_data identifies the certificate for which the \p ocsp_response is specified
    /// @return the actual OCSP data or an empty value
    std::optional<fs::path> retrieve_ocsp_cache(const CertificateHashData& certificate_hash_data);

    /// @brief Indicates if a CA certificate for the given \p certificate_type is installed on the filesystem
    /// Supports both CA certificate bundles and directories
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
    /// @param use_custom_provider  If the custom provider (which can be the TPM if using -DUSING_TPM2=ON) should be
    /// used for the CSR request
    /// @return the status and an optional PEM formatted certificate signing request string
    GetCertificateSignRequestResult generate_certificate_signing_request(LeafCertificateType certificate_type,
                                                                         const std::string& country,
                                                                         const std::string& organization,
                                                                         const std::string& common,
                                                                         bool use_custom_provider);

    /// @brief Generates a certificate signing request for the given \p certificate_type , \p country , \p organization
    /// and \p common without using the TPM
    /// @param certificate_type
    /// @param country
    /// @param organization
    /// @param common
    /// @return the status and an optional PEM formatted certificate signing request string
    GetCertificateSignRequestResult generate_certificate_signing_request(LeafCertificateType certificate_type,
                                                                         const std::string& country,
                                                                         const std::string& organization,
                                                                         const std::string& common);

    /// @brief Searches the filesystem on the specified directories for the given \p certificate_type and retrieves the
    /// most recent certificate that is already valid and the respective key.  If no certificate is present or no key is
    /// matching the certificate, this function returns a GetKeyPairStatus other than "Accepted". The function \ref
    /// update_leaf_certificate will install two files for each leaf, one containing the single leaf and one containing
    /// the leaf including any possible SUBCAs
    /// @param certificate_type type of the leaf certificate
    /// @param encoding specifies PEM or DER format
    /// @param include_ocsp if OCSP data should be included
    /// @return contains response result, with info related to the certificate chain and response status
    GetCertificateInfoResult get_leaf_certificate_info(LeafCertificateType certificate_type, EncodingFormat encoding,
                                                       bool include_ocsp = false);

    /// @brief Finds the latest valid leafs, for each root certificate that is present on the filesystem, and
    /// returns all the newest valid leafs that are present for different roots. This is required, because
    /// a query parameter when requesting the leaf is not advisable during the TLS handshake
    /// Existing filesystem:
    /// ROOT_V2G_Hubject->SUB_CA1->SUB_CA2->Leaf_Invalid_A
    /// ROOT_V2G_Hubject->SUB_CA1->SUB_CA2->Leaf_Valid_A
    /// ROOT_V2G_Hubject->SUB_CA1->SUB_CA2->Leaf_Valid_B
    /// ROOT_V2G_OtherProvider->SUB_CA_O1->SUB_CA_O2->Leav_Valid_A
    /// will return:
    /// ROOT_V2G_Hubject->SUB_CA1->SUB_CA2->Leaf_Valid_B +
    /// ROOT_V2G_OtherProvider->SUB_CA_O1->SUB_CA_O2->Leav_Valid_A
    /// Note: non self-signed roots and cross-signed certificates are not supported
    /// @param certificate_type type of leaf certificate that we start the search from
    /// @param encoding specifies PEM or DER format
    /// @param include_ocsp if OCSP data should be included
    /// @return contains response result, with info related to the full certificate chains info and response status
    GetCertificateFullInfoResult get_all_valid_certificates_info(LeafCertificateType certificate_type,
                                                                 EncodingFormat encoding, bool include_ocsp = false);

    /// @brief Checks and updates the symlinks for the V2G leaf certificates and keys to the most recent valid one
    /// @return true if one of the links was updated
    bool update_certificate_links(LeafCertificateType certificate_type);

    /// @brief Retrieves the PEM formatted CA bundle file for the given \p certificate_type It is not recommended to
    /// add the SUBCAs to any root certificate bundle, but to leave them in the leaf file. See \ref
    /// update_leaf_certificate
    /// @param certificate_type
    /// @return CA certificate file
    std::string get_verify_file(CaCertificateType certificate_type);

    /// @brief Retrieves the PEM formatted CA bundle location for the given \p certificate_type It is not recommended to
    /// add the SUBCAs to any root certificate bundle, but to leave them in the leaf file. Returns either file
    /// or directory where the certificates are located. In the case of directory, does also rehashing the directory.
    /// @param certificate_type
    /// @return CA certificate location
    std::string get_verify_location(CaCertificateType certificate_type);

    /// @brief An extension of 'get_verify_file' with error handling included
    GetCertificateInfoResult get_ca_certificate_info(CaCertificateType certificate_type);

    /// @brief Gets the expiry day count for the leaf certificate of the given \p certificate_type
    /// @param certificate_type
    /// @return day count until the leaf certificate expires
    int get_leaf_expiry_days_count(LeafCertificateType certificate_type);

    /// @brief Collects and deletes unfulfilled CSR private keys. It also deletes the expired
    /// certificates. The caller must be sure the system clock is properly set for detecting expired
    /// certificates. A minimum of 'DEFAULT_MINIMUM_CERTIFICATE_ENTRIES' certificates to
    /// have a safeguard against a poorly set system clock
    void garbage_collect();

    /// @brief Verifies the file at the given \p path using the provided \p signing_certificate and \p signature
    /// @param path
    /// @param signing_certificate
    /// @param signature
    /// @return true if the verification was successful, false if not
    static bool verify_file_signature(const fs::path& path, const std::string& signing_certificate,
                                      const std::string signature);

    /// @brief Decodes the base64 encoded string to the raw byte representation
    /// @param base64_string base64 encoded string
    /// @return decoded byte vector
    static std::vector<std::uint8_t> base64_decode_to_bytes(const std::string& base64_string);

    /// @brief Decodes the base64 encoded string to string representation
    /// @param base64_string base64 encoded string
    /// @return decoded string array
    static std::string base64_decode_to_string(const std::string& base64_string);

    /// @brief Encodes the raw bytes to a base64 string
    /// @param decoded_bytes raw byte array
    /// @return encoded base64 string
    static std::string base64_encode_from_bytes(const std::vector<std::uint8_t>& bytes);

    /// @brief Encodes the string containing raw bytes to a base64 string
    /// @param decoded_bytes string containing raw bytes
    /// @return encoded base64 string
    static std::string base64_encode_from_string(const std::string& string);

private:
    // Internal versions of the functions do not lock the mutex
    CertificateValidationResult verify_certificate_internal(const std::string& certificate_chain,
                                                            const std::vector<LeafCertificateType>& certificate_types);

    GetCertificateInfoResult get_leaf_certificate_info_internal(LeafCertificateType certificate_type,
                                                                EncodingFormat encoding, bool include_ocsp = false);

    /// @brief Retrieves information related to leaf certificates
    GetCertificateFullInfoResult get_full_leaf_certificate_info_internal(const CertificateQueryParams& params);

    OCSPRequestDataList generate_ocsp_request_data_internal(const std::set<CaCertificateType>& possible_roots,
                                                            const std::string& leaf_chain);

    GetCertificateInfoResult get_ca_certificate_info_internal(CaCertificateType certificate_type);
    std::optional<fs::path> retrieve_ocsp_cache_internal(const CertificateHashData& certificate_hash_data);

    bool is_ca_certificate_installed_internal(CaCertificateType certificate_type);

    GetCertificateSignRequestResult
    generate_certificate_signing_request_internal(LeafCertificateType certificate_type,
                                                  const CertificateSigningRequestInfo& info);

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
    FRIEND_TEST(EvseSecurityTests, verify_directory_bundles);
    FRIEND_TEST(EvseSecurityTests, verify_full_filesystem_install_reject);
    FRIEND_TEST(EvseSecurityTests, verify_full_filesystem);
    FRIEND_TEST(EvseSecurityTests, verify_expired_csr_deletion);
    FRIEND_TEST(EvseSecurityTests, verify_ocsp_garbage_collect);
    FRIEND_TEST(EvseSecurityTestsExpired, verify_expired_leaf_deletion);
#endif
};

} // namespace evse_security
