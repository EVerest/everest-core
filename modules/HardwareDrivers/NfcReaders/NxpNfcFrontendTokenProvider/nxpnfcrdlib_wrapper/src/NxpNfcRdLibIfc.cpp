// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <sstream>
#include <stdexcept>

#include <NxpNfcRdLibIfc.hpp>

using NxpNfcRdLibIfc = NxpNfcFrontendWrapper::NxpNfcRdLibIfc;

NxpNfcRdLibIfc::NxpNfcRdLibIfc() {
    phStatus_t status = PH_ERR_INTERNAL_ERROR;

    /* Pre-library initialization phase */
    /* 1. Prepare OS abstraction layer */
    status = phOsal_Init();
    if (status != PH_OSAL_SUCCESS) {
        std::string error_message = formatErrorInfo("phOsal_Init() failed! (Initialization of "
                                                    "Operating System Abstraction Layer)",
                                                    decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    /* 2. Prepare Bus abstraction layer */
    status = phbalReg_Init(&m_sBalParams, sizeof(phbalReg_Type_t));
    if (status != PH_DRIVER_SUCCESS) {
        std::string error_message = formatErrorInfo("phbalReg_Init() failed! (Initialization of Bus Abstraction Layer)",
                                                    decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    /* 3. Tell phNfcLib about Bus abstraction layer */
    phNfcLib_AppContext_t appContext = {0};
    appContext.pBalDataparams = &m_sBalParams;
    status = phNfcLib_SetContext(&appContext);
    if (status != PH_NFCLIB_STATUS_SUCCESS) {
        std::string error_message = formatErrorInfo("phNfcLib_SetContext() failed!", decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    /* Initialize library */
    status = phNfcLib_Init();
    if (status != PH_NFCLIB_STATUS_SUCCESS) {
        std::string error_message = formatErrorInfo("phNfcLib_Init() failed!", decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    /* Store ptrs to library's internal components' dataparams structs for HAL and DISCLOOP */
    m_pHal = static_cast<phhalHw_Rc663_DataParams_t*>(phNfcLib_GetDataParams(PH_COMP_HAL));
    m_pDiscLoop = static_cast<phacDiscLoop_Sw_DataParams_t*>(phNfcLib_GetDataParams(PH_COMP_AC_DISCLOOP));
}

void NxpNfcRdLibIfc::setCallback(const callback_func_t& callback) {
    m_callback = callback;
}

void NxpNfcRdLibIfc::setErrorCallback(const error_callback_func_t& callback) {
    m_err_callback = callback;
}

void NxpNfcRdLibIfc::startThreads() {
    m_irq_poll_thread = std::make_unique<std::thread>(&NxpNfcRdLibIfc::runIrqPollThread, this);
    m_discloop_thread = std::make_unique<std::thread>(&NxpNfcRdLibIfc::runDiscoveryLoopThread, this);
}

void NxpNfcRdLibIfc::stopThreads() {
    m_stopFlag = true;

    m_irq_poll_thread->join();
    m_discloop_thread->join();
}

void NxpNfcRdLibIfc::runDiscoveryLoopThread() {
    phStatus_t status;

    /* Get RFID Poll Configuration */
    status = phacDiscLoop_GetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, &m_bSavePollTechCfg);
    if (status != PH_ERR_SUCCESS) {
        std::string error_message = formatErrorInfo("phacDiscLoop_GetConfig() failed!", decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    /* Switch off RF field */
    status = phhalHw_FieldOff(m_pHal);
    if (status != PH_ERR_SUCCESS) {
        std::string error_message = formatErrorInfo("Initial phhalHw_FieldOff() failed!", decodeErrorCode(status));
        throw std::runtime_error(error_message);
    }

    // From now on, no more exception throwing, because the loop should keep running
    uint16_t wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
    while (!m_stopFlag) {
        status = phacDiscLoop_SetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE,
                                        PHAC_DISCLOOP_POLL_STATE_DETECTION);
        if (status != PH_ERR_SUCCESS) {
            doErrorCallback("Discovery Loop: phacDiscLoop_SetConfig: ", decodeErrorCode(status));
        }

        /* Start discovery loop */
        status = phacDiscLoop_Run(m_pDiscLoop, wEntryPoint);
        decodeDiscLoopStatusAndCallback(status);

        /* Set Poll Configuration */
        status = phacDiscLoop_SetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, m_bSavePollTechCfg);
        if (status != PH_ERR_SUCCESS) {
            doErrorCallback("Discovery Loop: phacDiscLoop_SetConfig: ", decodeErrorCode(status));
        }

        /* Switch off RF field */
        status = phhalHw_FieldOff(m_pHal);
        if (status != PH_ERR_SUCCESS) {
            doErrorCallback("Discovery Loop: phhalHw_FieldOff: ", decodeErrorCode(status));
        }

        /* Wait for field-off time-out */
        status = phhalHw_Wait(m_pHal, PHHAL_HW_TIME_MICROSECONDS, 5100);
        if (status != PH_ERR_SUCCESS) {
            doErrorCallback("Discovery Loop: phhalHw_Wait: ", decodeErrorCode(status));
        }

        phOsal_ThreadDelay(HI_LEVEL_LOOP_DELAY_MS);
    }
    /* Switch off RF field */
    phhalHw_FieldOff(m_pHal);
}

void NxpNfcRdLibIfc::runIrqPollThread() {
    uint8_t gpioValue = 0;

    while (GPIO_read_pin(PHDRIVER_PIN_IRQ, &gpioValue) != PH_ERR_SUCCESS) {
        GPIO_reconfigure_pin(PHDRIVER_PIN_IRQ, false);
    }

    // If it happens to be necessary call the IRQ handler now
    if (gpioValue == 1) {
        IrqHandler();
    }

    while (!m_stopFlag) {
        // Wait for the IRQ PIN to change, but wait until the timeout only
        if (GPIO_poll_pin(PHDRIVER_PIN_IRQ, 1000) == PH_ERR_SUCCESS) {
            IrqHandler();
        } else {
            GPIO_reconfigure_pin(PHDRIVER_PIN_IRQ, false);
        }
    }
}

void GPIO_reconfigure_pin(size_t gpio) {
    PiGpio_unexport(PHDRIVER_PIN_IRQ);
    PiGpio_export(PHDRIVER_PIN_IRQ);
    PiGpio_set_direction(PHDRIVER_PIN_IRQ, false);
    if (PIN_IRQ_TRIGGER_TYPE == PH_DRIVER_INTERRUPT_RISINGEDGE) {
        PiGpio_set_edge(PHDRIVER_PIN_IRQ, true, false);
    } else {
        PiGpio_set_edge(PHDRIVER_PIN_IRQ, false, true);
    }
}

void NxpNfcRdLibIfc::IrqHandler() {
    if (phDriver_PinRead(PHDRIVER_PIN_IRQ, PH_DRIVER_PINFUNC_INTERRUPT)) {
        phDriver_PinClearIntStatus(PHDRIVER_PIN_IRQ);

        if (m_pHal->pRFISRCallback != NULL) {
            m_pHal->pRFISRCallback(m_pHal);
        }
    }
}

void NxpNfcRdLibIfc::decodeDiscLoopStatusAndCallback(phStatus_t discLoopStatus) {
    switch (discLoopStatus & PH_ERR_MASK) {
    case PHAC_DISCLOOP_MULTI_TECH_DETECTED:    // Do not try to resolve multi-tech collision
    case PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED: // Do not try to resolve multi-device collision
    case PHAC_DISCLOOP_NO_TECH_DETECTED:       // Nothing to be done here
    case PHAC_DISCLOOP_NO_DEVICE_RESOLVED:     // Nothing to be done here
    case PHAC_DISCLOOP_EXTERNAL_RFON:          // External RF is ignored
    case PHAC_DISCLOOP_MERGED_SEL_RES_FOUND:   // T4T or NFC-DEP devices are ignored
        break;
    case PHAC_DISCLOOP_DEVICE_ACTIVATED:
        handleActivatedDevice();
        break;
    case PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED:  // Do not try to connect to active targets
    case PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED: // Do not try to connect to passive targets
    case PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED:    // Nothing to be done here
        break;
    case PHAC_DISCLOOP_FAILURE: {
        handleDiscoveryLoopFailure();
        break;
    }
    default:
        doErrorCallback("Decoding discovery loop status failed", decodeErrorCode(0x00));
    }
}

void NxpNfcRdLibIfc::handleActivatedDevice() {
    uint16_t noOfTagsDetected = 0;
    phStatus_t status = phacDiscLoop_GetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &noOfTagsDetected);

    if (status != PH_ERR_SUCCESS or noOfTagsDetected > 1) {
        return;
    }

    /* Get Detected Technology Type */
    uint16_t technologyCodeDetected = 0;
    status = phacDiscLoop_GetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &technologyCodeDetected);

    if (status != PH_ERR_SUCCESS) {
        return;
    }

    NxpNfcRdLibIfc::Technology technology = NxpNfcRdLibIfc::decodeTechnology(m_pDiscLoop, technologyCodeDetected);

    if (technology == NxpNfcRdLibIfc::Technology::UNKNOWN) {
        return;
    }

    std::vector<std::uint8_t> uid = NxpNfcRdLibIfc::getUID(m_pDiscLoop, technology);

    if (m_callback) {
        m_callback(technology, uid);
    }
}

void NxpNfcRdLibIfc::handleDiscoveryLoopFailure() {
    uint16_t additionalInfo;
    phStatus_t status = phacDiscLoop_GetConfig(m_pDiscLoop, PHAC_DISCLOOP_CONFIG_ADDITIONAL_INFO, &additionalInfo);
    if (status != PH_ERR_SUCCESS) {
        doErrorCallback("PHAC_DISCLOOP_FAILURE: Trying to get additional Information failed: ",
                        decodeErrorCode(status));
        return;
    }
    doErrorCallback("Decoding discovery loop status failed (PHAC_DISCLOOP_FAILURE)", decodeErrorCode(additionalInfo));
}

void NxpNfcRdLibIfc::doErrorCallback(std::string customText, ErrorInfo errorInfo) {
    std::string formattedMessage = formatErrorInfo(customText, errorInfo);

    if (m_err_callback) {
        m_err_callback(formattedMessage);
    }
}

// static helpers
std::string NxpNfcRdLibIfc::formatErrorInfo(const std::string& customText, const ErrorInfo& errorInfo) {
    return customText + ": Error in component '" + errorInfo.component + "': " + errorInfo.type;
}

NxpNfcRdLibIfc::Technology NxpNfcRdLibIfc::decodeTechnology(phacDiscLoop_Sw_DataParams_t* pDataParams,
                                                            uint16_t techCode) {
    if (PHAC_DISCLOOP_CHECK_ANDMASK(techCode, PHAC_DISCLOOP_POS_BIT_MASK_A)) {
        if (pDataParams->sTypeATargetInfo.bT1TFlag) {
            return NxpNfcRdLibIfc::Technology::ISO14443A_T1;
        } else {
            if ((pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & (uint8_t)0x04) == 0) {
                uint8_t tagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60) >> 5;

                switch (tagType) {
                case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
                    return NxpNfcRdLibIfc::Technology::ISO14443A_T2;
                case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
                    return NxpNfcRdLibIfc::Technology::ISO14443A_T4A;
                }
            }
        }
    } else if (PHAC_DISCLOOP_CHECK_ANDMASK(techCode, PHAC_DISCLOOP_POS_BIT_MASK_V)) {
        return NxpNfcRdLibIfc::Technology::ISO15693;
    }

    return NxpNfcRdLibIfc::Technology::UNKNOWN;
}

std::vector<std::uint8_t> NxpNfcRdLibIfc::getUID(phacDiscLoop_Sw_DataParams_t* pDataParams,
                                                 NxpNfcRdLibIfc::Technology technology) {
    int length = 0;
    uint8_t* uidArray = nullptr;

    switch (technology) {
    case NxpNfcRdLibIfc::Technology::ISO14443A_T1:
    case NxpNfcRdLibIfc::Technology::ISO14443A_T2:
    case NxpNfcRdLibIfc::Technology::ISO14443A_T4A:
        length = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
        uidArray = pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid;
        break;
    case NxpNfcRdLibIfc::Technology::ISO15693:
        length = 8;
        uidArray = pDataParams->sTypeVTargetInfo.aTypeV[0].aUid;
        break;
    case NxpNfcRdLibIfc::Technology::UNKNOWN: {
    }
    }

    if (uidArray) {
        return std::vector<uint8_t>(uidArray, uidArray + length);
    } else {
        return {};
    }
}
