// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <fstream>
#include <regex>
#include <streambuf>

#include <ocpp/common/pki_handler.hpp>

namespace ocpp {

X509Certificate::X509Certificate(std::filesystem::path path, X509* x509, std::string str) {
    this->path = path;
    this->x509 = x509;
    this->str = str;
}

X509Certificate::~X509Certificate() {
    X509_free(this->x509);
}

bool X509Certificate::write() {
    std::ofstream fs(this->path.string());
    fs << this->str << std::endl;
    fs.close();
    return true;
}

std::string X509Certificate::getCommonName() {
    X509_NAME* subject = X509_get_subject_name(this->x509);
    int nid = OBJ_txt2nid("CN");
    int index = X509_NAME_get_index_by_NID(subject, nid, -1);
    X509_NAME_ENTRY* entry = X509_NAME_get_entry(subject, index);
    ASN1_STRING* cnAsn1 = X509_NAME_ENTRY_get_data(entry);
    const unsigned char* cnStr = ASN1_STRING_get0_data(cnAsn1);
    std::string commonName(reinterpret_cast<const char*>(cnStr), ASN1_STRING_length(cnAsn1));
    return commonName;
}

std::string X509Certificate::getIssuerNameHash() {
    unsigned char md[SHA256_DIGEST_LENGTH];
    X509_NAME* name = X509_get_issuer_name(this->x509);
    X509_NAME_digest(name, EVP_sha256(), md, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)md[i];
    }
    std::string str = ss.str();
    return str;
}

std::string X509Certificate::getSerialNumber() {
    ASN1_INTEGER* serial = X509_get_serialNumber(this->x509);
    BIGNUM* bnser = ASN1_INTEGER_to_BN(serial, NULL);
    char* hex = BN_bn2hex(bnser);
    std::string str(hex);
    str.erase(0, std::min(str.find_first_not_of('0'), str.size() - 1));
    return str;
}

std::string X509Certificate::getIssuerKeyHash() {
    unsigned char tmphash[SHA256_DIGEST_LENGTH];
    X509_pubkey_digest(this->x509, EVP_sha256(), tmphash, NULL);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)tmphash[i];
    }
    std::string str = ss.str();
    return str;
}

std::string X509Certificate::getResponderURL() {
    const auto ocsp = X509_get1_ocsp(this->x509);
    std::string responderUrl;
    for (int i = 0; i < sk_OPENSSL_STRING_num(ocsp); i++) {
        responderUrl.append(sk_OPENSSL_STRING_value(ocsp, i));
    }

    if (responderUrl.empty()) {
        EVLOG_warning << "Could not retrieve OCSP Responder URL from certificate";
    }

    return responderUrl;
}

OCSPRequestData X509Certificate::getOCSPRequestData() {
    OCSPRequestData requestData;
    requestData.hashAlgorithm = HashAlgorithmEnumType::SHA256;
    requestData.issuerKeyHash = this->getIssuerKeyHash();
    requestData.issuerNameHash = this->getIssuerNameHash();
    requestData.serialNumber = this->getSerialNumber();
    requestData.responderUrl = this->getResponderURL();
    return requestData;
}

PkiEnum getPkiEnumFromCertificateType(const CertificateType& type) {
    switch (type) {
    case CertificateType::CentralSystemRootCertificate:
        return PkiEnum::CSMS;
    case CertificateType::ManufacturerRootCertificate:
        return PkiEnum::MF;
    case CertificateType::MORootCertificate:
        return PkiEnum::MO;
    case CertificateType::V2GCertificateChain:
        return PkiEnum::CSO;
    case CertificateType::V2GRootCertificate:
        return PkiEnum::V2G;
    case CertificateType::CSMSRootCertificate:
        return PkiEnum::CSMS;
    case CertificateType::CertificateProvisioningServiceRootCertificate:
        return PkiEnum::CPS;
    default:
        EVLOG_warning << "Conversion of CertificateType: " << conversions::certificate_type_to_string(type)
                      << " to PkiEnum was not successful";
        return PkiEnum::V2G;
    }
}

CertificateType getRootCertificateTypeFromPki(const PkiEnum& pki) {
    switch (pki) {
    case PkiEnum::CPS:
        return CertificateType::CertificateProvisioningServiceRootCertificate;
    case PkiEnum::CSMS:
        return CertificateType::CentralSystemRootCertificate;
    case PkiEnum::CSO:
        return CertificateType::CentralSystemRootCertificate;
    case PkiEnum::MF:
        return CertificateType::ManufacturerRootCertificate;
    case PkiEnum::MO:
        return CertificateType::MORootCertificate;
    case PkiEnum::OEM:
        return CertificateType::OEMRootCertificate;
    case PkiEnum::V2G:
        return CertificateType::V2GRootCertificate;
    default:
        return CertificateType::V2GRootCertificate;
    }
}

