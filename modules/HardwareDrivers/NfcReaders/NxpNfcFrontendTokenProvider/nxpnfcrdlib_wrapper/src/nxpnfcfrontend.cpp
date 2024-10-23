// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <string>

#include <NxpNfcRdLibIfc.hpp>
#include <nxpnfcfrontend.hpp>
#include <stdexcept>

using NxpNfcFrontend = NxpNfcFrontendWrapper::NxpNfcFrontend;

NxpNfcFrontend::NxpNfcFrontend() {
    if (s_someInstanceExists) {
        throw std::runtime_error("Only one NxpNfcFrontend Instance allowed!");
    }

    s_someInstanceExists = true;

    m_nfclib_ifc = std::make_unique<NxpNfcRdLibIfc>();
}

NxpNfcFrontend::~NxpNfcFrontend() {
    m_nfclib_ifc->stopThreads();

    s_someInstanceExists = false;
}

void NxpNfcFrontend::setDetectionCallback(
    const std::function<void(const std::pair<std::string, std::vector<std::uint8_t>>&)>& callback) {
    m_nfclib_ifc->setCallback([callback](NxpNfcRdLibIfc::Technology technology, std::vector<std::uint8_t> uid_vec) {
        std::string protocol;
        switch (technology) {
        case NxpNfcRdLibIfc::Technology::ISO14443A_T1:
        case NxpNfcRdLibIfc::Technology::ISO14443A_T2:
        case NxpNfcRdLibIfc::Technology::ISO14443A_T4A:
            protocol = "ISO14443";
            break;
        case NxpNfcRdLibIfc::Technology::ISO15693:
            protocol = "ISO15693";
            break;
        default:
            protocol = "UNKNOWN";
        }
        callback({protocol, uid_vec});
    });
}

void NxpNfcFrontend::setErrorLogCallback(const std::function<void(const std::string&)>& callback) {
    m_nfclib_ifc->setErrorCallback(callback);
}

void NxpNfcFrontend::run() {
    m_nfclib_ifc->startThreads();
}
