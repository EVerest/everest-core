function(_everest_exclude_modules_loop EVEREST_INTERNAL_DEPENDENT_MODULES_LIST OUTPUT_VARIABLE)
    # iterate over the provided list of dependent modules and check if all of these modules are included in EVEREST_EXCLUDE_MODULES
    set(EVEREST_INTERNAL_DEPENDENCY_EXCLUDE_COUNT 0)
    foreach(EVEREST_INTERNAL_DEPENDENT_MODULE IN LISTS EVEREST_INTERNAL_DEPENDENT_MODULES_LIST)
        if("${EVEREST_INTERNAL_DEPENDENT_MODULE}" IN_LIST EVEREST_EXCLUDE_MODULES)
            # a dependent module is excluded, exclude the dependency
            math(EXPR EVEREST_INTERNAL_DEPENDENCY_EXCLUDE_COUNT "${EVEREST_INTERNAL_DEPENDENCY_EXCLUDE_COUNT}+1")
        endif()
    endforeach()
    list(LENGTH EVEREST_EXCLUDE_MODULES EVEREST_INTERNAL_EXCLUDE_MODULES_LEN)
    if(EVEREST_INTERNAL_DEPENDENCY_EXCLUDE_COUNT GREATER 0 AND EVEREST_INTERNAL_DEPENDENCY_EXCLUDE_COUNT EQUAL EVEREST_INTERNAL_EXCLUDE_MODULES_LEN)
        # all modules that need this dependency are excluded
        set("${OUTPUT_VARIABLE}" OFF PARENT_SCOPE)
    endif()
endfunction()

function(_everest_include_modules_loop EVEREST_INTERNAL_DEPENDENT_MODULES_LIST OUTPUT_VARIABLE)
    # iterate over the provided list of dependent modules and check if one of these modules is included in EVEREST_INCLUDE_MODULES
    foreach(EVEREST_INTERNAL_DEPENDENT_MODULE IN LISTS EVEREST_INTERNAL_DEPENDENT_MODULES_LIST)
        if("${EVEREST_INTERNAL_DEPENDENT_MODULE}" IN_LIST EVEREST_INCLUDE_MODULES)
            # a dependent module is being build, include the dependency
            set("${OUTPUT_VARIABLE}" ON PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

function(ev_define_dependency)
    #
    # handle passed arguments
    #
    set(options "")
    set(one_value_args
        DEPENDENCY_NAME
        OUTPUT_VARIABLE_SUFFIX
    )
    set(multi_value_args
        DEPENDENT_MODULES_LIST
    )
    cmake_parse_arguments(OPTNS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if (OPTNS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} got unknown argument(s): ${OPTNS_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT OPTNS_DEPENDENCY_NAME)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires DEPENDENCY_NAME parameter for the dependency name")
    endif()
    set(DEPENDENCY_NAME "${OPTNS_DEPENDENCY_NAME}")
    string(TOUPPER "${OPTNS_DEPENDENCY_NAME}" EVEREST_INTERNAL_DEPENDENCY_NAME_UPPER)

    if (NOT OPTNS_DEPENDENT_MODULES_LIST)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} requires DEPENDENT_MODULES_LIST parameter for the dependent modules list")
    endif()
    set(EVEREST_INTERNAL_DEPENDENT_MODULES_LIST "${OPTNS_DEPENDENT_MODULES_LIST}")
    list(LENGTH EVEREST_INTERNAL_DEPENDENT_MODULES_LIST EVEREST_INTERNAL_DEPENDENT_MODULES_LIST_LEN)

    if (NOT OPTNS_OUTPUT_VARIABLE_SUFFIX)
        # use the uppercase dependency name
        set(EVEREST_INTERNAL_DEPENDENCY_CONDITION "EVEREST_DEPENDENCY_ENABLED_${EVEREST_INTERNAL_DEPENDENCY_NAME_UPPER}")
    else()
        # set output variable suffix
        set(EVEREST_INTERNAL_DEPENDENCY_CONDITION "EVEREST_DEPENDENCY_ENABLED_${OPTNS_OUTPUT_VARIABLE_SUFFIX}")
    endif()

    #
    # handle the various dependency conditions
    #
    if(DEFINED "${EVEREST_INTERNAL_DEPENDENCY_CONDITION}")
        # always enable a externally set dependency
        message(STATUS "${EVEREST_INTERNAL_DEPENDENCY_CONDITION} externally set to: ${${EVEREST_INTERNAL_DEPENDENCY_CONDITION}}")
    else()
        # disable dependency by default
        set("${EVEREST_INTERNAL_DEPENDENCY_CONDITION}" OFF)

        # find out if dependency is excluded or uncluded
        if(NOT "${DEPENDENCY_NAME}" IN_LIST EVEREST_EXCLUDE_DEPENDENCIES)
            if(NOT EVEREST_INCLUDE_MODULES)
                # all modules are being build, at the moment we cannot know which dependencies are needed
                set("${EVEREST_INTERNAL_DEPENDENCY_CONDITION}" ON)
                # find out if all modules that need this dependency are excluded
                _everest_exclude_modules_loop("${EVEREST_INTERNAL_DEPENDENT_MODULES_LIST}" "${EVEREST_INTERNAL_DEPENDENCY_CONDITION}")
            else()
                # EVEREST_INCLUDE_MODULES takes precendece over EVEREST_EXCLUDE_MODULES in everest-generate as well, reflect this here
                _everest_include_modules_loop("${EVEREST_INTERNAL_DEPENDENT_MODULES_LIST}" "${EVEREST_INTERNAL_DEPENDENCY_CONDITION}")
            endif()
        else()
            # dependency disabled because it is listed in EVEREST_EXCLUDE_DEPENDENCIES
            set("${EVEREST_INTERNAL_DEPENDENCY_CONDITION}" OFF)
        endif()
    endif()

    # log the result
    if(NOT ${EVEREST_INTERNAL_DEPENDENCY_CONDITION})
        message(STATUS "Dependency ${DEPENDENCY_NAME} NOT enabled:")
    else()
        message(STATUS "Dependency ${DEPENDENCY_NAME} enabled:")
    endif()

    message(STATUS "  ${EVEREST_INTERNAL_DEPENDENCY_CONDITION}=${${EVEREST_INTERNAL_DEPENDENCY_CONDITION}}")

    # propagate the result to the parent scope:
    set("${EVEREST_INTERNAL_DEPENDENCY_CONDITION}" "${${EVEREST_INTERNAL_DEPENDENCY_CONDITION}}" PARENT_SCOPE)
endfunction()
