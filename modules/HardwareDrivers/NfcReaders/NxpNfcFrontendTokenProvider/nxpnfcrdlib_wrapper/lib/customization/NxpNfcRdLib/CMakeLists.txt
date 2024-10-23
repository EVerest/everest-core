cmake_minimum_required(VERSION 3.15)


# Interface library to hold common includes and compile definitions
add_library(NxpNfcRdLibCommon INTERFACE)
target_include_directories(NxpNfcRdLibCommon
    # INTERFACE
    #     $<BUILD_INTERFACE:${NXPLIB_PATH}/intfs>
    #     $<BUILD_INTERFACE:${NXPLIB_PATH}/types>
    #     $<BUILD_INTERFACE:${PHOSAL_PATH}/inc>
    #     $<BUILD_INTERFACE:${CUSTOM_NXPLIB_PATH}/types>
    #     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/intfs>
    #     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/types>
    #     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/inc>
    INTERFACE
        $<BUILD_INTERFACE:${NXPLIB_PATH}/intfs>
        $<BUILD_INTERFACE:${NXPLIB_PATH}/types>
        $<BUILD_INTERFACE:${PHOSAL_PATH}/inc>
        $<BUILD_INTERFACE:${CUSTOM_NXPLIB_PATH}/types>
)
target_compile_definitions(NxpNfcRdLibCommon
    INTERFACE
        PH_OSAL_LINUX
        NXPBUILD_CUSTOMER_HEADER_INCLUDED
)


