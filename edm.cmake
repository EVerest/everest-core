function (evc_setup_edm)
    find_program(EVEREST_DEPENDENCY_MANAGER "edm")

    if(NOT EVEREST_DEPENDENCY_MANAGER)
        message(FATAL_ERROR "Could not find EVerest dependency manager. Please make it available in your PATH.")
    endif()

    # add the dependencies.yaml to the list of files which trigger
    # a cmake rerun, when changed
    set_property(
        DIRECTORY
        APPEND
        PROPERTY CMAKE_CONFIGURE_DEPENDS dependencies.yaml
    )

    execute_process(
        COMMAND "${EVEREST_DEPENDENCY_MANAGER}"
            --cmake
            --working_dir "${PROJECT_SOURCE_DIR}"
            --out "${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake"
    RESULT_VARIABLE 
        EVEREST_DEPENDENCY_MANAGER_RETURN_CODE
    )

    if(EVEREST_DEPENDENCY_MANAGER_RETURN_CODE AND NOT EVEREST_DEPENDENCY_MANAGER_RETURN_CODE EQUAL 0)
        message(FATAL_ERROR "EVerest dependency manager did not run successfully.")
    endif()

    evc_include("CPM")

    include("${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake")
endfunction()




