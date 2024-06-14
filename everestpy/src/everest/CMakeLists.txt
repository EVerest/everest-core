if (DISABLE_EDM)
    find_package(pybind11 REQUIRED)
    find_package(pybind11_json REQUIRED)
endif()

pybind11_add_module(everestpy misc.cpp module.cpp everestpy.cpp)

target_compile_options(everestpy PRIVATE ${COMPILER_WARNING_OPTIONS})

target_link_libraries(everestpy
    PRIVATE
    everest::framework
    everest::log
    pybind11_json
    fmt::fmt
)

set(EVERESTPY_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/everest/everestpy/everest/framework)
set(EVERESTPY_DIST_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${EVERESTPY_LIB_INSTALL_DIR}/$<TARGET_FILE_NAME:everestpy>)

add_custom_target(everestpy_ln_dist
    COMMAND
        test -f $$DESTDIR${EVERESTPY_DIST_INSTALL_PATH} || (echo "everestpy library not found, did you run the install target?" && false)
    COMMAND
        ${CMAKE_COMMAND} -E create_symlink $$DESTDIR${EVERESTPY_DIST_INSTALL_PATH} $<TARGET_FILE_NAME:everestpy>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    COMMENT
        "Creating symlink to distributed everestpy"
)

add_custom_target(everestpy_ln_local
    COMMAND
        ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:everestpy> $<TARGET_FILE_NAME:everestpy>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    DEPENDS
        everestpy
    COMMENT
        "Creating symlink to build everestpy"
)

install(
    TARGETS everestpy
    LIBRARY
    DESTINATION ${EVERESTPY_LIB_INSTALL_DIR}
)

install(
    FILES
        framework/__init__.py
    DESTINATION ${EVERESTPY_LIB_INSTALL_DIR}
)
