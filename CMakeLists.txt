cmake_minimum_required(VERSION 3.14)

project(everest-evse_security VERSION 0.4.0
        DESCRIPTION "Implementation of EVSE related security operations"
		LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(EVSE_SECURITY_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(EVSE_SECURITY_INSTALL OFF)
else()
    find_package(everest-log REQUIRED)
endif()

option(LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM "Usage of boost/filesystem.hpp instead of std::filesystem" OFF)

option(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL "Default OpenSSL cryptography supplier" ON)

# dependencies
find_package(OpenSSL REQUIRED)

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

if(BUILD_TESTING_EVSE_SECURITY)
    add_subdirectory(tests)
    set(CMAKE_BUILD_TYPE Debug)
endif()

