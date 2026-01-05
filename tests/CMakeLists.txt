# enabling coverage related things

evc_include(CodeCoverage)

# add compiler flags to all targets which should be coveraged

foreach(type MODULES LIBRARIES TESTS)
    ev_get_targets(targets ${type})

    foreach(target ${targets})
        append_coverage_compiler_flags_to_target(${target})

        if (type STREQUAL "MODULES")
            target_link_libraries(${target} PRIVATE everest::helpers)
        endif()
    endforeach()
endforeach()

get_target_property(GENERATED_OUTPUT_DIR generate_cpp_files EVEREST_GENERATED_OUTPUT_DIR)
setup_target_for_coverage_gcovr_html(
    NAME ${PROJECT_NAME}_create_coverage
    EXCLUDE "${GENERATED_OUTPUT_DIR}/*" ".*tests/"
)

# automatically install everest-testing if venv is active
ev_is_python_venv_active(
    RESULT_VAR IS_PYTHON_VENV_ACTIVE
)
if(IS_PYTHON_VENV_ACTIVE)
    get_target_property(SOURCE_DIRECTORY ev_pip_package_everest-testing SOURCE_DIRECTORY)
    message(STATUS "Installing everest-testing from: ${SOURCE_DIRECTORY}")
    ev_pip_install_local(
        PACKAGE_NAME "everest-testing"
        PACKAGE_SOURCE_DIRECTORY "${SOURCE_DIRECTORY}"
    )
endif()

add_subdirectory(everest-core_tests)
add_subdirectory(async_api_tests)
