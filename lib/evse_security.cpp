// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_security.hpp"
#include <everest/logging.hpp>

#include <algorithm>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <x509_wrapper.hpp>
namespace evse_security {

const std::filesystem::path PEM_EXTENSION = ".pem";
const std::filesystem::path DER_EXTENSION = ".der";
const std::filesystem::path KEY_EXTENSION = ".key";

using X509_STORE_ptr = std::unique_ptr<X509_STORE, decltype(&::X509_STORE_free)>;
using X509_STORE_CTX_ptr = std::unique_ptr<X509_STORE_CTX, decltype(&::X509_STORE_CTX_free)>;
using X509_REQ_ptr = std::unique_ptr<X509_REQ, decltype(&::X509_REQ_free)>;
using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using BIO_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

static std::filesystem::path get_private_key_path(const X509Wrapper& certificate, const std::filesystem::path& key_path,
                                                  const std::optional<std::string> password) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(key_path)) {
        if (std::filesystem::is_regular_file(entry)) {
            auto key_file_path = entry.path();
            if (key_file_path.extension() == KEY_EXTENSION) {
                try {
                    std::ifstream file(key_file_path, std::ios::binary);
                    std::string private_key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    BIO_ptr bio(BIO_new_mem_buf(private_key.c_str(), -1), ::BIO_free);
                    EVP_PKEY_ptr evp_pkey(
                        PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, (void*)password.value_or("").c_str()),
                        EVP_PKEY_free);
                    if (X509_check_private_key(certificate.get(), evp_pkey.get())) {
                        return key_path;
                    }
                } catch (const std::exception& e) {
                    EVLOG_debug << "Could not load or verify private key at: " << key_file_path << ": " << e.what();
                }
            }
        }
    }
    throw NoPrivateKeyException("Could not find private key for given certificate");
}

static X509Wrapper get_latest_valid_certificate(const std::vector<X509Wrapper>& certificates) {
    // Filter certificates with valid_in > 0
    std::vector<X509Wrapper> valid_certificates;
    for (const auto& cert : certificates) {
        if (cert.get_valid_in() >= 0) {
            valid_certificates.push_back(cert);
        }
    }

    if (valid_certificates.empty()) {
        // No valid certificates found
        throw NoCertificateValidException("No valid certificates available.");
    }

    // Find the certificate with the latest valid_in
    auto latest_certificate = std::max_element(
        valid_certificates.begin(), valid_certificates.end(),
        [](const X509Wrapper& cert1, const X509Wrapper& cert2) { return cert1.get_valid_in() < cert2.get_valid_in(); });

    return *latest_certificate;
}

