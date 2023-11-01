// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_security.hpp"
#include <everest/logging.hpp>

#include <algorithm>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/x509v3.h>
#include <stdio.h>

#include <evse_utilities.hpp>
#include <x509_bundle.hpp>
#include <x509_wrapper.hpp>
namespace evse_security {

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

static std::filesystem::path get_private_key_path(const X509Wrapper& certificate, const std::filesystem::path& key_path,
                                                  const std::optional<std::string> password) {
    // TODO(ioan): Before iterating the whole dir check by the filename first 'key_path'.key
    for (const auto& entry : std::filesystem::recursive_directory_iterator(key_path)) {
        if (std::filesystem::is_regular_file(entry)) {
            auto key_file_path = entry.path();
            if (key_file_path.extension() == KEY_EXTENSION) {
                try {
                    std::ifstream file(key_file_path, std::ios::binary);
                    std::string private_key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    BIO_ptr bio(BIO_new_mem_buf(private_key.c_str(), -1));

                    EVP_PKEY_ptr evp_pkey(
                        PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, (void*)password.value_or("").c_str()));

                    if (!evp_pkey) {
                        EVLOG_warning << "Invalid evp_pkey: " << key_path
                                      << " error: " << ERR_error_string(ERR_get_error(), NULL)
                                      << " Password configured correctly?";

                        // Next entry if the key was null
                        continue;
                    }

                    if (X509_check_private_key(certificate.get(), evp_pkey.get())) {
                        EVLOG_info << "Key found for certificate at path: " << key_file_path;
                        return key_file_path;
                    }
                } catch (const std::exception& e) {
                    EVLOG_info << "Could not load or verify private key at: " << key_file_path << ": " << e.what();
                }
            }
        }
    }
    std::string error = "Could not find private key for given certificate: ";
    error += certificate.get_file().value_or("N/A");
    error += " key path: ";
    error += key_path;

    throw NoPrivateKeyException(error);
}

X509CertificateBundle get_leaf_certificates(const std::filesystem::path& cert_dir) {
    return X509CertificateBundle(cert_dir, EncodingFormat::PEM);
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
            EVLOG_warning << "Could not find configured leaf directory at: " << path.string()
                          << " creating default dir!";
            if (!std::filesystem::create_directories(path)) {
                EVLOG_error << "Could not create default dir for path: " << path.string();
            }
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
            EVLOG_warning << "Could not find configured " << conversions::ca_certificate_type_to_string(pair.first)
                          << " bundle file at: " + pair.second.string() << ", creating default!";
            if (!EvseUtils::create_file_if_nonexistent(pair.second)) {
                EVLOG_error << "Could not create default bundle for path: " << pair.second;
            }
        }
    }

    this->directories = file_paths.directories;
}

EvseSecurity::~EvseSecurity() {
}

InstallCertificateResult EvseSecurity::install_ca_certificate(const std::string& certificate,
                                                              CaCertificateType certificate_type) {
    EVLOG_info << "Installing ca certificate: " << conversions::ca_certificate_type_to_string(certificate_type);
    // TODO(piet): Check CertificateStoreMaxEntries
    try {
        X509Wrapper new_cert(certificate, EncodingFormat::PEM);

        // Load existing
        const auto ca_bundle_path = this->ca_bundle_path_map.at(certificate_type);

        // TODO: (ioan) check if we are in bundle directory mode too
        // Ensure file exists
        EvseUtils::create_file_if_nonexistent(ca_bundle_path);

        X509CertificateBundle existing_certs(ca_bundle_path, EncodingFormat::PEM);

        // Check if cert is already installed
        if (existing_certs.contains_certificate(new_cert) == false) {
            existing_certs.add_certificate(std::move(new_cert));

            if (existing_certs.export_certificates()) {
                return InstallCertificateResult::Accepted;
            } else {
                return InstallCertificateResult::WriteError;
            }
        } else {
            // Else, simply update it
            if (existing_certs.update_certificate(std::move(new_cert))) {
                if (existing_certs.export_certificates()) {
                    return InstallCertificateResult::Accepted;
                } else {
                    return InstallCertificateResult::WriteError;
                }
            } else {
                return InstallCertificateResult::WriteError;
            }
        }
    } catch (const CertificateLoadException& e) {
        EVLOG_error << "Certificate load error: " << e.what();
        return InstallCertificateResult::InvalidFormat;
    }
}

