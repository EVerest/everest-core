// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_PKI_HANDLER
#define OCPP_COMMON_PKI_HANDLER

#include <boost/filesystem.hpp>
#include <memory>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <string>

#include <ocpp/common/types.hpp>

namespace ocpp {

enum class PkiEnum {
    CSMS, // Charging Station Management System
    CSO,  // Charging Station Owner
    CPS,  // Certificate Provisioning Service
    MF,   // Manufacturer of the Charging Station
    MO,   // Mobility Operator
    OEM,  // Car Manufacturer
    V2G   // Vehicle To Grid
};

/// \brief Struct for OCSPRequestData
struct OCSPRequestData {
    HashAlgorithmEnumType hashAlgorithm;
    std::string issuerNameHash;
    std::string issuerKeyHash;
    std::string serialNumber;
    std::string responderUrl;

    bool operator==(const OCSPRequestData& other) {
        return this->hashAlgorithm == other.hashAlgorithm && this->issuerNameHash == other.issuerNameHash &&
               this->issuerKeyHash == other.issuerKeyHash && this->serialNumber == other.serialNumber &&
               this->responderUrl == other.responderUrl;
    };
};

using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using BIO_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
using X509_STORE_ptr = std::unique_ptr<X509_STORE, decltype(&::X509_STORE_free)>;
using X509_STORE_CTX_ptr = std::unique_ptr<X509_STORE_CTX, decltype(&::X509_STORE_CTX_free)>;
using X509_REQ_ptr = std::unique_ptr<X509_REQ, decltype(&::X509_REQ_free)>;

// filenames of ca certificates
const boost::filesystem::path CSMS_ROOT_CA("CSMS_ROOT_CA.pem");
const boost::filesystem::path CSMS_ROOT_CA_BACKUP("CSMS_ROOT_CA_BACKUP.pem");

// filenames of leaf certificates, csrs and private keys
const boost::filesystem::path CSMS_LEAF("CSMS_LEAF.pem");
const boost::filesystem::path CSMS_LEAF_KEY("CSMS_LEAF.key");
const boost::filesystem::path CSMS_LEAF_KEY_BACKUP("CSMS_LEAF_BACKUP.key");
const boost::filesystem::path CSMS_CSR("CSMS_CSR.pem");
const boost::filesystem::path V2G_LEAF("SECC_LEAF.pem"); // SECC_LEAF in ISO15118
const boost::filesystem::path V2G_LEAF_KEY("SECC_LEAF.key");
const boost::filesystem::path V2G_LEAF_KEY_BACKUP("SECC_LEAF_BACKUP.key");
const boost::filesystem::path V2G_CSR("SECC_CSR.pem");

struct X509Certificate {
    boost::filesystem::path path;
    X509* x509;
    std::string str;
    CertificateType type;
    int validIn; // seconds. If > 0 cert is not valid yet
    int validTo; // seconds. If < 0 cert has expired

    X509Certificate(){};
    X509Certificate(boost::filesystem::path path, X509* x509, std::string str);
    ~X509Certificate();

    /// \brief writes the X509 certificate to the path set
    bool write();
    /// \brief writes the X509 certificate to the given \p path
    bool write(const boost::filesystem::path& path);

    /// \brief Gets CN of certificate
    std::string getCommonName();

    /// \brief Gets issuer name hash of certificate
    std::string getIssuerNameHash();

    /// \brief Gets serial number of certificate
    std::string getSerialNumber();

    /// \brief Gets issuer key hash of certificate
    std::string getIssuerKeyHash();

    /// \brief Gets OCSP responder URL of certificate if present, else returns an empty string
    std::string getResponderURL();