static bool write_to_file(const std::filesystem::path& file_path, const std::string& data, std::ios::openmode mode) {
    try {
        std::ofstream fs(file_path, mode | std::ios::binary);
        if (!fs.is_open()) {
            EVLOG_error << "Error opening file: " << file_path;
            return false;
        }
        fs.write(data.c_str(), data.size());

        if (!fs) {
            EVLOG_error << "Error writing to file: " << file_path;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown error occured while writing to file: " << file_path;
        return false;
    }
    return true;
}

static bool delete_certificate_from_bundle(const std::string& certificate,
                                           const std::filesystem::path& ca_bundle_path) {
    if (!std::filesystem::exists(ca_bundle_path)) {
        return false;
    }
    // Read the content of the file
    std::ifstream in_file(ca_bundle_path);
    if (!in_file) {
        EVLOG_error << "Error opening file: " << ca_bundle_path;
        return false;
    }

    std::string file_content((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
    in_file.close();

    size_t pos = file_content.find(certificate);
    if (pos == std::string::npos) {
        // cert is not part of bundle
        return true;
    }

    file_content.erase(pos, certificate.length());

    std::ofstream out_file(ca_bundle_path);
    if (!out_file) {
        EVLOG_error << "Error opening file for writing: " << ca_bundle_path;
        return false;
    }
    out_file << file_content;
    out_file.close();
    return true;
}

static InstallCertificateResult to_install_certificate_result(const int ec) {
    switch (ec) {
    case X509_V_ERR_CERT_HAS_EXPIRED:
        EVLOG_warning << "Certificate has expired";
        return InstallCertificateResult::Expired;
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
        EVLOG_warning << "Invalid signature";
        return InstallCertificateResult::InvalidSignature;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        EVLOG_warning << "Invalid certificate chain";
        return InstallCertificateResult::InvalidCertificateChain;
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        EVLOG_warning << "Unable to verify leaf signature";
        return InstallCertificateResult::InvalidSignature;
    default:
        EVLOG_warning << X509_verify_cert_error_string(ec);
        return InstallCertificateResult::InvalidCertificateChain;
    }
}

static std::vector<CaCertificateType> get_ca_certificate_types(const std::vector<CertificateType> certificate_types) {
    std::vector<CaCertificateType> ca_certificate_types;
    for (const auto& certificate_type : certificate_types) {
        if (certificate_type == CertificateType::V2GRootCertificate) {
            ca_certificate_types.push_back(CaCertificateType::V2G);
        }
        if (certificate_type == CertificateType::MORootCertificate) {
            ca_certificate_types.push_back(CaCertificateType::MO);
        }
        if (certificate_type == CertificateType::CSMSRootCertificate) {
            ca_certificate_types.push_back(CaCertificateType::CSMS);
        }
        if (certificate_type == CertificateType::MFRootCertificate) {
            ca_certificate_types.push_back(CaCertificateType::MF);
        }
    }
    return ca_certificate_types;
}

static CertificateType get_certificate_type(const CaCertificateType ca_certificate_type) {
    switch (ca_certificate_type) {
    case CaCertificateType::V2G:
        return CertificateType::V2GRootCertificate;
    case CaCertificateType::MO:
        return CertificateType::MORootCertificate;
    case CaCertificateType::CSMS:
        return CertificateType::CSMSRootCertificate;
    case CaCertificateType::MF:
        return CertificateType::MFRootCertificate;
    default:
        throw std::runtime_error("Could not convert CaCertificateType to CertificateType");
    }
}

static std::string get_random_file_name(const std::string& extension) {
    char path[] = "XXXXXX";
    mkstemp(path);

    return std::string(path) + extension;
}

std::vector<X509Wrapper> get_leaf_certificates(const std::filesystem::path& cert_dir) {
    std::vector<X509Wrapper> certificates;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(cert_dir)) {
        if (std::filesystem::is_regular_file(entry)) {
            const auto cert_path = entry.path();
            try {
                if (cert_path.extension() == PEM_EXTENSION) {
                    certificates.emplace_back(cert_path, EncodingFormat::PEM);
                } else if (cert_path.extension() == DER_EXTENSION) {
                    certificates.emplace_back(cert_path, EncodingFormat::DER);
                } else {
                    // Ignore other file formats
                }
            } catch (const CertificateLoadException& e) {
                EVLOG_debug << "Could not load client certificate from specified directory: " << cert_path.string();
            }
        }
    }
    return certificates;
}

std::vector<X509Wrapper> get_leaf_certificates(std::vector<std::filesystem::path> paths) {
    std::vector<X509Wrapper> certificates;

    for (const auto& path : paths) {
        auto certifs = get_leaf_certificates(path);

        if (certifs.size() > 0) {
            certificates.reserve(certificates.size() + certifs.size());
            std::move(std::begin(certifs), std::end(certifs), std::back_inserter(certificates));
        }
    }

    return certificates;
}

std::vector<X509Wrapper>
get_ca_certificates(const std::map<CaCertificateType, std::filesystem::path>& ca_bundle_path_map) {
    // x509 wrapper specific
    std::vector<X509Wrapper> ca_certificates;
    for (auto const& [certificate_type, ca_bundle_path] : ca_bundle_path_map) {
        try {
            X509Wrapper ca_bundle(ca_bundle_path, EncodingFormat::PEM);
            const auto certificates_of_bundle = ca_bundle.split();

            if (certificates_of_bundle.size() > 0) {
                ca_certificates.reserve(ca_certificates.size() + certificates_of_bundle.size());
                std::move(std::begin(certificates_of_bundle), std::end(certificates_of_bundle),
                          std::back_inserter(ca_certificates));
            }
        } catch (const CertificateLoadException& e) {
            EVLOG_info << "Could not load ca bundle from file: " << ca_bundle_path;
        }
    }
    return ca_certificates;
}

EvseSecurity::EvseSecurity(const FilePaths& file_paths, const std::optional<std::string>& private_key_password) :
    private_key_password(private_key_password) {

    std::vector<std::filesystem::path> dirs = {
        file_paths.directories.csms_leaf_cert_directory,
        file_paths.directories.csms_leaf_key_directory,
        file_paths.directories.secc_leaf_cert_directory,
        file_paths.directories.secc_leaf_key_directory,
    };

    for (const auto& path : dirs) {
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Could not find configured leaf directory at: " + path.string());
        } else if (!std::filesystem::is_directory(path)) {
            throw std::runtime_error(path.string() + " is not a directory.");
        }
    }

    this->ca_bundle_path_map[CaCertificateType::CSMS] = file_paths.csms_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::MF] = file_paths.mf_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::MO] = file_paths.mo_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::V2G] = file_paths.v2g_ca_bundle;

    for (const auto& pair : this->ca_bundle_path_map) {
        if (!std::filesystem::exists(pair.second)) {
            throw std::runtime_error("Could not find configured " +
                                     conversions::ca_certificate_type_to_string(pair.first) +
                                     " bundle file at: " + pair.second.string());
        } else if (std::filesystem::is_directory(pair.second)) {
            throw std::runtime_error("Provided bundle " + conversions::ca_certificate_type_to_string(pair.first) +
                                     " is directory: " + pair.second.string());
        }
    }

    this->directories = file_paths.directories;
}

