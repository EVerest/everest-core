add_library(everest_staging_util INTERFACE)
add_library(everest::staging::util ALIAS everest_staging_util)

target_include_directories(everest_staging_util
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(EVEREST_CORE_BUILD_TESTING)
    append_coverage_compiler_flags_to_target(everest_staging_util)
endif()
