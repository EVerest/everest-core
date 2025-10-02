
# Checks if the files provided by the INCLUDES argument
# are included in the sources of the TARGET
function(_target_includes_check)
    set(options)
    set(oneValueArgs
        TARGET
    )
    set(multiValueArgs
        INCLUDES
    )
    cmake_parse_arguments(arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    if (NOT arg_TARGET)
        message(FATAL_ERROR "TARGET not set")
    endif()
    if (NOT TARGET ${arg_TARGET})
        message(FATAL_ERROR "TARGET ${arg_TARGET} not found")
    endif()
    if (NOT arg_INCLUDES)
        message(FATAL_ERROR "INCLUDES not set")
    endif()

    get_target_property(TARGET_SOURCES ${arg_TARGET} SOURCES)
    foreach(include_file ${arg_INCLUDES})
        if (NOT include_file IN_LIST TARGET_SOURCES)
            message(FATAL_ERROR "include file ${include_file} not found in ${arg_TARGET} sources")
        endif()
    endforeach()
endfunction()

# Do some sanity checks to ensure that the generated files 
# are included in the libraries
macro(sanity_checks)
    _target_includes_check(
        TARGET proto_common_types
        INCLUDES
            ${COMMON_TYPES_HDRS}
            ${COMMON_TYPES_SRCS}
    )
    _target_includes_check(
        TARGET proto_control_service
        INCLUDES
            ${CONTROL_SERVICE_HDRS}
            ${CONTROL_SERVICE_SRCS}
    )
    _target_includes_check(
        TARGET proto_cs_lpc_service
        INCLUDES
            ${CS_LPC_SERVICE_HDRS}
            ${CS_LPC_SERVICE_SRCS}
    )
    message(STATUS "Sanity checks passed ✅")
endmacro()
