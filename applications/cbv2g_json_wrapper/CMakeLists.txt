cmake_minimum_required(VERSION 3.14)
project(cbv2g_json_wrapper VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build as shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# cJSON library (embedded)
add_library(cjson STATIC
    third_party/cJSON/cJSON.c
)
target_include_directories(cjson PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
)

# Wrapper sources. Per-protocol converters are added in subsequent PRs.
set(WRAPPER_SOURCES
    src/cbv2g_json_wrapper.c
    src/json_utils.c
    src/apphand_converter.c
    src/din_converter.c
)

add_library(cbv2g_json_wrapper SHARED ${WRAPPER_SOURCES})

target_include_directories(cbv2g_json_wrapper
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
)

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

# Optional: Build test executables
option(BUILD_CBV2G_JSON_WRAPPER_TESTS "Build cbv2g_json_wrapper test executables" OFF)
if(BUILD_CBV2G_JSON_WRAPPER_TESTS)
    enable_testing()

    add_executable(cbv2g_test_apphand
        tests/test_apphand.c
    )
    target_link_libraries(cbv2g_test_apphand
        cbv2g_json_wrapper
        cjson
    )
    target_include_directories(cbv2g_test_apphand PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
    )
    add_test(NAME cbv2g_test_apphand COMMAND cbv2g_test_apphand)

    add_executable(cbv2g_test_din
        tests/test_din.c
    )
    target_link_libraries(cbv2g_test_din
        cbv2g_json_wrapper
        cjson
    )
    target_include_directories(cbv2g_test_din PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cJSON
    )
    add_test(NAME cbv2g_test_din COMMAND cbv2g_test_din)
endif()