std::shared_ptr<X509Certificate> loadFromFile(const std::filesystem::path& path) {
    try {
        X509* x509;
        std::string fileStr;

        if (path.extension().string() == ".der") {
            std::ifstream t(path.c_str(), std::ios::binary);
            fileStr = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            t.close();

            const char* certDataPtr = fileStr.data();
            const int certDataLen = fileStr.size();

            // Convert certificate data to X509 certificate
            x509 = d2i_X509(NULL, (const unsigned char**)&certDataPtr, certDataLen);
            if (x509 == NULL) {
                EVLOG_warning << "Error loading X509 certificate from " << path.string();
                return nullptr;
            }
        } else {
            std::ifstream t(path.c_str());
            fileStr = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            t.close();
            BIO_ptr b(BIO_new(BIO_s_mem()), ::BIO_free);
            BIO_puts(b.get(), fileStr.c_str());
            x509 = PEM_read_bio_X509(b.get(), NULL, NULL, NULL);
            if (x509 == NULL) {
                EVLOG_error << "Error loading X509 certificate from " << path.string();
                return nullptr;
            }
        }
        std::shared_ptr<X509Certificate> cert = std::make_shared<X509Certificate>(path, x509, fileStr);
        cert->validIn = X509_cmp_current_time(X509_get_notBefore(cert->x509));
        cert->validTo = X509_cmp_current_time(X509_get_notAfter(cert->x509));

        return cert;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown error occured while loading certificate from file: " << e.what();
        return nullptr;
    }
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

PkiHandler::PkiHandler(const std::filesystem::path& certsPath, const bool multipleCsmsCaNotAllowed) :
    certsPath(certsPath) {

    this->caPath = this->certsPath / "ca";
    this->caCsmsPath = this->caPath / "csms";
    this->caCsoPath = this->caPath / "cso";
    this->caCpsPath = this->caPath / "cps";
    this->caMoPath = this->caPath / "mo";
    this->caMfPath = this->caPath / "mf";
    this->caOemPath = this->caPath / "oem";
    this->caV2gPath = this->caPath / "v2g";

    this->clientCsmsPath = this->certsPath / "client/csms";
    this->clientCsoPath = this->certsPath / "client/cso";
    if (multipleCsmsCaNotAllowed) {
        if (this->getNumberOfCsmsCaCertificates() > 1) {
            EVLOG_error << "Multiple CSMS CA certificates installed while additionalRootCertificateCheck is true. This "
                           "is not allowed";
            EVLOG_AND_THROW(std::runtime_error("Multiple CSMS CA not allowed!"));
        } else {
            const auto oldCsmsCaPath = this->getCsmsCaFilePath();
            if (oldCsmsCaPath.has_value()) {
                std::rename(oldCsmsCaPath.value().c_str(), (this->caCsmsPath / CSMS_ROOT_CA).c_str());
            } else {
                EVLOG_AND_THROW(std::runtime_error("Could not find CSMS CA Root"));
            }
        }
    }

    std::vector<PkiEnum> pkis = {PkiEnum::CSO, PkiEnum::CSMS, PkiEnum::MF, PkiEnum::MO, PkiEnum::OEM, PkiEnum::V2G};
    for (const auto& pki : pkis) {
        const auto caPath = this->getCaPath(pki);
        if (std::filesystem::exists(caPath)) {
            this->execOpenSSLRehash(caPath);
        } else {
            EVLOG_warning << "Certificate directory does not exist: " << caPath;
        }
    }
}

std::filesystem::path PkiHandler::getCaCsmsPath() {
    return this->caCsmsPath;
}

int PkiHandler::getNumberOfCsmsCaCertificates() {
    int fileCounter = 0;
    for (const auto& entry : std::filesystem::directory_iterator(this->caCsmsPath)) {
        if (entry.path().extension().string() == ".pem") {
            fileCounter++;
        }
    }
    return fileCounter;
}

CertificateVerificationResult PkiHandler::verifyChargepointCertificate(const std::string& certificateChain,
                                                                       const std::string& chargeBoxSerialNumber) {

    if (!this->isCentralSystemRootCertificateInstalled()) {
        EVLOG_warning << "Could not verify certificate because no Central System Root Certificate is installed";
        return CertificateVerificationResult::NoRootCertificateInstalled;
    }

    const auto certificates = this->getCertificatesFromChain(certificateChain);

    if (!certificates.empty()) {
        return this->verifyCertificate(PkiEnum::CSMS, certificates, chargeBoxSerialNumber);
    } else {
        return CertificateVerificationResult::InvalidCertificateChain;
    }
}

CertificateVerificationResult
PkiHandler::verifyV2GChargingStationCertificate(const std::string& certificateChain,
                                                const std::string& chargeBoxSerialNumber) {
    if (!this->isV2GRootCertificateInstalled()) {
        EVLOG_warning << "Could not verify certificate because no V2GRootCertificate is installed";
        return CertificateVerificationResult::NoRootCertificateInstalled;
    }

    const auto certificates = this->getCertificatesFromChain(certificateChain);

    if (!certificates.empty()) {
        return this->verifyCertificate(PkiEnum::CSO, certificates, chargeBoxSerialNumber);
    } else {
        return CertificateVerificationResult::InvalidCertificateChain;
    }
}

CertificateVerificationResult PkiHandler::verifyFirmwareCertificate(const std::string& firmwareCertificate) {
    if (!this->isManufacturerRootCertificateInstalled()) {
        EVLOG_warning << "Could not verify certificate because no MF root CA is installed";
        return CertificateVerificationResult::NoRootCertificateInstalled;
    }

    const auto certificates = this->getCertificatesFromChain(firmwareCertificate);

    if (!certificates.empty()) {
        return this->verifyCertificate(PkiEnum::MF, certificates);
    } else {
        return CertificateVerificationResult::InvalidCertificateChain;
    }
}

void PkiHandler::writeClientCertificate(const std::string& certificateChain,
                                        const CertificateSigningUseEnum& certificate_use) {

    const auto certificates = this->getCertificatesFromChain(certificateChain);

    if (certificates.empty()) {
        EVLOG_error << "Failed to write client certificate";
    } else {
        auto leafCert = certificates.at(0);
        std::string newPath;
        std::filesystem::path leafPath;

        if (certificate_use == CertificateSigningUseEnum::ChargingStationCertificate) {
            leafPath = this->clientCsmsPath / CSMS_LEAF;
        } else {
            leafPath = this->clientCsoPath / V2G_LEAF;
        }

        if (std::filesystem::exists(leafPath)) {
            const auto oldLeafPath =
                leafPath.parent_path() /
                (leafPath.filename().string().substr(0, leafPath.filename().string().find_last_of(".")) +
                 DateTime().to_rfc3339() + ".pem");
            std::rename(leafPath.c_str(), oldLeafPath.c_str());
        }
        leafCert->path = leafPath;
        leafCert->write();
    }
}

std::string PkiHandler::generateCsr(const CertificateSigningUseEnum& certificateType, const std::string& szCountry,
                                    const std::string& szOrganization, const std::string& szCommon) {
    int rc;
    int nVersion = 0;
    int bits = 256;

    // csr req
    X509_REQ_ptr x509ReqPtr(X509_REQ_new(), X509_REQ_free);
    EVP_PKEY_ptr evpKey(EVP_PKEY_new(), EVP_PKEY_free);
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    X509_NAME* x509Name = X509_REQ_get_subject_name(x509ReqPtr.get());

    std::filesystem::path csrFile;
    std::filesystem::path privateKeyFile;

    // FIXME(piet): This just overrides the private key and in case no valid certificate will be delivered by a future
    // CertificateSigned.req from CSMS for this CSR the private key is lost
    if (certificateType == CertificateSigningUseEnum::ChargingStationCertificate) {
        csrFile = this->clientCsmsPath / CSMS_CSR;
        privateKeyFile = this->clientCsmsPath / CSMS_LEAF_KEY;
        std::rename((this->clientCsmsPath / CSMS_LEAF_KEY).c_str(),
                    (this->clientCsmsPath / CSMS_LEAF_KEY_BACKUP).c_str());
    } else {
        // CertificateSigningUseEnum::CertificateSigningUseEnum::V2GCertificate
        csrFile = this->clientCsoPath / V2G_CSR;
        privateKeyFile = this->clientCsoPath / V2G_LEAF_KEY;
        std::rename((this->clientCsoPath / V2G_LEAF_KEY).c_str(), (this->clientCsoPath / V2G_LEAF_KEY_BACKUP).c_str());
    }

    BIO_ptr out(BIO_new_file(csrFile.c_str(), "w"), ::BIO_free);
    BIO_ptr prkey(BIO_new_file(privateKeyFile.c_str(), "w"), ::BIO_free);
    BIO_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);

    // generate ec key pair
    rc = EC_KEY_generate_key(ecKey);
    if (rc != 1) {
        EVLOG_error << "Failed to generate EC key pair";
    }

    rc = EVP_PKEY_assign_EC_KEY(evpKey.get(), ecKey);
    if (rc != 1) {
        EVLOG_error << "Failed to assign EC key to EVP key";
    }

    // write private key to file
    rc = PEM_write_bio_PrivateKey(prkey.get(), evpKey.get(), NULL, NULL, 0, NULL, NULL);
    if (rc != 1) {
        EVLOG_error << "Failed to write private key to file";
    }

    // set version of x509 req
    rc = X509_REQ_set_version(x509ReqPtr.get(), nVersion);
    if (rc != 1) {
        EVLOG_error << "Failed to set version of X509 request";
    }

    // set subject of x509 req
    rc = X509_NAME_add_entry_by_txt(x509Name, "C", MBSTRING_ASC,
                                    reinterpret_cast<const unsigned char*>(szCountry.c_str()), -1, -1, 0);
    if (rc != 1) {
        EVLOG_error << "Failed to add 'C' entry to X509 request";
    }

    rc = X509_NAME_add_entry_by_txt(x509Name, "O", MBSTRING_ASC,
                                    reinterpret_cast<const unsigned char*>(szOrganization.c_str()), -1, -1, 0);
    if (rc != 1) {
        EVLOG_error << "Failed to add 'O' entry to X509 request";
    }

    rc = X509_NAME_add_entry_by_txt(x509Name, "CN", MBSTRING_ASC,
                                    reinterpret_cast<const unsigned char*>(szCommon.c_str()), -1, -1, 0);
    if (rc != 1) {
        EVLOG_error << "Failed to add 'CN' entry to X509 request";
    }

    rc = X509_NAME_add_entry_by_txt(x509Name, "DC", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("CPO"), -1, -1,
                                    0);

    if (rc != 1) {
        EVLOG_error << "Failed to add 'DC' entry to X509 request";
    }

    // 5. set public key of x509 req
    rc = X509_REQ_set_pubkey(x509ReqPtr.get(), evpKey.get());
    if (rc != 1) {
        EVLOG_error << "Failed to set public key of X509 request";
    }

    STACK_OF(X509_EXTENSION)* extensions = sk_X509_EXTENSION_new_null();
    X509_EXTENSION* ext_key_usage = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, "digitalSignature, keyAgreement");
    X509_EXTENSION* ext_basic_constraints = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, "critical,CA:false");
    sk_X509_EXTENSION_push(extensions, ext_key_usage);
    sk_X509_EXTENSION_push(extensions, ext_basic_constraints);

    rc = X509_REQ_add_extensions(x509ReqPtr.get(), extensions);
    if (rc != 1) {
        EVLOG_error << "Failed to add extensions to X509 request";
    }

    // 6. set sign key of x509 req
    X509_REQ_sign(x509ReqPtr.get(), evpKey.get(), EVP_sha256());

    // 7. write csr to file
    rc = PEM_write_bio_X509_REQ(out.get(), x509ReqPtr.get());
    if (rc != 1) {
        EVLOG_error << "Failed to write X509 request to file";
    }

    // 8. read csr from file
    rc = PEM_write_bio_X509_REQ(bio.get(), x509ReqPtr.get());
    if (rc != 1) {
        EVLOG_error << "Failed to read X509 request from file";
    }

    BUF_MEM* mem = NULL;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string csr(mem->data, mem->length);

    EVLOG_debug << csr;

    return csr;
}

