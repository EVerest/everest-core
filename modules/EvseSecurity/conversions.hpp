// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SECURITY_CONVERSIONS_HPP
#define EVSE_SECURITY_CONVERSIONS_HPP

#include <evse_security/evse_security.hpp>
#include <generated/interfaces/evse_security/Implementation.hpp>

namespace module {

namespace conversions {

evse_security::EncodingFormat from_everest(types::evse_security::EncodingFormat other);
evse_security::CaCertificateType from_everest(types::evse_security::CaCertificateType other);
evse_security::LeafCertificateType from_everest(types::evse_security::LeafCertificateType other);
evse_security::CertificateType from_everest(types::evse_security::CertificateType other);
evse_security::HashAlgorithm from_everest(types::evse_security::HashAlgorithm other);
evse_security::InstallCertificateResult from_everest(types::evse_security::InstallCertificateResult other);
evse_security::DeleteCertificateResult from_everest(types::evse_security::DeleteCertificateResult other);
evse_security::GetInstalledCertificatesStatus from_everest(types::evse_security::GetInstalledCertificatesStatus other);

evse_security::CertificateHashData from_everest(types::evse_security::CertificateHashData other);
evse_security::CertificateHashDataChain from_everest(types::evse_security::CertificateHashDataChain other);
evse_security::GetInstalledCertificatesResult from_everest(types::evse_security::GetInstalledCertificatesResult other);
evse_security::OCSPRequestData from_everest(types::evse_security::OCSPRequestData other);
evse_security::OCSPRequestDataList from_everest(types::evse_security::OCSPRequestDataList other);
evse_security::CertificateInfo from_everest(types::evse_security::CertificateInfo other);

types::evse_security::EncodingFormat to_everest(evse_security::EncodingFormat other);
types::evse_security::CaCertificateType to_everest(evse_security::CaCertificateType other);
types::evse_security::LeafCertificateType to_everest(evse_security::LeafCertificateType other);
types::evse_security::CertificateType to_everest(evse_security::CertificateType other);
types::evse_security::HashAlgorithm to_everest(evse_security::HashAlgorithm other);
types::evse_security::InstallCertificateResult to_everest(evse_security::InstallCertificateResult other);
types::evse_security::CertificateValidationResult to_everest(evse_security::CertificateValidationResult other);
types::evse_security::DeleteCertificateResult to_everest(evse_security::DeleteCertificateResult other);
types::evse_security::GetInstalledCertificatesStatus to_everest(evse_security::GetInstalledCertificatesStatus other);
types::evse_security::GetCertificateInfoStatus to_everest(evse_security::GetCertificateInfoStatus other);

types::evse_security::CertificateHashData to_everest(evse_security::CertificateHashData other);
types::evse_security::CertificateHashDataChain to_everest(evse_security::CertificateHashDataChain other);
types::evse_security::GetInstalledCertificatesResult to_everest(evse_security::GetInstalledCertificatesResult other);
types::evse_security::OCSPRequestData to_everest(evse_security::OCSPRequestData other);
types::evse_security::OCSPRequestDataList to_everest(evse_security::OCSPRequestDataList other);
types::evse_security::CertificateInfo to_everest(evse_security::CertificateInfo other);

} // namespace conversions

} // namespace module

#endif
