// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/crypto/openssl_crypto_supplier.hpp>
#include <charge_bridge/crypto/openssl_types.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/x509v3.h>

#include <charge_bridge/utilities/filesystem.hpp>
#include <charge_bridge/utilities/logging.hpp>

namespace charge_bridge::crypto {

bool OpenSSLSupplier::digest_file_sha256(const fs::path& path, std::vector<std::uint8_t>& out_digest) {
    EVP_MD_CTX_ptr md_context_ptr(EVP_MD_CTX_create());
    if (!md_context_ptr.get()) {
        return false;
    }

    const EVP_MD* md = EVP_get_digestbyname("SHA256");
    if (EVP_DigestInit_ex(md_context_ptr.get(), md, nullptr) == 0) {
        return false;
    }

    bool digest_error = false;

    unsigned int sha256_out_length = 0;
    std::uint8_t sha256_out[EVP_MAX_MD_SIZE];

    // calculate sha256 of file
    bool processed_file =
        filesystem_utils::process_file(path, BUFSIZ, [&](const std::vector<uint8_t> bytes, bool last_chunk) -> bool {
            if (bytes.size() > 0) {
                if (EVP_DigestUpdate(md_context_ptr.get(), bytes.data(), bytes.size()) == 0) {
                    digest_error = true;
                    return true;
                }
            }

            if (last_chunk) {
                if (EVP_DigestFinal_ex(md_context_ptr.get(), reinterpret_cast<unsigned char*>(sha256_out),
                                       &sha256_out_length) == 0) {
                    digest_error = true;
                    return true;
                }
            }

            return false;
        });

    if ((processed_file == false) || (digest_error == true)) {
        return false;
    }

    out_digest.clear();
    out_digest.insert(std::end(out_digest), sha256_out, sha256_out + sha256_out_length);

    return true;
}

template <typename T> static bool base64_decode(const std::string& base64_string, T& out_decoded) {
    EVP_ENCODE_CTX_ptr base64_decode_context_ptr(EVP_ENCODE_CTX_new());
    if (!base64_decode_context_ptr.get()) {
        return false;
    }

    EVP_DecodeInit(base64_decode_context_ptr.get());
    if (!base64_decode_context_ptr.get()) {
        return false;
    }

    const unsigned char* encoded_str = reinterpret_cast<const unsigned char*>(base64_string.data());
    int base64_length = base64_string.size();

    std::uint8_t* decoded_out = static_cast<std::uint8_t*>(alloca(base64_length));

    int decoded_out_length;
    if (EVP_DecodeUpdate(base64_decode_context_ptr.get(), reinterpret_cast<unsigned char*>(decoded_out),
                         &decoded_out_length, encoded_str, base64_length) < 0) {
        return false;
    }

    int decode_final_out;
    if (EVP_DecodeFinal(base64_decode_context_ptr.get(), reinterpret_cast<unsigned char*>(decoded_out),
                        &decode_final_out) < 0) {
        return false;
    }

    out_decoded.clear();
    out_decoded.insert(std::end(out_decoded), decoded_out, decoded_out + decoded_out_length);

    return true;
}

static bool base64_encode(const unsigned char* bytes_str, int bytes_size, std::string& out_encoded) {
    EVP_ENCODE_CTX_ptr base64_encode_context_ptr(EVP_ENCODE_CTX_new());
    if (!base64_encode_context_ptr.get()) {
        return false;
    }

    EVP_EncodeInit(base64_encode_context_ptr.get());
    // evp_encode_ctx_set_flags(base64_encode_context_ptr.get(), EVP_ENCODE_CTX_NO_NEWLINES); // Of course it's not
    // public

    if (!base64_encode_context_ptr.get()) {
        return false;
    }

    int base64_length = ((bytes_size / 3) * 4) + 2;
    // If it causes issues, replace with 'alloca' on different platform
    std::vector<char> base64_out(base64_length + 66);
    int full_len = 0;

    int base64_out_length;
    if (EVP_EncodeUpdate(base64_encode_context_ptr.get(), reinterpret_cast<unsigned char*>(base64_out.data()),
                         &base64_out_length, bytes_str, bytes_size) < 0) {
        return false;
    }
    full_len += base64_out_length;

    EVP_EncodeFinal(base64_encode_context_ptr.get(), reinterpret_cast<unsigned char*>(base64_out.data()) + base64_out_length,
                    &base64_out_length);
    full_len += base64_out_length;

    out_encoded.assign(base64_out.data(), full_len);

    return true;
}

bool OpenSSLSupplier::base64_decode_to_bytes(const std::string& base64_string, std::vector<std::uint8_t>& out_decoded) {
    return base64_decode<std::vector<std::uint8_t>>(base64_string, out_decoded);
}

bool OpenSSLSupplier::base64_decode_to_string(const std::string& base64_string, std::string& out_decoded) {
    return base64_decode<std::string>(base64_string, out_decoded);
}

bool OpenSSLSupplier::base64_encode_from_bytes(const std::vector<std::uint8_t>& bytes, std::string& out_encoded) {
    return base64_encode(reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size(), out_encoded);
}

bool OpenSSLSupplier::base64_encode_from_string(const std::string& string, std::string& out_encoded) {
    return base64_encode(reinterpret_cast<const unsigned char*>(string.data()), string.size(), out_encoded);
}

} // namespace charge_bridge::crypto