bool PkiHandler::isCentralSystemRootCertificateInstalled() {
    return !std::filesystem::is_empty(this->getCaPath(PkiEnum::CSMS));
}

bool PkiHandler::isV2GRootCertificateInstalled() {
    return !std::filesystem::is_empty(this->getCaPath(PkiEnum::V2G));
}

bool PkiHandler::isManufacturerRootCertificateInstalled() {
    return !std::filesystem::is_empty(this->getCaPath(PkiEnum::MF));
}

bool PkiHandler::isCsmsLeafCertificateInstalled() {
    return std::filesystem::exists(this->clientCsmsPath / CSMS_LEAF);
}

std::optional<std::vector<CertificateHashDataChain>>
PkiHandler::getRootCertificateHashData(std::optional<std::vector<CertificateType>> certificateTypes) {
    std::optional<std::vector<CertificateHashDataChain>> certificateHashDataChainOpt = std::nullopt;

    std::vector<CertificateHashDataChain> certificateHashDataChain;
    std::vector<std::shared_ptr<X509Certificate>> caCertificates;

    // set certificateTypes to all types if not set
    if (!certificateTypes.has_value()) {
        std::vector<CertificateType> types = {
            CertificateType::CentralSystemRootCertificate, CertificateType::CSMSRootCertificate,
            CertificateType::ManufacturerRootCertificate,  CertificateType::MORootCertificate,
            CertificateType::V2GRootCertificate,           CertificateType::V2GCertificateChain};
        certificateTypes.emplace(types);
    }

    // collect all required ca certificates
    for (const auto& certificateType : certificateTypes.value()) {
        auto pki = getPkiEnumFromCertificateType(certificateType);
        auto tempCaCertificates =
            this->getCaCertificates(getPkiEnumFromCertificateType(certificateType), certificateType);
        caCertificates.insert(caCertificates.end(), tempCaCertificates.begin(), tempCaCertificates.end());
    }

    // collect all required ca certificate chains
    auto certChains = this->getCaCertificateChains(caCertificates);

    // special handling for V2GCertificateChain
    if (std::find(certificateTypes.value().begin(), certificateTypes.value().end(),
                  CertificateType::V2GCertificateChain) != certificateTypes.value().end()) {
        const auto v2gCertificateChain = this->getV2GCertificateChain();
        if (!v2gCertificateChain.empty()) {
            certChains.push_back(v2gCertificateChain);
        }
    }

    // iterate over certChains and add respective entries
    for (const auto& certChain : certChains) {
        CertificateHashDataChain certificateHashDataChainEntry;
        if (!certChain.empty()) {
            certificateHashDataChainEntry.certificateType = certChain.at(0)->type;
            CertificateHashDataType certificateHashData;
            certificateHashData.hashAlgorithm = HashAlgorithmEnumType::SHA256;
            certificateHashData.issuerNameHash = certChain.at(0)->getIssuerNameHash();
            certificateHashData.issuerKeyHash = certChain.at(0)->getIssuerKeyHash();
            certificateHashData.serialNumber = certChain.at(0)->getSerialNumber();
            certificateHashDataChainEntry.certificateHashData = certificateHashData;
        }
        // only add child certificate data for V2GCertificateChain
        if (certChain.size() > 1 and certChain.at(1)->type == CertificateType::V2GCertificateChain) {
            std::vector<ocpp::CertificateHashDataType> childCertificateHashData;
            for (size_t i = 1; i < certChain.size(); i++) {
                auto cert = certChain.at(i);
                CertificateHashDataType certificateHashData;
                certificateHashData.hashAlgorithm = HashAlgorithmEnumType::SHA256;
                certificateHashData.issuerNameHash = cert->getIssuerNameHash();
                certificateHashData.issuerKeyHash = cert->getIssuerKeyHash();
                certificateHashData.serialNumber = cert->getSerialNumber();
                childCertificateHashData.push_back(certificateHashData);
            }
            certificateHashDataChainEntry.childCertificateHashData.emplace(childCertificateHashData);
        }
        certificateHashDataChain.push_back(certificateHashDataChainEntry);
    }

    if (!caCertificates.empty()) {
        certificateHashDataChainOpt.emplace(certificateHashDataChain);
    }

    return certificateHashDataChainOpt;
}