DeleteCertificateResult EvseSecurity::delete_certificate(const CertificateHashData& certificate_hash_data) {
    auto response = DeleteCertificateResult::NotFound;

    bool found_certificate = false;
    bool failed_to_write = false;

    for (auto const& [certificate_type, ca_bundle_path] : ca_bundle_path_map) {
        try {
            X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);

            if (ca_bundle.delete_certificate(certificate_hash_data)) {
                found_certificate = true;
                if (!ca_bundle.export_certificates()) {
                    failed_to_write = true;
                }
            }

        } catch (const CertificateLoadException& e) {
            EVLOG_info << "Could not load ca bundle from file: " << ca_bundle_path;
        }
    }

    for (const auto& leaf_certificate_path :
         {directories.secc_leaf_cert_directory, directories.csms_leaf_cert_directory}) {
        try {
            X509CertificateBundle leaf_bundle(leaf_certificate_path, EncodingFormat::PEM);

            if (leaf_bundle.delete_certificate(certificate_hash_data)) {
                found_certificate = true;
                if (!leaf_bundle.export_certificates()) {
                    failed_to_write = true;
                    EVLOG_error << "Error removing leaf certificate: " << certificate_hash_data.issuer_name_hash;
                }
            }
        } catch (const CertificateLoadException& e) {
            EVLOG_info << "Could not load ca bundle from file: " << leaf_certificate_path;
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
        X509CertificateBundle chain_certificate(certificate_chain, EncodingFormat::PEM);
        std::vector<X509Wrapper> _certificate_chain = chain_certificate.split();
        if (_certificate_chain.empty()) {
            return InstallCertificateResult::InvalidFormat;
        }
        const auto result = this->verify_certificate(certificate_chain, certificate_type);
        if (result != InstallCertificateResult::Accepted) {
            return result;
        }

        // First certificate is always the leaf as per the spec
        const auto& leaf_certificate = _certificate_chain[0];

        // Check if a private key belongs to the provided certificate
        try {
            const auto private_key_path = get_private_key_path(leaf_certificate, key_path, this->private_key_password);
        } catch (const NoPrivateKeyException& e) {
            EVLOG_warning << "Provided certificate does not belong to any private key";
            return InstallCertificateResult::WriteError;
        }

        // Write certificate to file
        const auto file_name = std::string("SECC_LEAF_") + EvseUtils::get_random_file_name(PEM_EXTENSION.string());
        const auto file_path = cert_path / file_name;
        std::string str_cert = leaf_certificate.get_export_string();

        // Also write chain to file
        const auto chain_file_name =
            std::string("CPO_CERT_CHAIN_") + EvseUtils::get_random_file_name(PEM_EXTENSION.string());
        const auto chain_file_path = cert_path / chain_file_name;
        std::string str_chain_cert = chain_certificate.to_export_string();

        if (EvseUtils::write_to_file(file_path, str_cert, std::ios::out) &&
            EvseUtils::write_to_file(chain_file_path, str_chain_cert, std::ios::out)) {
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
            X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);
            auto certificates_of_bundle = ca_bundle.split();

            CertificateHashDataChain certificate_hash_data_chain;
            certificate_hash_data_chain.certificate_type =
                get_certificate_type(ca_certificate_type); // We always know type

            for (int i = 0; i < certificates_of_bundle.size(); i++) {
                CertificateHashData certificate_hash_data = certificates_of_bundle[i].get_certificate_hash_data();
                if (i == 0) {
                    certificate_hash_data_chain.certificate_hash_data = certificate_hash_data;
                } else {
                    certificate_hash_data_chain.child_certificate_hash_data.push_back(certificate_hash_data);
                }
            }

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

            // TODO (ioan): as per V2GCertificateChain: OCPP 2.0.1 part 2 spec 2 (3.36):
            // V2G certificate chain (excluding the V2GRootCertificate)
            // Exclude V2G root when returning the V2GCertificateChain
            const auto ca_bundle_path = this->ca_bundle_path_map.at(CaCertificateType::V2G);
            X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);
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

    X509CertificateBundle ca_bundle(this->ca_bundle_path_map.at(CaCertificateType::V2G), EncodingFormat::PEM);
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

    try {
        X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);
        const auto certificates_of_bundle = ca_bundle.split();

        for (const auto& cert : certificates_of_bundle) {
            if (cert == certificate_hash_data) {
                EVLOG_info << "Writing OCSP Response to filesystem";
                if (!cert.get_file().has_value()) {
                    continue;
                }
                const auto ocsp_path = cert.get_file().value().parent_path() / "ocsp";
                if (!std::filesystem::exists(ocsp_path)) {
                    std::filesystem::create_directories(ocsp_path);
                }
                const auto ocsp_file_path =
                    ocsp_path / cert.get_file().value().filename().replace_extension(".ocsp.der");
                std::ofstream fs(ocsp_file_path.c_str());
                fs << ocsp_response;
                fs.close();
            }
        }
    } catch (const CertificateLoadException& e) {
        EVLOG_error << "Could update ocsp cache, certificate load failure!";
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

    const auto file_name = std::string("SECC_LEAF_") + EvseUtils::get_random_file_name(KEY_EXTENSION.string());
    if (certificate_type == LeafCertificateType::CSMS) {
        key_path = this->directories.csms_leaf_key_directory / file_name;
    } else if (certificate_type == LeafCertificateType::V2G) {
        key_path = this->directories.secc_leaf_key_directory / file_name;
    } else {
        throw std::runtime_error("Attempt to generate CSR for MF certificate");
    }

    // csr req
    X509_REQ_ptr x509ReqPtr(X509_REQ_new());
    EVP_PKEY_ptr evpKey(EVP_PKEY_new());
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    X509_NAME* x509Name = X509_REQ_get_subject_name(x509ReqPtr.get());

    BIO_ptr prkey(BIO_new_file(key_path.c_str(), "w"));
    BIO_ptr bio(BIO_new(BIO_s_mem()));

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
    EVLOG_info << "Requesting key/pair: " << conversions::leaf_certificate_type_to_string(certificate_type);

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

    // choose appropriate cert (valid_from / valid_to)
    try {
        auto leaf_certificates = std::move(get_leaf_certificates(cert_dir));

        if (leaf_certificates.empty()) {
            EVLOG_warning << "Could not find any key pair";
            result.status = GetKeyPairStatus::NotFound;
            return result;
        }

        std::filesystem::path key_file;
        std::filesystem::path certificate_file;

        auto certificate = std::move(leaf_certificates.get_latest_valid_certificate());
        auto private_key_path = get_private_key_path(certificate, key_dir, this->private_key_password);

        // Key path doesn't change
        key_file = private_key_path;

        // We are searching for the full leaf bundle, containing both the leaf and the cso1/2
        X509CertificateDirectory leaf_directory(cert_dir);
        const X509CertificateBundle* leaf_fullchain = nullptr;

        // Collect the correct bundle
        leaf_directory.for_each([&certificate, &leaf_fullchain](const X509CertificateBundle& bundle) {
            // If we contain the latest valid, we found our generated bundle
            if (bundle.is_bundle() && bundle.contains_certificate(certificate)) {
                leaf_fullchain = &bundle;
                return false;
            }

            return true;
        });

        if (leaf_fullchain != nullptr) {
            certificate_file = leaf_fullchain->get_path();
        } else {
            EVLOG_warning << "V2G leaf requires full bundle, but full bundle not found at path: " << cert_dir;
        }

        // If chain is not found, set single leaf certificate file
        if (certificate_file.empty()) {
            certificate_file = certificate.get_file().value();
        }

        result.pair = {key_file, certificate_file, this->private_key_password};
        result.status = GetKeyPairStatus::Accepted;

        return result;
    } catch (const NoPrivateKeyException& e) {
        EVLOG_warning << "Could not find private key for the selected certificate: (" << e.what() << ")";
        result.status = GetKeyPairStatus::PrivateKeyNotFound;
        return result;
    } catch (const NoCertificateValidException& e) {
        EVLOG_warning << "Could not find valid cerificate";
        result.status = GetKeyPairStatus::NotFoundValid;
        return result;
    } catch (const CertificateLoadException& e) {
        EVLOG_warning << "Leaf certificate load exception";
        result.status = GetKeyPairStatus::NotFound;
        return result;
    }
}

