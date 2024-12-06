include_guard(GLOBAL)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "..." FORCE)
endif()

# set RPATH to the planned install path of lib folder
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# bundling
# FIXME (aw): in case of dist folder, we could only clean up the dist folder

add_custom_target(bundle
    COMMAND "${CMAKE_COMMAND}" -E remove_directory ${CMAKE_BINARY_DIR}/bundle
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
    COMMAND "${CMAKE_COMMAND}"
        -E env DESTDIR=bundle
        "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}"
        $<$<NOT:$<BOOL:${EVEREST_CMAKE_DISABLE_STRIPPING_BUNDLE}>>:--strip>
    COMMAND rm -rf
        bundle/${CMAKE_INSTALL_PREFIX}/include
        bundle/${CMAKE_INSTALL_PREFIX}/lib/cmake
        bundle/${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
        bundle/${CMAKE_INSTALL_PREFIX}/lib/*.a
    COMMAND "${CMAKE_COMMAND}" -E echo ""
    COMMAND "${CMAKE_COMMAND}" -E echo "Created bundle folder at ${CMAKE_BINARY_DIR}/bundle"
    COMMAND "${CMAKE_COMMAND}" -E echo ""
    WORKING_DIRECTORY
        ${CMAKE_BINARY_DIR}
)

set(BUNDLE_TAR_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tgz")
add_custom_target(bundle-tar
    DEPENDS bundle
    COMMAND tar
        -czf "${BUNDLE_TAR_FILE}"
        -C "${CMAKE_BINARY_DIR}/bundle/${CMAKE_INSTALL_PREFIX}"
        ./
    WORKING_DIRECTORY
        ${CMAKE_BINARY_DIR}
)
