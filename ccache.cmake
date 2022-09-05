include_guard(GLOBAL)

find_program(CCACHE_EXECUTABLE ccache)

if(CCACHE_EXECUTABLE)
    set_property(
        GLOBAL
        PROPERTY
            RULE_LAUNCH_COMPILE "${CCACHE_EXECUTABLE}"
    )
else()
    message(WARNING "Could not find ccache executable - caching disabled")
endif()
