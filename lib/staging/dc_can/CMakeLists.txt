add_library(dc_can)
add_library(dc_can::dc_can ALIAS dc_can)
target_include_directories(dc_can
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(dc_can
    PRIVATE
        src/dc_can.cpp
)

add_subdirectory(tests)
