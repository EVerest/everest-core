add_library(everest_yaml STATIC)
add_library(everest::yaml ALIAS everest_yaml)
ev_register_library_target(everest_yaml)

target_sources(everest_yaml
    PRIVATE
        lib/yaml_loader.cpp
)

target_include_directories(everest_yaml
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/everest>
)

target_compile_features(everest_yaml PUBLIC cxx_std_17)

set_target_properties(everest_yaml PROPERTIES
    EXPORT_NAME yaml
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(everest_yaml
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        ryml::ryml
        fmt::fmt
        everest::log
)

if (DISABLE_EDM)
    include(GNUInstallDirs)

    install(
        TARGETS everest_yaml
        EXPORT everest-core-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/everest
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/everest
        FILES_MATCHING PATTERN "*.hpp"
    )
endif()