DeleteCertificateResult PkiHandler::deleteRootCertificate(CertificateHashDataType certificate_hash_data,
                                                          int32_t security_profile) {
    // get ca certificates including symlinks to delete both files
    std::vector<std::shared_ptr<X509Certificate>> caCertificates = this->getCaCertificates(true);
    bool found = false;
    bool removeFailed = false;

    const auto numberOfCsmsCa =
        this->getCaCertificates(PkiEnum::CSMS, CertificateType::CentralSystemRootCertificate).size();
    for (std::shared_ptr<X509Certificate> cert : caCertificates) {
        if (cert->getIssuerNameHash() == certificate_hash_data.issuerNameHash.get() &&
            cert->getIssuerKeyHash() == certificate_hash_data.issuerKeyHash.get() &&
            cert->getSerialNumber() == certificate_hash_data.serialNumber.get()) {
            // dont delete if only one root ca is installed and tls is in use
            if (cert->type == CertificateType::CentralSystemRootCertificate && numberOfCsmsCa == 1 &&
                security_profile >= 2) {
                EVLOG_warning
                    << "Rejecting attempt to delete last CSMS root certificate while on security profile 2 or 3";
                return DeleteCertificateResult::Failed;
            }
            found = true;
            if (!std::filesystem::remove(cert->path)) {
                removeFailed = true;
            }
        }
    }
    if (found) {
        if (!removeFailed) {
            return DeleteCertificateResult::Accepted;
        } else {
            return DeleteCertificateResult::Failed;
        }
    } else {
        return DeleteCertificateResult::NotFound;
    }
}

