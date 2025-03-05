option(EVEREST_CORE_ENABLE_COVERAGE "Enable coverage for everest-core" OFF)

if(EVEREST_CORE_ENABLE_COVERAGE)
    # enabling coverage related things

    evc_include(CodeCoverage)

    # add compiler flags to all targets which should be coveraged

    foreach(type MODULES LIBRARIES TESTS)
        ev_get_targets(targets ${type})

        foreach(target ${targets})
            append_coverage_compiler_flags_to_target(${target})

            if (type STREQUAL "MODULES")
                target_link_libraries(${target} PRIVATE everest::staging::helpers)
            endif()
        endforeach()
    endforeach()

    get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
    setup_target_for_coverage_gcovr_html(
        NAME ${PROJECT_NAME}_create_coverage
        EXCLUDE "${GENERATED_OUTPUT_DIR}/*" ".*tests/"
    )
endif()

add_subdirectory(eebus_tests)
add_subdirectory(everest-core_tests)
