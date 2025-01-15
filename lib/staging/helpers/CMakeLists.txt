# EVerest helper functions

add_library(everest_staging_helpers STATIC)
add_library(everest::staging::helpers ALIAS everest_staging_helpers)

target_sources(everest_staging_helpers
    PRIVATE
        lib/helpers.cpp
)

target_include_directories(everest_staging_helpers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(everest_staging_helpers
    PRIVATE
        fmt::fmt
        nlohmann_json::nlohmann_json
)

add_dependencies(everest_staging_helpers generate_cpp_files)

ev_register_library_target(everest_staging_helpers)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
