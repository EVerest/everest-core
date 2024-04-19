include(ExternalProject)

if (CMAKE_CROSSCOMPILING)
    if (NOT CMAKE_TOOLCHAIN_FILE)
        message(FATAL "Cross-compiling the custom mbedtls library without supplying CMAKE_TOOLCHAIN_FILE is not supported")
    endif ()
    set (CUSTOM_MBEDTLS_CROSSCOMPILE_DEF "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif ()

set(ENABLE_TLS13_HEADER_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config_enable_tls13.h)

set(CUSTOM_MBEDTLS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_mbedtls-build)

set(STATIC_MBEDTLS_DIR "${CUSTOM_MBEDTLS_BINARY_DIR}/library/libmbedtls.a")
set(STATIC_MBEDX509_DIR "${CUSTOM_MBEDTLS_BINARY_DIR}/library/libmbedx509.a")
set(STATIC_MBEDCRYPT_DIR "${CUSTOM_MBEDTLS_BINARY_DIR}/library/libmbedcrypto.a")

ExternalProject_Add(custom_mbedtls
    BINARY_DIR
        ${CUSTOM_MBEDTLS_BINARY_DIR}
    GIT_REPOSITORY
        https://github.com/Mbed-TLS/mbedtls.git
    GIT_TAG
        mbedtls-3.5.0
    CMAKE_ARGS
        ${CUSTOM_MBEDTLS_CROSSCOMPILE_DEF}
        -DENABLE_PROGRAMS=OFF
        -DENABLE_TESTING=OFF
        -DDISABLE_PACKAGE_CONFIG_AND_INSTALL=ON
        -DMBEDTLS_USER_CONFIG_FILE=${ENABLE_TLS13_HEADER_CONFIG_FILE}
        # library should be build static by default
    INSTALL_COMMAND
        ""
    BUILD_BYPRODUCTS
        ${STATIC_MBEDTLS_DIR}
        ${STATIC_MBEDX509_DIR}
        ${STATIC_MBEDCRYPT_DIR}     
)

ExternalProject_Get_Property(custom_mbedtls SOURCE_DIR)

# well known hack for ExternalProject with include directories ...
set(CUSTOM_MBEDTLS_INCLUDE_DIR ${SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CUSTOM_MBEDTLS_INCLUDE_DIR})

add_library(mbedtls_custom STATIC IMPORTED GLOBAL)
target_compile_definitions(mbedtls_custom
    INTERFACE
        MBEDTLS_USER_CONFIG_FILE="${ENABLE_TLS13_HEADER_CONFIG_FILE}"
)
set_target_properties(mbedtls_custom
    PROPERTIES
        IMPORTED_LOCATION ${STATIC_MBEDTLS_DIR}
        INTERFACE_INCLUDE_DIRECTORIES ${CUSTOM_MBEDTLS_INCLUDE_DIR}
)
add_dependencies(mbedtls_custom custom_mbedtls)

add_library(mbedx509_custom STATIC IMPORTED GLOBAL)
target_compile_definitions(mbedx509_custom
    INTERFACE
        MBEDTLS_USER_CONFIG_FILE="${ENABLE_TLS13_HEADER_CONFIG_FILE}"
)
set_target_properties(mbedx509_custom
    PROPERTIES
        IMPORTED_LOCATION ${STATIC_MBEDX509_DIR}
        INTERFACE_INCLUDE_DIRECTORIES ${CUSTOM_MBEDTLS_INCLUDE_DIR}
)
add_dependencies(mbedx509_custom custom_mbedtls)

add_library(mbedcrypto_custom STATIC IMPORTED GLOBAL)
target_compile_definitions(mbedcrypto_custom
    INTERFACE
        MBEDTLS_USER_CONFIG_FILE="${ENABLE_TLS13_HEADER_CONFIG_FILE}"
)
set_target_properties(mbedcrypto_custom
    PROPERTIES
        IMPORTED_LOCATION ${STATIC_MBEDCRYPT_DIR}
        INTERFACE_INCLUDE_DIRECTORIES ${CUSTOM_MBEDTLS_INCLUDE_DIR}
)
add_dependencies(mbedcrypto_custom custom_mbedtls)
