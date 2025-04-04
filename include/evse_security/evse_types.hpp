// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <vector>

#include <evse_security/utils/evse_filesystem_types.hpp>

namespace evse_security {

const fs::path PEM_EXTENSION = ".pem";
const fs::path DER_EXTENSION = ".der";
const fs::path KEY_EXTENSION = ".key";
const fs::path CUSTOM_KEY_EXTENSION = ".tkey";
const fs::path CERT_HASH_EXTENSION = ".hash";

enum class EncodingFormat {
    DER,
    PEM,
};

enum class CaCertificateType {
    V2G,
    MO,
    CSMS,
    MF,
};

enum class LeafCertificateType {
    CSMS,
    V2G,
    MF,
    MO,
};

enum class CertificateType {
    V2GRootCertificate,
    MORootCertificate,
    CSMSRootCertificate,
    V2GCertificateChain,
    MFRootCertificate,
};

enum class HashAlgorithm {
    SHA256,
    SHA384,
    SHA512,
};

enum class CertificateValidationResult {
    Valid,
    Expired,
    InvalidSignature,
    IssuerNotFound,
    InvalidLeafSignature,
    InvalidChain,
    Unknown,
};

enum class InstallCertificateResult {
    InvalidSignature,
    InvalidCertificateChain,
    InvalidFormat,
    InvalidCommonName,
    NoRootCertificateInstalled,
    Expired,
    CertificateStoreMaxLengthExceeded,
    WriteError,
    Accepted,
};

enum class DeleteCertificateResult {
    Accepted,
    Failed,
    NotFound,
};

enum class GetInstalledCertificatesStatus {
    Accepted,
    NotFound,
};

enum class GetCertificateInfoStatus {
    Accepted,
    Rejected,
    NotFound,
    NotFoundValid,
    PrivateKeyNotFound,
};

enum class GetCertificateSignRequestStatus {
    Accepted,
    InvalidRequestedType, ///< Requested a CSR for non CSMS/V2G leafs
    KeyGenError,          ///< The key could not be generated with the requested/default parameters
    GenerationError,      ///< Any other error when creating the CSR
};

inline bool str_cast_insensitive_cmp(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }

    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](std::string::value_type a, std::string::value_type b) {
        return (std::tolower(a) == std::tolower(b));
    });
}

// types of evse_security

struct CertificateHashData {
    HashAlgorithm hash_algorithm; ///< Algorithm used for the hashes provided
    std::string issuer_name_hash; ///< The hash of the issuer's distinguished name (DN), calculated over the DER
                                  ///< encoding of the issuer's name field.
    std::string issuer_key_hash;  ///< The hash of the DER encoded public key: the value (excluding tag and length) of
                                  ///< the  subject public key field
    std::string serial_number; ///< The string representation of the hexadecimal value of the serial number without the
                               ///< prefix "0x" and without leading zeroes.

#ifdef DEBUG_MODE_EVSE_SECURITY
    std::string debug_common_name; ///< Common name for easy debug
#endif

    bool operator==(const CertificateHashData& Other) const {
        return hash_algorithm == Other.hash_algorithm && issuer_name_hash == Other.issuer_name_hash &&
               issuer_key_hash == Other.issuer_key_hash && serial_number == Other.serial_number;
    }

    bool case_insensitive_comparison(const CertificateHashData& Other) const {
        if (hash_algorithm != Other.hash_algorithm) {
            return false;
        }

        if (false == str_cast_insensitive_cmp(issuer_name_hash, Other.issuer_name_hash)) {
            return false;
        }

        if (false == str_cast_insensitive_cmp(issuer_key_hash, Other.issuer_key_hash)) {
            return false;
        }

        if (false == str_cast_insensitive_cmp(serial_number, Other.serial_number)) {
            return false;
        }

        return true;
    }

    bool is_valid() {
        return (false == issuer_name_hash.empty()) && (false == issuer_key_hash.empty()) &&
               (false == serial_number.empty());
    }
};
struct CertificateHashDataChain {
    CertificateType certificate_type; ///< Indicates the type of the certificate for which the hash data is provided
    CertificateHashData certificate_hash_data; ///< Contains the hash data of the certificate
    std::vector<CertificateHashData>
        child_certificate_hash_data; ///< Contains the hash data of the child's certificates
};
struct GetInstalledCertificatesResult {
    GetInstalledCertificatesStatus status; ///< Indicates the status of the request
    std::vector<CertificateHashDataChain>
        certificate_hash_data_chain; ///< the hashed certificate data for each requested certificates
};
struct OCSPRequestData {
    std::optional<CertificateHashData> certificate_hash_data; ///< Contains the hash data of the certificate
    std::optional<std::string> responder_url;                 ///< Contains the responder URL
};
struct OCSPRequestDataList {
    std::vector<OCSPRequestData> ocsp_request_data_list; ///< A list of OCSP request data
};

struct CertificateOCSP {
    CertificateHashData hash;          ///< Hash of the certificate for which the OCSP data is held
    std::optional<fs::path> ocsp_path; ///< Path to the file in which the certificate OCSP data is held
};

struct CertificateInfo {
    fs::path key;                                ///< The path of the PEM or DER encoded private key
    std::optional<std::string> certificate_root; ///< The PEM of root certificate used by the leaf, has a value only
                                                 /// when using 'get_all_valid_certificates_info'
    std::optional<fs::path> certificate;         ///< The path of the PEM or DER encoded certificate chain if found
    std::optional<fs::path> certificate_single;  ///< The path of the PEM or DER encoded single certificate if found
    int certificate_count;               ///< The count of certificates, if the chain is available, or 1 if single
                                         /// (the root is not taken into account because of the OCSP cache)
    std::optional<std::string> password; ///< Specifies the password for the private key if encrypted
    std::vector<CertificateOCSP> ocsp;   ///< The ordered list of OCSP certificate data based on the chain file order
};

struct GetCertificateInfoResult {
    GetCertificateInfoStatus status;
    std::optional<CertificateInfo> info;
};

struct GetCertificateFullInfoResult {
    GetCertificateInfoStatus status;
    std::vector<CertificateInfo> info;
};

struct GetCertificateSignRequestResult {
    GetCertificateSignRequestStatus status;
    std::optional<std::string> csr;
};

namespace conversions {
std::string encoding_format_to_string(EncodingFormat e);
std::string ca_certificate_type_to_string(CaCertificateType e);
std::string leaf_certificate_type_to_string(LeafCertificateType e);
std::string leaf_certificate_type_to_filename(LeafCertificateType e);
std::string certificate_type_to_string(CertificateType e);
std::string hash_algorithm_to_string(HashAlgorithm e);
HashAlgorithm string_to_hash_algorithm(const std::string& s);
std::string install_certificate_result_to_string(InstallCertificateResult e);
std::string delete_certificate_result_to_string(DeleteCertificateResult e);
std::string get_installed_certificates_status_to_string(GetInstalledCertificatesStatus e);
std::string get_certificate_info_status_to_string(GetCertificateInfoStatus e);
} // namespace conversions

} // namespace evse_security
