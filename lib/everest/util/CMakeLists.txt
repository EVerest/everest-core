add_library(everest_util INTERFACE)
add_library(everest::util ALIAS everest_util)
ev_register_library_target(everest_helpers)

target_include_directories(everest_util
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
