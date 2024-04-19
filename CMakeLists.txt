cmake_minimum_required(VERSION 3.14)

project(iso15118
    VERSION 0.1
    DESCRIPTION "iso15118 library suite"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(OPT_AUTODOWNLOAD_ISO20_SCHEMAS "\
Automatically download ISO15118-20 schemas.  Note: by setting this option to \
true and hence downloading the schema files, YOU accept the ISO Customer \
Licence Agreement (“Licence Agreement”), clauses 1. ISO’s Copyright, \
7. Termination, 8. Limitations, and 9. Governing Law." OFF)

option(ISO15118_LINK_CUSTOM_MBEDTLS "\
Link against a pinned mbedtls library\
" ON)


add_subdirectory(3rd_party)
add_subdirectory(input)
add_subdirectory(src)

if (ISO15118_BUILD_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

if (ISO15118_INSTALL)
    install(
        TARGETS
            cb_v2gtp
            cb_exi_codec
            cb_iso20
            iso15118
        EXPORT iso15118-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    evc_setup_package(
        NAME iso15118
        EXPORT iso15118-targets
        NAMESPACE iso15118
    )
endif()