std::string EvseSecurity::get_verify_file(CaCertificateType certificate_type) {
    EVLOG_info << "Requesting certificate file: " << conversions::ca_certificate_type_to_string(certificate_type);

    // Support bundle files, in case the certificates contain
    // multiple entries (should be 3) as per the specification
    X509CertificateBundle verify_file(this->ca_bundle_path_map.at(certificate_type), EncodingFormat::PEM);
    return verify_file.get_path().string();
}

int EvseSecurity::get_leaf_expiry_days_count(LeafCertificateType certificate_type) {
    EVLOG_info << "Requesting certificate expiry: " << conversions::leaf_certificate_type_to_string(certificate_type);

    const auto key_pair = this->get_key_pair(certificate_type, EncodingFormat::PEM);
    if (key_pair.status == GetKeyPairStatus::Accepted) {
        // In case it is a bundle, we know the leaf is always the first
        X509CertificateBundle cert(key_pair.pair.value().certificate, EncodingFormat::PEM);

        int64_t seconds = cert.get_at(0).get_valid_to();
        return std::chrono::duration_cast<ossl_days_to_seconds>(std::chrono::seconds(seconds)).count();
    }
    return 0;
}

bool EvseSecurity::verify_file_signature(const std::filesystem::path& path, const std::string& signing_certificate,
                                         const std::string signature) {
    EVLOG_info << "Verifying file signature for " << path.string();

    // calculate sha256 of file
    FILE_ptr file_ptr(fopen(path.string().c_str(), "rb"));
    if (!file_ptr.get()) {
        EVLOG_error << "Could not open file at: " << path.string();
        return false;
    }
    EVP_MD_CTX_ptr md_context_ptr(EVP_MD_CTX_create());
    if (!md_context_ptr.get()) {
        EVLOG_error << "Could not create EVP_MD_CTX";
        return false;
    }
    const EVP_MD* md = EVP_get_digestbyname("SHA256");
    if (EVP_DigestInit_ex(md_context_ptr.get(), md, nullptr) == 0) {
        EVLOG_error << "Error during EVP_DigestInit_ex";
        return false;
    }
    size_t in_length;
    unsigned char file_buffer[BUFSIZ];
    do {
        in_length = fread(file_buffer, 1, BUFSIZ, file_ptr.get());
        if (EVP_DigestUpdate(md_context_ptr.get(), file_buffer, in_length) == 0) {
            EVLOG_error << "Error during EVP_DigestUpdate";
            return false;
        }
    } while (in_length == BUFSIZ);
    unsigned int sha256_out_length;
    unsigned char sha256_out[EVP_MAX_MD_SIZE];
    if (EVP_DigestFinal_ex(md_context_ptr.get(), sha256_out, &sha256_out_length) == 0) {
        EVLOG_error << "Error during EVP_DigestFinal_ex";
        return false;
    }

    try {
        X509Wrapper x509_signing_cerificate(signing_certificate, EncodingFormat::PEM);

        // extract public key
        EVP_PKEY_ptr public_key_ptr(X509_get_pubkey(x509_signing_cerificate.get()));
        if (!public_key_ptr.get()) {
            EVLOG_error << "Error during X509_get_pubkey";
            return false;
        }

        // decode base64 encoded signature
        EVP_ENCODE_CTX_ptr base64_decode_context_ptr(EVP_ENCODE_CTX_new());
        if (!base64_decode_context_ptr.get()) {
            EVLOG_error << "Error during EVP_ENCODE_CTX_new";
            return false;
        }
        EVP_DecodeInit(base64_decode_context_ptr.get());
        if (!base64_decode_context_ptr.get()) {
            EVLOG_error << "Error during EVP_DecodeInit";
            return false;
        }
        const unsigned char* signature_str = reinterpret_cast<const unsigned char*>(signature.data());
        int base64_length = signature.size();
        unsigned char signature_out[base64_length];
        int signature_out_length;
        if (EVP_DecodeUpdate(base64_decode_context_ptr.get(), signature_out, &signature_out_length, signature_str,
                             base64_length) < 0) {
            EVLOG_error << "Error during DecodeUpdate";
            return false;
        }
        int decode_final_out;
        if (EVP_DecodeFinal(base64_decode_context_ptr.get(), signature_out, &decode_final_out) < 0) {
            EVLOG_error << "Error during EVP_DecodeFinal";
            return false;
        }

        // verify file signature
        EVP_PKEY_CTX_ptr public_key_context_ptr(EVP_PKEY_CTX_new(public_key_ptr.get(), nullptr));
        if (!public_key_context_ptr.get()) {
            EVLOG_error << "Error setting up public key context";
        }
        if (EVP_PKEY_verify_init(public_key_context_ptr.get()) <= 0) {
            EVLOG_error << "Error during EVP_PKEY_verify_init";
        }
        if (EVP_PKEY_CTX_set_signature_md(public_key_context_ptr.get(), EVP_sha256()) <= 0) {
            EVLOG_error << "Error during EVP_PKEY_CTX_set_signature_md";
        };
        int result =
            EVP_PKEY_verify(public_key_context_ptr.get(), reinterpret_cast<const unsigned char*>(signature_out),
                            signature_out_length, sha256_out, sha256_out_length);

        EVP_cleanup();

        if (result != 1) {
            EVLOG_error << "Failure to verify: " << result;
            return false;
        } else {
            EVLOG_error << "Succesful verification";
            return true;
        }
    } catch (const CertificateLoadException& e) {
        EVLOG_error << "Could not parse signing certificate: " << e.what();
        return false;
    }

    return false;
}

InstallCertificateResult EvseSecurity::verify_certificate(const std::string& certificate_chain,
                                                          LeafCertificateType certificate_type) {
    try {
        X509CertificateBundle certificate(certificate_chain, EncodingFormat::PEM);
        std::vector<X509Wrapper> _certificate_chain = certificate.split();
        if (_certificate_chain.empty()) {
            return InstallCertificateResult::InvalidFormat;
        }

        const auto leaf_certificate = _certificate_chain.at(0);

        X509_STORE_ptr store_ptr(X509_STORE_new());
        X509_STORE_CTX_ptr store_ctx_ptr(X509_STORE_CTX_new());

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
