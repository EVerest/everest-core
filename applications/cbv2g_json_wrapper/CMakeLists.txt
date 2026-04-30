cmake_minimum_required(VERSION 3.14)
project(cbv2g_json_wrapper VERSION 1.0.0 LANGUAGES C CXX)

# When this directory is configured standalone (i.e. not as part of an
# everest-core build), pull in libcbv2g via add_subdirectory so the
# cbv2g::din target is available. See cmake/local-build.cmake for details.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NOT TARGET cbv2g::din)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/local-build.cmake)
endif()

# cJSON library (embedded). C99 is sufficient for both cJSON and the
# wrapper itself (no C11 features used) and matches libcbv2g's baseline,
# keeping the toolchain requirement low for downstream consumers.
add_library(cjson STATIC
    third_party/cJSON/cJSON.c
)
target_include_directories(cjson PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
)
target_compile_features(cjson PRIVATE c_std_99)
set_property(TARGET cjson PROPERTY POSITION_INDEPENDENT_CODE ON)

# Wrapper. Per-protocol converters are added in subsequent PRs.
add_library(cbv2g_json_wrapper SHARED)
target_sources(cbv2g_json_wrapper
    PRIVATE
        src/cbv2g_json_wrapper.c
        src/json_utils.c
        src/apphand_converter.c
)

target_include_directories(cbv2g_json_wrapper
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
)

target_compile_features(cbv2g_json_wrapper PRIVATE c_std_99)
set_property(TARGET cbv2g_json_wrapper PROPERTY POSITION_INDEPENDENT_CODE ON)

# cbv2g::din carries the app_handshake/ sources; libcbv2g exposes no
# standalone apphand target. Subsequent PRs add cbv2g::iso2 and cbv2g::iso20.
target_link_libraries(cbv2g_json_wrapper
    PRIVATE
        cjson
        cbv2g::din
)

# Hide all symbols by default; the public API is tagged CBV2G_API in the
# header. This keeps the statically-linked cbv2g/cJSON internals from
# colliding with EvseV2G (and any other module that statically links cbv2g)
# at runtime.
set_target_properties(cbv2g_json_wrapper PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Install rules
install(TARGETS cbv2g_json_wrapper
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES include/cbv2g_json_wrapper.h
    DESTINATION include
)

# Optional: Build test executables. GTest is only required when this
# option is on, so production builds do not pull it in.
option(BUILD_CBV2G_JSON_WRAPPER_TESTS "Build cbv2g_json_wrapper test executables" OFF)
if(BUILD_CBV2G_JSON_WRAPPER_TESTS)
    enable_testing()

    if(NOT TARGET GTest::gtest_main)
        find_package(GTest REQUIRED)
    endif()

    add_executable(cbv2g_test_apphand)
    target_sources(cbv2g_test_apphand
        PRIVATE
            tests/test_apphand.cpp
    )
    target_include_directories(cbv2g_test_apphand PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
    )
    target_compile_features(cbv2g_test_apphand PRIVATE cxx_std_17)
    target_link_libraries(cbv2g_test_apphand
        PRIVATE
            cbv2g_json_wrapper
            cjson
            GTest::gtest_main
    )
    add_test(NAME cbv2g_test_apphand COMMAND cbv2g_test_apphand)
endif()
