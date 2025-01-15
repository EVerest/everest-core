include_guard(GLOBAL)

add_custom_target(everest_targets)

set_target_properties(everest_targets
    PROPERTIES
        LIBRARIES ""
        MODULES ""
)

function(_ev_register_target TYPE NAME)
    if (NOT TARGET ${NAME})
        message(FATAL_ERROR "The supplied name ${NAME} of type ${TYPE} is not a valid target")
    endif()

    set_property(
        TARGET everest_targets
        APPEND
        PROPERTY ${TYPE} ${NAME}
    )
endfunction()

function(ev_register_library_target NAME)
    _ev_register_target(LIBRARIES ${NAME})
endfunction()

function(ev_register_module_target NAME)
    _ev_register_target(MODULES ${NAME})
endfunction()

function(ev_get_targets NAME TYPE)
    get_target_property(tmp everest_targets ${TYPE})
    if (NOT tmp STREQUAL "" AND NOT tmp)
        message(FATAL_ERROR "There is no target of type ${TYPE} defined")
    endif()

    set(${NAME} ${tmp} PARENT_SCOPE)
endfunction()