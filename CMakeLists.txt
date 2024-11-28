cmake_minimum_required(VERSION 3.14)

project(everest-evse_security VERSION 0.9.2
        DESCRIPTION "Implementation of EVSE related security operations"
		LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(${PROJECT_NAME}_BUILD_TESTING "Build unit tests, used if included as dependency" OFF)
option(BUILD_TESTING "Build unit tests, used if standalone project" OFF)
option(EVSE_SECURITY_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})
option(USING_TPM2 "Include code for using OpenSSL 3 and the tpm2 provider" OFF)
option(USING_CUSTOM_PROVIDER "Include code for using OpenSSL 3 and the custom provider" OFF)

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(LIBEVSE_SECURITY_BUILD_TESTING ON)
    evc_include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

if(USING_TPM2 AND USING_CUSTOM_PROVIDER)
    message(FATAL_ERROR, "TPM2 provider and custom provider are incompatible")
endif()

if(USING_TPM2)
    set(CUSTOM_PROVIDER_NAME "tpm2")    
endif()

if(USING_CUSTOM_PROVIDER)
    set(CUSTOM_PROVIDER_NAME "custom_provider")    
endif()

if(USING_TPM2 OR USING_CUSTOM_PROVIDER)
# OpenSSL property string when using the default provider
    set(PROPQUERY_PROVIDER_DEFAULT "provider!=${CUSTOM_PROVIDER_NAME}")
    # OpenSSL property string when using the tpm2/custom provider
    set(PROPQUERY_PROVIDER_CUSTOM "?provider=${CUSTOM_PROVIDER_NAME},${CUSTOM_PROVIDER_NAME}.digest!=yes,${CUSTOM_PROVIDER_NAME}.cipher!=yes")
endif()

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(EVSE_SECURITY_INSTALL OFF)
else()
    find_package(date REQUIRED)
    find_package(everest-log REQUIRED)
    find_package(everest-timer REQUIRED)
endif()

option(LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM "Usage of boost/filesystem.hpp instead of std::filesystem" OFF)

option(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL "Default OpenSSL cryptography supplier" ON)

# dependencies
if (LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
    find_package(OpenSSL 3 REQUIRED)
endif()

add_subdirectory(lib)

# packaging
if (EVSE_SECURITY_INSTALL)
    install(
        TARGETS evse_security
        EXPORT evse_security-targets
        LIBRARY
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    install(
        DIRECTORY 3rd_party/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME everest-evse_security
        NAMESPACE everest
        EXPORT evse_security-targets
        ADDITIONAL_CONTENT
            "find_dependency(everest-log)"
    )
endif()

if(LIBEVSE_SECURITY_BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
    set(CMAKE_BUILD_TYPE Debug)
endif()

