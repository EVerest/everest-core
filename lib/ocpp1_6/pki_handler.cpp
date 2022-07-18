// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <fstream>
#include <streambuf>

#include <ocpp1_6/pki_handler.hpp>

namespace ocpp1_6 {

X509Certificate::X509Certificate(boost::filesystem::path path, X509* x509, std::string str) {
    this->path = path;
    this->x509 = x509;
    this->str = str;
}

X509Certificate::X509Certificate(const boost::filesystem::path path, X509* x509, const std::string str,
                                 const CertificateType type) {
    this->path = path;
    this->x509 = x509;
    this->str = str;
    this->type = type;
}

X509Certificate::~X509Certificate() {
    X509_free(this->x509);
}

std::shared_ptr<X509Certificate> loadFromFile(const boost::filesystem::path& path) {

    std::ifstream t(path.c_str());
    std::string file_str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    BIO_ptr b(BIO_new(BIO_s_mem()), ::BIO_free);
    BIO_puts(b.get(), file_str.c_str());
    X509* x509 = PEM_read_bio_X509(b.get(), NULL, NULL, NULL);
    std::shared_ptr<X509Certificate> cert = std::make_shared<X509Certificate>(path, x509, file_str);
    cert->validIn = X509_cmp_current_time(X509_get_notBefore(cert->x509));
    cert->validTo = X509_cmp_current_time(X509_get_notAfter(cert->x509));

    return cert;
}

std::shared_ptr<X509Certificate> loadFromString(const std::string& str) {
    BIO_ptr b(BIO_new(BIO_s_mem()), ::BIO_free);
    BIO_puts(b.get(), str.c_str());
    X509* x509 = PEM_read_bio_X509(b.get(), NULL, NULL, NULL);

    if (x509 == NULL) {
        EVLOG_error << "Could not parse str to X509";
    }

    std::shared_ptr<X509Certificate> cert = std::make_shared<X509Certificate>();
    cert->str = str;
    cert->x509 = x509;
    return cert;
}

bool X509Certificate::write() {
    std::ofstream fs(this->path.string());
    fs << this->str << std::endl;
    fs.close();
    return true;
}

PkiHandler::PkiHandler(std::string maindir) : maindir(boost::filesystem::path(maindir)) {
}

CertificateVerificationResult PkiHandler::verifyChargepointCertificate(const std::string& certificateChain,
                                                                       const std::string& chargeBoxSerialNumber) {

    if (!this->isCentralSystemRootCertificateInstalled()) {
        EVLOG_warning << "Could not verify certificate because no Central System Root Certificate is installed";
        return CertificateVerificationResult::NoRootCertificateInstalled;
    }

    std::shared_ptr<X509Certificate> rootCert = loadFromFile(this->getCertsPath() / CS_ROOT_CA_FILE);
    std::shared_ptr<X509Certificate> certChain = loadFromString(certificateChain);

    if (certChain->x509 == NULL || !this->verifyCertificateChain(rootCert, certChain)) {
        return CertificateVerificationResult::InvalidCertificateChain;
    }

    // verifies the signature of certificate x using public key of rootCA
    if (X509_verify(certChain->x509, X509_get_pubkey(rootCert->x509)) == 0) {
        EVLOG_warning << "Could not verify signature of certificate";
        return CertificateVerificationResult::InvalidSignature;
    }

    // expiration check
    if (X509_cmp_current_time(X509_get_notBefore(certChain->x509)) >= 0) {
        EVLOG_debug << "Certificate is not yet valid.";
    }

    if (X509_cmp_current_time(X509_get_notAfter(certChain->x509)) <= 0) {
        EVLOG_warning << "Certificate has expired." << std::endl;
        return CertificateVerificationResult::Expired;
    }

    if (X509_check_host(certChain->x509, chargeBoxSerialNumber.c_str(), chargeBoxSerialNumber.length(), 0, NULL) == 0) {
        EVLOG_warning << "Subject field CN of certificate is not equal to ChargeBoxSerialNumber: "
                      << chargeBoxSerialNumber;
        return CertificateVerificationResult::InvalidCommonName;
    }

    return CertificateVerificationResult::Valid;
}

void PkiHandler::writeClientCertificate(const std::string& certificate) {
    std::shared_ptr<X509Certificate> cert = loadFromString(certificate);
    std::string newPath =
        CLIENT_SIDE_CERTIFICATE_FILE.string().substr(0, CLIENT_SIDE_CERTIFICATE_FILE.string().find_last_of(".")) +
        DateTime().to_rfc3339() + ".pem";
    cert->path = this->getFile(newPath);
    cert->write();
}

std::string PkiHandler::generateCsr(const char* szCountry, const char* szProvince, const char* szCity,
                                    const char* szOrganization, const char* szCommon) {
    int rc;
    RSA* r = NULL;
    BN_ptr bn(BN_new(), ::BN_free);

    int nVersion = 0;
    int bits = 2048;
    unsigned long e = RSA_F4;

    // csr req
    X509_REQ* x509_req = X509_REQ_new();
    EVP_KEY_ptr pKey(EVP_PKEY_new(), ::EVP_PKEY_free);
    BIO_ptr out(BIO_new_file(this->getFile(CERTIFICATE_SIGNING_REQUEST_FILE).c_str(), "w"), ::BIO_free);
    BIO_ptr pbkey(BIO_new_file(this->getFile(PUBLIC_KEY_FILE).c_str(), "w"), ::BIO_free);
    BIO_ptr prkey(BIO_new_file(this->getFile(PRIVATE_KEY_FILE).c_str(), "w"), ::BIO_free);
    BIO_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);

