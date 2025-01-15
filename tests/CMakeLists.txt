# enabling coverage related things

evc_include(CodeCoverage)

# add compiler flags to all targets which should be coveraged

foreach(type MODULES LIBRARIES)
    ev_get_targets(targets ${type})

    foreach(target ${targets})
        append_coverage_compiler_flags_to_target(${target})
    endforeach()
endforeach()

get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_gcovr_coverage
    EXCLUDE "${GENERATED_OUTPUT_DIR}/*"
)

add_subdirectory(everest-core_tests)
