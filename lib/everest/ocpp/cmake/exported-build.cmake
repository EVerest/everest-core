find_package(everest-cmake 0.6
    COMPONENTS bundling
    PATHS ../everest-cmake
    NO_DEFAULT_PATH
)
find_package(everest-cmake 0.6
    COMPONENTS bundling
)

if (NOT everest-cmake_FOUND)
    message(STATUS "Retrieving everest-cmake using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        everest-cmake
        GIT_REPOSITORY https://github.com/EVerest/everest-cmake.git
        GIT_TAG        v0.6.0
    )
    FetchContent_MakeAvailable(everest-cmake)
    set(everest-cmake_DIR "${everest-cmake_SOURCE_DIR}")
    set(everest-cmake_FIND_COMPONENTS "bundling")
    include("${everest-cmake_SOURCE_DIR}/everest-cmake-config.cmake")
endif()

# download everest-core (source of libcbv2g)
include(ExternalProject)
ExternalProject_Add(
    everest-core-src
    DOWNLOAD_DIR "everest-core/src"
    GIT_REPOSITORY "https://github.com/EVerest/everest-core.git"
    GIT_TAG "8b8cfb2e3349408156c4ef84f2ecebafac0f6784" # FIXME: on branch
    TIMEOUT 30 # TODO: choose appropriate value
    LOG_DOWNLOAD ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# build everest-core/lib/everest/evse_security
ExternalProject_Add(
    evse_security-src
    DOWNLOAD_COMMAND ""
    SOURCE_DIR "everest-core-src-prefix/src/everest-core-src/lib/everest/evse_security"
    PREFIX "everest-core"
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    DEPENDS everest-core-src
)
ExternalProject_Get_Property(evse_security-src SOURCE_DIR)
ExternalProject_Get_Property(evse_security-src BINARY_DIR)
set(EVSE_SECURITY_INCLUDE_DIR "${SOURCE_DIR}/include")
set(EVSE_SECURITY_LIB_DIR "${BINARY_DIR}/lib/evse_security")

# workaround for https://gitlab.kitware.com/cmake/cmake/-/issues/15052
# create EVSE_SECURITY_INCLUDE_DIR since it will not exist in configure step
file(MAKE_DIRECTORY ${EVSE_SECURITY_INCLUDE_DIR})

add_library(evse_security STATIC IMPORTED)
add_library(everest::evse_security ALIAS evse_security)
set_property(TARGET evse_security PROPERTY IMPORTED_LOCATION ${EVSE_SECURITY_LIB_DIR}/libevse_security.a)
set_property(TARGET evse_security PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EVSE_SECURITY_INCLUDE_DIR})
add_dependencies(evse_security evse_security-src)

