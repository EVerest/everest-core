// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <evse_security/certificate/x509_hierarchy.hpp>

#include <algorithm>

namespace evse_security {

bool X509CertificateHierarchy::is_root(const X509Wrapper& certificate) const {
    if (certificate.is_selfsigned()) {
        return (std::find_if(hierarchy.begin(), hierarchy.end(), [&certificate](const X509Node& node) {
                    return node.certificate == certificate;
                }) != hierarchy.end());
    }

    return false;
}

std::vector<X509Wrapper> X509CertificateHierarchy::collect_descendants(const X509Wrapper& top) {
    std::vector<X509Wrapper> descendants;

    for_each([&](const X509Node& node) {
        // If we found the certificate
        if (node.certificate == top) {
            // Collect all descendants
            if (node.children.size()) {
                for_each_descendant(
                    [&](const X509Node& descendant, int depth) { descendants.push_back(descendant.certificate); },
                    node);
            }

            return false;
        }

        return true;
    });

    return descendants;
}

CertificateHashData X509CertificateHierarchy::get_certificate_hash(const X509Wrapper& certificate) {
    if (certificate.is_selfsigned()) {
        return certificate.get_certificate_hash_data();
    }

    // Search for certificate in the hierarchy and return the hash
    CertificateHashData hash;
    bool found;

    for_each([&](const X509Node& node) {
        if (node.certificate == certificate) {
            hash = node.hash;
            found = true;

            return false;
        }

        return true;
    });

    if (found)
        return hash;

    throw NoCertificateFound("Could not find owner for certificate: " + certificate.get_common_name());
}

bool X509CertificateHierarchy::contains_certificate_hash(const CertificateHashData& hash) {
    bool contains = false;

    for_each([&](const X509Node& node) {
        if (node.hash == hash) {
            contains = true;
            return false;
        }

        return true;
    });

    return contains;
}

X509Wrapper X509CertificateHierarchy::find_certificate(const CertificateHashData& hash) {
    X509Wrapper* certificate = nullptr;

    for_each([&](X509Node& node) {
        if (node.hash == hash) {
            certificate = &node.certificate;
            return false;
        }

        return true;
    });

    if (certificate)
        return *certificate;

    throw NoCertificateFound("Could not find a certificate for hash: " + hash.issuer_name_hash);
}

std::vector<X509Wrapper> X509CertificateHierarchy::find_certificates_multi(const CertificateHashData& hash) {
    std::vector<X509Wrapper> certificates;

    for_each([&](X509Node& node) {
        if (node.hash == hash) {
            certificates.push_back(node.certificate);
        }

        return true;
    });

    return certificates;
}

std::string X509CertificateHierarchy::to_debug_string() {
    std::stringstream str;

    for (const auto& root : hierarchy) {
        if (root.state.is_selfsigned)
            str << "* [ROOT]";
        else
            str << "+ [ORPH]";

        str << ' ' << root.certificate.get_common_name() << std::endl;

        for_each_descendant(
            [&](const X509Node& node, int depth) {
                while (depth-- > 0)
                    str << "---";

                str << ' ' << node.certificate.get_common_name() << std::endl;
            },
            root, 1);
    }

    return str.str();
}

void X509CertificateHierarchy::insert(X509Wrapper&& inserted_certificate) {
    // Invalid hash
    static CertificateHashData invalid_hash{HashAlgorithm::SHA256, {}, {}, {}};

    if (false == inserted_certificate.is_selfsigned()) {
        // If this certif has any link to any of the existing certificates
        bool hierarchy_found = false;

        // Create a new node, is not self-signed and is not a permanent orphan
        X509Node new_node = {{0, 0, 0}, inserted_certificate, invalid_hash, inserted_certificate, {}};

        // Search through all the list for a link
        for_each([&](X509Node& top) {
            if (top.certificate.is_child(inserted_certificate)) {
                // Some sanity checks
                if (top.state.is_selfsigned) {
                    throw InvalidStateException(
                        "Newly added certificate can't be parent of a self-signed certificate!");
                }

                if (top.state.is_hash_computed) {
                    throw InvalidStateException("Existing non-root top certificate can't have a valid hash!");
                }

                // If the top certificate is a descendant of the certificate we're adding

                // Cache top node
                auto temp_top = std::move(top);

                // Set the new state of the top node
                temp_top.state = {0, 0, 1};
                temp_top.hash = temp_top.certificate.get_certificate_hash_data(new_node.certificate);
                temp_top.issuer = X509Wrapper(new_node.certificate);

                // Set the top as a child of the new_node
                new_node.children.push_back(std::move(temp_top));

                // Set the new top
                top = std::move(new_node);
                hierarchy_found = true; // Found a link
            } else if (inserted_certificate.is_child(top.certificate)) {
                // If the certificate is the descendant of top certificate

                // Calculate hash and set issuer
                new_node.state = {0, 0, 1};
                new_node.hash = inserted_certificate.get_certificate_hash_data(top.certificate);
                new_node.issuer = X509Wrapper(top.certificate); // Set the new issuer

                // Add it to the top's descendant list
                top.children.push_back(new_node);
                hierarchy_found = true; // Found a link
            }

            // Keep iterating while we did not find a link
            return (false == hierarchy_found);
        });

        // Else insert it in the roots as a potentially orphan certificate
        if (hierarchy_found == false) {
            hierarchy.push_back(new_node);
        }
    } else {
        // If it is self-signed insert it in the roots, with the state set as a self-signed and a properly computed hash
        hierarchy.push_back({{1, 0, 1},
                             inserted_certificate,
                             inserted_certificate.get_certificate_hash_data(),
                             inserted_certificate,
                             {}});

        // Attempt a partial prune, by searching through all the contained temporary orphan certificates
        // and trying to add them to the newly inserted root certificate, if that is possible

        // Only iterate until last (not including) since the last is the new node
        for (int i = 0; i < (hierarchy.size() - 1); ++i) {
            auto& node = hierarchy[i];
            auto& state = node.state;

            // If we have a temporary orphan
            if (state.is_selfsigned == 0) {
                // Some sanity checks
                if (state.is_hash_computed)
                    throw InvalidStateException("Orphan certificate can't have a proper hash!");

                // If it is a child of the new root certificate insert it to it's list and break
                if (node.certificate.is_child(inserted_certificate)) {
                    auto& new_root = hierarchy.back();

                    // Hash is properly computed now
                    node.hash = node.certificate.get_certificate_hash_data(new_root.certificate);
                    node.state.is_hash_computed = 1;
                    node.state.is_orphan = 0;                        // Not an orphan any more
                    node.issuer = X509Wrapper(inserted_certificate); // Set the new valid issuer

                    // Add to the newly inserted root child list
                    new_root.children.push_back(std::move(node));

                    // Erase element since it was added to the root's descendants
                    hierarchy.erase(hierarchy.begin() + i);
                    // Decrement i again since it was erased
                    i--;
                }
            }
        }
    }
} // End insert

void X509CertificateHierarchy::prune() {
    if (hierarchy.size() <= 1)
        return;

    for (int i = 0; i < hierarchy.size(); ++i) {
        // Possible orphan
        auto& orphan = hierarchy[i];

        bool is_orphan = (orphan.state.is_selfsigned) == 0 && (orphan.state.is_orphan == 0);
        if (is_orphan == false)
            continue;

        // Found a non-permanent orphan, search for a issuer
        bool found_issuer = false;

        for_each([&](X509Node& top) {
            if (orphan.certificate.is_child(top.certificate)) {
                orphan.hash = orphan.certificate.get_certificate_hash_data(top.certificate);
                orphan.state.is_hash_computed = 1;
                orphan.state.is_orphan = 0;                   // Not an orphan any more
                orphan.issuer = X509Wrapper(top.certificate); // Set the new valid issuer

                top.children.push_back(std::move(orphan));
                found_issuer = true;
            }

            return (false == found_issuer);
        });

        if (false == found_issuer) {
            // Mark as permanent orphan
            orphan.state.is_orphan = 1; // Permanent orphan
            orphan.state.is_hash_computed = 0;
        } else {
            // Erase from hierarchy list and decrement iterator
            hierarchy.erase(std::begin(hierarchy) + i);
            i--;
        }
    }
}

X509CertificateHierarchy X509CertificateHierarchy::build_hierarchy(std::vector<X509Wrapper>& certificates) {
    X509CertificateHierarchy ordered;

    while (certificates.size()) {
        ordered.insert(std::move(certificates.back()));
        certificates.pop_back();
    }

    // Prune the tree
    ordered.prune();

    return ordered;
}

} // namespace evse_security
