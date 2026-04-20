add_executable(manager)
target_sources(manager
    PRIVATE
        config_service_api/config_service_api.cpp
        config_service_api/wrapper.cpp
        system_unix.cpp
        manager.cpp
)
# generate version information header
evc_generate_version_information()
target_include_directories(manager
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated/include
)

target_link_libraries(manager
    PRIVATE
        everest::framework
        Boost::program_options
        PkgConfig::libcap
        everest::everest_api_types
)

if (EVEREST_ENABLE_ADMIN_PANEL_BACKEND)
    target_link_libraries(manager
        PRIVATE
            controller-ipc
    )

    target_compile_definitions(manager PRIVATE ENABLE_ADMIN_PANEL)

    add_subdirectory(controller)
endif ()

target_compile_options(manager PRIVATE ${COMPILER_WARNING_OPTIONS})

target_compile_features(manager PRIVATE cxx_std_17)

install(
    TARGETS manager
    RUNTIME
)

if (EVEREST_ENABLE_CONFIG_TOOL)

add_executable(everest-config-tool)
target_sources(everest-config-tool
    PRIVATE
        everest-config.cpp
)
target_link_libraries(everest-config-tool
    PRIVATE
        everest::framework
        Boost::program_options
)
target_compile_options(everest-config-tool PRIVATE ${COMPILER_WARNING_OPTIONS})
target_compile_features(everest-config-tool PRIVATE cxx_std_17)
install(
    TARGETS everest-config-tool
    RUNTIME
)

endif ()

# FIXME (aw): the www folder currently always needs to exist, so that the manager does not complain
install(
    DIRECTORY # intentionally left blank
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/everest/www"
)