add_library(NxpRdLib_phTools)
target_sources(NxpRdLib_phTools
    PRIVATE
        ${NXPLIB_PATH}/comps/phTools/src/phTools.c
        ${NXPLIB_PATH}/comps/phTools/src/phTools_Q.c
)
target_link_libraries(NxpRdLib_phTools
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_phhalHw)
target_sources(NxpRdLib_phhalHw
    PRIVATE
        ${NXPLIB_PATH}/comps/phhalHw/src/phhalHw.c
        ${NXPLIB_PATH}/comps/phhalHw/src/Rc663/phhalHw_Rc663.c
        ${NXPLIB_PATH}/comps/phhalHw/src/Rc663/phhalHw_Rc663_Cmd.c
        ${NXPLIB_PATH}/comps/phhalHw/src/Rc663/phhalHw_Rc663_Int.c
        ${NXPLIB_PATH}/comps/phhalHw/src/Rc663/phhalHw_Rc663_Wait.c
)
target_include_directories(NxpRdLib_phhalHw
    PRIVATE
        ${NXPLIB_PATH}/comps/phhalHw/src/Rc663
        ${DAL_PATH}/inc
        ${CUSTOM_DAL_PATH}/cfg
    PUBLIC
        $<BUILD_INTERFACE:${DAL_PATH}/boards>
        $<BUILD_INTERFACE:${CUSTOM_DAL_PATH}/boards>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/boards>
)
target_compile_definitions(NxpRdLib_phhalHw
    PUBLIC
        NXPBUILD__PHHAL_HW_RC663
        PHDRIVER_PI4RC663_BOARD
)
target_link_libraries(NxpRdLib_phhalHw
    PRIVATE
        NxpRdLib_phTools
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_palI14443p3a)
target_sources(NxpRdLib_palI14443p3a
    PRIVATE
        ${NXPLIB_PATH}/comps/phpalI14443p3a/src/phpalI14443p3a.c
        ${NXPLIB_PATH}/comps/phpalI14443p3a/src/Sw/phpalI14443p3a_Sw.c
)
target_include_directories(NxpRdLib_palI14443p3a
    PRIVATE
        ${NXPLIB_PATH}/comps/phpalI14443p3a/src/Sw
)
target_link_libraries(NxpRdLib_palI14443p3a
    PRIVATE
        NxpRdLib_phhalHw
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_palMifare)
target_sources(NxpRdLib_palMifare
    PRIVATE
        ${NXPLIB_PATH}/comps/phpalMifare/src/phpalMifare.c
        ${NXPLIB_PATH}/comps/phpalMifare/src/Sw/phpalMifare_Sw.c
)
target_include_directories(NxpRdLib_palMifare
    PRIVATE
        ${NXPLIB_PATH}/comps/NxpRdLib_palMifare/src/Sw
)
target_link_libraries(NxpRdLib_palMifare
    PRIVATE
        NxpRdLib_phhalHw
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_palSli15693)
target_sources(NxpRdLib_palSli15693
    PRIVATE
        ${NXPLIB_PATH}/comps/phpalSli15693/src/phpalSli15693.c
        ${NXPLIB_PATH}/comps/phpalSli15693/src/Sw/phpalSli15693_Sw.c
)
target_include_directories(NxpRdLib_palSli15693
    PRIVATE
        ${NXPLIB_PATH}/comps/phpalSli15693/src/Sw
)
target_link_libraries(NxpRdLib_palSli15693
    PRIVATE
        NxpRdLib_phhalHw
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_alMfc)
target_sources(NxpRdLib_alMfc
    PRIVATE
        ${NXPLIB_PATH}/comps/phalMfc/src/phalMfc.c
        ${NXPLIB_PATH}/comps/phalMfc/src/phalMfc_Int.c
        ${NXPLIB_PATH}/comps/phalMfc/src/Sw/phalMfc_Sw.c
)
target_include_directories(NxpRdLib_alMfc
    PRIVATE
        ${NXPLIB_PATH}/comps/phalMfc/src
        ${NXPLIB_PATH}/comps/phalMfc/src/Sw
)
target_compile_definitions(NxpRdLib_alMfc
    PUBLIC
        NXPBUILD__PHHAL_HW_RC663
)
target_link_libraries(NxpRdLib_alMfc
    PRIVATE
        NxpRdLib_palMifare
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_KeyStore)
target_sources(NxpRdLib_KeyStore
    PRIVATE
        ${NXPLIB_PATH}/comps/phKeyStore/src/phKeyStore.c
        ${NXPLIB_PATH}/comps/phKeyStore/src/Rc663/phKeyStore_Rc663.c
        ${NXPLIB_PATH}/comps/phKeyStore/src/Sw/phKeyStore_Sw.c
)
target_include_directories(NxpRdLib_KeyStore
    PRIVATE
        ${NXPLIB_PATH}/comps/phKeyStore/src
        ${NXPLIB_PATH}/comps/phKeyStore/src/Rc663
        ${NXPLIB_PATH}/comps/phKeyStore/src/Sw
)
target_link_libraries(NxpRdLib_KeyStore
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_phNfcLib)
target_sources(NxpRdLib_phNfcLib
    PRIVATE
        ${NXPLIB_PATH}/comps/phNfcLib/src/phNfcLib.c
        ${NXPLIB_PATH}/comps/phNfcLib/src/phNfcLib_15693.c
        ${NXPLIB_PATH}/comps/phNfcLib/src/phNfcLib_Initialization.c
)
target_include_directories(NxpRdLib_phNfcLib
    PRIVATE
        ${NXPLIB_PATH}/comps/phNfcLib/src
)
target_link_libraries(NxpRdLib_phNfcLib
    PUBLIC
        NxpRdLib_phhalHw
        NxpRdLib_palI14443p3a
        NxpRdLib_palMifare
        NxpRdLib_palSli15693
        NxpRdLib_alMfc
        NxpRdLib_KeyStore
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_alT1T)
target_sources(NxpRdLib_alT1T
    PRIVATE
        ${NXPLIB_PATH}/comps/phalT1T/src/phalT1T.c
        ${NXPLIB_PATH}/comps/phalT1T/src/Sw/phalT1T_Sw.c
)
target_include_directories(NxpRdLib_alT1T
    PRIVATE
        ${NXPLIB_PATH}/comps/phalT1T/src/Sw
)
target_link_libraries(NxpRdLib_alT1T
    PUBLIC
        NxpNfcRdLibCommon
)


add_library(NxpRdLib_acDiscLoop)
target_sources(NxpRdLib_acDiscLoop
    PRIVATE
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/phacDiscLoop.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_A.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_B.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_F.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_I18000p3m3.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_ECP.c
        ${NXPLIB_PATH}/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_V.c
)
target_include_directories(NxpRdLib_acDiscLoop
    PRIVATE
        ${NXPLIB_PATH}/comps/phacDiscLoop/src
)
target_link_libraries(NxpRdLib_acDiscLoop
    PRIVATE
        NxpRdLib_phhalHw
        NxpRdLib_palI14443p3a
    PUBLIC
        NxpNfcRdLibCommon
)



# Theses components have been left out for now.
# phalFelica
# phalI18000p3m3
# phalICode
# phalMfdf
# phalMfdfEVx
# phalMfdfLight
# phalMfNtag42XDna
# phalMfp
# phalMfpEVx
# phalMful
# phalTop
# phalVca
# phceT4T
# phCidManager
# phCryptoRng
# phCryptoSym
# phlnLlcp
# phLog
# phnpSnep
# phpalEpcUid
# phpalFelica
# phpalI14443p3b
# phpalI14443p4
# phpalI14443p4a
# phpalI14443p4mC
# phpalI18000p3m3
# phpalI18092mPI
# phpalI18092mT
# phTMIUtils
