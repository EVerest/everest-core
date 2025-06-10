add_subdirectory(can_dpm1000)
add_subdirectory(external_energy_limits)
add_subdirectory(helpers)
add_subdirectory(run_application)
add_subdirectory(util)

if(EVEREST_DEPENDENCY_ENABLED_LIBEVSE_SECURITY)
    add_subdirectory(evse_security)
    add_subdirectory(tls)
endif()

if(EVEREST_DEPENDENCY_ENABLED_LIBSLAC AND EVEREST_DEPENDENCY_ENABLED_LIBFSM)
    add_subdirectory(slac)
endif()

if(EVEREST_DEPENDENCY_ENABLED_EVEREST_GPIO)
    add_subdirectory(gpio)
endif()

if(EVEREST_DEPENDENCY_ENABLED_LIBOCPP)
    add_subdirectory(ocpp)
endif()

add_subdirectory(run_application)