EvseSecurity::~EvseSecurity() {
}

InstallCertificateResult EvseSecurity::install_ca_certificate(const std::string& certificate,
                                                              CaCertificateType certificate_type) {
    // TODO(piet): Check CertificateStoreMaxEntries

    try {
        X509Wrapper cert(certificate, EncodingFormat::PEM);
        const auto ca_bundle_path = this->ca_bundle_path_map.at(certificate_type);
        if (write_to_file(ca_bundle_path, certificate, std::ios::app)) {
            return InstallCertificateResult::Accepted;
        } else {
            return InstallCertificateResult::WriteError;
        }
    } catch (const CertificateLoadException& e) {
        return InstallCertificateResult::InvalidFormat;
    }
}

DeleteCertificateResult EvseSecurity::delete_certificate(const CertificateHashData& certificate_hash_data) {
    // your code for cmd delete_certificate goes here
    auto response = DeleteCertificateResult::NotFound;

    const auto ca_certificates = get_ca_certificates(ca_bundle_path_map);
    bool found_certificate = false;
    bool failed_to_write = false;

    for (const auto& ca_certificate : ca_certificates) {
        // write a compare function for CertificateHashData and X509 wrapper !
        if (ca_certificate.get_issuer_name_hash() == certificate_hash_data.issuer_name_hash and
            ca_certificate.get_issuer_key_hash() == certificate_hash_data.issuer_key_hash and
            ca_certificate.get_serial_number() == certificate_hash_data.serial_number and
            ca_certificate.get_path().has_value()) {
            // cert could be present in multiple ca bundles
            found_certificate = true;
            if (!delete_certificate_from_bundle(ca_certificate.get_str(), ca_certificate.get_path().value())) {
                failed_to_write = true;
            }
        }
    }

    const auto leaf_certificates =
        get_leaf_certificates({directories.secc_leaf_cert_directory, directories.csms_leaf_cert_directory});
    for (const auto& leaf_certificate : leaf_certificates) {
        if (leaf_certificate.get_issuer_name_hash() == certificate_hash_data.issuer_name_hash and
            leaf_certificate.get_issuer_key_hash() == certificate_hash_data.issuer_key_hash and
            leaf_certificate.get_serial_number() == certificate_hash_data.serial_number and
            leaf_certificate.get_path().has_value()) {
            // cert could be present in multiple ca bundles
            found_certificate = true;
            try {
                std::filesystem::remove(leaf_certificate.get_path().value());
            } catch (const std::filesystem::filesystem_error& e) {
                // don't use liblog! unnecessary dependency
                EVLOG_error << "Error removing leaf certificate: " << e.what();
                failed_to_write = true;
            }
        }
    }

    if (!found_certificate) {
        return DeleteCertificateResult::NotFound;
    }
    if (failed_to_write) {
        // at least one certificate could not be deleted from the bundle
        return DeleteCertificateResult::Failed;
    }
    return DeleteCertificateResult::Accepted;
}