InstallCertificateResult PkiHandler::installRootCertificate(const std::string& rootCertificate,
                                                            const CertificateType& certificateType,
                                                            std::optional<int32_t> certificateStoreMaxLength,
                                                            std::optional<bool> additionalRootCertificateCheck) {
    EVLOG_info << "Installing new root certificate of type: "
               << ocpp::conversions::certificate_type_to_string(certificateType);
    InstallCertificateResult installCertificateResult = InstallCertificateResult::Valid;

    if (certificateStoreMaxLength && this->getCaCertificates().size() >= size_t(certificateStoreMaxLength.value())) {
        return InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    }

    std::shared_ptr<X509Certificate> cert = loadFromString(rootCertificate);
    std::vector<std::shared_ptr<X509Certificate>> certificates;
    certificates.push_back(cert);

    if (cert->x509 == NULL) {
        return InstallCertificateResult::InvalidFormat;
    }
    if (this->isCaCertificateAlreadyInstalled(cert)) {
        EVLOG_info << "Root certificate is already installed";
        return InstallCertificateResult::WriteError;
    }

    std::string certFileName = cert->getCommonName() + "-" + DateTime().to_rfc3339() + ".pem";
    if (this->isCentralSystemRootCertificateInstalled() &&
        (certificateType == CertificateType::CentralSystemRootCertificate or
         certificateType == CertificateType::CSMSRootCertificate) &&
        additionalRootCertificateCheck.has_value() && additionalRootCertificateCheck.value()) {
        if (this->verifyCertificate(PkiEnum::CSMS, certificates) != CertificateVerificationResult::Valid) {
            installCertificateResult = InstallCertificateResult::InvalidCertificateChain;
        }
        const auto oldCsmsCaPath = this->getCsmsCaFilePath();
        if (oldCsmsCaPath.has_value()) {
            cert->path = this->caCsmsPath / CSMS_ROOT_CA;
            std::rename(oldCsmsCaPath.value().c_str(), (this->caCsmsPath / CSMS_ROOT_CA_BACKUP).c_str());
        } else {
            EVLOG_warning << "Could not find and write CSMS CA file.";
            return InstallCertificateResult::WriteError;
        }

    } else {
        if (certificateType == CertificateType::CentralSystemRootCertificate or
            certificateType == CertificateType::CSMSRootCertificate) {
            cert->path = this->caCsmsPath / certFileName;
        } else if (certificateType == CertificateType::ManufacturerRootCertificate) {
            cert->path = this->caMfPath / certFileName;
        } else if (certificateType == CertificateType::V2GRootCertificate) {
            cert->path = this->caV2gPath / certFileName;
        } else if (certificateType == CertificateType::MORootCertificate) {
            cert->path = this->caMoPath / certFileName;
        }
        installCertificateResult = InstallCertificateResult::Ok;
    }

    if (installCertificateResult == InstallCertificateResult::Ok ||
        installCertificateResult == InstallCertificateResult::Valid) {
        if (!cert->write()) {
            installCertificateResult = InstallCertificateResult::WriteError;
            if (certificateType == CertificateType::CentralSystemRootCertificate or
                certificateType == CertificateType::CSMSRootCertificate) {
                std::rename((this->caCsmsPath / CSMS_ROOT_CA_BACKUP).c_str(),
                            (this->caCsmsPath / CSMS_ROOT_CA).c_str());
            }
        }
        this->execOpenSSLRehash(cert->path.parent_path());
    }
    return installCertificateResult;
}

