add_library(tls STATIC)
add_library(everest::tls ALIAS tls)

find_package(OpenSSL 3)

target_sources(tls
    PRIVATE
        openssl_conv.cpp
        openssl_util.cpp
        tls.cpp
)

target_include_directories(tls
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../util>
)

target_link_libraries(tls
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        everest::evse_security
        everest::framework
)

if(EVEREST_CORE_BUILD_TESTING)
    add_subdirectory(tests)
endif()