InstallCertificateResult EvseSecurity::update_leaf_certificate(const std::string& certificate_chain,
                                                               LeafCertificateType certificate_type) {
    std::filesystem::path cert_path;
    std::filesystem::path key_path;
    if (certificate_type == LeafCertificateType::CSMS) {
        cert_path = this->directories.csms_leaf_cert_directory;
        key_path = this->directories.csms_leaf_key_directory;
    } else {
        cert_path = this->directories.secc_leaf_cert_directory;
        key_path = this->directories.secc_leaf_key_directory;
    }

    try {
        X509Wrapper certificate(certificate_chain, EncodingFormat::PEM);
        std::vector<X509Wrapper> _certificate_chain = certificate.split();
        if (_certificate_chain.empty()) {
            return InstallCertificateResult::InvalidFormat;
        }
        const auto result = this->verify_certificate(certificate_chain, certificate_type);
        if (result != InstallCertificateResult::Accepted) {
            return result;
        }

        const auto& leaf_certificate = _certificate_chain[0];

        // check if a private key belongs to the provided certificate
        try {
            const auto private_key_path = get_private_key_path(leaf_certificate, key_path, this->private_key_password);
        } catch (const NoPrivateKeyException& e) {
            EVLOG_warning << "Provided certificate does not belong to any private key";
            return InstallCertificateResult::WriteError;
        }

        // write certificate to file
        const auto file_name = get_random_file_name(PEM_EXTENSION.string());
        const auto file_path = cert_path / file_name;
        std::string str_cert = leaf_certificate.get_str();

        if (write_to_file(file_path, str_cert, std::ios::out)) {
            return InstallCertificateResult::Accepted;
        } else {
            return InstallCertificateResult::WriteError;
        }

    } catch (const CertificateLoadException& e) {
        EVLOG_warning << "Could not load update leaf certificate because of invalid format";
        return InstallCertificateResult::InvalidFormat;
    }

    return InstallCertificateResult::Accepted;
}

GetInstalledCertificatesResult EvseSecurity::get_installed_certificate(CertificateType certificate_type) {
    return get_installed_certificates({certificate_type});
}