std::shared_ptr<X509Certificate>
PkiHandler::getLeafCertificate(const CertificateSigningUseEnum& certificate_signing_use) {
    std::shared_ptr<X509Certificate> cert = nullptr;
    int validIn = INT_MIN;

    std::filesystem::path leafPath;
    if (certificate_signing_use == CertificateSigningUseEnum::ChargingStationCertificate) {
        leafPath = this->clientCsmsPath / CSMS_LEAF;
    } else {
        leafPath = this->clientCsoPath / V2G_LEAF;
    }

    // TODO(piet): Client certificate could be V2G_LEAF or CSMS_LEAF, possibly only one cert is set
    for (const auto& dirEntry : std::filesystem::directory_iterator(leafPath.parent_path())) {
        if (dirEntry.path().string().find(leafPath.filename().c_str()) != std::string::npos) {
            std::shared_ptr<X509Certificate> c = loadFromFile(dirEntry.path());
            if (c != nullptr && c->validIn < 0 && c->validIn > validIn) {
                cert = c;
            }
        }
    }
    return cert;
}

std::filesystem::path PkiHandler::getLeafPrivateKeyPath(const CertificateSigningUseEnum& certificate_signing_use) {
    if (certificate_signing_use == CertificateSigningUseEnum::ChargingStationCertificate) {
        return this->clientCsmsPath / CSMS_LEAF_KEY;
    } else {
        return this->clientCsoPath / V2G_LEAF_KEY;
    }
}

void PkiHandler::removeCentralSystemFallbackCa() {
    std::remove((this->caCsmsPath / CSMS_ROOT_CA_BACKUP).c_str());
}

void PkiHandler::useCsmsFallbackRoot() {
    if (std::filesystem::exists(this->caCsmsPath / CSMS_ROOT_CA_BACKUP)) {
        std::remove((this->caCsmsPath / CSMS_ROOT_CA).c_str()); // remove recently installed ca
        std::filesystem::path new_path = this->caCsmsPath / CSMS_ROOT_CA;
        std::rename((this->caCsmsPath / CSMS_ROOT_CA_BACKUP).c_str(), new_path.c_str());
        this->execOpenSSLRehash(this->caCsmsPath);
    } else {
        EVLOG_warning << "Cant use csms fallback root CA because no fallback file exists";
    }
}

int PkiHandler::validIn(const std::string& certificate) {
    std::shared_ptr<X509Certificate> cert = loadFromString(certificate);
    return X509_cmp_current_time(X509_get_notBefore(cert->x509));
}

int PkiHandler::getDaysUntilLeafExpires(const CertificateSigningUseEnum& certificate_signing_use) {
    std::shared_ptr<X509Certificate> cert = this->getLeafCertificate(certificate_signing_use);
    if (cert != nullptr) {
        const ASN1_TIME* expires = X509_get0_notAfter(cert->x509);
        int days, seconds;
        ASN1_TIME_diff(&days, &seconds, NULL, expires);
        return days;
    } else {
        return 0;
    }
}

void PkiHandler::updateOcspCache(const OCSPRequestData& ocspRequestData, const std::string& ocspResponse) {
    const auto caCertificates = this->getCaCertificates(PkiEnum::CSO, CertificateType::V2GCertificateChain);
    for (const auto& cert : caCertificates) {
        if (cert->getIssuerNameHash() == ocspRequestData.issuerNameHash &&
            cert->getIssuerKeyHash() == ocspRequestData.issuerKeyHash &&
            cert->getSerialNumber() == ocspRequestData.serialNumber) {
            EVLOG_info << "Writing OCSP Response to filesystem";
            const auto ocspPath = cert->path.parent_path() / "ocsp";
            if (!std::filesystem::exists(ocspPath)) {
                std::filesystem::create_directories(ocspPath);
            }
            const auto ocspFilePath = ocspPath / cert->path.filename().replace_extension(".ocsp.der");
            std::ofstream fs(ocspFilePath.string());
            fs << ocspResponse << std::endl;
            fs.close();
        }
    }
}

std::vector<OCSPRequestData> PkiHandler::getOCSPRequestData() {
    std::vector<OCSPRequestData> ocspRequestData;
    const auto caCertificates = this->getCaCertificates(PkiEnum::CSO, CertificateType::V2GCertificateChain);

    for (auto const& certificate : caCertificates) {
        ocspRequestData.push_back(certificate->getOCSPRequestData());
    }
    return ocspRequestData;
}