    /// \brief Gets OCSP request data of certificate
    OCSPRequestData getOCSPRequestData();
};
/// \brief loads a X509Certificate from the given \p pat
std::shared_ptr<X509Certificate> loadFromFile(const boost::filesystem::path& path);
/// \brief loads a X509Certificate from the given \p st
std::shared_ptr<X509Certificate> loadFromString(std::string& str);
/// \brief reads a file from the given \p path to a strin
std::string readFileToString(const boost::filesystem::path& path);

/// \brief Handler for verifying, installing, deleting and managing certificates and security related operations.
/// Requires CA files to be located inside the following directory structure according to their use:
/// ca/csms (CA certificates of Charging Station Management System)
/// ca/cso (CA certificates of Charging Station Owner)
/// ca/mf (CA certificates of  Manufacturer)
/// ca/mo (CA certificates of Mobility Operator)
///
/// client/csms (Client certificates, Private keys, Password files for TLS websocket connection to Charging Station
/// Management System)
/// client/cso (Client certificates, Private keys, Password files for ISO15118 TLS connection to electric vehicle)
///
/// Use the default filenames CSMS_ROOT_CA, CSMS_ROOT_CA_BACKUP, CSMS_LEAF, CSMS_LEAF_KEY, CSMS_CSR, V2G_LEAF,
/// V2G_LEAF_KEY, V2G_CSR

class PkiHandler {

private:
    boost::filesystem::path certsPath;
    boost::filesystem::path caPath;
    boost::filesystem::path caCsmsPath;
    boost::filesystem::path caCsoPath;
    boost::filesystem::path caCpsPath;
    boost::filesystem::path caMfPath;
    boost::filesystem::path caMoPath;
    boost::filesystem::path caOemPath;
    boost::filesystem::path caV2gPath;

    boost::filesystem::path clientCsmsPath;
    boost::filesystem::path clientCsoPath;

    bool useRootCaFallback;

    /// \brief Returns all root and intermediate certificates for the given \p pki
    std::vector<std::shared_ptr<X509Certificate>> getCaCertificates(const PkiEnum& pki, const CertificateType& type,
                                                                    const bool includeSymlinks = false);

    /// \brief Returns all root and intermediate certificates
    std::vector<std::shared_ptr<X509Certificate>> getCaCertificates(const bool includeSymlinks = false);

    /// \brief Verifies the given \p certificate / certificate_chain. Verifies
    /// certificate_chain, signature, expiration,
    /// \param certificateType type of PKI //TODO(piet): Propably use another enum here
    /// \param certificates certificate_chain with leaf certificate at element 0 and
    /// optionally subCAs at succeeding positions
    /// \param chargeBoxSerialNumber \return
    CertificateVerificationResult
    verifyCertificate(const PkiEnum& pki, const std::vector<std::shared_ptr<X509Certificate>>& certificates,
                      const std::optional<std::string>& chargeBoxSerialNumber = std::nullopt);

    /// \brief Splits the given \p certChain into single certificates and returns them
    std::vector<std::shared_ptr<X509Certificate>> getCertificatesFromChain(const std::string& certChain);

    /// \brief Returns the path where the ca certificates of the given \p pki are located
    boost::filesystem::path getCaPath(const PkiEnum& pki);

    /// \brief Executes "openssl rehash" for the given \p caPath
    void execOpenSSLRehash(const boost::filesystem::path caPath);

    /// \brief Returns the file path of the CSMS root CA file
    std::optional<boost::filesystem::path> getCsmsCaFilePath();

    /// \brief Returns the number of installed CSMS CA certificates
    int getNumberOfCsmsCaCertificates();

    /// \brief Returns all certificate chains of the given \p certificates
    std::vector<std::vector<std::shared_ptr<X509Certificate>>>
    getCaCertificateChains(const std::vector<std::shared_ptr<X509Certificate>>& certificates);

    /// \brief Returns the V2GCertificateChain containing the V2G Client certificate at index 0 if present
    std::vector<std::shared_ptr<X509Certificate>> getV2GCertificateChain();

    /// \brief Returns true if the given \p certificate is already installed, else returns false
    bool isCaCertificateAlreadyInstalled(const std::shared_ptr<X509Certificate>& certificate);

public:
    explicit PkiHandler(const boost::filesystem::path& certsPath, const bool multipleCsmsCaNotAllowed);

