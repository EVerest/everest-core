// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace NxpNfcFrontendWrapper {
// Forward declarations, to keep headers private
class NxpNfcRdLibIfc;

class NxpNfcFrontend {
public:
    NxpNfcFrontend();
    ~NxpNfcFrontend();

    NxpNfcFrontend(const NxpNfcFrontend&) = delete;
    NxpNfcFrontend& operator=(const NxpNfcFrontend&) = delete;

    void setDetectionCallback(const std::function<void(const std::pair<std::string, std::vector<std::uint8_t>>&)>&);
    void setErrorLogCallback(const std::function<void(const std::string&)>&);
    void run();

private:
    std::unique_ptr<NxpNfcRdLibIfc> m_nfclib_ifc;

    static inline std::atomic<bool> s_someInstanceExists{false};
};

} // namespace NxpNfcFrontendWrapper
