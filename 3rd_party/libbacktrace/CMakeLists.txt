include(ExternalProject)

set(LIBBACKTRACE_HOST "")
if(LIBLOG_CROSS_COMPILE)
    set(LIBBACKTRACE_HOST "--host=arm-linux")
endif()

ExternalProject_Add(
    libbacktrace
# prefix
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
# download
    GIT_REPOSITORY https://github.com/ianlancetaylor/libbacktrace
    GIT_TAG 4d2dd0b172f2c9192f83ba93425f868f2a13c553
    GIT_SHALLOW false
# update
    UPDATE_COMMAND ""
# configure
    CONFIGURE_COMMAND env CC=${CMAKE_C_COMPILER} ../libbacktrace/configure --prefix=${CMAKE_CURRENT_BINARY_DIR} --includedir=${CMAKE_CURRENT_BINARY_DIR}/include/backtrace --libdir=${CMAKE_CURRENT_BINARY_DIR}/lib --enable-static=yes --disable-shared ${LIBBACKTRACE_HOST}
# make
    BUILD_COMMAND ${MAKE}
    BUILD_ALWAYS false
# specify byproducts
    BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/lib/libbacktrace.a
# log options
    LOG_DOWNLOAD true
    LOG_CONFIGURE true
    LOG_BUILD true
    LOG_INSTALL true
    LOG_OUTPUT_ON_FAILURE true
)

add_library(
    backtrace
    STATIC IMPORTED GLOBAL
)
add_dependencies(backtrace libbacktrace)

ExternalProject_Get_Property(libbacktrace INSTALL_DIR)

# hack to get INTERFACE_INCLUDE_DIRECTORIES working
# see https://gitlab.kitware.com/cmake/cmake/-/issues/15052
file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
set_target_properties(
    backtrace
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}/lib/libbacktrace.a
        INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include
)
