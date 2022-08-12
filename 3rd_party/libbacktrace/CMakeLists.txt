include(ExternalProject)

# set flags correct configure flags if cross compiling
if (CMAKE_CROSSCOMPILING)
    if (ENV{CONFIGURE_FLAGS})
        separate_arguments(CONFIGURE_FLAGS UNIX_COMMAND $ENV{CONFIGURE_FLAGS})
    else ()
        string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}" CONFIGURE_BUILD_SYSTEM)
        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}" CONFIGURE_HOST_SYSTEM)
        list(APPEND CONFIGURE_FLAGS "--host=${CONFIGURE_HOST_SYSTEM}" "--build=${CONFIGURE_BUILD_SYSTEM}")
    endif()

    if (NOT ENV{CC})
        set (CONFIGURE_ENV "CC=${CMAKE_C_COMPILER}")
    endif ()
endif ()

ExternalProject_Add(
    libbacktrace
# download
    GIT_REPOSITORY https://github.com/ianlancetaylor/libbacktrace
    GIT_TAG 4d2dd0b172f2c9192f83ba93425f868f2a13c553
    GIT_SHALLOW false
# update
    UPDATE_COMMAND ""
# configure
    CONFIGURE_COMMAND
        ${CMAKE_COMMAND} -E env ${CONFIGURE_ENV}
        <SOURCE_DIR>/configure
            ${CONFIGURE_FLAGS}
            --prefix=<INSTALL_DIR>
            --includedir=<INSTALL_DIR>/include/backtrace
            --libdir=<INSTALL_DIR>/lib
            --enable-static=yes
            --disable-shared
# build options
    BUILD_ALWAYS false
# specify byproducts
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libbacktrace.a
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
