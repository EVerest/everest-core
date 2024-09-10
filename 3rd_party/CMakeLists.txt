if ((NOT USE_OPENSSL) AND ISO15118_LINK_CUSTOM_MBEDTLS)
    add_subdirectory(mbedtls)
endif ()
