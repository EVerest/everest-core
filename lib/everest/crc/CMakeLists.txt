add_library(everest_crc STATIC src/crc.cpp)
add_library(everest::crc ALIAS everest_crc)
ev_register_library_target(everest_crc)

target_include_directories(everest_crc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