    /// \brief Returns the path where the certificates are located
    boost::filesystem::path getCaCsmsPath();

    /// \brief Verifies the given \p certificate and the \p charge_box_serial_number using the
    /// CentralSystemRootCertificate This method verifies the certificate chain, the signature, and the period when the
    /// certificate is valid
    CertificateVerificationResult verifyChargepointCertificate(const std::string& certificate,
                                                               const std::string& chargeBoxSerialNumber);

    /// \brief Verifies the given \p certificate using the V2GRootCertificate. This method verifies the certificate
    /// chain, the signature, and the period when the certificate is valid
    CertificateVerificationResult verifyV2GChargingStationCertificate(const std::string& certificate,
                                                                      const std::string& chargeBoxSerialNumber);

    /// \brief Verifies the given \p firmwareCertificate .
    /// This method verifies the certificate chain
    CertificateVerificationResult verifyFirmwareCertificate(const std::string& firmwareCertificate);

    /// \brief writes the given \p certificate to the client certificate file with the given \p certificateUse located
    /// in the certs directory
    void writeClientCertificate(const std::string& certificate, const CertificateSigningUseEnum& certificateUse);

    /// \brief generates a certifcate signing request from the given parameters
    std::string generateCsr(const CertificateSigningUseEnum& certificateType, const std::string& szCountry,
                            const std::string& szOrganization, const std::string& szCommon);

    /// \brief indicates if a central system root certificate is installed
    bool isCentralSystemRootCertificateInstalled();

    /// \brief indicates if a V2G root certificate is installed
    bool isV2GRootCertificateInstalled();

    /// \brief indicates if a manufacturer root certificate is installed
    bool isManufacturerRootCertificateInstalled();

    /// \brief indicates if a csms leaf certificate is installed
    bool isCsmsLeafCertificateInstalled();

    /// \brief Gets a list of the CertificateHashDataType for the given \p type
    std::optional<std::vector<CertificateHashDataChain>>
    getRootCertificateHashData(std::optional<std::vector<CertificateType>> certificateTypes);

    /// \brief Deletes a root certificate that matches the given \p certificateHashData
    DeleteCertificateResult deleteRootCertificate(CertificateHashDataType certificateHashData,
                                                  int32_t security_profile);

    /// \brief Installs the the given \p rootCertificate
    InstallCertificateResult installRootCertificate(const std::string& rootCertificate,
                                                    const CertificateType& certificateType,
                                                    std::optional<int32_t> certificateStoreMaxLength,
                                                    std::optional<bool> additionalRootCertificateCheck);

    /// \brief Get the leaf certificate of the given \p certificate_signing_use
    std::shared_ptr<X509Certificate> getLeafCertificate(const CertificateSigningUseEnum& certificate_signing_use);

    /// \brief Get the leaf private key of the given \p certificate_signing_use
    boost::filesystem::path getLeafPrivateKeyPath(const CertificateSigningUseEnum& certificate_signing_use);

    /// \brief Removes a fallback central system root certificate if present
    void removeCentralSystemFallbackCa();

    /// \brief Renames CSMS fallback CA to default CSMS and removes the previously installed CSMS CA
    void useCsmsFallbackRoot();

    /// \brief returns the time in milliseconds when the given certificate is valid.
    /// Result is <0 if it is already valid
    int validIn(const std::string& certificate);

    /// \brief Get the Days Until Leaf Certificate Expires
    int getDaysUntilLeafExpires(const CertificateSigningUseEnum& certificate_signing_use);

    /// \brief Writes the given \p ocspResponse for the respective \p ocspRequestData to the filesystem
    void updateOcspCache(const OCSPRequestData& ocspRequestData, const std::string& ocspResponse);

    /// \brief Returns a vector of OCSPReqestData for the CSO CA certificates
    std::vector<OCSPRequestData> getOCSPRequestData();
};

} // namespace ocpp

#endif // OCPP_COMMON_PKI_HANDLER
