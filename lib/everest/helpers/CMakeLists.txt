# EVerest helper functions

add_library(everest_helpers STATIC)
add_library(everest::helpers ALIAS everest_helpers)
ev_register_library_target(everest_helpers)

target_sources(everest_helpers
    PRIVATE
        src/helpers.cpp
)

if (EVEREST_CORE_BUILD_TESTING)
    target_compile_definitions(everest_helpers PUBLIC EVEREST_COVERAGE_ENABLED)

    target_sources(everest_helpers
        PRIVATE
            src/coverage.cpp
    )
endif()

target_include_directories(everest_helpers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

target_link_libraries(everest_helpers
    PRIVATE
        Boost::headers
        fmt::fmt
        nlohmann_json::nlohmann_json
        everest::tls
)

set_target_properties(everest_helpers
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

add_dependencies(everest_helpers generate_cpp_files)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(DISABLE_EDM)
    install(
        TARGETS
            everest_helpers
        EXPORT everest-helpers-targets
        LIBRARY
    )

    evc_setup_package(
        NAME everest-helpers
        EXPORT everest-helpers-targets
        NAMESPACE everest
        ADDITIONAL_CONTENT
            "find_dependency(everest-tls)"
            "find_dependency(nlohmann_json)"
            "find_dependency(nlohmann_json_schema_validator)"
            "find_dependency(fmt)"
    )
endif()
