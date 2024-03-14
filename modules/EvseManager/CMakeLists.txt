#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
target_sources(${MODULE_NAME}
    PRIVATE
        Charger.cpp
        SessionLog.cpp
        v2gMessage.cpp
        CarManufacturer.cpp
        IECStateMachine.cpp
        ErrorHandling.cpp
        backtrace.cpp
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Pal::Sigslot
        pugixml::pugixml
)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
target_link_libraries(${MODULE_NAME}
    PRIVATE
        stdc++fs
)
endif()

target_compile_options(${MODULE_NAME}
PRIVATE
    "-rdynamic"
)

target_link_options(${MODULE_NAME}
PRIVATE
    "-rdynamic"
)

# needed for std::filesystem
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_17)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "evse/evse_managerImpl.cpp"
        "energy_grid/energyImpl.cpp"
        "token_provider/auth_token_providerImpl.cpp"
        "random_delay/uk_random_delayImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