GetInstalledCertificatesResult
EvseSecurity::get_installed_certificates(const std::vector<CertificateType>& certificate_types) {
    GetInstalledCertificatesResult result;
    std::vector<CertificateHashDataChain> certificate_chains;
    const auto ca_certificate_types = get_ca_certificate_types(certificate_types);

    // retrieve ca certificates and chains
    for (const auto& ca_certificate_type : ca_certificate_types) {
        auto ca_bundle_path = this->ca_bundle_path_map.at(ca_certificate_type);
        try {
            X509Wrapper ca_bundle(ca_bundle_path, EncodingFormat::PEM);
            auto certificates_of_bundle = ca_bundle.split();

            CertificateHashDataChain certificate_hash_data_chain;
            std::vector<CertificateHashData> child_certificate_hash_data;
            for (int i = 0; i < certificates_of_bundle.size(); i++) {
                CertificateHashData certificate_hash_data = certificates_of_bundle[i].get_certificate_hash_data();
                if (i == 0) {
                    certificate_hash_data_chain.certificate_hash_data = certificate_hash_data;
                    certificate_hash_data_chain.certificate_type = get_certificate_type(ca_certificate_type);
                } else {
                    child_certificate_hash_data.push_back(certificate_hash_data);
                }
            }
            certificate_hash_data_chain.child_certificate_hash_data = child_certificate_hash_data;
            certificate_chains.push_back(certificate_hash_data_chain);
        } catch (const CertificateLoadException& e) {
            EVLOG_warning << "Could not load CA bundle file at: " << ca_bundle_path << " error: " << e.what();
        }
    }

    // retrieve v2g certificate chain
    if (std::find(certificate_types.begin(), certificate_types.end(), CertificateType::V2GCertificateChain) !=
        certificate_types.end()) {
        const auto secc_key_pair = this->get_key_pair(LeafCertificateType::V2G, EncodingFormat::PEM);
        if (secc_key_pair.status == GetKeyPairStatus::Accepted) {
            X509Wrapper cert(secc_key_pair.pair.value().certificate, EncodingFormat::PEM);
            CertificateHashDataChain certificate_hash_data_chain;
            CertificateHashData certificate_hash_data = cert.get_certificate_hash_data();
            certificate_hash_data_chain.certificate_hash_data = certificate_hash_data;
            certificate_hash_data_chain.certificate_type = CertificateType::V2GCertificateChain;

            const auto ca_bundle_path = this->ca_bundle_path_map.at(CaCertificateType::V2G);
            X509Wrapper ca_bundle(ca_bundle_path, EncodingFormat::PEM);
            const auto certificates_of_bundle = ca_bundle.split();
            std::vector<CertificateHashData> child_certificate_hash_data;
            bool keep_searching = true;
            while (keep_searching) {
                keep_searching = false;
                for (const auto& ca_cert : certificates_of_bundle) {
                    if (X509_check_issued(ca_cert.get(), cert.get()) == X509_V_OK and
                        ca_cert.get_issuer_name_hash() != cert.get_issuer_name_hash()) {
                        CertificateHashData sub_ca_certificate_hash_data = ca_cert.get_certificate_hash_data();
                        child_certificate_hash_data.push_back(sub_ca_certificate_hash_data);
                        cert.reset(ca_cert.get());
                        keep_searching = true;
                        break;
                    }
                }
            }
            certificate_hash_data_chain.child_certificate_hash_data = child_certificate_hash_data;
            certificate_chains.push_back(certificate_hash_data_chain);
        }
    }

    if (certificate_chains.empty()) {
        result.status = GetInstalledCertificatesStatus::NotFound;
    } else {
        result.status = GetInstalledCertificatesStatus::Accepted;
    }

    result.certificate_hash_data_chain = certificate_chains;
    return result;
}

OCSPRequestDataList EvseSecurity::get_ocsp_request_data() {
    OCSPRequestDataList response;
    std::vector<OCSPRequestData> ocsp_request_data_list;

    X509Wrapper ca_bundle(this->ca_bundle_path_map.at(CaCertificateType::V2G), EncodingFormat::PEM);
    const auto certificates_of_bundle = ca_bundle.split();
    for (const auto& certificate : certificates_of_bundle) {
        std::string responder_url = certificate.get_responder_url();
        if (!responder_url.empty()) {
            auto certificate_hash_data = certificate.get_certificate_hash_data();
            OCSPRequestData ocsp_request_data = {certificate_hash_data, responder_url};
            ocsp_request_data_list.push_back(ocsp_request_data);
        }
    }

    response.ocsp_request_data_list = ocsp_request_data_list;
    return response;
}

void EvseSecurity::update_ocsp_cache(const CertificateHashData& certificate_hash_data,
                                     const std::string& ocsp_response) {
    const auto ca_bundle_path = this->ca_bundle_path_map.at(CaCertificateType::V2G);
    X509Wrapper ca_bundle(ca_bundle_path, EncodingFormat::PEM);
    const auto certificates_of_bundle = ca_bundle.split();

    for (const auto& cert : certificates_of_bundle) {
        if (cert.get_issuer_name_hash() == certificate_hash_data.issuer_name_hash &&
            cert.get_issuer_key_hash() == certificate_hash_data.issuer_key_hash &&
            cert.get_serial_number() == certificate_hash_data.serial_number) {
            EVLOG_info << "Writing OCSP Response to filesystem";
            if (!cert.get_path().has_value()) {
                continue;
            }
            const auto ocsp_path = cert.get_path().value().parent_path() / "ocsp";
            if (!std::filesystem::exists(ocsp_path)) {
                std::filesystem::create_directories(ocsp_path);
            }
            const auto ocsp_file_path = ocsp_path / cert.get_path().value().filename().replace_extension(".ocsp.der");
            std::ofstream fs(ocsp_file_path.c_str());
            fs << ocsp_response;
            fs.close();
        }
    }
}

