// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef X509_HIERARCHY_HPP
#define X509_HIERARCHY_HPP

#include <queue>

#include <x509_wrapper.hpp>

namespace evse_security {

struct X509Node {
    X509Wrapper certificate;
    CertificateHashData hash;

    std::vector<X509Node> children;
};

/// @brief Utility class that is able to build a certificate hierarchy, with a 
/// list of self-signed root certificates and their respective sub-certificates
class X509CertificateHierarchy {
public:
    const std::vector<X509Node> &get_hierarchy() const {
        return hierarchy;
    }

    /// @brief Checks if the provided certificate is a self-signed root CA certificate
    /// contained in our hierarchy
    bool is_root(const X509Wrapper &certificate) const {
        if(certificate.is_selfsigned()) {
            return (std::find_if(hierarchy.begin(), hierarchy.end(), [&certificate](const X509Node &node) {
                return node.certificate == certificate;
            }) != hierarchy.end());
        }

        return false;
    }

    /// @brief Iterate through all the hierarchy of certificates
    template <typename function> void for_each(function func) {
        for (auto &root : hierarchy) {
            func(root, 0);
            for_each_child(func, root, 1);
        }
    }

    std::string to_debug_string() {
        std::ostringstream str;

        for_each ([&str](const X509Node &node, int depth) {
            if(depth == 0) {
                str << '*';
            } else {
                while(depth-- > 0)
                    str << "---";
            }

            str << ' ' << node.certificate.get_common_name() << ", issuer name hash: " << node.hash.issuer_name_hash << ", issuer key hash: " << node.hash.issuer_key_hash << std::endl;
        });

        return str.str();
    }

public:
    template <typename function> static void for_each_child(function func, const X509Node &node, int depth = 0) {
        if (node.children.empty())
            return;

        for (const auto &child : node.children) {
            func(child, depth);

            if (!child.children.empty()) {
                for_each_child(func, child, (depth + 1));
            }
        }
    }

    static X509CertificateHierarchy build_hierarchy(std::vector<X509Wrapper> &certificates)
    {
        X509CertificateHierarchy ordered;        
        
        // Search for all self-signed certificates and add them as the roots, and also search for
        // all owner-less certificates and also add them as root, since they are  not self-signed
        // but we can't find any owner for them anyway
        std::for_each(certificates.begin(), certificates.end(), [&](const X509Wrapper &certif) {
            if (certif.is_selfsigned()) {
                ordered.hierarchy.push_back({std::move(certif), certif.get_certificate_hash_data(), {}});
            } else {
                // Search for a possible owner
                bool has_owner = std::find_if(certificates.begin(), certificates.end(), [&](const X509Wrapper &owner) {
                    return certif.is_child(owner);
                }) != certificates.end();

                if (!has_owner) {
                    auto hash = certif.get_certificate_hash_data();
                    ordered.hierarchy.push_back({std::move(certif), hash, {}});
                }
            }
        });

        // Remove all root certificates from the provided certificate list
        auto remove_roots = std::remove_if(certificates.begin(), certificates.end(), [&](const X509Wrapper &certif) {
            return std::find_if(ordered.hierarchy.begin(), ordered.hierarchy.end(), [&](const X509Node &node) {
                return (certif == node.hash);
            }) != ordered.hierarchy.end();
        });
        
        certificates.erase(remove_roots, certificates.end());

        // Try build the full hierarchy, we are not assuming any order
        while(certificates.size()) {
            // The current certificate that we're testing for
            auto current = std::move(certificates.back());
            certificates.pop_back();

            // If we have any roots try and search in the hierarchy the owner
            if (!ordered.try_add_to_hierarchy(std::move(current))) {
                // If we couldn't add to the hierarchy move it down in the queue and try again later
                certificates.insert(certificates.begin(), std::move(current));
            }
        }

        return ordered;
    }

private:
    /// @brief Attempts to add to the hierarchy the provided certificate
    /// @return True if we found within our hierarchy any certificate that 
    /// owns the provided certificate, false otherwise
    bool try_add_to_hierarchy(X509Wrapper &&certificate) {
        bool added = false;

        std::queue<std::reference_wrapper<X509Node>> stack;
        for (auto &roots : hierarchy) {
            stack.push(roots);
        }        

        while (!stack.empty()) {
            X509Node& top = stack.front();
            stack.pop();

            for (auto &child : top.children) {
                stack.push(child);
            }

            // Process node
            if (certificate.is_child(top.certificate)) {
                auto hash = certificate.get_certificate_hash_data(top.certificate);
                top.children.push_back({std::move(certificate), hash, {}});
                added = true;
                break;
            }
        }

        return added;
    }

private:
    std::vector<X509Node> hierarchy;
};

} // namespace evse_security

#endif // X509_BUNDLE_HPP