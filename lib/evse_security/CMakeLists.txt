
add_library(evse_security)
add_library(everest::evse_security ALIAS evse_security)

target_sources(evse_security
    PRIVATE
        evse_security.cpp
        evse_types.cpp

        certificate/x509_bundle.cpp
        certificate/x509_hierarchy.cpp
        certificate/x509_wrapper.cpp

        utils/evse_filesystem.cpp

        crypto/interface/crypto_supplier.cpp
        crypto/openssl/openssl_supplier.cpp
)

target_include_directories(evse_security
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

#############
# Logging configuration
#############
if (EVEREST_CUSTOM_LOGGING_INCLUDE_PATH)
    if (NOT EXISTS "${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
        message(FATAL_ERROR "everest/logging.hpp not found in directory ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}")
    else()
        target_include_directories(evse_security
            PUBLIC
            include
            ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}
        )
    endif()
    message(STATUS "Using the following logging header: ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
endif()

if (NOT EVEREST_CUSTOM_LOGGING_INCLUDE_PATH)
    target_link_libraries(evse_security
        PUBLIC
        everest::log
    )
    message(STATUS "Using the default logging header")
endif()

#############
# End logging configuration
#############

target_link_libraries(evse_security
    PUBLIC
        everest::timer
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

if(LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    target_link_libraries(evse_security
        PRIVATE
            Boost::filesystem
    )
endif()

if(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
    add_compile_definitions(LIBEVSE_CRYPTO_SUPPLIER_OPENSSL)
endif()

target_compile_features(evse_security PUBLIC cxx_std_17)