    // generate rsa key
    rc = BN_set_word(bn.get(), e);
    assert(rc == 1);
    r = RSA_new();
    RSA_generate_key_ex(r, bits, bn.get(), NULL);
    assert(rc == 1);

    // write keys to files
    rc = PEM_write_bio_RSAPublicKey(pbkey.get(), r);
    assert(rc == 1);
    rc = PEM_write_bio_RSAPrivateKey(prkey.get(), r, NULL, NULL, 0, NULL, NULL);
    assert(rc == 1);

    // set version of x509 req
    X509_REQ_set_version(x509_req, nVersion);
    assert(rc == 1);

    // set subject of x509 req
    X509_NAME_ptr x509_name_ptr(X509_REQ_get_subject_name(x509_req), ::X509_NAME_free);
    assert(rc == 1);
    X509_NAME_add_entry_by_txt(x509_name_ptr.get(), "C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
    assert(rc == 1);
    X509_NAME_add_entry_by_txt(x509_name_ptr.get(), "ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
    assert(rc == 1);
    X509_NAME_add_entry_by_txt(x509_name_ptr.get(), "L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
    assert(rc == 1);
    X509_NAME_add_entry_by_txt(x509_name_ptr.get(), "O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
    assert(rc == 1);
    X509_NAME_add_entry_by_txt(x509_name_ptr.get(), "CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);
    assert(rc == 1);

    // 5. set public key of x509 req
    EVP_PKEY_assign_RSA(pKey.get(), r);
    r = NULL;
    X509_REQ_set_pubkey(x509_req, pKey.get());
    assert(rc == 1);

    // 6. set sign key of x509 req
    X509_REQ_sign(x509_req, pKey.get(), EVP_sha256());
    assert(rc == 1);

    // 7. write csr to file
    PEM_write_bio_X509_REQ(out.get(), x509_req);
    assert(rc == 1);

    // 8. read csr from file
    rc = PEM_write_bio_X509_REQ(bio.get(), x509_req);
    assert(rc == 1);
    BUF_MEM* mem = NULL;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string csr(mem->data, mem->length);

    EVLOG_debug << csr;

    return csr;
}

bool PkiHandler::isCentralSystemRootCertificateInstalled() {
    return boost::filesystem::exists(this->getFile(CS_ROOT_CA_FILE));
}

bool PkiHandler::isManufacturerRootCertificateInstalled() {
    return boost::filesystem::exists(this->getFile(MF_ROOT_CA_FILE));
}

bool PkiHandler::isClientCertificateInstalled() {
    return boost::filesystem::exists(this->getFile(CLIENT_SIDE_CERTIFICATE_FILE));
}

bool PkiHandler::isRootCertificateInstalled(CertificateUseEnumType type) {
    if (type == CertificateUseEnumType::CentralSystemRootCertificate) {
        return this->isCentralSystemRootCertificateInstalled();
    } else {
        return this->isManufacturerRootCertificateInstalled();
    }
}

bool PkiHandler::verifyFirmwareCertificate(const std::string& firmwareCertificate) {
    std::shared_ptr<X509Certificate> cert = loadFromString(firmwareCertificate);
    if (cert->x509 == NULL) {
        return false;
    }
    if (!this->isManufacturerRootCertificateInstalled()) {
        EVLOG_warning << "No manufacturer root certificate installed";
        return false;
    }

    std::shared_ptr<X509Certificate> mfRootCA = loadFromFile(this->getFile(MF_ROOT_CA_FILE));

    X509_STORE_ptr store_ptr(X509_STORE_new(), ::X509_STORE_free);
    X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new(), ::X509_STORE_CTX_free);

    X509_STORE_add_cert(store_ptr.get(), mfRootCA->x509);
    X509_STORE_CTX_init(store_ctx_ptr.get(), store_ptr.get(), cert->x509, NULL);

    int rc = 0;

    // verifies the certificate chain based on ctx
    rc = X509_verify_cert(store_ctx_ptr.get());
    if (rc == 0) {
        // TODO(piet): trigger InvalidChargepointCertificate security event
        int ec = X509_STORE_CTX_get_error(store_ctx_ptr.get());
        EVLOG_error << "Could not verify certificate chain. Error code: " << ec
                    << " human readable: " << X509_verify_cert_error_string(ec);
        return false;
    } else {
        EVLOG_debug << "Verified certificate chain of firmware certificate";
        return true;
    }
}

boost::optional<std::vector<CertificateHashDataType>>
PkiHandler::getRootCertificateHashData(CertificateUseEnumType type) {
    boost::optional<std::vector<CertificateHashDataType>> certificateHashDataOpt = boost::none;
    std::vector<CertificateHashDataType> certificate_hash_data_vec;

    std::vector<std::shared_ptr<X509Certificate>> caCertificates = this->getCaCertificates(type);

    for (std::shared_ptr<X509Certificate> cert : caCertificates) {
        CertificateHashDataType certificateHashData;
        std::string issuerNameHash = this->getIssuerNameHash(cert);
        std::string issuerKeyHash = this->getIssuerKeyHash(cert);
        std::string serial = this->getSerialNumber(cert);
        certificateHashData.hashAlgorithm = HashAlgorithmEnumType::SHA256;
        certificateHashData.issuerNameHash = issuerNameHash;
        certificateHashData.issuerKeyHash = issuerKeyHash;
        certificateHashData.serialNumber = serial;
        certificate_hash_data_vec.push_back(certificateHashData);
    }

    if (!caCertificates.empty()) {
        certificateHashDataOpt.emplace(certificate_hash_data_vec);
    }

    return certificateHashDataOpt;
}

DeleteCertificateStatusEnumType PkiHandler::deleteRootCertificate(CertificateHashDataType certificate_hash_data,
                                                                  int32_t security_profile) {
    std::vector<std::shared_ptr<X509Certificate>> caCertificates = this->getCaCertificates();
    DeleteCertificateStatusEnumType status = DeleteCertificateStatusEnumType::NotFound;

    for (std::shared_ptr<X509Certificate> cert : caCertificates) {
        std::string issuerNameHash = this->getIssuerNameHash(cert);
        std::string issuerKeyHash = this->getIssuerKeyHash(cert);
        std::string serial = this->getSerialNumber(cert);
        if (issuerNameHash == certificate_hash_data.issuerNameHash.get() &&
            issuerKeyHash == certificate_hash_data.issuerKeyHash.get() &&
            serial == certificate_hash_data.serialNumber.get()) {
            // dont delete if only one root ca is installed and tls is in use
            if (cert->type == CertificateType::CentralSystemRootCertificate &&
                this->getCaCertificates(CertificateUseEnumType::CentralSystemRootCertificate).size() == 1 &&
                security_profile >= 2) {
                return DeleteCertificateStatusEnumType::Failed;
            }
        }
        bool success = boost::filesystem::remove(cert->path);
        if (success) {
            status = DeleteCertificateStatusEnumType::Accepted;
        } else {
            status = DeleteCertificateStatusEnumType::Failed;
        }
    }
    return status;
}

InstallCertificateResult PkiHandler::installRootCertificate(InstallCertificateRequest msg,
                                                            boost::optional<int32_t> certificateStoreMaxLength,
                                                            boost::optional<bool> additionalRootCertificateCheck) {

    InstallCertificateResult installCertificateResult = InstallCertificateResult::Valid;

    if (certificateStoreMaxLength != boost::none &&
        this->getCaCertificates().size() >= size_t(certificateStoreMaxLength.get())) {
        return InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    }

    std::shared_ptr<X509Certificate> cert = loadFromString(msg.certificate.get());

    if (cert->x509 == NULL) {
        installCertificateResult = InstallCertificateResult::InvalidFormat;
    } else if (this->isRootCertificateInstalled(msg.certificateType) && additionalRootCertificateCheck) {
        std::shared_ptr<X509Certificate> root_cert = this->getRootCertificate(msg.certificateType);
        if (!this->verifySignature(root_cert, cert)) {
            installCertificateResult = InstallCertificateResult::InvalidSignature;
        }
        if (!this->verifyCertificateChain(root_cert, cert)) {
            installCertificateResult = InstallCertificateResult::InvalidCertificateChain;
        }
    } else {
        if (msg.certificateType == CertificateUseEnumType::CentralSystemRootCertificate) {
            std::rename(this->getFile(CS_ROOT_CA_FILE).c_str(), (this->getFile(CS_ROOT_CA_FILE_BACKUP_FILE)).c_str());
            cert->path = this->getFile(CS_ROOT_CA_FILE);
        } else if (msg.certificateType == CertificateUseEnumType::ManufacturerRootCertificate) {
            cert->path = this->getFile(MF_ROOT_CA_FILE);
        }

        installCertificateResult = InstallCertificateResult::Ok;
    }

    if (installCertificateResult == InstallCertificateResult::Ok ||
        installCertificateResult == InstallCertificateResult::Valid) {
        if (!cert->write()) {
            installCertificateResult = InstallCertificateResult::WriteError;
            std::rename(this->getFile(CS_ROOT_CA_FILE).c_str(), (this->getFile(CS_ROOT_CA_FILE_BACKUP_FILE)).c_str());
        }
    }

    return installCertificateResult;
}

boost::filesystem::path PkiHandler::getCertsPath() {
    return this->maindir / "certs";
}

boost::filesystem::path PkiHandler::getFile(boost::filesystem::path file_name) {
    return this->getCertsPath() / file_name;
}

std::shared_ptr<X509Certificate> PkiHandler::getClientCertificate() {
    std::shared_ptr<X509Certificate> cert = nullptr;
    int validIn = INT_MIN;
    for (const auto& dirEntry : boost::filesystem::recursive_directory_iterator(this->getCertsPath())) {
        if (dirEntry.path().string().find(CLIENT_SIDE_CERTIFICATE_FILE.c_str()) != std::string::npos) {
            std::shared_ptr<X509Certificate> c = loadFromFile(dirEntry.path());
            if (c->validIn < 0 && c->validIn > validIn) {
                cert = c;
            }
        }
    }
    return cert;
}

void PkiHandler::removeCentralSystemFallbackCa() {
    std::remove(this->getFile(CS_ROOT_CA_FILE_BACKUP_FILE).c_str());
}

int PkiHandler::validIn(const std::string& certificate) {
    std::shared_ptr<X509Certificate> cert = loadFromString(certificate);
    return X509_cmp_current_time(X509_get_notBefore(cert->x509));
}

int PkiHandler::getDaysUntilClientCertificateExpires() {
    std::shared_ptr<X509Certificate> cert = loadFromFile(this->getFile(CLIENT_SIDE_CERTIFICATE_FILE));
    const ASN1_TIME* expires = X509_get0_notAfter(cert->x509);
    int days, seconds;
    ASN1_TIME_diff(&days, &seconds, NULL, expires);
    return days;
}

bool PkiHandler::verifyCertificateChain(std::shared_ptr<X509Certificate> rootCA,
                                        std::shared_ptr<X509Certificate> cert) {
    X509_STORE_ptr store_ptr(X509_STORE_new(), ::X509_STORE_free);
    X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new(), ::X509_STORE_CTX_free);

    X509_STORE_add_cert(store_ptr.get(), rootCA->x509);
    X509_STORE_CTX_init(store_ctx_ptr.get(), store_ptr.get(), cert->x509, NULL);

    // verifies the certificate chain based on ctx
    if (X509_verify_cert(store_ctx_ptr.get()) == 0) {
        // TODO(piet): trigger InvalidChargepointCertificate security event
        int ec = X509_STORE_CTX_get_error(store_ctx_ptr.get());
        EVLOG_error << "Could not verify certificate chain. Error code: " << ec
                    << " reason: " << X509_verify_cert_error_string(ec);
        return false;
    } else {
        return true;
    }
}

bool PkiHandler::verifySignature(std::shared_ptr<X509Certificate> rootCA, std::shared_ptr<X509Certificate> newCA) {
    int rc = X509_verify(newCA->x509, X509_get_pubkey(rootCA->x509));
    if (rc == 0) {
        long ec = ERR_get_error();
        EVLOG_warning << "Could not verify signature of certificate: " << ERR_error_string(ec, NULL);
        return false;
    } else {
        EVLOG_debug << "Verified signature of certificate";
        return true;
    }
}

std::shared_ptr<X509Certificate> PkiHandler::getRootCertificate(CertificateUseEnumType type) {

    if (type == CertificateUseEnumType::CentralSystemRootCertificate) {
        return loadFromFile(this->getFile(CS_ROOT_CA_FILE));
    } else {
        return loadFromFile(this->getFile(MF_ROOT_CA_FILE));
    }
}

std::vector<std::shared_ptr<X509Certificate>> PkiHandler::getCaCertificates(CertificateUseEnumType type) {
    // FIXME(piet) iterate over directory and collect all ca files depending on the type
    std::vector<std::shared_ptr<X509Certificate>> caCertificates;
    if (type == CertificateUseEnumType::CentralSystemRootCertificate &&
        this->isCentralSystemRootCertificateInstalled()) {
        caCertificates.push_back(loadFromFile(this->getCertsPath() / CS_ROOT_CA_FILE));
    } else if (type == CertificateUseEnumType::ManufacturerRootCertificate &&
               this->isManufacturerRootCertificateInstalled()) {
        caCertificates.push_back(loadFromFile(this->getCertsPath() / MF_ROOT_CA_FILE));
    }
    return caCertificates;
}

std::vector<std::shared_ptr<X509Certificate>> PkiHandler::getCaCertificates() {
    // FIXME(piet) make this dynamic
    std::vector<std::shared_ptr<X509Certificate>> caCertificates;
    if (this->isCentralSystemRootCertificateInstalled()) {
        caCertificates.push_back(loadFromFile(this->getCertsPath() / CS_ROOT_CA_FILE));
    }
    if (this->isManufacturerRootCertificateInstalled()) {
        caCertificates.push_back(loadFromFile(this->getCertsPath() / MF_ROOT_CA_FILE));
    }
    return caCertificates;
}

std::string PkiHandler::getIssuerNameHash(std::shared_ptr<X509Certificate> cert) {
    unsigned char md[SHA256_DIGEST_LENGTH];
    X509_NAME* name = X509_get_issuer_name(cert->x509);
    X509_NAME_digest(name, EVP_sha256(), md, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)md[i];
    std::string str = ss.str();
    return str;
}

std::string PkiHandler::getSerialNumber(std::shared_ptr<X509Certificate> cert) {
    ASN1_INTEGER* serial = X509_get_serialNumber(cert->x509);
    BIGNUM* bnser = ASN1_INTEGER_to_BN(serial, NULL);
    char* hex = BN_bn2hex(bnser);
    std::string str(hex);
    str.erase(0, std::min(str.find_first_not_of('0'), str.size() - 1));
    return str;
}

std::string PkiHandler::getIssuerKeyHash(std::shared_ptr<X509Certificate> cert) {
    unsigned char tmphash[SHA256_DIGEST_LENGTH];
    X509_pubkey_digest(cert->x509, EVP_sha256(), tmphash, NULL);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)tmphash[i];
    std::string str = ss.str();
    return str;
}

} // namespace ocpp1_6
