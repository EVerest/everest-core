// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "NxpNfcRdLibIncludes.hpp"

namespace NxpNfcFrontendWrapper {

class NxpNfcRdLibIfc {
public:
    enum class Technology {
        UNKNOWN,
        ISO14443A_T1,
        ISO14443A_T2,
        ISO14443A_T4A,
        ISO15693,
    };

    NxpNfcRdLibIfc();

    using callback_func_t = std::function<void(const NxpNfcRdLibIfc::Technology&, const std::vector<std::uint8_t>&)>;
    using error_callback_func_t = std::function<void(const std::string&)>;

    void setCallback(const callback_func_t& callback);
    void setErrorCallback(const error_callback_func_t& callback);
    void startThreads();
    void stopThreads();

private:
    void decodeDiscLoopStatusAndCallback(phStatus_t);
    void handleActivatedDevice();
    void handleDiscoveryLoopFailure();
    void IrqHandler();
    void runIrqPollThread();
    void runDiscoveryLoopThread();
    void doErrorCallback(std::string, ErrorInfo);

    static std::string formatErrorInfo(const std::string&, const ErrorInfo&);
    static NxpNfcRdLibIfc::Technology decodeTechnology(phacDiscLoop_Sw_DataParams_t*, uint16_t);
    static std::vector<std::uint8_t> getUID(phacDiscLoop_Sw_DataParams_t*, NxpNfcRdLibIfc::Technology);

    callback_func_t m_callback;
    error_callback_func_t m_err_callback;
    std::atomic<bool> m_stopFlag{false};

    // Nxp Nfc Rd Lib Context
    uint16_t m_bSavePollTechCfg =
        PHAC_DISCLOOP_POS_BIT_MASK_A | PHAC_DISCLOOP_POS_BIT_MASK_V; // Configure to detect ISO14443A and 15693 cards
    phbalReg_Type_t m_sBalParams;
    phhalHw_Rc663_DataParams_t* m_pHal;
    phacDiscLoop_Profile_t m_bProfile = PHAC_DISCLOOP_PROFILE_UNKNOWN;
    phacDiscLoop_Sw_DataParams_t* m_pDiscLoop;

    std::unique_ptr<std::thread> m_irq_poll_thread;
    std::unique_ptr<std::thread> m_discloop_thread;
};

} // namespace NxpNfcFrontendWrapper
