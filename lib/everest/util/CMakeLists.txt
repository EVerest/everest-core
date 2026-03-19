add_library(everest_util INTERFACE)
add_library(everest::util ALIAS everest_util)
ev_register_library_target(everest_helpers)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set_target_properties(everest_util PROPERTIES
  VERSION 0.0.1
)


target_include_directories(everest_util
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(everest_util
    PROPERTIES
        BUILD_INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        EXPORT_NAME util
)

install(TARGETS everest_util
    EXPORT everest-core-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/everest
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

if(DISABLE_EDM)
    # Do not use evc_setup_package() here: it unconditionally installs an
    # export file for the package, which would export everest_util a second
    # time alongside everest-core-targets and break non-EDM consumers.
    set(EVEREST_UTIL_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/everest-util")

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/everest-util-config.cmake
        INSTALL_DESTINATION ${EVEREST_UTIL_CMAKE_INSTALL_DIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/everest-util-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/everest-util-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/everest-util-config-version.cmake
        DESTINATION ${EVEREST_UTIL_CMAKE_INSTALL_DIR}
    )
endif()

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
