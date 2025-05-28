# EVerest helper functions

add_library(everest_staging_helpers STATIC)
add_library(everest::staging::helpers ALIAS everest_staging_helpers)
ev_register_library_target(everest_staging_helpers)

target_sources(everest_staging_helpers
    PRIVATE
        lib/helpers.cpp
)

if (EVEREST_CORE_BUILD_TESTING)
    target_compile_definitions(everest_staging_helpers PUBLIC EVEREST_COVERAGE_ENABLED)

    target_sources(everest_staging_helpers
        PRIVATE
            lib/coverage.cpp
    )
endif()

target_include_directories(everest_staging_helpers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(everest_staging_helpers
    PRIVATE
        Boost::headers
        fmt::fmt
        nlohmann_json::nlohmann_json
        everest::tls
)

add_dependencies(everest_staging_helpers generate_cpp_files)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