CertificateVerificationResult getCertificateValidationResult(const int ec) {
    switch (ec) {
    case X509_V_ERR_CERT_HAS_EXPIRED:
        EVLOG_error << "Certificate has expired";
        return CertificateVerificationResult::Expired;
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
        EVLOG_error << "Invalid signature";
        return CertificateVerificationResult::InvalidSignature;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        EVLOG_error << "Invalid certificate chain";
        return CertificateVerificationResult::InvalidCertificateChain;
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        EVLOG_error << "Unable to verify leaf signature";
        return CertificateVerificationResult::InvalidSignature;
    default:
        EVLOG_error << X509_verify_cert_error_string(ec);
        return CertificateVerificationResult::InvalidCertificateChain;
    }
}

CertificateVerificationResult
PkiHandler::verifyCertificate(const PkiEnum& pki, const std::vector<std::shared_ptr<X509Certificate>>& certificates,
                              const std::optional<std::string>& chargeBoxSerialNumber) {
    EVLOG_info << "Verifying certificate...";
    const auto leaf_certificate = certificates.at(0);
    if (leaf_certificate->x509 == NULL) {
        return CertificateVerificationResult::InvalidCertificateChain;
    }

    X509_STORE_ptr store_ptr(X509_STORE_new(), ::X509_STORE_free);
    X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new(), ::X509_STORE_CTX_free);

    for (size_t i = 1; i < certificates.size(); i++) {
        if (certificates.at(i)->x509 == NULL) {
            return CertificateVerificationResult::InvalidCertificateChain;
        }
        X509_STORE_add_cert(store_ptr.get(), certificates.at(i)->x509);
    }

    X509_STORE_load_locations(store_ptr.get(), NULL, this->getCaPath(pki).c_str());

    if (pki != PkiEnum::MF) {
        // we always trust V2G if its not a Manufacturer certificate
        X509_STORE_load_locations(store_ptr.get(), NULL, this->getCaPath(PkiEnum::V2G).c_str());
    }

    // X509_STORE_set_default_paths(store_ptr.get()); //FIXME(piet): When do we load default verify paths?

    X509_STORE_CTX_init(store_ctx_ptr.get(), store_ptr.get(), leaf_certificate->x509, NULL);

    // verifies the certificate chain based on ctx
    // verifies the certificate has not expired and is already valid
    if (X509_verify_cert(store_ctx_ptr.get()) != 1) {
        int ec = X509_STORE_CTX_get_error(store_ctx_ptr.get());
        return getCertificateValidationResult(ec);
    }

    if (chargeBoxSerialNumber.has_value() and
        X509_check_host(leaf_certificate->x509, chargeBoxSerialNumber.value().c_str(),
                        chargeBoxSerialNumber.value().length(), 0, NULL) == 0) {
        EVLOG_warning << "Subject field CN of certificate is not equal to ChargeBoxSerialNumber: "
                      << chargeBoxSerialNumber.value();
        return CertificateVerificationResult::InvalidCommonName;
    }
    EVLOG_info << "Certificate is valid";
    return CertificateVerificationResult::Valid;
}

std::vector<std::shared_ptr<X509Certificate>> PkiHandler::getCertificatesFromChain(const std::string& certChain) {
    std::vector<std::shared_ptr<X509Certificate>> certificates;
    static const std::regex cert_regex("-----BEGIN CERTIFICATE-----[\\s\\S]*?-----END CERTIFICATE-----");
    std::regex_iterator<const char*> cert_begin(certChain.c_str(), certChain.c_str() + certChain.size(), cert_regex);
    std::regex_iterator<const char*> cert_end;
    for (auto i = cert_begin; i != cert_end; ++i) {
        certificates.push_back(loadFromString(i->str()));
    }
    return certificates;
}

std::vector<std::vector<std::shared_ptr<X509Certificate>>>
PkiHandler::getCaCertificateChains(const std::vector<std::shared_ptr<X509Certificate>>& certificates) {

    std::vector<std::vector<std::shared_ptr<X509Certificate>>> certChains;
    std::vector<std::shared_ptr<X509Certificate>> rootCertificates;
    std::vector<std::shared_ptr<X509Certificate>> remainingCertificates;

    // collect list of rootCertificates (self-signed) and remaining ca certificates
    for (const auto& cert : certificates) {
        // check if self signed
        if (X509_NAME_cmp(X509_get_subject_name(cert->x509), X509_get_issuer_name(cert->x509)) == 0) {
            rootCertificates.push_back(cert);
        } else {
            remainingCertificates.push_back(cert);
        }
    }

    // iterate over the rootCertificates
    for (const auto& rootCert : rootCertificates) {
        // each root certificate builds up its chain
        std::vector<std::shared_ptr<X509Certificate>> certChain;
        certChain.push_back(rootCert);

        bool keepSearching = true;
        // keep searching if a the back of the certChain is an issuer
        while (keepSearching) {
            keepSearching = false;
            for (const auto& cert : remainingCertificates) {
                // check if back of chain issued one of the remaining certificates
                if (X509_check_issued(certChain.back()->x509, cert->x509) == X509_V_OK and certChain.back() != cert) {
                    certChain.push_back(cert);
                    keepSearching = true;
                }
            }
        }
        // FIXME(piet): branch chains are not handled so far. This approach assumes a unique chain starting from root
        certChains.push_back(certChain);
    }
    return certChains;
}

