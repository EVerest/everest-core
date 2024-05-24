cmake_minimum_required(VERSION 3.14)

project(cb_v2g
    VERSION 0.1
    DESCRIPTION "V2GTP EXI library"
    HOMEPAGE_URL "https://github.com/Everest/libcbv2g"
	LANGUAGES C CXX
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(CB_V2G_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

add_subdirectory(lib)

message("-- Lib code based on cbexigen generator version: b64f296")

# tests
if (CB_V2G_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# packaging
if (CB_V2G_INSTALL)
    install(
        TARGETS
            cb_exi_codec
            cb_din
            cb_iso2
            cb_iso20
            cb_v2gtp
        EXPORT cb_v2g-targets
        LIBRARY
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    evc_setup_package(
        NAME cb_v2g
        NAMESPACE cb_v2g
        EXPORT cb_v2g-targets
    )
endif()

