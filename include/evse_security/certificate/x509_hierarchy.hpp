// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <queue>

#include <evse_security/certificate/x509_wrapper.hpp>

namespace evse_security {

class NoCertificateFound : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct X509Node {
    X509Wrapper certificate;
    CertificateHashData hash;

    X509Wrapper issuer;
    std::vector<X509Node> children;
};

/// @brief Utility class that is able to build a immutable certificate hierarchy
/// with a  list of self-signed root certificates and their respective sub-certificates
class X509CertificateHierarchy {
public:
    const std::vector<X509Node>& get_hierarchy() const {
        return hierarchy;
    }

    /// @brief Checks if the provided certificate is a self-signed root CA certificate
    /// contained in our hierarchy
    bool is_root(const X509Wrapper& certificate) const;

    /// @brief Collects all the descendants of the provided certificate
    /// @param top Certificate that issued the descendants
    std::vector<X509Wrapper> collect_descendants(const X509Wrapper& top);

    /// @brief obtains the hash data of the certificate, finding its issuer if needed
    CertificateHashData get_certificate_hash(const X509Wrapper& certificate);

    /// @brief returns true if we contain a certificate with the following hash
    bool contains_certificate_hash(const CertificateHashData& hash);

    /// @brief Searches for the provided hash, throwing a NoCertificateFound if not found
    X509Wrapper find_certificate(const CertificateHashData& hash);

public:
    std::string to_debug_string();

    /// @brief Breadth-first iteration through all the hierarchy of
    /// certificates. Will break when the function returns false
    template <typename function> void for_each(function func) {
        std::queue<std::reference_wrapper<X509Node>> queue;
        for (auto& root : hierarchy) {
            // Process roots
            if (!func(root))
                return;

            for (auto& child : root.children) {
                queue.push(child);
            }
        }

        while (!queue.empty()) {
            X509Node& top = queue.front();
            queue.pop();

            // Process node
            if (!func(top))
                return;

            for (auto& child : top.children) {
                queue.push(child);
            }
        }
    }

public:
    /// @brief Depth-first descendant iteration
    template <typename function> static void for_each_descendant(function func, const X509Node& node, int depth = 0) {
        if (node.children.empty())
            return;

        for (const auto& child : node.children) {
            func(child, depth);

            if (!child.children.empty()) {
                for_each_descendant(func, child, (depth + 1));
            }
        }
    }

public:
    /// @brief Builds a proper certificate hierarchy from the provided certificates. The
    /// hierarchy can be incomplete, in case orphan certificates are present in the list
    static X509CertificateHierarchy build_hierarchy(std::vector<X509Wrapper>& certificates);

private:
    /// @brief Attempts to add to the hierarchy the provided certificate
    /// @return True if we found within our hierarchy any certificate that
    /// owns the provided certificate, false otherwise
    bool try_add_to_hierarchy(X509Wrapper&& certificate);

private:
    std::vector<X509Node> hierarchy;
};

} // namespace evse_security
