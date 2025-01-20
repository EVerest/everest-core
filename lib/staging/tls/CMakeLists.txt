add_library(tls STATIC)
add_library(everest::tls ALIAS tls)
ev_register_library_target(tls)

find_package(OpenSSL 3)

target_sources(tls
    PRIVATE
        extensions/status_request.cpp
        extensions/trusted_ca_keys.cpp
        openssl_conv.cpp
        openssl_util.cpp
        tls.cpp
)

target_compile_definitions(tls PRIVATE
    -DLIBEVSE_CRYPTO_SUPPLIER_OPENSSL
)

target_include_directories(tls
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# FIXME (aw): check whether all of this needs to be publicly exposed
target_link_libraries(tls
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::evse_security
        everest::staging::util
    PRIVATE
        everest::framework
)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
