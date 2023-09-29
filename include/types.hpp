// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TYPES_HPP
#define TYPES_HPP

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace evse_security {

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
    MF
};

enum class CertificateType {
    V2GRootCertificate,
    MORootCertificate,
    CSMSRootCertificate,
    V2GCertificateChain, // is the certificate chain a certificate type?
    MFRootCertificate,
};

enum class HashAlgorithm {
    SHA256,
    SHA384,
    SHA512,
};

// the following 3 enum classes should go into evse_security
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

// why Status here and not Result as before?
enum class GetInstalledCertificatesStatus {
    Accepted,
    NotFound,
};

enum class GetKeyPairStatus {
    Accepted,
    Rejected,
    NotFound,
    NotFoundValid,
    PrivateKeyNotFound,
};

// types of evse_security

struct CertificateHashData {
    HashAlgorithm hash_algorithm; ///< Algorithm used for the hashes provided
    std::string issuer_name_hash; ///< The hash of the issuer's distinguished name (DN), calculated over the DER
                                  ///< encoding of the issuer's name field.
    std::string issuer_key_hash;  ///< The hash of the DER encoded public key: the value (excluding tag and length) of
                                  ///< the  subject public key field
    std::string serial_number; ///< The string representation of the hexadecimal value of the serial number without the
                               ///< prefix "0x" and without leading zeroes.

    bool operator==(const CertificateHashData& Other) {
        return hash_algorithm == Other.hash_algorithm && issuer_name_hash == Other.issuer_name_hash &&
               issuer_key_hash == Other.issuer_key_hash && serial_number == Other.serial_number;
    }
};
struct CertificateHashDataChain {
    CertificateType certificate_type; ///< Indicates the type of the certificate for which the hash data is provided
    CertificateHashData certificate_hash_data; ///< Contains the hash data of the certificate
    std::vector<CertificateHashData>
        child_certificate_hash_data;           ///< Contains the hash data of the child's certificates
};
struct GetInstalledCertificatesResult {
    GetInstalledCertificatesStatus status; ///< Indicates the status of the request
    std::vector<CertificateHashDataChain>
        certificate_hash_data_chain;       ///< the hashed certificate data for each requested certificates
};
struct OCSPRequestData {
    std::optional<CertificateHashData> certificate_hash_data; ///< Contains the hash data of the certificate
    std::optional<std::string> responder_url;                 ///< Contains the responder URL
};
struct OCSPRequestDataList {
    std::vector<OCSPRequestData> ocsp_request_data_list; ///< A list of OCSP request data
};
struct KeyPair {
    std::filesystem::path key;           ///< The path of the PEM or DER encoded private key
    std::filesystem::path certificate;   ///< The path of the PEM or DER encoded certificate
    std::optional<std::string> password; ///< Specifies the password for the private key if encrypted
};
struct GetKeyPairResult {
    GetKeyPairStatus status;
    std::optional<KeyPair> pair;
};

} // namespace evse_security

#endif // TYPES_HPP
