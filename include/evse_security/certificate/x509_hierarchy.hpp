// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <algorithm>
#include <queue>

#include <evse_security/certificate/x509_wrapper.hpp>

namespace evse_security {

class NoCertificateFound : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class InvalidStateException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct NodeState {
    std::uint32_t is_selfsigned : 1;
    std::uint32_t is_orphan : 1; // 0 means temporary orphan, 1 is permanent orphan, no relevance if it is self-signed
    std::uint32_t is_hash_computed : 1; // if the hash was correctly computed or if a placeholder hash is set
};

struct X509Node {
    NodeState state;

    X509Wrapper certificate;
    CertificateHashData hash;

    X509Wrapper issuer;
    std::vector<X509Node> children;
};

/// @brief Utility class that is able to build a immutable certificate hierarchy
/// with a  list of self-signed root certificates and their respective sub-certificates
/// Note: non self-signed roots and cross-signed certificates are not supported now
class X509CertificateHierarchy {
public:
    const std::vector<X509Node>& get_hierarchy() const {
        return hierarchy;
    }

    /// @brief Checks if the provided certificate is a self-signed root CA certificate
    /// contained in our hierarchy
    bool is_internal_root(const X509Wrapper& certificate) const;

    /// @brief Collects all the descendants of the provided certificate
    /// @param top Certificate that issued the descendants
    std::vector<X509Wrapper> collect_descendants(const X509Wrapper& top);

    /// @brief obtains the hash data of the certificate, finding its issuer if needed
    CertificateHashData get_certificate_hash(const X509Wrapper& certificate);

    /// @brief returns true if we contain a certificate with the following hash
    bool contains_certificate_hash(const CertificateHashData& hash);

    /// @brief Searches for the root of the provided leaf, throwing a NoCertificateFound if not found
    X509Wrapper find_certificate_root(const X509Wrapper& leaf);

    /// @brief Searches for the provided hash, throwing a NoCertificateFound if not found
    X509Wrapper find_certificate(const CertificateHashData& hash, bool case_insensitive_comparison = false);

    /// @brief Searches for all the certificates with the provided hash, throwing a NoCertificateFound
    // if none were found. Can be useful when we have SUB-CAs in multiple bundles
    std::vector<X509Wrapper> find_certificates_multi(const CertificateHashData& hash);

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

    template <typename... Args> static X509CertificateHierarchy build_hierarchy(Args... certificates) {
        X509CertificateHierarchy ordered;

        (std::for_each(certificates.begin(), certificates.end(),
                       [&ordered](X509Wrapper& cert) { ordered.insert(std::move(cert)); }),
         ...); // Fold expr

        // Prune the tree
        ordered.prune();

        return ordered;
    }

private:
    /// @brief Inserts the certificate in the hierarchy. If it is not a root
    /// and a parent is not found, it will be inserted as a temporary orphan
    void insert(X509Wrapper&& certificate);

    /// @brief After inserting all certificates in the hierarchy, attempts
    /// to parent all temporarly orphan certificates, marking the ones that
    /// were not successfully parented as permanently orphan
    void prune();

private:
    std::vector<X509Node> hierarchy;
};

} // namespace evse_security
