// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <evse_security/evse_security.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <stdio.h>

#include <evse_security/certificate/x509_bundle.hpp>
#include <evse_security/certificate/x509_hierarchy.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/crypto/evse_crypto.hpp>
#include <evse_security/utils/evse_filesystem.hpp>

namespace evse_security {

static InstallCertificateResult to_install_certificate_result(CertificateValidationError error) {
    switch (error) {
    case CertificateValidationError::Expired:
        EVLOG_warning << "Certificate has expired";
        return InstallCertificateResult::Expired;
    case CertificateValidationError::InvalidSignature:
        EVLOG_warning << "Invalid signature";
        return InstallCertificateResult::InvalidSignature;
    case CertificateValidationError::InvalidChain:
        EVLOG_warning << "Invalid certificate chain";
        return InstallCertificateResult::InvalidCertificateChain;
    case CertificateValidationError::InvalidLeafSignature:
        EVLOG_warning << "Unable to verify leaf signature";
        return InstallCertificateResult::InvalidSignature;
    case CertificateValidationError::IssuerNotFound:
        EVLOG_warning << "Issuer not found";
        return InstallCertificateResult::InvalidCertificateChain;
    default:
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

static fs::path get_private_key_path(const X509Wrapper& certificate, const fs::path& key_path,
                                     const std::optional<std::string> password) {
    // TODO(ioan): Before iterating the whole dir check by the filename first 'key_path'.key
    for (const auto& entry : fs::recursive_directory_iterator(key_path)) {
        if (fs::is_regular_file(entry)) {
            auto key_file_path = entry.path();
            if (key_file_path.extension() == KEY_EXTENSION) {
                try {
                    fsstd::ifstream file(key_file_path, std::ios::binary);
                    std::string private_key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                    if (CryptoSupplier::x509_check_private_key(certificate.get(), private_key, password)) {
                        EVLOG_debug << "Key found for certificate at path: " << key_file_path;
                        return key_file_path;
                    }
                } catch (const std::exception& e) {
                    EVLOG_debug << "Could not load or verify private key at: " << key_file_path << ": " << e.what();
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

X509CertificateBundle get_leaf_certificates(const fs::path& cert_dir) {
    return X509CertificateBundle(cert_dir, EncodingFormat::PEM);
}

EvseSecurity::EvseSecurity(const FilePaths& file_paths, const std::optional<std::string>& private_key_password) :
    private_key_password(private_key_password) {

    std::vector<fs::path> dirs = {
        file_paths.directories.csms_leaf_cert_directory,
        file_paths.directories.csms_leaf_key_directory,
        file_paths.directories.secc_leaf_cert_directory,
        file_paths.directories.secc_leaf_key_directory,
    };

    for (const auto& path : dirs) {
        if (!fs::exists(path)) {
            EVLOG_warning << "Could not find configured leaf directory at: " << path.string()
                          << " creating default dir!";
            if (!fs::create_directories(path)) {
                EVLOG_error << "Could not create default dir for path: " << path.string();
            }
        } else if (!fs::is_directory(path)) {
            throw std::runtime_error(path.string() + " is not a directory.");
        }
    }

    this->ca_bundle_path_map[CaCertificateType::CSMS] = file_paths.csms_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::MF] = file_paths.mf_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::MO] = file_paths.mo_ca_bundle;
    this->ca_bundle_path_map[CaCertificateType::V2G] = file_paths.v2g_ca_bundle;

    for (const auto& pair : this->ca_bundle_path_map) {
        if (!fs::exists(pair.second)) {
            EVLOG_warning << "Could not find configured " << conversions::ca_certificate_type_to_string(pair.first)
                          << " bundle file at: " + pair.second.string() << ", creating default!";
            if (!filesystem_utils::create_file_if_nonexistent(pair.second)) {
                EVLOG_error << "Could not create default bundle for path: " << pair.second;
            }
        }
    }

    this->directories = file_paths.directories;
    this->links = file_paths.links;
}

EvseSecurity::~EvseSecurity() {
}

InstallCertificateResult EvseSecurity::install_ca_certificate(const std::string& certificate,
                                                              CaCertificateType certificate_type) {
    EVLOG_debug << "Installing ca certificate: " << conversions::ca_certificate_type_to_string(certificate_type);

    // TODO(piet): Check CertificateStoreMaxEntries
    try {
        X509Wrapper new_cert(certificate, EncodingFormat::PEM);

        if (!new_cert.is_valid()) {
            return InstallCertificateResult::Expired;
        }

        // Load existing
        const auto ca_bundle_path = this->ca_bundle_path_map.at(certificate_type);

        if (!fs::is_directory(ca_bundle_path)) {
            // Ensure file exists
            filesystem_utils::create_file_if_nonexistent(ca_bundle_path);
        }

        X509CertificateBundle existing_certs(ca_bundle_path, EncodingFormat::PEM);

        if (existing_certs.is_using_directory()) {
            std::string filename = conversions::ca_certificate_type_to_string(certificate_type) + "_" +
                                   filesystem_utils::get_random_file_name(PEM_EXTENSION.string());
            fs::path new_path = ca_bundle_path / filename;

            // Sets the path of the new certificate
            new_cert.set_file(new_path);
        }

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
    EVLOG_debug << "Delete ca certificate: " << certificate_hash_data.serial_number;

    auto response = DeleteCertificateResult::NotFound;

    bool found_certificate = false;
    bool failed_to_write = false;

    // TODO (ioan): load all the bundles since if it's the V2G root in that case we might have to delete
    // whole hierarchies
    for (auto const& [certificate_type, ca_bundle_path] : ca_bundle_path_map) {
        try {
            X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);

            if (ca_bundle.delete_certificate(certificate_hash_data, true)) {
                found_certificate = true;
                if (!ca_bundle.export_certificates()) {
                    failed_to_write = true;
                }
            }

        } catch (const CertificateLoadException& e) {
            EVLOG_warning << "Could not load ca bundle from file: " << ca_bundle_path;
        }
    }

    for (const auto& leaf_certificate_path :
         {directories.secc_leaf_cert_directory, directories.csms_leaf_cert_directory}) {
        try {
            bool secc = (leaf_certificate_path == directories.secc_leaf_cert_directory);
            bool csms = (leaf_certificate_path == directories.csms_leaf_cert_directory) ||
                        (directories.csms_leaf_cert_directory == directories.secc_leaf_cert_directory);

            CaCertificateType load;

            if (secc)
                load = CaCertificateType::V2G;
            else if (csms)
                load = CaCertificateType::CSMS;

            // Also load the roots since we need to build the hierarchy for correct certificate hashes
            X509CertificateBundle root_bundle(ca_bundle_path_map[load], EncodingFormat::PEM);
            X509CertificateBundle leaf_bundle(leaf_certificate_path, EncodingFormat::PEM);

            auto full_list = root_bundle.split();
            for (X509Wrapper& certif : leaf_bundle.split()) {
                full_list.push_back(std::move(certif));
            }

            X509CertificateHierarchy hierarchy = X509CertificateHierarchy::build_hierarchy(full_list);

            EVLOG_debug << "Delete hierarchy:(" << leaf_certificate_path.string() << ")\n"
                        << hierarchy.to_debug_string();

            try {
                X509Wrapper to_delete = hierarchy.find_certificate(certificate_hash_data);

                if (leaf_bundle.delete_certificate(to_delete, true)) {
                    found_certificate = true;

                    if (csms) {
                        // Per M04.FR.06 we are not allowed to delete the CSMS (ChargingStationCertificate), we should
                        // return 'Failed'
                        failed_to_write = true;
                        EVLOG_error << "Error, not allowed to delete ChargingStationCertificate: "
                                    << to_delete.get_common_name();
                    } else if (!leaf_bundle.export_certificates()) {
                        failed_to_write = true;
                        EVLOG_error << "Error removing leaf certificate: " << certificate_hash_data.issuer_name_hash;
                    }
                }
            } catch (NoCertificateFound& e) {
                // Ignore, case is handled later
            }
        } catch (const CertificateLoadException& e) {
            EVLOG_warning << "Could not load ca bundle from file: " << leaf_certificate_path;
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
    fs::path cert_path;
    fs::path key_path;
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
        const auto file_name =
            std::string("SECC_LEAF_") + filesystem_utils::get_random_file_name(PEM_EXTENSION.string());
        const auto file_path = cert_path / file_name;
        std::string str_cert = leaf_certificate.get_export_string();

        // Also write chain to file
        const auto chain_file_name =
            std::string("CPO_CERT_CHAIN_") + filesystem_utils::get_random_file_name(PEM_EXTENSION.string());
        const auto chain_file_path = cert_path / chain_file_name;
        std::string str_chain_cert = chain_certificate.to_export_string();

        if (filesystem_utils::write_to_file(file_path, str_cert, std::ios::out) &&
            filesystem_utils::write_to_file(chain_file_path, str_chain_cert, std::ios::out)) {
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
            X509CertificateHierarchy& hierarchy = ca_bundle.get_certficate_hierarchy();

            EVLOG_debug << "Hierarchy:(" << conversions::ca_certificate_type_to_string(ca_certificate_type) << ")\n"
                        << hierarchy.to_debug_string();

            // Iterate the hierarchy and add all the certificates to their respective locations
            for (auto& root : hierarchy.get_hierarchy()) {
                CertificateHashDataChain certificate_hash_data_chain;

                certificate_hash_data_chain.certificate_type =
                    get_certificate_type(ca_certificate_type); // We always know type
                certificate_hash_data_chain.certificate_hash_data = root.hash;

                // Add all owned children/certificates in order
                X509CertificateHierarchy::for_each_descendant(
                    [&certificate_hash_data_chain](const X509Node& child, int depth) {
                        certificate_hash_data_chain.child_certificate_hash_data.push_back(child.hash);
                    },
                    root);

                // Add to our chains
                certificate_chains.push_back(certificate_hash_data_chain);
            }
        } catch (const CertificateLoadException& e) {
            EVLOG_warning << "Could not load CA bundle file at: " << ca_bundle_path << " error: " << e.what();
        }
    }

    // retrieve v2g certificate chain
    if (std::find(certificate_types.begin(), certificate_types.end(), CertificateType::V2GCertificateChain) !=
        certificate_types.end()) {

        const auto secc_key_pair = this->get_key_pair(LeafCertificateType::V2G, EncodingFormat::PEM);
        if (secc_key_pair.status == GetKeyPairStatus::Accepted) {
            // Leaf V2G chain
            X509CertificateBundle leaf_bundle(secc_key_pair.pair.value().certificate, EncodingFormat::PEM);

            // V2G chain
            const auto ca_bundle_path = this->ca_bundle_path_map.at(CaCertificateType::V2G);
            X509CertificateBundle ca_bundle(ca_bundle_path, EncodingFormat::PEM);

            // Merge the bundles
            for (auto& certif : leaf_bundle.split()) {
                ca_bundle.add_certificate_unique(std::move(certif));
            }

            // Create the certificate hierarchy
            X509CertificateHierarchy& hierarchy = ca_bundle.get_certficate_hierarchy();
            EVLOG_debug << "Hierarchy:(V2GCertificateChain)\n" << hierarchy.to_debug_string();

            for (auto& root : hierarchy.get_hierarchy()) {
                CertificateHashDataChain certificate_hash_data_chain;

                certificate_hash_data_chain.certificate_type = CertificateType::V2GCertificateChain;

                // Since the hierarchy starts with V2G and SubCa1/SubCa2 we have to add:
                // the leaf as the first when returning
                // * Leaf
                // --- SubCa1
                // --- SubCa2
                std::vector<CertificateHashData> hierarchy_hash_data;

                X509CertificateHierarchy::for_each_descendant(
                    [&](const X509Node& child, int depth) { hierarchy_hash_data.push_back(child.hash); }, root);

                if (hierarchy_hash_data.size()) {
                    // Leaf is the last
                    certificate_hash_data_chain.certificate_hash_data = hierarchy_hash_data.back();
                    hierarchy_hash_data.pop_back();

                    // Add others in order, except last
                    for (const auto& hash_data : hierarchy_hash_data) {
                        certificate_hash_data_chain.child_certificate_hash_data.push_back(hash_data);
                    }

                    // Add to our chains
                    certificate_chains.push_back(certificate_hash_data_chain);
                }
            }
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

int EvseSecurity::get_count_of_installed_certificates(const std::vector<CertificateType>& certificate_types) {
    int count = 0;

    std::set<fs::path> directories;
    const auto ca_certificate_types = get_ca_certificate_types(certificate_types);

    // Collect unique directories
    for (const auto& ca_certificate_type : ca_certificate_types) {
        directories.emplace(this->ca_bundle_path_map.at(ca_certificate_type));
    }

    for (const auto& unique_dir : directories) {
        X509CertificateBundle ca_bundle(unique_dir, EncodingFormat::PEM);
        count += ca_bundle.get_certificate_count();
    }

    // V2G Chain
    if (std::find(certificate_types.begin(), certificate_types.end(), CertificateType::V2GCertificateChain) !=
        certificate_types.end()) {
        auto leaf_dir = this->directories.secc_leaf_cert_directory;

        // Load all from chain, including expired/unused
        X509CertificateBundle leaf_bundle(leaf_dir, EncodingFormat::PEM);
        count += leaf_bundle.get_certificate_count();
    }

    return count;
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
                EVLOG_debug << "Writing OCSP Response to filesystem";
                if (!cert.get_file().has_value()) {
                    continue;
                }
                const auto ocsp_path = cert.get_file().value().parent_path() / "ocsp";
                if (!fs::exists(ocsp_path)) {
                    fs::create_directories(ocsp_path);
                }
                const auto ocsp_file_path =
                    ocsp_path / cert.get_file().value().filename().replace_extension(".ocsp.der");
                std::ofstream fs(ocsp_file_path.c_str());
                fs << ocsp_response;
                fs.close();
            }
        }
    } catch (const CertificateLoadException& e) {
        EVLOG_error << "Could not update ocsp cache, certificate load failure!";
    }
}

bool EvseSecurity::is_ca_certificate_installed(CaCertificateType certificate_type) {
    try {
        X509CertificateBundle bundle(this->ca_bundle_path_map.at(certificate_type), EncodingFormat::PEM);

        // Search for a valid self-signed root
        auto& hierarchy = bundle.get_certficate_hierarchy();

        // Get all roots and search for a valid self-signed
        for (auto& root : hierarchy.get_hierarchy()) {
            if (root.certificate.is_selfsigned() && root.certificate.is_valid())
                return true;
        }
    } catch (const CertificateLoadException& e) {
        EVLOG_error << "Could not load ca certificate type:"
                    << conversions::ca_certificate_type_to_string(certificate_type);
        return false;
    }

    return false;
}

std::string EvseSecurity::generate_certificate_signing_request(LeafCertificateType certificate_type,
                                                               const std::string& country,
                                                               const std::string& organization,
                                                               const std::string& common, bool use_tpm) {
    fs::path key_path;

    const auto file_name = std::string("SECC_LEAF_") + filesystem_utils::get_random_file_name(KEY_EXTENSION.string());
    if (certificate_type == LeafCertificateType::CSMS) {
        key_path = this->directories.csms_leaf_key_directory / file_name;
    } else if (certificate_type == LeafCertificateType::V2G) {
        key_path = this->directories.secc_leaf_key_directory / file_name;
    } else {
        throw std::runtime_error("Attempt to generate CSR for MF certificate");
    }

    std::string csr;
    CertificateSigningRequestInfo info;

    info.n_version = 0;
    info.commonName = common;
    info.country = country;
    info.organization = organization;

    info.key_info.key_type = CryptoKeyType::EC_prime256v1;
    info.key_info.generate_on_tpm = use_tpm;
    info.key_info.private_key_file = key_path;

    if ((use_tpm == false) && private_key_password.has_value()) {
        info.key_info.private_key_pass = private_key_password;
    }

    if (false == CryptoSupplier::x509_generate_csr(info, csr)) {
        throw std::runtime_error("Failed to generate certificate signing request!");
    }

    EVLOG_debug << csr;
    return csr;
}

std::string EvseSecurity::generate_certificate_signing_request(LeafCertificateType certificate_type,
                                                               const std::string& country,
                                                               const std::string& organization,
                                                               const std::string& common) {
    return generate_certificate_signing_request(certificate_type, country, organization, common, false);
}

GetKeyPairResult EvseSecurity::get_key_pair(LeafCertificateType certificate_type, EncodingFormat encoding) {
    EVLOG_debug << "Requesting key/pair: " << conversions::leaf_certificate_type_to_string(certificate_type);

    GetKeyPairResult result;
    result.pair = std::nullopt;

    fs::path key_dir;
    fs::path cert_dir;

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

        fs::path key_file;
        fs::path certificate_file;
        fs::path chain_file;

        auto certificate = std::move(leaf_certificates.get_latest_valid_certificate());
        auto private_key_path = get_private_key_path(certificate, key_dir, this->private_key_password);

        // Key path doesn't change
        key_file = private_key_path;

        X509CertificateBundle leaf_directory(cert_dir, EncodingFormat::PEM);

        const std::vector<X509Wrapper>* leaf_fullchain = nullptr;
        const std::vector<X509Wrapper>* leaf_single = nullptr;

        // We are searching for both the full leaf bundle, containing the leaf and the cso1/2 and the single leaf
        // without the cso1/2
        leaf_directory.for_each_chain([&](const std::filesystem::path& path, const std::vector<X509Wrapper>& chain) {
            // If we contain the latest valid, we found our generated bundle
            bool bFound = (std::find(chain.begin(), chain.end(), certificate) != chain.end());

            if (bFound) {
                if (chain.size() > 1) {
                    leaf_fullchain = &chain;
                } else if (chain.size() == 1) {
                    leaf_single = &chain;
                }
            }

            // Found both, break
            if (leaf_fullchain != nullptr && leaf_single != nullptr)
                return false;

            return true;
        });

        if (leaf_fullchain != nullptr) {
            chain_file = leaf_fullchain->at(0).get_file().value();
        } else {
            EVLOG_warning << "V2G leaf requires full bundle, but full bundle not found at path: " << cert_dir;
        }

        if (leaf_single != nullptr) {
            certificate_file = leaf_single->at(0).get_file().value();
        } else {
            EVLOG_warning << "V2G single leaf not found at path: " << cert_dir;
        }

        result.pair = {key_file, chain_file, certificate_file, this->private_key_password};
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

bool EvseSecurity::update_certificate_links(LeafCertificateType certificate_type) {
    bool changed = false;

    if (certificate_type != LeafCertificateType::V2G) {
        throw std::runtime_error("Link updating only supported for V2G certificates");
    }

    fs::path cert_link_path = this->links.secc_leaf_cert_link;
    fs::path key_link_path = this->links.secc_leaf_key_link;
    fs::path chain_link_path = this->links.cpo_cert_chain_link;

    // Get the most recent valid certificate
    const auto key_pair = this->get_key_pair(certificate_type, EncodingFormat::PEM);
    if ((key_pair.status == GetKeyPairStatus::Accepted) && key_pair.pair.has_value()) {

        // Create or update symlinks to SECC leaf cert
        if (!cert_link_path.empty()) {
            fs::path cert_path = key_pair.pair.value().certificate_single;
            if (fs::is_symlink(cert_link_path)) {
                if (fs::read_symlink(cert_link_path) != cert_path) {
                    fs::remove(cert_link_path);
                    changed = true;
                }
            }
            if (!fs::exists(cert_link_path)) {
                EVLOG_debug << "SECC cert link: " << cert_link_path << " -> " << cert_path;
                fs::create_symlink(cert_path, cert_link_path);
                changed = true;
            }
        }

        // Create or update symlinks to SECC leaf key
        if (!key_link_path.empty()) {
            fs::path key_path = key_pair.pair.value().key;
            if (fs::is_symlink(key_link_path)) {
                if (fs::read_symlink(key_link_path) != key_path) {
                    fs::remove(key_link_path);
                    changed = true;
                }
            }
            if (!fs::exists(key_link_path)) {
                EVLOG_debug << "SECC key link: " << key_link_path << " -> " << key_path;
                fs::create_symlink(key_path, key_link_path);
                changed = true;
            }
        }

        // Create or update symlinks to CPO chain
        fs::path chain_path = key_pair.pair.value().certificate;
        if (!chain_link_path.empty()) {
            if (fs::is_symlink(chain_link_path)) {
                if (fs::read_symlink(chain_link_path) != chain_path) {
                    fs::remove(chain_link_path);
                    changed = true;
                }
            }
            if (!fs::exists(chain_link_path)) {
                EVLOG_debug << "CPO cert chain link: " << chain_link_path << " -> " << chain_path;
                fs::create_symlink(chain_path, chain_link_path);
                changed = true;
            }
        }
    } else {
        // Remove existing symlinks if no valid certificate is found
        if (!cert_link_path.empty() && fs::is_symlink(cert_link_path)) {
            fs::remove(cert_link_path);
            changed = true;
        }
        if (!key_link_path.empty() && fs::is_symlink(key_link_path)) {
            fs::remove(key_link_path);
            changed = true;
        }
        if (!chain_link_path.empty() && fs::is_symlink(chain_link_path)) {
            fs::remove(chain_link_path);
            changed = true;
        }
    }

    return changed;
}

std::string EvseSecurity::get_verify_file(CaCertificateType certificate_type) {
    // Support bundle files, in case the certificates contain
    // multiple entries (should be 3) as per the specification
    X509CertificateBundle verify_file(this->ca_bundle_path_map.at(certificate_type), EncodingFormat::PEM);

    EVLOG_debug << "Requesting certificate file: [" << conversions::ca_certificate_type_to_string(certificate_type)
                << "] file:" << verify_file.get_path();

    // If we are using a directory, search for the first valid root file
    if (verify_file.is_using_directory()) {
        auto& hierarchy = verify_file.get_certficate_hierarchy();

        // Get all roots and search for a valid self-signed
        for (auto& root : hierarchy.get_hierarchy()) {
            if (root.certificate.is_selfsigned() && root.certificate.is_valid())
                return root.certificate.get_file().value_or("");
        }
    }

    return verify_file.get_path().string();
}

int EvseSecurity::get_leaf_expiry_days_count(LeafCertificateType certificate_type) {
    EVLOG_debug << "Requesting certificate expiry: " << conversions::leaf_certificate_type_to_string(certificate_type);

    const auto key_pair = this->get_key_pair(certificate_type, EncodingFormat::PEM);
    if (key_pair.status == GetKeyPairStatus::Accepted) {
        // In case it is a bundle, we know the leaf is always the first
        X509CertificateBundle cert(key_pair.pair.value().certificate, EncodingFormat::PEM);

        int64_t seconds = cert.split().at(0).get_valid_to();
        return std::chrono::duration_cast<days_to_seconds>(std::chrono::seconds(seconds)).count();
    }
    return 0;
}

bool EvseSecurity::verify_file_signature(const fs::path& path, const std::string& signing_certificate,
                                         const std::string signature) {
    EVLOG_debug << "Verifying file signature for " << path.string();

    std::vector<std::byte> sha256_digest;

    if (false == CryptoSupplier::digest_file_sha256(path, sha256_digest)) {
        EVLOG_error << "Error during digesting file: " << path;
        return false;
    }

    std::vector<std::byte> signature_decoded;

    if (false == CryptoSupplier::decode_base64_signature(signature, signature_decoded)) {
        EVLOG_error << "Error during decoding signature: " << signature;
        return false;
    }

    try {
        X509Wrapper x509_signing_cerificate(signing_certificate, EncodingFormat::PEM);

        if (CryptoSupplier::x509_verify_signature(x509_signing_cerificate.get(), signature_decoded, sha256_digest)) {
            EVLOG_debug << "Signature successful verification";
            return true;
        } else {
            EVLOG_error << "Failure to verify signature";
            return false;
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
        std::vector<X509Handle*> parent_certificates;
        std::optional<fs::path> store_file;

        for (size_t i = 1; i < _certificate_chain.size(); i++) {
            parent_certificates.emplace_back(_certificate_chain[i].get());
        }

        if (certificate_type == LeafCertificateType::CSMS) {
            store_file = this->ca_bundle_path_map.at(CaCertificateType::CSMS);
        } else if (certificate_type == LeafCertificateType::V2G) {
            store_file = this->ca_bundle_path_map.at(CaCertificateType::V2G);
        } else {
            store_file = this->ca_bundle_path_map.at(CaCertificateType::MF);
        }

        CertificateValidationError validated = CryptoSupplier::x509_verify_certificate_chain(
            leaf_certificate.get(), parent_certificates, true, std::nullopt, store_file);

        if (validated != CertificateValidationError::NoError) {
            return to_install_certificate_result(validated);
        }

        return InstallCertificateResult::Accepted;
    } catch (const CertificateLoadException& e) {
        EVLOG_warning << "Could not load update leaf certificate because of invalid format";
        return InstallCertificateResult::InvalidFormat;
    }
}

} // namespace evse_security