bool EvseSecurity::is_ca_certificate_installed(CaCertificateType certificate_type) {
    try {
        X509Wrapper(this->ca_bundle_path_map.at(certificate_type), EncodingFormat::PEM);
        return true;
    } catch (const CertificateLoadException& e) {
        return false;
    }
}

std::string EvseSecurity::generate_certificate_signing_request(LeafCertificateType certificate_type,
                                                               const std::string& country,
                                                               const std::string& organization,
                                                               const std::string& common) {
    int n_version = 0;
    int bits = 256;

    std::filesystem::path key_path;

    const auto file_name = get_random_file_name(KEY_EXTENSION.string());
    if (certificate_type == LeafCertificateType::CSMS) {
        key_path = this->directories.csms_leaf_key_directory / file_name;
    } else if (certificate_type == LeafCertificateType::V2G) {
        key_path = this->directories.secc_leaf_key_directory / file_name;
    } else {
        throw std::runtime_error("Attempt to generate CSR for MF certificate");
    }

    // csr req
    X509_REQ_ptr x509ReqPtr(X509_REQ_new(), X509_REQ_free);
    EVP_PKEY_ptr evpKey(EVP_PKEY_new(), EVP_PKEY_free);
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    X509_NAME* x509Name = X509_REQ_get_subject_name(x509ReqPtr.get());

    BIO_ptr prkey(BIO_new_file(key_path.c_str(), "w"), ::BIO_free);
    BIO_ptr bio(BIO_new(BIO_s_mem()), ::BIO_free);

    // generate ec key pair
    EC_KEY_generate_key(ecKey);
    EVP_PKEY_assign_EC_KEY(evpKey.get(), ecKey);
    // write private key to file
    PEM_write_bio_PrivateKey(prkey.get(), evpKey.get(), NULL, NULL, 0, NULL, NULL);

    // set version of x509 req
    X509_REQ_set_version(x509ReqPtr.get(), n_version);

    // set subject of x509 req
    X509_NAME_add_entry_by_txt(x509Name, "C", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(country.c_str()), -1,
                               -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "O", MBSTRING_ASC,
                               reinterpret_cast<const unsigned char*>(organization.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(common.c_str()), -1,
                               -1, 0);
    X509_NAME_add_entry_by_txt(x509Name, "DC", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("CPO"), -1, -1, 0);
    // set public key of x509 req
    X509_REQ_set_pubkey(x509ReqPtr.get(), evpKey.get());

    STACK_OF(X509_EXTENSION)* extensions = sk_X509_EXTENSION_new_null();
    X509_EXTENSION* ext_key_usage = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, "digitalSignature, keyAgreement");
    X509_EXTENSION* ext_basic_constraints = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, "critical,CA:false");
    sk_X509_EXTENSION_push(extensions, ext_key_usage);
    sk_X509_EXTENSION_push(extensions, ext_basic_constraints);

    X509_REQ_add_extensions(x509ReqPtr.get(), extensions);

    // set sign key of x509 req
    X509_REQ_sign(x509ReqPtr.get(), evpKey.get(), EVP_sha256());

    // write csr
    PEM_write_bio_X509_REQ(bio.get(), x509ReqPtr.get());

    BUF_MEM* mem_csr = NULL;
    BIO_get_mem_ptr(bio.get(), &mem_csr);
    std::string csr(mem_csr->data, mem_csr->length);

    EVLOG_debug << csr;

    return csr;
}

