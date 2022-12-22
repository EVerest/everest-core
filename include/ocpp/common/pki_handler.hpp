// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_PKI_HANDLER
#define OCPP_COMMON_PKI_HANDLER

#include <boost/filesystem.hpp>
#include <memory>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <string>

#include <ocpp/common/types.hpp>

namespace ocpp {

using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using BIO_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
using X509_NAME_ptr = std::unique_ptr<X509_NAME, decltype(&::X509_NAME_free)>;
using X509_STORE_ptr = std::unique_ptr<X509_STORE, decltype(&::X509_STORE_free)>;
using X509_STORE_CTX_ptr = std::unique_ptr<X509_STORE_CTX, decltype(&::X509_STORE_CTX_free)>;
using ASN1_TIME_ptr = std::unique_ptr<ASN1_TIME, decltype(&ASN1_STRING_free)>;

// filenames of all certificates used
const boost::filesystem::path CLIENT_SIDE_CERTIFICATE_FILE("CP_RSA.pem");
const boost::filesystem::path CERTIFICATE_SIGNING_REQUEST_FILE("CP_CSR.pem");
const boost::filesystem::path CS_ROOT_CA_FILE("CSMS_RootCA_RSA.pem");
const boost::filesystem::path CS_ROOT_CA_FILE_BACKUP_FILE("CSMS_RootCA_RSA_backup.pem");
const boost::filesystem::path MF_ROOT_CA_FILE("MF_RootCA_RSA.pem");
const boost::filesystem::path PUBLIC_KEY_FILE("CP_PUBLIC_KEY_RSA.pem");
const boost::filesystem::path PRIVATE_KEY_FILE("CP_PRIVATE_KEY_RSA.pem");

struct X509Certificate {
    boost::filesystem::path path;
    X509* x509;
    std::string str;
    CertificateType type;
    int validIn; // seconds. If > 0 cert is not valid yet
    int validTo; // seconds. If < 0 cert has expired

    X509Certificate(){};
    X509Certificate(boost::filesystem::path path, X509* x509, std::string str);
    X509Certificate(boost::filesystem::path path, X509* x509, std::string str, CertificateType type);
    ~X509Certificate();

    /// \brief writes the X509 certificate to the path set
    bool write();
    /// \brief writes the X509 certificate to the given \p path
    bool write(const boost::filesystem::path& path);
};
/// \brief loads a X509Certificate from the given \p pat
std::shared_ptr<X509Certificate> loadFromFile(const boost::filesystem::path& path);
/// \brief loads a X509Certificate from the given \p st
std::shared_ptr<X509Certificate> loadFromString(std::string& str);
/// \brief reads a file from the given \p path to a strin
std::string readFileToString(const boost::filesystem::path& path);
class PkiHandler {

private:
    boost::filesystem::path maindir;
    bool useRootCaFallback;

    std::string getIssuerNameHash(std::shared_ptr<X509Certificate> cert);
    std::string getSerialNumber(std::shared_ptr<X509Certificate> cert);
    std::string getIssuerKeyHash(std::shared_ptr<X509Certificate> cert);
    std::vector<std::shared_ptr<X509Certificate>> getCaCertificates(CertificateType type);
    std::vector<std::shared_ptr<X509Certificate>> getCaCertificates();
    bool verifyCertificateChain(std::shared_ptr<X509Certificate> rootCA, std::shared_ptr<X509Certificate> cert);
    bool verifySignature(std::shared_ptr<X509Certificate> rootCA, std::shared_ptr<X509Certificate> newCA);
    std::shared_ptr<X509Certificate> getRootCertificate(CertificateType type);

public:
    explicit PkiHandler(const std::string& maindir);
    /// \brief Verifies the given \p certificateChain and the \p charge_box_serial_number.
    /// This method verifies the certificate chain, the signature, and the period when the certificate is valid
    CertificateVerificationResult verifyChargepointCertificate(const std::string& certificateChain,
                                                               const std::string& charge_box_serial_number);
    /// \brief writes the given \p certificate to the client certificate file located in the certs directory
    void writeClientCertificate(const std::string& certificate);

    /// \brief generates a certifcate signing request from the given parameters
    std::string generateCsr(const char* szCountry, const char* szProvince, const char* szCity,
                            const char* szOrganization, const char* szCommon);

    /// \brief indicates if a central system root certificate is installed
    bool isCentralSystemRootCertificateInstalled();

    /// \brief indicates if a manufacturer root certificate is installed
    bool isManufacturerRootCertificateInstalled();

    /// \brief indicates if a client certificate is installed
    bool isClientCertificateInstalled();

    /// \brief indicates if a root certificate of the given \p type is installed
    bool isRootCertificateInstalled(CertificateType type);

    /// \brief Verifies the given \p firmwareCertificate .
    /// This method verifies the certificate chain
    bool verifyFirmwareCertificate(const std::string& firmwareCertificate);

    /// \brief Gets a list of the CertificateHashDataType for the given \p type
    boost::optional<std::vector<CertificateHashDataType>> getRootCertificateHashData(CertificateType type);

    /// \brief Deletes a root certificate that matches the given \p certificate_hash_data
    DeleteCertificateResult deleteRootCertificate(CertificateHashDataType certificate_hash_data,
                                                  int32_t security_profile);

    /// \brief Installs the the given \p root_certificate
    InstallCertificateResult installRootCertificate(const std::string& root_certificate,
                                                    const CertificateType& certificate_type,
                                                    boost::optional<int32_t> certificateStoreMaxLength,
                                                    boost::optional<bool> additionalRootCertificateCheck);

    /// \brief Get the Certs Path object
    boost::filesystem::path getCertsPath();

    /// \brief Get the File object
    boost::filesystem::path getFile(boost::filesystem::path fileName);

    /// \brief Get the Client Certificate object
    std::shared_ptr<X509Certificate> getClientCertificate();

    /// \brief Removes a fallback central system root certificate if present
    void removeCentralSystemFallbackCa();

    /// \brief returns the time in milliseconds when the given certificate is valid.
    /// Result is <0 if it is already valid
    int validIn(const std::string& certificate);

    /// \brief Get the Days Until Client Certificate Expires
    int getDaysUntilClientCertificateExpires();
};

} // namespace ocpp

#endif // OCPP_COMMON_PKI_HANDLER
