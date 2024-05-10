// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <evse_security/crypto/interface/crypto_types.hpp>

namespace evse_security {

namespace conversions {

std::string get_certificate_sign_request_result_to_string(CertificateSignRequestResult e) {

    switch (e) {
    case CertificateSignRequestResult::Valid:
        return "Valid";
    case CertificateSignRequestResult::KeyGenerationError:
        return "KeyGenerationError";
    case CertificateSignRequestResult::VersioningError:
        return "VersioningError";
    case CertificateSignRequestResult::PubkeyError:
        return "PubkeyError";
    case CertificateSignRequestResult::ExtensionsError:
        return "ExtensionsError";
    case CertificateSignRequestResult::SigningError:
        return "SigningError";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CertificateSignRequestResult");
}

} // namespace conversions

} // namespace evse_security