GetKeyPairResult EvseSecurity::get_key_pair(LeafCertificateType certificate_type, EncodingFormat encoding) {
    GetKeyPairResult result;
    result.pair = std::nullopt;

    std::filesystem::path key_dir;
    std::filesystem::path cert_dir;

    if (certificate_type == LeafCertificateType::CSMS) {
        key_dir = this->directories.csms_leaf_key_directory;
        cert_dir = this->directories.csms_leaf_cert_directory;
    } else if (certificate_type == LeafCertificateType::V2G) {
        key_dir = this->directories.secc_leaf_key_directory;
        cert_dir = this->directories.secc_leaf_cert_directory;
    } else {
        EVLOG_warning << "Rejected attempt to retrieve MF key pair";
        result.status = GetKeyPairStatus::Rejected;
        return result;
    }

    const auto certificates = get_leaf_certificates(cert_dir);

    if (certificates.empty()) {
        EVLOG_warning << "Could not find any key pair";
        result.status = GetKeyPairStatus::NotFound;
        return result;
    }

    // choose appropriate cert (valid_from / valid_to)
    try {
        const auto certificate = get_latest_valid_certificate(certificates);
        const auto private_key_path = get_private_key_path(certificate, key_dir, this->private_key_password);

        result.pair = {private_key_path.string(), certificate.get_path().value(), this->private_key_password};
        result.status = GetKeyPairStatus::Accepted;

        return result;
    } catch (const NoPrivateKeyException& e) {
        EVLOG_warning << "Could not find private key for the selected certificate";
        result.status = GetKeyPairStatus::PrivateKeyNotFound;
        return result;
    } catch (const NoCertificateValidException& e) {
        EVLOG_warning << "Could not find valid cerificate";
        result.status = GetKeyPairStatus::NotFoundValid;
        return result;
    }
}

std::string EvseSecurity::get_verify_file(CaCertificateType certificate_type) {
    X509Wrapper verify_file(this->ca_bundle_path_map.at(certificate_type), EncodingFormat::PEM);
    return verify_file.get_path().value().c_str();
}

int EvseSecurity::get_leaf_expiry_days_count(LeafCertificateType certificate_type) {
    const auto key_pair = this->get_key_pair(certificate_type, EncodingFormat::PEM);
    if (key_pair.status == GetKeyPairStatus::Accepted) {
        X509Wrapper cert(key_pair.pair.value().certificate, EncodingFormat::PEM);
        return cert.get_valid_to() / 86400;
    }
    return 0;
}

InstallCertificateResult EvseSecurity::verify_certificate(const std::string& certificate_chain,
                                                          LeafCertificateType certificate_type) {
    try {
        X509Wrapper certificate(certificate_chain, EncodingFormat::PEM);
        std::vector<X509Wrapper> _certificate_chain = certificate.split();
        if (_certificate_chain.empty()) {
            return InstallCertificateResult::InvalidFormat;
        }

        const auto leaf_certificate = _certificate_chain.at(0);

        X509_STORE_ptr store_ptr(X509_STORE_new(), ::X509_STORE_free);
        X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new(), ::X509_STORE_CTX_free);

        for (size_t i = 1; i < _certificate_chain.size(); i++) {
            X509_STORE_add_cert(store_ptr.get(), _certificate_chain[i].get());
        }

        if (certificate_type == LeafCertificateType::CSMS) {
            X509_STORE_load_locations(store_ptr.get(), this->ca_bundle_path_map.at(CaCertificateType::CSMS).c_str(),
                                      NULL);
        } else if (certificate_type == LeafCertificateType::V2G) {
            X509_STORE_load_locations(store_ptr.get(), this->ca_bundle_path_map.at(CaCertificateType::V2G).c_str(),
                                      NULL);
        } else {
            X509_STORE_load_locations(store_ptr.get(), this->ca_bundle_path_map.at(CaCertificateType::MF).c_str(),
                                      NULL);
        }

        X509_STORE_CTX_init(store_ctx_ptr.get(), store_ptr.get(), leaf_certificate.get(), NULL);

        // verifies the certificate chain based on ctx
        // verifies the certificate has not expired and is already valid
        if (X509_verify_cert(store_ctx_ptr.get()) != 1) {
            int ec = X509_STORE_CTX_get_error(store_ctx_ptr.get());
            return to_install_certificate_result(ec);
        }

        return InstallCertificateResult::Accepted;
    } catch (const CertificateLoadException& e) {
        EVLOG_warning << "Could not load update leaf certificate because of invalid format";
        return InstallCertificateResult::InvalidFormat;
    }
}

} // namespace evse_security
