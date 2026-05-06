add_library(tls STATIC)
add_library(everest::tls ALIAS tls)
ev_register_library_target(tls)

find_package(OpenSSL 3)

target_sources(tls
    PRIVATE
        extensions/status_request.cpp
        extensions/trusted_ca_keys.cpp
        src/openssl_conv.cpp
        src/openssl_util.cpp
        src/tls.cpp
)

target_compile_definitions(tls PRIVATE
    -DLIBEVSE_CRYPTO_SUPPLIER_OPENSSL
)

target_include_directories(tls
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# FIXME (aw): check whether all of this needs to be publicly exposed
target_link_libraries(tls
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::evse_security
        everest::util
)

set_target_properties(tls
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(DISABLE_EDM)
    install(
        TARGETS
            tls
        EXPORT everest-tls-targets
        LIBRARY
    )

    evc_setup_package(
        NAME everest-tls
        EXPORT everest-tls-targets
        NAMESPACE everest
        ADDITIONAL_CONTENT
            "find_dependency(everest-util)"
            "find_dependency(everest-evse_security)"
            "find_dependency(OpenSSL)"
    )
endif()
