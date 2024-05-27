cmake_minimum_required(VERSION 3.14)

project(everest-evse_security VERSION 0.6
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

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(LIBEVSE_SECURITY_BUILD_TESTING ON)
endif()

if(USING_TPM2)
    # OpenSSL property string when using the default provider
    set(PROPQUERY_DEFAULT "provider!=tpm2")
    # OpenSSL property string when using the tpm2 provider
    set(PROPQUERY_TPM2 "?provider=tpm2,tpm2.digest!=yes,tpm2.cipher!=yes")
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
find_package(OpenSSL 3 REQUIRED)

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

