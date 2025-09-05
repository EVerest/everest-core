cmake_minimum_required(VERSION 3.15)
project(
    NxpNfcFrontendLib
    VERSION 07.10.00
    DESCRIPTION "NXP NFC Frontend Lib"
    LANGUAGES CXX C
)

find_package(Threads REQUIRED)

set(EXTRACTION_PATH ${CMAKE_CURRENT_BINARY_DIR}/_deps)
set(PATH_NXPNFCRDLIB "${EXTRACTION_PATH}/NxpNfcRdLib_Linux_v07.10.00_PUB/")

if(EXTERNAL_NXPNFCRDLIB_ZIP_PATH AND EXISTS "${EXTERNAL_NXPNFCRDLIB_ZIP_PATH}")
    file(ARCHIVE_EXTRACT
        INPUT
            "${EXTERNAL_NXPNFCRDLIB_ZIP_PATH}"
        DESTINATION
            "${EXTRACTION_PATH}")
else()
    message(FATAL_ERROR "EXTERNAL_NXPNFCRDLIB_ZIP_PATH not set or invalid. Cannot build the NxpNfcFrontendLib wrapper library.")
endif()

if (NOT EXISTS "${PATH_NXPNFCRDLIB}/CMakeLists.txt")
    message(FATAL_ERROR "Path to NxpNfcRdLib looks empty: Did not find ${PATH_NXPNFCRDLIB}/CMakeLists.txt")
endif()

add_subdirectory(lib)

add_library(NxpNfcFrontend)
target_sources(NxpNfcFrontend
    PRIVATE
        src/errorDecoding.cpp
        src/nxpnfcfrontend.cpp
        src/NxpNfcRdLibIncludes.hpp
        src/NxpNfcRdLibIfc.hpp
        src/NxpNfcRdLibIfc.cpp
)
target_compile_features(NxpNfcFrontend
    INTERFACE
        cxx_std_17
)
target_include_directories(NxpNfcFrontend
    PRIVATE
        src
    PUBLIC
        inc
)
target_link_libraries(NxpNfcFrontend
    PRIVATE
        NxpRdLib
        Threads::Threads
)

add_subdirectory(testApplication)
