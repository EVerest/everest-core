# External Energy Limits

add_library(external_energy_limits STATIC)
add_library(everest::external_energy_limits ALIAS external_energy_limits)

target_sources(external_energy_limits
    PRIVATE
        external_energy_limits.cpp
)

target_include_directories(external_energy_limits
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

add_dependencies(external_energy_limits generate_cpp_files)

target_link_libraries(external_energy_limits
    PRIVATE
        everest::framework
)

if(EVEREST_CORE_BUILD_TESTING)
    append_coverage_compiler_flags_to_target(external_energy_limits)
endif()