std::vector<std::shared_ptr<X509Certificate>> PkiHandler::getV2GCertificateChain() {
    std::vector<std::shared_ptr<X509Certificate>> v2gCertificateChain;

    const auto v2gLeaf = this->getLeafCertificate(CertificateSigningUseEnum::V2GCertificate);
    if (v2gLeaf != nullptr) {
        v2gLeaf->type = CertificateType::V2GCertificateChain;
        v2gCertificateChain.push_back(v2gLeaf);
        const auto csoSubCaCertificates = this->getCaCertificates(PkiEnum::CSO, CertificateType::V2GCertificateChain);
        v2gCertificateChain.insert(v2gCertificateChain.end(), csoSubCaCertificates.begin(), csoSubCaCertificates.end());
    }

    return v2gCertificateChain;
}

bool PkiHandler::isCaCertificateAlreadyInstalled(const std::shared_ptr<X509Certificate>& certificate) {
    const auto caCertificates = this->getCaCertificates(false);
    for (const auto& caCert : caCertificates) {
        if (caCert->getIssuerNameHash() == certificate->getIssuerNameHash() and
            caCert->getIssuerKeyHash() == certificate->getIssuerKeyHash() and
            caCert->getSerialNumber() == certificate->getSerialNumber()) {
            return true;
        }
    }
    return false;
}

std::filesystem::path PkiHandler::getCaPath(const PkiEnum& pki) {
    switch (pki) {
    case PkiEnum::CSMS:
        return this->caCsmsPath;
    case PkiEnum::CSO:
        return this->caCsoPath;
    case PkiEnum::CPS:
        return this->caCpsPath;
    case PkiEnum::MO:
        return this->caMoPath;
    case PkiEnum::MF:
        return this->caMfPath;
    case PkiEnum::OEM:
        return this->caOemPath;
    case PkiEnum::V2G:
        return this->caV2gPath;
    default:
        return this->caPath;
    }
}

void PkiHandler::execOpenSSLRehash(const std::filesystem::path caPath) {
    for (const auto& entry : std::filesystem::directory_iterator(caPath)) {
        if (std::filesystem::is_symlink(entry.path())) {
            std::filesystem::remove(entry.path());
        }
    }

    auto cmd = std::string("openssl rehash ").append(caPath.string()).append(" > /dev/null 2>&1");
    if (!caPath.empty() and std::system(cmd.c_str()) != 0) {
        EVLOG_error << "Error executing the openssl rehash command for directory: " << caPath.string();
    }
}

std::optional<std::filesystem::path> PkiHandler::getCsmsCaFilePath() {
    std::optional<std::filesystem::path> csmsCaFilePath;
    for (const auto& entry : std::filesystem::directory_iterator(this->caCsmsPath)) {
        if (entry.path().extension().string() == ".pem") {
            csmsCaFilePath.emplace(entry.path());
        }
    }
    return csmsCaFilePath;
}

std::vector<std::shared_ptr<X509Certificate>>
PkiHandler::getCaCertificates(const PkiEnum& pki, const CertificateType& type, const bool includeSymlinks) {
    // FIXME(piet) iterate over directory and collect all ca files depending on the type
    std::vector<std::shared_ptr<X509Certificate>> caCertificates;

    const auto caPath = this->getCaPath(pki);

    if (std::filesystem::exists(caPath)) {
        for (const auto& dirEntry : std::filesystem::directory_iterator(caPath)) {
            if (dirEntry.path().extension().string() != ".ocsp" and
                (includeSymlinks or dirEntry.path().extension().string() == ".pem")) {
                auto cert = loadFromFile(dirEntry.path());
                if (cert != nullptr) {
                    cert->type = type;
                    if (X509_check_issued(cert->x509, cert->x509) == X509_V_OK) {
                        caCertificates.insert(caCertificates.begin(), cert);
                    } else {
                        caCertificates.push_back(cert);
                    }
                }
            }
        }
    }
    return caCertificates;
}

std::vector<std::shared_ptr<X509Certificate>> PkiHandler::getCaCertificates(const bool includeSymlinks) {
    std::vector<std::shared_ptr<X509Certificate>> caCertificates;
    std::vector<PkiEnum> pkis = {PkiEnum::CSO, PkiEnum::CSMS, PkiEnum::CPS, PkiEnum::MF,
                                 PkiEnum::MO,  PkiEnum::OEM,  PkiEnum::V2G};

    for (const auto& pki : pkis) {
        auto certs = this->getCaCertificates(pki, getRootCertificateTypeFromPki(pki), includeSymlinks);
        caCertificates.insert(caCertificates.end(), certs.begin(), certs.end());
    }
    return caCertificates;
}

} // namespace ocpp
